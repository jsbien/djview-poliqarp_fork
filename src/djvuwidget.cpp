/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "djvuwidget.h"
#include "messagedialog.h"

DjVuWidget::DjVuWidget(QWidget *parent) :
	QDjVuWidget(parent)
{
	m_document = new QDjVuNetDocument(this);
	connect(m_document, SIGNAL(docinfo()), this, SLOT(documentLoaded()));

	connect(this, SIGNAL(pointerSelect(QPoint,QRect)), this,
			  SLOT(regionSelected(QPoint,QRect)));

	m_regionMenu = new QMenu(this);
	connect(m_regionMenu, SIGNAL(triggered(QAction*)), this, SLOT(regionAction(QAction*)));

	createAction(CopyImage, tr("Copy image"));
	createAction(SaveImage, tr("Save image..."));
}

DjVuWidget::~DjVuWidget()
{
}

void DjVuWidget::openLink(const DjVuLink &link)
{
	if (m_link.isValid())
		clearHighlights(m_link.page());
	m_link = link;
	if (m_link.isValid()) {
		m_document->setUrl(context(), m_link.link());
		emit loading(m_link);
	}
	else closeDocument();
}

void DjVuWidget::openFile(const QString &filename)
{
	closeDocument();
	if (QFile(filename).exists())
		m_document->setFileName(context(), filename);
}

void DjVuWidget::closeDocument()
{
	setDocument(0);
	m_link = QUrl();
}

void DjVuWidget::documentLoaded()
{
	setDocument(m_document);

	if (!m_link.isValid())
		return;
	QDjVuWidget::Position pos;
	pos.pageNo = m_link.page();
	pos.inPage = true;
	pos.posPage = m_link.highlighted().topLeft();

	setPosition(pos, QPoint(width() / 2, height() / 2));

	QSettings settings;
	QColor color(settings.value("Display/highlight", "#ffff00").toString());
	color.setAlpha(96);
	addHighlight(m_link.page(), m_link.highlighted().left(),
					 m_link.highlighted().top(),
					 m_link.highlighted().width(),
					 m_link.highlighted().height(), color);

	emit loaded(m_link);
}

void DjVuWidget::regionSelected(const QPoint &point, const QRect &rect)
{
	m_lastRegion = rect;
	m_regionMenu->exec(point);
}

void DjVuWidget::regionAction(QAction *action)
{
	if (m_lastRegion.width() == 0)
		return;

	switch (RegionAction(action->data().toInt())) {
	case InvalidAction:
		break;
	case CopyImage:
		QApplication::clipboard()->setImage(getImageForRect(m_lastRegion));
		break;
	case SaveImage:
		MessageDialog::saveImage(getImageForRect(m_lastRegion));
		break;
	}
}

void DjVuWidget::createAction(DjVuWidget::RegionAction actionType, const QString &text)
{
	QAction* action = new QAction(text, this);
	action->setData(actionType);
	m_regionMenu->addAction(action);
}

QDjVuContext *DjVuWidget::context()
{
	if (!m_context)
		m_context = new QDjVuContext("djview-poliqarp", QApplication::instance());
	return m_context;
}

QDjVuContext* DjVuWidget::m_context = 0;
