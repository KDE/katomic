/*
    SPDX-FileCopyrightText: 2007-2009 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "highscores.h"

#include <KUser>
#include <KConfigGroup>

#include <QStandardPaths>

KAtomicHighscores::KAtomicHighscores()
{
    KUser user;
    m_playerName =  user.property(KUser::FullName).toString().isEmpty() ? user.loginName() : user.property(KUser::FullName).toString();

    m_hsFile = KSharedConfig::openConfig( QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QLatin1Char('/') + QStringLiteral("highscores"), KConfig::SimpleConfig);
}

bool KAtomicHighscores::addScore( int numMoves, const QString& levelSetName, int level )
{
    KConfigGroup userHsGroup( m_hsFile, QStringLiteral("Highscores_")+m_playerName );
    KConfigGroup levelSetGroup( &userHsGroup, levelSetName );

    QString keyStr = QStringLiteral("Level")+QString::number(level);
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
    KConfigGroup userHsGroup( m_hsFile, QStringLiteral("Highscores_")+m_playerName );
    KConfigGroup levelSetGroup( &userHsGroup, levelSetName );

    QString keyStr = QStringLiteral("Level")+QString::number(level);
    return levelSetGroup.readEntry( keyStr, 0 );
}
