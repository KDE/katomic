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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  */

#include "settings.h"
#include "gamewidget.h"
#include "feld.h"
#include "molek.h"

#include <qscrollbar.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qlabel.h>

#include <kscoredialog.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <ksimpleconfig.h>

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

void GameWidget::gameOver(int moves) {
    KMessageBox::information(this, i18n("You solved level %1 with %2 moves!").arg(level).arg(moves), i18n("Congratulations"));

    KScoreDialog high(KScoreDialog::Name | KScoreDialog::Score, this);
    high.setCaption(i18n("Level %1 High Scores").arg(level));
    high.setConfigGroup(QString("High Scores Level %1").arg(level));

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

    KScoreDialog high(KScoreDialog::Name | KScoreDialog::Score, this);
    high.setConfigGroup(QString("High Scores Level %1").arg(level));
    highest.setNum(high.highScore());

    hs->setText(highest);
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

    QVBox *vb = new QVBox(this);
    vb->setSpacing(20);
    top->addWidget(vb);

    // scrollbar
    scrl = new QScrollBar(1, nlevels, 1,
			5, 1, QScrollBar::Horizontal, vb, "scrl" );
    // scrl->setGeometry( MPOSX, 50, 160, 16 );
    connect (scrl, SIGNAL (valueChanged (int)), SLOT (updateLevel (int)));

    // molekül
    molek = new Molek (vb, "molek");
    feld->setMolek(molek);

    connect (feld, SIGNAL (gameOver(int)), SLOT(gameOver(int)));
    connect (feld, SIGNAL (sendMoves(int)), SLOT(getMoves(int)));

    // the score group
    QGroupBox *bg = new QGroupBox (i18n("Score"), vb, "bg");
    bg->setGeometry (MPOSX, 300, 160, 160);
    QBoxLayout *slay = new QVBoxLayout (bg, 10);

    slay->addSpacing(10);

    slay->addWidget(new QLabel(i18n("Highest score:"), bg));

    hs = new QLabel (highest, bg);
    hs->setAlignment(Qt::AlignRight);
    hs->setFont(QFont("Helvetica", 18, QFont::Bold));
    slay->addWidget(hs);

    slay->addSpacing(10);

    slay->addWidget(new QLabel(i18n("Your score so far:"), bg));

    ys = new QLabel (current, bg);
    ys->setAlignment(Qt::AlignRight);
    ys->setFont(QFont("Helvetica", 18, QFont::Bold));
    slay->addWidget(ys);

    updateLevel(1);

    KConfig *config = KGlobal::config();
    config->setGroup("Options");
    settings.anim_speed = config->readNumEntry("Animation Speed", 1);
    if (settings.anim_speed < 1 || settings.anim_speed > MAX_SPEED)
	settings.anim_speed = 1;

    config->setGroup("level1");
    highest = config->readEntry("Moves0", "100");

    config->setGroup("Colors");

    settings.changed = false;

    setMinimumSize(665, 471);
}

GameWidget::~GameWidget()
{
}

void GameWidget::showHighscores ()
{
    KScoreDialog high(KScoreDialog::Name | KScoreDialog::Score, this);
    high.setCaption(i18n("Level %1 High Scores").arg(level));
    high.setConfigGroup(QString("High Scores Level %1").arg(level));
    high.exec();
}

#include "gamewidget.moc"
