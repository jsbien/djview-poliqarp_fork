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

#if AUTOCONF
# include "config.h"
#endif

#include "stdlib.h"
#include "math.h"

#include <QAbstractItemDelegate>
#include <QAbstractListModel>
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QCheckBox>
#include <QDebug>
#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QStack>
#include <QListView>
#include <QMap>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QPixmap>
#include <QBitmap>
#include <QRegExp>
#include <QResizeEvent>
#include <QStackedLayout>
#include <QStringList>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QTableWidget>
#include <QVariant>
#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include <QGroupBox>
#include <QProgressBar>
#include <QFileDialog>
#include <QInputDialog>
#include <QClipboard>
#include <QtXml/QDomDocument>

#include "config.h"
#include <libdjvu/DjVuImage.h>
#include <libdjvu/DjVuDocument.h>
#include <libdjvu/JB2Image.h>
#include <libdjvu/ByteStream.h>
#include <libdjvu/GBitmap.h>
#include <libdjvu/ddjvuapi.h>
#include <libdjvu/miniexp.h>

#include "qdjvu.h"
#include "qdjvuwidget.h"
#include "qdjviewsidebar.h"
#include "qdjview.h"
#include "shapenode.h"
#include "qshapetablewidgetitem.h"



// =======================================
// QDJVIEWOUTLINE
// =======================================




QDjViewOutline::QDjViewOutline(QDjView *djview)
	: QWidget(djview),
	  djview(djview),
	  loaded(false)
{
	tree = new QTreeWidget(this);
	tree->setColumnCount(1);
	tree->setItemsExpandable(true);
	tree->setUniformRowHeights(true);
	tree->header()->hide();
	tree->header()->setStretchLastSection(true);
	tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree->setSelectionMode(QAbstractItemView::SingleSelection);
	tree->setTextElideMode(Qt::ElideRight);
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(tree);

	connect(tree, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
			  this, SLOT(itemActivated(QTreeWidgetItem*)) );
	connect(djview, SIGNAL(documentClosed(QDjVuDocument*)),
			  this, SLOT(clear()) );
	connect(djview, SIGNAL(documentOpened(QDjVuDocument*)),
			  this, SLOT(clear()) );
	connect(djview, SIGNAL(documentReady(QDjVuDocument*)),
			  this, SLOT(refresh()) );
	connect(djview->getDjVuWidget(), SIGNAL(pageChanged(int)),
			  this, SLOT(pageChanged(int)) );
	connect(djview->getDjVuWidget(), SIGNAL(layoutChanged()),
			  this, SLOT(refresh()) );

	setWhatsThis(tr("<html><b>Document outline.</b><br/> "
						 "This panel display the document outline, "
						 "or the page names when the outline is not available, "
						 "Double-click any entry to jump to the selected page."
						 "</html>"));

	if (djview->pageNum() > 0)
		refresh();
}


void
QDjViewOutline::clear()
{
	tree->clear();
	loaded = false;
}

void
QDjViewOutline::refresh()
{
	QDjVuDocument *doc = djview->getDocument();
	if (doc && !loaded && djview->pageNum()>0)
	{
		miniexp_t outline = doc->getDocumentOutline();
		if (outline == miniexp_dummy)
			return;
		loaded = true;
		if (outline)
		{
			if (!miniexp_consp(outline) ||
				 miniexp_car(outline) != miniexp_symbol("bookmarks"))
			{
				QString msg = tr("Outline data is corrupted");
				qWarning("%s", (const char*)msg.toLocal8Bit());
			}
			tree->clear();
			QTreeWidgetItem *root = new QTreeWidgetItem();
			fillItems(root, miniexp_cdr(outline));
			QTreeWidgetItem *item = root;
			while (item->childCount()==1 &&
					 item->data(0,Qt::UserRole).toInt() >= 0)
				item = item->child(0);
			while (item->childCount() > 0)
				tree->insertTopLevelItem(tree->topLevelItemCount(),
												 item->takeChild(0) );
			delete root;
		}
		else
		{
			tree->clear();
			QTreeWidgetItem *root = new QTreeWidgetItem(tree);
			root->setText(0, tr("Pages"));
			root->setFlags(Qt::ItemIsEnabled);
			root->setData(0, Qt::UserRole, -1);
			for (int pageno=0; pageno<djview->pageNum(); pageno++)
			{
				QTreeWidgetItem *item = new QTreeWidgetItem(root);
				QString name = djview->pageName(pageno);
				item->setText(0, tr("Page %1").arg(name));
				item->setData(0, Qt::UserRole, pageno);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				item->setToolTip(0, tr("Go to page %1").arg(name));
				item->setWhatsThis(0, whatsThis());
			}
			tree->setItemExpanded(root, true);
		}
		pageChanged(djview->getDjVuWidget()->page());
	}
}


void
QDjViewOutline::fillItems(QTreeWidgetItem *root, miniexp_t expr)
{
	while(miniexp_consp(expr))
	{
		miniexp_t s = miniexp_car(expr);
		expr = miniexp_cdr(expr);
		if (miniexp_consp(s) &&
			 miniexp_consp(miniexp_cdr(s)) &&
			 miniexp_stringp(miniexp_car(s)) &&
			 miniexp_stringp(miniexp_cadr(s)) )
		{
			// fill item
			const char *name = miniexp_to_str(miniexp_car(s));
			const char *link = miniexp_to_str(miniexp_cadr(s));
			int pageno = -1;
			if (link && link[0]=='#')
				pageno = djview->pageNumber(QString::fromUtf8(link+1));
			QTreeWidgetItem *item = new QTreeWidgetItem(root);
			if (name && name[0])
				item->setText(0, QString::fromUtf8(name));
			else if (pageno >= 0)
				item->setText(0, tr("Page %1").arg(djview->pageName(pageno)));
			item->setFlags(0);
			item->setWhatsThis(0, whatsThis());
			if (link && link[0]=='#' && pageno>=0)
			{
				item->setData(0, Qt::UserRole, pageno);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				QString pagename = djview->pageName(pageno);
				item->setToolTip(0, tr("Go to page %1").arg(pagename));
			}
			else if (link && link[0])
			{
				item->setData(0, Qt::UserRole, QString::fromUtf8(link));
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				item->setToolTip(0, QString::fromUtf8(link));
			}
			// recurse
			fillItems(item, miniexp_cddr(s));
		}
	}
}


void
QDjViewOutline::pageChanged(int pageno)
{
	int fp = -1;
	QTreeWidgetItem *fi = 0;
	// find current selection
	QList<QTreeWidgetItem*> sel = tree->selectedItems();
	QTreeWidgetItem *si = 0;
	if (sel.size() == 1)
		si = sel[0];
	// current selection has priority
	if (si)
		searchItem(si, pageno, fi, fp);
	// search
	for (int i=0; i<tree->topLevelItemCount(); i++)
		searchItem(tree->topLevelItem(i), pageno, fi, fp);
	// select
	if (si && fi && si != fi)
		tree->setItemSelected(si, false);
	if (fi && si != fi)
	{
		tree->setCurrentItem(fi);
		tree->setItemSelected(fi, true);
		tree->scrollToItem(fi);
	}
}


void
QDjViewOutline::searchItem(QTreeWidgetItem *item, int pageno,
									QTreeWidgetItem *&fi, int &fp)
{
	QVariant data = item->data(0, Qt::UserRole);
	if (data.type() == QVariant::Int)
	{
		int page = data.toInt();
		if (page>=0 && page<=pageno && page>fp)
		{
			fi = item;
			fp = page;
		}
	}
	for (int i=0; i<item->childCount(); i++)
		searchItem(item->child(i), pageno, fi, fp);
}


void
QDjViewOutline::itemActivated(QTreeWidgetItem *item)
{
	QVariant data = item->data(0, Qt::UserRole);
	if (data.type() == QVariant::Int)
	{
		int page = data.toInt();
		if (page >= 0 && page < djview->pageNum())
			djview->goToPage(page);
	}
	else if (data.type() == QVariant::String)
	{
		QString link = data.toString();
		if (link.size() > 0)
			djview->goToLink(link);
	}
}







// =======================================
// QDJVIEWTHUMBNAILS
// =======================================




class QDjViewThumbnails::View : public QListView
{
	Q_OBJECT
public:
	View(QDjViewThumbnails *widget);
protected:
	QStyleOptionViewItem viewOptions() const;
private:
	QDjViewThumbnails *widget;
	QDjView *djview;
};


