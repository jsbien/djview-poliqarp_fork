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

ReplaceLineEdit::~ReplaceLineEdit()
{
}

void ReplaceLineEdit::configure()
{
	m_replacements.clear();
	QString source = QSettings().value("Edit/replace").toString();
	for (const QString& line: source.split('\n', QString::SkipEmptyParts)) {
		Replacement replacement(line);
		if (replacement.isValid())
			m_replacements.append(replacement);
	}
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

ReplaceLineEdit::Replacement ReplaceLineEdit::findReplacement(const QString& suffix) const
{
	QString leftText = text().left(cursorPosition());
	for (const Replacement& r: m_replacements) {
		if (r.from.endsWith(suffix) && (leftText + suffix).endsWith(r.from))
			return r;
	}
	return Replacement();
}

ReplaceLineEdit::Replacement::Replacement(const QString& line)
{
	from = line.section('=', 0, 0);
	to = line.section('=', 1).trimmed();
}

bool ReplaceLineEdit::Replacement::isValid() const
{
	return !from.isEmpty() && !to.isEmpty();
}


QString ReplaceLineEdit::Replacement::expand(const QString& previous, int cursor, const QString& suffix) const
{
	return previous.left(cursor - (from.length() - suffix.length())) + to + previous.mid(cursor);
}
