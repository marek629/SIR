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
        set( sir_LINKING_LIBS
                ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY}
                ${QT_QTSVG_LIBRARY} ${QT_QTXML_LIBRARY}
            )
        set( sir_UT_LINKING_LIBS
                ${sir_LINKING_LIBS} ${QT_QTTEST_LIBRARY}
            )
endif( qt5 )

if( WIN32 )
        include_directories ( ${include_directories}
                windows/include
            )
endif( WIN32 )

function( check_q_object_headers )
    foreach( HEADER ${ARGN} )
#        message( STATUS "header: " ${HEADER} )
        file( STRINGS ${HEADER} HEADER_CONTENT )
        foreach( LINE ${HEADER_CONTENT} )
            string( COMPARE EQUAL ${LINE} "    Q_OBJECT" LINE_PASSED )
            if( ${LINE_PASSED} )
#                message( STATUS "YES" )
                set( MOCS ${MOCS} ${HEADER} )
                break( )
            endif( ${LINE_PASSED} )
        endforeach( LINE )
        if( NOT ${LINE_PASSED} )
            set( HDRS ${HDRS} ${HEADER} )
        endif( NOT ${LINE_PASSED} )
    endforeach( HEADER )
#    message( STATUS "hdrs: " ${HDRS} )
#    message( STATUS "mocs: " ${MOCS} )
    set( NO_Q_OBJECT_HEADERS ${HDRS} PARENT_SCOPE )
    set( Q_OBJECT_HEADERS ${MOCS} PARENT_SCOPE )
endfunction( check_q_object_headers )


file( GLOB HEADERS RELATIVE ${CMAKE_CURRENT_LIST_DIR} src/*.h )
check_q_object_headers( ${HEADERS} )
#message( STATUS "no Q_OBJECT headers: " ${NO_Q_OBJECT_HEADERS} )
#message( STATUS "Q_OBJECT headers: " ${Q_OBJECT_HEADERS} )

# ################################  SIR  exec  #################################
# Headers
set( sir_HDRS ${NO_Q_OBJECT_HEADERS}
        src/widgets/options/abstractoptions.h
        src/widgets/options/abstractoptionsgroupbox.h
        src/widgets/options/commonoptions.h
    )

file( GLOB SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} src/*.cpp )

message( STATUS "sources: " ${SOURCES} )

# Source files
set( sir_SRCS ${SOURCES}
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
        src/widgets/options/abstractoptionsgroupbox.cpp
        src/widgets/options/commonoptions.cpp
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
set( sir_MOCS ${Q_OBJECT_HEADERS}
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
        src/widgets/options/filelistgroupbox.h
        src/widgets/options/generalgroupbox.h
        src/widgets/options/rawgroupbox.h
        src/widgets/options/selectiongroupbox.h
    )

option( metadata "Metadata support using exiv2 library." ON)
if( metadata )
    set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake )
    find_package( Exiv2 )
    if( EXISTS ${EXIV2_LIBRARY} )
        message( STATUS "exiv2 found" )
        add_definitions( -DSIR_METADATA_SUPPORT )
        set( sir_MOCS ${sir_MOCS}
                ${METADATA_MOCS}
                src/widgets/metadatadialog.h
                src/widgets/options/detailsgroupbox.h
                src/widgets/options/metadatagroupbox.h
                src/widgets/selection/anymetadatagroupbox.h
            )
        set( sir_HDRS ${sir_HDRS}
                ${METADATA_HEADERS}
                src/metadata/metadata.h
                src/metadata/metadatautils.h
                src/metadata/error.h
                src/metadata/exif.h
                src/metadata/iptc.h
            )
        set( sir_SRCS ${sir_SRCS}
                src/metadata/metadata.cpp
                src/metadata/metadatautils.cpp
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
        set( sir_UT_LINKING_LIBS ${sir_UT_LINKING_LIBS} ${EXIV2_LIBRARY} )
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


# enable moc generation for tests
set(CMAKE_AUTOMOC ON)
add_definitions( -DSIR_TESTS )

# ############################  MetadataUtils  UT  #############################
set( sir_UT_metadatautils_HDRS
        src/sir_string.h
        src/metadata/metadatautils.h
        src/metadata/ut/metadatautilstest.h
    )

set( sir_UT_metadatautils_SRCS
        src/sir_string.cpp
        src/metadata/metadatautils.cpp
        src/metadata/ut/metadatautilstest.cpp
    )

if( qt5 )
        QT5_WRAP_CPP( UT_MOCS ${sir_UT_metadatautils_HDRS} )
else( qt5 )
        QT4_AUTOMOC( UT_MOCS ${sir_UT_metadatautils_SRCS} )
endif( qt5 )

add_executable( sir_metadatautils_test ${sir_UT_metadatautils_SRCS} ${UT_MOCS} )
target_link_libraries( sir_metadatautils_test ${sir_UT_LINKING_LIBS} )
if( qt5 )
        QT5_USE_MODULES( sir_metadatautils_test Core Test )
endif( qt5 )


# ###########################  ConvertEffects  UT  #############################
set( sir_UT_converteffects_HDRS
        src/converteffects.h
        src/languageutils.h
        src/rgb.h
        src/settings.h
        src/sir_string.h
        src/sharedinformation.h
        src/metadata/iptc.h
        src/metadata/error.h
        src/metadata/exif.h
        src/metadata/metadata.h
        src/metadata/metadatautils.h
        src/ut/converteffectstest.h
    )

set( sir_UT_converteffects_SRCS
        src/converteffects.cpp
        src/languageutils.cpp
        src/rgb.cpp
        src/settings.cpp
        src/sir_string.cpp
        src/sharedinformation.cpp
        src/metadata/iptc.cpp
        src/metadata/error.cpp
        src/metadata/exif.cpp
        src/metadata/metadata.cpp
        src/metadata/metadatautils.cpp
        src/ut/converteffectstest.cpp
    )

if( qt5 )
        QT5_WRAP_CPP( UT_MOCS ${sir_UT_converteffects_HDRS} )
else( qt5 )
        QT4_AUTOMOC( UT_MOCS ${sir_UT_converteffects_SRCS} )
endif( qt5 )

add_executable( sir_converteffects_test ${sir_UT_converteffects_SRCS} ${UT_MOCS} )
target_link_libraries( sir_converteffects_test ${sir_UT_LINKING_LIBS} )
if( qt5 )
        QT5_USE_MODULES( sir_converteffects_test Core Test )
endif( qt5 )


# ###############################  Version  UT  ################################
set( sir_UT_version_HDRS
        src/version.h
        src/ut/versiontest.h
    )

set( sir_UT_version_SRCS
        src/version.cpp
        src/ut/versiontest.cpp
    )

if( qt5 )
        QT5_WRAP_CPP( UT_MOCS ${sir_UT_version_HDRS} )
else( qt5 )
        QT4_AUTOMOC( UT_MOCS ${sir_UT_version_SRCS} )
endif( qt5 )

add_executable( sir_version_test ${sir_UT_version_SRCS} ${UT_MOCS} )
target_link_libraries( sir_version_test ${sir_UT_LINKING_LIBS} )
if( qt5 )
        QT5_USE_MODULES( sir_version_test Core Test )
endif( qt5 )


# ###############################  Installation  ###############################
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