class QDjViewThumbnails::Model : public QAbstractListModel
{
	Q_OBJECT
public:
	~Model();
	Model(QDjViewThumbnails*);
	virtual int rowCount(const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	int getSize() { return size; }
	int getSmart() { return smart; }
public slots:
	void setSize(int);
	void setSmart(bool);
	void scheduleRefresh();
protected slots:
	void documentClosed(QDjVuDocument *doc);
	void documentReady(QDjVuDocument *doc);
	void thumbnail(int);
	void refresh();
private:
	QDjView *djview;
	QDjViewThumbnails *widget;
	QStringList names;
	ddjvu_format_t *format;
	QIcon icon;
	int size;
	bool smart;
	bool refreshScheduled;
	int  pageInProgress;
	QIcon makeIcon(int pageno) const;
	QSize makeHint(int pageno) const;
};



// ----------------------------------------
// QDJVIEWTHUMBNAILS::VIEW


QDjViewThumbnails::View::View(QDjViewThumbnails *widget)
	: QListView(widget),
	  widget(widget),
	  djview(widget->djview)
{
	setDragEnabled(false);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setTextElideMode(Qt::ElideRight);
	setViewMode(QListView::IconMode);
	setFlow(QListView::LeftToRight);
	setWrapping(true);
	setMovement(QListView::Static);
	setResizeMode(QListView::Adjust);
	setSpacing(8);
	setUniformItemSizes(true);
}


QStyleOptionViewItem
QDjViewThumbnails::View::viewOptions() const
{
	int size = widget->model->getSize();
	QStyleOptionViewItem opt = QListView::viewOptions();
	opt.decorationAlignment = Qt::AlignCenter;
	opt.decorationPosition = QStyleOptionViewItem::Top;
	opt.decorationSize = QSize(size, size);
	opt.displayAlignment = Qt::AlignCenter;
	return opt;
}



// ----------------------------------------
// QDJVIEWTHUMBNAILS::MODEL


QDjViewThumbnails::Model::~Model()
{
	if (format)
		ddjvu_format_release(format);
}


QDjViewThumbnails::Model::Model(QDjViewThumbnails *widget)
	: QAbstractListModel(widget),
	  djview(widget->djview),
	  widget(widget),
	  format(0),
	  size(0),
	  smart(true),
	  refreshScheduled(false),
	  pageInProgress(-1)
{
	// create format
#if DDJVUAPI_VERSION < 18
	unsigned int masks[3] = { 0xff0000, 0xff00, 0xff };
	format = ddjvu_format_create(DDJVU_FORMAT_RGBMASK32, 3, masks);
#else
	unsigned int masks[4] = { 0xff0000, 0xff00, 0xff, 0xff000000 };
	format = ddjvu_format_create(DDJVU_FORMAT_RGBMASK32, 4, masks);
#endif
	ddjvu_format_set_row_order(format, true);
	ddjvu_format_set_y_direction(format, true);
	ddjvu_format_set_ditherbits(format, QPixmap::defaultDepth());
	// set size
	setSize(64);
	// connect
	connect(djview, SIGNAL(documentClosed(QDjVuDocument*)),
			  this, SLOT(documentClosed(QDjVuDocument*)) );
	connect(djview, SIGNAL(documentReady(QDjVuDocument*)),
			  this, SLOT(documentReady(QDjVuDocument*)) );
	// update
	if (djview->pageNum() > 0)
		documentReady(djview->getDocument());
}


void
QDjViewThumbnails::Model::documentClosed(QDjVuDocument *doc)
{
	if (names.size() > 0)
	{
		beginRemoveRows(QModelIndex(),0,names.size()-1);
		names.clear();
		pageInProgress = -1;
		endRemoveRows();
	}
	disconnect(doc, 0, this, 0);
}


void
QDjViewThumbnails::Model::documentReady(QDjVuDocument *doc)
{
	if (names.size() > 0)
	{
		beginRemoveRows(QModelIndex(),0,names.size()-1);
		names.clear();
		pageInProgress = -1;
		endRemoveRows();
	}
	int pagenum = djview->pageNum();
	if (pagenum > 0)
	{
		beginInsertRows(QModelIndex(),0,pagenum-1);
		for (int pageno=0; pageno<pagenum; pageno++)
			names << djview->pageName(pageno);
		endInsertRows();
	}
	connect(doc, SIGNAL(thumbnail(int)),
			  this, SLOT(thumbnail(int)) );
	connect(doc, SIGNAL(pageinfo()),
			  this, SLOT(scheduleRefresh()) );
	connect(doc, SIGNAL(idle()),
			  this, SLOT(scheduleRefresh()) );
	widget->pageChanged(djview->getDjVuWidget()->page());
	scheduleRefresh();
}


void
QDjViewThumbnails::Model::thumbnail(int pageno)
{
	QModelIndex mi = index(pageno);
	emit dataChanged(mi, mi);
	scheduleRefresh();
}


void
QDjViewThumbnails::Model::scheduleRefresh()
{
	if (! refreshScheduled)
		QTimer::singleShot(0, this, SLOT(refresh()));
	refreshScheduled = true;
}


void
QDjViewThumbnails::Model::refresh()
{
	QDjVuDocument *doc = djview->getDocument();
	ddjvu_status_t status;
	refreshScheduled = false;
	if (doc && pageInProgress >= 0)
	{
		status = ddjvu_thumbnail_status(*doc, pageInProgress, 0);
		if (status >= DDJVU_JOB_OK)
			pageInProgress = -1;
	}
	if (doc && pageInProgress < 0 && widget->isVisible())
	{
		QRect dr = widget->view->rect();
		for (int i=0; i<names.size(); i++)
		{
			QModelIndex mi = index(i);
			if (dr.intersects(widget->view->visualRect(mi)))
			{
				status = ddjvu_thumbnail_status(*doc, i, 0);
				if (status == DDJVU_JOB_NOTSTARTED)
				{
					if (smart && !ddjvu_document_check_pagedata(*doc, i))
						continue;
					status = ddjvu_thumbnail_status(*doc, i, 1);
					if (status == DDJVU_JOB_STARTED)
					{
						pageInProgress = i;
						break;
					}
				}
			}
		}
	}
}


void
QDjViewThumbnails::Model::setSmart(bool b)
{
	if (b != smart)
	{
		smart = b;
		scheduleRefresh();
	}
}


void
QDjViewThumbnails::Model::setSize(int newSize)
{
	newSize = qBound(16, newSize, 256);
	if (newSize != size)
	{
		size = newSize;
		QPixmap pixmap(size,size);
		pixmap.fill();
		QPainter painter;
		int s8 = size/8;
		if (s8 >= 1)
		{
			QPolygon poly;
			poly << QPoint(s8,0)
				  << QPoint(size-2*s8,0)
				  << QPoint(size-s8-1,s8)
				  << QPoint(size-s8-1,size-1)
				  << QPoint(s8,size-1);
			QPainter painter(&pixmap);
			painter.setBrush(Qt::NoBrush);
			painter.setPen(Qt::darkGray);
			painter.drawPolygon(poly);
		}
		icon = QIcon(pixmap);
	}
	emit layoutChanged();
}


QIcon
QDjViewThumbnails::Model::makeIcon(int pageno) const
{
	QDjVuDocument *doc = djview->getDocument();
	if (doc)
	{
		// render thumbnail
		int w = size;
		int h = size;
		QImage img(size, size, QImage::Format_RGB32);
		int status = ddjvu_thumbnail_status(*doc, pageno, 0);
		if (status == DDJVU_JOB_NOTSTARTED)
		{
			const_cast<Model*>(this)->scheduleRefresh();
		}
		else if (ddjvu_thumbnail_render(*doc, pageno, &w, &h, format,
												  img.bytesPerLine(), (char*)img.bits() ))
		{
			QPixmap pixmap(size,size);
			pixmap.fill();
			QPoint dst((size-w)/2, (size-h)/2);
			QRect src(0,0,w,h);
			QPainter painter;
			painter.begin(&pixmap);
			painter.drawImage(dst, img, src);
			painter.setBrush(Qt::NoBrush);
			painter.setPen(Qt::darkGray);
			painter.drawRect(dst.x(), dst.y(), w-1, h-1);
			painter.end();
			return QIcon(pixmap);
		}
	}
	return icon;
}


QSize
QDjViewThumbnails::Model::makeHint(int) const
{
	QFontMetrics metrics(widget->view->font());
	return QSize(size, size+metrics.height());
}


int
QDjViewThumbnails::Model::rowCount(const QModelIndex &) const
{
	return names.size();
}


QVariant
QDjViewThumbnails::Model::data(const QModelIndex &index, int role) const
{
	if (index.isValid())
	{
		int pageno = index.row();
		if (pageno>=0 && pageno<names.size())
		{
			switch(role)
			{
			case Qt::DisplayRole:
			case Qt::ToolTipRole:
				return names[pageno];
			case Qt::DecorationRole:
				return makeIcon(pageno);
			case Qt::WhatsThisRole:
				return widget->whatsThis();
			case Qt::UserRole:
				return pageno;
			case Qt::SizeHintRole:
				return makeHint(pageno);
			default:
				break;
			}
		}
	}
	return QVariant();
}






// ----------------------------------------
// QDJVIEWTHUMBNAILS


QDjViewThumbnails::QDjViewThumbnails(QDjView *djview)
	: QWidget(djview),
	  djview(djview)
{
	model = new Model(this);
	selection = new QItemSelectionModel(model);
	view = new View(this);
	view->setModel(model);
	view->setSelectionModel(selection);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(view);

	connect(djview->getDjVuWidget(), SIGNAL(pageChanged(int)),
			  this, SLOT(pageChanged(int)) );
	connect(view, SIGNAL(activated(const QModelIndex&)),
			  this, SLOT(activated(const QModelIndex&)) );

	menu = new QMenu(this);
	QActionGroup *group = new QActionGroup(this);
	QAction *action;
	action = menu->addAction(tr("Tiny","thumbnail menu"));
	connect(action,SIGNAL(triggered()),this,SLOT(setSize()) );
	action->setCheckable(true);
	action->setActionGroup(group);
	action->setData(32);
	action = menu->addAction(tr("Small","thumbnail menu"));
	connect(action,SIGNAL(triggered()),this,SLOT(setSize()) );
	action->setCheckable(true);
	action->setActionGroup(group);
	action->setData(64);
	action = menu->addAction(tr("Medium","thumbnail menu"));
	connect(action,SIGNAL(triggered()),this,SLOT(setSize()) );
	action->setCheckable(true);
	action->setActionGroup(group);
	action->setData(96);
	action = menu->addAction(tr("Large","thumbnail menu"));
	connect(action,SIGNAL(triggered()),this,SLOT(setSize()) );
	action->setCheckable(true);
	action->setActionGroup(group);
	action->setData(160);
	menu->addSeparator();
	action = menu->addAction(tr("Smart","thumbnail menu"));
	connect(action,SIGNAL(toggled(bool)),this,SLOT(setSmart(bool)) );
	action->setCheckable(true);
	action->setData(true);
	updateActions();

#ifdef Q_WS_MAC
	QString mc = tr("Control Left Mouse Button");
#else
	QString mc = tr("Right Mouse Button");
#endif
	setWhatsThis(tr("<html><b>Document thumbnails.</b><br/> "
						 "This panel display thumbnails for the document pages. "
						 "Double click a thumbnail to jump to the selected page. "
						 "%1 to change the thumbnail size or the refresh mode. "
						 "The smart refresh mode only computes thumbnails "
						 "when the page data is present (displayed or cached.)"
						 "</html>").arg(mc) );
}


void
QDjViewThumbnails::updateActions(void)
{
	QAction *action;
	int size = model->getSize();
	bool smart = model->getSmart();
	foreach(action, menu->actions())
	{
		QVariant data = action->data();
		if (data.type() == QVariant::Bool)
			action->setChecked(smart);
		else
			action->setChecked(data.toInt() == size);
	}
}


void
QDjViewThumbnails::pageChanged(int pageno)
{
	if (pageno>=0 && pageno<djview->pageNum())
	{
		QModelIndex mi = model->index(pageno);
		if (! selection->isSelected(mi))
			selection->select(mi, QItemSelectionModel::ClearAndSelect);
		view->scrollTo(mi);
	}
}


void
QDjViewThumbnails::activated(const QModelIndex &index)
{
	if (index.isValid())
	{
		int pageno = index.row();
		if (pageno>=0 && pageno<djview->pageNum())
			djview->goToPage(pageno);
	}
}


int
QDjViewThumbnails::size()
{
	return model->getSize();
}


void
QDjViewThumbnails::setSize(int size)
{
	model->setSize(size);
	updateActions();
}


void
QDjViewThumbnails::setSize()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if (action)
		setSize(action->data().toInt());
}


bool
QDjViewThumbnails::smart()
{
	return model->getSmart();
}


void
QDjViewThumbnails::setSmart(bool smart)
{
	model->setSmart(smart);
	updateActions();
}

void
QDjViewThumbnails::contextMenuEvent(QContextMenuEvent *event)
{
	menu->exec(event->globalPos());
	event->accept();
}







// =======================================
// QDJVIEWFIND
// =======================================


