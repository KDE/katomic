/* toplevel.cpp

  Copyright (C) 1998   Andreas Wüst <AndreasWuest@gmx.de>
  Copyright (C) 2006   Dmitry Suzdalev <dimsuz@gmail.com>
  Copyright (C) 2007   Simon Hürlimann <simon.huerlimann@huerlisi.ch>

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

#include "toplevel.h"

#include <QLayout>

#include <klocale.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kstandardaction.h>
#include <kstandardgameaction.h>
#include <kselectaction.h>
#include <kactioncollection.h>
#include <kdebug.h>
#include <kicon.h>
#include <kstatusbar.h>

#include "gamewidget.h"
#include "playfield.h"
#include "prefs.h"

AtomTopLevel::AtomTopLevel()
{
    int lastPlayed = Preferences::lastPlayedLevel();
    int maxLevel = Preferences::maxAccessibleLevel();

    m_gameWid = new GameWidget(lastPlayed <= maxLevel ? lastPlayed : maxLevel, this);
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
    m_gameWid->playfield()->saveLastBackground();
    return true;
}

void AtomTopLevel::createMenu()
{
    // Game
    KStandardGameAction::gameNew(m_gameWid, SLOT(restartLevel()), actionCollection());
    KStandardGameAction::restart(m_gameWid, SLOT(restartLevel()), actionCollection());
    KStandardGameAction::highscores(m_gameWid, SLOT(showHighscores()), actionCollection());
    KStandardGameAction::load(m_gameWid, SLOT(loadGame()), actionCollection());
    KStandardGameAction::save(m_gameWid, SLOT(saveGame()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
  
    // Move
    m_undoAct = KStandardGameAction::undo(m_gameWid->playfield(), SLOT(undo()), actionCollection());
    m_redoAct = KStandardGameAction::redo(m_gameWid->playfield(), SLOT(redo()), actionCollection());
  
    
    QAction *act;
    
    // TODO: gray out corresponding actions if we are at first or at last level
    act = actionCollection()->addAction( "prev_level" );
    act->setIcon( KIcon( "arrow-left" ) );
    act->setText( i18n( "Previous Level" ) );
    act->setShortcut( Qt::CTRL + Qt::Key_P );
    addAction( act );
    connect( act, SIGNAL( triggered( bool ) ), m_gameWid, SLOT( prevLevel() ) );

    act = actionCollection()->addAction( "next_level" );
    act->setIcon( KIcon( "arrow-right" ) );
    act->setText( i18n( "Next Level" ) );
    act->setShortcut( Qt::CTRL + Qt::Key_N );
    addAction( act );
    connect( act, SIGNAL( triggered( bool ) ), m_gameWid, SLOT( nextLevel() ) );


    m_animSpeedAct = new KSelectAction(i18n("Animation Speed"), this);
    actionCollection()->addAction("anim_speed", m_animSpeedAct);
    QStringList acts;
    acts << i18n("Slow") << i18n("Normal") << i18n("Fast");
    m_animSpeedAct->setItems(acts);
    connect( m_animSpeedAct, SIGNAL(triggered(int)), SLOT(slotAnimSpeedChanged(int)) );

    QAction *undoAll = actionCollection()->addAction( "move_undo_all" );
    undoAll->setIcon( KIcon("media-skip-backward") );
    undoAll->setText( i18n("Undo All") );
    connect( undoAll, SIGNAL(triggered(bool)), m_gameWid->playfield(), SLOT(undoAll()) );

    QAction *redoAll = actionCollection()->addAction( "move_redo_all" );
    redoAll->setIcon( KIcon("media-skip-forward") );
    redoAll->setText( i18n("Redo All") );
    connect( redoAll, SIGNAL(triggered(bool)), m_gameWid->playfield(), SLOT(redoAll()) );

    m_undoAct->setEnabled(false);
    m_redoAct->setEnabled(false);
    undoAll->setEnabled(false);
    redoAll->setEnabled(false);

    connect (m_gameWid->playfield(), SIGNAL (enableRedo(bool)), m_redoAct, SLOT(setEnabled(bool)));
    connect (m_gameWid->playfield(), SIGNAL (enableUndo(bool)), m_undoAct, SLOT(setEnabled(bool)));
    connect (m_gameWid->playfield(), SIGNAL (enableUndo(bool)), undoAll, SLOT(setEnabled(bool)));
    connect (m_gameWid->playfield(), SIGNAL (enableRedo(bool)), redoAll, SLOT(setEnabled(bool)));

    QAction* atomUpAct = actionCollection()->addAction("atom_up");
    atomUpAct->setText(i18n("Atom Up"));
    atomUpAct->setShortcut(Qt::Key_Up);
    addAction(atomUpAct);
    connect(atomUpAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(moveUp()));

    QAction* atomDownAct = actionCollection()->addAction("atom_down");
    atomDownAct->setText(i18n("Atom Down"));
    atomDownAct->setShortcut(Qt::Key_Down);
    addAction(atomDownAct);
    connect(atomDownAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(moveDown()));

    QAction* atomLeftAct = actionCollection()->addAction("atom_left");
    atomLeftAct->setText(i18n("Atom Left"));
    atomLeftAct->setShortcut(Qt::Key_Left);
    addAction(atomLeftAct);
    connect(atomLeftAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(moveLeft()));

    QAction* atomRightAct = actionCollection()->addAction("atom_right");
    atomRightAct->setText(i18n("Atom Right"));
    atomRightAct->setShortcut(Qt::Key_Right);
    addAction(atomRightAct);
    connect(atomRightAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(moveRight()));

    QAction* nextAtomAct = actionCollection()->addAction("next_atom");
    nextAtomAct->setText(i18n("Next Atom"));
    nextAtomAct->setShortcut(Qt::Key_Tab);
    addAction(nextAtomAct);
    connect(nextAtomAct, SIGNAL(triggered(bool)), m_gameWid->playfield(), SLOT(nextAtom()));

    QAction* prevAtomAct = actionCollection()->addAction("prev_atom");
    prevAtomAct->setText(i18n("Previous Atom"));
    prevAtomAct->setShortcut(Qt::SHIFT+Qt::Key_Tab);
    addAction(prevAtomAct);
    connect(prevAtomAct, SIGNAL(triggered(bool)), m_gameWid->playfield(), SLOT(previousAtom()));
}

void AtomTopLevel::loadSettings()
{
    m_animSpeedAct->setCurrentItem( Preferences::animationSpeed() );
    m_gameWid->playfield()->setAnimationSpeed( Preferences::animationSpeed() );
}

void AtomTopLevel::slotAnimSpeedChanged(int speed)
{
    m_gameWid->playfield()->setAnimationSpeed(speed);
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

void AtomTopLevel::enableHackMode()
{
    kDebug() << "Enabling hack mode" << endl;
    m_gameWid->enableSwitchToAnyLevel();
}

#include "toplevel.moc"
