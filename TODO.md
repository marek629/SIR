# To Do in SIR Project


## Nearest Release

* Describe release procedure
* Remove timer from files loading
* Graphical dcraw options, for example as checkboxes
    * Help message in dialog from dcraw command
* Support for animated graphics files (QMovie?)


## Next Release

* Restore "Let us know that you using SIR" option
* Search refactorizationable code
* Add traceback catching (see http://libcsdbg.sourceforge.net/index.html)
* Integrate tests within TravisCI


## 3.0 Roadmap

* SVG & metadata & effects as plugins
* dynamically add effects to effect list (empty initially)
* multiple effects convertion:
    * save few images from just one, for example diffrent color effects
    * generate a flow graph for few effects files
    * generate a interactive visualisation of the flow graph
* storing settings in XML file


## Wishlist

* SelectionDialog:
    * add in exifGroupBox: FocalLegth, Aperture, ExpBias, IsoSpeed, ExpTime, ShutterSpeed
* Geotagging
* Create ZIP archive containg converted images
* Add PDF files support
    * creating multipage PDF document from selected images
    * rotate multipage PDF document
* Conditional converting (ie. convert if image width > 1000 px)
    * scripts with exiv2 support
    * graphical flow diagram editor for scripts substitution
* Add color filters - colormaps (dog's view, jet); see also: http://www.mathworks.com/help/matlab/ref/colormap.html
* More JPEG compression options
* Run SIR as web service
* Crop images
