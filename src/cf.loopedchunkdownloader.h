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

#ifndef CFLOOPEDCHUNKDOWNLOADER_H
#define CFLOOPEDCHUNKDOWNLOADER_H

#include <QObject>

#include "cf.downloadhandler.h"

#define CF_LOOPEDCHUNKDOWNLOADER_TYPENAME "CFLoopedChunkDownloader"

/*!
 * \brief The CFLoopedChunkDownloader class handles automatic download mode
 * implementing following strategy: loop through user's groups in priority order
 * and download m_userPerIteration users in current groups then proceed to next;
 * do the above until time runs out or all group will be downloaded
 */
class CFLoopedChunkDownloader : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(int, remainingTimeInSecs)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged) // [0.0, 1.0]

public:
    explicit CFLoopedChunkDownloader(const QList<CFGroup *> &groups,
                                     int allottedTimeInSecs,
                                     QObject *parent = nullptr);

    double progress() const;

public slots:
    /*!
     * \brief End download process and cleanup
     */
    void end();

signals:
    void progressChanged();

    /*!
     * \brief Emitted when automatic process is finished regardless of the cause
     */
    void finished();

    void error(CFOpError *error);

private:
    void initDownloads(QList<CFGroup *> groups);

    void nextChunk(bool finished = false);

    /*!
     * \brief Sets up members, starts timers and starts download process
     */
    void start();

    /*!
     * \brief Stops timers and emit finished()
     */
    void finish();

    QVector<CFDownloadHandler *> m_downloads;

    /*!
     * \brief Index of the current download
     */
    int m_index = INVALID_INDEX;

    int m_startTimeInSecs = 0;
    int m_allottedTimeInSecs = 0;

    QTimer m_progressUpdateTimer;
    QTimer m_endTimer;

    /*!
     * \brief Amount of users to be processed per group per cycle
     */
    int m_usersPerIteration = 10000;

};

#endif // CFLOOPEDCHUNKDOWNLOADER_H
