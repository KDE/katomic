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

#include <q3groupbox.h>
#include <qlayout.h>

#include <kglobal.h>
#include <klocale.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kapplication.h>

#include "gamewidget.h"
#include "toplevel.h"
#include "settings.h"
#include "configbox.h"
#include <kaction.h>
#include <kstdaction.h>
#include <kstdgameaction.h>
#include <kdebug.h>

extern Options settings;

void AtomTopLevel::createMenu()
{
    KAction *act = KStdGameAction::highscores(main, SLOT(showHighscores()), actionCollection());
    act->setText(i18n("Show &Highscores"));
    KStdGameAction::quit(this, SLOT(close()), actionCollection());
    KStdGameAction::restart(main, SLOT(restartLevel()), actionCollection());

    KStdAction::preferences(this, SLOT(configopts()), actionCollection());

    undoAction = KStdGameAction::undo (main, SLOT(doUndo()), actionCollection());
    redoAction = KStdGameAction::redo (main, SLOT(doRedo()), actionCollection());
    undoAction->setEnabled(false);
    redoAction->setEnabled(false);
    connect (main, SIGNAL (enableRedo(bool)), SLOT(enableRedo(bool)));
    connect (main, SIGNAL (enableUndo(bool)), SLOT(enableUndo(bool)));

    new KAction(i18n("Atom Up"), Qt::Key_Up, main, SLOT(moveUp()), actionCollection(), "atom_up");
    new KAction(i18n("Atom Down"), Qt::Key_Down, main, SLOT(moveDown()), actionCollection(), "atom_down");
    new KAction(i18n("Atom Left"), Qt::Key_Left, main, SLOT(moveLeft()), actionCollection(), "atom_left");
    new KAction(i18n("Atom Right"), Qt::Key_Right, main, SLOT(moveRight()), actionCollection(), "atom_right");

    new KAction(i18n("Next Atom"), Qt::Key_Tab, main, SLOT(nextAtom()), actionCollection(), "next_atom");
    new KAction(i18n("Previous Atom"), Qt::SHIFT+Qt::Key_Tab, main, SLOT(previousAtom()), actionCollection(), "prev_atom");
}

void AtomTopLevel::configopts()
{
    (new ConfigBox(this, "Options"))->show();
}

void AtomTopLevel::initConfig()
{
    config = KGlobal::config();
}

void AtomTopLevel::saveConfig()
{
    config = KGlobal::config();

    if (settings.changed) {
	  config->setGroup("Options");
	  config->writeEntry("Animation Speed", settings.anim_speed);
	  config->setGroup("Colors");
    }
    config->sync();
}


AtomTopLevel::AtomTopLevel()
{
    main = new GameWidget(this);
    main->setObjectName("gamewidget");
    createMenu();
    initConfig();
    setCentralWidget(main);

    setupGUI( KMainWindow::Save | Keys | Create );
}

AtomTopLevel::~AtomTopLevel()
{
}

bool AtomTopLevel::queryExit()
{
    saveConfig();
    return true;
}

void AtomTopLevel::enableRedo(bool enable)
{
    redoAction->setEnabled(enable);
}

void AtomTopLevel::enableUndo(bool enable)
{
    undoAction->setEnabled(enable);
}

#include "toplevel.moc"
