#include <stdlib.h>
#include <stdio.h>
#include <graphics.h>
#include <assert.h>
#include "mouse.h"
#include "app.h"

application_t *Application;

void application_t::Init()
{
	int gmode, gdriver=DETECT, errorcode;
 	//(void)installuserdriver("SVGA256",TrueEnuf);
 	errorcode = registerfarbgidriver(EGAVGA_driver_far);
 	//errorcode = registerfarbgidriver(CGA_driver_far);
	initgraph(&gdriver, &gmode, "");
	if ((errorcode=graphresult()) != grOk)
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		exit(1);
	}
	Application = this;
}

application_t::application_t(color_t erasecolor)
{
	Init();
	if (mouse.Init() == 0)
	{
		closegraph();
		fprintf(stderr, "Mouse required.\n");
		exit(1);
	}
	mouse.Hide();
	setfillstyle(SolidFill, erasecolor);
	bar(0, 0, getmaxx(), getmaxy());
	mouse.Show();
	ev = NULL;
}

application_t::~application_t()
{
	closegraph();
}

int application_t::GotEvent()
{
	return (ev!=NULL || mouse.HasEvent());
}

event_t *application_t::GetEvent()
{
	if (ev) return ev;
	for (;;)
	{
		if (mouse.HasEvent())
			return ev = mouse.GetEvent();
	}
}

int application_t::HandleEvent()
{
	assert(ev);
	// avoid recursive popup invocations by NULLing ev before
	// dispatch
	event_t *et = ev;
	ev = NULL;
	int rtn = WindowManager.DispatchEvent(et);
	delete et;
	return rtn;
}

