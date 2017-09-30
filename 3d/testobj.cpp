#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include <assert.h>
#include <conio.h>
#include <time.h>
#include "glib.h"

// test polygon-shaded 3d objects

int frame = 0;
int eyesep = 5;

int Quit(void *arg);
void ShowInfo(char *msg);

class objectcanvas_t : public canvas_t
{
	int xpos, ypos, distance, spinx, spiny, spinz;
	Object3D pyramid;
	MatElt ax, ay, az;
public:
	objectcanvas_t(int left_in, int top_in, int right_in, int bottom_in,
		unsigned long flags_in = 0l, color_t fillcolor_in = Black);
	int HandleEvent(event_t *e);
	void DrawPyramid();
};

int pyramid_vertices[][3] =
{
  { -10,  10, -10 },
  {  10,  10, -10 },
  {   0, -10,   0 },
  {  10,  10,  10 },
  { -10,  10,  10 }
};

// Each polygon is held in an array, giving number of vertices,
// colour index, and indexes of vertices

int poly1[] = { 3, 1, 1, 2, 0 };
int poly2[] = { 3, 2, 3, 2, 1 };
int poly3[] = { 3, 3, 4, 2, 3 };
int poly4[] = { 3, 4, 0, 2, 4 };
int poly5[] = { 4, 5, 4, 3, 1, 0 };

int *(pyramid_polygons[]) =
{
	poly1, poly2, poly3, poly4, poly5
};

#define Elts(a)		(sizeof(a) / sizeof(a[0]))

objectcanvas_t::objectcanvas_t(int left_in, int top_in,
	int right_in, int bottom_in,
	unsigned long flags_in, color_t fillcolor_in)
	: canvas_t(left_in, top_in, right_in, bottom_in, 
			flags_in, fillcolor_in)
{
	distance = 1000;
	spinx = spiny = spinz = 0;
	xpos = ypos = 150;
	PaintMode(XOrPut);
	ax = ay = az = 0;
	pyramid.Convex(1);
	pyramid.SetVertices(Elts(pyramid_vertices), pyramid_vertices);
	pyramid.SetPolygons(Elts(pyramid_polygons), pyramid_polygons);
}

void objectcanvas_t::DrawPyramid()
{
	static Point2D tl, br;
	static int first = 1, oldx, oldy;
	pyramid.Init();
	pyramid.Scale(5);
	pyramid.Rotate(ax, ay, az);
	pyramid.Translate(0, 0, distance);
	pyramid.Project(600);	// perspective projection
	mouse.Hide();
	if (!first)
	{
		Foreground(Black);
		FilledRectangle(oldx+tl.X(), oldy+tl.Y(),
			oldx+br.X(), oldy+br.Y());
	}
	else first = 0;
	oldx = xpos;
	oldy = ypos;
	pyramid.Frame(tl, br);
	if (ThreeD) pyramid.DrawStereo(this, xpos, ypos);
	else pyramid.Draw(this, xpos, ypos);
	mouse.Show();
	delay(100);
#ifdef FIXED
	if (spinx) ax++;
	if (spiny) ay++;
	if (spinz) az++;
#else
	if (spinx) ax += 0.1;
	if (spiny) ay += 0.1;
	if (spinz) az += 0.1;
#endif
	frame++;
}

int objectcanvas_t::HandleEvent(event_t *e)
{
	static int oldx = -1, oldy = -1;
	int x, y;
	mouse.Position(x, y); x -= Left(); y -= Top();
	if (oldx != -1 && oldx != x)
	{
		distance += 5*(x-oldx);
		if (distance<800) distance = 800;
		if (distance>1500) distance = 1500;
		char buf[80];
		sprintf(buf,"Distance: %d", distance);
		ShowInfo(buf);
	}
	oldx = x;
	if (oldy != -1 && oldy != y)
	{
		xpos += y-oldy;
		ypos += y-oldy;
	}
	oldy = y;
	if (e->Type() == ButtonReleased)
	{
		switch (e->Arg1())
		{
			case 0: spinx = 1 - spinx; break;
			//case 1: eyesep++; break;
			case 1: spiny = 1 - spiny; break;
			case 2: spinz = 1 - spinz; break;
		}
	}
	return 1;
}

class objectapp_t : public application_t
{
	objectcanvas_t *canvas;
	menu_t *menu;
	canvas_t *infowin;
public:
	objectapp_t()
		: application_t(Blue)
	{
		int wl = textwidth("MenuWidth");
		canvas = new objectcanvas_t(wl+50, 1,
			getmaxx() - 1, (getmaxy()<=400) ? getmaxy()-1 : 400,
			W_BORDER);
		assert(canvas);
		canvas->Clear();
		menu = new menu_t(1, 1, 1);
		assert(menu);
		menu->AddEntry("Quit", Quit);
		menu->Foreground(Black);
		menu->Background(LightBlue);
		infowin = new canvas_t(2, getmaxy()-40, 238, getmaxy()-20, W_BORDER);
		assert(infowin);
		infowin->Foreground(Black);
		infowin->Background(Green);
		infowin->Clear();
	}
	int Execute();
	void ShowInfo(char *msg)
	{
		infowin->Color(Green);
		infowin->FilledRectangle(10,5,
			infowin->Width()-10,15);
		infowin->Color(Black);
		infowin->Text(10,5,msg);
	}
	~objectapp_t()
	{
		assert(canvas && menu && infowin);
		delete menu;
		delete canvas;
		delete infowin;
	}
};

int objectapp_t::Execute()
{
	menu->Show();
	for (;;)
	{
		if (GotEvent())
			if (GetEvent())
				if (HandleEvent()==0)
					break;
		canvas->DrawPyramid();
	}
	return 0;
}

objectapp_t *app;

void ShowInfo(char *msg)
{
	app->ShowInfo(msg);
}

int Quit(void *arg)
{
	(void)arg;
	return 0;
}

static void useage()
{
	fprintf(stderr, "Useage: testobj [-3 [-s<eyesep>] ]\n");
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
						eyesep = atoi(&argv[a][2]);
						break;
					default:
						useage();
				}
			}
			else useage();
		}
	}
	time_t start = time(NULL);
	app = new objectapp_t;
	assert(app);
	int rtn = app->Execute();
	delete app;
	printf("Frames per second: %d\n", frame / (time(NULL)-start));
	return rtn;
}


