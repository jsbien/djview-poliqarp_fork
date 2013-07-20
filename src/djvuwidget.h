/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
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
	enum RegionAction {InvalidAction, CopyImage, SaveImage};
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
