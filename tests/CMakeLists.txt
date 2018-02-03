include_directories( ${sir_SOURCE_DIR} )

set( sir_UT_LINKING_LIBS sir_library ${sir_UT_LINKING_LIBS} )


include_directories( ${CMAKE_BINARY_DIR}/src )
set( sir_IT_generalgroupbox_SRCS
        widgets/options/GeneralGroupBoxTest.cpp
    )
add_executable( sir_generalgroupbox_test ${sir_IT_generalgroupbox_SRCS} )
target_link_libraries( sir_generalgroupbox_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "GeneralGroupBox_IT" COMMAND sir_generalgroupbox_test )

include_directories( ${CMAKE_BINARY_DIR}/src )
set( sir_UT_rawcontroller_SRCS
        raw/RawControllerTest.cpp
    )
add_executable( sir_rawcontroller_test ${sir_UT_rawcontroller_SRCS} )
target_link_libraries( sir_rawcontroller_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "RawController_UT" COMMAND sir_rawcontroller_test )

include_directories( ${CMAKE_BINARY_DIR}/src )
set( sir_UT_rawloader_SRCS
        raw/core/graphics/RawLoaderTest.cpp
    )
add_executable( sir_rawloader_test ${sir_UT_rawloader_SRCS} )
target_link_libraries( sir_rawloader_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "RawLoader_UT" COMMAND sir_rawloader_test )

include_directories( ${CMAKE_BINARY_DIR}/src )
set( sir_UT_rawmodelvalidator_SRCS
        raw/RawModelValidatorTest.cpp
    )
add_executable( sir_rawmodelvalidator_test ${sir_UT_rawmodelvalidator_SRCS} )
target_link_libraries( sir_rawmodelvalidator_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "RawModelValidator_UT" COMMAND sir_rawmodelvalidator_test )


set( sir_UT_converteffects_SRCS
        ConvertEffectsTest.cpp
    )
add_executable( sir_converteffects_test ${sir_UT_converteffects_SRCS} )
target_link_libraries( sir_converteffects_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "ConvertEffects_UT" COMMAND sir_converteffects_test )

set( sir_UT_convertthread_SRCS
        ConvertThreadTest.cpp
    )
add_executable( sir_convertthread_test ${sir_UT_convertthread_SRCS} )
target_link_libraries( sir_convertthread_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "ConvertThread_UT" COMMAND sir_convertthread_test )

set( sir_IT_qimageloader_SRCS
        image/QImageLoaderTest.cpp
        fake/ConvertThreadFake.cpp
    )
add_executable( sir_qimageloader_test ${sir_IT_qimageloader_SRCS} )
target_link_libraries( sir_qimageloader_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "QImageLoader_IT" COMMAND sir_qimageloader_test )

set( sir_UT_languageutils_SRCS
        LanguageUtilsTest.cpp
    )
add_executable( sir_languageutils_test ${sir_UT_languageutils_SRCS} )
target_link_libraries( sir_languageutils_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "LanguageUtils_UT" COMMAND sir_languageutils_test )

set( sir_UT_version_SRCS
        VersionTest.cpp
    )
add_executable( sir_version_test ${sir_UT_version_SRCS} )
target_link_libraries( sir_version_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "Version_UT" COMMAND sir_version_test )


set( sir_IT_convertdialog_SRCS
        widgets/ConvertDialogTest.cpp
        fake/CommandLineAssistantFake.cpp
    )
add_executable( sir_convertdialog_test ${sir_IT_convertdialog_SRCS} )
target_link_libraries( sir_convertdialog_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "ConvertDialog_IT" COMMAND sir_convertdialog_test )

set( sir_IT_detailsthumbnail_SRCS
        thumbnail/DetailsThumbnailTest.cpp
        thumbnail/ThumbnailImageData.cpp
    )
add_executable( sir_detailsthumbnail_test ${sir_IT_detailsthumbnail_SRCS} )
target_link_libraries( sir_detailsthumbnail_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "DetailsThumbnail_IT" COMMAND sir_detailsthumbnail_test )

#include_directories( ${CMAKE_BINARY_DIR}/src )
file( GLOB imageformat_test_file_list "convert/model/*Test.cpp" )
foreach( test_file ${imageformat_test_file_list} )
    set( sir_UT_imageformat_SRCS
            ${test_file}
        )
    get_filename_component( test_name ${test_file} NAME_WE )
    add_executable( ${test_name} ${sir_UT_imageformat_SRCS} )
    target_link_libraries( ${test_name} ${sir_UT_LINKING_LIBS} )
    add_test( NAME "${test_name}" COMMAND ImageFormatLinearSizeTest )
endforeach( )


if( metadata )

    set( sir_UT_metadatautils_SRCS
            metadata/MetadataUtilsTest.cpp
        )
    add_executable( sir_metadatautils_test ${sir_UT_metadatautils_SRCS} )
    target_link_libraries( sir_metadatautils_test ${sir_UT_LINKING_LIBS} )
    add_test( NAME "MetadataUtils_UT" COMMAND sir_metadatautils_test )


    include_directories( ${CMAKE_BINARY_DIR}/src )
    set( sir_IT_detailsgroupbox_SRCS
            widgets/options/DetailsGroupBoxTest.cpp
        )
    add_executable( sir_detailsgroupbox_test ${sir_IT_detailsgroupbox_SRCS} )
    target_link_libraries( sir_detailsgroupbox_test ${sir_UT_LINKING_LIBS} )
    add_test( NAME "DetailsGroupBox_IT" COMMAND sir_detailsgroupbox_test )

endif( metadata )
