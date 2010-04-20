/*

  Copyright (C) 1998   Andreas Wüst (AndreasWuest@gmx.de)
  Copyright (C) 2006-2009   Dmitry Suzdalev (dimsuz@gmail.com)

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
#include <kdebug.h>

GameWidget::GameWidget ( const QString& levelSet, QWidget *parent )
    : QWidget( parent ), m_allowAnyLevelSwitch( false ), m_moves(0), m_level(0)
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

    // TODO uncomment DontSavePainterState optimization back after this bug in Qt 4.6 preview will be
    // fixed (darktears promised it will get fixed in 4.6.0 release). Bug is about wrong coordinates
    // for QPainter passed to QGS::drawForeground() function
    m_view->setOptimizationFlags( QGraphicsView::DontClipPainter |
                                  //QGraphicsView::DontSavePainterState |
                                  QGraphicsView::DontAdjustForAntialiasing );

    top->addWidget(m_view, 1);

    connect (m_playField, SIGNAL (gameOver(int)), SLOT(gameOver(int)));
    connect (m_playField, SIGNAL (updateMoves(int)), SLOT(updateMoves(int)));

    // Next level after some seconds after ending the game. See gameOver, nextLevel and prevLevel
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect (m_timer, SIGNAL(timeout()), this, SLOT(nextLevel()));

    setLevelSet(levelSet);
}

GameWidget::~GameWidget()
{
    delete m_highscore;
}


bool GameWidget::setLevelSet(const QString& levelSet)
{
    if (m_levelSet.name() == levelSet)
    {
        kDebug() << "level set named" << levelSet << "is already loaded";
        return true;
    }

    bool res = m_levelSet.load(levelSet);
    if (!res)
    {
        KMessageBox::error(this, i18n("Failed to load level set \"%1\". Check if it is installed on your computer.", levelSet));
        kDebug() << "failed to load levelset" << levelSet;
        return false;
    }

    int lastPlayed = lastPlayedLevel();
    int maxLevel = maxAccessibleLevel();

    int startingLevel = qMin(lastPlayed, maxLevel);
    switchToLevel(startingLevel);

    return true;
}

const LevelSet& GameWidget::levelSet() const
{
    return m_levelSet;
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
         maxAccessibleLevel() < m_level+1 )
    {
        saveMaxAccessibleLevel( m_level+1 );
    }

    QString message = i18n( "Level %1 finished. ", m_level );

    if( m_highscore->addScore( moves , m_levelSet.name(), m_level ) ) // new highscore!
    {
        message += i18n("Congratulations! You have a new highscore!" );
    }

    m_playField->showMessage( message );
    emit statsChanged(m_level, moves, m_highscore->levelHighscore(m_levelSet.name(), m_level));
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
    const LevelData* levelData = m_levelSet.levelData(l);
    if (levelData)
    {
        m_level=l;

        m_playField->setLevelData(levelData);

        m_levelHighscore = m_highscore->levelHighscore( m_levelSet.name(), m_level );

        emit statsChanged(m_level, 0, m_levelHighscore);
        emit levelChanged(m_level);

        saveLastPlayedLevel();
    }
    else
        kDebug() << "failed to load level" << l;
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
    gr.writeEntry( "LevelSet", m_levelSet.name() );
    m_playField->saveGame( gr );
}

void GameWidget::loadGame()
{
    QString fileName = KFileDialog::getOpenFileName( KUrl(), "*.katomic", this );
    if(fileName.isEmpty())
        return;
    KConfig config(fileName, KConfig::SimpleConfig);
    KConfigGroup gr = config.group("Savegame");
    QString levelSet = gr.readEntry("LevelSet");
    if (levelSet.isEmpty())
    {
        kDebug() << "note: savegame file doesn't contain info about levelset, assuming default one";
        levelSet = DEFAULT_LEVELSET_NAME;
    }

    bool res = setLevelSet(levelSet);
    if (res)
    {
        int l = gr.readEntry( "Level", 1 );
        switchToLevel(l);
        m_playField->loadGame( gr );
    }
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

    if (isPrevLevelAvailable())
    {
        switchToLevel(m_level-1);
    }
}

void GameWidget::nextLevel()
{
    // if user hits toolbar buttons, stop timer
    if (m_timer->isActive())
        m_timer->stop();

    if (isNextLevelAvailable())
    {
        switchToLevel(m_level+1);
    }
}

void GameWidget::resizeEvent( QResizeEvent* ev)
{
    m_playField->resize( ev->size().width(), ev->size().height() );
}

void GameWidget::saveLastPlayedLevel()
{
    KSharedConfigPtr cfg = KGlobal::config();
    KConfigGroup grp(cfg, m_levelSet.name());
    grp.writeEntry("LastPlayedLevel", m_level);

    Preferences::setLastPlayedLevelSet(m_levelSet.name());
}

void GameWidget::saveMaxAccessibleLevel(int level)
{
    KSharedConfigPtr cfg = KGlobal::config();
    KConfigGroup grp(cfg, m_levelSet.name());
    grp.writeEntry("MaxAccessibleLevel", level);
}

int GameWidget::lastPlayedLevel() const
{
    KSharedConfigPtr cfg = KGlobal::config();
    KConfigGroup grp(cfg, m_levelSet.name());
    int lastPlayed = grp.readEntry("LastPlayedLevel", 1);
    lastPlayed = qMax(1, lastPlayed); // can't be less than 1
    kDebug() << "last played level:" << lastPlayed;
    return lastPlayed;
}

int GameWidget::maxAccessibleLevel() const
{
    KSharedConfigPtr cfg = KGlobal::config();
    KConfigGroup grp(cfg, m_levelSet.name());
    int maxAccessible = grp.readEntry("MaxAccessibleLevel", 1);
    kDebug() << "max accessible level:" << maxAccessible;
    return maxAccessible;
}

bool GameWidget::isNextLevelAvailable() const
{
    bool avail = m_allowAnyLevelSwitch ? true :
                (m_level != maxAccessibleLevel() && m_level <= m_levelSet.levelCount());
    return avail;
}

bool GameWidget::isPrevLevelAvailable() const
{
    return m_level > 1;
}

#include "gamewidget.moc"
