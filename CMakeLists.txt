# SIR (Simple Image Resizer) build tips:
# cmake -DCMAKE_INSTALL_PREFIX=../sir-build/usr
#       you can add: -DCMAKE_BUILD_TYPE=debug (release is default)
# make -j<number_of_cores+1>
# make install

project( sir )

# SIR_CMAKE preprocessor define added for code parsing and highlighting QtCreator editor
add_definitions( -Wall -DSIR_CMAKE )

include_directories(
        ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        src/
    )

option( qt5 "Qt 5 support" ON)

if( qt5 )
        message( STATUS "Building using Qt 5" )
        message ( STATUS "To force build with Qt 4 type -Dqt5=OFF in cmake command." )
        cmake_minimum_required( VERSION 2.8.8 )
        find_package( Qt5Core REQUIRED )
        find_package( Qt5Widgets REQUIRED )
        find_package( Qt5Network REQUIRED )
        find_package( Qt5Svg REQUIRED )
        find_package( Qt5Xml REQUIRED )
        find_package( Qt5PrintSupport REQUIRED )
        find_package( Qt5LinguistTools REQUIRED )
else( qt5 )
        message( STATUS "Building using Qt 4" )
        cmake_minimum_required( VERSION 2.8.3 )
        find_package( Qt4 REQUIRED QtCore QtGui QtNetwork QtSvg QtXml )
        include( ${QT_USE_FILE} )
        include_directories( ${include_directories}
                ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR}
                ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTSVG_INCLUDE_DIR}
            )
endif( qt5 )

if( WIN32 )
        include_directories ( ${include_directories}
                windows/include
            )
endif( WIN32 )

# Headers
set( sir_HDRS
        src/commandlineassistant.h
        src/converteffects.h
        src/convertshareddata.h
        src/defines.h
        src/expressiontree.h
        src/languageutils.h
        src/main.h
        src/optionsenums.h
        src/rawutils.h
        src/regexputils.h
        src/session.h
        src/sharedinformation.h
        src/svgmodifier.h
        src/xmlstreamwriter.h
        src/metadata/string.h
    )

# Source files
set( sir_SRCS
        src/commandlineassistant.cpp
        src/convertdialog.cpp
        src/converteffects.cpp
        src/convertshareddata.cpp
        src/convertthread.cpp
        src/expressiontree.cpp
        src/languageutils.cpp
        src/main.cpp
        src/networkutils.cpp
        src/rawutils.cpp
        src/regexputils.cpp
        src/selection.cpp
        src/session.cpp
        src/settings.cpp
        src/sharedinformation.cpp
        src/svgmodifier.cpp
        src/xmlstreamwriter.cpp
        src/metadata/string.cpp
        src/widgets/aboutdialog.cpp
        src/widgets/brushframe.cpp
        src/widgets/colorframe.cpp
        src/widgets/detailsbrowser.cpp
        src/widgets/gradienteditwidget.cpp
        src/widgets/historycombobox.cpp
        src/widgets/messagebox.cpp
        src/widgets/optionsdialog.cpp
        src/widgets/posspinbox.cpp
        src/widgets/previewdialog.cpp
        src/widgets/selectiondialog.cpp
        src/widgets/timecombobox.cpp
        src/widgets/treewidget.cpp
        src/widgets/treewidgetheader.cpp
        src/widgets/convert/effectsscrollarea.cpp
        src/widgets/convert/optionsscrollarea.cpp
        src/widgets/convert/sizescrollarea.cpp
        src/widgets/convert/svgscrollarea.cpp
        src/widgets/options/abstractoptions.cpp
        src/widgets/options/filelistgroupbox.cpp
        src/widgets/options/generalgroupbox.cpp
        src/widgets/options/rawgroupbox.cpp
        src/widgets/options/selectiongroupbox.cpp
    )

# User interface files
set( sir_UIS
        dialogs/about.ui
        dialogs/convertdialog.ui
        dialogs/gradienteditwidget.ui
        dialogs/previewdialog.ui
        dialogs/convert/effectsscrollarea.ui
        dialogs/convert/optionsscrollarea.ui
        dialogs/convert/sizescrollarea.ui
        dialogs/convert/svgscrollarea.ui
        dialogs/options/filelistgroupbox.ui
        dialogs/options/generalgroupbox.ui
        dialogs/options/rawgroupbox.ui
        dialogs/options/selectiongroupbox.ui
        dialogs/selection/dirwidget.ui
    )

set( sir_RSCS
        application.qrc
    )
if( qt5 )
        QT5_ADD_RESOURCES(RSCS ${sir_RSCS})
else( qt5 )
        QT4_ADD_RESOURCES(RSCS ${sir_RSCS})
endif( qt5 )

# Translations
set( sir_TRANSLATIONS
        translations/sir_cz.ts
        translations/sir_de.ts
        translations/sir_en.ts
        translations/sir_es.ts
        translations/sir_fr.ts
        translations/sir_gr.ts
        translations/sir_hu_HU.ts
        translations/sir_nl.ts
        translations/sir_pl.ts
        translations/sir_pt_BR.ts
        translations/sir_ro_RO.ts
        translations/sir_ru_RU.ts
        translations/sir_sk.ts
        translations/sir_sr.ts
    )
