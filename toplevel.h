/* toplevel.h
 *
 * Andreas Wüst
 *
 */

#ifndef TOPLEVEL_H
#define TOPLEVEL_H

class GameWidget;
class KConfig;

#include <kmainwindow.h>

/**
 * This is the class AtomTopLevel. The class is used only for the program
 * AtomTopLevel.
 *
 * @short Basic class for AtomTopLevel
 * @author Andreas Wüst
 */

class AtomTopLevel : public KMainWindow
{
    Q_OBJECT

 public:

  AtomTopLevel();

  ~AtomTopLevel();

 protected:

  // Creates the menubar and connects the menu-entries to the
  // appropriate functions
  void createMenu();

  // Get the configuration from the config-file.
  void initConfig();

  // Save the current configuration to the config-file.
  void saveConfig();

  // called before exiting -> save configuration
  virtual bool queryExit();

  KConfig *config;

  GameWidget *main;

public slots:

  // Shows a window for configuring the keybindings
  void configkeys();

  // Shows a dialog for options other than keys
  void configopts();

};

#endif
