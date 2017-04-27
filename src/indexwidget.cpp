
/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "indexmodel.h"
#include "indexwidget.h"
#include "messagedialog.h"
#include "entryindexdialog.h"

IndexWidget::IndexWidget(QWidget *parent) :
	QWidget(parent)
{
	ui.setupUi(this);

	m_model = new IndexModel(this);
	ui.indexList->setModel(m_model);

	connect(ui.indexEdit, &QLineEdit::textEdited, this, &IndexWidget::findEntry);
	connect(ui.indexEdit, &QLineEdit::returnPressed, this, &IndexWidget::entryTriggered);
	connect(ui.indexList, &QAbstractItemView::activated, this, &IndexWidget::activateEntry);
	connect(ui.indexList, &QAbstractItemView::customContextMenuRequested, this, &IndexWidget::menuRequested);

	connect(ui.actionDeleteEntry, &QAction::toggled, this, &IndexWidget::deleteEntry);
	connect(ui.actionEditEntry, &QAction::triggered, this, &IndexWidget::editEntry);

	addAction(ui.actionDeleteEntry);
	addAction(ui.actionEditEntry);

	m_sortGroup = new QActionGroup(this);
	m_sortGroup->setExclusive(true);
	m_sortGroup->addAction(ui.actionOriginalOrder);
	m_sortGroup->addAction(ui.actionAlphabeticOrder);
	m_sortGroup->addAction(ui.actionLetterOrder);
	m_sortGroup->addAction(ui.actionAtergoOrder);
	connect(m_sortGroup, &QActionGroup::triggered, this, &IndexWidget::sort);

	configure();
}

IndexWidget::~IndexWidget()
{
}

bool IndexWidget::open(const QString &filename)
{
	if (filename.isEmpty())
		return false;
	if (!queryClose())
		return false;
	else if (!m_model->open(filename)) {
		MessageDialog::warning(tr("Cannot open index file:\n%1").arg(filename));
		return false;
	}
	m_filename = filename;
	setModified(false);
	return true;
}

void IndexWidget::save()
{
	qApp->setOverrideCursor(Qt::WaitCursor);
	m_model->save(m_filename);
	setModified(false);
	//m_history.clear();
	qApp->restoreOverrideCursor();
	emit saved(tr("Index saved to %1").arg(m_filename), 5000);
}

void IndexWidget::close()
{
	m_model->clear();
	m_filename.clear();
	ui.indexEdit->clear();
	//m_history.clear();
	setModified(false);
}

bool IndexWidget::queryClose()
{
	if (m_fileIndex.isModified()) {
		auto result = MessageDialog::yesNoCancel(tr("The index was modified. Do you want to save it?"));
		if (result == QMessageBox::Cancel)
			return false;
		else if (result == QMessageBox::Yes) {
			save();
			if (m_modified)
				return false;
		}
	}
	close();
	return true;
}

void IndexWidget::reload()
{
	if (m_filename.isEmpty())
		return;
	if (m_modified && !MessageDialog::yesNoQuestion(tr("Do you want to reload index, losing all changes?")))
		return;
	m_modified = false;
	open(m_filename);
}





void IndexWidget::addEntry(const Entry& entry)
{
	m_model->addEntry(entry);
	setModified(true);
}

void IndexWidget::updateEntry(const QUrl &link)
{
	QModelIndex index = ui.indexList->currentIndex();
	if (index.isValid()) {
		m_model->setData(index, link, IndexModel::EntryLinkRole);
		setModified(true);
		activateEntry(index);
	}
}

void IndexWidget::activateEntry(const QModelIndex& index)
{
	QModelIndex current = index.isValid() ? index : ui.indexList->currentIndex();
	QUrl url = current.data(IndexModel::EntryLinkRole).toUrl();
	emit documentRequested(DjVuLink(url));
}




void IndexWidget::menuRequested(const QPoint& position)
{
	QModelIndex index = ui.indexList->indexAt(position);

	QMenu menu;
	if (index.isValid()) {
		menu.addAction(ui.actionEditEntry);
		menu.addAction(ui.actionDeleteEntry);
		menu.addSeparator();
	}
	QMenu* sortMenu = menu.addMenu(tr("Sort order"));
	sortMenu->addActions(m_sortGroup->actions());
	menu.exec(ui.indexList->mapToGlobal(position));
}

void IndexWidget::editEntry()
{
	QModelIndex index = ui.indexList->currentIndex();
	if (!index.isValid())
		return;
	Entry entry = m_model->entry(index);
	EntryIndexDialog dlg(this);
	dlg.setEntry(entry);
	if (dlg.exec()) {
		m_model->setEntry(index, dlg.entry());
		setModified(true);
	}
}

