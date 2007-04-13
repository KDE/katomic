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

#include "gamewidget.h"
#include "playfield.h"
#include "prefs.h"

#include <QScrollBar>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>

#include <kscoredialog.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <kfiledialog.h>

void GameWidget::moveUp()
{
    if(!m_playField->isLevelFinished())
        m_playField->moveSelectedAtom( PlayField::Up );
}

void GameWidget::moveDown()
{
    if(!m_playField->isLevelFinished())
        m_playField->moveSelectedAtom( PlayField::Down );
}

void GameWidget::moveLeft()
{
    if(!m_playField->isLevelFinished())
        m_playField->moveSelectedAtom( PlayField::Left );
}

void GameWidget::moveRight()
{
    if(!m_playField->isLevelFinished())
        m_playField->moveSelectedAtom( PlayField::Right );
}

void GameWidget::nextAtom()
{
    if(!m_playField->isLevelFinished())
        m_playField->nextAtom();
}

void GameWidget::previousAtom()
{
    if(!m_playField->isLevelFinished())
        m_playField->previousAtom();
}

void GameWidget::doUndo ()
{
    m_playField->undo();
}

void GameWidget::doRedo ()
{
    m_playField->redo();
}

void GameWidget::undoAll()
{
    m_playField->undoAll();
}

void GameWidget::redoAll()
{
    m_playField->redoAll();
}

void GameWidget::setAnimationSpeed(int speed)
{
    m_playField->setAnimationSpeed(speed);
}

void GameWidget::gameOver(int moves) {
    int answer = KMessageBox::questionYesNo(this, i18n("Congratulations! You solved level %1 with %2 moves!\n Advance to the next one?", m_level, moves), i18n("Congratulations"));
    // writing this info only in normal mode
    if ( !m_allowAnyLevelSwitch )
    {
        Preferences::setMaxAccessibleLevel( m_level+1 );
        Preferences::writeConfig();
    }

    highScore->setCaption(i18n("Level %1 Highscores", m_level));
    highScore->setConfigGroup(QString("Highscores Level %1").arg(m_level));

    KScoreDialog::FieldInfo scoreInfo;

    if (highScore->addScore(moves, scoreInfo, KScoreDialog::AskName | KScoreDialog::LessIsMore))
        highScore->exec();

    emit statsChanged(m_level, moves, highScore->highScore());

    if ( answer == KMessageBox::Yes )
        switchToLevel(m_level+1);
}

void GameWidget::updateMoves(int moves)
{
    m_moves = moves;
    emit statsChanged(m_level, moves, highScore->highScore());
}

void GameWidget::switchToLevel (int l)
{
    m_level=l;
    QString levelFile = KStandardDirs::locate("appdata", QString("levels/level_%1").arg(l));
    if (levelFile.isNull()) {
        return switchToLevel(1);
    }

    KConfig cfg( levelFile, KConfig::OnlyLocal);
    KConfigGroup gr = cfg.group("Level");
    m_playField->loadLevel(gr);
    m_view->resetCachedContent();
    m_view->update();

    highScore->setConfigGroup(QString("Highscores Level %1").arg(m_level));

    emit statsChanged(m_level, 0, highScore->highScore());
}

void GameWidget::restartLevel()
{
    switchToLevel(m_level);
}

GameWidget::GameWidget ( int startingLevel, QWidget *parent )
    : QWidget( parent ), m_allowAnyLevelSwitch( false ), m_moves(0)
{
    QVBoxLayout *top = new QVBoxLayout(this);
    top->setMargin(10);

    // playfield
    m_playField = new PlayField(this);
    m_view = new PlayFieldView(m_playField, this);
    m_view->setCacheMode( QGraphicsView::CacheBackground );
    top->addWidget(m_view, 1);

    connect (m_playField, SIGNAL (gameOver(int)), SLOT(gameOver(int)));
    connect (m_playField, SIGNAL (enableUndo(bool)), SIGNAL(enableUndo(bool)));
    connect (m_playField, SIGNAL (enableRedo(bool)), SIGNAL(enableRedo(bool)));
    connect (m_playField, SIGNAL (updateMoves(int)), SLOT(updateMoves(int)));

    highScore = new KScoreDialog(KScoreDialog::Name | KScoreDialog::Score, this);

    switchToLevel(startingLevel);
}

GameWidget::~GameWidget()
{
}

void GameWidget::saveGame()
{
    QString fileName = KFileDialog::getSaveFileName( KUrl(), "*.katomic", this );
    if(fileName.isEmpty())
        return;
    KConfig config(fileName, KConfig::OnlyLocal);
    KConfigGroup gr = config.group("Savegame");
    gr.writeEntry( "Level", m_level );
    m_playField->saveGame( gr );
}

void GameWidget::loadGame()
{
    QString fileName = KFileDialog::getOpenFileName( KUrl(), "*.katomic", this );
    if(fileName.isEmpty())
        return;
    KConfig config(fileName, KConfig::OnlyLocal);
    KConfigGroup gr = config.group("Savegame");
    int l = gr.readEntry( "Level", 1 );
    m_level = l;
    switchToLevel(m_level);
    m_playField->loadGame( gr );
}

void GameWidget::showHighscores ()
{
    highScore->setCaption(i18n("Level %1 Highscores", m_level));
    highScore->setConfigGroup(QString("Highscores Level %1").arg(m_level));
    highScore->exec();
}

int GameWidget::currentHighScore() const
{
    return highScore->highScore();
}

void GameWidget::prevLevel()
{
    if ( m_level == 1 )
        return;
    m_level--;
    switchToLevel(m_level);
}

void GameWidget::nextLevel()
{
    if ( !m_allowAnyLevelSwitch && m_level >= Preferences::maxAccessibleLevel() )
    {
        KMessageBox::information(this, i18n("You must solve level %1 before advancing to the next one!",
                                            Preferences::maxAccessibleLevel()), i18n("Warning"));
        return;
    }
    m_level++;
    switchToLevel(m_level);
}

#include "gamewidget.moc"
