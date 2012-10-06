#include "filelistgroupbox.h"
#include "settings.h"
#include "optionsenums.h"

using namespace TreeWidgetOptions;

/** Default constructor.\n
  * Sets UI.
  */
FileListGroupBox::FileListGroupBox(QWidget *parent) : AbstractOptionsGroupBox(parent) {
    setupUi(this);
}

/** Load settings and sets member widgets values.
  * \sa Settings saveSettings()
  */
void FileListGroupBox::loadSettings() {
    int hex = Settings::instance().treeWidget.columns;
    nameCheckBox->setChecked (hex & NameColumn);
    extCheckBox->setChecked  (hex & ExtColumn);
    pathCheckBox->setChecked (hex & PathColumn);
    imageSizeCheckBox->setChecked(hex & ImageSizeColumn);
    fileSizeCheckBox->setChecked (hex & FileSizeColumn);
    statusCheckBox->setChecked   (hex & StatusColumn);
}

/** Saves settings basing member widgets values.
  * \sa Settings loadSettings()
  */
void FileListGroupBox::saveSettings() {
    int hex = 0;
    if (nameCheckBox->isChecked())
        hex |= NameColumn;
    if (extCheckBox->isChecked())
        hex |= ExtColumn;
    if (pathCheckBox->isChecked())
        hex |= PathColumn;
    if (imageSizeCheckBox->isChecked())
        hex |= ImageSizeColumn;
    if (fileSizeCheckBox->isChecked())
        hex |= FileSizeColumn;
    if (statusCheckBox->isChecked())
        hex |= StatusColumn;
    Settings::instance().treeWidget.columns = hex;
}

/** Returns true if at least 1 column check box is checked, otherwise returns
  * false.
  */
bool FileListGroupBox::isColumnChecked() {
    QList<QCheckBox*> checkBoxList = columnsGroupBox->findChildren<QCheckBox*>();
    foreach (QCheckBox *checkBox, checkBoxList) {
        if (checkBox->isChecked())
            return true;
    }
    return false;
}
