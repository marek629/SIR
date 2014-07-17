include_directories( ${sir_SOURCE_DIR} )

set( sir_UT_LINKING_LIBS sir_library ${sir_UT_LINKING_LIBS} )


set( sir_UT_metadatautils_SRCS
        metadata/metadatautilstest.cpp
    )
add_executable( sir_metadatautils_test ${sir_UT_metadatautils_SRCS} )
target_link_libraries( sir_metadatautils_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "MetadataUtils_UT" COMMAND sir_metadatautils_test )

set( sir_UT_converteffects_SRCS
        converteffectstest.cpp
    )
add_executable( sir_converteffects_test ${sir_UT_converteffects_SRCS} )
target_link_libraries( sir_converteffects_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "ConvertEffects_UT" COMMAND sir_converteffects_test )

set( sir_UT_version_SRCS
        languageutilstest.cpp
    )
add_executable( sir_languageutils_test ${sir_UT_version_SRCS} )
target_link_libraries( sir_languageutils_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "LanguageUtils_UT" COMMAND sir_languageutils_test )

set( sir_UT_rawgroupbox_SRCS
        rawgroupboxtest.cpp
    )
add_executable( sir_rawgroupbox_test ${sir_UT_version_SRCS} )
target_link_libraries( sir_rawgroupbox_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "RawGroupBox_UT" COMMAND sir_rawgroupbox_test )

set( sir_UT_version_SRCS
        versiontest.cpp
    )
add_executable( sir_version_test ${sir_UT_version_SRCS} )
target_link_libraries( sir_version_test ${sir_UT_LINKING_LIBS} )
add_test( NAME "Version_UT" COMMAND sir_version_test )