class QDjViewFind::Model : public QAbstractListModel
{
	// This class implements the listview model.
	// But the bulk contains private data for qdjviewfind!
	Q_OBJECT
public:
	Model(QDjViewFind*);
	// model stuff
public:
	virtual int rowCount(const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	void modelClear();
	int  modelFind(int pageno);
	bool modelSelect(int pageno);
	void modelAdd(int pageno, int hits);
private:
	struct RowInfo { int pageno; int hits; QString name; };
	QList<RowInfo> pages;
	// private data stuff
public:
	void nextHit(bool backwards);
	void startFind(bool backwards, int delay=0);
	void stopFind();
	typedef QList<miniexp_t> Hit;
	typedef QList<Hit> Hits;
	QMap<int, Hits> hits;
protected:
	virtual bool eventFilter(QObject*, QEvent*);
public slots:
	void documentClosed(QDjVuDocument*);
	void documentReady(QDjVuDocument*);
	void clear();
	void doHighlights(int pageno);
	void doPending();
	void workTimeout();
	void animTimeout();
	void makeSelectionVisible();
	void pageChanged(int);
	void textChanged();
	void pageinfo();
	void itemActivated(const QModelIndex&);
private:
	friend class QDjViewFind;
	QDjViewFind *widget;
	QDjView *djview;
	QTimer *animTimer;
	QTimer *workTimer;
	QItemSelectionModel *selection;
	QAbstractButton *animButton;
	QIcon animIcon;
	QIcon findIcon;
	QRegExp find;
	int curWork;
	int curPage;
	int curHit;
	bool searchBackwards;
	bool caseSensitive;
	bool wordOnly;
	bool regExpMode;
	bool working;
	bool pending;
};


// ----------------------------------------
// QDJVIEWFIND::MODEL


QDjViewFind::Model::Model(QDjViewFind *widget)
	: QAbstractListModel(widget),
	  widget(widget),
	  djview(widget->djview),
	  selection(0),
	  animButton(0),
	  curPage(0),
	  curHit(0),
	  searchBackwards(false),
	  caseSensitive(false),
	  wordOnly(true),
	  regExpMode(false),
	  working(false),
	  pending(false)
{
	selection = new QItemSelectionModel(this);
	animTimer = new QTimer(this);
	workTimer = new QTimer(this);
	workTimer->setSingleShot(true);
	connect(animTimer, SIGNAL(timeout()), this, SLOT(animTimeout()));
	connect(workTimer, SIGNAL(timeout()), this, SLOT(workTimeout()));
	findIcon = QIcon(":/images/icon_empty.png");
}


int
QDjViewFind::Model::rowCount(const QModelIndex&) const
{
	return pages.size();
}


QVariant
QDjViewFind::Model::data(const QModelIndex &index, int role) const
{
	if (index.isValid())
	{
		int row = index.row();
		if (row>=0 && row<pages.size())
		{
			const RowInfo &info = pages[row];
			switch(role)
			{
			case Qt::DisplayRole:
				return info.name;
			case Qt::ToolTipRole:
				if (info.hits == 1)
					return tr("1 hit");
				return tr("%n hits", 0, info.hits);
			case Qt::WhatsThisRole:
				return widget->whatsThis();
			default:
				break;
			}
		}
	}
	return QVariant();
}


void
QDjViewFind::Model::modelClear()
{
	int nrows = pages.size();
	if (nrows > 0)
	{
		beginRemoveRows(QModelIndex(), 0, nrows-1);
		pages.clear();
		endRemoveRows();
	}
}


int
QDjViewFind::Model::modelFind(int pageno)
{
	int lo = 0;
	int hi = pages.size();
	while (lo < hi)
	{
		int k = (lo + hi - 1) / 2;
		if (pageno > pages[k].pageno)
			lo = k + 1;
		else if (pageno < pages[k].pageno)
			hi = k;
		else
			lo = hi = k;
	}
	return lo;
}


bool
QDjViewFind::Model::modelSelect(int pageno)
{
	int lo = modelFind(pageno);
	QModelIndex mi = index(lo);
	if (lo < pages.size() && pages[lo].pageno == pageno)
	{
		if (!selection->isSelected(mi))
			selection->select(mi, QItemSelectionModel::ClearAndSelect);
		return true;
	}
	selection->select(mi, QItemSelectionModel::Clear);
	return false;
}


void
QDjViewFind::Model::modelAdd(int pageno, int hits)
{
	QString name = djview->pageName(pageno);
	RowInfo info;
	info.pageno = pageno;
	info.hits = hits;
	if (hits == 1)
		info.name = tr("Page %1 (1 hit)").arg(name);
	else
		info.name = tr("Page %1 (%n hits)", 0, hits).arg(name);
	int lo = modelFind(pageno);
	if (lo < pages.size() && pages[lo].pageno == pageno)
	{
		pages[lo] = info;
		QModelIndex mi = index(lo);
		emit dataChanged(mi, mi);
	}
	else
	{
		beginInsertRows(QModelIndex(), lo, lo);
		pages.insert(lo, info);
		endInsertRows();
		if (pageno == djview->getDjVuWidget()->page())
			modelSelect(pageno);
	}
}


bool
QDjViewFind::Model::eventFilter(QObject*, QEvent *event)
{
	switch (event->type())
	{
	case QEvent::Show:
		if (working)
		{
			animTimer->start();
			workTimer->start();
		}
	default:
		break;
	}
	return false;
}


void
QDjViewFind::Model::documentClosed(QDjVuDocument *doc)
{
	disconnect(doc, 0, this, 0);
	stopFind();
	clear();
	curWork = 0;
	curPage = 0;
	curHit = -1;
	searchBackwards = false;
	pending = false;
	widget->eraseText();
	widget->combo->setEnabled(false);
	widget->label->setText(QString());
	widget->stack->setCurrentWidget(widget->label);
}


void
QDjViewFind::Model::documentReady(QDjVuDocument *doc)
{
	curWork = djview->getDjVuWidget()->page();
	curPage = curWork;
	curHit = -1;
	if (doc)
	{
		widget->combo->setEnabled(true);
		connect(doc, SIGNAL(pageinfo()), this, SLOT(pageinfo()));
		connect(doc, SIGNAL(idle()), this, SLOT(pageinfo()));
		if (! find.isEmpty())
			startFind(false);
	}
}


static bool
miniexp_get_int(miniexp_t &r, int &x)
{
	if (! miniexp_numberp(miniexp_car(r)))
		return false;
	x = miniexp_to_int(miniexp_car(r));
	r = miniexp_cdr(r);
	return true;
}


static bool
miniexp_get_rect(miniexp_t &r, QRect &rect)
{
	int x1,y1,x2,y2;
	if (! (miniexp_get_int(r, x1) && miniexp_get_int(r, y1) &&
			 miniexp_get_int(r, x2) && miniexp_get_int(r, y2) ))
		return false;
	if (x2<x1 || y2<y1)
		return false;
	rect.setCoords(x1, y1, x2, y2);
	return true;
}


static int
parse_text_type(miniexp_t exp)
{
	static const char *names[] = {
		"char", "word", "line", "para", "region", "column", "page"
	};
	static const int nsymbs = sizeof(names)/sizeof(const char*);
	static miniexp_t symbs[nsymbs];
	if (! symbs[0])
		for (int i=0; i<nsymbs; i++)
			symbs[i] = miniexp_symbol(names[i]);
	for (int i=0; i<nsymbs; i++)
		if (exp == symbs[i])
			return i;
	return -1;
}

static bool
miniexp_get_text(miniexp_t exp, QString &result,
					  QMap<int,miniexp_t> &positions, int &state)
{
	miniexp_t type = miniexp_car(exp);
	int typenum = parse_text_type(type);
	miniexp_t r = exp = miniexp_cdr(exp);
	if (! miniexp_symbolp(type))
		return false;
	QRect rect;
	if (! miniexp_get_rect(r, rect))
		return false;
	miniexp_t s = miniexp_car(r);
	state = qMax(state, typenum);
	if (miniexp_stringp(s) && !miniexp_cdr(r))
	{
		result += (state >= 2) ? "\n" : (state >= 1) ? " " : "";
		state = -1;
		positions[result.size()] = exp;
		result += QString::fromUtf8(miniexp_to_str(s));
		r = miniexp_cdr(r);
	}
	while(miniexp_consp(s))
	{
		miniexp_get_text(s, result, positions, state);
		r = miniexp_cdr(r);
		s = miniexp_car(r);
	}
	if (r)
		return false;
	state = qMax(state, typenum);
	return true;
}


static QList<QList<miniexp_t> >
miniexp_search_text(miniexp_t exp, QRegExp regex)
{
	QList<QList<miniexp_t> > hits;
	QString text;
	QMap<int, miniexp_t> positions;
	// build string
	int state = -1;
	if (! miniexp_get_text(exp, text, positions, state))
		return hits;
	// search hits
	int offset = 0;
	int match;
	while ((match = regex.indexIn(text, offset)) >= 0)
	{
		QList<miniexp_t> hit;
		int endmatch = match + regex.matchedLength();
		offset += 1;
		if (endmatch <= match)
			continue;
		QMap<int,miniexp_t>::const_iterator pos = positions.lowerBound(match);
		while (pos != positions.begin() && pos.key() > match)
			--pos;
		for (; pos != positions.end() && pos.key() < endmatch; ++pos)
			hit += pos.value();
		hits += hit;
		if (pos != positions.end())
			offset = pos.key();
		else
			break;
	}
	return hits;
}


void
QDjViewFind::Model::clear()
{
	QDjVuWidget *djvuWidget = djview->getDjVuWidget();
	QMap<int,Hits>::const_iterator pos;
	for (pos=hits.begin(); pos!=hits.end(); ++pos)
		if (pos.value().size() > 0)
			djvuWidget->clearHighlights(pos.key());
	pending = false;
	hits.clear();
	modelClear();
}


void
QDjViewFind::Model::doHighlights(int pageno)
{
	if (hits.contains(pageno))
	{
		QColor color = Qt::blue;
		QDjVuWidget *djvu = djview->getDjVuWidget();
		Hit pageHit;
		color.setAlpha(96);
		foreach(pageHit, hits[pageno])
		{
			QRect rect;
			miniexp_t exp;
			foreach(exp, pageHit)
			{
				if (miniexp_get_rect(exp, rect))
					djvu->addHighlight(pageno, rect.x(), rect.y(),
											 rect.width(), rect.height(),
											 color, true );
			}
		}
	}
}


void
QDjViewFind::Model::doPending()
{
	QDjVuWidget *djvu = djview->getDjVuWidget();
	while (pending && hits.contains(curPage) && pages.size()>0)
	{
		Hits pageHits = hits[curPage];
		int nhits = pageHits.size();
		if (searchBackwards)
		{
			if (curHit < 0 || curHit >= nhits)
				curHit = nhits;
			curHit--;
		}
		else
		{
			if (curHit < 0 || curHit >= nhits)
				curHit = -1;
			curHit++;
		}
		if (curHit >= 0 && curHit < nhits)
		{
			// jump to position
			pending = false;
			Hit hit = pageHits[curHit];
			QRect rect;
			if (hit.size() > 0 && miniexp_get_rect(hit[0], rect))
			{
				QDjVuWidget::Position pos;
				pos.pageNo = curPage;
				pos.posPage = rect.center();
				pos.inPage = true;
				djvu->setPosition(pos, djvu->viewport()->rect().center());
			}
		}
		else
		{
			// next page
			curHit = -1;
			if (searchBackwards)
			{
				curPage -= 1;
				if (curPage < 0)
					curPage = djview->pageNum() - 1;
			}
			else
			{
				curPage += 1;
				if (curPage >= djview->pageNum())
					curPage = 0;
			}
		}
	}
	if (! pending)
		djview->statusMessage(QString());
}


void
QDjViewFind::Model::workTimeout()
{
	// do some work
	int startingPoint = curWork;
	bool somePagesWithText = false;
	doPending();
	while (working)
	{
		if (hits.contains(curWork))
		{
			somePagesWithText = true;
		}
		else
		{
			QString name = djview->pageName(curWork);
			QDjVuDocument *doc = djview->getDocument();
			miniexp_t exp = doc->getPageText(curWork, false);
			if (exp == miniexp_dummy)
			{
				// data not present
				if (pending)
					djview->statusMessage(tr("Searching page %1 (waiting for data.)")
												 .arg(name) );
				if (pending || widget->isVisible())
					doc->getPageText(curWork, true);
				// timer will be reactivated by pageinfo()
				return;
			}
			Hits pageHits;
			hits[curWork] = pageHits;
			if (exp != miniexp_nil)
			{
				somePagesWithText = true;
				if (pending)
					djview->statusMessage(tr("Searching page %1.").arg(name));
				pageHits = miniexp_search_text(exp, find);
				hits[curWork] = pageHits;
				if (pageHits.size() > 0)
				{
					modelAdd(curWork, pageHits.size());
					doHighlights(curWork);
					doPending();
					makeSelectionVisible();
				}
				// enough
				break;
			}
		}
		// next page
		int pageNum = djview->pageNum();
		if (searchBackwards)
		{
			if (curWork <= 0)
				curWork = pageNum;
			curWork -= 1;
		}
		else
		{
			curWork += 1;
			if (curWork >= pageNum)
				curWork = 0;
		}
		// finished?
		if (curWork == startingPoint)
		{
			stopFind();
			djview->statusMessage(QString());
			if (! pages.size())
			{
				QString msg = tr("No hits!");
				if (! somePagesWithText)
				{
					widget->eraseText();
					widget->combo->setEnabled(false);
					msg = tr("<html>Document is not searchable. "
								"No page contains information "
								"about its textual content.</html>");
				}
				else if (! find.isValid())
				{
					msg = tr("<html>Invalid regular expression.</html>");
				}
				widget->stack->setCurrentWidget(widget->label);
				widget->label->setText(msg);
			}
		}
	}
	// restart timer
	if (working)
		workTimer->start(0);
}


void
QDjViewFind::Model::animTimeout()
{
	if (animButton && !animIcon.isNull())
	{
		if (animButton->icon().serialNumber() == findIcon.serialNumber())
			animButton->setIcon(animIcon);
		else
			animButton->setIcon(findIcon);
	}
}


void
QDjViewFind::Model::nextHit(bool backwards)
{
	if (working && backwards != searchBackwards)
	{
		pending = false;
		startFind(backwards);
	}
	searchBackwards = backwards;
	if (! find.isEmpty())
	{
		pending = true;
		doPending();
		if (working && pending && !workTimer->isActive())
			workTimer->start(0);
	}
}


void
QDjViewFind::Model::startFind(bool backwards, int delay)
{
	stopFind();
	searchBackwards = backwards;
	if (! find.isEmpty() && djview->pageNum() > 0)
	{
		widget->label->setText(QString());
		widget->stack->setCurrentWidget(widget->view);
		animButton = (backwards) ? widget->upButton : widget->downButton;
		animIcon = animButton->icon();
		animTimer->start(250);
		workTimer->start(delay);
		curWork = djview->getDjVuWidget()->page();
		working = true;
	}
}


void
QDjViewFind::Model::stopFind()
{
	animTimer->stop();
	workTimer->stop();
	if (animButton)
	{
		animButton->setIcon(animIcon);
		animButton = 0;
	}
	working = false;
}


void
QDjViewFind::Model::pageinfo()
{
	if (working && !workTimer->isActive())
		workTimer->start(0);
}


void
QDjViewFind::Model::makeSelectionVisible()
{
	QModelIndexList s = selection->selectedIndexes();
	if (s.size() > 0)
		widget->view->scrollTo(s[0]);
}


void
QDjViewFind::Model::pageChanged(int pageno)
{
	if (pageno != curPage)
	{
		curHit = -1;
		curPage = pageno;
		pending = false;
	}
	curWork = pageno;
}


void
QDjViewFind::Model::textChanged()
{
	stopFind();
	clear();
	QString s = widget->text();
	widget->label->setText(QString());
	widget->stack->setCurrentWidget(widget->label);
	if (s.isEmpty())
	{
		find = QRegExp();
	}
	else
	{
		if (!regExpMode)
		{
			s = QRegExp::escape(widget->text());
			s.replace(QRegExp("\\s+"), " ");
		}
		if (wordOnly)
			s = "\\b" + s;
		find = QRegExp(s);
		if (caseSensitive)
			find.setCaseSensitivity(Qt::CaseSensitive);
		else
			find.setCaseSensitivity(Qt::CaseInsensitive);
		startFind(searchBackwards, 250);
	}
}


void
QDjViewFind::Model::itemActivated(const QModelIndex &mi)
{
	if (mi.isValid())
	{
		int row = mi.row();
		if (row>=0 && row<pages.size())
			djview->goToPage(pages[row].pageno);
	}
}



// ----------------------------------------
// QDJVIEWFIND


QDjViewFind::QDjViewFind(QDjView *djview)
	: QWidget(djview),
	  djview(djview),
	  model(0),
	  view(0)
{
	model = new Model(this);
	installEventFilter(model);

	view = new QListView(this);
	view->setModel(model);
	view->setSelectionModel(model->selection);
	view->setDragEnabled(false);
	view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setSelectionMode(QAbstractItemView::SingleSelection);
	view->setTextElideMode(Qt::ElideMiddle);
	view->setViewMode(QListView::ListMode);
	view->setWrapping(false);
	view->setResizeMode(QListView::Adjust);
	caseSensitiveAction = new QAction(tr("Case sensitive"), this);
	caseSensitiveAction->setCheckable(true);
	caseSensitiveAction->setChecked(model->caseSensitive);
	wordOnlyAction = new QAction(tr("Words only"), this);
	wordOnlyAction->setCheckable(true);
	wordOnlyAction->setChecked(model->wordOnly);
	regExpModeAction = new QAction(tr("Regular expression"), this);
	regExpModeAction->setCheckable(true);
	regExpModeAction->setChecked(model->regExpMode);
	menu = new QMenu(this);
	menu->addAction(caseSensitiveAction);
	menu->addAction(wordOnlyAction);
	menu->addAction(regExpModeAction);
	QBoxLayout *vlayout = new QVBoxLayout(this);
	combo = new QComboBox(this);
	combo->setEditable(true);
	combo->setMaxCount(8);
	combo->setInsertPolicy(QComboBox::InsertAtTop);
	combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	vlayout->addWidget(combo);
	QBoxLayout *hlayout = new QHBoxLayout;
	hlayout->setSpacing(0);
	vlayout->addLayout(hlayout);
	upButton = new QToolButton(this);
	upButton->setIcon(QIcon(":/images/icon_up.png"));
	upButton->setToolTip(tr("Find Previous (Shift+F3) "));
	upButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	hlayout->addWidget(upButton);
	downButton = new QToolButton(this);
	downButton->setIcon(QIcon(":/images/icon_down.png"));
	downButton->setToolTip(tr("Find Next (F3) "));
	downButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	hlayout->addWidget(downButton);
	hlayout->addStretch(2);
	resetButton = new QToolButton(this);
	resetButton->setIcon(QIcon(":/images/icon_erase.png"));
	resetButton->setToolTip(tr("Reset search options to default values."));
	resetButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	hlayout->addWidget(resetButton);
	QToolButton *optionButton = new QToolButton(this);
	optionButton->setText(tr("Options"));
	optionButton->setPopupMode(QToolButton::InstantPopup);
	optionButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
	optionButton->setMenu(menu);
	optionButton->setAttribute(Qt::WA_CustomWhatsThis);
	optionButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	hlayout->addWidget(optionButton);
	stack = new QStackedLayout();
	view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	view->setFrameShadow(QFrame::Sunken);
	view->setFrameShape(QFrame::StyledPanel);
	stack->addWidget(view);
	label = new QLabel(this);
	label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	label->setAlignment(Qt::AlignCenter);
	label->setWordWrap(true);
	label->setFrameShadow(QFrame::Sunken);
	label->setFrameShape(QFrame::StyledPanel);
	stack->addWidget(label);
	vlayout->addLayout(stack);

	connect(djview, SIGNAL(documentClosed(QDjVuDocument*)),
			  model, SLOT(documentClosed(QDjVuDocument*)) );
	connect(djview, SIGNAL(documentReady(QDjVuDocument*)),
			  model, SLOT(documentReady(QDjVuDocument*)) );
	connect(view, SIGNAL(activated(const QModelIndex&)),
			  model, SLOT(itemActivated(const QModelIndex&)));
	connect(djview->getDjVuWidget(), SIGNAL(pageChanged(int)),
			  this, SLOT(pageChanged(int)));
	connect(combo->lineEdit(), SIGNAL(textChanged(QString)),
			  model, SLOT(textChanged()));
	connect(combo->lineEdit(), SIGNAL(returnPressed()),
			  this, SLOT(findAgain()));
	connect(caseSensitiveAction, SIGNAL(triggered(bool)),
			  this, SLOT(setCaseSensitive(bool)));
	connect(wordOnlyAction, SIGNAL(triggered(bool)),
			  this, SLOT(setWordOnly(bool)));
	connect(regExpModeAction, SIGNAL(triggered(bool)),
			  this, SLOT(setRegExpMode(bool)));
	connect(upButton, SIGNAL(clicked()),
			  this, SLOT(findPrev()));
	connect(downButton, SIGNAL(clicked()),
			  this, SLOT(findNext()));
	connect(resetButton, SIGNAL(clicked()),
			  this, SLOT(eraseText()));

	setWhatsThis
			(tr("<html><b>Finding text.</b><br/> "
				 "Search hits appear progressively as soon as you type "
				 "a search string. Typing enter jumps to the next hit. "
				 "To move to the previous or next hit, you can also use "
				 "the arrow buttons or the shortcuts <tt>F3</tt> or "
				 "<tt>Shift-F3</tt>. You can also double click a page name. "
				 "Use the <tt>Options</tt> menu to search words only or "
				 "to specify the case sensitivity."
				 "</html>"));
	wordOnlyAction->setStatusTip
			(tr("Specify whether search hits must begin on a word boundary."));
	caseSensitiveAction->setStatusTip
			(tr("Specify whether searches are case sensitive."));
	regExpModeAction->setStatusTip
			(tr("Regular expressions describe complex string matching patterns."));
	regExpModeAction->setWhatsThis
			(tr("<html><b>Regular Expression Quick Guide</b><ul>"
				 "<li>The dot <tt>.</tt> matches any character.</li>"
				 "<li>Most characters match themselves.</li>"
				 "<li>Prepend a backslash <tt>\\</tt> to match special"
				 "    characters <tt>()[]{}|*+.?!^$\\</tt>.</li>"
				 "<li><tt>\\b</tt> matches a word boundary.</li>"
				 "<li><tt>\\w</tt> matches a word character.</li>"
				 "<li><tt>\\d</tt> matches a digit character.</li>"
				 "<li><tt>\\s</tt> matches a blank character.</li>"
				 "<li><tt>\\n</tt> matches a newline character.</li>"
				 "<li><tt>[<i>a</i>-<i>b</i>]</tt> matches characters in range"
				 "    <tt><i>a</i></tt>-<tt><i>b</i></tt>.</li>"
				 "<li><tt>[^<i>a</i>-<i>b</i>]</tt> matches characters outside range"
				 "    <tt><i>a</i></tt>-<tt><i>b</i></tt>.</li>"
				 "<li><tt><i>a</i>|<i>b</i></tt> matches either regular expression"
				 "    <tt><i>a</i></tt> or regular expression <tt><i>b</i></tt>.</li>"
				 "<li><tt><i>a</i>{<i>n</i>,<i>m</i>}</tt> matches regular expression"
				 "    <tt><i>a</i></tt> repeated <tt><i>n</i></tt> to <tt><i>m</i></tt>"
				 "    times.</li>"
				 "<li><tt><i>a</i>?</tt>, <tt><i>a</i>*</tt>, and <tt><i>a</i>+</tt>"
				 "    are shorthands for <tt><i>a</i>{0,1}</tt>, <tt><i>a</i>{0,}</tt>, "
				 "    and <tt><i>a</i>{1,}</tt>.</li>"
				 "<li>Use parentheses <tt>()</tt> to group regular expressions "
				 "    before <tt>?+*{</tt>.</li>"
				 "</ul></html>"));

	eraseText();
	combo->setEnabled(false);
	label->setText(QString());
	stack->setCurrentWidget(label);
	if (djview->getDocument())
		model->documentReady(djview->getDocument());
}


void
QDjViewFind::contextMenuEvent(QContextMenuEvent *event)
{
	menu->exec(event->globalPos());
	event->accept();
}


void
QDjViewFind::takeFocus(Qt::FocusReason reason)
{
	if (combo->isVisible())
		combo->setFocus(reason);
}


QString
QDjViewFind::text()
{
	return combo->lineEdit()->text();
}


bool
QDjViewFind::caseSensitive()
{
	return model->caseSensitive;
}


bool
QDjViewFind::wordOnly()
{
	return model->wordOnly;
}


bool
QDjViewFind::regExpMode()
{
	return model->regExpMode;
}


void
QDjViewFind::setText(QString s)
{
	if (s != text())
		combo->lineEdit()->setText(s);
}


void
QDjViewFind::selectAll()
{
	combo->lineEdit()->selectAll();
	combo->lineEdit()->setFocus();
}


void
QDjViewFind::eraseText()
{
	setText(QString::null);
	setRegExpMode(false);
	setWordOnly(true);
	setCaseSensitive(false);
}


void
QDjViewFind::setCaseSensitive(bool b)
{
	if (b != model->caseSensitive)
	{
		caseSensitiveAction->setChecked(b);
		model->caseSensitive = b;
		model->textChanged();
	}
}


void
QDjViewFind::setWordOnly(bool b)
{
	if (b != model->wordOnly)
	{
		wordOnlyAction->setChecked(b);
		model->wordOnly = b;
		model->textChanged();
	}
}


void
QDjViewFind::setRegExpMode(bool b)
{
	if (b != model->regExpMode)
	{
		regExpModeAction->setChecked(b);
		model->regExpMode = b;
		model->textChanged();
	}
}


void
QDjViewFind::findNext()
{
	if (text().isEmpty())
		djview->showSideBar("find");
	model->nextHit(false);
}


void
QDjViewFind::findPrev()
{
	if (text().isEmpty())
		djview->showSideBar("find");
	model->nextHit(true);
}


void
QDjViewFind::findAgain()
{
	if (model->searchBackwards)
		findPrev();
	else
		findNext();
}


void
QDjViewFind::pageChanged(int pageno)
{
	if (pageno>=0 && pageno<djview->pageNum())
	{
		model->pageChanged(pageno);
		if (model->modelSelect(pageno))
			model->makeSelectionVisible();
	}
}



// =======================================
// QDJVIEWSHAPES
// =======================================




QDjViewShapes::QDjViewShapes(QDjView *djview, QDjViewSamplesArea *samples)
	: QWidget(djview),
	  djview(djview),
	  samples(samples),
	  treeOrder(TOP_DOWN_ORDER) /*,
		 loaded(false)*/
{
	table = new QTableWidget(this);
	table->horizontalHeader()->hide();
	table->verticalHeader()->hide();

	noShapesFound = new QLabel(this);
	noShapesFound->setText(tr("No shapes found for current page."));
	noShapesFound->setAlignment(Qt::AlignCenter);
	noShapesFound->setVisible(false);
	noShapesFound->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	progress = new QProgressBar(this);
	progress->setVisible(false);

	color_factor = new QSpinBox(this);
	color_factor->setMinimum(100);
	color_factor->setMaximum(200);
	color_factor->setSingleStep(5);
	color_factor->setValue(120);

	layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(table);
	layout->addWidget(noShapesFound);
	layout->addWidget(progress);
	layout->addWidget(color_factor);

	connect(djview, SIGNAL(documentClosed(QDjVuDocument*)),
			  this, SLOT(clear()) );
	connect(djview, SIGNAL(documentOpened(QDjVuDocument*)),
			  this, SLOT(clear()) );
	connect(djview, SIGNAL(documentReady(QDjVuDocument*)),
			  this, SLOT(documentReady(QDjVuDocument*)));
	connect(djview->getDjVuWidget(), SIGNAL(pageChanged(int)),
			  this, SLOT(pageChanged(int)) );
	/* connect(djview->getDjVuWidget(), SIGNAL(layoutChanged()),
			 this, SLOT(refresh()) );*/
	connect(table, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(shapeClicked(QTableWidgetItem*)));
	connect(color_factor, SIGNAL(valueChanged(int)), this, SLOT(colorFactorChanged(int)));

	shapeContextMenu = new QMenu(this);
	exportShapeAction = new QAction(tr("Export shape bitmap..."), shapeContextMenu);
	copyURLAction = new QAction(tr("Copy url to clipboard"), shapeContextMenu);
	exportShapeAndUrlAction = new QAction(tr("Export shape bitmap and url..."), shapeContextMenu);
	topDownAction = new QAction(tr("Top-down, left-right"), shapeContextMenu);
	topDownAction->setCheckable(true);
	topDownAction->setChecked(treeOrder == TOP_DOWN_ORDER);
	leftRightAction = new QAction(tr("Left-right, top-down"), shapeContextMenu);
	leftRightAction->setCheckable(true);
	leftRightAction->setChecked(treeOrder == LEFT_RIGHT_ORDER);
	findShapeAction = new QAction(tr("Find shape..."), shapeContextMenu);

	shapeContextMenu->addAction(exportShapeAction);
	shapeContextMenu->addAction(copyURLAction);
	shapeContextMenu->addAction(exportShapeAndUrlAction);
	shapeContextMenu->addSeparator();
	shapeContextMenu->addAction(topDownAction);
	shapeContextMenu->addAction(leftRightAction);
	shapeContextMenu->addSeparator();
	shapeContextMenu->addAction(findShapeAction);

	table->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

	connect(djview->getDjVuWidget(), SIGNAL(preciseClick(QDjVuWidget::Position)), this, SLOT(pointerClick(QDjVuWidget::Position)));

	setWhatsThis(tr("<html><b>Document outline.</b><br/> "
						 "This panel display the document outline, "
						 "or the page names when the outline is not available, "
						 "Double-click any entry to jump to the selected page."
						 "</html>"));

	if (djview->pageNum() > 0)
		refresh();
}


void
QDjViewShapes::clear()
{
	//tree->clear();
	for (QVector<QPair<int, QList<ShapeNode *> > >::iterator i = page_root_shapes.begin(); i != page_root_shapes.end(); ++i) {
		qDeleteAll(i->second);
		i->second.clear();
		qDebug("list of size %d destroyed", i->first);
	}
	page_root_shapes.clear();

	highlighed_shapes.clear();

	qDeleteAll(displayed_shapes);
	displayed_shapes.clear();

	//loaded = false;
}

void
QDjViewShapes::documentReady(QDjVuDocument * doc)
{
	qDebug("document ready");
	QTimer::singleShot(3000, this, SLOT(refresh()));
}

void
QDjViewShapes::refresh()
{
	QDjVuDocument *doc = djview->getDocument();
	if (doc /*&& !loaded*/ && djview->pageNum()>0)
	{
		page_root_shapes = QVector<QPair<int, QList<ShapeNode *> > >(djview->pageNum());
		qDebug("vector of size %d created", page_root_shapes.size());
		/*/for (each page) {
			 QList<ShapeNode *> roots;
		}*/
		/*(miniexp_t outline = doc->getDocumentOutline();
		if (outline == miniexp_dummy)
		  return;*/
		/*loaded = true;*/
		/*if (outline)
		  {
			 if (!miniexp_consp(outline) ||
				  miniexp_car(outline) != miniexp_symbol("bookmarks"))
				{
				  QString msg = tr("Outline data is corrupted");
				  qWarning("%s", (const char*)msg.toLocal8Bit());
				}
			 tree->clear();
			 QTreeWidgetItem *root = new QTreeWidgetItem();
			 fillItems(root, miniexp_cdr(outline));
			 QTreeWidgetItem *item = root;
			 while (item->childCount()==1 &&
					  item->data(0,Qt::UserRole).toInt() >= 0)
				item = item->child(0);
			 while (item->childCount() > 0)
				tree->insertTopLevelItem(tree->topLevelItemCount(),
												 item->takeChild(0) );
			 delete root;
		  }
		else
		  {
			 tree->clear();
			 QTreeWidgetItem *root = new QTreeWidgetItem(tree);
			 root->setText(0, tr("Pages"));
			 root->setFlags(Qt::ItemIsEnabled);
			 root->setData(0, Qt::UserRole, -1);
			 for (int pageno=0; pageno<djview->pageNum(); pageno++)
				{
				  QTreeWidgetItem *item = new QTreeWidgetItem(root);
				  QString name = djview->pageName(pageno);
				  item->setText(0, tr("Page %1").arg(name));
				  item->setData(0, Qt::UserRole, pageno);
				  item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				  item->setToolTip(0, tr("Go to page %1").arg(name));
				  item->setWhatsThis(0, whatsThis());
				}
			 tree->setItemExpanded(root, true);
		  }*/
		pageChanged(djview->getDjVuWidget()->page());
	}


}


/*void
QDjViewShapes::fillItems(QTreeWidgetItem *root, miniexp_t expr)
{
  while(miniexp_consp(expr))
	 {
		miniexp_t s = miniexp_car(expr);
		expr = miniexp_cdr(expr);
		if (miniexp_consp(s) &&
			 miniexp_consp(miniexp_cdr(s)) &&
			 miniexp_stringp(miniexp_car(s)) &&
			 miniexp_stringp(miniexp_cadr(s)) )
		  {
			 // fill item
			 const char *name = miniexp_to_str(miniexp_car(s));
			 const char *link = miniexp_to_str(miniexp_cadr(s));
			 int pageno = -1;
			 if (link && link[0]=='#')
				pageno = djview->pageNumber(QString::fromUtf8(link+1));
			 QTreeWidgetItem *item = new QTreeWidgetItem(root);
			 if (name && name[0])
				item->setText(0, QString::fromUtf8(name));
			 else if (pageno >= 0)
				item->setText(0, tr("Page %1").arg(djview->pageName(pageno)));
			 item->setFlags(0);
			 item->setWhatsThis(0, whatsThis());
			 if (link && link[0]=='#' && pageno>=0)
				{
				  item->setData(0, Qt::UserRole, pageno);
				  item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				  QString pagename = djview->pageName(pageno);
				  item->setToolTip(0, tr("Go to page %1").arg(pagename));
				}
			 else if (link && link[0])
				{
				  item->setData(0, Qt::UserRole, QString::fromUtf8(link));
				  item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				  item->setToolTip(0, QString::fromUtf8(link));
				}
			 // recurse
			 fillItems(item, miniexp_cddr(s));
		  }
	 }
}*/


void
QDjViewShapes::pageChanged(int pageno)
{
	if (!this->isVisible())
		return; // ignore when panel is closed

	if (page_root_shapes.size() <= pageno)
		return;

	QPair<int, QList<ShapeNode *> > &page_roots = page_root_shapes[pageno];
	if (page_roots.second.size() == 0)
		fillPageGlyph(pageno);

	QSize shapeSize(32, 32); // TODO get actual size/or configure

	int cols = table->width() / (shapeSize.width() + 7);
	int rows = (page_roots.first + cols - 1) / cols;
	int added = 0;

	//table->clear(); deleting below clears table too
	qDeleteAll(displayed_shapes);
	displayed_shapes.clear();
	highlighed_shapes.clear();

	if (page_roots.first == 0) {
		table->hide();
		noShapesFound->show();
	} else {
		noShapesFound->hide();
		table->show();
	}

	table->setColumnCount(cols);
	table->setRowCount(rows);
	table->setIconSize(shapeSize);
	QList<ShapeNode *> shape_left;

	bool left_right = treeOrder == LEFT_RIGHT_ORDER;

	QList<ShapeNode *>::iterator root_itr = page_roots.second.begin();
	if (!left_right)
		for (; root_itr != page_roots.second.end(); ++root_itr)
			shape_left.push_back(*root_itr);
	else if (root_itr != page_roots.second.end()) {
		shape_left.push_back(*root_itr);
		++root_itr;
	}

	while(!shape_left.isEmpty()) {
		ShapeNode * node = shape_left.takeFirst();

		QList<ShapeNode *> children = node->getChildren();
		for (QList<ShapeNode *>::iterator children_itr = children.begin(); children_itr != children.end(); ++children_itr)
			if (left_right)
				shape_left.push_back(*children_itr);
			else
				shape_left.push_front(*children_itr);

		QShapeTableWidgetItem *itm = new QShapeTableWidgetItem(node);
		table->setItem(added / cols, added % cols, itm);
		displayed_shapes.insert(node->getId(), itm);

		added++;

		if (left_right && shape_left.isEmpty() && root_itr != page_roots.second.end()) {
			shape_left.push_back(*root_itr);
			++root_itr;
		}
	}

	table->resizeColumnsToContents();
	table->resizeRowsToContents();

	qDebug("shape count %d (%d, %d) for page %d", page_roots.first, added, displayed_shapes.size(), pageno);
}


void
QDjViewShapes::itemActivated(QTreeWidgetItem *item)
{
	QVariant data = item->data(0, Qt::UserRole);
	if (data.type() == QVariant::Int)
	{
		int page = data.toInt();
		if (page >= 0 && page < djview->pageNum())
			djview->goToPage(page);
	}
	else if (data.type() == QVariant::String)
	{
		QString link = data.toString();
		if (link.size() > 0)
			djview->goToLink(link);
	}
}


namespace DJVU
{
struct ddjvu_page_s;
};

GP<DjVuImage> ddjvu_get_DjVuImage(DJVU::ddjvu_page_s *page);

void QDjViewShapes::fillPageGlyph(int pageno)
{
	QPair<int, QList<ShapeNode *> > &roots = page_root_shapes[pageno];
	roots.first = 0;
	qDeleteAll(roots.second); // just for sure
	roots.second.clear();

	struct DJVU::ddjvu_page_s * page = reinterpret_cast<DJVU::ddjvu_page_s *>(
			ddjvu_page_create_by_pageno(*djview->getDocument(), pageno)
			);
	if (!page) {
		qWarning("Cound not render djvupage, page %d", pageno);
		return;
	}

	GP<DjVuImage> img = ddjvu_get_DjVuImage(page);
	if (!img) {
		qWarning("Cound not render djvuimage, page %d", pageno);
		return;
	}

	GP<JB2Image> jimg = img->get_fgjb();
	if (!jimg) {
		qWarning("Cound not get fbjb, page %d", pageno);
		return;
	}

	int sh_count = jimg->get_shape_count();
	int blit_count = jimg->get_blit_count();

	progress->setMaximum(sh_count + blit_count);
	progress->show();

	QVector<ShapeNode *> shapes_nodes(sh_count);
	QMultiHash<int, int> shapes_children;
	//QSize boundingShapeSize(0, 0);
	for (int i = 0; i < sh_count; i++) {
		JB2Shape shape = jimg->get_shape(i);
		/*if (!shape)
				continue;*/

		int idx = shape.parent >= 0 ? shape.parent : -1;
		shapes_children.insert(idx, i);

		GP<GBitmap> bits = shape.bits;
		if (!bits)
			continue;
		GP<ByteStream> bs = ByteStream::create();
		bits->save_pbm(*bs);
		TArray<char> array = bs->get_data();

		ShapeNode * node = new ShapeNode(i);
		node->getPixmap().loadFromData(reinterpret_cast<const uchar*>((char*)array), array.size());
		node->getPixmap().setMask(node->getPixmap().createMaskFromColor(Qt::white, Qt::MaskInColor)); //add transparency
		//boundingShapeSize = boundingShapeSize.expandedTo(node->getPixmap().size());

		shapes_nodes[i] = node;
		progress->setValue(i);
	}

	// now put blits
	for (int i = 0; i < blit_count; i++) {
		JB2Blit *blit = jimg->get_blit(i);
		if (blit && shapes_nodes[blit->shapeno]) {
			shapes_nodes[blit->shapeno]->addBlit(blit->left, blit->bottom);
			//qDebug("blit %d %u %u %u", i, blit->left, blit->bottom, blit->shapeno);
		}
		progress->setValue(i + sh_count);
	}

	QMultiHash<int, int>::iterator itr;

	//set parentness
	for (int parent_id = 0; parent_id < sh_count; parent_id++) {
		itr = shapes_children.find(parent_id);
		while (itr != shapes_children.end() && itr.key() == parent_id) {
			if (shapes_nodes[itr.value()])
				shapes_nodes[itr.value()]->setParent(shapes_nodes[parent_id]);
			++itr;
		}
	}

	//find root shapes
	itr = shapes_children.find(-1);
	while (itr != shapes_children.end() && itr.key() == -1) {
		if (shapes_nodes[itr.value()]) {
			roots.second.append(shapes_nodes[itr.value()]);
			shapes_nodes[itr.value()]->caluculateTreeHeights();

			//DEBUG
			//if (shapes_nodes[itr.value()]->getToRootHeight() || shapes_nodes[itr.value()]->getToLeafHeight()) {
			//    *itr;
			//}
		}
		++itr;
	};
	roots.first = sh_count;

	qSort(roots.second.begin(), roots.second.end(), ShapeNode::greaterThan);
	progress->hide();

	qDebug("Grabed %d shapes for page %d", sh_count, pageno);
}

void
QDjViewShapes::shapeClicked(QTableWidgetItem *item, bool scrollTo)
{
	QShapeTableWidgetItem * itm = static_cast<QShapeTableWidgetItem *>(item);
	if (itm && itm->node) {
		samples->clearUnanchored();
		djview->getDjVuWidget()->clearHighlights(djview->getDjVuWidget()->page());
		while(!highlighed_shapes.isEmpty())
			highlighed_shapes.takeFirst()->setBackgroundColor(Qt::white); //TODO original color

		highlightOne(itm->node, Qt::yellow);

		highlight(itm->node->getSiblings(), Qt::magenta);

		highlightParent(itm->node->getParent(), itm->node, Qt::cyan, Qt::red, false);
		highlightChildren(itm->node, Qt::green);

		// position view
		if (scrollTo && !itm->node->getBlits().isEmpty()) {
			//double x, y;
			//x = (double) itm->node->getBlits().first().first;
			//y = (double) itm->node->getBlits().first().second;
			//djview->goToPosition(QString(), x, y);
			QDjVuWidget::Position pos;
			pos.pageNo = djview->getDjVuWidget()->page();
			pos.inPage = true;
			pos.posPage.setX(itm->node->getBlits().first().first);
			pos.posPage.setY(itm->node->getBlits().first().second + itm->node->getPixmap().height());
			//pos.posView = pos.posPage;
			pos.doPage = true;
			//pos.hAnchor = pos.posPage.x();
			//pos.vAnchor = pos.posPage.y();

			djview->getDjVuWidget()->setPosition(pos);
		}
		/*djview->pendingPosition.clear();
		  djview->pendingPosition << x << y;
		  djview->performPendingLater();*/

		table->scrollToItem(item);
	}
}

void
QDjViewShapes::highlight(QList<ShapeNode *> nodes, QColor color)
{
	for (QList<ShapeNode *>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
		highlightOne(*i, color);
		highlightChildren(*i, color.lighter(color_factor->value()));
	}
}

void
QDjViewShapes::highlightOne(ShapeNode * node, QColor color)
{
	if (node->tbl_itm) {
		node->color = color;
		node->tbl_itm->setBackgroundColor(node->color);
		highlighed_shapes.push_back(node->tbl_itm);


		// print coords
		QList<QPair<unsigned short, unsigned short> > blits = node->getBlits();
		QList<QPair<unsigned short, unsigned short> >::const_iterator itr;
		for (itr = blits.constBegin(); itr != blits.constEnd(); ++itr) {
			//qDebug("shape %i, left %u, bottom %u", node->getId(), itr->first, itr->second);
			color.setAlpha(128);
			djview->getDjVuWidget()->addHighlight(djview->getDjVuWidget()->page(), itr->first, itr->second, node->getPixmap().width(), node->getPixmap().height(), color);

			// create sample
			QDjVuWidget *qwid = new QDjVuWidget(djview->getDjVuWidget()->document());
			QDjVuWidget::Position pos;
			pos.pageNo = djview->getDjVuWidget()->page();
			pos.inPage = true;
			pos.posPage.setX(itr->first);
			pos.posPage.setY(itr->second + node->getPixmap().height());
			pos.doPage = true;

			samples->addQDjVuWidget(qwid, pos.pageNo, QRect(itr->first, itr->second, node->getPixmap().width(), node->getPixmap().height()), color, djview->getDjVuWidget());
			qwid->setPosition(pos);
			qwid->setMinimumSize(QSize(17, 17) + node->getPixmap().size());
			qDebug("shape size %d %d", node->getPixmap().size().width(), node->getPixmap().size().height());
		}

	}
}

void
QDjViewShapes::highlightParent(ShapeNode * parent, ShapeNode * node, QColor parentColor, QColor uncleColor, bool colorSiblings)
{
	if (parent) {
		parentColor = parentColor.lighter(color_factor->value());
		highlightOne(parent, parentColor);

		// color uncles
		if (colorSiblings) {
			uncleColor = uncleColor.lighter(color_factor->value());
			QList<ShapeNode *>::const_iterator itr;
			for (itr = parent->getChildren().constBegin(); itr != parent->getChildren().constEnd(); ++itr)
				if (*itr != node) { // siblings
					highlightOne(*itr, uncleColor);
					highlightChildren(*itr, uncleColor);
				}
		}

		highlightParent(parent->getParent(), parent, parentColor, uncleColor);
	}


	// XXX junk
	/*color = QColor::fromHsl(180, 96, 32);
	 for (int i=10; i < 20; i++) {
		  color = color.lighter(125);
		  table->item(i, 0)->setBackgroundColor(color);
	 }
	 color = QColor::fromHsl(0, 96, 32);
	 for (int i=10; i > 0; i--) {
		  color = color.lighter(125);
		  table->item(i, 0)->setBackgroundColor(color);
	 }*/
}

void
QDjViewShapes::highlightChildren(ShapeNode * node, QColor color)
{
	QList<ShapeNode *> children = node->getChildren();
	color = color.lighter(color_factor->value());
	for (QList<ShapeNode *>::iterator i = children.begin(); i != children.end(); ++i) {
		highlightOne(*i, color);

		highlightChildren(*i, color);
	}
}

void
QDjViewShapes::colorFactorChanged(int)
{
	refreshShapeTree();
}

void
QDjViewShapes::resizeEvent(QResizeEvent *)
{
	refreshShapeTree();
}

void
QDjViewShapes::refreshShapeTree()
{
	pageChanged(djview->getDjVuWidget()->page());
}

QUrl
QDjViewShapes::shape_URL(ShapeNode *node)
{
	QUrl url = djview->getDecoratedUrl();
	QDjVuWidget::Position pos = djview->getDjVuWidget()->position(); //position(pointerPos);
	//QRect seg = widget->getSegmentForRect(rect, pos.pageNo);
	if (url.isValid() && pos.pageNo>=0 && pos.pageNo<djview->pageNum()) {
		if (! url.hasQueryItem("djvuopts"))
			url.addQueryItem("djvuopts", QString::null);
		/*QList<ddjvu_fileinfo_t> &dp = djview->getDocument()->documentPages;
				if (! url.hasQueryItem("page"))
				  if (pos.pageNo>=0 && pos.pageNo<documentPages.size())
					 url.addQueryItem("page", QString::fromUtf8(dp[pos.pageNo].id));*/
		url.addQueryItem("highlight", QString("%1,%2,%3,%4")
							  .arg(node->getBlits().first().first).arg(node->getBlits().first().second)
							  .arg(node->getPixmap().width()).arg(node->getPixmap().height()) );
		/*.arg(seg.left()).arg(seg.top())
				.arg(seg.width()).arg(seg.height())*/
		if (url.hasQueryItem("page")) {
			// page should be last parameter for Caminova DjVu Browser Plug-in
			QStringList pages = url.allQueryItemValues("page");
			url.removeAllQueryItems("page");
			QStringListIterator pagesItr(pages);
			while (pagesItr.hasNext())
				url.addQueryItem("page", pagesItr.next());
		}
	}

	return url;
}

void
QDjViewShapes::showContextMenu(QPoint pos)
{
	QPoint globalPos = table->viewport()->mapToGlobal(pos);

	QShapeTableWidgetItem * itm = static_cast<QShapeTableWidgetItem *>(table->itemAt(pos));
	if (itm && itm->node){
		exportShapeAction->setEnabled(true);
		copyURLAction->setEnabled(true);
		exportShapeAndUrlAction->setEnabled(true);
	} else {
		exportShapeAction->setEnabled(false);
		copyURLAction->setEnabled(false);
		exportShapeAndUrlAction->setEnabled(false);
	}

	QAction* selectedItem = shapeContextMenu->exec(globalPos);
	if ((selectedItem == exportShapeAction) || (selectedItem == exportShapeAndUrlAction)) {
		QString fileName = djview->getShortFileName(); //djview->getDocumentFileName();
		fileName.remove(QRegExp(".djvu$", Qt::CaseInsensitive));
		fileName.append(QString("-shape-%1.png").arg(itm->node->getId()));
		fileName = QFileDialog::getSaveFileName(this, tr("Save shape %1 to file").arg(itm->node->getId()),
															 fileName, tr("PNG Images (*.png)")); //tr("Images (*.png *.xpm *.jpg)"));

		if (!fileName.isEmpty()) {
			itm->node->getPixmap().save(fileName);
			if (selectedItem == exportShapeAndUrlAction) {
				QFile urlFile(fileName.append(".url"));
				if (!urlFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
					QMessageBox::warning(this, tr("Error"), tr("Cannot save url to file. Please try copying to clipboard instead."));
				else {
					urlFile.write(shape_URL(itm->node).toEncoded());
					urlFile.close();
				}
			}
		}
	} else if (selectedItem == copyURLAction) {
		QApplication::clipboard()->setText(shape_URL(itm->node).toString());
	} else if (selectedItem == leftRightAction) {
		topDownAction->setChecked(!leftRightAction->isChecked());

		treeOrder = leftRightAction->isChecked() ? LEFT_RIGHT_ORDER : TOP_DOWN_ORDER;
		refreshShapeTree();
	} else if (selectedItem == topDownAction) {
		leftRightAction->setChecked(!topDownAction->isChecked());

		treeOrder = topDownAction->isChecked() ? TOP_DOWN_ORDER : LEFT_RIGHT_ORDER;
		refreshShapeTree();
	} else if (selectedItem == findShapeAction) {
		bool ok;
		QString idText = QInputDialog::getText(this, tr("Find shape..."), tr("Shape id:"));
		if (!idText.isNull()) {
			int id = idText.toInt(&ok);
			if (!ok)
				QMessageBox::warning(this, tr("Error"), tr("Invalid id (not a valid integer)."));
			else {
				if (displayed_shapes.contains(id)) {
					shapeClicked(displayed_shapes.value(id));
					table->setCurrentItem(displayed_shapes.value(id), QItemSelectionModel::NoUpdate);
				}
				else
					QMessageBox::warning(this, tr("Error"), tr("Shape of id %1 cannot be found.").arg(id));
			}
		}

	}
}

void
QDjViewShapes::pointerClick(const QDjVuWidget::Position &pos)
{
	//qDebug("clicked on %d %d", pos.posPage.x(), pos.posPage.y());
	//TODO fast tree lookup
	QPair<int, QList<ShapeNode *> > &page_roots = page_root_shapes[pos.pageNo];
	QList<ShapeNode *> found;
	for (QList<ShapeNode *>::const_iterator itr = page_roots.second.constBegin(); itr != page_roots.second.constEnd(); ++itr)
		lookupShapesInTree(*itr, pos.posPage.x(), pos.posPage.y(), found);

	if (found.size() == 1)
		shapeClicked(found[0]->tbl_itm, false);
	else if (found.size() > 1) {
		QShapeSelectionDialog shapeSelectWindow(found, this);
		ShapeNode * node = shapeSelectWindow.selectShape();
		if (node)
			shapeClicked(node->tbl_itm, false);
	}
}

void
QDjViewShapes::lookupShapesInTree(ShapeNode * node, int x, int y, QList<ShapeNode *> &found)
{
	if (!node)
		return;

	QSize shapeSize = node->getPixmap().size();
	for (QList<QPair<unsigned short, unsigned short> >::const_iterator itr = node->getBlits().constBegin(); itr != node->getBlits().constEnd(); ++itr) {
		if (x >= itr->first && x <= itr->first + shapeSize.width() &&
			 y >= itr->second && y <= itr->second + shapeSize.height()) {
			found.append(node);
		}
	}

	for (QList<ShapeNode *>::const_iterator itr = node->getChildren().constBegin(); itr != node->getChildren().constEnd(); ++itr)
		lookupShapesInTree(*itr, x, y, found);
}

QDjViewShapes::QShapeSelectionDialog::QShapeSelectionDialog(QList<ShapeNode *> &shapes, QWidget *parent) : QDialog(parent), selected(NULL) {
	setModal(true);
	QHBoxLayout * layout = new QHBoxLayout(this);

	foreach(ShapeNode * shape, shapes) {
		if (shape) {
			QLabel * label = new QLabel(this);
			label->setPixmap(shape->getPixmap());
			label->setFrameStyle(QFrame::Box);
			labels_shapes[label] = shape;
			layout->addWidget(label);
		}
	}

	setWindowTitle(tr("Select one shape"));
	layout->setSizeConstraint(QLayout::SetFixedSize);
}

QDjViewShapes::QShapeSelectionDialog::~QShapeSelectionDialog()
{
	qDeleteAll(labels_shapes.keys());
}

void
QDjViewShapes::QShapeSelectionDialog::mousePressEvent(QMouseEvent * event)
{
	event->accept();
	if (event->buttons() == Qt::LeftButton) {
		QWidget * child = this->childAt(event->pos());
		if (child && labels_shapes.contains(child))
			selected = labels_shapes[child];
	}
}

void
QDjViewShapes::QShapeSelectionDialog::mouseReleaseEvent(QMouseEvent * event)
{
	event->accept();
	if (selected)
		accept();
}

ShapeNode *
QDjViewShapes::QShapeSelectionDialog::selectShape()
{
	if (exec() == QDialog::Accepted)
		return selected;

	return NULL;
}



// =======================================
// QDJVIEWSAMPLES
// =======================================




QDjViewSamplesArea::QDjViewSamplesArea(QDjView *djview)
	: QWidget(djview),
	  djview(djview)
{
	samples = new QDjVuWidgetSamples(this);

	QScrollArea * samplesArea = new QScrollArea(this);
	samplesArea->setBackgroundRole(QPalette::Dark);
	samplesArea->setWidgetResizable(true);
	samplesArea->setWidget(samples);

	layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(samplesArea);

	setWhatsThis(tr("<html><b>Document outline.</b><br/> "
						 "This panel display the document outline, "
						 "or the page names when the outline is not available, "
						 "Double-click any entry to jump to the selected page."
						 "</html>"));
}

void
QDjViewSamplesArea::clearUnanchored()
{
	samples->clearUnanchored();
}

void
QDjViewSamplesArea::addQDjVuWidget(QDjVuWidget *widget, int highlightOnPage, QRect toHighlight, QColor color, QDjVuWidget *main_widget)
{
	samples->addQDjVuWidget(widget, highlightOnPage, toHighlight, color, main_widget);
}

QDjVuWidgetSamples::QDjVuWidgetSamples(QWidget *parent) : QWidget(parent)
{
	//setAcceptDrops(true);
	samplesLayout = new QVBoxLayout(this);
	samplesLayout->setMargin(0);
	samplesLayout->setSpacing(2);
}

/*void
QDjVuWidgetSamples::dragEnterEvent(QDragEnterEvent *event)
{

}
void
QDjVuWidgetSamples::dragMoveEvent(QDragMoveEvent *event)
{

}

void
QDjVuWidgetSamples::dropEvent(QDropEvent *event)
{
}

void
QDjVuWidgetSamples::mousePressEvent(QMouseEvent *event)
{
}*/

void
QDjVuWidgetSamples::clearUnanchored()
{
	QList<QDjVuWidgetSample *>::iterator itr = samples.begin();
	while (itr != samples.end()) {
		if (!(*itr)->isPined()) {
			samplesLayout->removeWidget(*itr);
			delete *itr;
			itr = samples.erase(itr);
		} else {
			++itr;
		}
	}
}

void
QDjVuWidgetSamples::addQDjVuWidget(QDjVuWidget *widget, int highlightOnPage, QRect toHighlight, QColor color, QDjVuWidget *main_widget)
{
	QDjVuWidgetSample * sample = new QDjVuWidgetSample(widget, highlightOnPage, toHighlight, color, main_widget, this);
	samplesLayout->addWidget(sample);
	samples.append(sample);
}

QDjVuWidgetSample::QDjVuWidgetSample(QDjVuWidget *widget, int highlightOnPage, QRect toHighlight, QColor color, QDjVuWidget *main_widget, QWidget *parent) : QWidget(parent), main_widget(main_widget)
{
	this->highlightOnPage = highlightOnPage;
	this->highlight = toHighlight;
	this->highlightColor = color;
	layout = new QHBoxLayout(this);
	layout->setMargin(0);
	check = new QCheckBox(this);
	widget->setParent(this);
	layout->addWidget(widget);
	layout->addWidget(check);
	widget->addHighlight(highlightOnPage, toHighlight.x(), toHighlight.y(), toHighlight.width(), toHighlight.height(), highlightColor);

	connect(widget, SIGNAL(preciseClick(QDjVuWidget::Position)), this, SLOT(preciseClick(QDjVuWidget::Position)));
}

QDjVuWidgetSample::~QDjVuWidgetSample()
{
	//delete widget;
	delete check;
	delete layout;
}

void
QDjVuWidgetSample::preciseClick(const QDjVuWidget::Position &pos)
{
	qDebug("got precise click %d %d, %p", pos.posPage.x(), pos.posPage.y(), main_widget);

	if (main_widget) {
		main_widget->setPosition(pos);
		main_widget->clearHighlights(this->highlightOnPage);
		main_widget->addHighlight(this->highlightOnPage, this->highlight.x(), this->highlight.y(), this->highlight.width(), this->highlight.height(), this->highlightColor);
	}
}

bool
QDjVuWidgetSample::isPined()
{
	if (check)
		return check->isChecked();

	return false;
}




// =======================================
// QDJVIEWTEXT
// =======================================

QDjViewText::QDjViewText(QDjView *djview)
	: QWidget(djview),
	  djview(djview)
{
	textLabel = new QLabel(this);
	textLabel->setAlignment(Qt::AlignCenter);
	textLabel->setTextFormat(Qt::PlainText);
	textLabel->setWordWrap(false);

	layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(textLabel);
}

/*void
QDjViewText::updateTextLabelForPosition(const QDjVuWidget::Position &pos)
{
	 QString text, results[3];

	 QString lb = QString::fromUtf8(" \302\253 ");
	 QString rb = QString::fromUtf8(" \302\273 ");
	 QRect rect;

	 if (djview->getDjVuWidget()->getTextForPosition(pos, results, rect))
		{
		  if (results[0].size() || results[2].size())
			 results[1] = "[" + results[1] + "]";

		  text = results[0] + results[1] + results[2];

		  text = text.trimmed();
		  if (text.size()) {
			 textLabel->setText(lb + text + rb);
			 djview->getDjVuWidget()->addTemporaryHighlight(djview->getDjVuWidget()->page(),
																			rect.bottomLeft().x(), rect.bottomLeft().y(), rect.width(), rect.height());
		  } else
			 djview->getDjVuWidget()->clearTemporaryHighlight();
		}
}*/

void
QDjViewText::updateTextLabel()
{
	textLabel->clear();
	if (textLabel->isVisible())
	{
		QString text;
		if (! textDockRect.isEmpty())
		{
			text = djview->getDjVuWidget()->getTextForRect(textDockRect);
			text = text.replace(QRegExp("\\s+"), " ");
		}
		else
		{
			QString results[3];
			QRect rect;

			if (djview->getDjVuWidget()->getTextForPointer(results, rect))
			{
				if (results[0].size() || results[2].size())
					results[1] = "[" + results[1] + "]";

				text = results[0] + results[1] + results[2];

				djview->getDjVuWidget()->addTemporaryHighlight(djview->getDjVuWidget()->page(), rect);

			}
		}
		QString lb = QString::fromUtf8(" \302\253 ");
		QString rb = QString::fromUtf8(" \302\273 ");

		text = text.trimmed();
		if (text.size()) {
			textLabel->setText(lb + text + rb);
		} else
			djview->getDjVuWidget()->clearTemporaryHighlight();
	}
}




// =======================================
// QDJVIEWPOLIQARP
// =======================================

QDjViewPoliqarp::QDjViewPoliqarp(QDjView *djview)
	: QWidget(djview),
	  djview(djview), volumeOpened(false)
{
	poliqarpURL = new QLineEdit(this);
	poliqarpURL->setToolTip(tr("Enter Poliqarp URL"));
	poliqarpURL->setText("http://poliqarp.wbl.klf.uw.edu.pl/");
	goURLButton = new QPushButton(this);
	goURLButton->setIcon(goURLButton->style()->standardIcon(QStyle::SP_ArrowRight));
	goURLButton->setToolTip(tr("Go to Poliqarp and get volumes list"));
	connect(poliqarpURL, SIGNAL(returnPressed()), goURLButton, SLOT(click()));

	URLLayout = new QHBoxLayout();
	URLLayout->setSpacing(0);
	URLLayout->setMargin(0);
	URLLayout->addWidget(poliqarpURL, 1);
	URLLayout->addWidget(goURLButton);

	poliqarpVolume = new QComboBox(this);
	poliqarpVolume->setEditable(false);
	poliqarpVolume->setToolTip(tr("Select volume"));

	poliqarpQuery = new QLineEdit(this);
	poliqarpQuery->setToolTip(tr("Enter Poliqarp query"));
	poliqarpQuery->setMaxLength(1000);
	goQueryButton = new QPushButton(this);
	goQueryButton->setIcon(goQueryButton->style()->standardIcon(QStyle::SP_ArrowRight));
	goQueryButton->setToolTip(tr("Execute Poliqarp query"));
	connect(poliqarpQuery, SIGNAL(returnPressed()), goQueryButton, SLOT(click()));

	queryLayout = new QHBoxLayout();
	queryLayout->setSpacing(0);
	queryLayout->setMargin(0);
	queryLayout->addWidget(poliqarpQuery, 1);
	queryLayout->addWidget(goQueryButton);

	textEdit = new QPlainTextEdit(this);
	textEdit->hide();

	layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);

	samples = new QDjViewSamplesArea(djview);

	prevButton = new QPushButton(this);
	prevButton->setIcon(goURLButton->style()->standardIcon(QStyle::SP_ArrowLeft));
	prevButton->setToolTip(tr("Prevoius results"));
	nextButton = new QPushButton(this);
	nextButton->setIcon(goURLButton->style()->standardIcon(QStyle::SP_ArrowRight));
	nextButton->setToolTip(tr("Next results"));

	navigateButtonsLayout = new QHBoxLayout();
	navigateButtonsLayout->setSpacing(0);
	navigateButtonsLayout->setMargin(0);
	navigateButtonsLayout->addWidget(prevButton);
	navigateButtonsLayout->addWidget(nextButton);

	poliqarpClientStatus = new QLabel(this);
	poliqarpClientStatus->setAlignment(Qt::AlignCenter);

	layout->addLayout(URLLayout);
	layout->addWidget(poliqarpVolume);
	layout->addLayout(queryLayout);
	layout->addWidget(textEdit, 1);
	layout->addWidget(samples, 1);
	layout->addLayout(navigateButtonsLayout);
	layout->addWidget(poliqarpClientStatus);

	manager = new QNetworkAccessManager(this);
	queryManager = new QNetworkAccessManager(this);

	connect(goURLButton, SIGNAL(released()), this, SLOT(goURL()));
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseVolumeReply(QNetworkReply*)));
	connect(poliqarpVolume, SIGNAL(currentIndexChanged(int)), this, SLOT(changeVolume(int)));
	connect(queryManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseQueryReply(QNetworkReply*)));
	connect(goQueryButton, SIGNAL(released()), this, SLOT(goQuery()));
	connect(prevButton, SIGNAL(released()), this, SLOT(prevResults()));
	connect(nextButton, SIGNAL(released()), this, SLOT(nextResults()));

	//replyExpected = PQRP_MAINPAGE;
}

