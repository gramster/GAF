#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include <assert.h>
#include <conio.h>
#include <time.h>
#include "glib.h"

// test 3d wireframe objects

int framecnt = 0;

int cube_vertices[][3] =
{
  { -1, -1,  1 },
  {  1, -1,  1 },
  {  1,  1,  1 },
  { -1,  1,  1 },
  { -1, -1, -1 },
  {  1, -1, -1 },
  {  1,  1, -1 },
  { -1,  1, -1 }
};

int cube_edges[][2] =
{
  { 0, 1 },
  { 1, 2 },
  { 2, 3 },
  { 3, 0 },
  { 4, 5 },
  { 5, 6 },
  { 6, 7 },
  { 7, 4 },
  { 0, 4 },
  { 1, 5 },
  { 2, 6 },
  { 3, 7 } 
};

//---------------------------------------------------------------

int Quit(void *arg);

class wirecanvas_t : public canvas_t
{
	MatElt distance, ax, ay, az;
	int xpos, ypos, spinx, spiny, spinz;
	World3D *w;
public:
	wirecanvas_t(int left_in, int top_in, int right_in, int bottom_in,
		unsigned long flags_in = 0l, color_t fillcolor_in = Black)
		: canvas_t(left_in, top_in, right_in, bottom_in, 
				flags_in, fillcolor_in)
	{
		distance = 1000;
		spinx = spiny = spinz = 0;
		xpos = ypos = 100;
		PaintMode(XOrPut);
		w = new World3D(2);
		w->objects[0].SetVertices(8, cube_vertices);
		w->objects[0].SetEdges(12, cube_edges);
		w->objects[0].Init();
		w->objects[0].Scale(500);
		//w->objects[0].Translate(100, 0, 0);
		w->objects[1].SetVertices(8, cube_vertices);
		w->objects[1].SetEdges(12, cube_edges);
		w->objects[1].Init();
		w->objects[1].Scale(100);
		//w->objects[1].Translate(100, 0, 0);
		ax = ay = az = 0;
	}
	int HandleEvent(event_t *e);
	void Draw();
};

void wirecanvas_t::Draw()
{
	World3D tmp = *w;
	static World3D oldworld;
	static int oldx, oldy, first = 1, dist = 0;
	tmp.Turn(ax, ay, az);
	tmp.Move(0, 0, -5000-distance-PPdist);
	tmp.objects[1].Translate(0, 0, -dist);
	if (dist>=5000)
		dist = 0;
	else dist += 20;
	//tmp.Project(PPdist);	// perspective projection
	mouse.Hide();
	if (ThreeD)
	{
		if (!first) oldworld.DrawStereo(this, oldx, oldy);
		tmp.DrawStereo(this, xpos, ypos);
	}
	else
	{
		if (!first) oldworld.Draw(this, oldx, oldy);
		tmp.Draw(this, xpos, ypos);
	}
	mouse.Show();
	first = 0;
	oldx = xpos;
	oldy = ypos;
	oldworld = tmp;
	delay(5);
#ifdef FIXED
	if (spinx) ax++;
	if (spiny) ay++;
	if (spinz) az++;
#else
	if (spinx) ax += 0.1;
	if (spiny) ay += 0.1;
	if (spinz) az += 0.1;
#endif
	framecnt++;
}

int wirecanvas_t::HandleEvent(event_t *e)
{
	static int oldx = -1, oldy = -1;
	int x, y; mouse.Position(x, y); x -= Left(); y -= Top();
	if (oldx != x)
	{
		distance += 5*(x-oldx);
		if (distance>1500) distance = 1500;
		if (distance<300) distance = 300;
		oldx = x;
	}
	if (oldy != y)
	{
		xpos += y-oldy;
		ypos += y-oldy;
		oldy = y;
	}
	if (e->Type() == ButtonReleased)
	{
		switch (e->Arg1())
		{
			case 0: spinx = 1 - spinx; break;
			case 1: spiny = 1 - spiny; break;
			case 2: spinz = 1 - spinz; break;
		}
	}
	return 1;
}

class wireapp_t : public application_t
{
	wirecanvas_t *canvas;
	menu_t *menu;
public:
	wireapp_t()
		: application_t(Blue)
	{
		int wl = textwidth("MenuWidth");
		canvas = new wirecanvas_t(wl+50, 1,
			getmaxx() - 1, (getmaxy()<=400) ? getmaxy()-1 : 400,
			W_BORDER);
		assert(canvas);
		canvas->Clear();
		menu = new menu_t(1, 1, 1);
		assert(menu);
		menu->AddEntry("Quit", Quit);
		menu->Foreground(Black);
		menu->Background(LightBlue);
	}
	int Execute();
	~wireapp_t()
	{
		assert(canvas && menu);
		delete menu;
		delete canvas;
	}
};

int wireapp_t::Execute()
{
	menu->Show();
	for (;;)
	{
		if (GotEvent())
			if (GetEvent())
				if (HandleEvent()==0)
					break;
		canvas->Draw();
	}
	return 0;
}

wireapp_t *app;

int Quit(void *arg)
{
	(void)arg;
	return 0;
}

static void useage()
{
	fprintf(stderr, "Useage: testwire [-3 [-s<sep>]]\n");
	exit(-1);
}

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		for (int a = 1; a < argc; a++)
		{
			if (argv[a][0] == '-')
			{
				switch(argv[a][1])
				{
					case '3':
						ThreeD = 1;
						break;
					case 's':
						EyeSep = atoi(&argv[a][2]);
						break;
					default:
						useage();
				}
			}
			else useage();
		}
	}
	time_t start = time(NULL);
	app = new wireapp_t;
	assert(app);
	int rtn = app->Execute();
	delete app;
	printf("Frames per second: %d\n", framecnt / (time(NULL)-start));
	return rtn;
}


