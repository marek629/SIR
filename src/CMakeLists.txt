# ################################  SIR  exec  #################################

# Source files
file( GLOB_RECURSE sir_SRCS
        exceptions/*.cpp
        raw/*.cpp
    )
set( sir_SRCS ${sir_SRCS}
        CommandLineAssistant.cpp
        ConvertEffects.cpp
        ConvertSharedData.cpp
        ConvertThread.cpp
        DetailsThumbnail.cpp
        EffectsCollector.cpp
        ExpressionTree.cpp
        LanguageUtils.cpp
        main.cpp
        NetworkUtils.cpp
        OptionsGroupBoxManager.cpp
        RegExpUtils.cpp
        Rgb.cpp
        Selection.cpp
        Session.cpp
        Settings.cpp
        SharedInformation.cpp
        SharedInformationBuilder.cpp
        sir_String.cpp
        SvgModifier.cpp
        Version.cpp
        XmlHelper.cpp
        XmlStreamWriter.cpp
        file/FileInfo.cpp
        shared/EffectsConfiguration.cpp
        visitors/RichTextVisitor.cpp
        widgets/AboutDialog.cpp
        widgets/ConvertDialog.cpp
        widgets/BrushFrame.cpp
        widgets/ColorFrame.cpp
        widgets/DetailsBrowserController.cpp
        widgets/DetailsBrowserView.cpp
        widgets/GradientEditWidget.cpp
        widgets/HistoryComboBox.cpp
        widgets/MessageBox.cpp
        widgets/OptionsDialog.cpp
        widgets/PosSpinBox.cpp
        widgets/PreviewDialog.cpp
        widgets/SelectionDialog.cpp
        widgets/StatusWidget.cpp
        widgets/TimeComboBox.cpp
        widgets/TreeWidget.cpp
        widgets/TreeWidgetHeader.cpp
        widgets/convert/EffectsScrollArea.cpp
        widgets/convert/EffectsScrollAreaVisitor.cpp
        widgets/convert/OptionsScrollArea.cpp
        widgets/convert/RawTabWidget.cpp
        widgets/convert/SizeScrollArea.cpp
        widgets/convert/SvgScrollArea.cpp
        widgets/options/AbstractOptions.cpp
        widgets/options/AbstractOptionsGroupBox.cpp
        widgets/options/CommonOptions.cpp
        widgets/options/FileListGroupBoxController.cpp
        widgets/options/FileListGroupBoxView.cpp
        widgets/options/GeneralGroupBoxController.cpp
        widgets/options/GeneralGroupBoxView.cpp
        widgets/options/RawGroupBoxView.cpp
        widgets/options/SelectionGroupBoxController.cpp
        widgets/options/SelectionGroupBoxView.cpp
    )

# User interface files
set( sir_UIS
        raw/AdvancedRawWidget.ui
        raw/BasicRawScrollArea.ui
        widgets/AboutDialog.ui
        widgets/ConvertDialog.ui
        widgets/GradientEditWidget.ui
        widgets/PreviewDialog.ui
        widgets/StatusWidget.ui
        widgets/convert/EffectsScrollArea.ui
        widgets/convert/OptionsScrollArea.ui
        widgets/convert/SizeScrollArea.ui
        widgets/convert/SvgScrollArea.ui
        widgets/options/FileListGroupBoxView.ui
        widgets/options/GeneralGroupBoxView.ui
        widgets/options/SelectionGroupBoxView.ui
        widgets/selection/DirWidget.ui
    )

if( metadata )
    set( sir_SRCS ${sir_SRCS}
            metadata/Error.cpp
            metadata/Exif.cpp
            metadata/Flash.cpp
            metadata/Iptc.cpp
            metadata/Metadata.cpp
            metadata/MetadataUtils.cpp
            metadata/structs/ExifPrintSettings.cpp
            metadata/structs/ExifStruct.cpp
            metadata/structs/IptcPrintSettings.cpp
            metadata/structs/IptcStruct.cpp
            metadata/structs/MetadataStruct.cpp
            metadata/visitors/ExifRichTextVisitor.cpp
            metadata/visitors/IptcRichTextVisitor.cpp
            widgets/MetadataDialog.cpp
            widgets/options/DetailsGroupBoxController.cpp
            widgets/options/DetailsGroupBoxView.cpp
            widgets/options/MetadataGroupBoxController.cpp
            widgets/options/MetadataGroupBoxView.cpp
            widgets/selection/AnyMetadataGroupBox.cpp
        )
    set( sir_UIS ${sir_UIS}
            widgets/MetadataDialog.ui
            widgets/options/DetailsGroupBoxView.ui
            widgets/options/MetadataGroupBoxView.ui
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
