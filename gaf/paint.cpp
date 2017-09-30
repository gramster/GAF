#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include <assert.h>
#include "app.h"
#include "mouse.h"

int Quit(void *arg);
#if 0

int Quit(void *arg)
{
	(void)arg;
	return 0;
}
#endif

typedef enum
{
	Initial, Penciling, Painting, Erasing, Lines, Circles
} mode_t;

int Mode(void *arg);

class paintcanvas_t : public canvas_t
{
	mode_t mode;
	int down, startx, starty, oldx, oldy;
public:
	paintcanvas_t(int left_in, int top_in, int right_in, int bottom_in,
		unsigned long flags_in = 0l, color_t fillcolor_in = Black)
		: canvas_t(left_in, top_in, right_in, bottom_in, 
				flags_in, fillcolor_in)
	{
		mode = Initial;
		down = 0;
	}
	int HandleEvent(event_t *e);
	void Mode(mode_t mode_in)
	{
		mode = mode_in;
	}
};

int paintcanvas_t::HandleEvent(event_t *e)
{
	int x, y; mouse.Position(x, y); x -= Left(); y -= Top();
	PaintMode(XOrPut);
	if (down && e->Type() == MouseMoved)
	{
		switch(mode)
		{
		case Penciling:
			break;
		case Painting:
			break;
		case Erasing:
			break;
		case Lines:
			Line(startx, starty, oldx, oldy);
			Line(startx, starty, oldx = x, oldy = y);
			break;
		case Circles:
			Circle(startx, starty, abs(oldx-startx));
			Circle(startx, starty, abs((oldx=x)-startx));
			break;
		}
	}
	else if (e->Type() == ButtonPressed && e->Arg1()==0)
	{
		if (!down)
		{
			down = 1;
			startx = oldx = x;
			starty = oldy = y;
			if (mode == Lines) Line(x, y, x, y);
		}
	}
	else if (e->Type() == ButtonReleased && e->Arg1()==0)
	{
		if (down)
		{
			down = 0;
			if (mode == Lines)
			{
				Line(startx, starty, oldx, oldy);
				PaintMode(CopyPut);
				Line(startx, starty, x, y);
			}
			else if (mode == Circles)
			{
				Circle(startx, starty, abs(oldx-startx));
				PaintMode(CopyPut);
				Circle(startx, starty, abs(x-startx));
			}
		}
	}
	return 1;
}

class paintapp_t : public application_t
{
	paintcanvas_t *canvas;
	menu_t *menu;
public:
	paintapp_t()
		: application_t(Blue)
	{
		int wl = textwidth("MenuWidth");
		canvas = new paintcanvas_t(wl+50, 1,
			getmaxx() - 1, (getmaxy()<=400) ? getmaxy()-1 : 400,
			W_BORDER);
		assert(canvas);
		canvas->Clear();
		menu = new menu_t(1, 1, 6);
		assert(menu);
		menu->AddEntry("Pencil", Mode, (void*)Penciling);
		menu->AddEntry("SprayCan",  Mode, (void*)Painting);
		menu->AddEntry("Erase",  Mode, (void*)Erasing);
		menu->AddEntry("Lines",  Mode, (void*)Lines);
		menu->AddEntry("Circle",  Mode, (void*)Circles);
		menu->AddEntry("Quit", Quit);
		menu->Foreground(Black);
		menu->Background(LightBlue);
	}
	int Execute();
	void SetMode(mode_t mode)
	{
		canvas->Mode(mode);
	}
	~paintapp_t()
	{
		assert(canvas && menu);
		delete menu;
		delete canvas;
	}
};

int paintapp_t::Execute()
{
	menu->Show();
	while (GetEvent())
		if (HandleEvent()==0)
			break;
	return 0;
}

paintapp_t *app;

int Cancel(void *arg)
{
	*((int *)arg) = 1;
	return 0;
}

int Confirm(void *arg)
{
	*((int *)arg) = 0;
	return 0;
}

int Quit(void *arg)
{
	int cancelled = 0;
	(void)arg;
	menu_t *m = new menu_t(60, 20, 2);
	assert(m);
	m->AddEntry("Confirm", Confirm, (void *)&cancelled);
	m->AddEntry("Cancel", Cancel, (void *)&cancelled);
	m->Foreground(Black);
	m->Background(Red);
	m->Show();
	WindowManager.SetFocus(m->Handle());
	while (app->GetEvent())
		if (app->HandleEvent()==0)
			break;
	WindowManager.ReleaseFocus(m->Handle());
	delete m;
	return cancelled;
}

int Mode(void *arg)
{
	app->SetMode((mode_t)arg);
	return 1;
}

int main()
{
	app = new paintapp_t;
	assert(app);
	int rtn = app->Execute();
	delete app;
	return rtn;
}


