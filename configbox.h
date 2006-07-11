/* configbox.h
 *
 * Copyright 1999 Cristian Tibirna (tibirna@kde.org)
 *
 */

#ifndef CONFIGBOX_H
#define CONFIGBOX_H

#include <QLabel>
#include <qslider.h>
#include <qlcdnumber.h>

#include <kdialog.h>

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
