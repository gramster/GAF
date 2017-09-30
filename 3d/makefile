CC=bcc
MODEL=l
INC=..\include
LIBP=..\lib
DEBUG=-v
OPTIM=
CFLAGS=$(DEBUG) $(OPTIM) -ls -m$(MODEL) -I$(INC) 
LFLAGS=$(DEBUG) $(OPTIM) -ls -m$(MODEL) -L$(LIBP)
LIB=glib$(MODEL).lib
LIBS=$(LIB) gaf$(MODEL).lib bgif.lib
GAFH=$(INC)\mouse.h $(INC)\event.h $(INC)\app.h $(INC)\window.h

all: $(LIB) testwire.exe testobj.exe tstwire2.exe tstwire3.exe

zip:
	zip -u 3d testwire.cpp testobj.cpp glib.cpp glib.h tmatrix.cpp 
	zip -u 3d tmatrix.cpp tmatrix.h makefile tstwire2.cpp tstwire3.cpp

testwire.exe: testwire.obj $(LIB) $(LIBP)\gaf$(MODEL).lib $(LIBP)\bgif.lib
	$(CC) $(LFLAGS) testwire.obj $(LIBS) graphics.lib

tstwire2.exe: tstwire2.obj $(LIB) $(LIBP)\gaf$(MODEL).lib $(LIBP)\bgif.lib
	$(CC) $(LFLAGS) tstwire2.obj $(LIBS) graphics.lib

tstwire3.exe: tstwire3.obj $(LIB) $(LIBP)\gaf$(MODEL).lib $(LIBP)\bgif.lib
	$(CC) $(LFLAGS) tstwire3.obj $(LIBS) graphics.lib

testobj.exe: testobj.obj $(LIB) $(LIBP)\gaf$(MODEL).lib $(LIBP)\bgif.lib
	$(CC) $(LFLAGS) testobj.obj $(LIBS) graphics.lib

testwire.obj: testwire.cpp $(GAFH) glib.h
	$(CC) $(CFLAGS) -c testwire.cpp

tstwire2.obj: tstwire2.cpp $(GAFH) glib.h
	$(CC) $(CFLAGS) -c tstwire2.cpp

tstwire3.obj: tstwire3.cpp $(GAFH) glib.h
	$(CC) $(CFLAGS) -c tstwire3.cpp

testobj.obj: testobj.cpp $(GAFH) glib.h
	$(CC) $(CFLAGS) -c testobj.cpp

$(LIB): glib.obj tmatrix.obj
	tlib $(LIB) +-glib.obj+-tmatrix

glib.obj: glib.cpp $(GAFH) glib.h
	$(CC) $(CFLAGS) -c glib.cpp

tmatrix.obj: tmatrix.cpp tmatrix.h
	$(CC) $(CFLAGS) -c tmatrix.cpp

glib.h: tmatrix.h
	touch glib.h

