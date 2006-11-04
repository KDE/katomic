/****************************************************************
**
** Definition of Molecule class,
**
****************************************************************/

#ifndef MOLECULE_H
#define MOLECULE_H

#include <QWidget>
#include <QPixmap>
#include <QList>
#include "atom.h"

class KSimpleConfig;

#define MOLECULE_SIZE 15

class KAtomicRenderer;

class Molecule : public QWidget
{
    Q_OBJECT

public:
    Molecule (QWidget *parent=0);
   ~Molecule ();

   void load(const KSimpleConfig& config);

   const atom& getAtom(int index) const;
   int atomSize() const { return atoms.count(); }

   QSize molecSize() const { return _size; }
   uint getAtom(int x, int y) const { return molek[x][y]; }
   
protected:
   void paintEvent( QPaintEvent * );

private:
   uint molek[MOLECULE_SIZE][MOLECULE_SIZE]; // the indexes within atoms
   QList<atom> atoms;
   QString mname;
   QSize _size;

   KAtomicRenderer* m_renderer;
   int m_elemSize;
};

#endif // MOLECULE_H
