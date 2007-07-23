project(katomic)

add_subdirectory( levels )

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames )


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

kde4_add_executable(katomic ${katomic_SRCS})

target_link_libraries(katomic  ${KDE4_KDEUI_LIBS} kdegames )

install(TARGETS katomic  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES katomic.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES katomicui.rc  DESTINATION  ${DATA_INSTALL_DIR}/katomic )
install( FILES default_theme.svgz  DESTINATION  ${DATA_INSTALL_DIR}/katomic/pics/ )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

