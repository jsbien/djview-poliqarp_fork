/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QKeyEvent>
#include <QSettings>
#include "replacelineedit.h"

ReplaceLineEdit::ReplaceLineEdit(QWidget* parent) : QLineEdit(parent)
{
	configure();
}

void ReplaceLineEdit::configure()
{
	m_replacements.read(QSettings().value("Edit/replace").toString());
}

void ReplaceLineEdit::keyPressEvent(QKeyEvent* event)
{
	QString suffix = event->text();
	if (!suffix.isEmpty()) {
		Replacement r = findReplacement(suffix);
		if (r.isValid()) {
			setText(r.expand(text(), cursorPosition(), suffix));
			event->accept();
			return;
		}
	}
	QLineEdit::keyPressEvent(event);
}

Replacement ReplaceLineEdit::findReplacement(const QString& suffix) const
{
	QString leftText = text().left(cursorPosition());
	for (const Replacement& r: m_replacements) {
		if (r.from.endsWith(suffix) && (leftText + suffix).endsWith(r.from))
			return r;
	}
	return Replacement();
}
