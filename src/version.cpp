/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QtCore>

#ifdef HGID
const char* HgVersion = HGID;
#else
const char HgVersion = "?";
#endif
