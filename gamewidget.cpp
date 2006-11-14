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
#include <ksimpleconfig.h>
#include <kglobalsettings.h>
#include <kfiledialog.h>

// FIXME dimsuz: get rid of it
int level;

void GameWidget::moveUp()
{
    m_playField->moveSelectedAtom( PlayField::Up );
}

void GameWidget::moveDown()
{
    m_playField->moveSelectedAtom( PlayField::Down );
}

void GameWidget::moveLeft()
{
    m_playField->moveSelectedAtom( PlayField::Left );
}

void GameWidget::moveRight()
{
    m_playField->moveSelectedAtom( PlayField::Right );
}

void GameWidget::nextAtom()
{
    m_playField->nextAtom();
}

void GameWidget::previousAtom()
{
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
    KMessageBox::information(this, i18n("You solved level %1 with %2 moves!", level, moves), i18n("Congratulations"));

    KScoreDialog high(KScoreDialog::Name | KScoreDialog::Score, this);
    high.setCaption(i18n("Level %1 Highscores", level));
    high.setConfigGroup(QString("Highscores Level %1").arg(level));

    KScoreDialog::FieldInfo scoreInfo;

    if (high.addScore(moves, scoreInfo, true, true))
    {
        high.exec();
    }
}

void GameWidget::updateMoves(int moves)
{
    current.setNum(moves);
    ys->setText(current);
}

void GameWidget::updateLevel (int l)
{
    level=l;
    QString levelFile = KStandardDirs::locate("appdata", QString("levels/level_%1").arg(l));
    if (levelFile.isNull()) {
        return updateLevel(1);
    }

    KSimpleConfig cfg(levelFile, true);
    cfg.setGroup("Level");
    m_playField->loadLevel(cfg);
    m_view->resetCachedContent();
    m_view->update();

    highScore->setConfigGroup(QString("Highscores Level %1").arg(level));
    highest.setNum(highScore->highScore());

    if (highest != "0" ) hs->setText(highest);
    else hs->setText("-");
    ys->setText("0");
    scrl->setValue(level);
}

void GameWidget::restartLevel()
{
    updateLevel(level);
}

GameWidget::GameWidget ( QWidget *parent )
    : QWidget( parent )
{
    level = 1;
    nlevels = KGlobal::dirs()->findAllResources("appdata", "levels/level_*",
            false, true).count();

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

    // the score group
    QGroupBox *bg = new QGroupBox (i18n("Score"));
    QGridLayout *slay = new QGridLayout(bg);
    slay->setMargin(10);
    slay->addWidget(new QLabel(i18n("Highscore:"), bg), 0, 0);

    QFont headerFont = KGlobalSettings::generalFont();
    headerFont.setBold(true);

    hs = new QLabel (highest, bg);
    hs->setAlignment(Qt::AlignRight);
    hs->setFont(headerFont);
    slay->addWidget(hs, 0, 1);
    slay->addWidget(new QLabel(i18n("Your score so far:"), bg), 1, 0);

    ys = new QLabel (current, bg);
    ys->setAlignment(Qt::AlignRight);
    ys->setFont(headerFont);
    slay->addWidget(ys, 1, 1);

    // FIXME dimsuz: decide where to put this stuff
    bg->hide();

    updateLevel(1);
}

GameWidget::~GameWidget()
{
}

void GameWidget::saveGame()
{
    QString fileName = KFileDialog::getSaveFileName( KUrl(), "*.katomic", this );
    if(fileName.isEmpty())
        return;
    KSimpleConfig config(fileName);
    config.setGroup("Savegame");
    config.writeEntry( "Level", level );
    m_playField->saveGame( config );
}

void GameWidget::loadGame()
{
    QString fileName = KFileDialog::getOpenFileName( KUrl(), "*.katomic", this );
    if(fileName.isEmpty())
        return;
    KSimpleConfig config(fileName);
    config.setGroup("Savegame");
    int l = config.readEntry( "Level", 1 );
    level = l;
    updateLevel(level);
    m_playField->loadGame( config );
}

void GameWidget::showHighscores ()
{
    KScoreDialog high(KScoreDialog::Name | KScoreDialog::Score, this);
    high.setCaption(i18n("Level %1 Highscores", level));
    high.setConfigGroup(QString("Highscores Level %1").arg(level));
    high.exec();
}

#include "gamewidget.moc"
