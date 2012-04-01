/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf <michal@rudolf.waw.pl>              *
****************************************************************************/

#include "poliqarpwidget.h"
#include "poliqarp.h"
#include "messagedialog.h"
#include "djvuitemlist.h"


PoliqarpWidget::PoliqarpWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
    ui.textResultTable->verticalHeader()->setDefaultSectionSize(
                1.2 * ui.textResultTable->verticalHeader()->fontMetrics().height());

    connect(ui.connectButton, SIGNAL(clicked()), this, SLOT(connectToServer()));
    connect(ui.searchButton, SIGNAL(clicked()), this, SLOT(doSearch()));
    connect(ui.corpusCombo, SIGNAL(currentIndexChanged(int)), this,
              SLOT(doSelectSource()));
    connect(ui.textResultTable, SIGNAL(doubleClicked(QModelIndex)), this,
              SLOT(showDocument(QModelIndex)));
    connect(ui.graphicalResultList, SIGNAL(documentRequested(DjVuLink)), this,
              SIGNAL(documentRequested(DjVuLink)));
    connect(ui.queryCombo->lineEdit(), SIGNAL(returnPressed()), this,
              SLOT(doSearch()));
    connect(ui.resultWidget, SIGNAL(currentChanged(int)), this,
            SLOT(displayModeChanged()));

    m_poliqarp = new Poliqarp(this);
    connect(m_poliqarp, SIGNAL(connected(QStringList)), this,
              SLOT(connected(QStringList)));
    connect(m_poliqarp, SIGNAL(connectionError(QString)), this,
              SLOT(connectionError(QString)));
    connect(m_poliqarp, SIGNAL(sourceSelected()), this,
              SLOT(sourceSelected()));
    connect(m_poliqarp, SIGNAL(queryFinished()), this,
              SLOT(updateQueries()));

    connect(ui.moreButton, SIGNAL(clicked()), m_poliqarp, SLOT(fetchMore()));

    QSettings settings;
    settings.beginGroup("Poliqarp");
    ui.queryCombo->addItems(settings.value("queries").toStringList());
    ui.queryCombo->clearEditText();
    settings.endGroup();

}


PoliqarpWidget::~PoliqarpWidget()
{
    QSettings settings;
    settings.beginGroup("Poliqarp");
    QStringList items;
    for (int i = 0; i < ui.queryCombo->count(); i++)
        items.append(ui.queryCombo->itemText(i));
    settings.setValue("queries", items);
    settings.endGroup();
}

void PoliqarpWidget::connectToServer()
{
    QUrl url;
    url.setHost(ui.urlCombo->currentText());

    // MRTODO: remove when Kanji is ported
    if (url.host().contains("wbl"))
        url.setPath("/en");
    url.setScheme("http");
    if (!url.isValid()) {
        MessageDialog::warning(tr("This URL is not valid"));
        return;
    }
    ui.corpusCombo->clear();
    ui.connectButton->setEnabled(false);
    clearQueries();
    setCursor(QCursor(Qt::WaitCursor));
    m_poliqarp->connectToServer(url);
}

void PoliqarpWidget::doSearch()
{
    clearQueries();
    setCursor(QCursor(Qt::WaitCursor));
    m_poliqarp->runQuery(ui.queryCombo->currentText());
}

void PoliqarpWidget::doSelectSource()
{
    ui.searchButton->setEnabled(false);
    clearQueries();
    m_poliqarp->setCurrentSource(ui.corpusCombo->currentIndex());
}

void PoliqarpWidget::connected(const QStringList& sources)
{
    unsetCursor();
    ui.corpusCombo->addItems(sources);
    ui.connectButton->setEnabled(true);
    ui.corpusCombo->setEnabled(true);
    QSettings settings;
    int lastIndex = settings.value(QString("Poliqarp/") +
                                             m_poliqarp->serverUrl().host(), 0).toInt();
    ui.corpusCombo->setCurrentIndex(lastIndex);
}

void PoliqarpWidget::connectionError(const QString &message)
{
    unsetCursor();
    ui.connectButton->setEnabled(true);
    MessageDialog::warning(message);
}

