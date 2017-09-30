// graphic mode mouse

#include <dos.h>
#include <graphics.h>
#include <assert.h>
#include "mouse.h"

// Mouse driver service request IDs

#define RESET_MOUSE          	0
#define SHOW_MOUSE           	1
#define HIDE_MOUSE           	2
#define GET_MOUSE_STATUS 	3
#define PRESSED         	5     /* Button presses */
#define RELEASED        	6     /* Button releases */
#define SETCURSOR		9

mouse_t mouse;

// Mouse cursors

unsigned arrowcursor[34] = // mask and hots spots x,y at end
{
	0x3fff,	0x1fff,	0xfff,	0x7ff,	0x3ff,	0x1ff,	0xff,	0x7f,
	0x3f,	0x1f,	0x1ff,	0x10ff,	0x30ff,	0xf87f,	0xf87f,	0xfc3f,
        0x0,	0x4000,	0x6000,	0x7000,	0x7800,	0x7c00,	0x7e00,	0x7f00,
	0x7f80,	0x7fc0,	0x7c00,	0x4600,	0x600,	0x300,	0x300,	0x180,
	1,	1
};

unsigned hourglasscursor[34] =
{
	0xc003,	0xc003,	0xc003,	0xc003,	0xc003,	0xe007,	0xf00f,	0xf81f,
	0xf81f,	0xf00f,	0xe007,	0xc003,	0xc003,	0xc003,	0xc003,	0xc003,
	0x0,	0x1ff8,	0x1008,	0x1008,	0x810,	0x5a0,	0x7e0,	0x3c0,
	0x240,	0x420,	0x990,	0xbd0,	0x13c8,	0x17e8,	0x1ff8,	0x0,
	8,	8

};

//--------------------------------------------------------------------

void mouse_t::Service(int &m1, int &m2, int &m3, int &m4, int es)
{
	union REGS inregs, outregs;
	struct SREGS sregs;

	inregs.x.ax = m1;   inregs.x.bx = m2;
	inregs.x.cx = m3;   inregs.x.dx = m4;
	if (es)
	{
		sregs.ds = _DS;
		sregs.es = es;
		int86x(0x33,&inregs,&outregs,&sregs);
	}
	else int86(0x33,&inregs,&outregs);
	m1 = outregs.x.ax;  m2 = outregs.x.bx;
	m3 = outregs.x.cx;  m4 = outregs.x.dx;
}

void mouse_t::SetCursor(int hotx, int hoty, unsigned *masks)
{
	int seg = FP_SEG(masks), off = FP_OFF(masks), svc = SETCURSOR;
	Service(svc, hotx, hoty, off, seg);
}

void mouse_t::SetCursor(mousecursor_t type)
{
	switch (type)
	{
		case ArrowCursor:
			SetCursor(arrowcursor[32], arrowcursor[33],
				arrowcursor);
			break;
		case HourglassCursor:
			SetCursor(hourglasscursor[32], hourglasscursor[33],
				hourglasscursor);
			break;
	}
}

int mouse_t::Reset()
{
	int m1 = RESET_MOUSE, m2, m3, m4;
	Service(m1, m2, m3, m4);
	return m1;
}

void mouse_t::Reveal()
{
	int m1 = SHOW_MOUSE, m2, m3, m4;
	Service(m1, m2, m3, m4);
	hidden = 0;
}

void mouse_t::Conceal()
{
	int m1 = HIDE_MOUSE, m2, m3, m4;
	Service(m1, m2, m3, m4);
}

int mouse_t::Init()
{
	if (Reset())
	{
		/* Hercules card requires an extra step */
		if (getgraphmode() == HERCMONOHI)
		{
			char far *memory = (char far *)0x004000049L;
			*memory = 0x06;
			(void)Reset();
		}
		Show();
		return 1;
	}
	else return 0; /* Mouse not found! */
}

void mouse_t::Position(int &x, int &y)
{
	int m1 = GET_MOUSE_STATUS, m2;
	Service(m1, m2, x, y);
	// Adjust for virtual coordinates of the mouse
	if (getmaxx() == 319) x /= 2;
}

int mouse_t::ButtonDown(int b)
{
	int m1 = PRESSED, m3, m4;
	Service(m1, b, m3, m4);
	if (b) return 1;
	else return 0;
}

int mouse_t::ButtonUp(int b)
{
	int m1 = RELEASED, m3, m4;
	Service(m1, b, m3, m4);
	if (b) return 1;
	else return 0;
}

int mouse_t:: HasEvent()
{
	int x, y;
	if (!nextevent)
	{
		Position(x, y);
		if (leftstate == 0 && ButtonDown(0))
		{
			nextevent = new event_t(ButtonPressed, 0);
			assert(nextevent);
			leftstate = 1;
		}
		else if (leftstate == 1 && ButtonUp(0))
		{
			nextevent = new event_t(ButtonReleased, 0);
			assert(nextevent);
			leftstate = 0;
		}
		else if (rightstate == 0 && ButtonDown(1))
		{
			nextevent = new event_t(ButtonPressed, 1);
			assert(nextevent);
			rightstate = 1;
		}
		else if (rightstate == 1 && ButtonUp(1))
		{
			nextevent = new event_t(ButtonReleased, 1);
			assert(nextevent);
			rightstate = 0;
		}
		else if (x != lastx || y != lasty)
		{
			nextevent = new event_t(MouseMoved, x, y);
			assert(nextevent);
			lastx = x;
			lasty = y;
		}
	}
	return nextevent ? 1 : 0;
}

event_t* mouse_t::GetEvent()
{
	assert(nextevent);
	event_t *rtn = nextevent;
	nextevent = NULL;
	return rtn;
}

void mouse_t::Flush()
{
	for (;;)
	{
		if (nextevent)
		{
			delete nextevent;
			nextevent = NULL;
		}
		if (HasEvent()==0) break;
	}
}

