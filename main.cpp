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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  */


#include "toplevel.h"
#include "kapp.h"
#include <klocale.h>
#include <kcmdlineargs.h>

#include <kaboutdata.h>


static const char *description = 
	I18N_NOOP("KDE Atomic Entertainment Game");

static const char *version = "v0.0.1";


// ##########################
// #	Main                #
// ##########################

int main(int argc, char **argv)
{
  KAboutData aboutData( "katomic", I18N_NOOP("KAtomic"), 
    version, description, KAboutData::GPL, 
    "(c) 1998, Andreas Wüst");
  aboutData.addAuthor("Andreas Wüst",0, "AndreasWuest@gmx.de");
  KCmdLineArgs::init( argc, argv, &aboutData );

  QApplication::setColorSpec(QApplication::ManyColor);
  KApplication a;
  AtomTopLevel *top = new AtomTopLevel("katomic");
  a.setMainWidget(top);
  top->show();
  return a.exec();
}

