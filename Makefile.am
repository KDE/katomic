
bin_PROGRAMS = katomic
INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)

katomic_SOURCES = feld.cpp molek.cpp main.cpp toplevel.cpp configbox.cpp gamewidget.cpp
katomic_LDFLAGS = $(all_libraries) $(KDE_RPATH)
katomic_LDADD = $(top_builddir)/libkdegames/libkdegames.la $(LIB_KDEUI)

EXTRA_DIST = katomic.xpm katomic.desktop

appdir = $(kde_datadir)/katomic
app_DATA = katomicui.rc

pics_DATA = abilder.png molek.png
picsdir = $(appdir)/pics

KDE_ICON = katomic

appsdir = $(kde_appsdir)/Games/TacticStrategy
apps_DATA = katomic.desktop

METASOURCES = AUTO

messages: rc.cpp
	: > levelnames.cpp ;\
	for i in levels/level_*; do \
	egrep '^Name=' $$i | sed -e 's#^Name=\(.*\)$$#i18n(\"\1\")#' >> levelnames.cpp ;\
        done 
	$(XGETTEXT) *.cpp -o $(podir)/katomic.pot
	rm -rf levelnames.cpp

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/katomic
	-rm -rf $(DESTDIR)$(kde_datadir)/katomic/levels
	mkdir $(DESTDIR)$(kde_datadir)/katomic/levels	
	cp $(srcdir)/levels/level_* $(DESTDIR)$(kde_datadir)/katomic/levels/
	rm -f $(DESTDIR)$(kde_appsdir)/Games/katomic.desktop
