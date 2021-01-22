/*
    SPDX-FileCopyrightText: 2007 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KATOMIC_HIGHSCORES_H
#define KATOMIC_HIGHSCORES_H

#include <KSharedConfig>

#include <QString>

/**
 * This class is used to manage katomic's highscores.
 * There's one highscore for each level per each player - it's
 * the number of moves it took to complete level.
 */
class KAtomicHighscores
{
public:
    /**
     * Constructor.
     * By default sets player name to current user login name
     * @see setCurrentPlayerName
     */
    KAtomicHighscores();
    /**
     * Changes current player name to playerName
     */
    void setCurrentPlayerName( const QString& playerName ) { m_playerName = playerName; }
    /**
     * @return current player name
     */
    QString currentPlayerName() const { return m_playerName; }
    /**
     * Tries to add new result to highscores.
     * If numMoves is lesser than current highscore for this level,
     * this result will become a new highscore of currentPlayerName for this level
     * @return whether numMoves was set as a new highscore
     */
    bool addScore( int numMoves, const QString& levelSetName, int level );
    /**
     * @return highscore of current player for level
     */
    int levelHighscore( const QString& levelSetName, int level ) const;
private:
    /**
     * Current player name
     */
    QString m_playerName;

    KSharedConfigPtr m_hsFile;
};

#endif
