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
	/** Add custom action to context menu. */
	void addCustomAction(QAction *action);
	/** @return recently selected region. */
	QUrl lastSelection();
	/** @return last selected region. */
	QRect lastRegion() const {return m_lastRegion;}
public slots:
	void openLink(const DjVuLink& link);
	void openFile(const QString& filename);
private slots:
	void documentLoaded();
	/** Show context menu to operate on selection. */
	void regionSelected(const QPoint& point, const QRect& rect);
	/** Handle context menu action. */
	void regionAction(QAction* action);
	/** Reenable mouse after popup menu is closed. */
	void reenableMouse() {m_mouseGrabbed = false;}

protected:
	/** Update hidden text . */
	void mouseMoveEvent(QMouseEvent *event);
	/** Toggle hidden text. */
	void keyPressEvent(QKeyEvent *event);
	/** Toggle hidden text. */
	void keyReleaseEvent(QKeyEvent *event);

signals:
	void loading(const DjVuLink& link);
	void loaded(const DjVuLink& link);
private:
	enum RegionAction {InvalidAction, ZoomToRegion, CopyLink, CopyText, CopyImage, SaveImage};
	/** Create menu action. */
	QAction* createAction(RegionAction actionType, const QString& text);
	/** @return context used for all items. */
	QDjVuContext* context();
	/** Show or hidden text. */
	void showHiddenText(const QPoint& point);
	/** Hide hidden text. */
	void hideHiddenText();
	QSharedPointer<QDjVuNetDocument> m_document;
	DjVuLink m_link;
	static QDjVuContext* m_context;

	QRect m_lastRegion;
	QMenu* m_regionMenu;
	QAction* m_copyImageAction;
	QAction* m_copyTextAction;

	bool m_hiddenTextVisible;
	bool m_mouseGrabbed;
	/** Cache of currently (and previously) open documents */
	static QHash<QString, QWeakPointer<QDjVuNetDocument>> documents;
	/** Guard for documents */
	static QMutex documentsMutex;
};

#endif // DJVUWIDGET_H
