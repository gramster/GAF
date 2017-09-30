#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include <assert.h>
#include <conio.h>
#include <time.h>
#include "glib.h"

void ShowInfo(char *msg);

typedef enum
{
	CUBE, PYRAMID, W
} object_t;

object_t object = CUBE;

// test 3d wireframe objects

int framecnt = 0;

int cube_vertices[][3] =
{
  { -10, -10,  10 },
  {  10, -10,  10 },
  {  10,  10,  10 },
  { -10,  10,  10 },
  { -10, -10, -10 },
  {  10, -10, -10 },
  {  10,  10, -10 },
  { -10,  10, -10 }
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

int pyramid_vertices[][3] =
{
	{  0,-10,  0 },
	{ 10, 10, 10 },
	{-10, 10, 10 },
	{-10, 10,-10 },
	{ 10, 10,-10 }
};

int pyramid_edges[][2] =
{
	{ 0, 1 },
	{ 0, 2 },
	{ 0, 3 },
	{ 0, 4 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 4 },
	{ 4, 1 }
};

int w_vertices[][3] =
{
	{-25,-15, 10 },
	{-15,-15, 10 },
	{-10,  0, 10 },
	{ -5,-15, 10 },
	{  5,-15, 10 },
	{ 10,  0, 10 },
	{ 15,-15, 10 },
	{ 25,-15, 10 },
	{ 20, 15, 10 },
	{  7, 15, 10 },
	{  0,  0, 10 },
	{ -7, 15, 10 },
	{-20, 15, 10 },
	{-25,-15,-10 },
	{-15,-15,-10 },
	{-10,  0,-10 },
	{ -5,-15,-10 },
	{  5,-15,-10 },
	{ 10,  0,-10 },
	{ 15,-15,-10 },
	{ 25,-15,-10 },
	{ 20, 15,-10 },
	{  7, 15,-10 },
	{  0,  0,-10 },
	{ -7, 15,-10 },
	{-20, 15,-10 }
};

int w_edges[][2] =
{
	{0,1 },
	{1,2 },
	{2,3 },
	{3,4 },
	{4,5 },
	{5,6 },
	{6,7 },
	{7,8 },
	{8,9 },
	{9,10 },
	{10,11 },
	{11,12 },
	{12,0 },
	{13,14 },
	{14,15 },
	{15,16 },
	{16,17 },
	{17,18 },
	{18,19 },
	{19,20 },
	{20,21 },
	{21,22 },
	{22,23 },
	{23,24 },
	{24,25 },
	{25,13 },
	{0,13 },
	{1,14 },
	{2,15 },
	{3,16 },
	{4,17 },
	{5,18 },
	{6,19 },
	{7,20 },
	{8,21 },
	{9,22 },
	{10,23 },
	{11,24 },
	{12,25 } 
};

//---------------------------------------------------------------

int Quit(void *arg);

class wirecanvas_t : public canvas_t
{
	MatElt distance, ax, ay, az;
	int xpos, ypos, spinx, spiny, spinz;
	WireFrame frame;
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
		if (object == CUBE)
		{
			frame.SetVertices(8, cube_vertices);
			frame.SetEdges(12, cube_edges);
		}
		else if (object == PYRAMID)
		{
			frame.SetVertices(5, pyramid_vertices);
			frame.SetEdges(8, pyramid_edges);
		}
		else
		{
			frame.SetVertices(26, w_vertices);
			frame.SetEdges(39, w_edges);
		}
		frame.Init();
		frame.Scale(10);
		ax = ay = az = 0;
	}
	int HandleEvent(event_t *e);
	void DrawCube();
};

void wirecanvas_t::DrawCube()
{
	WireFrame tmp = frame;
	static WireFrame oldframe;
	static int oldx, oldy, first = 1;
	tmp.Rotate(ax, ay, az);
	tmp.Translate(0, 0, distance+PPdist);
	tmp.Project(PPdist);	// perspective projection
	mouse.Hide();
	if (ThreeD)
	{
		if (!first) oldframe.DrawStereo(this, oldx, oldy);
		tmp.DrawStereo(this, xpos, ypos);
	}
	else
	{
		if (!first) oldframe.Draw(this, oldx, oldy);
		tmp.Draw(this, xpos, ypos);
	}
	first = 0;
	oldx = xpos;
	oldy = ypos;
	oldframe = tmp;
	mouse.Show();
	delay(15);
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
	char buf[80];
	sprintf(buf, "%d, %d, %d", tmp.Point(0).x, tmp.Point(0).y,
		tmp.Point(0).z);
	ShowInfo(buf);
}

int wirecanvas_t::HandleEvent(event_t *e)
{
	static int oldx = -1, oldy = -1;
	int x, y; mouse.Position(x, y); x -= Left(); y -= Top();
	if (oldx != x)
	{
		distance += 5*(x-oldx);
		oldx = x;
		if (distance>1500) distance = 1500;
		if (distance<300) distance = 300;
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
	canvas_t *infowin;
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
		infowin = new canvas_t(2, getmaxy()-40, 238, getmaxy()-20, W_BORDER);
		assert(infowin);
		infowin->Foreground(Black);
		infowin->Background(Green);
		infowin->Clear();
	}
	int Execute();
	~wireapp_t()
	{
		assert(canvas && menu && infowin);
		delete menu;
		delete canvas;
		delete infowin;
	}
	void ShowInfo(char *msg)
	{
		infowin->Color(Green);
		infowin->FilledRectangle(10,5,
			infowin->Width()-10,15);
		infowin->Color(Black);
		infowin->Text(10,5,msg);
	}
};

int wireapp_t::Execute()
{
	menu->Show();
	canvas->Twist(150, 150, 120, 5, 0);
	sleep(1);
	canvas->Clear();
	canvas->CircleLattice(150, 150, 120, 15);
	sleep(1);
	canvas->Clear();
	{
		int pts[8];
		pts[0] = pts[1] = pts[3] = pts[6] = -100;
		pts[2] = pts[4] = pts[5] = pts[7] = 100;
		canvas->NestPoly(150,150,4,pts,15,.1);
		sleep(1);
		canvas->Clear();
	}

	for (;;)
	{
		if (GotEvent())
			if (GetEvent())
				if (HandleEvent()==0)
					break;
		canvas->DrawCube();
	}
	return 0;
}

wireapp_t *app;

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
	fprintf(stderr, "Useage: testwire [-3 [-s<sep>]] [-P | -W]\n");
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
					case 'P':
						object = PYRAMID;
						break;
					case 'W':
						object = W;
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


