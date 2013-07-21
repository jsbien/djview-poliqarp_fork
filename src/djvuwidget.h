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

#ifndef DJVUWIDGET_H
#define DJVUWIDGET_H

#include "djvulink.h"
#include "qdjvuwidget.h"
#include "qdjvunet.h"

class DjVuWidget : public QDjVuWidget
{
	Q_OBJECT
public:
	explicit DjVuWidget(QWidget *parent = 0);
	~DjVuWidget();
	DjVuLink link() const {return m_link;}
	void closeDocument();
public slots:
	void openLink(const DjVuLink& link);
	void openFile(const QString& filename);
private slots:
	void documentLoaded();
	/** Show context menu to operate on selection. */
	void regionSelected(const QPoint& point, const QRect& rect);
	/** Handle context menu action. */
	void regionAction(QAction* action);
signals:
	void loading(const DjVuLink& link);
	void loaded(const DjVuLink& link);
private:
	enum RegionAction {InvalidAction, CopyLink, CopyImage, SaveImage};
	/** Create menu action. */
	void createAction(RegionAction actionType, const QString& text);
	/** @return context used for all items. */
	QDjVuContext* context();
	QDjVuNetDocument* m_document;
	DjVuLink m_link;
	static QDjVuContext* m_context;

	QRect m_lastRegion;
	QMenu* m_regionMenu;
};

#endif // DJVUWIDGET_H
