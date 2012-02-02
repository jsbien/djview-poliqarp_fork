#ifndef QSHAPETABLEWIDGETITEM_H
#define QSHAPETABLEWIDGETITEM_H

#include <QTableWidgetItem>
#include "shapenode.h"

class QShapeTableWidgetItem : public QTableWidgetItem
{
public:
    QShapeTableWidgetItem(ShapeNode *node);
    ShapeNode *node;
};

#endif // QSHAPETABLEWIDGETITEM_H
