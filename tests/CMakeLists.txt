include_directories( ${sir_SOURCE_DIR} )

set( sir_UT_LINKING_LIBS sir_library ${sir_UT_LINKING_LIBS} )


set( sir_UT_metadatautils_SRCS
        metadata/metadatautilstest.cpp
    )
add_executable( sir_metadatautils_test ${sir_UT_metadatautils_SRCS} )
target_link_libraries( sir_metadatautils_test ${sir_UT_LINKING_LIBS} )
add_test( NAME MetadataUtilsTest COMMAND sir_metadatautils_test )

set( sir_UT_converteffects_SRCS
        converteffectstest.cpp
    )
add_executable( sir_converteffects_test ${sir_UT_converteffects_SRCS} )
target_link_libraries( sir_converteffects_test ${sir_UT_LINKING_LIBS} )
add_test( NAME ConvertEffectsTest COMMAND sir_converteffects_test )

set( sir_UT_version_SRCS
        versiontest.cpp
    )
add_executable( sir_version_test ${sir_UT_version_SRCS} )
target_link_libraries( sir_version_test ${sir_UT_LINKING_LIBS} )
add_test( NAME VersionTest COMMAND sir_version_test )
