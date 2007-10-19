/*
  Copyright (C) 2007   Dmitry Suzdalev (dimsuz@gmail.com)

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
#include "highscores.h"
#include "prefs.h"

#include <kuser.h>

KAtomicHighscores::KAtomicHighscores()
{
    KUser user;
    m_playerName =  user.property(KUser::FullName).toString().isEmpty() ? user.loginName() : user.property(KUser::FullName).toString();
}

bool KAtomicHighscores::addScore( int numMoves, int level )
{
    KConfigGroup grp( Preferences::self()->config(), "Highscores_"+m_playerName );
    QString keyStr = "Level_"+QString::number(level);
    int curHighScore = grp.readEntry( keyStr, -1 );
    if( numMoves < curHighScore || curHighScore == -1 ) // new highscore!
    {
        grp.writeEntry( keyStr, numMoves );
        return true;
    }
    return false;
}

int KAtomicHighscores::levelHighscore( int level ) const
{
    KConfigGroup grp( Preferences::self()->config(), "Highscores_"+m_playerName );
    QString keyStr = "Level_"+QString::number(level);
    return grp.readEntry( keyStr, 0 );
}