if( qt5 )
        QT5_ADD_TRANSLATION( QMS ${sir_TRANSLATIONS} )
else( qt5 )
        QT4_ADD_TRANSLATION( QMS ${sir_TRANSLATIONS} )
endif( qt5 )

# Images
set( sir_IMGS
        images/favorite.png
        images/unfavorite.png
        images/czech.png
        images/dutch.png
        images/english.png
        images/french.png
        images/german.png
        images/greek.png
        images/hungarian.png
        images/polish.png
        images/portuguese.png
        images/romanian.png
        images/russian.png
        images/serbian.png
        images/slovak.png
        images/spanish.png
    )

# Meta-object code files
set( sir_MOCS
        src/convertdialog.h
        src/convertthread.h
        src/networkutils.h
        src/selection.h
        src/settings.h
        src/widgets/aboutdialog.h
        src/widgets/brushframe.h
        src/widgets/colorframe.h
        src/widgets/detailsbrowser.h
        src/widgets/gradienteditwidget.h
        src/widgets/historycombobox.h
        src/widgets/messagebox.h
        src/widgets/optionsdialog.h
        src/widgets/posspinbox.h
        src/widgets/previewdialog.h
        src/widgets/selectiondialog.h
        src/widgets/timecombobox.h
        src/widgets/treewidget.h
        src/widgets/treewidgetheader.h
        src/widgets/convert/effectsscrollarea.h
        src/widgets/convert/optionsscrollarea.h
        src/widgets/convert/sizescrollarea.h
        src/widgets/convert/svgscrollarea.h
        src/widgets/options/abstractoptions.h
        src/widgets/options/filelistgroupbox.h
        src/widgets/options/generalgroupbox.h
        src/widgets/options/rawgroupbox.h
        src/widgets/options/selectiongroupbox.h
    )

if( NOT qt5 )
        set( sir_LINKING_LIBS
                ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY}
                ${QT_QTSVG_LIBRARY} ${QT_QTXML_LIBRARY}
            )
endif( NOT qt5 )


option( metadata "Metadata support using exiv2 library." ON)
if( metadata )
    set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake )
    find_package( Exiv2 )
    if( EXISTS ${EXIV2_LIBRARY} )
        message( STATUS "exiv2 found" )
        add_definitions( -DSIR_METADATA_SUPPORT )
        set( sir_MOCS ${sir_MOCS}
                src/widgets/metadatadialog.h
                src/widgets/options/detailsgroupbox.h
                src/widgets/options/metadatagroupbox.h
                src/widgets/selection/anymetadatagroupbox.h
            )
        set( sir_HDRS ${sir_HDRS}
                src/metadatautils.h
                src/metadata/error.h
                src/metadata/exif.h
                src/metadata/iptc.h
            )
        set( sir_SRCS ${sir_SRCS}
                src/metadatautils.cpp
                src/metadata/exif.cpp
                src/metadata/error.cpp
                src/metadata/iptc.cpp
                src/widgets/metadatadialog.cpp
                src/widgets/selection/anymetadatagroupbox.cpp
                src/widgets/options/detailsgroupbox.cpp
                src/widgets/options/metadatagroupbox.cpp
            )
        set( sir_UIS ${sir_UIS}
                dialogs/metadatadialog.ui
                dialogs/options/detailsgroupbox.ui
                dialogs/options/metadatagroupbox.ui
                dialogs/selection/anymetadatagroupbox.ui
                dialogs/selection/exifgroupbox.ui
                dialogs/selection/iptcgroupbox.ui
            )
        set( sir_LINKING_LIBS ${sir_LINKING_LIBS} ${EXIV2_LIBRARY} )
    else( EXISTS ${EXIV2_LIBRARY} )
        message ( FATAL_ERROR "eviv2 library not found! Disable metadata support calling -Dmetadata=OFF in cmake command." )
    endif( EXISTS ${EXIV2_LIBRARY} )
endif( metadata )

if( qt5 )
        QT5_WRAP_UI( UIS ${sir_UIS} )
        QT5_WRAP_CPP( MOCS ${sir_MOCS} )
else( qt5 )
        QT4_WRAP_UI( UIS ${sir_UIS} )
        QT4_WRAP_CPP( MOCS ${sir_MOCS} )
endif( qt5 )

add_executable( sir ${sir_SRCS} ${UIS} ${MOCS} ${QMS} ${RSCS} )
target_link_libraries( sir ${sir_LINKING_LIBS} )
if( qt5 )
        QT5_USE_MODULES( sir Core Widgets Network Svg Xml PrintSupport )
endif( qt5 )

# installation
# bin
install( TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin )
# translations
install( FILES ${QMS} translations/translation_info.csv DESTINATION share/sir/translations )
# icon and desktop file
if( WIN32 )
    install( FILES images/sir.ico DESTINATION share/pixmaps )
else( UNIX )
    install( FILES images/sir.png DESTINATION share/pixmaps )
    install( FILES sir.desktop DESTINATION share/applications )
endif( )
# images
install( FILES ${sir_IMGS} DESTINATION share/sir/images )

