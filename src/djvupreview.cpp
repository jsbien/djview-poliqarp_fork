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
	if (event->button() == Qt::MiddleButton || event->modifiers() & Qt::AltModifier) {
		event->ignore();
		QString cmd = QSettings().value("Tools/djviewPath", "djview").toString();
		QStringList args;
		args << link().link().toString();
		if (!QProcess::startDetached(cmd, args)) {
			QString msg = tr("Cannot execute program:") + "<br><i>%1</i>";
			MessageDialog::warning(msg.arg(cmd));
		}
	}
	else DjVuWidget::mousePressEvent(event);
}

