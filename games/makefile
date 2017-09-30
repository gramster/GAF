DIR=\games
CC=bcc
MODEL=l
INC=\gram\libs\glib\include
CFLAGS=-v -ls -DDEBUG -O -I$(INC) -m$(MODEL) -DDRAWTREE
LFLAGS=-v -ls -m$(MODEL)
XXXOUT=
HEXOUT=
DGOUT=
OVOUT=
GOOUT=
OTOUT=
GAFDIR=..\gaf
GAMDIR=..\gamelib
GAFLIB=$(GAFDIR)\gaf$(MODEL).lib
GAMLIB=$(GAMDIR)\game$(MODEL).lib
LOCLIBS=debug.obj $(GAFLIB) $(GAMLIB)
LIBS=$(LOCLIBS) graphics.lib

all: libs go.exe xxx.exe hex.exe dodgem.exe ovid.exe othello.exe

go: libs go.exe

xxx: libs xxx.exe
hex: libs hex.exe
ovid: libs ovid.exe
dodgem: libs dodgem.exe
othello: libs othello.exe

zip:
	pkzip -upr xxx *.cpp *.h to* read* makef* *.tex

ovid.exe: ovid.obj gafgame.obj $(LOCLIBS)
	$(CC) $(LFLAGS) $(OVOUT) ovid.obj gafgame.obj $(LIBS)

xxx.exe: xxx.obj gafgame.obj $(LOCLIBS)
	$(CC) $(LFLAGS) $(XXXOUT) xxx.obj gafgame.obj $(LIBS)

go.exe: go.obj gafgame.obj $(LOCLIBS)
	$(CC) $(LFLAGS) $(GOOUT) go.obj gafgame.obj $(LIBS)

othello.exe: othello.obj gafgame.obj $(LOCLIBS)
	$(CC) $(LFLAGS) $(OTOUT) othello.obj gafgame.obj $(LIBS)

dodgem.exe: dodgem.obj gafgame.obj $(LOCLIBS)
	$(CC) $(LFLAGS) $(DGOUT) dodgem.obj gafgame.obj $(LIBS)

hex.exe: hex.obj gafgame.obj $(LOCLIBS)
	$(CC) $(LFLAGS) $(HEXOUT) hex.obj gafgame.obj $(LIBS)

xxxok.exe: xxxok.cpp
	$(CC) xxxok.cpp

ovid.obj: ovid.cpp $(INC)\movegame.h 
	$(CC) -c $(CFLAGS) ovid.cpp

xxx.obj: xxx.cpp $(INC)\game.h $(INC)\movegame.h
	$(CC) -c $(CFLAGS) xxx.cpp

go.obj: go.cpp $(INC)\game.h $(INC)\movegame.h $(INC)\app.h
	$(CC) -c $(CFLAGS) go.cpp

othello.obj: othello.cpp $(INC)\movegame.h $(INC)\app.h
	$(CC) -c $(CFLAGS) othello.cpp

hex.obj: hex.cpp $(INC)\game.h $(INC)\movegame.h
	$(CC) -c $(CFLAGS) hex.cpp

dodgem.obj: dodgem.cpp $(INC)\game.h $(INC)\gridgame.h
	$(CC) -c $(CFLAGS) dodgem.cpp

gafgame.obj: gafgame.cpp $(INC)\app.h $(INC)\mouse.h $(INC)\movegame.h
	$(CC) -c $(CFLAGS) gafgame.cpp

debug.obj: debug.cpp $(INC)\debug.h
	$(CC) -c $(CFLAGS) debug.cpp

libs:
	(cd $(GAFDIR)^make lib^cd $(DIR)^cd $(GAMDIR)^make lib^cd $(DIR) )
