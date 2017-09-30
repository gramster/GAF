#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef MEMDEBUG
#define MBUG1	: public memdebug_t
#define MBUG2 , public memdebug_t
#define INITDEBUG	InitDebug();
#else
#define MBUG1
#define MBUG2
#define INITDEBUG
#endif

#ifdef MEMDEBUG

extern void InitDebug(void);
extern int NoteAlloc(void);
extern void NoteDealloc(int ID);
extern void Report();

class memdebug_t
{
	int ID;
public:
	memdebug_t()
	{
		ID = NoteAlloc();
	}
	~memdebug_t()
	{
		NoteDealloc(ID);
	}
};

#endif
#endif

