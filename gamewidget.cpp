/* toplevel.cpp

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
  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.

  */

#include "settings.h"
#include "gamewidget.h"
#include "feld.h"
#include "molek.h"

#include <qscrollbar.h>
#include <q3groupbox.h>
#include <qlayout.h>
#include <q3vbox.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>

#include <kscoredialog.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <ksimpleconfig.h>
#include <kglobalsettings.h>

Options settings;

#define MPOSX 480
#define MPOSY 90

// ##########################
// #	class GameWidget    #
// ##########################

int level;

void GameWidget::moveUp()
{
  feld->startAnimation (Feld::MoveUp);
}

void GameWidget::moveDown()
{
  feld->startAnimation (Feld::MoveDown);
}

void GameWidget::moveLeft()
{
  feld->startAnimation (Feld::MoveLeft);
}

void GameWidget::moveRight()
{
  feld->startAnimation (Feld::MoveRight);
}

void GameWidget::nextAtom()
{
  feld->nextAtom();
}

void GameWidget::previousAtom()
{
  feld->previousAtom();
}

void GameWidget::getButton (int button)
{
    feld->startAnimation ((Feld::Direction)button);
}

void GameWidget::doUndo ()
{
  feld->doUndo ();
}

void GameWidget::doRedo ()
{
  feld->doRedo ();
}

void GameWidget::gameOver(int moves) {
    KMessageBox::information(this, i18n("You solved level %1 with %2 moves!").arg(level).arg(moves), i18n("Congratulations"));

    KScoreDialog high(KScoreDialog::Name | KScoreDialog::Score, this);
    high.setCaption(i18n("Level %1 Highscores").arg(level));
    high.setConfigGroup(QString("Highscores Level %1").arg(level));

    KScoreDialog::FieldInfo scoreInfo;

    if (high.addScore(moves, scoreInfo, true, true))
    {
       high.exec();
    }
    updateLevel(level+1);
}

void GameWidget::getMoves(int moves)
{
    current.setNum(moves);
    ys->setText(current);
}

void GameWidget::updateLevel (int l)
{
    level=l;
    QString levelFile = locate("appdata", QString("levels/level_%1").arg(l));
    if (levelFile.isNull()) {
	return updateLevel(1);
    }

    KSimpleConfig cfg(levelFile, true);
    cfg.setGroup("Level");
    feld->load(cfg);

    highScore->setConfigGroup(QString("High Scores Level %1").arg(level));
    highest.setNum(highScore->highScore());

    if (highest != "0" ) hs->setText(highest);
    else hs->setText("-");
    ys->setText("0");
    scrl->setValue(level);

    feld->repaint();
}

void GameWidget::restartLevel()
{
    updateLevel(level);
}

GameWidget::GameWidget ( QWidget *parent, const char* name )
    : QWidget( parent, name )
{
    level = 1;
    nlevels = KGlobal::dirs()->findAllResources("appdata", "levels/level_*",
				false, true).count();

    QHBoxLayout *top = new QHBoxLayout(this, 10);

    // spielfeld
    feld = new Feld (this, "feld");
    feld->setFocus();

    top->addWidget(feld);

    Q3VBox *vb = new Q3VBox(this);
    vb->setSpacing(20);
    top->addWidget(vb);

    // scrollbar
    scrl = new QScrollBar(1, nlevels, 1,
			5, 1, Qt::Horizontal, vb, "scrl" );
    connect (scrl, SIGNAL (valueChanged (int)), SLOT (updateLevel (int)));

    // molekül
    molek = new Molek (vb, "molek");
    feld->setMolek(molek);

    connect (feld, SIGNAL (gameOver(int)), SLOT(gameOver(int)));
    connect (feld, SIGNAL (sendMoves(int)), SLOT(getMoves(int)));
    connect (feld, SIGNAL (enableRedo(bool)), SIGNAL(enableRedo(bool)));
    connect (feld, SIGNAL (enableUndo(bool)), SIGNAL(enableUndo(bool)));

    highScore = new KScoreDialog(KScoreDialog::Name | KScoreDialog::Score, this);

    // the score group
    Q3GroupBox *bg = new Q3GroupBox (i18n("Score"), vb, "bg");
    QBoxLayout *slay = new QVBoxLayout (bg, 10);

    slay->addSpacing(10);

    slay->addWidget(new QLabel(i18n("Highscore:"), bg));

    QFont headerFont = KGlobalSettings::generalFont();
    headerFont.setBold(true);

    hs = new QLabel (highest, bg);
    hs->setAlignment(Qt::AlignRight);
    hs->setFont(headerFont);
    slay->addWidget(hs);

    slay->addSpacing(10);

    slay->addWidget(new QLabel(i18n("Your score so far:"), bg));

    ys = new QLabel (current, bg);
    ys->setAlignment(Qt::AlignRight);
    ys->setFont(headerFont);
    slay->addWidget(ys);

    updateLevel(1);

    KConfig *config = KGlobal::config();
    config->setGroup("Options");
    settings.anim_speed = config->readNumEntry("Animation Speed", 1);
    if (settings.anim_speed < 1 || settings.anim_speed > MAX_SPEED)
	settings.anim_speed = 1;

    settings.changed = false;
}

GameWidget::~GameWidget()
{
}

void GameWidget::showHighscores ()
{
    KScoreDialog high(KScoreDialog::Name | KScoreDialog::Score, this);
    high.setCaption(i18n("Level %1 Highscores").arg(level));
    high.setConfigGroup(QString("Highscores Level %1").arg(level));
    high.exec();
}

#include "gamewidget.moc"
