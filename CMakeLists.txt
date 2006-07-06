project(katomic)

add_subdirectory( levels )

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/highscore )


########### next target ###############

set(katomic_SRCS 
   feld.cpp 
   molek.cpp 
   main.cpp 
   toplevel.cpp 
   configbox.cpp 
   gamewidget.cpp )

kde4_automoc(${katomic_SRCS})

kde4_add_executable(katomic ${katomic_SRCS})

target_link_libraries(katomic  ${KDE4_KDEUI_LIBS} kdegames )

install(TARGETS katomic  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES katomic.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES katomicui.rc  DESTINATION  ${DATA_INSTALL_DIR}/katomic )
install( FILES abilder.png molek.png  DESTINATION  ${DATA_INSTALL_DIR}/katomic/pics/ )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )




#original Makefile.am contents follow:

#
#INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)
#katomic_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#katomic_LDADD = $(LIB_KDEGAMES)
#katomic_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#METASOURCES = AUTO
#
#bin_PROGRAMS = katomic
#katomic_SOURCES = feld.cpp molek.cpp main.cpp toplevel.cpp configbox.cpp\
#	gamewidget.cpp
#
#pics_DATA = abilder.png molek.png
#picsdir = $(appdir)/pics
#
#xdg_apps_DATA = katomic.desktop
#
#EXTRA_DIST = $(pics_DATA)
#KDE_ICON = katomic
#
#appdir = $(kde_datadir)/katomic
#app_DATA = katomicui.rc
#
#messages: rc.cpp
#	: > levelnames.cpp ;\
#	for i in levels/level_*; do \
#	egrep '^Name=' $$i | sed -e 's#^Name=\(.*\)$$#i18n(\"\1\")#' >> levelnames.cpp ;\
#        done
#	$(XGETTEXT) *.cpp -o $(podir)/katomic.pot
#	rm -f levelnames.cpp
#
#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/katomic
#	-rm -rf $(DESTDIR)$(kde_datadir)/katomic/levels
#	mkdir $(DESTDIR)$(kde_datadir)/katomic/levels	
#	cp $(srcdir)/levels/level_* $(DESTDIR)$(kde_datadir)/katomic/levels/
