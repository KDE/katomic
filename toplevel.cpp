/* toplevel.cpp
 
  Copyright (C) 1998   Andreas Wüst (if0626@pc4.fh-isny.de)

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


#include "gamewidget.h"

#include <qlayout.h>
#include <qgroupbox.h>
#include <kkeydialog.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>
#include <kglobal.h>
#include "toplevel.h"
#include <klocale.h>
#include "settings.h"
#include "configbox.h"
#include <kmenubar.h>

extern Options settings;

void AtomTopLevel::createMenu()
{
    file = new QPopupMenu();
    file->insertItem (i18n ("&Highscores"),
		      main, SLOT (showHighscores ()));
  		
    file->insertSeparator(-1);
    file->insertItem(i18n("&Quit"), this, SLOT(quitapp()) );
	
    options = new QPopupMenu();
    options->insertItem(i18n("&Configure keys"), this,
			SLOT(configkeys()) );
    options->insertItem(i18n("&Options"), this,
			SLOT(configopts()) );

    KMenuBar *menu = menuBar();
    menu->insertItem(i18n("&File"), file);
    menu->insertItem(i18n("&Options"), options);
    menu->insertSeparator(-1);
    menu->insertItem(i18n("&Help"), kapp->getHelpMenu(false, 
		    i18n("Atomic 2.0 by Stephan Kulow <coolo@kde.org>\n"
			 "and Cristian Tibirna <tibirna@kde.org>\n"
			 "based on Atomic 1.0.67 by Andreas Wüst (if0626@pc4.fh-isny.de)\n")));
}


void AtomTopLevel::configkeys()
{
    KKeyDialog::configureKeys(accel);
}

void AtomTopLevel::configopts()
{
    (new ConfigBox(this, "Options"))->show();
}

void AtomTopLevel::initKeys()
{
    // here we create to shortcuts according to
    // the standard Kde keybinding
    accel = new KAccel(this);
	
    accel->insertItem(i18n("About"), "about", "CTRL+A");
    accel->connectItem("about", this, SLOT(about()) );
    accel->insertStdItem(KAccel::Quit, i18n("Quit"));
    accel->connectItem(KAccel::Quit, this, SLOT(quitapp()) );
    accel->insertItem(i18n("Highscores"), "highscore", "CTRL+H");
    accel->connectItem ("highscore", main, 
			SLOT (showHighscores ()));
    // moving keys
    accel->insertItem(i18n("Atom Up"), "up", "J");
    accel->insertItem(i18n("Atom Down"), "down", "N");
    accel->insertItem(i18n("Atom Left"), "left", "S");
    accel->insertItem(i18n("Atom Right"), "right", "D");
    accel->insertItem(i18n("Next Atom"), "down", "Space");

}

void AtomTopLevel::initConfig()
{
    config = KGlobal::config();

    /* CT this segfaults :-(
       accel->readSettings(config);
    */


}

void AtomTopLevel::saveConfig()
{
    /* CT would this segfault too?
       accel->writeSettings(config);
  */

    if (settings.changed) {
	config->setGroup("Options");
	config->writeEntry("Animation Speed", settings.anim_speed);
	config->setGroup("Colors");
    }
    config->sync();

}


AtomTopLevel::AtomTopLevel ( const char* name )
    : KTMainWindow ( name )
{
    main = new GameWidget(this, "gamewidget");

    createMenu();
    initKeys();
    setView(main);
}

AtomTopLevel::~AtomTopLevel()
{
    delete file;
    delete options;
    delete main;
}

void AtomTopLevel::quitapp()
{
    saveConfig();
    kapp->quit();	
}

#include "toplevel.moc"
