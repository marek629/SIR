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

#include "tests/DetailsThumbnailTest.hpp"

#include "DetailsThumbnail.hpp"
#include "Settings.hpp"


DetailsThumbnailTest::DetailsThumbnailTest()
    : temporaryPath(QDir::tempPath() + QDir::separator())
    , fileNamePrefix("sir_DetailsThumbnailTest")
{}

DetailsThumbnailTest::~DetailsThumbnailTest() {}

QFileInfoList DetailsThumbnailTest::existingTestFileInfoList()
{
    QStringList fileFilters;
    fileFilters << QString("%1*").arg(fileNamePrefix);

    QDir tempDir = QDir(temporaryPath);
    tempDir.setNameFilters(fileFilters);

    return tempDir.entryInfoList();
}

bool DetailsThumbnailTest::writeFile(const QString &filePath,
                                     const QByteArray &bytes)
{
    QFile file(filePath);

    if (file.open(QIODevice::WriteOnly))
    {
        return file.write(bytes);
    }

    return false;
}

void DetailsThumbnailTest::initTestCase()
{
    QString errorMessage = temporaryPath +
            " directory is not clear. Please remove files within prefix " +
            fileNamePrefix;

    QVERIFY2(existingTestFileInfoList().empty(),
             errorMessage.toLatin1().constData());
}

void DetailsThumbnailTest::cleanupTestCase()
{
    foreach (QFileInfo fileInfo, existingTestFileInfoList()) {
        QFile file(fileInfo.absoluteFilePath());
        QVERIFY(file.remove());
    }
}

#ifdef SIR_METADATA_SUPPORT
void DetailsThumbnailTest::test_writeThumbnailFromMetadata_metadataEnabled_emptyPreviewList()
{
    bool expectedResult = false;

    Settings *settings = Settings::instance();
    settings->metadata.enabled = true;

    QString testImagePath = temporaryPath + fileNamePrefix + "_test_metadata_nopreview.jpg";

    QByteArray imageData = QByteArray::fromBase64(metadataNoPreviewImageData);
    QVERIFY2(writeFile(testImagePath, imageData),
             "Write image file for test was failed.");

    DetailsThumbnail thumbnail = DetailsThumbnail(settings);
    thumbnail.imagePath = testImagePath;
    thumbnail.thumbPath = temporaryPath + fileNamePrefix;

    QCOMPARE(thumbnail.writeThumbnailFromMetadata(), expectedResult);
}

void DetailsThumbnailTest::test_writeThumbnailFromMetadata_metadataEnabled_metadataThumbnail()
{
    bool expectedResult = true;

    Settings *settings = Settings::instance();
    settings->metadata.enabled = true;

    QString testImagePath = temporaryPath + fileNamePrefix + "_test_metadata_preview.jpg";

    QByteArray imageData = QByteArray::fromBase64(metadataPreviewImageData);
    QVERIFY2(writeFile(testImagePath, imageData),
             "Write image file for test was failed.");

    DetailsThumbnail thumbnail = DetailsThumbnail(settings);
    thumbnail.imagePath = testImagePath;
    thumbnail.thumbPath = temporaryPath + fileNamePrefix;

    QCOMPARE(thumbnail.writeThumbnailFromMetadata(), expectedResult);
}

void DetailsThumbnailTest::test_writeThumbnailFromMetadata_metadataEnabled_invalidMetadata()
{
    bool expectedResult = false;

    Settings *settings = Settings::instance();
    settings->metadata.enabled = true;

    // test image file should not exist and it's fine
    QString testImagePath = temporaryPath + fileNamePrefix + "_test_metadata_invalid.bmp";

    DetailsThumbnail thumbnail = DetailsThumbnail(settings);
    thumbnail.imagePath = testImagePath;
    thumbnail.thumbPath = temporaryPath + fileNamePrefix;

    QCOMPARE(thumbnail.writeThumbnailFromMetadata(), expectedResult);
}
#endif // SIR_METADATA_SUPPORT

void DetailsThumbnailTest::test_writeThumbnailFromMetadata_metadataDisabled()
{
    bool expectedResult = false;

    Settings *settings = Settings::instance();
    settings->metadata.enabled = false;

    DetailsThumbnail thumbnail = DetailsThumbnail(settings);

    QCOMPARE(thumbnail.writeThumbnailFromMetadata(), expectedResult);
}

QTEST_MAIN(DetailsThumbnailTest)
#include "DetailsThumbnailTest.moc"
