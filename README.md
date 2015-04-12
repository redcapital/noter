General purpose note taker

# Building

General steps should be common for all platforms:

1. Get the sources
2. Install Qt SDK 5.4. QML and Quick modules must be installed.
  - Windows - install Qt SDK for Windows 32-bit (MinGW): http://www.qt.io/download-open-source/#section-2
  - MacOS - use brew `brew install qt5` and install GCC or Clang
  - Linux - use your package manager to get build deps
3. Open noter.pro with Qt Creator and choose Build or Run

Alternatively you can manually build with qmake:

1. Create a build directory outside of the source directory
2. cd into it and run: `qmake path/to/source`
3. Now run `make` or `mingw32-make`, this should produce an executable `noter` or `noter.exe`

