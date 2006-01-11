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
#include <q3groupbox.h>
#include <qslider.h>
//Added by qt3to4:
#include <QLabel>
#include <QGridLayout>

#include "settings.h"

extern Options settings;

ConfigBox::ConfigBox ( QWidget *parent, const char *name)
    : KDialogBase ( parent, name, true, i18n("Configure"), Ok | Cancel, Ok, true )
{
  QWidget *page = makeMainWidget();  

  QGridLayout *glay = new QGridLayout (page, 4, 5, 0, spacingHint());
  glay->setRowStretch(0, 1);
  glay->setRowStretch(3, 1);
  glay->setColStretch(0, 1);
  glay->setColStretch(4, 1);
 
  glay->addWidget(new QLabel(i18n("Animation speed:"),page), 2, 1);

  disp = new QLCDNumber(page);
  glay->addWidget(disp, 1, 2);
  disp->display(1);

  speed = new QSlider(1, 10, 1, 1, Qt::Horizontal, page);
  glay->addMultiCellWidget(speed, 2, 2, 2, 3);

  connect(speed, SIGNAL(valueChanged(int)), disp, SLOT(display(int)));

  speed->setValue(settings.anim_speed);
  incInitialSize(QSize(20,20));
}
  
void ConfigBox::slotOk()
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
