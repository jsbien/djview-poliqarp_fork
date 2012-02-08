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

#ifndef __RECENTFILES_H__
#define __RECENTFILES_H__

#include <QtGui>

class RecentFiles : public QObject {
	Q_OBJECT
public:
	/** Create a list of recent files and read history. */
	RecentFiles(QWidget* parent = 0, int limit = 5);
	/** Save settings on quit. */
	~RecentFiles();
	/** Add menu with recent files to given actions. */
	void plug(QAction* action);
	/** Add a file to the list. */
	void addFile(const QString& filename);
	/** Remove a file frm the list. */
	void removeFile(const QString& filename);
	/** Read recent file list to config file. */
	bool readSettings();
	/** Write recent file list to config file. */
	void saveSettings();
signals:
	/** User requested opening a file. */
	void selected(const QString& filename);
private slots:
	/** Given action was selected. */
	void triggered(QAction* action);
private:
	/** Format file to use just a base filename. */
	QString strippedFile(const QString& filename) const;
	/** Update action labels and hide unused. */
	void updateActions();

	int m_limit;
	QMenu* m_menu;
	QStringList m_files;

};


#endif
