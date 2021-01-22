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
   chooselevelsetdialog.cpp
)

ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX KATOMIC
    VERSION_HEADER katomic_version.h
)

ecm_qt_declare_logging_category(katomic_SRCS
    HEADER katomic_debug.h
    IDENTIFIER KATOMIC_LOG
    CATEGORY_NAME org.kde.kdegames.katomic
    DESCRIPTION "katomic (kdegames)"
    EXPORT KATOMIC
)

kconfig_add_kcfg_files(katomic_SRCS prefs.kcfgc)

ki18n_wrap_ui(katomic_SRCS levelsetwidget.ui)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-katomic.png")
ecm_add_app_icon(katomic_SRCS ICONS ${ICONS_SRCS})

add_executable(katomic ${katomic_SRCS})

target_link_libraries(katomic
    KF5::NewStuff
    KF5KDEGames
    KF5::I18n
    KF5::DBusAddons
    KF5::Crash
    KF5::NewStuff
)

install(TARGETS katomic  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.katomic.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.katomic.appdata.xml DESTINATION  ${KDE_INSTALL_METAINFODIR})
install(FILES katomicui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/katomic)
# default theme is installed without .desktop file since KgTheme does not need it
# when only one theme is available. See: 30a1cadd20
install(FILES default_theme.svgz  DESTINATION  ${KDE_INSTALL_DATADIR}/katomic/pics/)
install(FILES katomic.knsrc  DESTINATION  ${KDE_INSTALL_KNSRCDIR})

if(NOT WIN32)
  install(PROGRAMS katomic-levelset-upd.pl DESTINATION  ${KDE_INSTALL_KCONFUPDATEDIR})
  install(FILES katomic-levelset.upd DESTINATION  ${KDE_INSTALL_KCONFUPDATEDIR})
endif(NOT WIN32)

ecm_qt_install_logging_categories(
    EXPORT KATOMIC
    FILE katomic.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
