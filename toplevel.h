/* MyWidget.h
 *
 * Andreas W�st
 *
 */

#ifndef _TOPLEVEL_WIDGET_H
#define _TOPLEVEL_WIDGET_H

class GameWidget;
class KConfig;
class KAccel;

#include <ktmainwindow.h>

/**
 * This is the class AtomTopLevel. The class is used only for the program 
 * AtomTopLevel.
 * 
 * @short Basic class for AtomTopLevel
 * @author Andreas W�st
 * @version 0.00
 * 
 */

class AtomTopLevel : public KTMainWindow
{
  Q_OBJECT

    public:
	
  // The constructor method for class KFortune
  //
  // @see AtomTopLevel
  // @param parent Parent widget, used for QWidget
  // @param name	Name for QWidget
	 	 
  AtomTopLevel ( const char *name=0 );
	
  // The destrucor method for class KFortune
	
  ~AtomTopLevel();
		
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
  KAccel *accel;

  QPopupMenu *file, *options, *help;
       
  GameWidget *main;
  
public slots:

  // Quit the application
  void quitapp();

  // Shows a window for configuring the keybindings
  void configkeys();

  // Shows a dialog for options other than keys
  void configopts();

  // Shows information about the program
  void about();

  // Show the online-help
  void helpmenu();
};

#endif
