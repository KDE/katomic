/* configbox.h
 *
 * Copyright 1999 Cristian Tibirna (tibirna@kde.org)
 *
 */

#ifndef CONFIGBOX_H
#define CONFIGBOX_H

#include <kdialog.h>

class QSlider;
class QLCDNumber;

class ConfigBox : public KDialog
{
  Q_OBJECT

public:
  ConfigBox ( QWidget * );
  ~ConfigBox();
  
protected slots:
  virtual void slotButtonClicked(int);

signals:
  void speedChanged();

private:
  QSlider *speed;
  QLCDNumber *disp;
};

#endif		
