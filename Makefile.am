
bin_PROGRAMS = katomic
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KDEUI)

SUBDIRS = . levels

katomic_SOURCES = feld.cpp highscore.cpp molek.cpp mywidget.cpp levelnames.cpp

data_DATA = highscore.dat
datadir = $(kde_datadir)/katomic

pics_DATA = abilder.png molek.png
picsdir = $(datadir)/pics

METASOURCES = AUTO

levelnames:
	cd $(srcdir); \
	  rm -f levelnames.cpp; \
	  echo "#if 0" >> levelnames.cpp ;\
	  for i in levels/level_*; do \
		egrep '^Name=' $$i | sed -e 's#^Name=\(.*\)$$#i18n(\"\1\")#' >> levelnames.cpp ;\
	  done ;\
	  echo "#endif" >> levelnames.cpp

messages:
	$(XGETTEXT) -ki18n -x $(includedir)/kde.pot *.cpp && mv messages.po ../po/katomic.pot


