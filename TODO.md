# To Do in SIR Project


## Nearest Release

* Add saturation support to Raw basic tab
* Check add background color effect for transparent PNG images
* Check store last localization of added images to SIR
* CMake optiomalization for Travis CI system - not needed build sir binary, sir_library only is sufficient
* More JPEG compression options


## Next Release

* Restore "Let us know that you using SIR" option
* Search refactorizationable code
* Add Coveralls integration (see https://coveralls.zendesk.com/hc/en-us/articles/201342799-C-C-)
* Add traceback catching (see http://libcsdbg.sourceforge.net/index.html)
* Remove timer from files loading
* Support for animated graphics files (QMovie?)


## 3.x Roadmap

* Extract Raw module to plugin
* Add more features to Raw basic settings view


## 4.0 Roadmap

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
* Run SIR as web service
* Crop images
