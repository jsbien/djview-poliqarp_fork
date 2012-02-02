#include "shapenode.h"

ShapeNode::ShapeNode(ShapeNode *parent, int id, QPixmap pixmap) : id(id), pixmap(pixmap), tbl_itm(NULL)
{
    setParent(parent);
}

void ShapeNode::setParent(ShapeNode *parent)
{
    this->parent = parent;

    if (parent)
        parent->children.append(this);
}

ShapeNode::~ShapeNode()
{
    qDeleteAll(children);
    //while (!children.isEmpty())
    //     delete children.takeFirst();
}

QList<ShapeNode *> ShapeNode::getSiblings()
{
    if (parent == NULL)
        return QList<ShapeNode *>();
    else {
        QList<ShapeNode *> ret = QList<ShapeNode *>(parent->getChildren());
        ret.removeOne(this);
        return ret;
    }
}

int ShapeNode::caluculateTreeHeights(int from_root)
{
    to_root_height = from_root;
    to_leaf_height = 0;
    for (QList<ShapeNode *>::iterator i = children.begin(); i != children.end(); ++i) {
        int to_leaf = (*i)->caluculateTreeHeights(from_root + 1);
        to_leaf_height = to_leaf > to_leaf_height ? to_leaf : to_leaf_height;
    }

    return to_leaf_height + 1;
}

void ShapeNode::addBlit(unsigned short left, unsigned short bottom)
{
    blits.append(qMakePair(left, bottom));
}

bool ShapeNode::greaterThan(ShapeNode * n1, ShapeNode *n2)
{
    if (!n1 || !n2)
        return false;

    return n1->getPixmap().size().width() > n2->getPixmap().size().width();
}
