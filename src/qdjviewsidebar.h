//C-  -*- C++ -*-
//C- -------------------------------------------------------------------
//C- DjView4
//C- Copyright (c) 2006-  Leon Bottou
//C-
//C- This software is subject to, and may be distributed under, the
//C- GNU General Public License, either version 2 of the license,
//C- or (at your option) any later version. The license should have
//C- accompanied the software or you may obtain a copy of the license
//C- from the Free Software Foundation at http://www.fsf.org .
//C-
//C- This program is distributed in the hope that it will be useful,
//C- but WITHOUT ANY WARRANTY; without even the implied warranty of
//C- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//C- GNU General Public License for more details.
//C-  ------------------------------------------------------------------

#ifndef QDJVIEWSIDEBAR_H
#define QDJVIEWSIDEBAR_H

#if AUTOCONF
# include "config.h"
#endif

#include <Qt>
#include <QObject>
#include <QModelIndex>
#include <QString>
#include <QUrl>
#include <QWidget>
#include <QList>
#include <QRadioButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDockWidget>

#include <libdjvu/miniexp.h>
#include <libdjvu/ddjvuapi.h>

#include "qdjvu.h"
#include "qdjvuwidget.h"
#include "qdjview.h"
#include "shapenode.h"
#include "qshapetablewidgetitem.h"

class QAction;
class QComboBox;
class QContextMenuEvent;
class QItemSelectionModel;
class QLabel;
class QLineEdit;
class QListView;
class QMenu;
class QPushButton;
class QStackedLayout;
class QToolButton;
class QTreeWidget;
class QTreeWidgetItem;
class QTableWidget;

// ----------------------------------------
// OUTLINE


class QDjViewOutline : public QWidget
{
  Q_OBJECT
public:
  QDjViewOutline(QDjView *djview);
public slots:
  void clear(); 
  void refresh(); 
protected slots:
  void pageChanged(int pageno);
  void itemActivated(QTreeWidgetItem *item);
private:
  QDjView *djview;
  QTreeWidget *tree;
  bool loaded;
  void fillItems(QTreeWidgetItem *item, miniexp_t expr);
  void searchItem(QTreeWidgetItem *item, int pageno, 
                  QTreeWidgetItem *&fi, int &fp);
};



// ----------------------------------------
// THUMBNAILS


class QDjViewThumbnails : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int size READ size WRITE setSize)
  Q_PROPERTY(bool smart READ smart WRITE setSmart)
public:
  QDjViewThumbnails(QDjView *djview);
  int size();
  bool smart();
public slots:
  void setSize(int);
  void setSmart(bool);
protected slots:
  void setSize();
  void pageChanged(int pageno);
  void activated(const QModelIndex &index);
protected:
  void contextMenuEvent(QContextMenuEvent *event);
  void updateActions();
private:
  class Model;
  class View;
  QDjView             *djview;
  Model               *model;
  View                *view;
  QItemSelectionModel *selection;
  QMenu               *menu;
};




// ----------------------------------------
// FIND


class QDjViewFind : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(bool caseSensitive READ caseSensitive WRITE setCaseSensitive)
  Q_PROPERTY(bool wordOnly READ wordOnly WRITE setWordOnly)
public:
  QDjViewFind(QDjView *djview);
  void takeFocus(Qt::FocusReason);
  QString text();
  bool caseSensitive();
  bool wordOnly();
  bool regExpMode();
public slots:
  void selectAll();
  void eraseText();
  void setText(QString s);
  void setCaseSensitive(bool);
  void setWordOnly(bool);
  void setRegExpMode(bool);
  void findNext();
  void findPrev();
  void findAgain();
protected slots:
  void pageChanged(int);
protected:
  void contextMenuEvent(QContextMenuEvent *event);
private:
  class Model;
  QDjView        *djview;
  Model          *model;
  QListView      *view;
  QMenu          *menu;
  QComboBox      *combo;
  QToolButton    *upButton;
  QToolButton    *downButton;
  QToolButton    *resetButton;
  QStackedLayout *stack;
  QLabel         *label;
  QAction        *caseSensitiveAction;
  QAction        *wordOnlyAction;
  QAction        *regExpModeAction;
};




// ----------------------------------------
// SHAPES


class QDjViewShapes : public QWidget
{
    Q_OBJECT
  public:
    QDjViewShapes(QDjView *djview, QDjViewSamplesArea *samples);
  public slots:
    void clear();
    void refresh();
    void documentReady(QDjVuDocument * doc);
    void shapeClicked (QTableWidgetItem * item, bool scrollTo=true);
    void showContextMenu(QPoint point);
  protected slots:
    void pageChanged(int pageno);
    void itemActivated(QTreeWidgetItem *item);
    void colorFactorChanged(int);
    void pointerClick(const QDjVuWidget::Position &pos);
  private:
    QDjView *djview;
    QDjViewSamplesArea *samples;
    //QTreeWidget *tree;
    QVector<QPair<int, QList<ShapeNode *> > > page_root_shapes; // count and actual shapes
    QList<QShapeTableWidgetItem *> highlighed_shapes;
    QHash<int, QShapeTableWidgetItem *> displayed_shapes; //id -> table cell

    QVBoxLayout *layout;

    QTableWidget *table;
    QLabel *noShapesFound;
    QProgressBar *progress;
    QLabel *factor_label;
    QSpinBox *color_factor;

