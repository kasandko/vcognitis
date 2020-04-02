#include "cf.loopedchunkdownloader.h"

#include "managers/cf.groupmanager.h"

CFLoopedChunkDownloader::CFLoopedChunkDownloader(const QList<CFGroup *> &groups, int allottedTimeInSecs, QObject *parent)
    : QObject(parent),
      m_remainingTimeInSecs(allottedTimeInSecs), // Init with allotted time so progress will be 0 initially (see formula)
      m_startTimeInSecs(QDateTime::currentSecsSinceEpoch()),
      m_allottedTimeInSecs(allottedTimeInSecs)
{
    connect(this, &CFLoopedChunkDownloader::remainingTimeInSecsChanged,
            this, &CFLoopedChunkDownloader::progressChanged);

    m_progressUpdateTimer.setInterval(1000);
    connect(&m_progressUpdateTimer, &QTimer::timeout, [=]{
        int passedTimeInSecs = QDateTime::currentSecsSinceEpoch() - m_startTimeInSecs;
        update_remainingTimeInSecs(allottedTimeInSecs - passedTimeInSecs);
    });

    m_endTimer.setInterval(allottedTimeInSecs * 1000);
    m_endTimer.setSingleShot(true);
    connect(&m_endTimer, &QTimer::timeout,
            this, &CFLoopedChunkDownloader::end);

    initDownloads(groups);

    // Start download
    if (!m_downloads.isEmpty()) {
        start();
    } else {
        QTimer::singleShot(0, this, &CFLoopedChunkDownloader::finished);
    }
}

double CFLoopedChunkDownloader::progress() const
{
    return std::min(1 - (static_cast<double>(m_remainingTimeInSecs) / (m_allottedTimeInSecs)), 1.);
}

void CFLoopedChunkDownloader::end()
{
    // I am not sure but think that deleteLater() will be safer than operator delete
    for (CFDownloadHandler *download : m_downloads) {
        download->deleteLater();
    }
    m_downloads.clear();

    finish();
}

void CFLoopedChunkDownloader::initDownloads(QList<CFGroup *> groups)
{
    // Provided groups are in ID order so they need to be sorted
    std::sort(groups.begin(), groups.end(),[](CFGroup *left, CFGroup *right){
        return left->get_priority() > right->get_priority();
    });

    int wholeSubCount = 0;
    int groupCount = 0;

    for (CFGroup *group : groups) {
        CFDownloadHandler *download = new CFDownloadHandler(group, this);

        wholeSubCount += group->get_subCount();
        groupCount++;

        // Resend error up the chain
        connect(download, &CFDownloadHandler::error, [=](CFOpError *error){
            error->update_groupId(download->get_group()->get_uid());

            int errorCode = error->get_code();
            if (errorCode > QNetworkReply::UnknownServerError) {
                errorCode -= QNetworkReply::UnknownServerError;
                switch (errorCode) {
                case CFAPIError::ACCESS_DENIED:
                case CFAPIError::INVALID_GROUP_ID:
                case CFAPIError::ACCESS_TO_GROUP_DENIED:
                    download->get_group()->setAndPersistSubHidden(true);
                }
            }

            emit this->error(error);
        });

        connect(download, &CFDownloadHandler::partlyDownloadFinished, [this] { nextChunk(); });
        connect(download, &CFDownloadHandler::finished, [this] {
            // Then this group needs to be removed from the array
            // Deleting group will end all opertaion related to it safely
            m_downloads.at(m_index)->deleteLater();
            m_downloads.remove(m_index);
            nextChunk(true);
        });

        m_downloads.append(download);
    }


    // Caluculate aprox chunk size
    wholeSubCount = qMin(wholeSubCount / 1000 + 1, m_allottedTimeInSecs * 3);
    m_usersPerIteration = groupCount ? (wholeSubCount / groupCount) * 1000 : 0;
}

void CFLoopedChunkDownloader::nextChunk(bool finished)
{
    if (m_downloads.isEmpty()) {
        finish();
        return;
    }

    if (!finished) {
        m_index++;
    }
    if (m_index >= m_downloads.count()) {
        m_index = 0;

        int wholeSubCount = 0;
        int groupCount = 0;
        for (CFDownloadHandler *handler : m_downloads) {
            wholeSubCount += handler->get_group()->get_subCount() - handler->get_group()->get_offset();
            groupCount++;
        }
        // Recalculate aprox chunk size
        wholeSubCount = qMin(wholeSubCount / 1000 + 1, m_remainingTimeInSecs * 3);
        m_usersPerIteration = (wholeSubCount / groupCount) * 1000;
    }

    m_downloads.at(m_index)->startPartly(m_usersPerIteration);
}

void CFLoopedChunkDownloader::start()
{
    m_index = 0;
    m_downloads.at(m_index)->startPartly(m_usersPerIteration);
    m_progressUpdateTimer.start();
    m_endTimer.start();
}

void CFLoopedChunkDownloader::finish()
{
    m_endTimer.stop();
    m_progressUpdateTimer.stop();
    emit finished();
}
