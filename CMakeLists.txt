project(katomic)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.3.0")

find_package(ECM 1.7.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets)
find_package(KF5 REQUIRED COMPONENTS
    CoreAddons
    Config
    WidgetsAddons
    I18n
    XmlGui
    NewStuff
    DocTools
    DBusAddons)

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(ECMAddAppIcon)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)


add_subdirectory(levels)
add_subdirectory(doc)

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
   chooselevelsetdialog.cpp)

kconfig_add_kcfg_files(katomic_SRCS prefs.kcfgc)
ki18n_wrap_ui(katomic_SRCS levelsetwidget.ui)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-katomic.png")
ecm_add_app_icon(katomic_SRCS ICONS ${ICONS_SRCS})
add_executable(katomic ${katomic_SRCS})

target_link_libraries(katomic  KF5::NewStuff KF5KDEGames KF5::I18n KF5::DBusAddons KF5::NewStuff)

install(TARGETS katomic  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install(PROGRAMS org.kde.katomic.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES katomic.appdata.xml DESTINATION  ${KDE_INSTALL_DATADIR}/appdata)
install(FILES katomicui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/katomic)
# default theme is installed without .desktop file since KgTheme does not need it
# when only one theme is available. See: 30a1cadd20
install(FILES default_theme.svgz  DESTINATION  ${KDE_INSTALL_DATADIR}/katomic/pics/)
install(FILES katomic.knsrc  DESTINATION  ${KDE_INSTALL_CONFDIR})

ecm_install_icons(ICONS 128-apps-katomic.png 16-apps-katomic.png 22-apps-katomic.png 32-apps-katomic.png 48-apps-katomic.png 64-apps-katomic.png DESTINATION ${KDE_INSTALL_ICONDIR} THEME hicolor)

if(NOT WIN32)
  install(PROGRAMS katomic-levelset-upd.pl DESTINATION  ${KDE_INSTALL_KCONFUPDATEDIR})
  install(FILES katomic-levelset.upd DESTINATION  ${KDE_INSTALL_KCONFUPDATEDIR})
endif(NOT WIN32)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