    QMenu *shapeContextMenu;
    QAction *exportShapeAction;
    QAction *copyURLAction;
    QAction *exportShapeAndUrlAction;
    QAction *topDownAction;
    QAction *leftRightAction;
    QAction *findShapeAction;

    enum {TOP_DOWN_ORDER, LEFT_RIGHT_ORDER} treeOrder;

    //bool loaded;
    /*void fillItems(QTreeWidgetItem *item, miniexp_t expr);
    void searchItem(QTreeWidgetItem *item, int pageno,
                    QTreeWidgetItem *&fi, int &fp);*/
    void fillPageGlyph(int pageno);

    void highlight(QList<ShapeNode *> nodes, QColor color);
    void highlightOne(ShapeNode * node, QColor color);
    void highlightParent(ShapeNode * parent, ShapeNode * node, QColor parentColor, QColor uncleColor, bool colorSiblings = true);
    void highlightChildren(ShapeNode * node, QColor color);

    void refreshShapeTree();

    QUrl shape_URL(ShapeNode *node);

    void lookupShapesInTree(ShapeNode * node, int x, int y, QList<ShapeNode *> &found);

    class QShapeSelectionDialog : public QDialog {
        QHash<QWidget *, ShapeNode *> labels_shapes;
        ShapeNode * selected;

    public:
        QShapeSelectionDialog(QList<ShapeNode *> &shapes, QWidget *parent = NULL);
        ~QShapeSelectionDialog();

        ShapeNode * selectShape();

    protected:
        virtual void mousePressEvent(QMouseEvent * event);
        virtual void mouseReleaseEvent(QMouseEvent * event);
    };

  protected:
    virtual void resizeEvent(QResizeEvent * event);
};




// ----------------------------------------
// SAMPLES

/* this is just one sample */
class QDjVuWidgetSample : public QWidget {
    Q_OBJECT
  public:
    QDjVuWidgetSample(QDjVuWidget *widget, int highlightOnPage, QRect toHighlight, QColor color, QDjVuWidget *main_widget = NULL, QWidget *parent = NULL);
    ~QDjVuWidgetSample();

    bool isPined();

  public slots:
    void preciseClick(const QDjVuWidget::Position &pos);

  private:
    QHBoxLayout *layout;

    QDjVuWidget *widget;
    QDjVuWidget *main_widget; // document widget
    QCheckBox *check;

    int highlightOnPage;
    QRect highlight;
    QColor highlightColor;
};

// this seems to bo whole list
class QDjVuWidgetSamples : public QWidget
{
  private:
    QList<QDjVuWidgetSample *> samples;
    QVBoxLayout * samplesLayout;

  public:
    QDjVuWidgetSamples(QWidget *parent = 0);
    void clearUnanchored();
    void addQDjVuWidget(QDjVuWidget *widget, int highlightOnPage, QRect toHighlight, QColor color, QDjVuWidget *main_widget);

/*  protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    virtual void paintEvent (QPaintEvent * event);*/
};


//this seems to be some kind of container with scroll area?
class QDjViewSamplesArea : public QWidget
{
    Q_OBJECT
  public:
    QDjViewSamplesArea(QDjView *djview);

    void clearUnanchored();
    void addQDjVuWidget(QDjVuWidget *widget, int highlightOnPage, QRect toHighlight, QColor color, QDjVuWidget *main_widget);

  public slots:

  protected slots:

  private:
    QDjView *djview;

    QDjVuWidgetSamples *samples;

    QVBoxLayout * layout;

  protected:
//    virtual void resizeEvent(QResizeEvent * event);
};




// ----------------------------------------
// TEXT


class QDjViewText : public QWidget
{
    Q_OBJECT
  public:
    QDjViewText(QDjView *djview);

    QRect   textDockRect;

  protected slots:
    //void updateTextLabelForPosition(const QDjVuWidget::Position &pos);
    void updateTextLabel();

  private:
    QDjView *djview;

    QVBoxLayout *layout;

    QLabel *textLabel;
};



// ----------------------------------------
// POLIQARP


class QDjViewPoliqarp : public QWidget
{
    Q_OBJECT
  public:
    QDjViewPoliqarp(QDjView *djview);

  protected slots:
    void goURL();
    void parseVolumeReply(QNetworkReply *reply);
    void changeVolume(int index);
    void parseQueryReply(QNetworkReply *reply);
    void goQuery();
    void getPendingResults();
    void nextResults();
    void prevResults();

  private:
    QDjView *djview;

    QVBoxLayout *layout;

    QHBoxLayout *URLLayout;
    QHBoxLayout *queryLayout;
    QHBoxLayout *navigateButtonsLayout;

    QLineEdit *poliqarpURL;
    QPushButton *goURLButton;
    QComboBox *poliqarpVolume;
    QLineEdit *poliqarpQuery;
    QPushButton *goQueryButton;

    QPushButton *nextButton;
    QPushButton *prevButton;

    QPlainTextEdit *textEdit;

    QNetworkAccessManager *manager;
    QNetworkAccessManager *queryManager;

    QDjViewSamplesArea *samples;

    QLabel *poliqarpClientStatus;

    QUrl bareQueryExecuted;
    bool volumeOpened;
    int firstShown;

    //enum { PQRP_MAINPAGE, PQRP_QUERYPAGE, PQRP_RESULT } replyExpected;
};

#endif
/* -------------------------------------------------------------
   Local Variables:
   c++-font-lock-extra-types: ( "\\sw+_t" "[A-Z]\\sw*[a-z]\\sw*" )
   End:
   ------------------------------------------------------------- */
