/****************************************************************************
**
** Copyright © 2020 Barbatum™.
** All rights reserved.
** Contact: support@barbatum.ru
**
**
** This file is part of VCognitis project.
**
** VCognitis is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** VCognitis is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with VCognitis.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef CFDOWNLOADQUEUE_H
#define CFDOWNLOADQUEUE_H

#include <QObject>
#include <QTimer>

#include "QQmlHelpers"
#include "QQmlObjectListModel"

#include "cf.global.h"
#include "structs/cf.operror.h"

class CFDownloadHandler;
class CFGroup;

#define CF_DOWNLOADQUEUE_TYPENAME "CFDownloadQueue"

class CFDownloadQueue : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(bool, paused) // Controls pause/play state
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged) // [0.0, 1.0]
    Q_PROPERTY(double estimatedDuration READ estimatedDuration NOTIFY estimatedDurationChanged) // In seconds
    Q_PROPERTY(int count READ count NOTIFY countChanged) // Count of groups in download queue
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged) // Index of the current download in queue
public:
    explicit CFDownloadQueue(QObject *parent = nullptr);

    double progress() const;
    int estimatedDuration() const;
    int count() const;
    int currentIndex() const;

    /*!
     * \brief Replace group object with given one
     * if such group is present in download queue
     *
     * This is needed for a nasty case when groups' select() is called during download
     */
    void replace(CFGroup *group);

public slots:
    void add(int groupId);
    void remove(int groupId);
    void togglePaused();

    /*!
     * \brief Immediately removes all downloads
     */
    void cancel();

    int indexOf(int groupId) const;
    CFDownloadHandler *get(int groupId);
    CFDownloadHandler *at(int index);

signals:
    void progressChanged();
    void estimatedDurationChanged();
    void countChanged();
    void currentIndexChanged();

    /*!
     * \brief Emitted when all groups were downloaded
     */
    void finished();
    /*!
     * \brief Emitted either on cancel() or when user manually removes the last group
     */
    void cancelled();
    /*!
     * \brief Emitted when group with given ID is finished
     * \param success true if group was downloaded successfully
     */
    void groupFinished(int groupId, bool success);

    void error(CFOpError *error);

private:
    void onError(CFOpError *error);
    void onChunkFinished(int processedCount);
    void onFinished(bool success);

    /*!
     * \brief Resets data members to default states
     */
    void cleanup();

    QList<CFDownloadHandler *> m_downloads;
    int m_currentIndex = INVALID_INDEX;

    /*!
     * \brief Restarts current download if there was no progress for 30 secs
     */
    QTimer m_restartTimer;
};

#endif // CFDOWNLOADQUEUE_H
