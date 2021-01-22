/*
    SPDX-FileCopyrightText: 1998 Andreas Wüst <AndreasWuest@gmx.de>
    SPDX-FileCopyrightText: 2006-2009 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "gamewidget.h"

#include "highscores.h"
#include "playfield.h"
#include "prefs.h"
#include "katomic_debug.h"

#include <KMessageBox>
#include <KLocalizedString>
#include <KConfig>

#include <QGraphicsView>
#include <QResizeEvent>
#include <QApplication> // for qApp->quit()
#include <QVBoxLayout>
#include <QTimer> // Next Level after N seconds
#include <QFileDialog>

GameWidget::GameWidget ( const QString& levelSet, QWidget *parent )
    : QWidget( parent ), m_allowAnyLevelSwitch( false ), m_moves(0), m_level(0)
{
    m_highscore = new KAtomicHighscores();
    m_levelHighscore = 0;

    QVBoxLayout *top = new QVBoxLayout(this);
    top->setContentsMargins(0, 0, 0, 0);

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
    m_view->setOptimizationFlags( 
            //QGraphicsView::DontSavePainterState |
            QGraphicsView::DontAdjustForAntialiasing );

    top->addWidget(m_view, 1);

    connect(m_playField, &PlayField::gameOver, this, &GameWidget::gameOver);
    connect(m_playField, &PlayField::updateMoves, this, &GameWidget::updateMoves);

    // Next level after some seconds after ending the game. See gameOver, nextLevel and prevLevel
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &GameWidget::nextLevel);

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
        //qCDebug(KATOMIC_LOG) << "level set named" << levelSet << "is already loaded";
        return true;
    }

    bool res = m_levelSet.load(levelSet);
    if (!res)
    {
        KMessageBox::error(this, i18n("Failed to load level set \"%1\". Check if it is installed on your computer.", levelSet));
        //qCDebug(KATOMIC_LOG) << "failed to load levelset" << levelSet;
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
    Q_EMIT statsChanged(m_level, moves, m_highscore->levelHighscore(m_levelSet.name(), m_level));
    if (!m_allowAnyLevelSwitch)
    {
        // reuse this signal to allow switching levels over toolbar
        Q_EMIT levelChanged(m_level);
        // after 4 seconds, nextLevel
        m_timer->start(4000);
    }
}

void GameWidget::updateMoves(int moves)
{
    m_moves = moves;
    Q_EMIT statsChanged(m_level, moves, m_levelHighscore);
}

void GameWidget::switchToLevel (int l)
{
    const LevelData* levelData = m_levelSet.levelData(l);
    if (levelData)
    {
        m_level=l;

        m_playField->setLevelData(levelData);

        m_levelHighscore = m_highscore->levelHighscore( m_levelSet.name(), m_level );

        Q_EMIT statsChanged(m_level, 0, m_levelHighscore);
        Q_EMIT levelChanged(m_level);

        saveLastPlayedLevel();
    }
    else {
        //qCDebug(KATOMIC_LOG) << "failed to load level" << l;
    }
}

void GameWidget::restartLevel()
{
    switchToLevel(m_level);
}

void GameWidget::saveGame()
{
    QString fileName = QFileDialog::getSaveFileName( this, QString(), QString(), QStringLiteral("*.katomic") );
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
    QString fileName = QFileDialog::getOpenFileName( this, QString(), QString(), QStringLiteral("*.katomic") );
    if(fileName.isEmpty())
        return;
    KConfig config(fileName, KConfig::SimpleConfig);
    KConfigGroup gr = config.group("Savegame");
    QString levelSet = gr.readEntry("LevelSet");
    if (levelSet.isEmpty())
    {
        //qCDebug(KATOMIC_LOG) << "note: savegame file doesn't contain info about levelset, assuming default one";
        levelSet = QStringLiteral(DEFAULT_LEVELSET_NAME);
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
    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup grp(cfg, m_levelSet.name());
    grp.writeEntry("LastPlayedLevel", m_level);

    Preferences::setLastPlayedLevelSet(m_levelSet.name());
}

void GameWidget::saveMaxAccessibleLevel(int level)
{
    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup grp(cfg, m_levelSet.name());
    grp.writeEntry("MaxAccessibleLevel", level);
}

int GameWidget::lastPlayedLevel() const
{
    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup grp(cfg, m_levelSet.name());
    int lastPlayed = grp.readEntry("LastPlayedLevel", 1);
    lastPlayed = qMax(1, lastPlayed); // can't be less than 1
    //qCDebug(KATOMIC_LOG) << "last played level:" << lastPlayed;
    return lastPlayed;
}

int GameWidget::maxAccessibleLevel() const
{
    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup grp(cfg, m_levelSet.name());
    int maxAccessible = grp.readEntry("MaxAccessibleLevel", 1);
    //qCDebug(KATOMIC_LOG) << "max accessible level:" << maxAccessible;
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


