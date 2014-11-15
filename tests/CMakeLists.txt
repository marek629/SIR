include_directories( ${sir_SOURCE_DIR} )

set( sir_UT_LINKING_LIBS sir_library ${sir_UT_LINKING_LIBS} )


set( sir_UT_metadatautils_SRCS
        metadata/metadatautilstest.cpp
    )
add_executable( sir_metadatautils_test ${sir_UT_metadatautils_SRCS} )
target_link_libraries( sir_metadatautils_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "MetadataUtils_UT" COMMAND sir_metadatautils_test )


include_directories( ${CMAKE_BINARY_DIR}/src )
set( sir_UT_generalgroupbox_SRCS
        widgets/options/GeneralGroupBoxTest.cpp
    )
add_executable( sir_generalgroupbox_test ${sir_UT_generalgroupbox_SRCS} )
target_link_libraries( sir_generalgroupbox_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "GeneralGroupBox_UT" COMMAND sir_generalgroupbox_test )

include_directories( ${CMAKE_BINARY_DIR}/src )
set( sir_UT_rawgroupboxcontroller_SRCS
        widgets/options/rawgroupboxcontrollertest.cpp
    )
add_executable( sir_rawgroupboxcontroller_test ${sir_UT_rawgroupboxcontroller_SRCS} )
target_link_libraries( sir_rawgroupboxcontroller_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "RawGroupBoxController_UT" COMMAND sir_rawgroupboxcontroller_test )


set( sir_UT_converteffects_SRCS
        ConvertEffectsTest.cpp
    )
add_executable( sir_converteffects_test ${sir_UT_converteffects_SRCS} )
target_link_libraries( sir_converteffects_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "ConvertEffects_UT" COMMAND sir_converteffects_test )

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
