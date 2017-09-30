#ifdef MEMDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "debug.h"

#define MAXALLOCS	1024
#define ARRSZ		(1024/32)

int debugID = 0;
unsigned long dbtbl[ARRSZ];

void InitDebug()
{
	for (int i = 0; i < ARRSZ; i++)
		dbtbl[i] = 0l;
	atexit(Report);
}

int NoteAlloc(void)
{
	assert(debugID < MAXALLOCS);
	dbtbl[debugID/32] |= 1l << (debugID%32);
	return debugID++;
}

void NoteDealloc(int ID)
{
	assert(ID>=0 && ID < MAXALLOCS);
//	assert(dbtbl[ID/32] & (1l << (ID%32)));
	dbtbl[ID/32] &= ~(1l << (ID%32));
}

void Report()
{
	for (int i = 0; i < MAXALLOCS; i++)
		if (dbtbl[i/32] & (1l << (i%32)))
			fprintf(stderr,"Allocation %d not freed\n",i);
}


#endif

