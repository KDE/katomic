
bin_PROGRAMS = katomic
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KDEUI)

SUBDIRS = . levels

katomic_SOURCES = feld.cpp highscore.cpp molek.cpp mywidget.cpp

data_DATA = highscore.dat
datadir = $(kde_datadir)/katomic

pics_DATA = abilder.png molek.png
picsdir = $(datadir)/pics

METASOURCES = AUTO

