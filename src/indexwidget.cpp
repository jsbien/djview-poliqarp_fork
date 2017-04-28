
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
	connect(ui.indexEdit, &QLineEdit::returnPressed, this, &IndexWidget::findEntry);
	connect(ui.indexList->selectionModel(), &QItemSelectionModel::currentChanged, this, &IndexWidget::currentIndexChanged);
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
	if (m_modified) {
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
	QModelIndex last = m_sortModel->index(m_sortModel->rowCount() - 1, 0);
	ui.indexList->setCurrentIndex(last);
	setModified(true);
}

void IndexWidget::updateEntry(const QUrl &link)
{
	QModelIndex index = ui.indexList->currentIndex();
	if (index.isValid()) {
		m_model->setData(index, link, IndexModel::EntryLinkRole);
		setModified(true);
		currentIndexChanged(index);
	}
}

void IndexWidget::currentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);
	if (current.isValid())
		m_history.add(m_sortModel->mapToSource(current));
	QString back;
	if (m_history.hasPrevious())
		back = m_history.previous().data(Qt::DisplayRole).toString();
	QString forward;
	if (m_history.hasNext())
		forward = m_history.next().data(Qt::DisplayRole).toString();
	emit historyChanged(back, forward);
	QUrl url = current.data(IndexModel::EntryLinkRole).toUrl();
	if (!url.isEmpty())
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
	QString pattern = ui.indexEdit->text().trimmed();
	bool atergo = ui.actionAtergoOrder->isChecked();
	bool substring = pattern.startsWith("*");

	Qt::MatchFlags flags = Qt::MatchWrap;
	if (substring)
		flags |= Qt::MatchContains;
	else if (atergo)
		flags |= Qt::MatchEndsWith;
	else flags |= Qt::MatchStartsWith;
	if (pattern.toLower() != pattern)
		flags |= Qt::MatchCaseSensitive;

	if (substring)
		pattern = pattern.mid(1);
	if (atergo)
		std::reverse(pattern.begin(), pattern.end());

	QModelIndex start = m_sortModel->index(ui.indexList->currentIndex().row() + 1, 0);
	if (!start.isValid())
		 start = m_sortModel->index(0, 0);
	QModelIndexList results = m_sortModel->match(start, Qt::DisplayRole, pattern, 1, flags);
	if (!results.isEmpty()) {
		ui.indexList->setCurrentIndex(results.at(0));
		ui.indexList->scrollTo(results.at(0), QListView::PositionAtTop);
	}
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
		ui.indexList->setCurrentIndex(m_sortModel->mapFromSource(m_history.current()));
		ui.indexList->setFocus();
	}
}

void IndexWidget::showPreviousEntry()
{
	m_history.back();
	if (m_history.current().isValid()) {
		ui.indexList->setCurrentIndex(m_sortModel->mapFromSource(m_history.current()));
		ui.indexList->setFocus();
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
