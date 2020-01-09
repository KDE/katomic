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
#include <kstandardgameaction.h>
#include <kselectaction.h>
#include <kactioncollection.h>
#include "katomic_debug.h"
#include <QIcon>
#include <qstatusbar.h>
#include <QLabel>

#include "gamewidget.h"
#include "playfield.h"
#include "prefs.h"
#include "commondefs.h"
#include "chooselevelsetdialog.h"


AtomTopLevel::AtomTopLevel()
{
    QString lastPlayedLevelSet = Preferences::lastPlayedLevelSet();
    if (lastPlayedLevelSet.isEmpty())
        lastPlayedLevelSet = QStringLiteral(DEFAULT_LEVELSET_NAME);

    m_gameWid = new GameWidget( lastPlayedLevelSet, this);
    m_gameWid->setObjectName( QStringLiteral("gamewidget" ));
    createMenu();
    loadSettings();
    setCentralWidget(m_gameWid);

    mLevel = new QLabel(i18n("Level:"));
    mCurrentScore = new QLabel(i18n("Current score:"));
    mHighScore = new QLabel(i18n("Highscore:"));
    mMoleculeName = new QLabel;
    statusBar()->addWidget(mLevel);
    statusBar()->addPermanentWidget(mCurrentScore);
    statusBar()->addPermanentWidget(mHighScore);
    statusBar()->addWidget(mMoleculeName);

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
    // (katomic needs special one)
    //    KStandardGameAction::highscores(m_gameWid, SLOT(showHighscores()), actionCollection());
    KStandardGameAction::load(m_gameWid, SLOT(loadGame()), actionCollection());
    KStandardGameAction::save(m_gameWid, SLOT(saveGame()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());

    // Move
    m_undoAct = KStandardGameAction::undo(m_gameWid->playfield(), SLOT(undo()), actionCollection());
    m_redoAct = KStandardGameAction::redo(m_gameWid->playfield(), SLOT(redo()), actionCollection());


    m_prevLevelAct = actionCollection()->addAction( QStringLiteral(  "prev_level" ) );
    m_prevLevelAct->setIcon( QIcon::fromTheme( QStringLiteral(  "arrow-left" ) ) );
    m_prevLevelAct->setText( i18n( "Previous Level" ) );
    actionCollection()->setDefaultShortcut(m_prevLevelAct, Qt::CTRL + Qt::Key_P );
    addAction( m_prevLevelAct );
    connect( m_prevLevelAct, &QAction::triggered, m_gameWid, &GameWidget::prevLevel );

    m_nextLevelAct = actionCollection()->addAction( QStringLiteral(  "next_level" ) );
    m_nextLevelAct->setIcon( QIcon::fromTheme( QStringLiteral(  "arrow-right" ) ) );
    m_nextLevelAct->setText( i18n( "Next Level" ) );
    actionCollection()->setDefaultShortcut(m_nextLevelAct, Qt::CTRL + Qt::Key_N );
    addAction( m_nextLevelAct );
    connect( m_nextLevelAct, &QAction::triggered, m_gameWid, &GameWidget::nextLevel );

    QAction* chooseLevelSet = actionCollection()->addAction( QStringLiteral(  "choose_level_set" ) );
    chooseLevelSet->setText( i18n( "Choose level set..." ) );
    addAction( chooseLevelSet );
    connect(chooseLevelSet, &QAction::triggered, this, &AtomTopLevel::chooseLevelSet);

    m_animSpeedAct = new KSelectAction(i18n("Animation Speed"), this);
    actionCollection()->addAction( QStringLiteral( "anim_speed" ), m_animSpeedAct);
    QStringList acts;
    acts << i18n("Slow") << i18n("Normal") << i18n("Fast");
    m_animSpeedAct->setItems(acts);
    connect(m_animSpeedAct, static_cast<void (KSelectAction::*)(int)>(&KSelectAction::triggered), this, &AtomTopLevel::slotAnimSpeedChanged);

    QAction *undoAll = actionCollection()->addAction( QStringLiteral(  "move_undo_all" ) );
    undoAll->setIcon( QIcon::fromTheme( QStringLiteral( "media-skip-backward" )) );
    undoAll->setText( i18n("Undo All") );
    connect( undoAll, &QAction::triggered, m_gameWid->playfield(), &PlayField::undoAll );

    QAction *redoAll = actionCollection()->addAction( QStringLiteral(  "move_redo_all" ) );
    redoAll->setIcon( QIcon::fromTheme( QStringLiteral( "media-skip-forward" )) );
    redoAll->setText( i18n("Redo All") );
    connect( redoAll, &QAction::triggered, m_gameWid->playfield(), &PlayField::redoAll );

    m_undoAct->setEnabled(false);
    m_redoAct->setEnabled(false);
    undoAll->setEnabled(false);
    redoAll->setEnabled(false);

    connect (m_gameWid->playfield(), &PlayField::enableRedo, m_redoAct, &QAction::setEnabled);
    connect (m_gameWid->playfield(), &PlayField::enableUndo, m_undoAct, &QAction::setEnabled);
    connect (m_gameWid->playfield(), &PlayField::enableUndo, undoAll, &QAction::setEnabled);
    connect (m_gameWid->playfield(), &PlayField::enableRedo, redoAll, &QAction::setEnabled);

    QAction * atomUpAct = actionCollection()->addAction( QStringLiteral( "atom_up" ));
    atomUpAct->setText(i18n("Atom Up"));
    actionCollection()->setDefaultShortcut(atomUpAct, Qt::Key_Up);
    addAction(atomUpAct);
    connect(atomUpAct, &QAction::triggered, m_gameWid, &GameWidget::moveUp);

    QAction * atomDownAct = actionCollection()->addAction( QStringLiteral( "atom_down" ));
    atomDownAct->setText(i18n("Atom Down"));
    actionCollection()->setDefaultShortcut(atomDownAct,Qt::Key_Down);
    addAction(atomDownAct);
    connect(atomDownAct, &QAction::triggered, m_gameWid, &GameWidget::moveDown);

    QAction * atomLeftAct = actionCollection()->addAction( QStringLiteral( "atom_left" ));
    atomLeftAct->setText(i18n("Atom Left"));
    actionCollection()->setDefaultShortcut(atomLeftAct,Qt::Key_Left);
    addAction(atomLeftAct);
    connect(atomLeftAct, &QAction::triggered, m_gameWid, &GameWidget::moveLeft);

    QAction * atomRightAct = actionCollection()->addAction( QStringLiteral( "atom_right" ));
    atomRightAct->setText(i18n("Atom Right"));
    actionCollection()->setDefaultShortcut(atomRightAct,Qt::Key_Right);
    addAction(atomRightAct);
    connect(atomRightAct, &QAction::triggered, m_gameWid, &GameWidget::moveRight);

    QAction * nextAtomAct = actionCollection()->addAction( QStringLiteral( "next_atom" ));
    nextAtomAct->setText(i18n("Next Atom"));
    actionCollection()->setDefaultShortcut(nextAtomAct,Qt::Key_Tab);
    addAction(nextAtomAct);
    connect(nextAtomAct, &QAction::triggered, m_gameWid->playfield(), &PlayField::nextAtom);

    QAction * prevAtomAct = actionCollection()->addAction( QStringLiteral( "prev_atom" ));
    prevAtomAct->setText(i18n("Previous Atom"));
    actionCollection()->setDefaultShortcut(prevAtomAct,Qt::SHIFT+Qt::Key_Tab);
    addAction(prevAtomAct);
    connect(prevAtomAct, &QAction::triggered, m_gameWid->playfield(), &PlayField::previousAtom);
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
    mLevel->setText(i18n("Level: %1 (%2)", level, m_gameWid->levelSet().visibleName()));
    mCurrentScore->setText(i18n("Current score: %1", score));
    QString str;
    if(highscore == 0)
        str = QLatin1Char('-');
    else
        str.setNum(highscore);
    mHighScore->setText(i18n("Highscore: %1", str));
}

void AtomTopLevel::enableHackMode()
{
    //qCDebug(KATOMIC_LOG) << "Enabling hack mode";
    m_prevLevelAct->setDisabled(false);
    m_nextLevelAct->setDisabled(false);
    m_gameWid->enableSwitchToAnyLevel();
}

void AtomTopLevel::levelHasChanged(int level)
{
    // Update level name
    mMoleculeName->setText(m_gameWid->currentMolecule());
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


