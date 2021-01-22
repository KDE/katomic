/*
    SPDX-FileCopyrightText: 1998 Andreas Wüst <AndreasWuest@gmx.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "levelset.h"

#include <QWidget>

class PlayField;
class QGraphicsView;
class QTimer;

class KAtomicHighscores;

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget ( const QString& levelSet, QWidget *parent );
    ~GameWidget() override;

    bool setLevelSet(const QString& levelSet);

    /**
     * @return levelset name
     */
    const LevelSet& levelSet() const;

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

Q_SIGNALS:
    void statsChanged(int level,int score,int highscore);
    void levelChanged(int level);

public Q_SLOTS:
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

    void resizeEvent( QResizeEvent* ) override;
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
