/****************************************************************************
*   Copyright (C) 2017 by Michal Rudolf
*   This software is subject to, and may be distributed under, the
*   GNU General Public License, either version 2 of the license,
*   or (at your option) any later version. The license should have
*   accompanied the software or you may obtain a copy of the license
*   from the Free Software Foundation at http://www.fsf.org .
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
****************************************************************************/

#include "indexmodel.h"
#include "indexwidget.h"
#include "messagedialog.h"
#include "entryindexdialog.h"

IndexWidget::IndexWidget(QWidget *parent) :
	QWidget(parent), m_modified(false)
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
	connect(ui.indexEdit, &QLineEdit::returnPressed, this, &IndexWidget::findNextEntry);
	connect(ui.indexList->selectionModel(), &QItemSelectionModel::currentChanged, this, &IndexWidget::currentIndexChanged);
	connect(ui.indexList, &QAbstractItemView::customContextMenuRequested, this, &IndexWidget::menuRequested);
	connect(ui.commentEdit, &QLineEdit::textEdited, this, &IndexWidget::editComment);

	connect(ui.actionDeleteEntry, &QAction::toggled, this, &IndexWidget::deleteEntry);
	connect(ui.actionReloadEntry, &QAction::triggered, this, &IndexWidget::reloadEntry);
	connect(ui.actionEditEntry, &QAction::triggered, this, &IndexWidget::editEntry);
	connect(ui.actionFind, &QAction::triggered, ui.indexEdit, static_cast<void (QLineEdit::*)()>(&QLineEdit::setFocus));

	addAction(ui.actionDeleteEntry);
	addAction(ui.actionEditEntry);
	addAction(ui.actionReloadEntry);
	addAction(ui.actionFind);

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

	QSettings settings;
	QString sortOrder = settings.value("Index/sorting").toString();
	foreach (QAction* action, m_sortGroup->actions())
		if (action->objectName() == sortOrder)
			action->setChecked(true);

	connect(m_sortGroup, &QActionGroup::triggered, this, &IndexWidget::sort);

	configure();
}

IndexWidget::~IndexWidget()
{
	QSettings settings;
	settings.beginGroup("Index");
	settings.setValue("sorting", m_sortGroup->checkedAction()->objectName());
	settings.endGroup();
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
	emit opened(filename);
	return true;
}

void IndexWidget::save()
{
	qApp->setOverrideCursor(Qt::WaitCursor);
	bool success = m_model->save(m_filename);
	qApp->restoreOverrideCursor();
	if (success) {
		setModified(false);
		emit saved(tr("Index saved to %1").arg(m_filename), 5000);
	}
	else MessageDialog::error(tr("Cannot save index to %1").arg(m_filename));
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
		QMessageBox::StandardButton result = MessageDialog::yesNoCancel(tr("The index was modified. Do you want to save it?"));
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
	QString reopen = m_filename; // m_filename is cleared on close()
	if (m_modified && !MessageDialog::yesNoQuestion(tr("Do you want to reload index, losing all changes?")))
		return;
	m_modified = false;
	open(reopen);
}

QString IndexWidget::filename() const
{
	return m_filename;
}





void IndexWidget::addEntry(const Entry& entry)
{
	m_model->addEntry(entry);
	QModelIndex lastItem = m_model->index(m_model->rowCount() - 1, 0);
	QModelIndex lastSortedItem = m_sortModel->mapFromSource(lastItem);
	ui.indexList->setCurrentIndex(lastSortedItem);
	setModified(true);
}

void IndexWidget::updateEntry(const QUrl &link)
{
	QModelIndex currentSorted = ui.indexList->currentIndex();
	QModelIndex current = currentEntry();
	if (current.isValid()) {
		m_model->setData(current, link, IndexModel::EntryLinkRole);
		setModified(true);
		currentIndexChanged(currentSorted);
	}
}

void IndexWidget::currentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);
	if (current.isValid())
		m_history.add(m_sortModel->mapToSource(current));
	ui.commentEdit->setText(current.data(IndexModel::EntryCommentRole).toString());
	ui.commentEdit->setReadOnly(!current.isValid());
	QString back;
	if (m_history.hasPrevious())
		back = m_history.previous().data(Qt::DisplayRole).toString();
	QString forward;
	if (m_history.hasNext())
		forward = m_history.next().data(Qt::DisplayRole).toString();
	emit historyChanged(back, forward);
	QUrl url = current.data(IndexModel::EntryLinkRole).toUrl();
	if (!url.isEmpty())
		emit documentRequested(DjVuLink(m_urlReplacements.replace(url)));
}

void IndexWidget::menuRequested(const QPoint& position)
{
	QModelIndex index = m_sortModel->mapToSource(ui.indexList->indexAt(position));
	QMenu menu;
	if (index.isValid()) {
		menu.addAction(ui.actionEditEntry);
		if (m_model->isModified(index))
			menu.addAction(ui.actionReloadEntry);
		menu.addAction(ui.actionDeleteEntry);
		menu.addSeparator();
	}
	QMenu* sortMenu = menu.addMenu(tr("Sort order"));
	sortMenu->addActions(m_sortGroup->actions());
	menu.exec(ui.indexList->mapToGlobal(position));
}

void IndexWidget::editEntry()
{
	QModelIndex index = currentEntry();
	if (!index.isValid())
		return;
	Entry entry = m_model->entry(index);
	EntryIndexDialog dlg(this);
	dlg.setEntry(entry);
	if (dlg.exec()) {
		m_model->setEntry(index, dlg.entry());
		ui.commentEdit->setText(dlg.entry().comment());
		setModified(true);
	}
}

void IndexWidget::reloadEntry()
{
	m_model->reloadEntry(currentEntry());
}

void IndexWidget::deleteEntry()
{
	m_model->setData(currentEntry(), ui.actionDeleteEntry->isChecked(), IndexModel::EntryDeletedRole);
	setModified(true);
}

void IndexWidget::editComment()
{
	if (ui.indexList->currentIndex().isValid()) {
		setModified(true);
		m_model->setData(ui.indexList->currentIndex(), ui.commentEdit->text(), IndexModel::EntryCommentRole);
	}
}

void IndexWidget::findEntry()
{
	findEntryFrom(ui.indexList->currentIndex());

}

void IndexWidget::findNextEntry()
{
	QModelIndex index = m_sortModel->index(ui.indexList->currentIndex().row() + 1, 0);
	findEntryFrom(index);
}

void IndexWidget::findEntryFrom(const QModelIndex& index)
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

	QModelIndex start = index;
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
	ui.indexEdit->configure();
	m_model->configure();
	m_urlReplacements.read(QSettings().value("Edit/urlReplace").toString());
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
	m_model->addEntries(index);
	setModified(true);
	emit opened(filename);
}

QModelIndex IndexWidget::currentEntry() const
{
	return m_sortModel->mapToSource(ui.indexList->currentIndex());
}
