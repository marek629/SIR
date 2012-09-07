TEMPLATE = app
LANGUAGE = C++
RESOURCES += application.qrc
QT += network svg

UI_DIR += .ui
MOC_DIR += .moc
OBJECTS_DIR += .obj

CONFIG += qt \
    release_and_debug \
    warn_on
INCLUDEPATH += src/
LIBS += -lexiv2
SOURCES += src/main.cpp \
    src/convertdialog.cpp \
    src/widgets/previewdialog.cpp \
    src/widgets/optionsdialog.cpp \
    src/widgets/myqtreewidget.cpp \
    src/convertthread.cpp \
    src/widgets/aboutdialog.cpp \
    src/languageutils.cpp \
    src/sharedinformation.cpp \
    src/rawutils.cpp \
    src/networkutils.cpp \
    src/widgets/messagebox.cpp \
    src/metadatautils.cpp \
    src/widgets/historycombobox.cpp \
    src/metadata/exif.cpp \
    src/widgets/metadatadialog.cpp \
    src/widgets/timecombobox.cpp \
    src/metadata/string.cpp \
    src/metadata/error.cpp \
    src/metadata/iptc.cpp \
    src/selection.cpp \
    src/expressiontree.cpp \
    src/settings.cpp \
    src/widgets/detailsbrowser.cpp \
    src/widgets/selectiondialog.cpp \
    src/widgets/selection/anymetadatagroupbox.cpp \
    src/widgets/options/rawgroupbox.cpp \
    src/widgets/options/abstractoptions.cpp \
    src/widgets/options/selectiongroupbox.cpp \
    src/widgets/options/detailsgroupbox.cpp \
    src/widgets/options/metadatagroupbox.cpp \
    src/widgets/options/generalgroupbox.cpp
FORMS += dialogs/convertdialog.ui \
    dialogs/about.ui \
    dialogs/previewdialog.ui \
    dialogs/metadatadialog.ui \
    dialogs/selection/anymetadatagroupbox.ui \
    dialogs/selection/dirwidget.ui \
    dialogs/selection/exifgroupbox.ui \
    dialogs/selection/iptcgroupbox.ui \
    dialogs/options/rawgroupbox.ui \
    dialogs/options/selectiongroupbox.ui \
    dialogs/options/detailsgroupbox.ui \
    dialogs/options/metadatagroupbox.ui \
    dialogs/options/generalgroupbox.ui
HEADERS += src/main.h \
    src/convertdialog.h \
    src/widgets/previewdialog.h \
    src/widgets/optionsdialog.h \
    src/widgets/myqtreewidget.h \
    src/convertthread.h \
    src/widgets/aboutdialog.h \
    src/defines.h \
    src/languageutils.h \
    src/sharedinformation.h \
    src/rawutils.h \
    src/networkutils.h \
    src/widgets/messagebox.h \
    src/metadatautils.h \
    src/widgets/historycombobox.h \
    src/metadata/exif.h \
    src/widgets/metadatadialog.h \
    src/widgets/timecombobox.h \
    src/metadata/string.h \
    src/metadata/error.h \
    src/metadata/iptc.h \
    src/selection.h \
    src/expressiontree.h \
    src/settings.h \
    src/widgets/detailsbrowser.h \
    src/detailsoptions.h \
    src/widgets/selectiondialog.h \
    src/widgets/selection/anymetadatagroupbox.h \
    src/widgets/options/rawgroupbox.h \
    src/widgets/options/abstractoptions.h \
    src/widgets/options/selectiongroupbox.h \
    src/widgets/options/detailsgroupbox.h \
    src/widgets/options/metadatagroupbox.h \
    src/widgets/options/generalgroupbox.h
TRANSLATIONS += translations/sir_pt_BR.ts \
    translations/sir_de.ts \
    translations/sir_es.ts \
    translations/sir_sk.ts \
    translations/sir_ru_RU.ts \
    translations/sir_hu_HU.ts \
    translations/sir_pl.ts \
    translations/sir_nl.ts \
    translations/sir_ro_RO.ts \
    translations/sir_fr.ts \
    translations/sir_gr.ts \
    translations/sir_cz.ts \
    translations/sir_sr.ts
unix {
    target.path += /usr/bin
    images.path = /usr/share/pixmaps
    images.files = images/sir*
    service.path = /usr/share/kde4/services/ServiceMenus
    service.files = sir_service.desktop
    desktop.path = /usr/share/applications
    desktop.files = sir.desktop
    INSTALLS += target
    INSTALLS += images
    INSTALLS += desktop
    INSTALLS += service
    DEFINES += SIR_METADATA_SUPPORT
}
win32 {
    RC_FILE = windows/res.rc
    INCLUDEPATH += windows/include
    LIBS += -L"i:\sir\windows\include"
}

OTHER_FILES += \
    TODO \
    LICENSE \
    INSTALL \
    README.txt \
    CHANGELOG.txt
