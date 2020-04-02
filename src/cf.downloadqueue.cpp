#include "cf.downloadqueue.h"

#include "cf.downloadhandler.h"
#include "managers/cf.groupmanager.h"
#include "managers/cf.usermanager.h"
#include "cf.requestsender.h"

CFDownloadQueue::CFDownloadQueue(QObject *parent)
    : QObject(parent),
      m_paused(false)
{
    connect(this, &CFDownloadQueue::progressChanged,
            this, &CFDownloadQueue::estimatedDurationChanged);
    connect(this, &CFDownloadQueue::progressChanged, [this]{
        if (m_restartTimer.isActive()) {
            m_restartTimer.start();
        }
    });

    m_restartTimer.setInterval(30000);
    m_restartTimer.setSingleShot(true);
    connect(&m_restartTimer, &QTimer::timeout, [this]{
        if (!m_downloads.isEmpty()) {
            // Replace current download in-place

            CFGroup *group = m_downloads.last()->get_group();

            m_downloads[m_currentIndex]->stop();
            m_downloads[m_currentIndex]->deleteLater();

            CFDownloadHandler *download = new CFDownloadHandler(group, this);
            connect(download, &CFDownloadHandler::error, this, &CFDownloadQueue::onError);
            connect(download, &CFDownloadHandler::chunkFinished, this, &CFDownloadQueue::onChunkFinished);
            connect(download, &CFDownloadHandler::finished, this, &CFDownloadQueue::onFinished);
            m_downloads.replace(m_currentIndex, download);
            m_downloads[m_currentIndex]->start();
            m_restartTimer.start();
        }
    });
}

double CFDownloadQueue::progress() const
{
    // Sub count and count of processed users (aka offset) are available
    // as data members in a group object so all we need to do is sum them up
    int count = 0;
    int processedCount = 0;

    for (CFDownloadHandler *download : m_downloads) {
        count += download->get_allRequestCount();
        processedCount += download->get_requestCount();
    }

    return count ? std::min(static_cast<double>(processedCount) / count, 1.) : 0;
}

int CFDownloadQueue::estimatedDuration() const
{
    // Sub count and count of processed users (aka offset) are available
    // as data members in a group object so so all we need to do is sum them up
    int count = 0;
    int processedCount = 0;

    for (CFDownloadHandler *download : m_downloads) {
        count += download->get_allRequestCount();
        processedCount += download->get_requestCount();
    }

    int remainingCount = std::max(count - processedCount, 0);
    double requestsPerSecond = 1000. / CFRequestSender::k_intervalBetweenRequest;

    return std::round(remainingCount / requestsPerSecond);
}

int CFDownloadQueue::count() const
{
    return m_downloads.count();
}

int CFDownloadQueue::currentIndex() const
{
    return m_currentIndex;
}

void CFDownloadQueue::replace(CFGroup *group)
{
    for (int i = 0; i < m_downloads.count(); i++) {
        if (m_downloads.at(i)->get_group()->get_uid() == group->get_uid()) {
            m_downloads.at(i)->update_group(group);
        }
    }
}

void CFDownloadQueue::add(int groupId)
{
    // Do nothing if group is present
    for (CFDownloadHandler *download : m_downloads) {
        if (download->get_group()->get_uid() == groupId) {
            return;
        }
    }

    CFDownloadHandler *download = new CFDownloadHandler(
                CFGroupManager::instance().get_groupModel()->getByUid(
                    QString::number(groupId)), this);

    // Save download errors
    connect(download, &CFDownloadHandler::error, this, &CFDownloadQueue::onError);

    connect(download, &CFDownloadHandler::chunkFinished, this, &CFDownloadQueue::onChunkFinished);

    // The key connection
    connect(download, &CFDownloadHandler::finished, this, &CFDownloadQueue::onFinished);

    m_downloads.append(download);

    // If it is first download added to empty queue, then start it immediately
    if (m_downloads.count() == 1) {
        m_currentIndex = 0;
        m_downloads.first()->start();
        m_restartTimer.start();
    }

    emit progressChanged();
    emit currentIndexChanged();
    emit countChanged();
}

