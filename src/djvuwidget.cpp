/****************************************************************************
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

#include "djvuwidget.h"
#include "messagedialog.h"
#include <libdjvu/miniexp.h>
#include <libdjvu/ddjvuapi.h>

DjVuWidget::DjVuWidget(QWidget *parent) :
	QDjVuWidget(parent)
{
	connect(this, SIGNAL(pointerSelect(QPoint,QRect)), this,
			  SLOT(regionSelected(QPoint,QRect)));

	m_regionMenu = new QMenu(this);
	connect(m_regionMenu, SIGNAL(triggered(QAction*)), this, SLOT(regionAction(QAction*)));

	createAction(ZoomToRegion, tr("Zoom to selection"));
	QAction* separatorAction = new QAction(this);
	separatorAction->setSeparator(true);

	createAction(CopyLink, tr("Copy link"));
	m_copyTextAction = createAction(CopyText, QString());
	m_copyImageAction = createAction(CopyImage, QString());
	createAction(SaveImage, tr("Save image..."));

	separatorAction = new QAction(this);
	separatorAction->setSeparator(true);
	m_regionMenu->addAction(separatorAction);

	m_hiddenTextVisible = false;
	m_mouseGrabbed = false;
}

DjVuWidget::~DjVuWidget()
{
}

void DjVuWidget::openLink(const DjVuLink &link)
{
	QDjVuWidget::clearHighlights(m_link.page());
	if (!link.isValid()) {
		closeDocument();
		return;
	}

	closeDocument();
	// Retrieve document from cache or create new if needed
	{
		QMutexLocker lock(&m_documentsLock);

		m_link = link;
		m_document = m_documentCache.value(link.downloadLink().toString());
		if (m_document.isNull()) {
			// Failed to reuse document (either never existed or was already freed)
			m_document.reset(new QDjVuNetDocument());
			m_document->setUrl(context(), m_link.downloadLink(), true);
			m_documentCache.insert(link.downloadLink().toString(), m_document);
		}
	}
	emit loading(m_link);
	connect(m_document.data(), SIGNAL(docinfo()), this, SLOT(documentLoaded()), Qt::QueuedConnection);
	documentLoaded(); // cached document might have been loaded already
}

void DjVuWidget::openFile(const QString &filename)
{
	closeDocument();
	if (QFile(filename).exists()) {
		m_document.reset(new QDjVuNetDocument());

		connect(m_document.data(), SIGNAL(docinfo()), this, SLOT(documentLoaded()));
		m_document->setFileName(context(), filename);
	}
}

void DjVuWidget::clearHighlights()
{
	// We can't currently enumerate highlights. addHighlight can be called by anyone and we can't track it.
	// Solution:
	//  * remove any created from link - they must be on its page
	//  * clear current page, so user doesn't think nothing has happened
	//  * leave any others alone

	// This heurestic requires page to be loaded
	if (m_link.isValid() && m_link.page() >= 0) {
		QDjVuWidget::clearHighlights(m_link.page());
		QDjVuWidget::clearHighlights(page());
	}

}

void DjVuWidget::closeDocument()
{
	setDocument(nullptr);
	m_link = QUrl();
}

void DjVuWidget::addCustomAction(QAction *action)
{
	m_regionMenu->addAction(action);
}

QUrl DjVuWidget::lastSelection()
{
	if (m_lastRegion.width() == 0)
		return QUrl();
	else return m_link.regionLink(getSegmentForRect(m_lastRegion, page()), page());
}

void DjVuWidget::documentLoaded()
{
	// One spurious wake-up is allowed, IF connected with Qt::QueuedConnection
	// (to ensure that we can disconnect if the first call succeeds)
	if (ddjvu_document_decoding_done(*m_document)) {
		// Only one call can succeed
		disconnect(m_document.data(), SIGNAL(docinfo()), this, SLOT(documentLoaded()));
		setDocument(m_document.data());

		if (!m_link.isValid())
			return;

		// Retrieve page information
		const int numPages = ddjvu_document_get_pagenum(*document());
		QList<ddjvu_fileinfo_t> documentPages;

		int m = ddjvu_document_get_filenum(*document());
		for (int i = 0; i < m; i++)
		{
			ddjvu_fileinfo_t info;
			if (ddjvu_document_get_fileinfo(*document(), i, &info) != DDJVU_JOB_OK)
				qWarning("Internal(docinfo): ddjvu_document_get_fileinfo fails.");
			if (info.type == 'P')
				documentPages << info;
		}
		if (documentPages.size() != numPages)
			qWarning("Internal(docinfo): inconsistent number of pages.");

		QDjVuWidget::Position pos;
		if (m_link.page() < 0) {
			// Page can be deducted only after loading entire document
			// due to existence of page ids
			QString pageName = m_link.pageId();
			m_link.setPage(pageName, documentPages);
		}

		pos.pageNo = m_link.page();
		pos.inPage = true;
		if (!m_link.highlights().isEmpty())
			pos.posPage = m_link.highlights().first().rect.topLeft();
		else pos.posPage = m_link.position();

		setPosition(pos, QPoint(width() / 2, height() / 2));
		foreach(const DjVuLink::Highlight& h, m_link.highlights()) {
			addHighlight(m_link.page(), h.rect.left(), h.rect.top(),
				h.rect.width(), h.rect.height(), h.color);
		}
		emit loaded(m_link);
	}
}

void DjVuWidget::regionSelected(const QPoint &point, const QRect &rect)
{
	m_lastRegion = rect;
	int textLength = getTextForRect(m_lastRegion).count();
	m_copyTextAction->setText(tr("Copy text (%1 characters)").arg(textLength));
	m_copyTextAction->setVisible(textLength > 0);

	QSize imageSize = getImageForRect(m_lastRegion).size();
	m_copyImageAction->setText(tr("Copy image (%1x%2 pixels)")
										.arg(imageSize.width()).arg(imageSize.height()));

	m_mouseGrabbed = true;
	m_regionMenu->exec(point);
	// Workaround for problems with mouse events. See bug #125.
	QTimer::singleShot(100, this, SLOT(reenableMouse()));
}

void DjVuWidget::regionAction(QAction *action)
{
	if (m_lastRegion.width() == 0)
		return;

	switch (RegionAction(action->data().toInt())) {
	case InvalidAction:
		break;
	case ZoomToRegion:
		zoomRect(m_lastRegion);
		break;
	case CopyLink:
		QApplication::clipboard()->setText(m_link.colorRegionLink(getSegmentForRect(m_lastRegion, page()), page()).toString());
		break;
	case CopyText:
		QApplication::clipboard()->setText(getTextForRect(m_lastRegion));
		break;
	case CopyImage:
		QApplication::clipboard()->setImage(getImageForRect(m_lastRegion));
		break;
	case SaveImage:
		MessageDialog::saveImage(getImageForRect(m_lastRegion));
		break;
	}
}

void DjVuWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (m_mouseGrabbed) {
		event->accept();
		return;
	}

	if (event->modifiers() & Qt::ShiftModifier)
		showHiddenText(event->globalPos());
	else hideHiddenText();
	QDjVuWidget::mouseMoveEvent(event);
}

void DjVuWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->modifiers() & Qt::ShiftModifier)
		showHiddenText(cursor().pos());
	else hideHiddenText();
	QDjVuWidget::keyPressEvent(event);
}

void DjVuWidget::keyReleaseEvent(QKeyEvent *event)
{
	if (event->modifiers() & Qt::ShiftModifier)
		showHiddenText(cursor().pos());
	else hideHiddenText();
	QDjVuWidget::keyReleaseEvent(event);
}

QAction* DjVuWidget::createAction(DjVuWidget::RegionAction actionType, const QString &text)
{
	QAction* action = new QAction(text, this);
	action->setData(actionType);
	m_regionMenu->addAction(action);
	return action;
}

QDjVuContext *DjVuWidget::context()
{
	if (!m_context)
		m_context = new QDjVuContext("djview-poliqarp", QApplication::instance());
	return m_context;
}

void DjVuWidget::showHiddenText(const QPoint &point)
{
	QString texts[3];
	if (getTextForPointer(texts)) {
		m_hiddenTextVisible = true;
		QString text = texts[0] + texts[1] + texts[2];
		QToolTip::showText(point, text, this);
	}
}

void DjVuWidget::hideHiddenText()
{
	if (m_hiddenTextVisible) {
		m_hiddenTextVisible = false;
		QToolTip::hideText();
	}
}

QDjVuContext* DjVuWidget::m_context = 0;
QHash<QString, QWeakPointer<QDjVuNetDocument>> DjVuWidget::m_documentCache;
QMutex DjVuWidget::m_documentsLock;
