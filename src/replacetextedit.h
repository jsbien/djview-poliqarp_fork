/****************************************************************************
*   Copyright (C) 2018 by Michal Rudolf <michal@rudolf.waw.pl>              *
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

