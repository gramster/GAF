#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include <assert.h>
#include "app.h"
#include "mouse.h"

static unsigned char blackstone_bits[] = {
   0xc0, 0x03, 0xf0, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xfe, 0x7f,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xef, 0xfe, 0x77, 0xfe, 0x7b,
   0xfc, 0x3c, 0xf8, 0x1f, 0xf0, 0x0f, 0xc0, 0x03};

static unsigned char whitestone_bits[] = {
   0xc0, 0x03, 0x30, 0x0c, 0x08, 0x10, 0x04, 0x20, 0x02, 0x40, 0x02, 0x40,
   0x01, 0x80, 0x01, 0x80, 0x01, 0x90, 0x01, 0x90, 0x02, 0x48, 0x02, 0x44,
   0x04, 0x23, 0x08, 0x10, 0x30, 0x0c, 0xc0, 0x03};

static unsigned char graystone_bits[] = {
   0xc0, 0x03, 0xb0, 0x0e, 0x58, 0x15, 0xac, 0x2a, 0x56, 0x55, 0xaa, 0x6a,
   0x55, 0xd5, 0xab, 0xaa, 0x55, 0xd5, 0xab, 0xba, 0x56, 0x5d, 0xaa, 0x6e,
   0x54, 0x37, 0xa8, 0x1a, 0x70, 0x0d, 0xc0, 0x03};

bitmap_t blackstone(16, 16, blackstone_bits);
bitmap_t whitestone(16, 16, whitestone_bits);
bitmap_t  graystone(16, 16, graystone_bits);

int Quit(void *arg)
{
	(void)arg;
	return 0;
}

class gamecanvas_t : public canvas_t
{
	int down, startx, starty, oldx, oldy, bsize;
public:
	gamecanvas_t(int left_in, int top_in, int bsize_in,
		unsigned long flags_in = 0l, color_t fillcolor_in = Black)
		: canvas_t(left_in, top_in,
			left_in+bsize_in*20+20, top_in+bsize_in*20+20, 
			flags_in, fillcolor_in)
	{
		bsize = bsize_in;
	}
	int HandleEvent(event_t *e);
};

int gamecanvas_t::HandleEvent(event_t *e)
{
	int x, y; mouse.Position(x, y); x -= Left(); y -= Top();
	if (down && e->Type() == MouseMoved)
	{
	}
	else if (e->Type() == ButtonPressed && e->Arg1()==0)
	{
		if (!down)
		{
			down = 1;
			startx = oldx = x;
			starty = oldy = y;
		}
	}
	else if (e->Type() == ButtonReleased && e->Arg1()==0)
	{
		if (down)
		{
			down = 0;
		}
	}
	return 1;
}

class gameapp_t : public application_t
{
	gamecanvas_t *canvas;
	menu_t *menu;
public:
	gameapp_t(int bsize)
		: application_t(Blue)
	{
		int wl = textwidth("MenuWidth");
		canvas = new gamecanvas_t(wl, 1, bsize, W_BORDER);
		assert(canvas);
		canvas->Clear();
		canvas->Grid(10, 10, bsize, 20);
		menu = new menu_t(1, 1, 1);
		assert(menu);
		menu->AddEntry("Quit", Quit);
	}
	int Execute();
	~gameapp_t()
	{
		assert(canvas && menu);
		delete menu;
		delete canvas;
	}
};

int gameapp_t::Execute()
{
	canvas->Blit(2,2,blackstone);
	canvas->Blit(22,22,whitestone);
	canvas->Blit(42,42,graystone);
	menu->Show();
//	int sz = imagesize(0,0,80,40);
//	char *image = new char[sz];
//	getimage(0,0,80,40, (void far *)image);
	while (GetEvent())
		if (HandleEvent()==0)
			break;
	return 0;
}


int main()
{
	gameapp_t *app = new gameapp_t(19);
	assert(app);
	int rtn = app->Execute();
	delete app;
	return rtn;
}


