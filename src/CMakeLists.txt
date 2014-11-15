# ################################  SIR  exec  #################################

# Source files
set( sir_SRCS
        CommandLineAssistant.cpp
        ConvertEffects.cpp
        ConvertSharedData.cpp
        ConvertThread.cpp
        DetailsThumbnail.cpp
        effectscollector.cpp
        ExpressionTree.cpp
        languageutils.cpp
        main.cpp
        networkutils.cpp
        rawutils.cpp
        regexputils.cpp
        rgb.cpp
        Selection.cpp
        Session.cpp
        settings.cpp
        sharedinformation.cpp
        sir_string.cpp
        svgmodifier.cpp
        version.cpp
        xmlhelper.cpp
        xmlstreamwriter.cpp
        widgets/aboutdialog.cpp
        widgets/ConvertDialog.cpp
        widgets/brushframe.cpp
        widgets/colorframe.cpp
        widgets/DetailsBrowserController.cpp
        widgets/detailsbrowserview.cpp
        widgets/gradienteditwidget.cpp
        widgets/historycombobox.cpp
        widgets/messagebox.cpp
        widgets/OptionsDialog.cpp
        widgets/posspinbox.cpp
        widgets/previewdialog.cpp
        widgets/selectiondialog.cpp
        widgets/timecombobox.cpp
        widgets/TreeWidget.cpp
        widgets/TreeWidgetHeader.cpp
        widgets/convert/effectsscrollarea.cpp
        widgets/convert/optionsscrollarea.cpp
        widgets/convert/sizescrollarea.cpp
        widgets/convert/svgscrollarea.cpp
        widgets/options/abstractoptions.cpp
        widgets/options/abstractoptionsgroupbox.cpp
        widgets/options/commonoptions.cpp
        widgets/options/filelistgroupboxcontroller.cpp
        widgets/options/filelistgroupboxview.cpp
        widgets/options/generalgroupboxcontroller.cpp
        widgets/options/generalgroupboxview.cpp
        widgets/options/rawgroupboxcontroller.cpp
        widgets/options/rawgroupboxview.cpp
        widgets/options/selectiongroupboxcontroller.cpp
        widgets/options/selectiongroupboxview.cpp
    )

# User interface files
set( sir_UIS
        widgets/about.ui
        widgets/ConvertDialog.ui
        widgets/gradienteditwidget.ui
        widgets/previewdialog.ui
        widgets/convert/effectsscrollarea.ui
        widgets/convert/optionsscrollarea.ui
        widgets/convert/sizescrollarea.ui
        widgets/convert/svgscrollarea.ui
        widgets/options/filelistgroupbox.ui
        widgets/options/generalgroupbox.ui
        widgets/options/rawgroupbox.ui
        widgets/options/selectiongroupbox.ui
        widgets/selection/dirwidget.ui
    )

if( metadata )
    set( sir_SRCS ${sir_SRCS}
            metadata/Metadata.cpp
            metadata/MetadataUtils.cpp
            metadata/Exif.cpp
            metadata/Error.cpp
            metadata/Iptc.cpp
            widgets/MetadataDialog.cpp
            widgets/options/DetailsGroupBoxController.cpp
            widgets/options/DetailsGroupBoxView.cpp
            widgets/options/MetadataGroupBoxController.cpp
            widgets/options/MetadataGroupBoxView.cpp
            widgets/selection/AnyMetadataGroupBox.cpp
        )
    set( sir_UIS ${sir_UIS}
            widgets/MetadataDialog.ui
            widgets/options/DetailsGroupBox.ui
            widgets/options/MetadataGroupBox.ui
            widgets/selection/AnyMetadataGroupBox.ui
            widgets/selection/ExifGroupBox.ui
            widgets/selection/IptcGroupBox.ui
        )
endif( metadata )

qt_wrap_ui( UIS ${sir_UIS} )
set_source_files_properties( ${UIS} PROPERTIES GENERATED ON )
qt_add_resources( RSCS sir.qrc )
set_source_files_properties( ${RSCS} PROPERTIES GENERATED ON )

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
qt_add_translation( QMS ${sir_TRANSLATIONS} )
set_source_files_properties( ${QMS} PROPERTIES GENERATED ON )

add_executable( sir ${GUI_TYPE} ${sir_SRCS} ${UIS} ${MOCS} ${QMS} ${RSCS} )
target_link_libraries( sir ${sir_LINKING_LIBS} )

if( SIR_UNDER_TESTS )
    add_library( sir_library SHARED ${sir_SRCS} ${UIS} ${MOCS} ${QMS} ${RSCS} )
    target_link_libraries( sir_library ${sir_LINKING_LIBS} )
endif( )

# 'make lupdate' command
add_custom_target( lupdate
        COMMAND lupdate -I ./ metadata/ widgets/ widgets/convert ${sir_SRCS} ${sir_HDRS} ${sir_UIS} -ts ${sir_TRANSLATIONS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Install files
# bin
install( TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin )
# translations
install( FILES ${QMS} translations/translation_info.csv DESTINATION share/sir/translations )
