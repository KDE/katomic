/*

  Copyright (C) 1998   Andreas Wüst (AndreasWuest@gmx.de)
  Copyright (C) 2006-2007   Dmitry Suzdalev (dimsuz@gmail.com)

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
#include "highscores.h"
#include "playfield.h"
#include "prefs.h"

#include <QGraphicsView>
#include <QResizeEvent>
#include <QApplication> // for qApp->quit()
#include <QVBoxLayout>
#include <QTimer> // Next Level after N seconds
#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <kfiledialog.h>

GameWidget::GameWidget ( int startingLevel, QWidget *parent )
    : QWidget( parent ), m_allowAnyLevelSwitch( false ), m_moves(0)
{
    m_highscore = new KAtomicHighscores();
    m_levelHighscore = 0;

    QVBoxLayout *top = new QVBoxLayout(this);
    top->setMargin(0);

    // playfield
    m_playField = new PlayField(this);

    m_view = new QGraphicsView(m_playField, this);
    int defaultFieldSize = FIELD_SIZE*MIN_ELEM_SIZE;
    // reserve some room for molecule preview
    m_view->setMinimumSize( defaultFieldSize+defaultFieldSize/4, defaultFieldSize );
    m_view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_view->setFrameStyle( QFrame::NoFrame );
    m_view->setCacheMode( QGraphicsView::CacheBackground );

    m_view->setOptimizationFlags( QGraphicsView::DontClipPainter |
                                  QGraphicsView::DontSavePainterState |
                                  QGraphicsView::DontAdjustForAntialiasing );

    top->addWidget(m_view, 1);

    connect (m_playField, SIGNAL (gameOver(int)), SLOT(gameOver(int)));
    connect (m_playField, SIGNAL (updateMoves(int)), SLOT(updateMoves(int)));

    // Next level after some seconds after ending the game. See gameOver, nextLevel and prevLevel
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect (m_timer, SIGNAL(timeout()), this, SLOT(nextLevel()));

    m_levelSet.load("default_levels");
    switchToLevel(startingLevel);
}

GameWidget::~GameWidget()
{
    delete m_highscore;
}

QString GameWidget::currentMolecule() const
{
    return m_playField->moleculeName();
}

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

void GameWidget::gameOver(int moves) {
    // writing this info only in normal mode
    if ( !m_allowAnyLevelSwitch &&
		    Preferences::maxAccessibleLevel() < m_level+1 )
    {
        Preferences::setMaxAccessibleLevel( m_level+1 );
        Preferences::self()->writeConfig();
    }

    QString message = i18n( "Level %1 finished. ", m_level );

    if( m_highscore->addScore( moves , m_level ) ) // new highscore!
    {
        message += i18n("Congratulations! You have a new highscore!" );
    }

    m_playField->showMessage( message );
    emit statsChanged(m_level, moves, m_highscore->levelHighscore(m_level));
    if (!m_allowAnyLevelSwitch)
    {
        // reuse this signal to allow switching levels over toolbar
        emit levelChanged(m_level);
        // after 4 seconds, nextLevel
        m_timer->start(4000);
    }
}

void GameWidget::updateMoves(int moves)
{
    m_moves = moves;
    emit statsChanged(m_level, moves, m_levelHighscore);
}

void GameWidget::switchToLevel (int l)
{
    m_level=l;

    const LevelData* level = m_levelSet.levelData(m_level);
    if (level)
    {
        m_playField->setLevelData(level);

        m_levelHighscore = m_highscore->levelHighscore( m_level );

        emit statsChanged(m_level, 0, m_levelHighscore);
        emit levelChanged(m_level);
    }
    else
        kDebug() << "failed to load level" << m_level;
}

void GameWidget::restartLevel()
{
    switchToLevel(m_level);
}

void GameWidget::saveGame()
{
    QString fileName = KFileDialog::getSaveFileName( KUrl(), "*.katomic", this );
    if(fileName.isEmpty())
        return;
    KConfig config(fileName, KConfig::SimpleConfig);
    KConfigGroup gr = config.group("Savegame");
    gr.writeEntry( "Level", m_level );
    m_playField->saveGame( gr );
}

void GameWidget::loadGame()
{
    QString fileName = KFileDialog::getOpenFileName( KUrl(), "*.katomic", this );
    if(fileName.isEmpty())
        return;
    KConfig config(fileName, KConfig::SimpleConfig);
    KConfigGroup gr = config.group("Savegame");
    int l = gr.readEntry( "Level", 1 );
    m_level = l;
    switchToLevel(m_level);
    m_playField->loadGame( gr );
}

void GameWidget::showHighscores ()
{
    // TODO: Implement me! Please... 8-)
}

int GameWidget::currentHighScore() const
{
    return m_levelHighscore;
}

void GameWidget::prevLevel()
{
    // if user hits toolbar buttons, stop timer
    if (m_timer->isActive())
        m_timer->stop();
    if ( m_level == 1 )
        return;
    m_level--;
    switchToLevel(m_level);
}

void GameWidget::nextLevel()
{
    // if user hits toolbar buttons, stop timer
    if (m_timer->isActive())
        m_timer->stop();
    if ( !m_allowAnyLevelSwitch && m_level >= Preferences::maxAccessibleLevel() )
    {
        KMessageBox::information(this, i18n("You must solve level %1 before advancing to the next one!",
                                            Preferences::maxAccessibleLevel()), i18n("Warning"));
        return;
    }
    m_level++;
    switchToLevel(m_level);
}

void GameWidget::resizeEvent( QResizeEvent* ev)
{
    m_playField->resize( ev->size().width(), ev->size().height() );
}

#include "gamewidget.moc"
