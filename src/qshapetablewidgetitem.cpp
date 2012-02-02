#include "qshapetablewidgetitem.h"

QShapeTableWidgetItem::QShapeTableWidgetItem(ShapeNode *node) : QTableWidgetItem(), node(node)
{
    if (node == NULL) {
        setFlags(Qt::NoItemFlags);
        setText("NULL");
        setBackgroundColor(Qt::red);
    } else {
        node->tbl_itm = this;

        setFlags(Qt::ItemIsEnabled);

        setToolTip(QString("Shape %1, level %2").arg(node->getId()).arg(node->getToRootHeight()));
        setIcon(QIcon(node->getPixmap()));
    }
}
