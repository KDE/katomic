/* configbox.h
 *
 * 1999 (c) Cristian Tibirna (tibirna@kde.org)
 *
 */

#ifndef CONFIGBOX_H
#define CONFIGBOX_H

#include <qlabel.h>
#include <qslider.h>
#include <qlcdnumber.h>

#include <kdialogbase.h>

class ConfigBox : public KDialogBase
{
  Q_OBJECT

public:
  ConfigBox ( QWidget *, const char* name );
  ~ConfigBox();
  
protected slots:
  void slotOk();

signals:
  void speedChanged();

private:
  QSlider *speed;
  QLCDNumber *disp;
};

#endif		
