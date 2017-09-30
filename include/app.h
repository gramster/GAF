#ifndef __APP_H
#define __APP_H

#include "debug.h"
#include "window.h"
#include "event.h"

class application_t MBUG1
{
	event_t *ev; // next event to handle
	void Init();
public:
	application_t(color_t erasecolor = Black);
	virtual int Execute() = 0;
	virtual ~application_t();
	int GotEvent();
	event_t *GetEvent();
	int HandleEvent();
};

extern application_t *Application;

#endif

