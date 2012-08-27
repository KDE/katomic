project(katomic)

if(NOT COMPILING_KDEGAMES)
	find_package(KDE4 REQUIRED)
	include(KDE4Defaults)
	include(MacroLibrary)
	find_package(KDEGames 4.9.0 REQUIRED)

	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	include_directories(${QDBUS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${KDEGAMES_INCLUDE_DIRS})
endif(NOT COMPILING_KDEGAMES)

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

kde4_add_kcfg_files(katomic_SRCS prefs.kcfgc)
kde4_add_ui_files(katomic_SRCS levelsetwidget.ui)

kde4_add_app_icon(katomic_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-katomic.png")
kde4_add_executable(katomic ${katomic_SRCS})

target_link_libraries(katomic  ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KNEWSTUFF3_LIBS} kdegames )

install(TARGETS katomic  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS katomic.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES katomicui.rc  DESTINATION  ${DATA_INSTALL_DIR}/katomic )
install( FILES default_theme.svgz  DESTINATION  ${DATA_INSTALL_DIR}/katomic/pics/ )
# ^^^ default_theme.desktop is not installed at the moment
install( FILES katomic.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

if(NOT WIN32)
  install( PROGRAMS katomic-levelset-upd.pl DESTINATION  ${KCONF_UPDATE_INSTALL_DIR} )
  install( FILES katomic-levelset.upd DESTINATION  ${KCONF_UPDATE_INSTALL_DIR} )
endif(NOT WIN32)
