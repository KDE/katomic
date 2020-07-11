cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "20")
set(RELEASE_SERVICE_VERSION_MINOR "07")
set(RELEASE_SERVICE_VERSION_MICRO "80")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(katomic VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "5.7.0")
set (KF5_MIN_VERSION "5.30.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Config
    Crash
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
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)

if (${KF5Config_VERSION} STRGREATER "5.56.0")
        add_definitions(-DQT_NO_FOREACH)
        MESSAGE(STATUS "compile without foreach")
endif()


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

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KATOMIC VERSION_HEADER katomic_version.h)
ecm_qt_declare_logging_category(katomic_SRCS HEADER katomic_debug.h IDENTIFIER KATOMIC_LOG CATEGORY_NAME org.kde.kdegames.katomic)
kconfig_add_kcfg_files(katomic_SRCS prefs.kcfgc)
ki18n_wrap_ui(katomic_SRCS levelsetwidget.ui)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-katomic.png")
ecm_add_app_icon(katomic_SRCS ICONS ${ICONS_SRCS})
add_executable(katomic ${katomic_SRCS})

target_link_libraries(katomic
    KF5::NewStuff
    KF5KDEGames
    KF5::I18n
    KF5::DBusAddons
    KF5::Crash
    KF5::NewStuff)

install(TARGETS katomic  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install(PROGRAMS org.kde.katomic.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.katomic.appdata.xml DESTINATION  ${KDE_INSTALL_METAINFODIR})
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
install( FILES katomic.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR} )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
