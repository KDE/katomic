####################################
#  Makefile for Atomeo V. 1.53     #
#  Version mit virtuellen Screens  #         
####################################

CC = gcc 
INCDIR = /usr/X11R6/lib/qt/include
CFLAGS = -O2 -m486 -g -fno-strength-reduce -Wall -W -I/usr/X11R6/include -L/opt/kde/lib -I/opt/kde/include
LFLAGS = -lqt -lkfm -lkdeui -lkdecore -lX11 -lXext -g
OBJS = mywidget.o feld.o highscore.o molek.o
TARGET = Atomic

PROG : $(OBJS)
	@echo "Linking ........"
	$(CC) -o $(TARGET) $(LFLAGS) $(OBJS) $(LIBS) $(CFLAGS) 
	strip $(TARGET) --strip-all
#	cp $(TARGET) /opt/kde/bin/$(TARGET)

mywidget.o : mywidget.cpp 
	@echo "Compiling ......"
	moc mywidget.h > mywidget.moc
#	moc feld.h > feld.moc
#	moc next.h > next.moc
#	moc highscore.h > highscore.moc
	$(CC) -c $(CFLAGS) mywidget.cpp -I$(INCDIR)


feld.o : feld.cpp
	moc feld.h > feld.moc
	$(CC) -c $(CFLAGS) feld.cpp -I$(INCDIR)

highscore.o : highscore.cpp
	moc highscore.h > highscore.moc
	$(CC) -c $(CFLAGS) highscore.cpp -I$(INCDIR)

molek.o : molek.cpp
	moc molek.h > molek.moc
	$(CC) -c $(CFLAGS) molek.cpp -I$(INCDIR)