void IndexWidget::deleteEntry()
{
	m_model->setData(ui.indexList->currentIndex(), ui.actionDeleteEntry->isChecked(), IndexModel::EntryDeletedRole);
	setModified(true);
}



void IndexWidget::findEntry()
{
	//doSearch(0, ui.indexEdit->text().trimmed());
}

void IndexWidget::entryTriggered()
{
	/*
	if (qApp->keyboardModifiers().testFlag(Qt::ControlModifier)) {
		if (ui.indexList->currentRow() == -1)
			findEntry();
		else doSearch(ui.indexList->currentRow() + 1, ui.indexEdit->text().trimmed());
	}
	else showCurrent();
	*/
}


void IndexWidget::sort()
{
	/*
	FileIndex::SortOrder order = FileIndex::OriginalOrder;
	if (ui.actionAlphabeticOrder->isChecked())
		order = FileIndex::AlphabeticOrder;
	else if (ui.actionAtergoOrder->isChecked())
		order = FileIndex::AtergoOrder;
	else if (ui.actionLetterOrder->isChecked())
		order = FileIndex::LetterOrder;

	QApplication::setOverrideCursor(Qt::WaitCursor);
	m_fileIndex.sort(order);

	ui.indexList->clear();
	for (int i = 0; i < m_fileIndex.count(); i++) {
		QListWidgetItem* item = new QListWidgetItem;
		ui.indexList->addItem(item);
		updateItem(i);
	}
	QApplication::restoreOverrideCursor();
	*/
}

void IndexWidget::indexChanged(int row)
{
	/*
	if (item)
		m_history.add(item);
	QString previous;
	if (m_history.hasPrevious())
		previous = m_history.previous()->text();
	QString next;
	if (m_history.hasNext())
		next = m_history.next()->text();
	emit historyChanged(previous, next);
	*/
}

void IndexWidget::setModified(bool enabled)
{
	m_modified = enabled;
	QString label;
	if (m_filename.isEmpty())
		label = tr("No index");
	else {
		QString flag = m_modified ? "[*] " : "";
		label = flag + m_filename;
	}
	ui.indexGroup->setTitle(label);
}


void IndexWidget::configure()
{
	m_model->configure();
	/*
	int iconHeight = ui.indexList->fontMetrics().height();
	m_commentIcon = QPixmap(":/images/comment.png").scaled(iconHeight, iconHeight,
																	Qt::KeepAspectRatio,
																	Qt::SmoothTransformation);
	for (int i = 0; i < ui.indexList->count(); i++)
		ui.indexList->item(i)->setFont(listFont);
		*/
}

void IndexWidget::showNextEntry()
{
	/*
	m_history.forward();
	if (m_history.current())
		ui.indexList->setCurrentItem(m_history.current());
		*/
}

void IndexWidget::showPreviousEntry()
{
//	m_history.back();
//	if (m_history.current())
//		ui.indexList->setCurrentItem(m_history.current());
}

void IndexWidget::append()
{
//	QString filename = MessageDialog::openFile(tr("CSV files (*.csv)"), tr("Select index file"),
//															 "Index");
//	if (filename.isEmpty())
//		return;
//	FileIndex index;
//	if (!index.open(filename)) {
//		MessageDialog::warning(tr("Cannot open index file:\n%1").arg(filename));
//		return;
//	}
//	for (int i = 0; i < index.count(); i++) {
//		Entry entry = index.entry(i);
//		if (m_fileIndex.appendEntry(entry)) {
//			QListWidgetItem* item = new QListWidgetItem(entry.word);
//			item->setToolTip(entry.comment);
//			ui.indexList->addItem(item);
//		}
//	}
//	updateList();
}


void IndexWidget::doSearch(int start, const QString &text)
{
//	Qt::CaseSensitivity cs = (text.toLower() == text) ? Qt::CaseInsensitive
//																	  : Qt::CaseSensitive;

//	bool substring = text.startsWith("*");
//	QString searchText = substring ? text.mid(1) : text;

//	bool atergo = ui.actionAtergoOrder->isChecked();
//	QString pattern;
//	if (atergo)
//		for (int i = searchText.count() - 1; i >= 0; i--)
//			pattern.append(searchText[i]);
//	else pattern = searchText;

//	for (int i = start; i < m_fileIndex.count(); i++) {
//		bool match = false;
//		QString word = m_fileIndex.entry(i).word;
//		if (substring)
//			match =  word.contains(pattern, cs);
//		else if (atergo)
//			match = word.endsWith(pattern, cs);
//		else match = word.startsWith(pattern, cs);
//		if (match) {
//			ui.indexList->setCurrentItem(ui.indexList->item(i));
//			ui.indexList->scrollToItem(ui.indexList->item(i), QListWidget::PositionAtTop);
//			break;
//		}
//	}
}
