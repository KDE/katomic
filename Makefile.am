
bin_PROGRAMS = katomic
INCLUDES = -I$(top_srcdir)/libkdegames $(all_includes)

katomic_SOURCES = feld.cpp molek.cpp main.cpp toplevel.cpp levelnames.cpp configbox.cpp gamewidget.cpp
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

levelnames:
	cd $(srcdir); \
	  rm -f levelnames.cpp; \
	  echo "#if 0" >> levelnames.cpp ;\
	  for i in levels/level_*; do \
		egrep '^Name=' $$i | sed -e 's#^Name=\(.*\)$$#i18n(\"\1\")#' >> levelnames.cpp ;\
	  done ;\
	  echo "#endif" >> levelnames.cpp

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/katomic.pot

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/katomic
	-rm -rf $(DESTDIR)$(kde_datadir)/katomic/levels
	mkdir $(DESTDIR)$(kde_datadir)/katomic/levels	
	cp $(srcdir)/levels/level_* $(DESTDIR)$(kde_datadir)/katomic/levels/
	rm -f $(kde_appsdir)/Games/katomic.desktop
