/****************************************************************
**
** Definition of Molek class,
**
****************************************************************/

#ifndef MOLEK_H
#define MOLEK_H

#include <QWidget>
#include <QPixmap>
#include <QList>
#include "atom.h"

class KSimpleConfig;

#define MOLEK_SIZE 15

class KAtomicRenderer;

class Molek : public QWidget
{
    Q_OBJECT

public:
    Molek (QWidget *parent=0);
   ~Molek ();

   void load(const KSimpleConfig& config);

   const atom& getAtom(int index) const;
   int atomSize() const { return atoms.count(); }

   QSize molecSize() const { return _size; }
   uint getAtom(int x, int y) const { return molek[x][y]; }
   
protected:
   void paintEvent( QPaintEvent * );

private:
   uint molek[MOLEK_SIZE][MOLEK_SIZE]; // the indexes within atoms
   QList<atom> atoms;
   QString mname;
   QSize _size;

   KAtomicRenderer* m_renderer;
   int m_elemSize;
};

#endif // MOLEK_H
