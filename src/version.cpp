/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "version.h"


int Version::version()
{
	return m_version;
}

QString Version::versionText()
{
	return QString("%1.%2").arg(m_version / 100)
			.arg(m_version % 100, 2, 10, QChar('0'));
}

int Version::buildNumber()
{
	return buildText().toInt();
}

QString Version::buildText()
{
	return m_build.section('+', 0, 0);
}


int Version::m_version = 50;

#ifdef HGID
QString Version::m_build = HGID;
#else
QString Version::m_build = "?";
#endif
