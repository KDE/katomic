/* configbox.cpp
 *
 * 1999 (c) Cristian Tibirna (tibirna@kde.org)
 *
 */

#include "configbox.h"
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qslider.h>

#include "settings.h"

extern Options settings;

ConfigBox::ConfigBox ( QWidget *parent, const char *name)
    : QDialog ( parent, name, TRUE )
{
  setCaption(i18n("Options"));

  QGridLayout *lay = new QGridLayout (this, 1, 1, 10);

  QGroupBox *gb = new QGroupBox(i18n("General"), this);
  lay->addMultiCellWidget(gb, 0, 0, 0, 2);

  QGridLayout *glay = new QGridLayout (gb, 1, 1, 10);
 
  glay->addWidget(new QLabel(i18n("Animation Speed"),gb), 1, 0);

  disp = new QLCDNumber(gb);
  glay->addWidget(disp,0, 2);
  
  speed = new QSlider(1, 10, 1, 1, QSlider::Horizontal, gb);
  glay->addMultiCellWidget(speed, 1, 1, 1, 3);

  connect(speed, SIGNAL(valueChanged(int)), disp, SLOT(display(int)));

  speed->setValue(settings.anim_speed);

  ok = new QPushButton(i18n("OK"), this);
  lay->addWidget(ok, 1, 1);

  connect(ok, SIGNAL(clicked()), this, SLOT(quitConfig()) );

  cancel = new QPushButton(i18n("Cancel"), this);
  lay->addWidget(cancel, 1, 2);

  connect(cancel, SIGNAL(clicked()), this, SLOT(reject()) );

}
  
void ConfigBox::quitConfig()
{
  settings.anim_speed = speed->value();
  settings.changed = true;

  emit speedChanged();

  accept();
}

ConfigBox::~ConfigBox()
{
}

#include "configbox.moc" 
