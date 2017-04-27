
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
	m_sortModel = new QSortFilterProxyModel(this);
	m_sortModel->setSourceModel(m_model);
	m_sortModel->setSortRole(IndexModel::EntrySortRole);
	m_sortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_sortModel->setSortLocaleAware(true);
	ui.indexList->setModel(m_sortModel);

	connect(ui.indexEdit, &QLineEdit::textEdited, this, &IndexWidget::findEntry);
	connect(ui.indexEdit, &QLineEdit::returnPressed, this, &IndexWidget::entryTriggered);
	connect(ui.indexList, &QAbstractItemView::activated, this, &IndexWidget::activateEntry);
	connect(ui.indexList->selectionModel(), &QItemSelectionModel::currentChanged, this, &IndexWidget::addToHistory);
	connect(ui.indexList, &QAbstractItemView::customContextMenuRequested, this, &IndexWidget::menuRequested);

	connect(ui.actionDeleteEntry, &QAction::toggled, this, &IndexWidget::deleteEntry);
	connect(ui.actionEditEntry, &QAction::triggered, this, &IndexWidget::editEntry);

	addAction(ui.actionDeleteEntry);
	addAction(ui.actionEditEntry);

	m_sortGroup = new QActionGroup(this);
	m_sortGroup->setExclusive(true);
	ui.actionOriginalOrder->setData(IndexModel::SortByIndex);
	ui.actionAlphabeticOrder->setData(IndexModel::SortAlphabetically);
	ui.actionLetterOrder->setData(IndexModel::SortByLetters);
	ui.actionAtergoOrder->setData(IndexModel::SortAtergo);
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
	if (filename.isEmpty() || !queryClose())
		return false;
	QApplication::processEvents();
	if (!m_model->open(filename)) {
		MessageDialog::warning(tr("Cannot open index file:\n%1").arg(filename));
		return false;
	}
	m_filename = filename;
	setModified(false);
	sort();
	return true;
}

void IndexWidget::save()
{
	qApp->setOverrideCursor(Qt::WaitCursor);
	m_model->save(m_filename);
	setModified(false);
	qApp->restoreOverrideCursor();
	emit saved(tr("Index saved to %1").arg(m_filename), 5000);
}

void IndexWidget::close()
{
	m_model->clear();
	m_filename.clear();
	ui.indexEdit->clear();
	m_history.clear();
	emit historyChanged(QString(), QString());
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

void IndexWidget::addToHistory(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);
	if (current.isValid())
		m_history.add(current);
	QString back;
	if (m_history.hasPrevious())
		back = m_history.previous().data(Qt::DisplayRole).toString();
	QString forward;
	if (m_history.hasNext())
		forward = m_history.next().data(Qt::DisplayRole).toString();
	emit historyChanged(back, forward);
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
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QApplication::processEvents();
	IndexModel::SortMethod method = IndexModel::SortMethod(m_sortGroup->checkedAction()->data().toInt());
	m_model->setSortingMethod(method);
	m_sortModel->invalidate();
	m_sortModel->sort(method == IndexModel::SortByIndex ? -1 : 0, Qt::AscendingOrder);
	ui.indexList->scrollTo(ui.indexList->currentIndex());
	QApplication::restoreOverrideCursor();
}


void IndexWidget::setModified(bool enabled)
{
	m_modified = enabled;
	QString label;
	if (m_filename.isEmpty())
		label = tr("No index");
	else {
		QString flag = m_modified ? "[*] " : "";
		label = tr("%1%2: %L3 item(s)").arg(flag).arg(QFileInfo(m_filename).fileName()).arg(m_model->rowCount());
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
	m_history.forward();
	if (m_history.current().isValid()) {
		ui.indexList->setCurrentIndex(m_history.current());
		activateEntry(m_history.current());
	}
}

void IndexWidget::showPreviousEntry()
{
	m_history.back();
	if (m_history.current().isValid()) {
		ui.indexList->setCurrentIndex(m_history.current());
		activateEntry(m_history.current());
	}
}

void IndexWidget::append()
{
	QString filename = MessageDialog::openFile(tr("CSV files (*.csv)"), tr("Select index file"),
															 "Index");
	if (filename.isEmpty())
		return;
	EntryList index;
	if (!index.open(filename)) {
		MessageDialog::warning(tr("Cannot open index file:\n%1").arg(filename));
		return;
	}
	for (const Entry& e : index)
		m_model->addEntry(e);
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