void CFDownloadQueue::remove(int groupId)
{
    for (int i = 0; i < m_downloads.count(); i++) {
        if (m_downloads.at(i)->get_group()->get_uid() == groupId) {
            m_downloads.at(i)->stop();
            m_downloads.takeAt(i)->deleteLater();

            // Four cases:
            // user removed last download (that means cancellation)
            if (m_currentIndex >= m_downloads.count()) {
                cleanup();
                emit currentIndexChanged();
                emit cancelled();
            } else if (m_currentIndex == i) {
                // user removed currently downloaded group
                // then start next download, which is now positioned at currentIndex
                m_downloads.at(m_currentIndex)->start();
            } else if (m_currentIndex > i) {
                // user removed already downloaded group
                // then we need to decrement currentIndex
                m_currentIndex--;
                emit currentIndexChanged();
            }
            // fourth case: user removed yet to be downloaded group
            // nothing to do in that case

            emit progressChanged();
            emit countChanged();
            return;
        }
    }
}

void CFDownloadQueue::togglePaused()
{
    if (m_paused) {
        CFGlobal::vkApiRequestSender().resume();
        m_restartTimer.start();
    } else {
        CFGlobal::vkApiRequestSender().pause();
        m_restartTimer.stop();
    }
    update_paused(!m_paused);
}

void CFDownloadQueue::cancel()
{
    cleanup();

    emit currentIndexChanged();
    emit progressChanged();
    emit countChanged();
    emit cancelled();
}

int CFDownloadQueue::indexOf(int groupId) const
{
    int index = INVALID_INDEX;
    for (int i = 0; i < m_downloads.count(); i++) {
        if (m_downloads.at(i)->get_group()->get_uid() == groupId) {
            index = i;
        }
    }
    return index;
}

CFDownloadHandler *CFDownloadQueue::get(int groupId)
{
    CFDownloadHandler *download = nullptr;
    for (CFDownloadHandler *d : m_downloads) {
        if (d->get_group()->get_uid() == groupId) {
            download = d;
        }
    }

    return download;
}

CFDownloadHandler *CFDownloadQueue::at(int index)
{
    if (index < 0 || index >= m_downloads.count()) {
        return nullptr;
    }

    return m_downloads[index];
}

void CFDownloadQueue::onError(CFOpError *error)
{
    CFGroup *group = m_downloads[m_currentIndex]->get_group();

    error->update_groupId(group->get_uid());

    int errorCode = error->get_code();
    if (errorCode > QNetworkReply::UnknownServerError) {
        errorCode -= QNetworkReply::UnknownServerError;
        switch (errorCode) {
        case CFAPIError::ACCESS_DENIED:
        case CFAPIError::INVALID_GROUP_ID:
        case CFAPIError::ACCESS_TO_GROUP_DENIED:
            group->setAndPersistSubHidden(true);
        }
    }

    emit this->error(error);

    m_restartTimer.start();
}

void CFDownloadQueue::onChunkFinished(int processedCount)
{
    if (processedCount) {
        emit progressChanged();
    }
}

void CFDownloadQueue::onFinished(bool success)
{
    emit groupFinished(m_downloads.at(m_currentIndex)->get_group()->get_uid(), success);

    m_currentIndex++;
    // either download process is finished
    if (m_currentIndex >= m_downloads.count()) {
        m_currentIndex--;
        cleanup();
        emit countChanged();
        emit finished();
    } else {
        // or we need to start next download in queue
        m_downloads.at(m_currentIndex)->start();
        m_restartTimer.start();

        CFUserManager::instance().get_model()->updateUsersDownloaded();
    }
    emit currentIndexChanged();
    emit progressChanged();
}

void CFDownloadQueue::cleanup()
{
    if (m_downloads.length() > m_currentIndex && m_currentIndex >= 0)
        m_downloads[m_currentIndex]->stop();
    for (CFDownloadHandler *download : m_downloads) {
        download->deleteLater();
    }

    m_downloads.clear();
    m_currentIndex = INVALID_INDEX;
    m_restartTimer.stop();

    CFUserManager::instance().get_model()->updateUsersDownloaded();
}
