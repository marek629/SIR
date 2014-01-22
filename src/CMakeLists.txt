# ################################  SIR  exec  #################################

# Source files
set( sir_SRCS
        commandlineassistant.cpp
        convertdialog.cpp
        converteffects.cpp
        convertshareddata.cpp
        convertthread.cpp
        effectscollector.cpp
        expressiontree.cpp
        languageutils.cpp
        main.cpp
        networkutils.cpp
        rawutils.cpp
        regexputils.cpp
        rgb.cpp
        selection.cpp
        session.cpp
        settings.cpp
        sharedinformation.cpp
        sir_string.cpp
        svgmodifier.cpp
        version.cpp
        xmlhelper.cpp
        xmlstreamwriter.cpp
        widgets/aboutdialog.cpp
        widgets/brushframe.cpp
        widgets/colorframe.cpp
        widgets/detailsbrowser.cpp
        widgets/gradienteditwidget.cpp
        widgets/historycombobox.cpp
        widgets/messagebox.cpp
        widgets/optionsdialog.cpp
        widgets/posspinbox.cpp
        widgets/previewdialog.cpp
        widgets/selectiondialog.cpp
        widgets/timecombobox.cpp
        widgets/treewidget.cpp
        widgets/treewidgetheader.cpp
        widgets/convert/effectsscrollarea.cpp
        widgets/convert/optionsscrollarea.cpp
        widgets/convert/sizescrollarea.cpp
        widgets/convert/svgscrollarea.cpp
        widgets/options/abstractoptions.cpp
        widgets/options/abstractoptionsgroupbox.cpp
        widgets/options/commonoptions.cpp
        widgets/options/filelistgroupbox.cpp
        widgets/options/generalgroupbox.cpp
        widgets/options/rawgroupbox.cpp
        widgets/options/selectiongroupbox.cpp
    )

# User interface files
set( sir_UIS
        widgets/about.ui
        widgets/convertdialog.ui
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
qt_wrap_ui( UIS ${sir_UIS} )
set_source_files_properties( ${UIS} PROPERTIES GENERATED ON )

if( metadata )
    set( sir_MOCS ${sir_MOCS}
            ${METADATA_MOCS}
            widgets/metadatadialog.h
            widgets/options/detailsgroupbox.h
            widgets/options/metadatagroupbox.h
            widgets/selection/anymetadatagroupbox.h
        )
    set( sir_HDRS ${sir_HDRS}
            ${METADATA_HEADERS}
            metadata/metadata.h
            metadata/metadatautils.h
            metadata/error.h
            metadata/exif.h
            metadata/iptc.h
        )
    set( sir_SRCS ${sir_SRCS}
            metadata/metadata.cpp
            metadata/metadatautils.cpp
            metadata/exif.cpp
            metadata/error.cpp
            metadata/iptc.cpp
            widgets/metadatadialog.cpp
            widgets/selection/anymetadatagroupbox.cpp
            widgets/options/detailsgroupbox.cpp
            widgets/options/metadatagroupbox.cpp
        )
    set( sir_UIS ${sir_UIS}
            widgets/metadatadialog.ui
            widgets/options/detailsgroupbox.ui
            widgets/options/metadatagroupbox.ui
            widgets/selection/anymetadatagroupbox.ui
            widgets/selection/exifgroupbox.ui
            widgets/selection/iptcgroupbox.ui
        )
endif( metadata )

#qt_wrap_cpp( MOCS ${sir_MOCS} )

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
