#ifndef METADATADIALOG_H
#define METADATADIALOG_H

#include "ui_metadatadialog.h"

namespace MetadataUtils
{
    class Metadata;
    struct ExifStruct;
}
class QStringList;
class QString;


class MetadataDialog : public QDialog, public Ui::MetadataDialog
{
    Q_OBJECT
public:
    explicit MetadataDialog(QWidget *parent = 0, QStringList *images = 0,
                            int currentImage = 0);
    ~MetadataDialog();

private:
    void setupValues();
    MetadataUtils::Metadata *metadata;
    MetadataUtils::ExifStruct *exifStruct;
    QStringList *images;
    QString imagePath;
    int currentImage;

signals:

private slots:
    void saveChanges();

};

#endif // METADATADIALOG_H
