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

#pragma once
#include "replacements.h"
#include <QPlainTextEdit>

class ReplaceTextEdit : public QPlainTextEdit
{
public:
   ReplaceTextEdit(QWidget* parent = nullptr);
   ~ReplaceTextEdit() = default;
   void configure();
protected:
   void keyPressEvent(QKeyEvent* event);

   /** @return first matching replacement for given suffix or empty replacement if there is none. */
   Replacement findReplacement(const QString& suffix) const;
   Replacements m_replacements;
};