void QDjViewPoliqarp::goURL()
{
	poliqarpVolume->clear();
	poliqarpClientStatus->clear();

	manager->get(QNetworkRequest(QUrl(poliqarpURL->text())));
	poliqarpClientStatus->setText(tr("[in progress]"));
}

void QDjViewPoliqarp::parseVolumeReply(QNetworkReply *reply)
{
	QDomDocument doc;
	QDomElement docElem, aElem;

	if (!reply->isFinished()) {
		qWarning("Request not finished!");
		return;
	}
	if (reply->error() != QNetworkReply::NoError) {
		qWarning("Request error!");
		return;
	}

	if (!doc.setContent(reply, false)) {
		qWarning("Request processing error (1)!");
		return;
	}

	poliqarpClientStatus->clear();

	docElem = doc.documentElement().firstChildElement("body").firstChildElement("div").firstChildElement("ul").firstChildElement("li");
	docElem = docElem.nextSiblingElement("li").nextSiblingElement("li");
	docElem = docElem.firstChildElement("ul").firstChildElement("li");
	while (!docElem.isNull()) {
		aElem = docElem.firstChildElement("a");
		if (!aElem.isNull())
			poliqarpVolume->addItem(aElem.firstChild().nodeValue(), QUrl(poliqarpURL->text().remove(QRegExp("[/]+$")) + aElem.attribute("href")));

		docElem = docElem.nextSiblingElement("li");
	}

	reply->deleteLater();
}

