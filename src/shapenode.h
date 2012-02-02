#ifndef SHAPENODE_H
#define SHAPENODE_H

#include <QList>
#include <QPair>
#include <QPixmap>
#include <QColor>

class QShapeTableWidgetItem;

class ShapeNode
{
    int id;
    ShapeNode *parent;
    QPixmap pixmap;
    QList<ShapeNode *> children;
    QList<QPair<unsigned short, unsigned short> > blits;
    int to_root_height;
    int to_leaf_height;

    int caluculateTreeHeights(int from_root);

public:
    ShapeNode(ShapeNode *parent, int id, QPixmap pixmap);
    ShapeNode(int id, QPixmap pixmap) : id(id), parent(NULL), pixmap(pixmap), tbl_itm(NULL) {};
    ShapeNode(int id) : id(id), parent(NULL), tbl_itm(NULL) {};
    ~ShapeNode();

    int getId() { return id; };
    QPixmap& getPixmap() { return pixmap; };

    QList<ShapeNode *> getChildren() { return children; };
    QList<ShapeNode *> getSiblings();
    ShapeNode * getParent() { return parent; };

    QList<QPair<unsigned short, unsigned short> > getBlits() { return blits; };
    void addBlit(unsigned short left, unsigned short bottom);

    void setParent(ShapeNode *parent);

    void caluculateTreeHeights() { caluculateTreeHeights(0); }
    int getToRootHeight() { return to_root_height; }
    int getToLeafHeight() { return to_leaf_height; }

    QShapeTableWidgetItem * tbl_itm;
    QColor color;

    static bool greaterThan(ShapeNode * n1, ShapeNode *n2);
};

#endif // SHAPENODE_H
