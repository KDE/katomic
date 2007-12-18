project(katomic)

add_subdirectory( levels )

########### next target ###############

set(katomic_SRCS 
   katomicrenderer.cpp
   highscores.cpp
   playfield.cpp
   fielditem.cpp
   molecule.cpp 
   main.cpp 
   toplevel.cpp 
   gamewidget.cpp )

kde4_add_kcfg_files(katomic_SRCS prefs.kcfgc)

set(katomic_icons
	hi16-app-katomic.png
	hi32-app-katomic.png
	hi48-app-katomic.png
	hi64-app-katomic.png
	hi128-app-katomic.png
)
kde4_add_win32_app_icon(katomic_SRCS ${katomic_icons})
kde4_add_executable(katomic ${katomic_SRCS})

target_link_libraries(katomic  ${KDE4_KDEUI_LIBS} kdegames )

install(TARGETS katomic  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES katomic.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES katomicui.rc  DESTINATION  ${DATA_INSTALL_DIR}/katomic )
install( FILES default_theme.svgz  DESTINATION  ${DATA_INSTALL_DIR}/katomic/pics/ )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

