/* This file is part of SIR, an open-source cross-platform Image tool
 * 2007-2010  Rafael Sachetto <rsachetto@gmail.com>
 * 2011-2016  Marek Jędryka   <jedryka89@gmail.com>
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
 * Program URL: http://marek629.github.io/SIR/
 */

#ifndef CONVERTTHREAD_HPP
#define CONVERTTHREAD_HPP

#ifndef SIR_CMAKE
#define SIR_METADATA_SUPPORT
#endif // SIR_CMAKE

#include <QThread>
#include <QMutex>
#include "metadata/MetadataUtils.hpp"
#include "SharedInformation.hpp"

class QSvgRenderer;

#ifndef SIR_CMAKE
#define SIR_METADATA_SUPPORT
#endif // SIR_CMAKE

#ifndef SIR_METADATA_SUPPORT
class QImage;
#endif // SIR_METADATA_SUPPORT

/** \brief Image convertion thread class.
  *
  * Threads converting images work in main loop implemented in run() method.
  * \sa run()
  */
class ConvertThread : public QThread {
    Q_OBJECT
    friend class ConvertDialog;
    friend class ConvertThreadTest;
    friend class ImageFormatLinearSizeTest;
    friend class ImageFormatTargetFileSizeTest;

public:
    //! Enumerator for ConvertThread::question() signal.
    enum Question {
        Overwrite,
        Enlarge
    };
    //! Describes status of file convertion.
    enum Status {
        NotConverted,
        Converted,
        Skipped,
        Failed,
        Removed,
        Converting,
        Cancelled
    };

    ConvertThread(QObject *parent, int tid);
    void convertImage(const QString& name, const QString& extension,
                      const QString& path);
    void setAcceptWork(bool work);
    void getNextOrStop();

    void setImageStatus(const QStringList &imageData, const QString &message,
                        Status status);
    void askUser(const QString &targetFilePath, Question question);
    int getUserAnswer(Question question) const;
    char askOverwrite(QFile *tempFile);

    void fillImage(QImage *img);
    char computeSize(QSvgRenderer *renderer, const QString &imagePath);
#ifdef SIR_METADATA_SUPPORT
    void printError();
#endif // SIR_METADATA_SUPPORT
    static SharedInformation *sharedInfo();
    static void setSharedInfo(const SharedInformation &info);

signals:
    void imageStatus(QStringList imageData, QString status, int statusNum);
    void question(const QString& targetFilePath, int questionCode);
    void getNextImage(int tid);

protected:
    void run();

private:
    // fields
    static SharedInformation shared; /**< The theads shared information. */
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

    double countTargetFileSize(double fileSize);

    QString temporaryFilePath(int threadId);
    char askEnlarge(const QImage &image, const QString &imagePath);

    QImage *loadImage(const QString &imagePath, RawModel *rawModel,
                      bool isSvgSource);

    QImage paintEffects(QImage *image);
};

#endif // CONVERTTHREAD_HPP
