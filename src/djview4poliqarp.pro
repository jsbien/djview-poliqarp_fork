# #####################################################################
# Automatically generated by qmake (2.01a) czw. sie 27 19:06:01 2009
# #####################################################################
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .
QT += network xml
# RESOURCES = resources.qrc
# TRANSLATIONS = translations/pl.ts
LIBS += -ldjvulibre

# Input
HEADERS += recentfiles.h \
	 mainwindow.h \
	 qdjvuwidget.h \
	 qdjvuhttp.h \
	 qdjvu.h \
	 messagedialog.h \
	 poliqarpwidget.h \
    poliqarp.h \
    queryitem.h
SOURCES += recentfiles.cpp \
	 mainwindow.cpp \
	 main.cpp \
	 qdjvuwidget.cpp \
	 qdjvuhttp.cpp \
	 qdjvu.cpp \
	 messagedialog.cpp \
	 poliqarpwidget.cpp \
    poliqarp.cpp \
    queryitem.cpp
FORMS = mainwindow.ui \
	 poliqarpwidget.ui
