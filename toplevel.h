/* toplevel.h

  Copyright (C) 1998   Andreas Wüst (AndreasWuest@gmx.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  */

#ifndef TOPLEVEL_H
#define TOPLEVEL_H

class GameWidget;
class KSelectAction;
class QAction;

#include <kxmlguiwindow.h>

/**
 * This is the class AtomTopLevel. The class is used only for the program
 * AtomTopLevel.
 *
 * @short Basic class for AtomTopLevel
 * @author Andreas Wüst
 */

class AtomTopLevel : public KXmlGuiWindow
{
    Q_OBJECT

public:
    AtomTopLevel();
    ~AtomTopLevel();
    /**
     * If called, katomic will allow user to switch to any
     * level even if he didn't solved it yet.
     * May enable some other "hacks" in future (if any will emerge)
     */
    void enableHackMode();
private slots:
    void slotAnimSpeedChanged(int);
    void updateStatusBar(int level, int score, int highscore);
private:
    virtual bool queryClose(); // reimp

    // Creates the menubar and connects the menu-entries to the
    // appropriate functions
    void createMenu();

    // Get the configuration from the config-file.
    void loadSettings();

    GameWidget *m_gameWid;

    QAction *m_redoAct, *m_undoAct;
    KSelectAction* m_animSpeedAct;
};

#endif
