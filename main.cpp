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

#include <KAboutData>
#include <kglobal.h>
#include <kmessagebox.h>
#include <qtimer.h>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "levelset.h"


static const char description[] =
	I18N_NOOP("KDE Atomic Entertainment Game");

static const char version[] = "3.0";

// ##########################
// #	Main                #
// ##########################

int main(int argc, char **argv)
{
  KAboutData aboutData( "katomic", i18n("KAtomic"),
    version, i18n(description), KAboutLicense::GPL,
    i18n("(c) 1998, Andreas Wuest\n(c) 2007-2009 Dmitry Suzdalev"), "http://games.kde.org/katomic" );
  aboutData.addAuthor(i18n("Andreas Wuest"), i18n("Original author"), "AndreasWuest@gmx.de");
  aboutData.addAuthor(i18n("Dmitry Suzdalev"), i18n("Porting to KDE4. Current maintainer"), "dimsuz@gmail.com");
  aboutData.addAuthor(i18n("Stephan Kulow"), QString(), "coolo@kde.org");
  aboutData.addAuthor(i18n("Cristian Tibirna"), QString(), "tibirna@kde.org");
  aboutData.addCredit(i18n("Carsten Pfeiffer"), QString(), "pfeiffer@kde.org");
  aboutData.addCredit(i18n("Dave Corrie"), QString(), "kde@davecorrie.com");
  aboutData.addCredit(i18n("Kai Jung"), i18n("6 new levels"), "jung@fh-fresenius.de");
  aboutData.addCredit(i18n("Danny Allen"), i18n("Game graphics and application icon"), "danny@dannyallen.co.uk");
  aboutData.addCredit(i18n("Johann Ollivier Lapeyre"), i18n("New great SVG artwork for KDE4"), "johann.ollivierlapeyre@gmail.com");
  aboutData.addCredit(i18n("Brian Croom"), i18n("Port to use KGameRenderer"), "brian.s.croom@gmail.com");

    QApplication app(argc, argv);
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
  parser.addOption(QCommandLineOption(QStringList() <<  QLatin1String("hackmode"), i18n( "Enable access to all levels" )));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

  if (!LevelSet::isDefaultLevelsAvailable())
  {
      KMessageBox::error(0, i18n("KAtomic failed to find its default level set and will quit. Please check your installation."));
      QTimer::singleShot(0, &app, SLOT(quit()));
  }
  else
  {
      if ( app.isSessionRestored() )
          RESTORE(AtomTopLevel)
      else {
          AtomTopLevel *top = new AtomTopLevel;
          if ( parser.isSet( "hackmode" ) )
              top->enableHackMode();
          
          top->show();
      }
  }

  return app.exec();
}

