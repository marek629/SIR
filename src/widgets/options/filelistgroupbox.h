#ifndef FILELISTGROUPBOX_H
#define FILELISTGROUPBOX_H

#include "ui_filelistgroupbox.h"
#include "widgets/options/abstractoptions.h"

//! File list group box class used in OptionsDialog dialog.
class FileListGroupBox : public AbstractOptionsGroupBox, private Ui::FileListGroupBox {
    Q_OBJECT

public:
    explicit FileListGroupBox(QWidget *parent = 0);
    void loadSettings();
    void saveSettings();
    bool isColumnChecked();
};

#endif // FILELISTGROUPBOX_H