void QDjViewPoliqarp::changeVolume(int index)
{
	QNetworkRequest req(poliqarpVolume->itemData(index).toUrl());
	textEdit->appendPlainText(req.url().toString());
	queryManager->get(req);

	volumeOpened = false;
	//queryManager->get(QNetworkRequest(poliqarpVolume->itemData(index).toUrl()));
	//needed for cookie init
	poliqarpClientStatus->setText(tr("[in progress]"));
}

void QDjViewPoliqarp::parseQueryReply(QNetworkReply *reply)
{
	QColor color = Qt::green;
	color.setAlpha(128);
	int x, y, w, h;

	if (reply->isFinished()) {
		if (reply->error() == QNetworkReply::NoError) {
			textEdit->appendPlainText("\n\n-----------------------------\n\n");

			poliqarpClientStatus->clear();

			if (!reply->attribute(QNetworkRequest::RedirectionTargetAttribute).isNull()) {
				//redirection
				textEdit->appendPlainText(QString("redirect to: %1").arg(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString()));
				QNetworkRequest req(reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl());

				queryManager->get(req);
				poliqarpClientStatus->setText(tr("[in progress]"));
			} else {
				QDomDocument doc;
				QDomElement docElem, aElem;

				samples->clearUnanchored();

				if (!doc.setContent(reply, false)) {
					qWarning("Query result processing error (1)!");
					return;
				}

				//result count
				docElem = doc.documentElement().firstChildElement("body").firstChildElement("div").nextSiblingElement("div").firstChildElement("div").nextSiblingElement("div").firstChildElement("p");
				if (!docElem.isNull()) {
					poliqarpClientStatus->setText(docElem.text().trimmed());
				} else {
					poliqarpClientStatus->clear();
				}

				docElem = doc.documentElement().firstChildElement("body").firstChildElement("div").nextSiblingElement("div").firstChildElement("div").nextSiblingElement("div").firstChildElement("table").firstChildElement("tr");

				if (docElem.isNull()) {
					qDebug(" null doc elem");
					//textEdit->appendPlainText("================\nnull\n\n");
					//textEdit->appendPlainText(doc.toString());

					//pending?
					aElem = doc.documentElement().firstChildElement("body").firstChildElement("div").nextSiblingElement("div").firstChildElement("p").firstChildElement("a");
					if (!aElem.isNull()) {
						poliqarpClientStatus->setText(tr("[in progress - pending results]"));

						QTimer::singleShot(1500, this, SLOT(getPendingResults()));
					}
				}
				else
					qDebug("not null");

				while (!docElem.isNull()) {
					aElem = docElem.firstChildElement("td").nextSiblingElement("td").nextSiblingElement("td").nextSiblingElement("td").firstChildElement("a");

					QUrl url;

					if (!aElem.isNull()) {
						url = aElem.attribute("href");
						//http://poliqarp.wbl.klf.uw.edu.pl/extra/linde/index.djvu?djvuopts&page=163&zoom=width&showposition=0.084,0.593&highlight=155,1296,91,26#Linde
						textEdit->appendPlainText(aElem.attribute("href"));
					}

					if (!volumeOpened && url.isValid()) {
						djview->open(url);

						volumeOpened = true;
						firstShown = 0;
					}

					if (volumeOpened && url.isValid()) {
						// create sample
						QDjVuWidget *qwid = new QDjVuWidget(djview->getDjVuWidget()->document());

						QDjVuWidget::Position pos;
						pos.pageNo = djview->getDjVuWidget()->page();
						pos.inPage = true;
						pos.posPage.setX(1);
						pos.posPage.setY(1);
						pos.doPage = true;
						x = y = w = h = 0;

						QPair<QString,QString> pair;
						foreach(pair, url.queryItems())
						{
							if (pair.first.toLower() == "page") {
								pos.pageNo = pair.second.toInt() - 1;
							}
							if (pair.first.toLower() == "highlight")
							{
								QStringList list = pair.second.split(",");
								if (list.length() == 4) {
									x = list[0].toInt();
									y = list[1].toInt();
									w = list[2].toInt();
									h = list[3].toInt();

									pos.posPage.setX(x);
									pos.posPage.setY(y + h);

									qwid->setPosition(pos);

									qwid->setMinimumSize(QSize(17, 17) + QSize(w, h));
								}
							}

							qDebug("pair %s -> %s", pair.first.toLocal8Bit().constData(), pair.second.toLocal8Bit().constData());
						}

						samples->addQDjVuWidget(qwid, pos.pageNo, QRect(x, y, w, h), color, djview->getDjVuWidget());
					}

					docElem = docElem.nextSiblingElement("tr");
				}

				//textEdit->appendHtml(QString(reply->readAll()));
			}
		} else {
			qWarning("Query error %s", reply->errorString().toLocal8Bit().constData());
		}
	}
}

