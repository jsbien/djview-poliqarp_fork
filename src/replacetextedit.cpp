/****************************************************************************
*   Copyright (C) 2018 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include <QSettings>
#include "replacetextedit.h"


ReplaceTextEdit::ReplaceTextEdit(QWidget* parent) : QPlainTextEdit(parent)
{
   configure();
}

void ReplaceTextEdit::configure()
{
   m_replacements.read(QSettings().value("Edit/replace").toString());
}

void ReplaceTextEdit::keyPressEvent(QKeyEvent* event)
{
   QString suffix = event->text();
   if (!suffix.isEmpty()) {
      int position = textCursor().position();
      Replacement r = findReplacement(suffix);
      if (r.isValid()) {
         setPlainText(r.expand(toPlainText(), position, suffix));
         event->accept();
         return;
      }
   }
   QPlainTextEdit::keyPressEvent(event);
}

Replacement ReplaceTextEdit::findReplacement(const QString& suffix) const
{
   int position = textCursor().position();
   QString leftText = toPlainText().left(position);
   for (const Replacement& r: m_replacements) {
      if (r.from.endsWith(suffix) && (leftText + suffix).endsWith(r.from))
         return r;
   }
   return Replacement();
}
