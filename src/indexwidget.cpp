/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "indexwidget.h"
#include "messagedialog.h"
#include "entryindexdialog.h"

IndexWidget::IndexWidget(QWidget *parent) :
	QWidget(parent)
{
	ui.setupUi(this);

	ui.indexList->addAction(ui.actionHideEntry);

	connect(ui.indexEdit, SIGNAL(textEdited(QString)), this, SLOT(findEntry()));
	connect(ui.indexEdit, SIGNAL(returnPressed()), this, SLOT(entryTriggered()));
	connect(ui.indexList, SIGNAL(activated(QModelIndex)), this, SLOT(activateEntry()));
	connect(ui.indexList, SIGNAL(currentRowChanged(int)), this, SLOT(updateActions()));

	connect(ui.actionHideEntry, SIGNAL(triggered()), this, SLOT(hideCurrent()));
	connect(ui.actionShowEntry, SIGNAL(triggered()), this, SLOT(unhideCurrent()));
	connect(ui.actionEditEntry, SIGNAL(triggered()), this, SLOT(editEntry()));
	connect(ui.actionViewHidden, SIGNAL(toggled(bool)), this, SLOT(updateList()));

	m_sortGroup = new QActionGroup(this);
	m_sortGroup->setExclusive(true);
	m_sortGroup->addAction(ui.actionOriginalOrder);
	m_sortGroup->addAction(ui.actionAlphabeticOrder);
	m_sortGroup->addAction(ui.actionLetterOrder);
	m_sortGroup->addAction(ui.actionAtergoOrder);
	connect(m_sortGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateList()));

	QMenu* sortMenu = new QMenu(this);
	sortMenu->addActions(m_sortGroup->actions());

	QAction* sortAction = new QAction(tr("Sort order"), this);
	sortAction->setMenu(sortMenu);

	QAction* separatorAction = new QAction(this);
	separatorAction->setSeparator(true);

	ui.indexList->addAction(ui.actionHideEntry);
	ui.indexList->addAction(ui.actionShowEntry);
	ui.indexList->addAction(ui.actionEditEntry);
	ui.indexList->addAction(separatorAction);
	ui.indexList->addAction(sortAction);
	ui.indexList->addAction(ui.actionViewHidden);

	int iconHeight = ui.indexList->fontMetrics().height();
	m_commentIcon = QPixmap(":/images/comment.png").scaled(iconHeight, iconHeight,
																	Qt::KeepAspectRatio,
																	Qt::SmoothTransformation);

	hide();
}

IndexWidget::~IndexWidget()
{
	if (m_fileIndex.isModified())
		m_fileIndex.save();
}

void IndexWidget::open(const QString &corpus)
{
	QString filename = QSettings().value(QString("%1/file_index").arg(corpus)).toString();
	if (filename == m_fileIndex.filename())
		return;
	close();

	if (filename.isEmpty()) {
		hide();
		emit indexClosed();
	}
	else {
		m_fileIndex.open(filename);
		show();
		updateList();
		emit indexOpened();
	}
}


void IndexWidget::addEntry(const Entry& entry)
{
	if (m_fileIndex.appendEntry(entry)) {
		QListWidgetItem* item = new QListWidgetItem(entry.word);
		item->setToolTip(entry.comment);
		ui.indexList->addItem(item);
		ui.indexList->setCurrentItem(item);
		showCurrent();
	}
	else MessageDialog::warning(tr("The entry '%1' already exists").arg(entry.word.trimmed()));
}

void IndexWidget::updateCurrentEntry(const QUrl &link)
{
	int row = ui.indexList->currentRow();
	if (row != -1) {
		m_fileIndex.setLink(row, link);
		updateItem(row);
		showCurrent();
	}
}

void IndexWidget::activateEntry()
{
	if (qApp->keyboardModifiers().testFlag(Qt::ControlModifier))
		editEntry();
	else showCurrent();
}

void IndexWidget::findEntry()
{
	doSearch(0, ui.indexEdit->text().trimmed());
}

void IndexWidget::entryTriggered()
{
	if (qApp->keyboardModifiers().testFlag(Qt::ControlModifier)) {
		if (ui.indexList->currentRow() == -1)
			findEntry();
		else doSearch(ui.indexList->currentRow() + 1, ui.indexEdit->text().trimmed());
	}
	else showCurrent();
}

void IndexWidget::showCurrent()
{
	int row = ui.indexList->currentRow();
	if (row == -1)
		return;
	QUrl url = m_fileIndex.validLink(row);
	if (url.isValid() && !qApp->keyboardModifiers().testFlag(Qt::ControlModifier))
		emit documentRequested(DjVuLink(url));
	else editEntry();
}

