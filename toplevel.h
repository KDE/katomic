/* toplevel.h
 *
 * Andreas W�st
 *
 */

#ifndef TOPLEVEL_H
#define TOPLEVEL_H

class GameWidget;
class KAction;
class KConfig;

#include <kmainwindow.h>

/**
 * This is the class AtomTopLevel. The class is used only for the program
 * AtomTopLevel.
 *
 * @short Basic class for AtomTopLevel
 * @author Andreas W�st
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

  KAction *redoAction, *undoAction;

  protected slots:
  void enableRedo(bool enable);
  void enableUndo(bool enable);

public slots:

  // Shows a dialog for options other than keys
  void configopts();

};

#endif
