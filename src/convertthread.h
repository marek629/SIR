/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2013  Marek Jędryka   <jedryka89@gmail.com>
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

#ifndef SIR_CMAKE
#define SIR_METADATA_SUPPORT
#endif // SIR_CMAKE

#include <QThread>
#include <QMutex>
#include "metadatautils.h"
#include "sharedinformation.h"

class QSvgRenderer;
#ifndef SIR_METADATA_SUPPORT
class QImage;
#endif // SIR_METADATA_SUPPORT

//! Enumerator for ConvertThread::question() signal.
enum Question {
    Overwrite,
    Enlarge
};

/** \brief Image convertion thread class.
  *
  * Threads converting images work in main loop implemented in run() method.
  * \sa run()
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
#ifdef SIR_METADATA_SUPPORT
    void printError();
#endif // SIR_METADATA_SUPPORT
    static SharedInformation *sharedInfo();

signals:
    void imageStatus(QStringList imageData, QString status, int statusNum);
    void question(const QString& targetFilePath, Question whatToDo);
    void getNextImage(int tid);

protected:
    void run();

private:
    // fields
    static SharedInformation *shared; /**< The theads shared information. */
    bool work; /**< True means this thread still working. */
    QStringList imageData; /**< List of strings: file name, extension and path. */
    int tid; /**< The thread ID. */
    /** If it's true the converting image will be scaled to #width value. */
    bool hasWidth;
    /** If it's true the converting image will be scaled to #height value. */
    bool hasHeight;
    /** Desired width of the converting image.
      * \sa #hasWidth
      */
    int width;
    /** Desired height of the converting image.
      * \sa #hasHeight
      */
    int height;
    /** Describes size compute state.
      * \sa computeSize()
      */
    char sizeComputed;
    /** If it's true the converting image will be rotated by #angle value. */
    bool rotate;
    /** Desired rotation angle of the converting image in degree.
      * \sa #rotate
      */
    double angle;
    struct ThreadPrivateData {
        QString imagePath;
        QStringList imgData;
    } pd;
#ifdef SIR_METADATA_SUPPORT
    bool saveMetadata;
    MetadataUtils::Metadata metadata;
#endif // SIR_METADATA_SUPPORT
    QString targetFilePath;
    // methods
    QImage rotateImage(const QImage &image);
#ifdef SIR_METADATA_SUPPORT
    void updateThumbnail(const QImage &image);
#endif // SIR_METADATA_SUPPORT
    char computeSize(const QImage *image, const QString &imagePath);
    char computeSize(QSvgRenderer *renderer, const QString &imagePath);
    bool isLinearFileSizeFormat(double *destSize);
    char askEnlarge(const QImage &image, const QString &imagePath);
    char askOverwrite(QFile *tempFile);
    void fillImage(QImage *img);
    QImage *loadSvgImage();
    QImage paintEffects(QImage *image);
};

#endif // CONVERTTHREAD_H