void IndexWidget::editEntry()
{
	int row = ui.indexList->currentRow();
	if (row == -1)
		return;

	Entry entry = m_fileIndex.entry(row);
	EntryIndexDialog dlg(this);
	dlg.setEntry(entry);
	if (dlg.exec()) {
		entry = dlg.entry();
		m_fileIndex.setEntry(row, entry);
		if (!entry.isVisible() && !ui.actionViewHidden->isChecked()) {
			delete ui.indexList->takeItem(row);
			ui.indexList->setCurrentRow(row);
		}
		else updateItem(row);
	}
}

void IndexWidget::hideCurrent()
{
	int row = ui.indexList->currentRow();
	if (row != -1) {
		m_fileIndex.hide(row);
		if (ui.actionViewHidden->isChecked()) {
			updateItem(row);
			updateActions();
		}
		else {
			delete ui.indexList->takeItem(row);
			ui.indexList->setCurrentRow(row);
		}
	}
}

void IndexWidget::unhideCurrent()
{
	int row = ui.indexList->currentRow();
	if (row != -1) {
		m_fileIndex.show(row);
		updateItem(row);
		updateActions();
	}
}

void IndexWidget::updateList()
{
	FileIndex::SortOrder order = FileIndex::OriginalOrder;
	if (ui.actionAlphabeticOrder->isChecked())
		order = FileIndex::AlphabeticOrder;
	else if (ui.actionAtergoOrder->isChecked())
		order = FileIndex::AtergoOrder;
	else if (ui.actionLetterOrder->isChecked())
		order = FileIndex::LetterOrder;

	m_fileIndex.showHidden(ui.actionViewHidden->isChecked());

	QApplication::setOverrideCursor(Qt::WaitCursor);
	m_fileIndex.sort(order);

	ui.indexList->clear();
	for (int i = 0; i < m_fileIndex.count(); i++) {
		QListWidgetItem* item = new QListWidgetItem;
		ui.indexList->addItem(item);
		updateItem(i);
	}
	QApplication::restoreOverrideCursor();
}

void IndexWidget::updateActions()
{
	int row = ui.indexList->currentRow();
	Entry entry = m_fileIndex.entry(row);
	ui.actionShowEntry->setVisible(row != -1 && !entry.isVisible());
	ui.actionHideEntry->setVisible(row != -1 && entry.isVisible());
	ui.actionEditEntry->setVisible(row != -1);
}

void IndexWidget::close()
{
	if (m_fileIndex.isModified())
		m_fileIndex.save();
	m_fileIndex.clear();
	ui.indexList->clear();
	ui.indexEdit->clear();
}

void IndexWidget::doSearch(int start, const QString &text)
{
	Qt::CaseSensitivity cs = (text.toLower() == text) ? Qt::CaseInsensitive
																	  : Qt::CaseSensitive;

	bool substring = text.startsWith("*");
	QString searchText = substring ? text.mid(1) : text;

	bool atergo = ui.actionAtergoOrder->isChecked();
	QString pattern;
	if (atergo)
		for (int i = searchText.count() - 1; i >= 0; i--)
			pattern.append(searchText[i]);
	else pattern = searchText;

	for (int i = start; i < m_fileIndex.count(); i++) {
		bool match = false;
		QString word = m_fileIndex.entry(i).word;
		if (substring)
			match =  word.contains(pattern, cs);
		else if (atergo)
			match = word.endsWith(pattern, cs);
		else match = word.startsWith(pattern, cs);
		if (match) {
			ui.indexList->setCurrentItem(ui.indexList->item(i));
			ui.indexList->scrollToItem(ui.indexList->item(i), QListWidget::PositionAtTop);
			break;
		}
	}
}

void IndexWidget::updateItem(int row) const
{
	if (row == -1)
		return;
	Entry entry = m_fileIndex.entry(row);
	QListWidgetItem* item = ui.indexList->item(row);

	item->setText(entry.word);
	item->setToolTip(entry.comment);

	if (!entry.link.isValid())
		item->setForeground(Qt::darkGray);
	else item->setForeground(Qt::black);

	if (ui.actionAtergoOrder->isChecked())
		item->setTextAlignment(Qt::AlignRight);

	bool hasComment = !entry.formattedComment().isEmpty();
	item->setIcon(hasComment ? m_commentIcon : QIcon());

	if (!entry.isVisible()) {
		QFont strikeFont = ui.indexList->font();
		strikeFont.setStrikeOut(true);
		item->setFont(strikeFont);
	}
	else item->setFont(ui.indexList->font());
}
