/****************************************************************************
*   Copyright (C) 2012-2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
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
