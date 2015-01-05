/* toplevel.cpp

  Copyright (C) 1998   Andreas Wüst <AndreasWuest@gmx.de>
  Copyright (C) 2006   Dmitry Suzdalev <dimsuz@gmail.com>
  Copyright (C) 2007   Simon Hürlimann <simon.huerlimann@huerlisi.ch>

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

#include "toplevel.h"


#include <KLocalizedString>
#include <kconfig.h>
#include <kstandardaction.h>
#include <kstandardgameaction.h>
#include <kselectaction.h>
#include <kactioncollection.h>
#include <QDebug>
#include <QIcon>
#include <qstatusbar.h>

#include "gamewidget.h"
#include "playfield.h"
#include "prefs.h"
#include "commondefs.h"
#include "chooselevelsetdialog.h"

static const int LEVEL_BAR_ID = 0;
static const int CUR_SCORE_BAR_ID = 1;
static const int HIGHSCORE_BAR_ID = 2;
static const int MOLECULE_NAME_ID = 3;

AtomTopLevel::AtomTopLevel()
{
    QString lastPlayedLevelSet = Preferences::lastPlayedLevelSet();
    if (lastPlayedLevelSet.isEmpty())
        lastPlayedLevelSet = DEFAULT_LEVELSET_NAME;

    m_gameWid = new GameWidget( lastPlayedLevelSet, this);
    m_gameWid->setObjectName( QLatin1String("gamewidget" ));
    createMenu();
    loadSettings();
    setCentralWidget(m_gameWid);

    //QT5 statusBar()->insertItem( i18n("Level:"), LEVEL_BAR_ID , 1);
    //QT5 statusBar()->insertPermanentItem( i18n("Current score:"), CUR_SCORE_BAR_ID, 1);
    //QT5 statusBar()->insertPermanentItem( i18n("Highscore:"), HIGHSCORE_BAR_ID, 1 );
    //QT5 statusBar()->insertItem( QString(), MOLECULE_NAME_ID , 1);
    //QT5 statusBar()->setItemAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);

    updateStatusBar( m_gameWid->currentLevel(), m_gameWid->currentScore(), m_gameWid->currentHighScore() );

    connect(m_gameWid, &GameWidget::statsChanged, this, &AtomTopLevel::updateStatusBar);
    connect(m_gameWid, &GameWidget::levelChanged, this, &AtomTopLevel::levelHasChanged);

    // update next/prev actions
    levelHasChanged(m_gameWid->currentLevel());
    setupGUI();
}

AtomTopLevel::~AtomTopLevel()
{
}

bool AtomTopLevel::queryClose()
{
    // saves last played level and levelset
    m_gameWid->saveLastPlayedLevel();
    Preferences::self()->save();
    return true;
}

void AtomTopLevel::createMenu()
{
    // Game
    KStandardGameAction::restart(m_gameWid, SLOT(restartLevel()), actionCollection());
    // FIXME: comment this until highscore dialog will be implemented.
    // (katomic needs spesial one)
//    KStandardGameAction::highscores(m_gameWid, SLOT(showHighscores()), actionCollection());
    KStandardGameAction::load(m_gameWid, SLOT(loadGame()), actionCollection());
    KStandardGameAction::save(m_gameWid, SLOT(saveGame()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());

    // Move
    m_undoAct = KStandardGameAction::undo(m_gameWid->playfield(), SLOT(undo()), actionCollection());
    m_redoAct = KStandardGameAction::redo(m_gameWid->playfield(), SLOT(redo()), actionCollection());


    m_prevLevelAct = actionCollection()->addAction( QLatin1String(  "prev_level" ) );
    m_prevLevelAct->setIcon( QIcon::fromTheme( QLatin1String(  "arrow-left" ) ) );
    m_prevLevelAct->setText( i18n( "Previous Level" ) );
    actionCollection()->setDefaultShortcut(m_prevLevelAct, Qt::CTRL + Qt::Key_P );
    addAction( m_prevLevelAct );
    connect( m_prevLevelAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(prevLevel()) );

    m_nextLevelAct = actionCollection()->addAction( QLatin1String(  "next_level" ) );
    m_nextLevelAct->setIcon( QIcon::fromTheme( QLatin1String(  "arrow-right" ) ) );
    m_nextLevelAct->setText( i18n( "Next Level" ) );
    actionCollection()->setDefaultShortcut(m_nextLevelAct, Qt::CTRL + Qt::Key_N );
    addAction( m_nextLevelAct );
    connect( m_nextLevelAct, SIGNAL(triggered(bool)), m_gameWid, SLOT(nextLevel()) );

    QAction* chooseLevelSet = actionCollection()->addAction( QLatin1String(  "choose_level_set" ) );
    chooseLevelSet->setText( i18n( "Choose level set..." ) );
    addAction( chooseLevelSet );
    connect(chooseLevelSet, &QAction::triggered, this, &AtomTopLevel::chooseLevelSet);

    m_animSpeedAct = new KSelectAction(i18n("Animation Speed"), this);
    actionCollection()->addAction( QLatin1String( "anim_speed" ), m_animSpeedAct);
    QStringList acts;
    acts << i18n("Slow") << i18n("Normal") << i18n("Fast");
    m_animSpeedAct->setItems(acts);
    connect(m_animSpeedAct, static_cast<void (KSelectAction::*)(int)>(&KSelectAction::triggered), this, &AtomTopLevel::slotAnimSpeedChanged);

    QAction *undoAll = actionCollection()->addAction( QLatin1String(  "move_undo_all" ) );
    undoAll->setIcon( QIcon::fromTheme( QLatin1String( "media-skip-backward" )) );
    undoAll->setText( i18n("Undo All") );
    connect( undoAll, SIGNAL(triggered(bool)), m_gameWid->playfield(), SLOT(undoAll()) );

    QAction *redoAll = actionCollection()->addAction( QLatin1String(  "move_redo_all" ) );
    redoAll->setIcon( QIcon::fromTheme( QLatin1String( "media-skip-forward" )) );
    redoAll->setText( i18n("Redo All") );
    connect( redoAll, SIGNAL(triggered(bool)), m_gameWid->playfield(), SLOT(redoAll()) );

    m_undoAct->setEnabled(false);
    m_redoAct->setEnabled(false);
    undoAll->setEnabled(false);
    redoAll->setEnabled(false);

    connect (m_gameWid->playfield(), SIGNAL (enableRedo(bool)), m_redoAct, SLOT(setEnabled(bool)));
    connect (m_gameWid->playfield(), SIGNAL (enableUndo(bool)), m_undoAct, SLOT(setEnabled(bool)));
    connect (m_gameWid->playfield(), SIGNAL (enableUndo(bool)), undoAll, SLOT(setEnabled(bool)));
    connect (m_gameWid->playfield(), SIGNAL (enableRedo(bool)), redoAll, SLOT(setEnabled(bool)));

    QAction * atomUpAct = actionCollection()->addAction( QLatin1String( "atom_up" ));
    atomUpAct->setText(i18n("Atom Up"));
    actionCollection()->setDefaultShortcut(atomUpAct, Qt::Key_Up);
    addAction(atomUpAct);
    connect(atomUpAct, &QAction::triggered, m_gameWid, &GameWidget::moveUp);

    QAction * atomDownAct = actionCollection()->addAction( QLatin1String( "atom_down" ));
    atomDownAct->setText(i18n("Atom Down"));
    actionCollection()->setDefaultShortcut(atomDownAct,Qt::Key_Down);
    addAction(atomDownAct);
    connect(atomDownAct, &QAction::triggered, m_gameWid, &GameWidget::moveDown);

    QAction * atomLeftAct = actionCollection()->addAction( QLatin1String( "atom_left" ));
    atomLeftAct->setText(i18n("Atom Left"));
    actionCollection()->setDefaultShortcut(atomLeftAct,Qt::Key_Left);
    addAction(atomLeftAct);
    connect(atomLeftAct, &QAction::triggered, m_gameWid, &GameWidget::moveLeft);

    QAction * atomRightAct = actionCollection()->addAction( QLatin1String( "atom_right" ));
    atomRightAct->setText(i18n("Atom Right"));
    actionCollection()->setDefaultShortcut(atomRightAct,Qt::Key_Right);
    addAction(atomRightAct);
    connect(atomRightAct, &QAction::triggered, m_gameWid, &GameWidget::moveRight);

    QAction * nextAtomAct = actionCollection()->addAction( QLatin1String( "next_atom" ));
    nextAtomAct->setText(i18n("Next Atom"));
    actionCollection()->setDefaultShortcut(nextAtomAct,Qt::Key_Tab);
    addAction(nextAtomAct);
    connect(nextAtomAct, SIGNAL(triggered(bool)), m_gameWid->playfield(), SLOT(nextAtom()));

    QAction * prevAtomAct = actionCollection()->addAction( QLatin1String( "prev_atom" ));
    prevAtomAct->setText(i18n("Previous Atom"));
    actionCollection()->setDefaultShortcut(prevAtomAct,Qt::SHIFT+Qt::Key_Tab);
    addAction(prevAtomAct);
    connect(prevAtomAct, SIGNAL(triggered(bool)), m_gameWid->playfield(), SLOT(previousAtom()));
}

void AtomTopLevel::loadSettings()
{
    m_animSpeedAct->setCurrentItem( Preferences::animationSpeed() );
    m_gameWid->playfield()->setAnimationSpeed( Preferences::animationSpeed() );
}

void AtomTopLevel::slotAnimSpeedChanged(int speed)
{
    m_gameWid->playfield()->setAnimationSpeed(speed);
    Preferences::setAnimationSpeed(speed);
    Preferences::self()->save();
}

void AtomTopLevel::updateStatusBar( int level, int score, int highscore )
{
    //QT5 statusBar()->changeItem( i18n("Level: %1 (%2)", level, m_gameWid->levelSet().visibleName()), LEVEL_BAR_ID );
    //QT5 statusBar()->changeItem( i18n("Current score: %1", score), CUR_SCORE_BAR_ID );
    QString str;
    if(highscore == 0)
        str = '-';
    else
        str.setNum(highscore);
    //QT5 statusBar()->changeItem( i18n("Highscore: %1", str), HIGHSCORE_BAR_ID );
}

void AtomTopLevel::enableHackMode()
{
    //qDebug() << "Enabling hack mode";
    m_prevLevelAct->setDisabled(false);
    m_nextLevelAct->setDisabled(false);
    m_gameWid->enableSwitchToAnyLevel();
}

void AtomTopLevel::levelHasChanged(int level)
{
    // Update level name
    //QT5 statusBar()->changeItem( m_gameWid->currentMolecule(), MOLECULE_NAME_ID);
    // don't gray out actions in hackmode, else do
    if(!m_gameWid->switchToAnyLevelAllowed())
        updateActionsForLevel(level);
}

void AtomTopLevel::updateActionsForLevel(int)
{
    m_prevLevelAct->setEnabled( m_gameWid->isPrevLevelAvailable() );
    m_nextLevelAct->setEnabled( m_gameWid->isNextLevelAvailable() );
}

void AtomTopLevel::chooseLevelSet()
{
    // will delete itself on close
    ChooseLevelSetDialog* dlg = new ChooseLevelSetDialog(this);
    connect(dlg, &ChooseLevelSetDialog::levelSetChanged, this, &AtomTopLevel::changeLevelSet);

    dlg->setCurrentLevelSet(m_gameWid->levelSet().name());
    dlg->show();
}

void AtomTopLevel::changeLevelSet(const QString& levelSet)
{
    if (!levelSet.isEmpty())
        m_gameWid->setLevelSet(levelSet);
}


