project(katomic)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")

find_package(ECM 1.3.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets )
find_package(KF5 REQUIRED COMPONENTS 
    CoreAddons 
    Config 
    WidgetsAddons 
    Config 
    I18n 
    GuiAddons 
    ConfigWidgets 
    ItemViews 
    IconThemes 
    XmlGui
    KIO
    NotifyConfig
    NewStuff
    )

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)


add_subdirectory( levels )
add_subdirectory( doc )

########### next target ###############

set(katomic_SRCS
   highscores.cpp
   playfield.cpp
   fielditem.cpp
   molecule.cpp
   main.cpp
   toplevel.cpp
   gamewidget.cpp
   levelset.cpp
   levelsetdelegate.cpp
   chooselevelsetdialog.cpp )

kconfig_add_kcfg_files(katomic_SRCS prefs.kcfgc)
ki18n_wrap_ui(katomic_SRCS levelsetwidget.ui)

#kde4_add_app_icon(katomic_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-katomic.png")
add_executable(katomic ${katomic_SRCS})

target_link_libraries(katomic  KF5::KIOCore  KF5::NewStuff KF5KDEGames KF5::I18n KF5::DBusAddons KF5::NewStuff)

install(TARGETS katomic  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.katomic.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES katomic.appdata.xml DESTINATION  ${DATA_INSTALL_DIR}/appdata )
install( FILES katomicui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/katomic )
install( FILES default_theme.svgz  DESTINATION  ${DATA_INSTALL_DIR}/katomic/pics/ )
# ^^^ default_theme.desktop is not installed at the moment
install( FILES katomic.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )

ecm_install_icons(ICONS hi128-apps-katomic.png  hi16-apps-katomic.png  hi22-apps-katomic.png  hi32-apps-katomic.png  hi48-apps-katomic.png  hi64-apps-katomic.png DESTINATION ${ICON_INSTALL_DIR}   )

if(NOT WIN32)
  install( PROGRAMS katomic-levelset-upd.pl DESTINATION  ${KCONF_UPDATE_INSTALL_DIR} )
  install( FILES katomic-levelset.upd DESTINATION  ${KCONF_UPDATE_INSTALL_DIR} )
endif(NOT WIN32)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
