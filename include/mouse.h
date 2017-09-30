#ifndef __MOUSE_H
#define __MOUSE_H

#include "debug.h"
#include "event.h"

typedef enum
{
	ArrowCursor,
	HourglassCursor
} mousecursor_t;

// graphic mode mouse

#define LEFT_BUTTON          	0    /* Use left button */

class mouse_t : public eventgen_t
{
private:
	int lastx, lasty;
	int leftstate, rightstate;
	int hidden;
	event_t *nextevent;

	void Service(int &req, int &arg1, int &arg2, int &arg3, int es=0);
	int  Reset();
	int  ButtonDown(int b);
	int  ButtonUp(int b);
	void Reveal();
	void Conceal();
public:
	mouse_t()
		: eventgen_t()
	{
		nextevent = NULL;
		leftstate = rightstate = lastx = lasty = 0;
		hidden = 1;
	}
	~mouse_t()
	{
		if (nextevent)
			delete nextevent;
	}
	void SetCursor(int hotx, int hoty, unsigned *masks);
	void SetCursor(mousecursor_t type);
	int  Init();
	void Position(int &x, int &y);
	void Show()
	{
		if (--hidden <= 0)
			Reveal();
	}
	void Hide()
	{
		if (!hidden)
			Conceal();
		hidden++;
	}
	void Flush();
	int	 HasEvent();
//	void Cursor(char *bitmap);
	event_t *GetEvent();
};

extern mouse_t mouse;

#endif

