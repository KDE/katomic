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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  */

#include <qgroupbox.h>
#include <qlayout.h>

#include <kglobal.h>
#include <kkeydialog.h>
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
    KStdGameAction::highscores(main, SLOT(showHighscores()), actionCollection());
    KStdGameAction::quit(this, SLOT(close()), actionCollection());
    KStdGameAction::restart(main, SLOT(restartLevel()), actionCollection());

    KStdAction::keyBindings(this, SLOT(configkeys()), actionCollection());
    KStdAction::preferences(this, SLOT(configopts()), actionCollection());

    new KAction(i18n("Atom Up"), Key_Up, main, SLOT(moveUp()), actionCollection(), "atom_up");
    new KAction(i18n("Atom Down"), Key_Down, main, SLOT(moveDown()), actionCollection(), "atom_down");
    new KAction(i18n("Atom Left"), Key_Left, main, SLOT(moveLeft()), actionCollection(), "atom_left");
    new KAction(i18n("Atom Right"), Key_Right, main, SLOT(moveRight()), actionCollection(), "atom_right");

    new KAction(i18n("Next Atom"), Key_Tab, main, SLOT(nextAtom()), actionCollection(), "next_atom");
    new KAction(i18n("Previous Atom"), SHIFT+Key_Tab, main, SLOT(previousAtom()), actionCollection(), "prev_atom");
}

void AtomTopLevel::configkeys()
{
    KKeyDialog::configure(actionCollection(),this);
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
    main = new GameWidget(this, "gamewidget");
    createMenu();
    initConfig();
    setCentralWidget(main);

    createGUI();
}

AtomTopLevel::~AtomTopLevel()
{
}

bool AtomTopLevel::queryExit()
{
    saveConfig();
    return true;
}

#include "toplevel.moc"
