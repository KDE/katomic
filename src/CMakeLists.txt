ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX KATOMIC
    VERSION_HEADER katomic_version.h
)

add_executable(katomic)

target_sources(katomic PRIVATE
    atom.h
    chooselevelsetdialog.cpp
    chooselevelsetdialog.h
    commondefs.h
    fielditem.cpp
    fielditem.h
    gamewidget.cpp
    gamewidget.h
    highscores.cpp
    highscores.h
    levelset.cpp
    levelsetdelegate.cpp
    levelsetdelegate.h
    levelset.h
    main.cpp
    molecule.cpp
    molecule.h
    playfield.cpp
    playfield.h
    toplevel.cpp
    toplevel.h

    katomic.qrc
)

ecm_qt_declare_logging_category(katomic
    HEADER katomic_debug.h
    IDENTIFIER KATOMIC_LOG
    CATEGORY_NAME katomic
    OLD_CATEGORY_NAMES org.kde.kdegames.katomic
    DESCRIPTION "KAtomic game"
    EXPORT KATOMIC
)

kconfig_add_kcfg_files(katomic prefs.kcfgc)

ki18n_wrap_ui(katomic levelsetwidget.ui)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-katomic.png")
ecm_add_app_icon(katomic ICONS ${ICONS_SRCS})

if(APPLE)
    set_target_properties(katomic PROPERTIES
        MACOSX_BUNDLE_DISPLAY_NAME "Katomic"
        MACOSX_BUNDLE_BUNDLE_NAME "Katomic"
        MACOSX_BUNDLE_LONG_VERSION_STRING "Katomic ${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.katomic"
        MACOSX_BUNDLE_COPYRIGHT "2024 The Katomic Developers")
endif()

target_link_libraries(katomic
    KDEGames6
    KF6::NewStuffWidgets
    KF6::I18n
    KF6::DBusAddons
    KF6::Crash
    KF6::XmlGui
    KF6::IconThemes
)

install(TARGETS katomic  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS org.kde.katomic.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.katomic.appdata.xml DESTINATION  ${KDE_INSTALL_METAINFODIR})
install(FILES katomic.knsrc  DESTINATION  ${KDE_INSTALL_KNSRCDIR})

ecm_qt_install_logging_categories(
    EXPORT KATOMIC
    FILE katomic.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
