/* toplevel.cpp

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

#include <QLayout>

#include <klocale.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kstdaction.h>
#include <kstdgameaction.h>
#include <kselectaction.h>
#include <kdebug.h>
#include <kicon.h>

#include "gamewidget.h"
#include "toplevel.h"
#include "prefs.h"

void AtomTopLevel::createMenu()
{
    KAction *act = KStdGameAction::highscores(m_gameWid, SLOT(showHighscores()), actionCollection());
    act->setText(i18n("Show &Highscores"));
    KStdGameAction::load( m_gameWid, SLOT(loadGame()), actionCollection() );
    KStdGameAction::save( m_gameWid, SLOT(saveGame()), actionCollection() );
    KStdGameAction::quit(this, SLOT(close()), actionCollection());
    KStdGameAction::restart(m_gameWid, SLOT(restartLevel()), actionCollection());

    m_animSpeedAct = new KSelectAction(i18n("Animation speed"), actionCollection(), "anim_speed");
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

AtomTopLevel::AtomTopLevel()
{
    m_gameWid = new GameWidget(this);
    m_gameWid->setObjectName("gamewidget");
    createMenu();
    loadSettings();
    setCentralWidget(m_gameWid);

    setupGUI( KMainWindow::Save | Keys | Create );
}

AtomTopLevel::~AtomTopLevel()
{
}

#include "toplevel.moc"
