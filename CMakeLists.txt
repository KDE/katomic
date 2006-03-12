kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/highscore ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


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

target_link_libraries(katomic  ${LIB_KDEUI} kdegames )

install_targets(/bin katomic )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES katomic.desktop )
install_files( ${DATA_INSTALL_DIR}/katomic FILES katomicui.rc )
install_files( ${DATA_INSTALL_DIR}/katomic/pics/ FILES abilder.png molek.png )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



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
