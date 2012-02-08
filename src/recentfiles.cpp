/****************************************************************************
*   Copyright (C) 2012 by Michal Rudolf
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

#include "recentfiles.h"

RecentFiles::RecentFiles(QWidget* parent, int limit) :
	QObject(parent), m_limit(limit)
{
	m_menu = new QMenu(parent);
	connect(m_menu, SIGNAL(triggered(QAction*)), this, SLOT(triggered(QAction*)));
	for (int i = 0; i < m_limit; i++) {
		QAction* action = new QAction(parent);
		action->setVisible(false);
		m_menu->addAction(action);
	}
	readSettings();
}

RecentFiles::~RecentFiles()
{
	saveSettings();
}

void RecentFiles::plug(QAction* action)
{
	action->setMenu(m_menu);
}

void RecentFiles::addFile(const QString& filename)
{
	m_files.removeAll(filename);
	m_files.prepend(filename);
	if (m_files.size() > m_limit)
		m_files.removeLast();
	updateActions();
}

void RecentFiles::removeFile(const QString& filename)
{
	m_files.removeAll(filename);
	updateActions();
}

bool RecentFiles::readSettings()
{
	QSettings settings;
	m_files = settings.value("Recent/Files").toStringList();
	updateActions();
	return !m_files.empty();
}

void RecentFiles::saveSettings()
{
	QSettings settings;
	settings.setValue("Recent/Files", m_files);
}

QString RecentFiles::strippedFile(const QString& filename) const
{
	return QFileInfo(filename).fileName();
}

void RecentFiles::updateActions()
{
	QList<QAction*> actions = m_menu->actions();
	for (int i = 0; i < actions.count(); i++)
		if (i < m_files.count()) {
			QString text = QString("&%1 %2").arg(i + 1).arg(strippedFile(m_files[i]));
			actions[i]->setText(text);
			actions[i]->setData(m_files[i]);
			actions[i]->setVisible(true);
		}
	else actions[i]->setVisible(false);
}

void RecentFiles::triggered(QAction *action)
{
	emit selected(action->data().toString());
}
