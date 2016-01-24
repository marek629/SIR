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
{

}

DetailsThumbnailTest::~DetailsThumbnailTest()
{

}

void DetailsThumbnailTest::initTestCase()
{

}

void DetailsThumbnailTest::cleanupTestCase()
{

}

#ifdef SIR_METADATA_SUPPORT
void DetailsThumbnailTest::test_writeThumbnailFromMetadata_metadataEnabled_emptyPreviewList()
{
    bool expectedResult = false;

    Settings *settings = Settings::instance();
    settings->metadata.enabled = true;

    DetailsThumbnail thumbnail = DetailsThumbnail(settings);
    thumbnail.imagePath = "/mnt/sda7/Zdjęcia/sir_test/metadata/test_metadata_nopreview.jpg";
    thumbnail.thumbPath = QDir::tempPath() + QDir::separator() + "sir_DetailsThumbnailTest";

    QCOMPARE(thumbnail.writeThumbnailFromMetadata(), expectedResult);
}

void DetailsThumbnailTest::test_writeThumbnailFromMetadata_metadataEnabled_metadataThumbnail()
{
    bool expectedResult = true;

    Settings *settings = Settings::instance();
    settings->metadata.enabled = true;

    DetailsThumbnail thumbnail = DetailsThumbnail(settings);
    thumbnail.imagePath = "/mnt/sda7/Zdjęcia/sir_test/metadata/test_metadata_preview.jpg";
    thumbnail.thumbPath = QDir::tempPath() + QDir::separator() + "sir_DetailsThumbnailTest";

    QCOMPARE(thumbnail.writeThumbnailFromMetadata(), expectedResult);
}

void DetailsThumbnailTest::test_writeThumbnailFromMetadata_metadataEnabled_invalidMetadata()
{
    bool expectedResult = false;

    Settings *settings = Settings::instance();
    settings->metadata.enabled = true;

    DetailsThumbnail thumbnail = DetailsThumbnail(settings);
    thumbnail.imagePath = "/mnt/sda7/Zdjęcia/sir_test/metadata/test_metadata_invalid.bmp";
    thumbnail.thumbPath = QDir::tempPath() + QDir::separator() + "sir_DetailsThumbnailTest";

    QCOMPARE(thumbnail.writeThumbnailFromMetadata(), expectedResult);
}
#endif // SIR_METADATA_SUPPORT

void DetailsThumbnailTest::test_writeThumbnailFromMetadata_metadataDisabled()
{
    bool expectedResult = false;
}

QTEST_MAIN(DetailsThumbnailTest)
#include "DetailsThumbnailTest.moc"
