/* configbox.h
 *
 * 1999 (c) Cristian Tibirna (tibirna@kde.org)
 *
 */

#ifndef CONFIGBOX_H
#define CONFIGBOX_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qslider.h>
#include <qlcdnumber.h>

class ConfigBox : public QDialog
{
  Q_OBJECT

public:
  ConfigBox ( QWidget *, const char* name );
  ~ConfigBox();
  
protected slots:
  void quitConfig();
  void quit();

signals:
  void speedChanged();

private:
  QSlider *speed;
  QLCDNumber *disp;
  QPushButton *ok, *cancel;
};

#endif		
