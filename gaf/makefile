CC=bcc
MODEL=l
INC=..\include
DEBUG=-v
OPTIM=
CFLAGS=$(DEBUG) $(OPTIM) -ls -m$(MODEL) -I$(INC) -DUSE_BGI
LFLAGS=$(DEBUG) $(OPTIM) -ls -m$(MODEL) -L../lib
LIB=gaf$(MODEL).lib
#PRE=rse
#SUF= | tee /a errs

lib: $(LIB)
	copy $(LIB) ..\lib

all: $(LIB) paint.exe game.exe

paint.exe: paint.obj $(LIB)
	$(PRE) $(CC) $(LFLAGS) paint.obj $(LIB) graphics.lib bgif.lib $(SUF)

morph.exe: morph.obj $(LIB)
	$(PRE) $(CC) $(LFLAGS) morph.obj $(LIB) graphics.lib bgif.lib $(SUF)

game.exe: game.obj $(LIB)
	$(PRE) $(CC) $(LFLAGS) game.obj $(LIB) graphics.lib $(SUF)

paint.obj: paint.cpp $(INC)\app.h $(INC)\mouse.h
	$(PRE) $(CC) $(CFLAGS) -c paint.cpp $(SUF)

morph.obj: morph.cpp $(INC)\app.h $(INC)\mouse.h
	$(PRE) $(CC) $(CFLAGS) -c morph.cpp $(SUF)

game.obj: game.cpp $(INC)\app.h $(INC)\mouse.h
	$(PRE) $(CC) $(CFLAGS) -c game.cpp $(SUF)

$(LIB): app.obj window.obj mouse.obj
	tlib $(LIB) +-app.obj+-window.obj+-mouse.obj

app.obj: app.cpp $(INC)\mouse.h $(INC)\app.h
	$(PRE) $(CC) $(CFLAGS) -c app.cpp $(SUF)

window.obj: window.cpp $(INC)\window.h $(INC)\mouse.h
	$(PRE) $(CC) $(CFLAGS) -c window.cpp $(SUF)

mouse.obj: mouse.cpp $(INC)\mouse.h
	$(PRE) $(CC) $(CFLAGS) -c mouse.cpp $(SUF)

$(INC)\app.h: $(INC)\event.h $(INC)\window.h
	touch $(INC)\app.h

$(INC)\mouse.h: $(INC)\event.h
	touch $(INC)\mouse.h

$(INC)\window.h: $(INC)\event.h
	touch $(INC)\window.h

