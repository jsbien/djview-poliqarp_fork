## ABOUT THE FORK

The primary purpose of the fork of https://bitbucket.org/mrudolf/djview-poliqarp/ is to enable the discussions and creating a wiki.

## ABOUT DJVIEW4POLIQARP

djview4poliqarp is a fork of djview4 (http://djvu.sourceforge.net/djview4.html).
Its purpose is to serve as a remote client to Poliqarp for DjVu server
providing access to so called graphical concordances in a convenient way,
cf. eg. https://github.com/jsbien/ndt/wiki/z_djview4p.

The ideas to use DjView as a Poliqarp client and other suggestions
were formulated by Janusz S. Bień. A prototype was implemented by
Grzegorz Chimosz, the present version has been developed by Michał
Rudolf.

In years 2009-2012 the work was supported by the Ministry of Science
and Higher Education's grant no. N N519 384036, now it is occasionally
supported by Formal Linguistics Department of the University of Warsaw.

The recent versions of the program support indexes, cf. Bień, Janusz
S. (2014) Elektroniczny indeks do słownika Lindego. In: V Glosa do
leksykografii, 18-19 września 2014 r. 
(https://www.researchgate.net/publication/364994883, https://www.researchgate.net/publication/335627459)
and https://github.com/jsbien/iLindeCSV.

The most recent presentation of the program (in Polish) is available at 
https://www.researchgate.net/publication/351548535 (slides) and https://www.youtube.com/watch?v=mOYzwpjTAf4 (video).

Some aspects of the program has been described in English
in a paper available at https://www.researchgate.net/publication/375934534.
## HOW TO COMPILE WINDOWS VERSION

### Native compilation on Windows

You will need Qt SDK (available from http://qt-project.org). Compile djview4poliqap,
using MINGW32. To run, DjView4Poliqarp needs Qt DLLs (available with Qt),
DjVuLibre DLL (available from http://djvu.sourceforge.net) and MINGW runtime libraries
(available with Qt).
You can get all the necessary files by extracting the installer available at
DjView4Poliqarp website (https://bitbucket.org/mrudolf/djview-poliqarp/downloads/DjView4Poliqarp-1.5.exe).

To create installer, you need Inno Setup from http://www.jrsoftware.org/isinfo.php.
You will need to adjust version number and the paths to local versions DLL libraries.

See [Windows.md](Windows.md) for more details.

### Cross-compilation for Windows on Linux

You will need a cross-compiling toolchain and cross-compiled Qt and djvulibre libraries.
The `cross-compile.sh` script automates the building process of those and the application itself.

See [Windows-cross-compile.md](Windows-cross-compile.md) for details.

## HOW TO COMPILE LINUX VERSION

Install the build dependencies listed in debian/control, then run

dpkg-buildpackage -uc -us -b -rfakeroot

or an equivalent command.

## HOW TO COMPILE MACINTOSH VERSION

Contact Michał Rudolf.
