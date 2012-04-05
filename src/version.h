/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef VERSION_H
#define VERSION_H

#include <QtCore>

class Version {
public:
	static int version();
	static QString versionText();
	static int buildNumber();
	static QString buildText();
private:
	static int m_version;
	static QString m_build;
};

#endif // VERSION_H
