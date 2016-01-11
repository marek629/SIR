# SIR

Simple Image Resizer is a tool created for batch parallel resizing, rotate
and converting images with metadata support.

You can add frame or text and apply color and gradient filters on your images.

You can also manipulate on histogram.

Finally you can do this all activities on many images at the same time using
your multiple core CPU!


[![Build Status](https://travis-ci.org/marek629/SIR.svg)](https://travis-ci.org/marek629/SIR)


## Licence

This is free software distributed under GNU GPL v2 license.
Read [LICENSE](LICENSE) file for details.



## Supported image formats


### Read/write supported image formats

* BMP  - Windows Bitmap
* JPEG - Joint Photographic Experts Group
* PNG  - Portable Network Graphics
* PPM  - Portable Pixmap
* TIFF - Tagged Image File Format
* XBM  - X11 Bitmap
* XPM  - X11 Pixmap


### Read only supported image formats

* GIF  - Graphic Interchange Format (optional)
* MNG  - Multiple-image Network Graphics
* PBM  - Portable Bitmap
* PGM  - Portable Graymap
* SVG  - Scalable Vector Graphics
* TGA  - Targa Image Format


### Supported metadata standards using exiv2

* Exif (edition supported)
* IPTC (edition supported)
* XMP



## Installation

Installation instruction was stored in [INSTALL](INSTALL.md) file.



## Dependences

* [Qt 4.7+ or Qt 5.0+](http://qt-project.org/)


### Optional dependences

* [libexiv2](http://exiv2.org/)
* [dcraw](http://www.cybercom.net/~dcoffin/dcraw/)


### Build dependences

* [CMake 2.8.3+](http://www.cmake.org/)


### Documentation build dependences

SIR documentation was created using [Doxygen](http://www.stack.nl/~dimitri/doxygen/).
