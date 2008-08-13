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

kde4_add_app_icon(katomic_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-katomic.png")
kde4_add_executable(katomic ${katomic_SRCS})

target_link_libraries(katomic  ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS} kdegames )

install(TARGETS katomic  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES katomic.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES katomicui.rc  DESTINATION  ${DATA_INSTALL_DIR}/katomic )
install( FILES default_theme.svgz  DESTINATION  ${DATA_INSTALL_DIR}/katomic/pics/ )

kde4_install_icons( ${ICON_INSTALL_DIR}   )


