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
	m_actionNext = m_actionPrevious = 0;

	connect(ui.indexEdit, &QLineEdit::textEdited, this, &IndexWidget::findEntry);
	connect(ui.indexEdit, &QLineEdit::returnPressed, this, &IndexWidget::entryTriggered);
	connect(ui.indexList, &QAbstractItemView::activated, this, &IndexWidget::activateEntry);
	connect(ui.indexList, &QListWidget::currentRowChanged, this, &IndexWidget::indexChanged);

	connect(ui.actionHideEntry, &QAction::triggered, this, &IndexWidget::hideCurrent);
	connect(ui.actionShowEntry, &QAction::triggered, this, &IndexWidget::unhideCurrent);
	connect(ui.actionEditEntry, &QAction::triggered, this, &IndexWidget::editEntry);
	connect(ui.actionViewHidden, &QAction::toggled, this, &IndexWidget::updateList);

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

	configure();
}

IndexWidget::~IndexWidget()
{
	close();
}

bool IndexWidget::open(const QString &filename)
{
	close();
	if (filename.isEmpty())
		return false;
	else if (!m_fileIndex.open(filename)) {
		MessageDialog::warning(tr("Cannot open index file:\n%1").arg(filename));
		return false;
	}
	else {
		updateList();
		updateTitle();
		return true;
	}
}

void IndexWidget::setHistoryAction(QAction* previous, QAction* next)
{
	m_actionPrevious = previous;
	m_actionNext = next;
}


void IndexWidget::addEntry(const Entry& entry)
{
	if (m_fileIndex.appendEntry(entry)) {
		QListWidgetItem* item = new QListWidgetItem(entry.word);
		item->setToolTip(entry.comment);
		ui.indexList->addItem(item);
		ui.indexList->setCurrentItem(item);
		showCurrent();
		updateTitle();
	}
	else MessageDialog::warning(tr("The entry '%1' already exists").arg(entry.word.trimmed()));
}

void IndexWidget::updateCurrentEntry(const QUrl &link)
{
	int row = ui.indexList->currentRow();
	if (row != -1) {
		m_fileIndex.setLink(row, link);
		updateItem(row);
		updateTitle();
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
	QUrl url = m_fileIndex.entry(row).validLink();
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
		updateTitle();
	}
}

void IndexWidget::hideCurrent()
{
	int row = ui.indexList->currentRow();
	if (row != -1) {
		m_fileIndex.hide(row);
		if (ui.actionViewHidden->isChecked()) {
			updateItem(row);
			indexChanged(row);
		}
		else {
			delete ui.indexList->takeItem(row);
			ui.indexList->setCurrentRow(row);
		}
		updateTitle();
	}
}

void IndexWidget::unhideCurrent()
{
	int row = ui.indexList->currentRow();
	if (row != -1) {
		m_fileIndex.show(row);
		updateItem(row);
		indexChanged(row);
		updateTitle();
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

void IndexWidget::indexChanged(int row)
{
	Entry entry = m_fileIndex.entry(row);
	QListWidgetItem* item = ui.indexList->item(row);
	ui.actionShowEntry->setVisible(item && !entry.isVisible());
	ui.actionHideEntry->setVisible(item && entry.isVisible());
	ui.actionEditEntry->setVisible(item);
	if (item)
		m_history.add(item);
	if (m_actionPrevious) {
		m_actionPrevious->setEnabled(m_history.hasPrevious());
		QString previous = m_history.hasPrevious() ? m_history.previous()->text() : tr("None");
		m_actionPrevious->setText(tr("Previous entry: %1").arg(previous));
	}
	if (m_actionNext) {
		m_actionNext->setEnabled(m_history.hasNext());
		QString next = m_history.hasNext() ? m_history.next()->text() : tr("None");
		m_actionNext->setText(tr("Next entry: %1").arg(next));
	}
}

void IndexWidget::updateTitle()
{
	QString label;
	if (m_fileIndex.isEmpty())
		label = tr("No index");
	else {
		QString flag = m_fileIndex.isModified() ? "[*] " : "";
		label = flag + QFileInfo(m_fileIndex.filename()).fileName();
	}
	ui.indexGroup->setTitle(label);
}

void IndexWidget::save()
{
	qApp->setOverrideCursor(Qt::WaitCursor);
	m_fileIndex.save();
	qApp->restoreOverrideCursor();
	updateTitle();
	emit saved(tr("Index saved to %1").arg(m_fileIndex.filename()), 5000);
}

void IndexWidget::configure()
{
	QFont listFont;
	listFont.setStyleStrategy(QFont::NoFontMerging);
	listFont.fromString(QSettings().value("Display/textFont", listFont.toString()).toString());
	if (listFont.family().isEmpty())
		listFont = font();
	ui.indexList->setFont(listFont);

	int iconHeight = ui.indexList->fontMetrics().height();
	m_commentIcon = QPixmap(":/images/comment.png").scaled(iconHeight, iconHeight,
																	Qt::KeepAspectRatio,
																	Qt::SmoothTransformation);
	for (int i = 0; i < ui.indexList->count(); i++)
		ui.indexList->item(i)->setFont(listFont);
}

void IndexWidget::showNextEntry()
{
	m_history.forward();
	if (m_history.current())
		ui.indexList->setCurrentItem(m_history.current());
}

void IndexWidget::showPreviousEntry()
{
	m_history.back();
	if (m_history.current())
		ui.indexList->setCurrentItem(m_history.current());
}


void IndexWidget::close()
{
	if (m_fileIndex.isModified())
		save();
	m_fileIndex.clear();
	ui.indexList->clear();
	ui.indexEdit->clear();
	m_history.clear();
	updateTitle();
	indexChanged(-1);
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

	if (!entry.validLink().isValid())
		item->setForeground(Qt::darkGray);
	else item->setForeground(Qt::black);

	if (ui.actionAtergoOrder->isChecked())
		item->setTextAlignment(Qt::AlignRight);

	bool hasComment = !entry.formattedComment().isEmpty();
	item->setIcon(hasComment ? m_commentIcon : QIcon());

	QFont font = ui.indexList->font();
	if (!entry.isVisible())
		font.setStrikeOut(true);
	if (!entry.validLink().isValid())
		font.setItalic(true);
	item->setFont(font);
}
