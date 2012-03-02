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
    src/previewdialog.cpp \
    src/optionsdialog.cpp \
    src/myqtreewidget.cpp \
    src/convertthread.cpp \
    src/aboutdialog.cpp \
    src/languageutils.cpp \
    src/sharedinformation.cpp \
    src/rawutils.cpp \
    src/networkutils.cpp \
    src/messagebox.cpp \
    src/metadatautils.cpp \
    src/historycombobox.cpp \
    src/metadata/exif.cpp \
    src/metadatadialog.cpp \
    src/timecombobox.cpp \
    src/metadata/string.cpp \
    src/metadata/error.cpp
FORMS += dialogs/convertdialog.ui \
    dialogs/about.ui \
    dialogs/optionsdialog.ui \
    dialogs/previewdialog.ui \
    dialogs/metadatadialog.ui
HEADERS += src/main.h \
    src/convertdialog.h \
    src/previewdialog.h \
    src/optionsdialog.h \
    src/myqtreewidget.h \
    src/convertthread.h \
    src/aboutdialog.h \
    src/defines.h \
    src/languageutils.h \
    src/sharedinformation.h \
    src/rawutils.h \
    src/networkutils.h \
    src/messagebox.h \
    src/metadatautils.h \
    src/historycombobox.h \
    src/metadata/exif.h \
    src/metadatadialog.h \
    src/timecombobox.h \
    src/metadata/string.h \
    src/metadata/error.h
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
    translations/sir_cz.ts
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
    README.txt
