General purpose note taker

# Building

General steps should be common for all platforms:

1. Get the sources
2. Install Qt SDK >= 5.4. QML and Quick modules must be installed.
3. Open noter.pro with Qt Creator and choose Build or Run

Alternatively you can manually build with qmake:

1. Create a build directory outside of the source directory
2. cd into it and run: `qmake path/to/source`
3. Now run `make` or `mingw32-make`, this should produce an executable `noter` or `noter.exe`
