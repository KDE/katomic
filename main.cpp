/* main.cpp
 
  Copyright (C) 1998   Andreas W�st (if0626@pc4.fh-isny.de)

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

// ##########################
// #	Main                #
// ##########################

int main(int argc, char **argv)
{
  QApplication::setColorSpec(QApplication::ManyColor);
  KApplication a(argc, argv, "katomic" );
  AtomTopLevel *top = new AtomTopLevel("katomic");
  a.setMainWidget(top);
  top->show();
  return a.exec();
}