void QDjViewPoliqarp::goQuery()
{
	if (poliqarpQuery->text().length()) {
		//QUrl url = poliqarpVolume->itemData(poliqarpVolume->currentIndex()).toUrl();
		//QUrl queryUrl("query/");
		bareQueryExecuted = poliqarpVolume->itemData(poliqarpVolume->currentIndex()).toUrl().resolved(QUrl("query/"));

		QNetworkRequest req(bareQueryExecuted); //req(url.resolved(queryUrl));

		QByteArray args;
		args.append(QString("query=%1").arg(poliqarpQuery->text()));

		queryManager->post(req, args);
		poliqarpClientStatus->setText(tr("[in progress]"));
	}
}

void QDjViewPoliqarp::getPendingResults()
{
	QNetworkRequest req(bareQueryExecuted);
	queryManager->get(req);

	poliqarpClientStatus->setText(tr("[in progress]"));
}

void QDjViewPoliqarp::prevResults()
{
	if (firstShown > 0) {
		firstShown -= 25;
		QUrl next(QString("%1+/").arg(firstShown));
		QNetworkRequest req(bareQueryExecuted.resolved(next));

		queryManager->get(req);
		poliqarpClientStatus->setText(tr("[in progress]"));
	}
}

void QDjViewPoliqarp::nextResults()
{
	firstShown += 25;
	QUrl next(QString("%1+/").arg(firstShown));
	QNetworkRequest req(bareQueryExecuted.resolved(next));

	queryManager->get(req);
	poliqarpClientStatus->setText(tr("[in progress]"));
}

// ----------------------------------------
// MOC

#include "qdjviewsidebar.moc"




/* -------------------------------------------------------------
	Local Variables:
	c++-font-lock-extra-types: ( "\\sw+_t" "[A-Z]\\sw*[a-z]\\sw*" )
	End:
	------------------------------------------------------------- */
