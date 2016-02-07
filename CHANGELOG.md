# v3.1

## Marek Jędryka <jedryka89@gmail.com>

### unreleased

* Changed format of generated thumbnails from TIFF to JPEG
* Fixed wait currsor after unselect files issue
* Integrated tests with TravisCI platform
* Updated Polish translation (Raw basic tab)


# v3.0

## Marek Jędryka <jedryka89@gmail.com>

### Wed Jan 06 2016

* Added Raw tab into main window
* Added Basic view tab for user frendly Raw configuration
* Changed required C++ version to C++11


# v2.8

## Marek Jędryka <jedryka89@gmail.com>

### Thu Mar 05 2015

* Changed SIR to single instance application
* Changed minimum Qt 4.7 required
* Added status bar
* Added Dotate button in About dialog
* Improved saving "Exif.Image.Orientation" metadata tag
* Improved view in details for Aperture, Exposure Time and Exposure Bias metadata
* Improved keyboard navigation for selecting files in TreeWidget
* Removed setting empty dcraw path string after browse cancel
* Fixed left arrow shortcut for previous image in PreviewDialog
* Fixed no files with lowercase extension in "Add File" dialog
* Fixed enabling/disabling buttons in "File Details" options dialog after load settings
* Fixed refresh file details settings after save settings
* Fixed open link in default web browser  instead About dialog
* Fixed freeze of the application while loading many images
* Removed defines.h file
* Removed files coresponded uploading to Google Code
* CamelCase source files naming
* Updated Polish translation


# v2.7.3

## Marek Jędryka <jedryka89@gmail.com>

### Sun Aug 24 2014

* Changed all occurrences of old SIR webpage URL
* Windows: fixed button icons
* Disabled drag & drop URL checking for Qt 4.7 support


# v2.7.2

## Marek Jędryka <jedryka89@gmail.com>

### Sat May 31 2014

* Polish translation improved
* Spanish translation updated (thanks Garbage Collector)
* fixed language autosetting
* Windows: fixed Drag & Drop
* Windows: improved icon


# v2.7.1

## Marek Jędryka <jedryka89@gmail.com>

### Thu Apr 17 2014

* fixed translating of Gradient Effect widgets
* fixed initialization of Gradient Effect
* patched finding icon theme in Qt5 (Tango, oxygen and gnome themes are supported)


# v2.7

## Marek Jędryka <jedryka89@gmail.com>

### Fri Feb 28 2014

* added filter effect
* added disabling SVG tab if no SVG file loaded on file tree view and the view is not empty
* added Version class
* added effects storage in XML files
* added histogram stretching and equalization effects
* added clearing file list before session restoring
* added initializing session and effects directory based restored session from command line
* improved loading files contains incomplete/broken metadata
* CMake: build using choosed Qt version (4/5)
* code update for Qt 5 compatibility
* bugfix: painting effect when desired size typed in Bytes now
* String class moved from MetadataUtils namespace to sir namespace
* Polish translation updated


# v2.6

## Marek Jędryka <jedryka89@gmail.com>

### Sun Apr 28 2013

* added command line arguments --help, --session and --lang support
* added background color in convertion options
* added SVG convertion tab: remove single text node and group of text node containing specifed string
* added adding frame, text and picture effect and effect tab
* added sessions storage in XML files
* create empty metadata when converting from SVG file
* set current date time in Exif if needed
* iconsets moved from UI to C++ code for uic<4.8 compatibility
* Polish translation updated


# v2.5.1

## Marek Jędryka <jedryka89@gmail.com>

### Wed Nov 14 2012

* relative path bug-fix


# v2.5

## Marek Jędryka <jedryka89@gmail.com>

### Tue Nov 13 2012

* project moved to CMake
* Conditional compilation: disable or enable metadata support
* Settings and DetailsBrowser classes added
* Image width and height line edit replaced by spin box
* faster selection using Exiv2 image size detection if it's available
* bugfix: delete selected items from list in ConvertDialog - now delete all selected items
* shared data manipulation methods moved to SharedInformation;
* save and enabled fields in MetadataUtils::Metadata removed - now used Settings fields
* DetailsOptions namespace moved to separated file
* widgets moved to separated directory
* option and selection dialogs splited to few widgets for more flexibility
* remove temporary files at end of session
* width of SVG images render in DetailsBrowser patch
* drop event improved
* myQTreeWidget renamed to TreeWidget, some code moved from ConvertDialog
* external columns into tree view like size (px,K/MiB) added
* new options group box "File list" and header view context menu for TreeWidget settings
* converting from SVG bug when dest size is more than default size fixed
* keep aspect ratio added to settings
* English translation added, Polish translation updated
* many files moved from resource file to another files


