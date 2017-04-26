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

	connect(ui.indexEdit, &QLineEdit::textEdited, this, &IndexWidget::findEntry);
	connect(ui.indexEdit, &QLineEdit::returnPressed, this, &IndexWidget::entryTriggered);
	connect(ui.indexList, &QAbstractItemView::activated, this, &IndexWidget::activateEntry);
	connect(ui.indexList, &QListWidget::currentRowChanged, this, &IndexWidget::indexChanged);

	connect(ui.actionHideEntry, &QAction::triggered, this, &IndexWidget::hideCurrent);
	connect(ui.actionShowEntry, &QAction::triggered, this, &IndexWidget::unhideCurrent);
	connect(ui.actionDeleteEntry, &QAction::triggered, this, &IndexWidget::deleteEntry);
	connect(ui.actionUndeleteEntry, &QAction::triggered, this, &IndexWidget::undeleteEntry);
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

	ui.indexList->addAction(ui.actionEditEntry);
	ui.indexList->addAction(ui.actionHideEntry);
	ui.indexList->addAction(ui.actionShowEntry);
	ui.indexList->addAction(ui.actionDeleteEntry);
	ui.indexList->addAction(ui.actionUndeleteEntry);
	ui.indexList->addAction(separatorAction);
	ui.indexList->addAction(sortAction);
	ui.indexList->addAction(ui.actionViewHidden);

	configure();
}

IndexWidget::~IndexWidget()
{
}

bool IndexWidget::open(const QString &filename)
{
	if (!queryClose())
		return false;
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
	showCurrent();
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

void IndexWidget::deleteEntry()
{
	toggleDeleted(ui.indexList->currentRow(), true);
}

void IndexWidget::undeleteEntry()
{
	toggleDeleted(ui.indexList->currentRow(), false);
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
	ui.actionDeleteEntry->setVisible(!entry.isDeleted());
	ui.actionUndeleteEntry->setVisible(entry.isDeleted());
	if (item)
		m_history.add(item);


	QString previous;
	if (m_history.hasPrevious())
		previous = m_history.previous()->text();
	QString next;
	if (m_history.hasNext())
		next = m_history.next()->text();
	emit historyChanged(previous, next);
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

void IndexWidget::close()
{
	m_fileIndex.clear();
	ui.indexList->clear();
	ui.indexEdit->clear();
	m_history.clear();
	updateTitle();
	indexChanged(-1);
}

void IndexWidget::save()
{
	qApp->setOverrideCursor(Qt::WaitCursor);
	m_fileIndex.save();
	m_history.clear();
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

void IndexWidget::reload()
{
	if (m_fileIndex.filename().isEmpty())
		return;
	if (m_fileIndex.isModified() && !MessageDialog::yesNoQuestion(tr("Do you want to reload index, losing all changes?")))
		return;
	QString filename = m_fileIndex.filename();
	close();
	open(filename);
}

void IndexWidget::append()
{
	QString filename = MessageDialog::openFile(tr("CSV files (*.csv)"), tr("Select index file"),
															 "Index");
	if (filename.isEmpty())
		return;
	FileIndex index;
	if (!index.open(filename)) {
		MessageDialog::warning(tr("Cannot open index file:\n%1").arg(filename));
		return;
	}
	for (int i = 0; i < index.count(); i++) {
		Entry entry = index.entry(i);
		if (m_fileIndex.appendEntry(entry)) {
			QListWidgetItem* item = new QListWidgetItem(entry.word);
			item->setToolTip(entry.comment);
			ui.indexList->addItem(item);
		}
	}
	updateList();
}

bool IndexWidget::queryClose()
{
	if (m_fileIndex.isModified()) {
		auto result = MessageDialog::yesNoCancel(tr("The index was modified. Do you want to save it?"));
		if (result == QMessageBox::Cancel)
			return false;
		else if (result == QMessageBox::Yes) {
			save();
			if (m_fileIndex.isModified())
				return false;
		}
	}
	close();
	return true;
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
		item->setForeground(Qt::red);
	else if (!entry.isVisible())
		item->setForeground(Qt::gray);
	else item->setForeground(Qt::black);

	if (ui.actionAtergoOrder->isChecked())
		item->setTextAlignment(Qt::AlignRight);

	bool hasComment = !entry.formattedComment().isEmpty();
	item->setIcon(hasComment ? m_commentIcon : QIcon());

	QFont font = ui.indexList->font();
	if (entry.isDeleted())
		font.setStrikeOut(true);
	if (!entry.validLink().isValid())
		font.setItalic(true);
	item->setFont(font);
}

void IndexWidget::toggleDeleted(int row, bool deleted)
{
	if (row == -1)
		return;
	Entry entry = m_fileIndex.entry(row);
	entry.setDeleted(deleted);
	m_fileIndex.setEntry(row, entry);
	updateItem(row);
}
