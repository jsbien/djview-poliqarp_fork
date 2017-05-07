# Compiling djview4poliqarp in Windows

## 1. Requirement
* Microsoft Visual C++ Express 2012, 2013 or 2015 - choose the version
  appropriate for your Windows version. Just google it and download
  from Microsoft site. It is free, but the links keep changing.
* [Qt](https://www.qt.io/download-open-source/) - choose the version
  matching your version of Visual C++ Express. Look for VS 201x code.
  For VSCE 2012 you have to use older version of Qt (5.5 seems
 the last one supporting it).
* [DjVuLibre/djview](https://www.qt.io/download-open-source/) - select
  the last available Windows version.
* [djview4poliqarp sources](https://bitbucket.org/mrudolf/djview-poliqarp/downloads/)
  You can use [Git](https://git-scm.com) if you like, but the easiest way
  is to download the repository directly from Bitbucket site.

## 2. Compilation
1. Open Qt Creator
2. Open project file. The file name is *djview4poliqarp.pro* and it
   is in *src/* subdirectory.
3. Find the project file in the file list on the left. Open it and check the path
   for *djvulibre.lib*. Depending on your Windows version, the correct
   path will probably be either *C:\Program Files (x86)/DjVuLibre\djvulibre.lib* or
   *C:\Program Files/DjVuLibre\djvulibre.lib*.
4. Select *Release* from bottom left panel if it says *Debug*. Debug is
   useful only if you want to debug the program. If so, different versions
   of Qt libraries are needed (see next section).
5. Build the program by starting the compilation from the *Build* menu.

## 3. Running from Qt Creator
To make djview4poliqarp run, you have to copy appropriate DLL files.
1. First, find the build directory. It should be a direct subdirectory
   of your djview-poliqarp sources. If you haven't changed it,
   it probably starts with *build-djview4poliqarp-Desktop*, ends with
   *Release* and includes Qt version. If you find it, go into the *release*
   subdirectory of it. We will copy all files there.
2. Go to *C:\Program Files*\DjVuLibre\ and copy all *.dll files into your
   build directory. Be sure to copy at least:
   * libdjvulibre.dll
   * libeay32.dll
   * libjpeg.dll
   * libssl32.dll
   * libtiff.dll
   * ssleay32.dll
3. Run the program by selecting *Run* from *Build* menu

## 4. Running outside Qt Creator
You need to copy more files into build directory
1. Go into your Qt directory (*C:\Qt* if you used default installation
   path). Go inside your Qt version, then inside compiler directory,
   until there is *bin* subdirectory. Go there.
2. Copy following files into your build directory:
   * libEGL.dll
   * libGLESv2.dll
   * icuin5*.dll
   * icuuc5*.dll
   * icudt5*.dll
   * Qt5Core.dll
   * Qt5Gui.dll
   * Qt5Widgets.dll
   * Qt5Network.dll
   * Qt5Xml.dll
3. Go one directory up and go into plugins subdirectory
   3.1 Go into imageformats and copy *qjpeg.dll* into
      *imageformats* subdirectory of your build directory (you have
      to create it).
   3.2 Go one directory up and into platfforms directory of plugins. Copy
      *qwindows.dll* into platforms subdirectory of your build directory
      (you have to create it).
4. Go into your Windows system directory (probably *C:\windows\system32*)
   Copy msvcp*.dll and msvcr*.dll into your build directory.
   Probably it is enough to copy files matching your Visual C++ Express
   version (e.g. msvcp120.dll for 2012), but your mileage may vary.

## 5. Creating installer (optional)
1. Install [Inno Setup](http://www.jrsoftware.org/isinfo.php).
2. Open setup.iss from djview-poliqarp directory in Inno Setup. It should
   be enough to click on it.
3. Adjust the paths of your *.exe and *.dll files. See above to find
   correct paths.
4. Compile the file to create the installer.
