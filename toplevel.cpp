/* toplevel.cpp

  Copyright (C) 1998   Andreas Wüst (AndreasWuest@gmx.de)
  Copyright (C) 2006   Dmitry Suzdalev <dimsuz@gmail.com>

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

#include <QLayout>

#include <klocale.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kstandardaction.h>
#include <kstdgameaction.h>
#include <kselectaction.h>
#include <kdebug.h>
#include <kicon.h>
#include <kstatusbar.h>

#include "gamewidget.h"
#include "toplevel.h"
#include "prefs.h"

AtomTopLevel::AtomTopLevel()
{
    m_gameWid = new GameWidget(Preferences::lastPlayedLevel(), this);
    m_gameWid->setObjectName("gamewidget");
    createMenu();
    loadSettings();
    setCentralWidget(m_gameWid);

    statusBar()->insertItem( i18n("Level:"), 0 );
    statusBar()->insertItem( i18n("Current score:"), 1 );
    statusBar()->insertItem( i18n("Highscore:"), 2 );
    updateStatusBar( m_gameWid->currentLevel(), m_gameWid->currentScore(), m_gameWid->currentHighScore() );

    connect(m_gameWid, SIGNAL(statsChanged(int,int,int)), SLOT(updateStatusBar(int,int,int)));

    setupGUI();
}

AtomTopLevel::~AtomTopLevel()
{
}

bool AtomTopLevel::queryClose()
{
    Preferences::setLastPlayedLevel(m_gameWid->currentLevel());
    Preferences::writeConfig();
    return true;
}

void AtomTopLevel::createMenu()
{
    KStdGameAction::highscores(m_gameWid, SLOT(showHighscores()), actionCollection());
    KStdGameAction::load( m_gameWid, SLOT(loadGame()), actionCollection() );
    KStdGameAction::save( m_gameWid, SLOT(saveGame()), actionCollection() );
    KStdGameAction::quit(this, SLOT(close()), actionCollection());
    KStdGameAction::restart(m_gameWid, SLOT(restartLevel()), actionCollection());

    m_animSpeedAct = new KSelectAction(i18n("Animation Speed"), actionCollection(), "anim_speed");
    QStringList acts;
    acts << i18n("Slow") << i18n("Normal") << i18n("Fast");
    m_animSpeedAct->setItems(acts);
    connect( m_animSpeedAct, SIGNAL(triggered(int)), SLOT(slotAnimSpeedChanged(int)) );

    KAction *undoAll = new KAction( KIcon("player_start"), i18n("Undo All"), actionCollection(), "undo_all" );
    connect( undoAll, SIGNAL(triggered(bool)), m_gameWid, SLOT(undoAll()) );

    KAction *redoAll = new KAction( KIcon("player_end"), i18n("Redo All"), actionCollection(), "redo_all" );
    connect( redoAll, SIGNAL(triggered(bool)), m_gameWid, SLOT(redoAll()) );

    m_undoAct = new KAction( KIcon("undo"), i18n("Undo"), actionCollection(), "undo" );
    connect( m_undoAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(doUndo()) );

    m_redoAct = new KAction( KIcon("redo"), i18n("Redo"), actionCollection(), "redo" );
    connect( m_redoAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(doRedo()) );

    m_undoAct->setEnabled(false);
    m_redoAct->setEnabled(false);
    undoAll->setEnabled(false);
    redoAll->setEnabled(false);

    connect (m_gameWid, SIGNAL (enableRedo(bool)), m_redoAct, SLOT(setEnabled(bool)));
    connect (m_gameWid, SIGNAL (enableUndo(bool)), m_undoAct, SLOT(setEnabled(bool)));
    connect (m_gameWid, SIGNAL (enableUndo(bool)), undoAll, SLOT(setEnabled(bool)));
    connect (m_gameWid, SIGNAL (enableRedo(bool)), redoAll, SLOT(setEnabled(bool)));

    KAction* atomUpAct = new KAction(i18n("Atom Up"), actionCollection(), "atom_up");
    atomUpAct->setShortcut(Qt::Key_Up);
    addAction(atomUpAct);
    connect(atomUpAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(moveUp()));

    KAction* atomDownAct = new KAction(i18n("Atom Down"), actionCollection(), "atom_down");
    atomDownAct->setShortcut(Qt::Key_Down);
    addAction(atomDownAct);
    connect(atomDownAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(moveDown()));

    KAction* atomLeftAct = new KAction(i18n("Atom Left"), actionCollection(), "atom_left");
    atomLeftAct->setShortcut(Qt::Key_Left);
    addAction(atomLeftAct);
    connect(atomLeftAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(moveLeft()));

    KAction* atomRightAct = new KAction(i18n("Atom Right"), actionCollection(), "atom_right");
    atomRightAct->setShortcut(Qt::Key_Right);
    addAction(atomRightAct);
    connect(atomRightAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(moveRight()));

    KAction* nextAtomAct = new KAction(i18n("Next Atom"), actionCollection(), "next_atom");
    nextAtomAct->setShortcut(Qt::Key_Tab);
    addAction(nextAtomAct);
    connect(nextAtomAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(nextAtom()));

    KAction* prevAtomAct = new KAction(i18n("Previous Atom"), actionCollection(), "prev_atom");
    prevAtomAct->setShortcut(Qt::SHIFT+Qt::Key_Tab);
    addAction(prevAtomAct);
    connect(prevAtomAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(previousAtom()));
}

void AtomTopLevel::loadSettings()
{
    m_animSpeedAct->setCurrentItem( Preferences::animationSpeed() );
    m_gameWid->setAnimationSpeed( Preferences::animationSpeed() );
}

void AtomTopLevel::slotAnimSpeedChanged(int speed)
{
    m_gameWid->setAnimationSpeed(speed);
    Preferences::setAnimationSpeed(speed);
    Preferences::writeConfig();
}

void AtomTopLevel::updateStatusBar( int level, int score, int highscore )
{
    statusBar()->changeItem( i18n("Level: %1", level), 0 );
    statusBar()->changeItem( i18n("Current score: %1", score), 1 );
    QString str;
    if(highscore == 0)
        str = "-";
    else
        str.setNum(highscore);
    statusBar()->changeItem( i18n("Highscore: ")+str, 2 );
}

#include "toplevel.moc"
