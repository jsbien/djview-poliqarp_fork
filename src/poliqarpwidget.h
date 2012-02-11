/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#ifndef POLIQARPWIDGET_H
#define POLIQARPWIDGET_H

#include "ui_poliqarpwidget.h"

class PoliqarpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PoliqarpWidget(QWidget *parent = 0);

private:
    Ui::PoliqarpWidget ui;
};

#endif // POLIQARPWIDGET_H
