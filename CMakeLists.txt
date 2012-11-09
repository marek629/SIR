# SIR (Simple Image Resizer) build tips:
# cmake -DCMAKE_INSTALL_PREFIX=../sir-build/usr
#       you can add: -DCMAKE_BUILD_TYPE=debug (release is default)
# make -j<number_of_cores+1>
# make install

project( sir )
cmake_minimum_required( VERSION 2.8.3 )

# SIR_CMAKE preprocessor define added for QtCreator code parsing and highlighting
add_definitions( -Wall -DSIR_CMAKE )

find_package( Qt4 REQUIRED QtCore QtGui QtNetwork QtSvg )
include( ${QT_USE_FILE} )

include_directories(
        ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTSVG_INCLUDE_DIR}
        src/
    )
if(WIN32)
        include_directories ( ${include_directories} windows/include )
endif()

# Headers
set( sir_HDRS
        src/main.h
        src/defines.h
        src/languageutils.h
        src/sharedinformation.h
        src/rawutils.h
        src/expressiontree.h
        src/optionsenums.h
        src/metadata/string.h
    )

# Source files
set( sir_SRCS
        src/main.cpp
        src/convertdialog.cpp
        src/widgets/previewdialog.cpp
        src/widgets/optionsdialog.cpp
        src/widgets/treewidget.cpp
        src/widgets/treewidgetheader.cpp
        src/convertthread.cpp
        src/widgets/aboutdialog.cpp
        src/languageutils.cpp
        src/sharedinformation.cpp
        src/rawutils.cpp
        src/networkutils.cpp
        src/widgets/messagebox.cpp
        src/widgets/historycombobox.cpp
        src/widgets/timecombobox.cpp
        src/selection.cpp
        src/expressiontree.cpp
        src/settings.cpp
        src/widgets/detailsbrowser.cpp
        src/widgets/selectiondialog.cpp
        src/widgets/options/abstractoptions.cpp
        src/widgets/options/generalgroupbox.cpp
        src/widgets/options/filelistgroupbox.cpp
        src/widgets/options/selectiongroupbox.cpp
        src/widgets/options/rawgroupbox.cpp
        src/metadata/string.cpp
    )

# User interface files
set( sir_UIS
        dialogs/convertdialog.ui
        dialogs/about.ui
        dialogs/previewdialog.ui
        dialogs/selection/dirwidget.ui
        dialogs/options/generalgroupbox.ui
        dialogs/options/filelistgroupbox.ui
        dialogs/options/selectiongroupbox.ui
        dialogs/options/rawgroupbox.ui
    )

set( sir_RSCS
        application.qrc
    )
QT4_ADD_RESOURCES(RSCS ${sir_RSCS})

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
QT4_ADD_TRANSLATION( QMS ${sir_TRANSLATIONS} )

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
        src/widgets/previewdialog.h
        src/widgets/optionsdialog.h
        src/widgets/treewidget.h
        src/widgets/treewidgetheader.h
        src/convertthread.h
        src/widgets/aboutdialog.h
        src/networkutils.h
        src/widgets/messagebox.h
        src/widgets/historycombobox.h
        src/widgets/timecombobox.h
        src/selection.h
        src/settings.h
        src/widgets/detailsbrowser.h
        src/widgets/selectiondialog.h
        src/widgets/options/generalgroupbox.h
        src/widgets/options/filelistgroupbox.h
        src/widgets/options/selectiongroupbox.h
        src/widgets/options/rawgroupbox.h
        src/widgets/options/abstractoptions.h
    )

set( sir_LINKING_LIBS
        ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
        ${QT_QTNETWORK_LIBRARY} ${QT_QTSVG_LIBRARY}
    )

option( metadata "Metadata support using exiv2 library." ON)
if( metadata )
    set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR} )
    find_package( Exiv2 )
    if( EXISTS ${EXIV2_LIBRARY} )
        message( STATUS "exiv2 found" )
        add_definitions( -DSIR_METADATA_SUPPORT )
        set( sir_MOCS ${sir_MOCS}
                src/widgets/metadatadialog.h
                src/widgets/selection/anymetadatagroupbox.h
                src/widgets/options/detailsgroupbox.h
                src/widgets/options/metadatagroupbox.h
            )
        set( sir_HDRS ${sir_HDRS}
                src/metadatautils.h
                src/metadata/exif.h
                src/metadata/error.h
                src/metadata/iptc.h
            )
        set( sir_SRCS ${sir_SRCS}
                src/metadatautils.cpp
                src/metadata/exif.cpp
                src/widgets/metadatadialog.cpp
                src/metadata/error.cpp
                src/metadata/iptc.cpp
                src/widgets/options/detailsgroupbox.cpp
                src/widgets/options/metadatagroupbox.cpp
                src/widgets/selection/anymetadatagroupbox.cpp
            )
        set( sir_UIS ${sir_UIS}
                dialogs/metadatadialog.ui
                dialogs/selection/anymetadatagroupbox.ui
                dialogs/selection/exifgroupbox.ui
                dialogs/selection/iptcgroupbox.ui
                dialogs/options/metadatagroupbox.ui
                dialogs/options/detailsgroupbox.ui
            )
        set( sir_LINKING_LIBS ${sir_LINKING_LIBS} ${EXIV2_LIBRARY} )
    else( EXISTS ${EXIV2_LIBRARY} )
        message ( FATAL_ERROR "eviv2 library not found! Disable metadata support calling -Dmetadata=OFF in cmake command." )
    endif( EXISTS ${EXIV2_LIBRARY} )
endif( metadata )

QT4_WRAP_UI( UIS ${sir_UIS} )
QT4_WRAP_CPP( MOCS ${sir_MOCS} )

add_executable( sir ${sir_SRCS} ${UIS} ${MOCS} ${QMS} ${RSCS} )
target_link_libraries( sir ${sir_LINKING_LIBS} )

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

