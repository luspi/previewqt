# Installing PreviewQt

The instructions below serve as a guide to compile and install PreviewQt from source. There are various means of installing PreviewQt (Windows installer, Flatpak, etc.) listed on the website at https://previewqt.org/.

## DEPENDENCIES

- Qt >= 6.2 (6.5 or higher recommended)
- CMake (needed for building PreviewQt)
- Qt6 ImageFormats

Make sure that you have all the required QML modules installed:
QtMultimedia, QtQuick, QtQuick.Controls, QtQuick.Controls.Styles, QtQuick.Layouts, QtQuick.Window

Dependencies that are needed by default, but can be disabled via CMake:

- [LibArchive](https://libarchive.org)
- [Exiv2](https://exiv2.org)
- [ImageMagick](https://imagemagick.org) *or* [GraphicsMagick](http://www.graphicsmagick.org/)
- [LibRaw](https://www.libraw.org)
- [Poppler](https://poppler.freedesktop.org) (can be replaced by QtPDF)
- [FreeImage](https://freeimage.sourceforge.io)
- [DevIL](http://openil.sourceforge.net)
- [KSyntaxHighlighting](https://invent.kde.org/frameworks/syntax-highlighting)

Dependencies that are disabled by default, but can be enabled via CMake:

- [LibVips](https://www.libvips.org/)
- [resvg](https://github.com/RazrFalcon/resvg)
- [libmpv](https://mpv.io/) (requires switching off QtMultimedia)


Please note that you probably want to have as many of these enabled as possible as they greatly enhance the experience of PreviewQt.

#### NOTE

On some systems you also need the *-dev package for compiling (e.g. exiv2-dev - names can vary slightly depending on your distribution). These packages usually can be uninstalled again after compilation is done.

#### NOTE

PreviewQt can work with either ImageMagick and GraphicsMagick, but due to conflicting naming schemes it is not possible to use both at the same time. By default ImageMagick will be enabled in CMake.

#### NOTE

Exiv2 can be compiled with support for the BMFF format. Note that there is the possibility that BMFF support may be the subject of patent rights. PreviewQt will by default opt-in to reading this format (if supported by Exiv2). If you prefer to not include support for this format in PreviewQt simply set the `EXIV2_ENABLE_BMFF` CMake option to `OFF`.

## ADDITIONAL IMAGE FORMATS

These are some libraries and tools that can add additional formats to PreviewQt if installed. None of them are needed at compile time, but they can be picked up at runtime if available.

- [KImageFormats](https://api.kde.org/frameworks/kimageformats/html/)
- [Qt plug-in for AVIF images](https://github.com/novomesk/qt-avif-image-plugin)
- [Qt plug-in for JPEG XL images](https://github.com/novomesk/qt-jpegxl-image-plugin)
- [Qt plug-in for HEIF/HEIC images](https://github.com/novomesk/qt-heic-image-plugin)
- [XCFtools](https://github.com/j-jorge/xcftools)
- [libqpsd](https://github.com/Code-ReaQtor/libqpsd)
- [unrar](https://www.rarlab.com/)

## BUILDING AND INSTALLING

1. _mkdir build && cd build/_

2. _cmake .._

    \# Note: This installs PreviewQt by default into /usr/local/{bin,share}
    \# To install PreviewQt into another prefix e.g. /usr/{bin,share}, run:

    _cmake -DCMAKE\_INSTALL\_PREFIX=/usr .._

    \# At this step you can also en-/disable any compile time features.
    \# For example the remote previewqt application can be turned off by

    _cmake -DBUILD_PREVIEWQT_REMOTE=OFF .._

3. _make_

    \# This creates an executeable previewqt binary located in the ./build/ folder

4. (as root or sudo) _make install_

    \# This command:
    \# 1. is only required if you want to **install** PreviewQt
    \# 2. installs the desktop file to share/applications/
    \# 3. moves some icons to icons/hicolor/
    \# 4. moves the binary to bin/
    \# 5. installs the metainfo file to share/metainfo/

## UNINSTALL

If you want to uninstall PreviewQt, simply run __make uninstall__ as root. This removes the desktop file (via _xdg-desktop-menu uninstall_), the icons, the binary file, and the metainfo file. Alternatively you can simply remove all the files manually which should yield the same result.

## BUILDING ON WINDOWS

PreviewQt offers installers for pre-built binaries on its website: https://previewqt.org/

If you prefer to build it yourself, this process is not as hard as it might seem at first. The main challenge in building PreviewQt on Windows lies in getting the environment set up and all dependencies installed.

The following are required dependencies:

1. Install Visual Studio 2019 Community Edition (free, be sure to install the 'Desktop Development with C++' workload)
    - Website: https://visualstudio.microsoft.com/
2. Install CMake
    - Website: https://cmake.org/
    - In the installer set the system path option to Add CMake to the system PATH for all users
3. Install Qt 6.2 (6.5+ recommended)
    - Website: https://qt.io
    - In the installer, make sure to install all required modules as listed above under dependencies
    - After installation, confirm that your installation of Qt finds both CMake and the compiler installed in steps 1 and 2

The following dependencies are recommended but can be disabled through CMake if not wanted:

1. [LibArchive](https://libarchive.org)
2. [Exiv2](https://exiv2.org)
3. [ImageMagick](https://imagemagick.org)
4. [LibRaw](https://www.libraw.org)
5. [Poppler](https://poppler.freedesktop.org) (can be replaced by QtPDF)
6. [FreeImage](https://freeimage.sourceforge.io)
7. [DevIL](http://openil.sourceforge.net)

Make sure that any installed dependency is added to the system path, or otherwise you need to explicitely point CMake to the right location for each of them. Regardless, CMake might have to be explicitely pointed to the library/include paths of some of the dependencies by specifying `target_include_directories()` and `target_link_libraries()`.

Once all the requried and desired dependencies are installed, then the source code of PreviewQt can be fetched from the website (https://previewqt.og/). Then simply follow the instructions in the `BUILDING AND INSTALLING` section above
