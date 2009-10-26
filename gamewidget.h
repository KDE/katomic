/*

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
#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

class PlayField;
class QGraphicsView;
class QTimer;

#include <QWidget>
#include "levelset.h"

class KAtomicHighscores;

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    GameWidget ( const QString& levelSet, QWidget *parent );
    ~GameWidget();

    bool setLevelSet(const QString& levelSet);
    /**
     * @return levelset name
     */
    QString levelSetName() const;
    /**
     * @return levelset name suitable for showing in gui
     */
    QString levelSetVisibleName() const;

    void enableSwitchToAnyLevel() { m_allowAnyLevelSwitch = true; }
    bool switchToAnyLevelAllowed() const { return m_allowAnyLevelSwitch; }

    PlayField* playfield() { return m_playField; }

    int currentLevel() const { return m_level; }
    QString currentMolecule() const;
    int currentScore() const { return m_moves; }
    int currentHighScore() const;

    bool isNextLevelAvailable() const;
    bool isPrevLevelAvailable() const;

    void saveMaxAccessibleLevel(int level);
    void saveLastPlayedLevel();

signals:
    void statsChanged(int level,int score,int highscore);
    void levelChanged(int level);

public slots:
    void prevLevel();
    void nextLevel();

    void saveGame();
    void loadGame();

    // restart current level
    void restartLevel();

    void gameOver(int moves);

    // use this slot to update the moves continually
    void updateMoves(int moves);

    void showHighscores ();

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
private:

    virtual void resizeEvent( QResizeEvent* );
    void switchToLevel (int);

    int lastPlayedLevel() const;
    int maxAccessibleLevel() const;

    /**
     * If on, katomic will allow user to switch to any
     * level even if she didn't solved it yet.
     */
    bool m_allowAnyLevelSwitch;

    QGraphicsView *m_view;
    PlayField *m_playField;
    /**
     * Manages highscores
     */
    KAtomicHighscores *m_highscore;

    int m_moves;
    /**
     * Current levelset
     */
    LevelSet m_levelSet;
    /**
     * Highscore of the current level
     */
    int m_levelHighscore;
    /**
     * Number of the current level
     */
    int m_level;
    /**
     * Timer for automatic next level
     */
    QTimer *m_timer;
};

#endif
