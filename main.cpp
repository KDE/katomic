/* main.cpp

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


#include "toplevel.h"

#include <kapplication.h>
#include <KLocalizedString>
#include <kcmdlineargs.h>
#include <K4AboutData>
#include <kglobal.h>
#include <kmessagebox.h>
#include <qtimer.h>

#include "levelset.h"


static const char description[] =
	I18N_NOOP("KDE Atomic Entertainment Game");

static const char version[] = "3.0";

// ##########################
// #	Main                #
// ##########################

int main(int argc, char **argv)
{
  K4AboutData aboutData( "katomic", 0, ki18n("KAtomic"),
    version, ki18n(description), K4AboutData::License_GPL,
    ki18n("(c) 1998, Andreas Wuest\n(c) 2007-2009 Dmitry Suzdalev"), KLocalizedString(), "http://games.kde.org/katomic" );
  aboutData.addAuthor(ki18n("Andreas Wuest"), ki18n("Original author"), "AndreasWuest@gmx.de");
  aboutData.addAuthor(ki18n("Dmitry Suzdalev"), ki18n("Porting to KDE4. Current maintainer"), "dimsuz@gmail.com");
  aboutData.addAuthor(ki18n("Stephan Kulow"), KLocalizedString(), "coolo@kde.org");
  aboutData.addAuthor(ki18n("Cristian Tibirna"), KLocalizedString(), "tibirna@kde.org");
  aboutData.addCredit(ki18n("Carsten Pfeiffer"), KLocalizedString(), "pfeiffer@kde.org");
  aboutData.addCredit(ki18n("Dave Corrie"), KLocalizedString(), "kde@davecorrie.com");
  aboutData.addCredit(ki18n("Kai Jung"), ki18n("6 new levels"), "jung@fh-fresenius.de");
  aboutData.addCredit(ki18n("Danny Allen"), ki18n("Game graphics and application icon"), "danny@dannyallen.co.uk");
  aboutData.addCredit(ki18n("Johann Ollivier Lapeyre"), ki18n("New great SVG artwork for KDE4"), "johann.ollivierlapeyre@gmail.com");
  aboutData.addCredit(ki18n("Brian Croom"), ki18n("Port to use KGameRenderer"), "brian.s.croom@gmail.com");

  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add("hackmode", ki18n( "Enable access to all levels" ));
  KCmdLineArgs::addCmdLineOptions( options );

  KApplication a;

  if (!LevelSet::isDefaultLevelsAvailable())
  {
      KMessageBox::error(0, i18n("KAtomic failed to find its default level set and will quit. Please check your installation."));
      QTimer::singleShot(0, &a, SLOT(quit()));
  }
  else
  {
      if ( a.isSessionRestored() )
          RESTORE(AtomTopLevel)
      else {
          AtomTopLevel *top = new AtomTopLevel;
          KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
          if ( args->isSet( "hackmode" ) )
              top->enableHackMode();
          args->clear();
          top->show();
      }
  }

  return a.exec();
}

