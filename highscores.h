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

#ifndef KATOMIC_HIGHSCORES_H
#define KATOMIC_HIGHSCORES_H

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
    bool addScore( int numMoves, int level );
    /**
     * @return highscore of current player for level
     */
    int levelHighscore( int level ) const;
private:
    /**
     * Current player name
     */
    QString m_playerName;
};

#endif
