
INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)
katomic_LDFLAGS = $(all_libraries) $(KDE_RPATH)
katomic_LDADD = $(LIB_KDEGAMES)
METASOURCES = AUTO
katomic_la_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

bin_PROGRAMS = katomic
katomic_SOURCES = feld.cpp molek.cpp main.cpp toplevel.cpp configbox.cpp\
	gamewidget.cpp

pics_DATA = abilder.png molek.png
picsdir = $(appdir)/pics

xdg_apps_DATA = katomic.desktop

EXTRA_DIST = $(pics_DATA)
KDE_ICON = katomic

appdir = $(kde_datadir)/katomic
app_DATA = katomicui.rc

messages: rc.cpp
	: > levelnames.cpp ;\
	for i in levels/level_*; do \
	egrep '^Name=' $$i | sed -e 's#^Name=\(.*\)$$#i18n(\"\1\")#' >> levelnames.cpp ;\
        done
	$(XGETTEXT) *.cpp -o $(podir)/katomic.pot
	rm -f levelnames.cpp

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/katomic
	-rm -rf $(DESTDIR)$(kde_datadir)/katomic/levels
	mkdir $(DESTDIR)$(kde_datadir)/katomic/levels	
	cp $(srcdir)/levels/level_* $(DESTDIR)$(kde_datadir)/katomic/levels/
