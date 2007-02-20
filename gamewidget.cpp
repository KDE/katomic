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
    KMessageBox::information(this, i18n("You solved level %1 with %2 moves!", m_level, moves), i18n("Congratulations"));

    highScore->setCaption(i18n("Level %1 Highscores", m_level));
    highScore->setConfigGroup(QString("Highscores Level %1").arg(m_level));

    KScoreDialog::FieldInfo scoreInfo;

    if (highScore->addScore(moves, scoreInfo, true, true))
        highScore->exec();

    emit statsChanged(m_level, moves, highScore->highScore());
}

void GameWidget::updateMoves(int moves)
{
    m_moves = moves;
    emit statsChanged(m_level, moves, highScore->highScore());
}

void GameWidget::updateLevel (int l)
{
    m_level=l;
    QString levelFile = KStandardDirs::locate("appdata", QString("levels/level_%1").arg(l));
    if (levelFile.isNull()) {
        return updateLevel(1);
    }

    KConfig cfg( levelFile, KConfig::OnlyLocal);
    cfg.setGroup("Level");
    m_playField->loadLevel(cfg);
    m_view->resetCachedContent();
    m_view->update();

    highScore->setConfigGroup(QString("Highscores Level %1").arg(m_level));

    scrl->setValue(m_level);
    emit statsChanged(m_level, 0, highScore->highScore());
}

void GameWidget::restartLevel()
{
    updateLevel(m_level);
}

GameWidget::GameWidget ( int startingLevel, QWidget *parent )
    : QWidget( parent ), m_moves(0)
{
    int nlevels = KGlobal::dirs()->findAllResources("appdata", "levels/level_*",
                                                    KStandardDirs::NoDuplicates).count();

    QVBoxLayout *top = new QVBoxLayout(this);
    top->setMargin(10);

    QHBoxLayout *hlay = new QHBoxLayout;
    hlay->addWidget( new QLabel(i18n("Level:"),this) );
    // scrollbar
    scrl = new QScrollBar( Qt::Horizontal );
    scrl->setRange(1, nlevels);
    scrl->setSingleStep(1);
    scrl->setPageStep(5);
    scrl->setValue(1);
    connect (scrl, SIGNAL (valueChanged (int)), SLOT (updateLevel (int)));

    hlay->addWidget(scrl,1);

    top->addLayout(hlay);

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

    updateLevel(startingLevel);
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
    config.setGroup("Savegame");
    config.writeEntry( "Level", m_level );
    m_playField->saveGame( config );
}

void GameWidget::loadGame()
{
    QString fileName = KFileDialog::getOpenFileName( KUrl(), "*.katomic", this );
    if(fileName.isEmpty())
        return;
    KConfig config(fileName, KConfig::OnlyLocal);
    config.setGroup("Savegame");
    int l = config.readEntry( "Level", 1 );
    m_level = l;
    updateLevel(m_level);
    m_playField->loadGame( config );
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

#include "gamewidget.moc"