void PoliqarpWidget::sourceSelected()
{
    ui.searchButton->setEnabled(true);
    ui.queryCombo->setFocus();
    QSettings settings;
    settings.setValue(QString("Poliqarp/") + m_poliqarp->serverUrl().host(),
                            ui.corpusCombo->currentIndex());
}

void PoliqarpWidget::updateQueries()
{
    unsetCursor();
    updateTextQueries();
    updateGraphicalQueries();

    ui.moreButton->setEnabled(m_poliqarp->hasMore());
}

void PoliqarpWidget::showDocument(const QModelIndex& index)
{
    QHeaderView* header = ui.textResultTable->horizontalHeader();
    ui.textResultTable->resizeColumnToContents(1);
    int sizeLeft = header->width() - header->sectionSize(1) - 20;
    header->resizeSection(0, sizeLeft / 2 - 2);
    header->resizeSection(2, sizeLeft / 2 - 2);
    if (index.isValid()) {
        DjVuLink item = m_poliqarp->query(index.row());
        if (item.link().isValid())
            emit documentRequested(item);
    }
}

void PoliqarpWidget::displayModeChanged()
{
    if (ui.resultWidget->currentIndex() == 0)
        adjustTextColumns();
}

void PoliqarpWidget::updateTextQueries()
{
    int oldCount = ui.textResultTable->rowCount();
    ui.textResultTable->setRowCount(m_poliqarp->queryCount());
    QFont boldFont = ui.textResultTable->font();
    boldFont.setBold(true);
    for (int i = oldCount; i < m_poliqarp->queryCount(); i++) {
        DjVuLink item = m_poliqarp->query(i);
        QTableWidgetItem* left = new QTableWidgetItem(item.leftContext());
        left->setTextAlignment(Qt::AlignRight);
        ui.textResultTable->setItem(i, 0, left);

        QTableWidgetItem* center = new QTableWidgetItem(item.word());
        center->setTextAlignment(Qt::AlignCenter);
        center->setFont(boldFont);
        center->setForeground(Qt::darkBlue);
        ui.textResultTable->setItem(i, 1, center);

        QTableWidgetItem* right = new QTableWidgetItem(item.rightContext());
        right->setTextAlignment(Qt::AlignLeft);
        ui.textResultTable->setItem(i, 2, right);
    }
    if (m_poliqarp->queryCount() == 0)
        ui.matchLabel->setText(tr("No matches"));
    else {
        QString text = ui.queryCombo->currentText();
        int old = ui.queryCombo->findText(text);
        if (old != -1)
            ui.queryCombo->removeItem(old);
        else if (ui.queryCombo->count() >= 20)
            ui.queryCombo->removeItem(ui.queryCombo->count() - 1);
        ui.queryCombo->insertItem(0, text);
        ui.queryCombo->setCurrentIndex(0);
        ui.matchLabel->setText(tr("Matches %1 of %2")
                                      .arg(m_poliqarp->queryCount())
                                      .arg(m_poliqarp->matchesFound()));
    }

    if (ui.textResultTable->isVisible())
        adjustTextColumns();
}

void PoliqarpWidget::updateGraphicalQueries()
{
    // Graphical results
    int oldCount = ui.graphicalResultList->count();
    for (int i = oldCount; i < m_poliqarp->queryCount(); i++)
        ui.graphicalResultList->addItem(m_poliqarp->query(i));
}

void PoliqarpWidget::clearQueries()
{
    ui.textResultTable->setRowCount(0);
    ui.graphicalResultList->clear();
    ui.matchLabel->clear();
    emit documentRequested(DjVuLink());
}

void PoliqarpWidget::adjustTextColumns()
{
    // Resize columns
    QHeaderView* header = ui.textResultTable->horizontalHeader();
    ui.textResultTable->resizeColumnToContents(1);
    int sizeLeft = header->width() - header->sectionSize(1) - 20;
    header->resizeSection(0, sizeLeft / 2 - 5);
    header->resizeSection(2, sizeLeft / 2 - 5);
    qDebug() << "SL" << sizeLeft << header->width() << header->sectionSize(1);
}



