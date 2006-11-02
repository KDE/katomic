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
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kglobal.h>


static const char description[] =
	I18N_NOOP("KDE Atomic Entertainment Game");

static const char version[] = "2.0";


// ##########################
// #	Main                #
// ##########################

int main(int argc, char **argv)
{
  KAboutData aboutData( "katomic", I18N_NOOP("KAtomic"),
    version, description, KAboutData::License_GPL,
    "(c) 1998, Andreas Wuest");
  aboutData.addAuthor("Andreas Wuest", 0, "AndreasWuest@gmx.de");
  aboutData.addAuthor("Stephan Kulow", 0, "coolo@kde.org");
  aboutData.addAuthor("Cristian Tibirna", 0, "tibirna@kde.org");
  aboutData.addAuthor("Dmitry Suzdalev", I18N_NOOP("Porting to KDE4. Current maintainer"), "dimsuz@gmail.com");
  aboutData.addCredit("Carsten Pfeiffer");
  aboutData.addCredit("Dave Corrie");
  aboutData.addCredit("Kai Jung", I18N_NOOP("6 new levels"), "jung@fh-fresenius.de");
  aboutData.addCredit("Danny Allen", I18N_NOOP("Game graphics and application icon"), "danny@dannyallen.co.uk");

  KCmdLineArgs::init( argc, argv, &aboutData );

  QApplication::setColorSpec(QApplication::ManyColor);
  KApplication a;
  KGlobal::locale()->insertCatalog("libkdegames");

  if ( a.isSessionRestored() )
        RESTORE(AtomTopLevel)
  else {
      AtomTopLevel *top = new AtomTopLevel;
      top->show();
  }
  return a.exec();
}

