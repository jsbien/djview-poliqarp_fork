/****************************************************************************
*   Copyright (C) 2012-2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
*   This software is subject to, and may be distributed under, the
*   GNU General Public License, either version 2 of the license,
*   or (at your option) any later version. The license should have
*   accompanied the software or you may obtain a copy of the license
*   from the Free Software Foundation at http://www.fsf.org .

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
****************************************************************************/

#include "version.h"


int Version::version()
{
	return m_version;
}

QString Version::versionText()
{
	return QString("%1.%2").arg(m_version / 10)
			.arg(m_version % 10);
}

int Version::buildNumber()
{
	return buildText().toInt();
}

QString Version::buildText()
{
	return m_build.section('+', 0, 0);
}


int Version::m_version = 20;

#ifdef HGID
QString Version::m_build = HGID;
#else
QString Version::m_build = "?";
#endif
