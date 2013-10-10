/****************************************************************************
*   Copyright (C) 2013 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "indexwidget.h"
#include "messagedialog.h"

IndexWidget::IndexWidget(QWidget *parent) :
	QWidget(parent)
{
	ui.setupUi(this);

	ui.indexList->addAction(ui.actionHideEntry);

	connect(ui.indexEdit, SIGNAL(textEdited(QString)), this, SLOT(findEntry()));
	connect(ui.indexEdit, SIGNAL(returnPressed()), this, SLOT(findNextEntry()));
	connect(ui.indexList, SIGNAL(activated(QModelIndex)), this, SLOT(showCurrent()));
	connect(ui.indexList, SIGNAL(currentRowChanged(int)), this, SLOT(updateActions()));

	connect(ui.actionHideEntry, SIGNAL(triggered()), this, SLOT(hideCurrent()));
	connect(ui.actionShowEntry, SIGNAL(triggered()), this, SLOT(unhideCurrent()));
	connect(ui.actionEditComment, SIGNAL(triggered()), this, SLOT(editComment()));
	connect(ui.actionViewHidden, SIGNAL(toggled(bool)), this, SLOT(updateList()));

	m_sortGroup = new QActionGroup(this);
	m_sortGroup->setExclusive(true);
	m_sortGroup->addAction(ui.actionOriginalOrder);
	m_sortGroup->addAction(ui.actionAlphabeticOrder);
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
	ui.indexList->addAction(ui.actionEditComment);
	ui.indexList->addAction(separatorAction);
	ui.indexList->addAction(sortAction);
	ui.indexList->addAction(ui.actionViewHidden);

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


void IndexWidget::addEntry(const QString &word, const QUrl &link)
{
	if (m_fileIndex.addWord(word, link)) {
		QListWidgetItem* item = new QListWidgetItem(word);
		ui.indexList->addItem(item);
		ui.indexList->setCurrentItem(item);
		showCurrent();
	}
	else MessageDialog::warning(tr("The entry '%1' already exists").arg(word.trimmed()));
}

void IndexWidget::updateCurrentEntry(const QUrl &link)
{
	if (QListWidgetItem* item = ui.indexList->currentItem()) {
		m_fileIndex.setLink(item->text(), link);
		if (link.isValid())
			item->setForeground(Qt::black);
		showCurrent();
	}
}

void IndexWidget::findEntry()
{
	doSearch(0, ui.indexEdit->text().trimmed());
}

void IndexWidget::findNextEntry()
{
	if (ui.indexList->currentRow() == -1)
		findEntry();
	else doSearch(ui.indexList->currentRow() + 1, ui.indexEdit->text().trimmed());
}

void IndexWidget::showCurrent()
{
	int row = ui.indexList->currentRow();
	if (row == -1)
		return;
	QUrl url = m_fileIndex.link(ui.indexList->item(row)->text());
	if (!url.isEmpty())
		emit documentRequested(DjVuLink(url));
}

void IndexWidget::editComment()
{
	int row = ui.indexList->currentRow();
	if (row == -1)
		return;
	QString word = ui.indexList->item(row)->text().trimmed();
	QString comment = QInputDialog::getText(this, tr("Edit comment"), tr("Comment:"),
														 QLineEdit::Normal,
														 m_fileIndex.comment(word));
	if (!comment.isEmpty())
		m_fileIndex.setComment(word, comment);
}

void IndexWidget::hideCurrent()
{
	int row = ui.indexList->currentRow();
	if (row != -1) {
		m_fileIndex.hide(ui.indexList->item(row)->text());
		if (ui.actionViewHidden->isChecked()) {
			updateItem(ui.indexList->item(row), m_fileIndex.entry(ui.indexList->item(row)->text()));
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
		m_fileIndex.show(ui.indexList->item(row)->text());
		updateItem(ui.indexList->item(row), m_fileIndex.entry(ui.indexList->item(row)->text()));
		updateActions();
	}
}

void IndexWidget::updateList()
{
	int flags = FileIndex::OriginalOrder;
	if (ui.actionAlphabeticOrder->isChecked())
		flags |= FileIndex::AlphabeticOrder;
	else if (ui.actionAtergoOrder->isChecked())
		flags |= FileIndex::AtergoOrder;

	if (ui.actionViewHidden->isChecked())
		flags |= FileIndex::ViewHidden;

	ui.indexList->clear();
	QList<FileIndex::Entry> entries = m_fileIndex.items(flags);
	for (int i = 0; i < entries.count(); i++) {
		QListWidgetItem* item = new QListWidgetItem;
		updateItem(item, entries[i]);
		ui.indexList->addItem(item);
	}
}

void IndexWidget::updateActions()
{
	QListWidgetItem* item = ui.indexList->currentItem();
	FileIndex::Entry entry;
	if (item)
		entry = m_fileIndex.entry(item->text());

	ui.actionShowEntry->setVisible(item != 0 && !entry.isVisible());
	ui.actionHideEntry->setVisible(item != 0 && entry.isVisible());
	ui.actionEditComment->setVisible(item != 0);
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
	for (int i = start; i < ui.indexList->count(); i++)
		if (ui.indexList->item(i)->text().startsWith(text)) {
			ui.indexList->setCurrentItem(ui.indexList->item(i));
			break;
		}
}

void IndexWidget::updateItem(QListWidgetItem *item, const FileIndex::Entry &entry) const
{
	item->setText(entry.word);
	if (entry.link.isEmpty())
		item->setForeground(Qt::darkGray);
	if (ui.actionAtergoOrder->isChecked())
		item->setTextAlignment(Qt::AlignRight);
	if (!entry.isVisible()) {
		QFont strikeFont = ui.indexList->font();
		strikeFont.setStrikeOut(true);
		item->setFont(strikeFont);
	}
	else item->setFont(ui.indexList->font());
}
