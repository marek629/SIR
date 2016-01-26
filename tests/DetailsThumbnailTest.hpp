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

#ifndef DETAILSTHUMBNAILTEST_HPP
#define DETAILSTHUMBNAILTEST_HPP

#include <QtTest/QTest>


class DetailsThumbnailTest : public QObject
{
    Q_OBJECT

public:
    DetailsThumbnailTest();
    ~DetailsThumbnailTest();

private:
    const QString temporaryPath;
    const QString fileNamePrefix;

    QFileInfoList existingTestFileInfoList();
    bool writeFile(const QString &filePath, const QByteArray &bytes);

private slots:
    void initTestCase();
    void cleanupTestCase();

#ifdef SIR_METADATA_SUPPORT
    void test_writeThumbnailFromMetadata_metadataEnabled_emptyPreviewList();
    void test_writeThumbnailFromMetadata_metadataEnabled_metadataThumbnail();
    void test_writeThumbnailFromMetadata_metadataEnabled_invalidMetadata();
#endif // SIR_METADATA_SUPPORT
    void test_writeThumbnailFromMetadata_metadataDisabled();

private:
    // below is stored test images BASE64 encoded
    const char *metadataNoPreviewImageData =
        "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"
        "d1GrF1ndRbF1nbGtRbF1nbF1ndRqxdZ2xdRbnrUXUW53UWxdTaAAAAAAAAAAAB0fxvngAAAAA8M7"
        "qNWLrO2LqLYus9ai2LrO2LrO6jVi6i3O6i2LrO2Nai3O6i6i3O6jVi6zti6i6ztjWotzuotzuo1q"
        "LYus7YuptAAAAAAAAAAAA6P43zwAAAAAeEXWdsa1Fud1FsXWdsa1F1nbF1Fud1GrF1ndRbF1nbGt"
        "RbndRdRbndRqxdRbndRbF1nrUW53UWxdZ2xrUXUW53U2gAAAAAAAAAAAdH8b54AAAAAPCLYus7qN"
        "WLrO2LqLc7qNWLrO6i3O6jVi6i3O6i6i3O6jVi6zti6i6z1Yuotzuoti6zuo1Yus7Yuotz1qLqLY"
        "uotAAAAAAAAAAAA6P43zwAAAAAeEWxdRdZ6sXWdsXUW53UasXWd1FsXUW561FsXWd1FsXWerF1Fu"
        "d1FsXWetRbF1nbF1FuetRdRbndRbF1nqxdRdRaAAAAAAAAAAAB0fxvngAAAAA8Iti6i3PWouotzu"
        "oti6z1Yuous7Yuotz1qLYus7qLYus9WLqLc7qLYus7qNWLrO2LqLc9ai6i3O6i2LqNXO6i6i0AAA"
        "AAAAAAAADo/jfPAAAAAB4RbF1Fsa1nbF1ndRbF1FuetRbF1nbF1GtZ2xdRbndRbF1nrUWxdZ2xdR"
        "bnrUXUW53UWxdZ6sXUXWdsXUaud1FsXQAAAAAAAAAAAHR/G+eAAAAADwm2LrO2LrPVi6i2LrO6i3"
        "PWoti6zti6jWs7Yuotzuoti6jWs7Yus7YuotjWs7Yuous7Yus9WLqLqLc7qNWLrO2LoAAAAAAAAA"
        "AADo/jfPAAAAAAIti6zti6jVzuoti6zti6jWs7Yuotzuouo1c7qLYus7Yuotz1qLqLc7qLY1rO2L"
        "qLrO2LqLc9ai2LqLrPVi6zti0oAAAAAAAAAAA6P43zwAAAAACLYtzuouotjWs7Yus7Yuouo1c7qL"
        "c7qLY1qLrO2LqLc7qLY1rO6i2LrO2LrPVi6i2LrO2LqNazti6i3O6jVi6ztKAAAAAAAAAAAOj+N8"
        "8AAAAAAi2LYus7Yuo1rO2LqLc7qLY1rO6i2LrO2Nai3O6i6i3O6i2LrPVi6i6zti6i3PWoti6zti"
        "6jWs7YuotzuotjWot8UAAAAAAAAAAAdH8b54AAAAABNudsXUW53UXUaud1FsXWdsXUa1nbF1Fud1"
        "GrF1nbF1F1nbF1GrndRbF1nbF1GtZ2xdRbndRdRq53UWxdZ2xrUW+KAAAAAAAAAAAOj+N88AAAAA"
        "Am3O2LqLc7qLY1rO6i2LrO2LqNXO6i6i2LrPVi6zti6i3O6i6jVi6zti6i3O6i6jVzuoti6ztjWo"
        "us7Yus7Yuot8tAAAAAAAAAAAHR/G+eAAAAAASsaud1FsXWdsXUa1nbF1Fud1Fsa1ndRbF1nbGtRb"
        "ndRbF1ndRbGtZ2xdRbndRbGtZ2xdRdZ2xdRq53UWxdZ3UW+WgAAAAAAAAAADo/jfPAAAAAAJI1qL"
        "rO2LqLc7qNWLrO6i3O6i2Nai3O6i3O6jWoti6zti6i3O6jWotzuoti6ztjWotzuouotzuo1Yus7Y"
        "uotzuvLQAAAAAAAAAAB0fxvngAAAAAEkXUa1nbF1Fud1Fsa1nbF1F1FuetRbndRbF1F1nqxdZ2xd"
        "RbndRrUW53UWxdZ2xdRq53UWxdZ3UasXUW53UWxdTaAAAAAAAAAAAP/EABQQAQAAAAAAAAAAAAAA"
        "AAAAALD/2gAIAQEAAQUCeg//xAAUEQEAAAAAAAAAAAAAAAAAAACw/9oACAEDAQE/AXoP/8QAFBEB"
        "AAAAAAAAAAAAAAAAAAAAsP/aAAgBAgEBPwF6D//EABQQAQAAAAAAAAAAAAAAAAAAALD/2gAIAQEA"
        "Bj8Ceg//xAAYEAEBAQEBAAAAAAAAAAAAAAABAFAwQP/aAAgBAQABPyHkzMzMzMzMzMzMzMzMzMzM"
        "zMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMz"
        "MzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMz"
        "MzMzOOzMzMzMzMzMzMzMzMzMzMzMzMzMzOOzMzMzMzMzMzMzMzMzMzMzMzMzMzOOzMzMzMzMzMzM"
        "zMzMzMzMzMzMzMzOOzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzj"
        "MzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMz"
        "MzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzM"
        "zMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzM"
        "zMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMz"
        "MzMzMzMzMzMzMzMzMzMzM47MzMzMzMzMzMzMzMzMzMzMzMzMzM47MzMzMzMzMzMzMzMzMzMzMzMz"
        "MzM47MzMzMzMzMzMzMzMzMzMzMzMzMzM47MzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzM"
        "zMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzj"
        "MzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMz"
        "MzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzM"
        "zMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzM"
        "zMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOOzMzMzMz"
        "MzMzMzMzMzMzMzMzMzMzOOzMzMzMzMzMzMzMzMzMzMzMzMzMzOOzMzMzMzMzMzMzMzMzMzMzMzMz"
        "MzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzM"
        "zMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzO"
        "MzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMz"
        "MzMzMzMzMzMzPp//2gAMAwEAAgADAAAAEAAAAAPjY9RYvDulBc9TevCO9wc2AAAAAAAAAAAAAAAA"
        "AAHiO1YYnCOnSZ1wenqK34d0AAAAAAAAAAAAAAAAAAHqLl4Z1qHnqZ14T2uD24R0AAAAAAAAAAAA"
        "AAAAAAGvDG9R85XOrjU5RevDu5R8AAAAAAAAAAAAAAAAAAMnCOnTc9S+nCe1Qc1KO3Y4AAAAAAAA"
        "AAAAAAAAAAJ3KPnqd0Yf2qf24Z1IHnIAAAAAAAAAAAAAAAAAAAJ06HuqR0ZX+rDupR85HuqAAAAA"
        "AAAAAAAAAAAAAAJ87TunC85Q8/C+nQdxQujLAAAAAAAAAAAAAAAAAAId1a/nK+xY91KPmId1YNnL"
        "AAAAAAAAAAAAAAAAAAAV0YVmrHwY106HmqHkYV+LQAAAAAAAAAAAACAAAABT85T+rHuhU8xHurCs"
        "xQsjQAAAAAAAAAAAAAAAAABC81c9jOvBM81YvjKvxY9yQAAAAAAAAAAAAAQAAABKfkYV0aniKV9Y"
        "ViKnmY1wQAAAAAAAAAAAAAAAAAALH+YU8LnqLH8ZU+DHsZ18wAAAAAAAAAAAAAAAAAAjGuhQ95ev"
        "DGsRU+jOvDc9wAAAAAAAAAAAAAAAAABhKnnItxMvDOvhc1waniK1wAAAAAAAAAAAAAAAAABwLHiL"
        "lwZ0qLnoZ14bnqf1wAAAAAAAAAAAAAAAAAB8RmqDk4R06DHuDU8R2uDmwAAAAAAAAAAAAAAAAAA9"
        "RMvjOtQ85TOvCe1Qe3COgAAAAAAAAAAAAAAAAAA1ZY1jLnYd1SanKf14d3KPgAAAAAAAAAAAAAAA"
        "AAA14R06DnqR14T2qDW5d04PgAAAAAAAAAAAAAAAAABm8R84TurG89R+rDO7B85AgAAAAAAAAAAA"
        "AAAAAABOnYd9SOjC+1YenKPnA81QQAAAAAAAAAAAAAAAAABPnoV04PnKf34f1KDmoN0YYAAAAAAA"
        "AAAAAAAAAABDmrX04X2LDuoV84DuLBsZYAAAAAAAAAAAAAAAAAAC+jC8xR8jCuhQ8xQ+jGsxYAAA"
        "AAAAAAAAAAAAAAACvjKf1Q9zKvnI11Y9nKniaAAAAAAAAAAAAAAAAAAKXmLD0YVyLnmKV0ZV2Knk"
        "SAAAAAAAAAAAAAAAAAAJX+rHu508RGurHs5U8DGuSAAAAAAAAAAAAAAAAAABYdjKvxU9RYvjGtxY"
        "9zKviAAAAAAAAAAAAAAAAAABY1wKnjM1wY1iKHkY1xa3uAAAAAAAAAAAAAAAAAAEJV8JHuLl4ZVm"
        "LGubV8b2uAAAAAAAAAAAAAAAAAAOBU8R9uDO8RU8DOvTc9R++AAAAAAAAAAAAAAAAAAPiM9xcnDO"
        "nRc9CbvCO1Qe2AAAAAAAAAAAAAAAAAAHqJF4Y3iPn6Y16T3qb3Yd2AAAAAAAAAAAAAAAAAAHqLm4"
        "R1qDuuT09R2uD34V8AAAAAAAAAAAAAAAAAAGvDOvR89TuvD89Qe3De/Q8AAAAAAAAAAAAAAAAAAO"
        "3iPnyc9Sfnie1Yd3oOnIQAAAAAAAAAAAAAAAAAAJ0rPnqd04XmqD24R04HmqAAAAAAAAAAAAAAAA"
        "AAAJ87DurR05XurDm9R84DuiAAAAAAAAAAAAAAAAAAAJcxTunC+xU81CvnQd1QujLAAAAAAAAAAA"
        "AAAAAAAIV1YXnKPxY90KLmKF1YliLAAAAAAAAAAAAAAAAAAAV0YT+LHkJV86DmLBs5UqLQAAAAAA"
        "AAAAAAAAAAABD8xR8jH+RQ8zCurCvxU9TQAAAAAAAAAAAAAAAAABO/xY9zKvDId1YtnKvxc9yQAA"
        "AAAAAAAAAAAAAAAAKH0YV0KniKF8Y1+KHsZV4QAAAAAAAAAAAAAAAAAALHuJV8YkqLHsRU8jGuRV"
        "8wAAAAAAAAAAAAAAAAAAjKvhY9xsvDKtxc9zevje9wAAAAAAAAAAAAAAAAAByKniI9wc9CKngYV4"
        "aniO3wAAAAAAAAAAAAAAAAAB8JHuLE4Z0qLnoZV8Z3qD2wAAAAAAAAAAAP/EABkRAQEBAQEBAAAA"
        "AAAAAAAAAAEAEFAwQP/aAAgBAwEBPxDyIiIjCIiIiIiMIiIiIiIiIiIiIiIjjGEREREREYREYRER"
        "ERERERERGERHHMMIiIiIiIiIiIiIiIiIiMIiIiIiOMRERERhERERERERERERERERERERERxiIwiI"
        "iIiIiIiIiIiIiIiIiIiIiIiOMRGERERERERERERERERERERERERERxyIiIjCIiIiIiIiIiIiIiMI"
        "iIiIiOOREREREREREREYRERERERhEREYRHHIiIiIiIiIiIiIjCIiIiIiMIiIiI45ERERERERERhE"
        "RhERERERGEREREcciIiIiIiIiIiIiMMIiIiIiIiIiIiOMREREREREREYREYYRERERERhERERxiIi"
        "IiIiIiIiIiIiIiIjCIiIiIiIiOMRERERERERERERERhhGGEREREREcciIiMIiIiIiIiIiIiIiIwi"
        "IiIiIiOMRERGEREREREYRERERERERERERERHGIiIiIiIiIiIiIiIiIiIjCIiIiIiI4xEREYRERGE"
        "REREREREREREREREREcYwiIiIiIiIiIiIiIiIwiIiIiMIiOORGERGERERhERERERERERhEREREcc"
        "iMIiIiIjDCIiIiIiIiIiIiIiMIjjERhERERERGERERERERERERERERERxyIiIiIiIiMIiIiIiIiI"
        "iIjCIiIiOOREYREREREREREREREREREREREREcciIiIiIiIiIiIiMIiIiIiIwwiIiOOREYREYRER"
        "ERERhEREREREREREYcciIiIiIiIiIwiIiIiIjCIiIiIiIiOMREREREREREREREREREYRERERERhH"
        "GIiIiIjDCIiIiIiIiIiIwiIwiIiI4xERERERERERERERGGEYYRERERERHGIiIjCIjCIiIwjCIiIi"
        "MIiIjCIiOMYREYRERGERGERERERERERERERERxjCIiIiIwwiIiIiMIiIiIiIiIiIiOMYREYRERGE"
        "REREREYRERERERERERxjCIiIiIiMIiMIiIiIiIiIiIiIiI45hhERhEREYRERERERERERERERERHG"
        "MMIiIiIiIwiIiIiIiIiIiIiIiIjjkRERERERERhEREREREREREREYYRHGIiIiIiIiIiIiIiIiIiI"
        "iIiIiIiIiOOREREREREREREREREREREYREREREcciIiIiIiIiIiIiIiIjCIjCIwjCI45ERERERER"
        "EREREYRERGEYRERERhxyIjCIiIiIiIiIiIiIiMIiMIiIiIjjERERERhERERERGERERhERhEREREc"
        "YiIiIiMIiIiIwjDDCMIiIjCIiIjjERERERhEREYREYRERERERERERERHGIiIjCIjCIiIiIiIiIiI"
        "iIiIiIiIjjERERERERGEYYRERERERERERGEREcYiIiIiIjDCIjCIiIwiIiIiIjCIiOMYRERERGGE"
        "RGERERERERERERERER9P/8QAGhEBAQEBAQEBAAAAAAAAAAAAAQAQUCAwQP/aAAgBAgEBPxD5EYYR"
        "EYRGkR4IiIjCMIiIjCIiOMRGERHgjSI9ERhGkaRhhEcYjSIjDCIjCIjSIjDCIjSIwiI4xGERER4I"
        "jCMMMIjCMIiMIiMIjjkREYR4IjCIjyRGmkaRGEcgiIiMI8EYYRhpERpEaREcsiIwjCNIjSMIjCIw"
        "jTCIiIjkEeCMI0iMIiIjCIjDSIjCIjlEaRERpEaRhGEYaRER4I5RhGEREeCIiMIiNIwiIiI0iI5B"
        "hERGERGERpERGkRhhERERGHLIjDCNIjDDCPBhEYRGkRHLIiIiIiIiIjDCI8ERGkeCI5BGERGkRpE"
        "aYREaYRhERERERHGIiIiMIiPBERpERhGERER5Iw4xhERhGEaREYaRGEYeCIiIiMOMREYRGEREYRh"
        "GkREaREaRGERHGIjCIiMIjSI8ERHgiIwiI0iI4xEYRERpERERGmERhhERhERGERxiI0jDSI8ERhp"
        "EYRERGEREREcYiMIwiNIjCIjDSPBhGEREYRHGIiPBEYRERhEYRERHgiIiIjDCOORph4MI8EYRhER"
        "GGERGERhyCIiPBGERGERhEREaRhEaRHLIiNIjSIjCIiMIjSMMMIiIw45EYYRERGERHgjSMNIiNIi"
        "OURGGkRpEaYRGkYaRERhHLIjTSIjCNIiPBGGGkRERHKIjCIwiMIiPBERpGERERpERyiIwjCI0iNM"
        "IjSIiIiIiIiI5BhERhERGkeCIiMIjTSIjCIiOORERERhERhHgwjSMIjSI0iI4xpEREaRhhGGkRhh"
        "EeSMIiI4xhERERhERhER4IiIwwjDCI0iOMR4MIiIiMIjwRERhGEaRHgiOMREREREYRHgiNIiIwjC"
        "IiIjSI454IjDCI8ERhEREaYYaRGEcciMIiIjSI0jTCIjCIiI0iIiI45EYRhhhERERGERhEaRERhE"
        "RhHIIiMIjSIiIiMIwiMIwjSIiMOQRhERERhERphGkREYRGGkRhyCIw0iMMIiIiIjCIwiIiIiIiMO"
        "SREaRGkREaRERGGGkRGkYcowjCIjwRpERHgjDDCNIjlERhEREeCPBERpERERERpERyiIiNIiMIjC"
        "IiI0wwjCPBEcgwiIjDCIwiNIiI0iIiIiI0iIjjHgwjSIwiIwwiIjCIjSIiMIiOURGGERhER4IiIj"
        "wRGERhERHGIiIiIiIiI0wjSI0iIjCIjDCI/T/8QAHxAAAQQDAQEBAQAAAAAAAAAAQQEwMVAQIEBg"
        "AHAh/9oACAEBAAE/EG5fJzHmn3WUS6pOt3k8/wB0eEN+keENj0fautt2xU0O1NLvljs7DsHUcz6/"
        "Y9usqgh5BdbbPtqlYwPveLbdPLYp6VZpVill2WzVePaa0qxwD7YtuFy6bbSzSrVzvT5J7pS8DfNt"
        "uk4FhHc8tPKojd+2k9TpN1eKVNLQloj2ib+nGbuc0/A33fXieVPlDTL7FJhFj+Bp/thtot23x7UP"
        "leOtDxitmy8Pbpcb59psOyqli+mpH+Ihvpl8inxzyr9CoR6q5xd51BBhFHBDtPhueONhytKsVaq2"
        "2bfxizSrD0u8+Gv0UYngTDlZpV2GemnmS2bMW08Nj2s0qtiDGgjfp5fS8gLPtODm1KqM9IWmjnn1"
        "eqSeUfS8HtctMo9IfirI/M9tJ7cKy8njy9GwheILMPNss2r0V3rttj/JKCvpTt9lO9vs27nl1vh9"
        "sx4lVdDCO85uLPi+WXHWfw0SGds3dt0vSqyqKHOnm3B3NtSiGB8qr5YfJTxZWlWKFfPbNBJpvrSr"
        "HBPMC67PLP8AWaVfi0KW8+Pjp08mx2lSq3/sehvOHk9vHhK5nu2nzqPnVLwItm26nie2y3zlT4a6"
        "bnto+DPuNn8ntu2vHGPjw+wp5+eJ+j4Qtpv2M3Psp/lyG2+J+v8Ayes2R63/2Q==";
    const char *metadataPreviewImageData =
        "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"
        "AAAAAAAAAQIGB//aAAwDAQACEAMQAAAB+l4fz4AAAASRdRq53UXUW53UasXWdsXUXWdsa1Fud1Fs"
        "XUXWerF1nbF1Fud1GrF1nbF1F1nbGtRbF1nbF1ndRbGtRbndRbF1NoAAAAAAAAAAAAHR/G+eAAAA"
        "Dwzuo1Yus7Yuous9WLqLc7qLqLc9ai2LrO2LqLc9ai2LrO6i2LrO2Nai3O6i6i3O6jVi6zti6i3O"
        "6jWotzuoti6m0AAAAAAAAAAAADo/jfPAAAAB4Z3UasXWdsXUXWdsa1Fud1FsXWd1GrF1Fud1Fud1"
        "GrF1ndRbF1nbGtRbF1nbF1F1nbGtRbndRbF1nrUWxdZ2xdTaAAAAAAAAAAAAB0fxvngAAAA8Ius7"
        "Y1qLc7qLYus7qNWLrO2LqLc9ai2LrO6i2LrPVhqNXO6i6i3PWoti6zti6i6ztjWotzuoti6z1qLY"
        "us7YuptAAAAAAAAAAAAA6P43zwAAAAeEXUW561FsXWdsXUW561Fud1F1Fud1GrF1Fud1F1FuetRb"
        "F1nbF1F1nqxdRbndRbF1nrUWxdZ2xdRbnrUXUW53U2gAAAAAAAAAAAAdH8b54AAAAPCLYus9ai2L"
        "rO2LqLc7qNWLrO6i2LqNXO6i3O6i2LqLrPVi6zti6i2LrPWotzuoti6z1qLYuotzuotz1qLqLYuo"
        "tAAAAAAAAAAAAA6P43zwAAAAeEWxdRbnrUXUW53UWxdZ6sXUXWdsXUaud1FsXWdsXUXWerF1Fud1"
        "FsXWetRbF1nbF1FuetRdRbndRbF1nqxdRdRaAAAAAAAAAAAAB0fxvngAAAA8Iti6i3PWoti6zuot"
        "i6i3PWoti6zuotz1qLYuotzuotzuo1qLYus7Yuotz1qLqLc7qLY1rO2LqLrO2LqNXO6i2LoAAAAA"
        "AAAAAAADo/jfPAAAAB4TbndRbF1nqxdRbndRdRbnrUWxdZ2xdRrWdsXUW53UWxdZ61FsXWdsXUWx"
        "rWd1Fud1FsXWerF1F1nbF1FuetRbF0AAAAAAAAAAAAB0fxvngAAAAEWxdZ2xdRq53UWxdZ3UWxdZ"
        "6sXUW53Ua1Fud1FsXWdsXUW561F1Fud1FsXWerF1Fud1F1GrndRbF1nbF1GtZ2xaUAAAAAAAAAAA"
        "AdH8b52UAAAARbFud1F1FuetRbF1nbF1F1nqxdRbndRbF1GtZ2xdRbndRbGtZ3UW53UWxdZ6sXUW"
        "xdZ3UWxrWdsXUXWdsXUaudpQAAAAAAAAAAAB0fxvngAAAAEWxbF1ndRbF1nqxdRbndRdRq53UWxd"
        "Z2xrUXWdsXUW53UWxdZ6sXUXWdsXUaud1FsXWdsXUa1nbF1Fud1Fsa1FvigAAAAAAAAAAADo/jfP"
        "CFAAAAi2LYus7Yuouo1c7qLYus7Yuo1c7qLqLc7qNWLrO2LqLrO2LrPVi6i2LrO6jVi6zti6i3O6"
        "i2Nazuoti6ztjWot8UAAAAAAAAAAAAdH8b54AAAABNudsXUW53UWxrWd1FsXUW53Ua1nbF1Fud1G"
        "rF1nbF1Fud1F1GrndRbF1Fud1GtRbndRbndRdRq53UWxdZ2xdRb5aAAAAAAAAAAAAOj+N88AAAAA"
        "lY1c7qLYus7Yuo1rO2LqLc7qLc9ai2LqLrO2Nai3O6i2LrO6jVi6zti6i3O6i6jVzuoti6ztjWot"
        "zuouotzuot8tAAAAAAAAAAAAHR/G+eAAAAASsaud1FsXUW53UW561F1Fud1Fsa1ndRbF1nbGtRbn"
        "dRbF1nbF1GtZ2xdRbF1nbGtRbndRdRbndRqxdZ2xdRdZ2+WgAAAAAAAAAAADo/jfPAAAAAJI1qLr"
        "O2LqLc7qNWLrO2LqLrO2Nai3O6i2LqLrPVi6zti6i3O6jWotzuoti6ztjWotzuouotzuo1Yus7Yu"
        "ous7fLQAAAAAAAAAAAB0fxvngAAAAEkXUasXWd1FsXWerF1Fud1F1FuetRbF1nbF1FuetRdRbndR"
        "bndRbGtRbndRdRbndRqxdZ2xdRdZ2xrUW53UWxdTaAAAAAAAAAAAAOj+N88AAAAA8M7qNWLrO6i2"
        "LrO2LqNXO6i2LrO6jVi6i3O6i3PWoti6zuoti6ztjWoti6zti6i6z1Yuotzuoti6z1qLYus7Yupt"
        "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"
        "rWdsXUXWdsXUaud1FsXWd1FvloAAAAAAAAAAA6P43zwAAAAACSNai6zti6i3O6jVi6zuotzuotjW"
        "otzuotzuo1qLYus7Yuotzuo1qLc7qLYus7Y1qLc7qLqLc7qNWLrO2LqLc7ry0AAAAAAAAAAAdH8b"
        "54AAAAABJF1GtZ2xdRbndRbGtZ2xdRdRbnrUW53UWxdRdZ6sXWdsXUW53Ua1Fud1FsXWdsXUaud1"
        "FsXWd1GrF1Fud1FsXU2gAAAAAAAAAAD/xAAUEAEAAAAAAAAAAAAAAAAAAACw/9oACAEBAAEFAnoP"
        "/8QAFBEBAAAAAAAAAAAAAAAAAAAAsP/aAAgBAwEBPwF6D//EABQRAQAAAAAAAAAAAAAAAAAAALD/"
        "2gAIAQIBAT8Beg//xAAUEAEAAAAAAAAAAAAAAAAAAACw/9oACAEBAAY/AnoP/8QAGBABAQEBAQAA"
        "AAAAAAAAAAAAAQBQMED/2gAIAQEAAT8h5MzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMz"
        "MzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOM"
        "zMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjszMzMzMzMzMzMzMzM"
        "zMzMzMzMzMzjszMzMzMzMzMzMzMzMzMzMzMzMzMzjszMzMzMzMzMzMzMzMzMzMzMzMzMzjszMzMz"
        "MzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMz"
        "MzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzM"
        "zMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzM"
        "zM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMz"
        "MzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOO"
        "zMzMzMzMzMzMzMzMzMzMzMzMzMzOOzMzMzMzMzMzMzMzMzMzMzMzMzMzOOzMzMzMzMzMzMzMzMzM"
        "zMzMzMzMzOOzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMz"
        "MzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMz"
        "MzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzM"
        "zMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzM"
        "zM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMz"
        "MzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjszMzMzMzMzMzMzMzMzMzMzMzMzMzjs"
        "zMzMzMzMzMzMzMzMzMzMzMzMzMzjszMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMzMzMzMzMzMzMzM"
        "zMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMzMzMzMzOMzMzMz"
        "MzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMzjMzMzMzMzMzMzMzMzMzMzMz"
        "MzMzMzOMzMzMzMzMzMzMzMzMzMzMzMzMzMzM4zMzMzMzMzMzMzMzMzMzMzMzMzMzMz6f/9oADAMB"
        "AAIAAwAAABAAAAAD42PUWLw7pQXPU3rwjvcHNgAAAAAAAAAAAAAAAAAB4jtWGJwjp0mdcHp6it+H"
        "dAAAAAAAAAAAAAAAAAAB6i5eGdah56mdeE9rg9uEdAAAAAAAAAAAAAAAAAABrwxvUfOVzq41OUXr"
        "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"
        "REREYRERGERxyIiIiIiIiIiIiIwiIiIiIjCIiIiOOREREREREREYREYRERERERhERERHHIiIiIiI"
        "iIiIiIjDCIiIiIiIiIiIjjERERERERERGERGGEREREREYREREcYiIiIiIiIiIiIiIiIiIwiIiIiI"
        "iIjjEREREREREREREREYYRhhERERERHHIiIjCIiIiIiIiIiIiIiMIiIiIiIjjERERhERERERGERE"
        "RERERERERERERxiIiIiIiIiIiIiIiIiIiIwiIiIiIiOMRERGERERhERERERERERERERERERHGMIi"
        "IiIiIiIiIiIiIiMIiIiIjCIjjkRhERhEREYREREREREREYRERERHHIjCIiIiIwwiIiIiIiIiIiIi"
        "IjCI4xEYRERERERhEREREREREREREREREcciIiIiIiIjCIiIiIiIiIiIwiIiIjjkRGERERERERER"
        "ERERERERERERERHHIiIiIiIiIiIiIjCIiIiIiMMIiIjjkRGERGEREREREYRERERERERERGHHIiIi"
        "IiIiIiMIiIiIiIwiIiIiIiIjjERERERERERERERERERGEREREREYRxiIiIiIwwiIiIiIiIiIiMIi"
        "MIiIiOMRERERERERERERERhhGGERERERERxiIiIwiIwiIiMIwiIiIjCIiIwiIjjGERGERERhERhE"
        "REREREREREREREcYwiIiIiMMIiIiIjCIiIiIiIiIiIjjGERGERERhERERERGEREREREREREcYwiI"
        "iIiIjCIjCIiIiIiIiIiIiIiOOYYREYRERGERERERERERERERERERxjDCIiIiIiMIiIiIiIiIiIiI"
        "iIiI45EREREREREYRERERERERERERGGERxiIiIiIiIiIiIiIiIiIiIiIiIiIiIjjkRERERERERER"
        "ERERERERGERERERHHIiIiIiIiIiIiIiIiIwiIwiMIwiOORERERERERERERGERERhGEREREYcciIw"
        "iIiIiIiIiIiIiIjCIjCIiIiI4xEREREYRERERERhEREYREYRERERHGIiIiIjCIiIiMIwwwjCIiIw"
        "iIiI4xEREREYRERGERGERERERERERERERxiIiIwiIwiIiIiIiIiIiIiIiIiIiI4xERERERERhGGE"
        "RERERERERERhERHGIiIiIiIwwiIwiIiMIiIiIiIwiIjjGERERERhhERhEREREREREREREREfT//E"
        "ABoRAQEBAQEBAQAAAAAAAAAAAAEAEFAgMED/2gAIAQIBAT8Q+RGGERGERpEeCIiIwjCIiIwiIjjE"
        "RhER4I0iPREYRpGkYYRHGI0iIwwiIwiI0iIwwiI0iMIiOMRhEREeCIwjDDCIwjCIjCIjCI45ERGE"
        "eCIwiI8kRppGkRhHIIiIjCPBGGEYaREaRGkRHLIiMIwjSI0jCIwiMI0wiIiI5BHgjCNIjCIiIwiI"
        "w0iIwiI5RGkREaRGkYRhGGkREeCOUYRhERHgiIjCIjSMIiIiNIiOQYRERhERhEaRERpEYYRERERh"
        "yyIwwjSIwwwjwYRGERpERyyIiIiIiIiIwwiPBERpHgiOQRhERpEaRGmERGmEYRERERERxiIiIjCI"
        "jwREaREYRhEREeSMOMYREYRhGkRGGkRhGHgiIiIjDjERGERhERGEYRpERGkRGkRhERxiIwiIjCI0"
        "iPBER4IiMIiNIiOMRGEREaRERERphEYYREYRERhEcYiNIw0iPBEYaRGERERhERERHGIjCMIjSIwi"
        "Iw0jwYRhERGERxiIjwRGEREYRGERER4IiIiIwwjjkaYeDCPBGEYRERhhERhEYcgiIjwRhERhEYRE"
        "RGkYRGkRyyIjSI0iIwiIjCI0jDDCIiMOORGGERERhER4I0jDSIjSIjlERhpEaRGmERpGGkREYRyy"
        "I00iIwjSIjwRhhpERERyiIwiMIjCIjwREaRhEREaREcoiMIwiNIjTCI0iIiIiIiIiOQYREYRERpH"
        "giIjCI00iIwiIjjkREREYREYR4MI0jCI0iNIiOMaRERGkYYRhpEYYRHkjCIiOMYREREYREYREeCI"
        "iMMIwwiNIjjEeDCIiIjCI8EREYRhGkR4IjjERERERGER4IjSIiMIwiIiI0iOOeCIwwiPBEYRERGm"
        "GGkRhHHIjCIiI0iNI0wiIwiIiNIiIiOORGEYYYRERERhEYRGkREYREYRyCIjCI0iIiIjCMIjCMI0"
        "iIjDkEYREREYREaYRpERGERhpEYcgiMNIjDCIiIiIwiMIiIiIiIjDkkRGkRpERGkRERhhpERpGHK"
        "MIwiI8EaRER4IwwwjSI5REYRERHgjwREaREREREaREcoiIjSIjCIwiIiNMMIwjwRHIMIiIwwiMIj"
        "SIiNIiIiIiNIiI4x4MI0iMIiMMIiIwiI0iIjCIjlERhhEYREeCIiI8ERhEYRERxiIiIiIiIiNMI0"
        "iNIiIwiIwwiP0//EAB8QAAEEAwEBAQEAAAAAAAAAAEEBMDFQECBAYABwIf/aAAgBAQABPxBuXycx"
        "5p91lEuqTrd5PP8AdHhDfpHhDY9H2rrbdsVNDtTS75Y7Ow7B1HM+v2PbrKoIeQXW2z7apWMD73i2"
        "3Ty2KelWaVYpZdls1Xj2mtKscA+2Lbhcum20s0q1c70+Se6UvA3zbbpOBYR3PLTyqI3ftpPU6TdX"
        "ilTS0JaI9om/pxm7nNPwN9314nlT5Q0y+xSYRY/gaf7YbaLdt8e1D5XjrQ8YrZsvD26XG+fabDsq"
        "pYvpqR/iIb6ZfIp8c8q/QqEequcXedQQYRRwQ7T4bnjjYcrSrFWqttm38Ys0qw9LvPhr9FGJ4Ew5"
        "WaVdhnpp5ktmzFtPDY9rNKrYgxoI36eX0vICz7Tg5tSqjPSFpo559XqknlH0vB7XLTKPSH4qyPzP"
        "bSe3CsvJ48vRsIXiCzDzbLNq9Fd67bY/ySgr6U7fZTvb7Nu55db4fbMeJVXQwjvObiz4vllx1n8N"
        "EhnbN3bdL0qsqihzp5twdzbUohgfKq+WHyU8WVpVihXz2zQSab60qxwTzAuuzyz/AFmlX4tClvPj"
        "46dPJsdpUqt/7Hobzh5Pbx4SuZ7tp86j51S8CLZtup4ntst85U+Gum57aPgz7jZ/J7btrxxj48Ps"
        "Kefnifo+ELab9jNz7Kf5chtvifr/AMnrNket/9k=";
};

#endif // DETAILSTHUMBNAILTEST_HPP
