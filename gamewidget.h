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
class PlayFieldView;
class Molecule;
class QScrollBar;
class QLabel;
class KScoreDialog;

#include <QWidget>

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    GameWidget ( int startingLevel, QWidget *parent );
    ~GameWidget();
    void setAnimationSpeed(int);

    void enableSwitchToAnyLevel() { m_allowAnyLevelSwitch = true; }

    // TODO: remove most of wrapper functions (signals,slots) and
    // by using this function in toplevel.cpp do connects directly to playfield
    PlayField* playfield() { return m_playField; }

    int currentLevel() const { return m_level; }
    int currentScore() const { return m_moves; }
    int currentHighScore() const;
signals:
    void enableRedo(bool enable);
    void enableUndo(bool enable);
    void statsChanged(int level,int score,int highscore);
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

    // Menupunkt Highscores im Pop-up Menu, der Highscore anzeigt
    void showHighscores ();

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void nextAtom();
    void previousAtom();
    void doUndo ();
    void doRedo ();
    void undoAll();
    void redoAll();
private:
    void switchToLevel (int);
    /**
     * If on, katomic will allow user to switch to any
     * level even if she didn't solved it yet.
     */
    bool m_allowAnyLevelSwitch;

    PlayFieldView *m_view;
    PlayField *m_playField;

    int m_level;
    int m_moves;

    KScoreDialog *highScore;
};

#endif
