/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2012  Marek Jędryka   <jedryka89@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Program URL: http://sir.projet-libre.org/
 */

#ifndef CONVERTTHREAD_H
#define CONVERTTHREAD_H

#include <QThread>
#include <QMutex>
#include "metadatautils.h"
#include "sharedinformation.h"

class QSvgRenderer;

//! Enumerator for ConvertThread::question signal.
enum Question {
    Overwrite,
    Enlarge
};

/** \brief Image convertion thread class.
  *
  * Threads converting images work in main loop implemented in run method.
  * \sa run
  */
class ConvertThread : public QThread {
    Q_OBJECT
    friend class ConvertDialog;

public:
    ConvertThread(QObject *parent, int tid);
    void convertImage(const QString& name, const QString& extension,
                      const QString& path);
    void setAcceptWork(bool work);
    void getNextOrStop();
    void printError();
    static SharedInformation *sharedInfo();

signals:
    void imageStatus(QStringList imageData, QString status, int statusNum);
    void question(const QString& targetFilePath, Question whatToDo);
    void getNextImage(int tid);

private:
    // fields
    static SharedInformation *shared;
    bool work;
    QStringList imageData;
    int tid;
    bool hasWidth;
    bool hasHeight;
    int width;
    int height;
    char sizeComputed;
    bool saveMetadata;
    bool rotate;
    double angle;
    MetadataUtils::Metadata metadata;
    QString targetFilePath;
    // methods
    void run();
    void rotateImage(QImage *image);
    void updateThumbnail(const QImage *image);
    char computeSize(const QImage *image, const QString &imagePath);
    char computeSize(QSvgRenderer *renderer, const QString &imagePath);
    bool isLinearFileSizeFormat(double *destSize);
    char askEnlarge(const QImage &image, const QString &imagePath);
    char askOverwrite(QFile *tempFile);
};

#endif
