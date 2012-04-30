/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QtGui>
#include "djvupreview.h"
#include "messagedialog.h"

DjVuPreview::DjVuPreview(QWidget *parent) :
	DjVuWidget(parent)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	viewport()->installEventFilter(this);
	setFocusPolicy(Qt::StrongFocus);

	configure();
}

void DjVuPreview::configure()
{
	QSettings settings;
	int height = settings.value("Display/previewHeight", 40).toInt();
	setMaximumHeight(height);
}

void DjVuPreview::focusInEvent(QFocusEvent* event)
{
	if (event->reason() == Qt::TabFocusReason || event->reason() == Qt::MouseFocusReason)
		emit activated();
}

void DjVuPreview::mouseDoubleClickEvent(QMouseEvent*)
{
	if (link().isValid())
		emit documentRequested(link());
}

void DjVuPreview::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MiddleButton) {
		event->ignore();
		QString cmd = QSettings().value("Tools/djviewPath", "djview").toString();
		QStringList args;
		args << link().link().toString();
		qDebug() << args;
		if (!QProcess::startDetached(cmd, args)) {
			QString msg = tr("Cannot execute program:") + "<br><i>%1</i>";
			MessageDialog::warning(msg.arg(cmd));
		}
	}
	else DjVuWidget::mousePressEvent(event);
}

