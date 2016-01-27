/*
   Copyright (C) 2007-2009   Dmitry Suzdalev (dimsuz@gmail.com)

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

#include <kuser.h>

#include <kconfiggroup.h>
#include <QStandardPaths>
#include <KSharedConfig>

KAtomicHighscores::KAtomicHighscores()
{
    KUser user;
    m_playerName =  user.property(KUser::FullName).toString().isEmpty() ? user.loginName() : user.property(KUser::FullName).toString();

    m_hsFile = KSharedConfig::openConfig( QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') + "highscores", KConfig::SimpleConfig);
}

bool KAtomicHighscores::addScore( int numMoves, const QString& levelSetName, int level )
{
    KConfigGroup userHsGroup( m_hsFile, "Highscores_"+m_playerName );
    KConfigGroup levelSetGroup( &userHsGroup, levelSetName );

    QString keyStr = "Level"+QString::number(level);
    int curHighScore = levelSetGroup.readEntry( keyStr, -1 );
    if( numMoves < curHighScore || curHighScore == -1 ) // new highscore!
    {
        levelSetGroup.writeEntry( keyStr, numMoves );
        return true;
    }
    return false;
}

int KAtomicHighscores::levelHighscore( const QString& levelSetName, int level ) const
{
    KConfigGroup userHsGroup( m_hsFile, "Highscores_"+m_playerName );
    KConfigGroup levelSetGroup( &userHsGroup, levelSetName );

    QString keyStr = "Level"+QString::number(level);
    return levelSetGroup.readEntry( keyStr, 0 );
}
