/*
    SPDX-FileCopyrightText: 1998 Andreas Wüst <AndreasWuest@gmx.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <KXmlGuiWindow>

class GameWidget;
class KSelectAction;
class QAction;
class QLabel;

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
private Q_SLOTS:
    void slotAnimSpeedChanged(int);
    void updateStatusBar(int level, int score, int highscore);
    void levelHasChanged(int level);
    void chooseLevelSet();
    void changeLevelSet(const QString&);

private:
    bool queryClose() override; // reimp

    // Creates the menubar and connects the menu-entries to the
    // appropriate functions
    void createMenu();

    // Get the configuration from the config-file.
    void loadSettings();

    // Whenever a level gets changed, we update the actions
    // "m_prevLevelAct" and "m_nextLevelAct".
    void updateActionsForLevel(int level);

    GameWidget *m_gameWid;

    QAction *m_redoAct, *m_undoAct;
    QAction *m_prevLevelAct, *m_nextLevelAct;
    KSelectAction* m_animSpeedAct;
    QLabel *mLevel;
    QLabel *mCurrentScore;
    QLabel *mHighScore;
    QLabel *mMoleculeName;
};

#endif