# v2.4

## Marek Jędryka <jedryka89@gmail.com>

### Tue Jul 31 2012

* Changelog file provided
* Selected file (or files) details in splitter on rigth option added
* Easier translations of about dialog, about dialog corrected
* Droped QObject::tr() translated strings, non QObject inherited classes are using Q_DECLARE_TR_FUNCTIONS() macro now
* Added selection/import dialog
* Added Selection category in options dialog containing a few settings
* Bugfix loading preview from Exif metadata
* Added main page in documentation
* Save last visited directory
* Simplification of mutual exlusions in ConvertThread
* Bugfix image rotation and set empty iptcData
* In metadata dialog Exif's original and digitized date and time edit field was separated
* Save text edit history when metadata is saved only
* Minor improvements in metadata dialog and history combo box
* Corrected LICENSE file and Free Software Foundation's address in SIR source files. Reported by Praveen Kumar
* Polish translation updated


# v2.3.1

## Marek Jędryka <jedryka89@gmail.com>

### Mon May 07 2012

* saving image size in Exif metadata bugfix
* Serbian translation added
* Czech translation updated

# v2.3

## Marek Jędryka <jedryka89@gmail.com>

### Tue Apr 10 2012

* flip support in convert and preview dialogs
* full support of Exif.Image.Orientation field
* metadata dialog with view and edit features for same Exif metadata fields
* documentation in Doxygen for non private members (private members will be documented latter)
* conversion options in convert dialog is shown in tabs now
* target file size in convert dialog can be set in pixels, percent and Ki-/Mi- bytes now
* new options dialog with categories panel on the left
* suffix file name support
* Polish translation updated


# v2.2

## Marek Jędryka <jedryka89@gmail.com>

### Sun Sep 25 2011

* Bug fixes in convert threads
* New message box
* Metadata support using exiv2 library


# v2.1.1

## Rafael Sachetto <rsachetto@gmail.com>

### Wed Dec 15 2010

* Russian translation updated (Thanks Timur Antipin)


# v2.1

## Rafael Sachetto <rsachetto@gmail.com>

### Sun Apr 11 2010

* Czech translation added (Thanks Pavel Fric).
* Corrected some possible memory leak.
* Now sir can check for updates.
* Now the user can the us that he/she installed sir.
* File menu now is called actions.
* Corrected some name inconsistencies.
* Translations Updates.
* Corrected the about dialog
* Windows Installer Released!


# v2.0.2

## Rafael Sachetto <rsachetto@gmail.com>

### Sat Mar 13 2010

* Greek Translation Added (Thanks Steve Jemens)


# v2.0.1

## Rafael Sachetto <rsachetto@gmail.com>

### Sun Mar 07 2010

* Translations update
* Debian control file updated to recommend dcraw
* Corrected some bugs in Preview Dialog
* Raw utils does not need to be a QOBJECT
* Changed rawutils to be compatible with older Qt versions


#v2.0

## Rafael Sachetto <rsachetto@gmail.com>

### Sun Feb 28 2010

* Added option to use dcraw to convert raw images
* Added Cancel Button when converting images
* Added a browse button on options dialog
* Improved about dialog
* Corrected mouse icon when cancel the new folder creation
* Minor bug fixes


# v1.9.8

## Rafael Sachetto <rsachetto@gmail.com>

### Sun Feb 21 2010

* French translation added. Thanks Jérôme Launay.


# v1.9.7

## Rafael Sachetto <rsachetto@gmail.com>

### Sat Feb 06 2010

* Restore convert selected function because in last release was commented
* Setup conversion threads added - parralel conversion added
* Multiple CPU cores support added
* sir.desktop file impoved


# v1.9.6

## Rafael Sachetto <rsachetto@gmail.com>

### Thu May 28 2009

* Code refactorization
* Multithreading support added. SharedInformation class provided.
* LanguageUtils class provided
* Language menu in options dialog added
* SIR window icon added
* Tranlations updated
* Romanian translation added. Thanks Ovidiu Pop
* Created files: translations/translation_info.txt, TODO


# v1.9.5

## Rafael Sachetto <rsachetto@gmail.com>

### Fri Sep 12 2008

* Debian directory added containign Debian package build rules


# v1.9.4

## Rafael Sachetto <rsachetto@gmail.com>

### Sun May 04 2008

* Polish translation added. Thanks zoteek
* Dutch translation added. Thanks Youp
* Code refactorization
* Header file defines.h provided


# v1.9.3

## Rafael Sachetto <rsachetto@gmail.com>

### Tue Jul 17 2007

* It's first release in SVN repository. It's all I know.
