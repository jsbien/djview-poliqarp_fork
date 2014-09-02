ABOUT DJVIEW4POLIQARP

djview4poliqarp is a fork of djview4 (http://djvu.sourceforge.net/djview4.html).
Its purpose is to serve as a remote client to Poliqarp for DjVu server
providing access to so called graphical concordances in a convenient way,
cf. eg. https://bitbucket.org/jsbien/ndt/wiki/z_djview4p.

The ideas to use DjView as a Poliqarp client and other suggestions
were formulated by Janusz S. Bień. A prototype was implemented by
Grzegorz Chimosz, the present version has been developed by Michał
Rudolf.

In years 2009-2012 the work was supported by the Ministry of Science
and Higher Education's grant no. N N519 384036, now it is occasionally
supported by Formal Linguistics Department of the University of Warsaw.



HOW TO COMPILE WINDOWS VERSION

You will need Qt SDK (available from http://qt-project.org). Compile djview4poliqap,
using MINGW32. To run, DjView4Poliqarp needs Qt DLLs (available with Qt),
DjVuLibre DLL (available from http://djvu.sourceforge.net) and MINGW runtime libraries
(available with Qt).
You can get all the necessary files by extracting the installer available at
DjView4Poliqarp website (https://bitbucket.org/mrudolf/djview-poliqarp/downloads/DjView4Poliqarp-1.5.exe).

To create installer, you need Inno Setup from http://www.jrsoftware.org/isinfo.php.
You will need to adjust version number and the paths to local versions DLL libraries.

HOW TO COMPILE LINUX VERSION

A very brief instruction for Debian and derivates can be found at https://bitbucket.org/jsbien/ndt/wiki/d_djv4s.

HOW TO COMPILE MACINTOSH

Contact Michał Rudolf.