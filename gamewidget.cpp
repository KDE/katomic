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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  */

#include "settings.h"
#include "gamewidget.h"
#include "feld.h"
#include "molek.h"

#include <qscrollbar.h>
#include <qgroupbox.h>
#include <qlayout.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kglobal.h>
#include <ksimpleconfig.h>
#include <qvbox.h>

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

    Highscore high(this, "highscore", level, moves);
    high.exec ();
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
    QString level = locate("appdata", QString("levels/level_%1").arg(l));
    if (level.isNull()) {
	return updateLevel(1);
    }

    KSimpleConfig cfg(level, true);
    cfg.setGroup("Level");
    feld->load(cfg);

    KConfig *config = kapp->config();
    level = QString("level%1").arg(l);
    config->setGroup(level);
    highest = config->readEntry("Moves0", "100");

    hs->setText(highest);
    ys->setText("0");
    scrl->setValue(l);

    feld->repaint();
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
    Highscore *h = new Highscore (this, "highscore", level, -1);
    h->exec ();
    delete h;
}

#include "gamewidget.moc"
