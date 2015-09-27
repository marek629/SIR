# SIR (Simple Image Resizer) build tips:
# cmake -DCMAKE_INSTALL_PREFIX=../sir-build/usr
#       you can add: -DCMAKE_BUILD_TYPE=debug (release is default)
# make -j<number_of_cores+1>
# make install

project( sir )

# SIR_CMAKE preprocessor define added for code parsing and highlighting QtCreator editor
add_definitions( -Wall -DSIR_CMAKE -std=c++11 )

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        src/
    )

# 'make ico' command
add_custom_target( ico
        COMMAND convert sir-128x128.png sir-64x64.png sir.png sir-32x32.png sir-16x16.png sir.ico
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/images
    )
# 'make archive' command
add_custom_target( archive
        COMMAND git archive --format=tar.gz --prefix=sir/ -o ${CMAKE_BINARY_DIR}/sir-src.tar.gz HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
# 'make upload' command
add_custom_target( upload
        COMMAND python upload.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/web
    )


if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE "release" CACHE STRING "" FORCE )
endif( )
string( TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE )
if( CMAKE_BUILD_TYPE MATCHES "^debug$" )
    set( SIR_UNDER_TESTS TRUE )
    add_definitions( -DSIR_TESTS )

    # 'make coverage' command
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage" )
    set( CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage" )
    set( sir_LINKING_LIBS gcov )
    add_custom_target( coverage
        COMMAND lcov --capture --rc lcov_branch_coverage=1 --directory ./ --output-file coverage.info
        COMMAND genhtml coverage.info --rc lcov_branch_coverage=1 --output-directory coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    message( STATUS "SIR is under tests!" )
else( )
    set( SIR_UNDER_TESTS FALSE )
    message( STATUS "SIR is building in release mode." )
endif( )

option( qt5 "Qt 5 support." ON )
option( metadata "Metadata support using exiv2 library." ON )
option( testingDcrawExe "Testing dcraw executable file permissions." ON )

if( qt5 )
        message( STATUS "Building using Qt 5" )
        message( STATUS "To force build with Qt 4 type -Dqt5=OFF in cmake command." )
        cmake_minimum_required( VERSION 2.8.8 )
        find_package( Qt5Core REQUIRED )
        find_package( Qt5Widgets REQUIRED )
        find_package( Qt5Network REQUIRED )
        find_package( Qt5Svg REQUIRED )
        find_package( Qt5Xml REQUIRED )
        find_package( Qt5PrintSupport REQUIRED )
        find_package( Qt5LinguistTools REQUIRED )
        find_package( Qt5Test )
        include_directories(
                ${Qt5Core_INCLUDE_DIRS}
                ${Qt5Widgets_INCLUDE_DIRS}
                ${Qt5Network_INCLUDE_DIRS}
                ${Qt5Svg_INCLUDE_DIRS}
                ${Qt5Xml_INCLUDE_DIRS}
                ${Qt5PrintSupport_INCLUDE_DIRS}
            )
        set( sir_LINKING_LIBS
                ${sir_LINKING_LIBS}
                ${Qt5Core_LIBRARIES}
                ${Qt5Widgets_LIBRARIES}
                ${Qt5Network_LIBRARIES}
                ${Qt5Svg_LIBRARIES}
                ${Qt5Xml_LIBRARIES}
                ${Qt5PrintSupport_LIBRARIES}
            )
        set( sir_UT_LINKING_LIBS
                ${sir_LINKING_LIBS}
                ${Qt5Test_LIBRARIES}
            )
        add_definitions(
                ${Qt5Core_DEFINITIONS}
                ${Qt5Widgets_DEFINITIONS}
                ${Qt5Network_DEFINITIONS}
                ${Qt5Svg_DEFINITIONS}
                ${Qt5Xml_DEFINITIONS}
                ${Qt5PrintSupport_DEFINITIONS}
            )
        if( SIR_UNDER_TESTS )
            add_definitions( ${Qt5Test_DEFINITIONS} )
        endif( )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

        macro( qt_add_resources )
            QT5_ADD_RESOURCES( ${ARGN} )
        endmacro( )
        macro( qt_add_translation )
            QT5_ADD_TRANSLATION( ${ARGN} )
        endmacro( )
        macro( qt_wrap_ui )
            QT5_WRAP_UI( ${ARGN} )
        endmacro( )
        macro( qt_wrap_cpp )
            QT5_WRAP_CPP( ${ARGN} )
        endmacro( )
else( qt5 )
        message( STATUS "Building using Qt 4" )
        cmake_minimum_required( VERSION 2.8.3 )
        find_package( Qt4 REQUIRED QtCore QtGui QtNetwork QtSvg QtXml )
        include( ${QT_USE_FILE} )
        include_directories( ${include_directories}
                ${QT_QTCORE_INCLUDE_DIR}
                ${QT_QTGUI_INCLUDE_DIR}
                ${QT_QTNETWORK_INCLUDE_DIR}
                ${QT_QTSVG_INCLUDE_DIR}
            )
        set( sir_LINKING_LIBS
                ${sir_LINKING_LIBS}
                ${QT_QTCORE_LIBRARY}
                ${QT_QTGUI_LIBRARY}
                ${QT_QTNETWORK_LIBRARY}
                ${QT_QTSVG_LIBRARY}
                ${QT_QTXML_LIBRARY}
            )
        set( sir_UT_LINKING_LIBS
                ${sir_LINKING_LIBS}
                ${QT_QTTEST_LIBRARY}
            )

        macro( qt_add_resources )
            QT4_ADD_RESOURCES( ${ARGN} )
        endmacro( )
        macro( qt_add_translation )
            QT4_ADD_TRANSLATION( ${ARGN} )
        endmacro( )
        macro( qt_wrap_ui )
            QT4_WRAP_UI( ${ARGN} )
        endmacro( )
        macro( qt_wrap_cpp )
            QT4_WRAP_CPP( ${ARGN} )
        endmacro( )
endif( qt5 )

if( metadata )
    set( CMAKE_MODULE_PATH ${sir_SOURCE_DIR}/cmake )
    find_package( Exiv2 )
    if( EXISTS ${EXIV2_LIBRARY} )
        message(  STATUS "exiv2 library found" )
        add_definitions( -DSIR_METADATA_SUPPORT )
        set( sir_LINKING_LIBS
                ${sir_LINKING_LIBS}
                ${EXIV2_LIBRARY}
            )
        set( sir_UT_LINKING_LIBS
                ${sir_UT_LINKING_LIBS}
                ${EXIV2_LIBRARY}
            )
        set( sir_IMGS
                # options
                images/options_details.png
                images/options_metadata.png
            )
    else( EXISTS ${EXIV2_LIBRARY} )
        message ( FATAL_ERROR "eviv2 library not found! Disable metadata support calling -Dmetadata=OFF in cmake command." )
    endif( EXISTS ${EXIV2_LIBRARY} )
else( metadata )
    message(  STATUS "Building SIR without metadata support." )
endif( metadata )

if( testingDcrawExe )
    add_definitions( -DTESTING_DCRAW_EXE_PERMISSIONS )
endif( testingDcrawExe )


if( WIN32 )
    include_directories( windows/include )
    set( GUI_TYPE WIN32 )
    cmake_policy( SET CMP0020 NEW )
endif( WIN32 )

# Images
set( sir_IMGS
        ${sir_IMGS}
        # stars
        images/favorite.png
        images/unfavorite.png
        # flags
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
        # options
        images/options_general.png
        images/options_list.png
        images/options_raw.png
        images/options_selection.png
        # preview
        images/exit.png
        images/filesave.png
        images/filesaveas.png
        images/flip_horizontal.png
        images/flip_vertical.png
        images/next.png
        images/previous.png
        images/print.png
        images/rotate_ccw.png
        images/rotate_cw.png
        images/window_fullscreen.png
        images/zoom.png
    )

set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )

add_subdirectory( src )

if( SIR_UNDER_TESTS )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests )
    enable_testing( )
    add_subdirectory( tests )
endif()


 ###############################  Installation  ###############################
# icon and desktop file
if( WIN32 )
    install( FILES images/sir.ico DESTINATION share/pixmaps )
else( UNIX )
    install( FILES images/sir.png DESTINATION share/pixmaps )
    install( FILES sir.desktop DESTINATION share/applications )
endif( )
# images
install( FILES ${sir_IMGS} DESTINATION share/sir/images )

