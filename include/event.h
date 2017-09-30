#ifndef __EVENT_H
#define __EVENT_H

#include <stdlib.h> // for NULL
#include "debug.h"

// This should really be #defines to make it extensible by the user

typedef enum
{
	MouseMoved, ButtonPressed, ButtonReleased, KeyPressed, KeyReleased,
	GotFocus, LostFocus
} event_type_t;

class event_t MBUG1
{
	event_type_t type;
	int arg1, arg2;
public:
	event_t(event_type_t type_in, int arg1_in = 0, int arg2_in = 0)
	{
		type = type_in;
		arg1 = arg1_in;
		arg2 = arg2_in;
	}
	inline event_type_t Type()
	{
		return type;
	}
	inline int Arg1()
	{
		return arg1;
	}
	inline int Arg2()
	{
		return arg2;
	}
};

// event generators like mouse amd keyboard (linked list)

class eventgen_t MBUG1
{
	eventgen_t 	*next;
public:
	eventgen_t()
	{
		next = NULL;
	}
	virtual int HasEvent() = 0;
	virtual event_t *GetEvent() = 0;
};

class eventmanager_t MBUG1
{
	eventgen_t *firstgen;
public:
	void AddGenerator(eventgen_t *gen);
	event_t *GetEvent();
	int DispatchEvent(event_t *e);
};

#endif

