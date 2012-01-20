/*
* This file is part of SIR, an open-source cross-platform Image tool
* 2007  Rafael Sachetto
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
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Rafael Sachetto <rsachetto@gmail.com>
* Program URL: http://sir.projet-libre.org/
*
* File contributed by: Víctor Fernández <deejayworld@gmail.com>
*/

#ifndef CONVERTTHREAD_H
#define CONVERTTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QImage>
#include <QDir>
#include "sharedinformation.h"
#include "metadatautils.h"

//! Image convertion thread class.
/** Threads converting images work in main loop implemented in run method.
  * \sa run
  */
class ConvertThread : public QThread {

    Q_OBJECT

public:
    //! Default constructor.
    /** \param parent parent object
      * \param tid thread ID
      */
    ConvertThread(QObject *parent, int tid);

    void convertImage(const QString& name, const QString& extension,
                      const QString& path);
    void confirmOverwrite(int result);
    void confirmEnlarge(int result);
    void confirmImage();
    void setAcceptWork(bool work);
    void getNextOrStop();
    void printError();

    //! Set desired size.
    /** Set desired size in pixels or percent, depend on \a percent value.
      * \param width Width of desired image.
      * \param height Height of desired image.
      * \param percent Sets desired width and height in pixels if false, otherwise
      *     sets size as percent of original image size.
      * \param hasWidth Sets desired width to \a width if true.
      * \param hasHeight Sets desired height to \a height if true.
      * \param maintainAspect If true image will be scaled with keeping apsect ratio.
      */
    static void setDesiredSize(int width, int height, bool percent = false,
                        bool hasWidth = false, bool hasHeight = false,
                        bool maintainAspect = true);
    //! Set desired size.
    /** \par
      * This is overloaded function.
      *
      * \par
      * Sets disired size in bytes. Result file size can be lower
      * but never grower than \a bytes.
      */
    static void setDesiredSize(quint32 bytes);

    //! Set desired format string without point prefix.
    /** \note Call this function after calling #setSaveMetadata.
      */
    static void setDesiredFormat(const QString& format);

    //! Allow rotate and set desired rotation angle.
    /** \param rotate Allows rotate.
      * \param angle Clockwise rotation angle.
      */
    static void setDesiredRotation(bool rotate, double angle = 0.0);

    //! Set desired image quality.
    /** \param quality Integer factor in range 0 to 100.
      */
    static void setQuality(int quality);

    //! Set destination file name prefix.
    static void setDestPrefix(const QString& destPrefix);
    //! Set destination file name suffix.
    static void setDestSuffix(const QString& destSuffix);
    //! Set destination directory path.
    static void setDestFolder(const QDir& destFolder);
    //! Allow overwrite all files.
    static void setOverwriteAll(bool overwriteAll = false);

    //! Enable metadata support.
    /** Enable metadata support if true, otherwise disables metadata support.
      */
    static void setMetadataEnabled(bool value);

    //! Enable save metadata option.
    /** Enable save metadata option if true, otherwise disables saving metadata.
      * \note Call this function before calling #setDesiredFormat.
      */
    static void setSaveMetadata(bool value);
    static void setRealRotate(bool rotate);
    static void setUpdateThumbnail(bool update);
    static void setRotateThumbnail(bool rotate);

    static SharedInformation *shared;

signals:
    void imageStatus(QStringList imageData, QString status, int statusNum);
    void question(const QString& targetFilePath, int tid, const QString& whatToDo);
    void getNextImage(int tid);

private:
    void run();
    void rotateImage(QImage *image);
    void updateThumbnail(const QImage *image);
    bool computeSize(const QImage *image, const QString &imagePath);

    bool work;
    QStringList imageData;
    QMutex imageMutex;
    QWaitCondition imageCondition;
    int tid;
    bool hasWidth;
    bool hasHeight;
    int width;
    int height;
    bool saveMetadata;
    bool rotate;
    double angle;
    QMutex overwriteMutex;
    QWaitCondition overwriteCondition;
    QMutex enlargeMutex;
    QWaitCondition enlargeCondition;
    MetadataUtils::Metadata metadata;
    QString targetFilePath;
};


#endif
