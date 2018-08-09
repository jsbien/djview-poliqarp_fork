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
