/* MyWidget.h
 *
 * Andreas Wüst
 *
 */

#ifndef MYWIDGET_H
#define MYWIDGET_H



#include <qpushbt.h>
#include <qpopmenu.h>
#include <qlabel.h>
#include <qmenubar.h>
#include <qevent.h>
#include <qmsgbox.h>
#include <qsize.h>
#include <qgrpbox.h>
#include <qbttngrp.h>

#include <qframe.h>
#include <qlcdnum.h>
#include <qfiledlg.h>

#include <kapp.h>
#include <kmenubar.h>
#include <kpopmenu.h>
#include <ktopwidget.h>
#include <kmsgbox.h>
#include <kconfig.h>
#include <kkeyconf.h>
#include <kstdaccel.h>
#include <klocale.h>
#include <kdebug.h>

/**
 * This is the class MyWidget. The class is used only for the program 
 * MyWidget.
 * 
 * @short Basic class for MyWidget
 * @author Andreas Wüst
 * @version 0.00
 * 
 */

class MyWidget : public KTopLevelWidget
{
	Q_OBJECT

public:
	
	// The constructor method for class KFortune
	//
	// @see MyWidget
	// @param parent Parent widget, used for QWidget
	// @param name	Name for QWidget
	 	 
	MyWidget ( QWidget *parent=0, const char *name=0 );
	
	// The destrucor method for class KFortune
	
	~MyWidget();
		
protected:
 

     
	// @see KKeyConfig
	void initKeys();

	// Creates the menubar and connects the menu-entries to the
	// appropriate functions
	void createMenu();

        // Get the configuration from the config-file.
	void initConfig();

	// Save the current configuration to the config-file.
	void saveConfig();


	KConfig *config;
	KStdAccel *accel;

	KMenuBar *menu;
	QPopupMenu *file, *options, *help;
       
        // stellt das spielfeld dar !
        Feld *feld;      
  
        // stellt molekül dar
        Molek *molek;

        // scorllbar zur levelwahl
        QScrollBar *scrl;

        // buttongroup
        QButtonGroup *bg;
 
  // buttons
  QPushButton *up, *down, *left, *right, *done;
	
  // enthält akt level
  int level;    


public slots:
  // bringt level auf neuesten stand 
  void updateLevel (int);

        // getbutton erhält button der gedrückt wurde
        void getButton (int);
  
        // showdir, zeigt richtungsfelder an, wenn feld ausgewählt
        void showDir ();

        // versteckt richtungsfelder, wenn feld wieder abgewählt
        void hideDir ();

        // Menupunkt Highscores im Pop-up Menu, der Highscore anzeigt
        void showHighscores ();

        // Quit the application
	void quitapp();

	// Shows a message telling the user that a function has
	// not been implemented yet.
	void notImpl();
	
	// Shows a window for configuring the keybindings
	void configkeys();

	// Shows information about the program
	void about();

	// Show the online-help
	void helpmenu();
};

#endif
