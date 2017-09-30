#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include <assert.h>
#include <conio.h>
#include "app.h"
#include "mouse.h"

int Quit(void *arg);
int FileMenu(void *arg);
int CommandHook(void *arg);

canvas_t *Canvas = NULL; // hax

typedef enum
{
	Start, Finish, Morph, File
} cmd_t;

const MAXPOINTS = 50;

class Point
{
	int x, y;
	void Copy(int x_in, int y_in)
	{
		x = x_in;
		y = y_in;
	}
public:
	Point()
	{ }
	Point(int x_in, int y_in)
	{
		Copy(x_in, y_in);
	}
	Point(Point &p)
	{
		Copy(p.x, p.y);
	}
	void Set(int x_in, int y_in)
	{
		Copy(x_in, y_in);
	}
	Point& operator=(const Point &rhs)
	{
		Copy(rhs.x, rhs.y);
		return *this;
	}
	int X()
	{
		return x;
	}
	int Y()
	{
		return y;
	}
	void Save(FILE *fp)
	{
		fwrite(&x, sizeof(x), 1, fp);
		fwrite(&y, sizeof(y), 1, fp);
	}
	void Load(FILE *fp)
	{
		fread(&x, sizeof(x), 1, fp);
		fread(&y, sizeof(y), 1, fp);
	}
};

class Line_t
{
	Point start, end;
	void Copy(const Point &start_in, const Point & end_in)
	{
		start = start_in;
		end = end_in;
	}
	void Copy(const Line_t &l)
	{
		Copy(l.start, l.end);
	}
public:
	Line_t()
	{ }
	Line_t(const Point &start_in, const Point &end_in)
	{
		Copy(start_in, end_in);
	}
	Line_t(const Line_t &l)
	{
		Copy(l);
	}
	void Set(const Point &start_in, const Point &end_in)
	{
		Copy(start_in, end_in);
	}
	Line_t& operator=(const Line_t &rhs)
	{
		Copy(rhs);
		return *this;
	}
	Point Midpoint()
	{
		Point p((start.X() + end.X())/2, (start.Y()+end.Y())/2);
		return p;
	}
};

class Polygon
{
	int numpts;
	Point pts[MAXPOINTS];
public:
	void Copy(const Polygon &p)
	{
		numpts = p.numpts;
		for (int i = 0; i < numpts; i++)
			pts[i] = p.pts[i];
	}
	Polygon()
	{
		numpts = 0;
	}
	Polygon(Polygon &p)
	{
		Copy(p);
	}
	Polygon& operator=(const Polygon &rhs)
	{
		Copy(rhs);
		return *this;
	}
	void Line(Point &start, Point &end)
	{
		Canvas->Line(start.X(), start.Y(), end.X(), end.Y());
	}
	void Draw()
	{
		for (int i = 1; i < numpts; i++)
			Line(pts[i-1], pts[i]);
		Line(pts[i-1], pts[0]);
	}
	void Empty()
	{
		numpts = 0;
	}
	void AddPoint(int x, int y)
	{
		pts[numpts].Set(x, y);
		if (numpts)
			Line(pts[numpts], pts[numpts-1]);
		numpts++;
		assert(numpts <= MAXPOINTS);
	}
	void AddPoint(Point &p)
	{
		AddPoint(p.X(), p.Y());
	}
	void Close()
	{
		Line(pts[numpts-1], pts[0]);
	}
	int Points()
	{
		return numpts;
	}
	void Morph(Polygon &p1, Polygon &p2)
	{
		numpts = p1.numpts;
		for (int i = 0; i < numpts; i++)
		{
			Line_t l;
			l.Set(p1.pts[i], p2.pts[i]);
			pts[i] = l.Midpoint();
		}
	}
	void Save(FILE *fp)
	{
		fwrite(&numpts, sizeof(numpts), 1, fp);
		for (int i = 0; i < numpts; i++)
			pts[i].Save(fp);
	}
	void Load(FILE *fp)
	{
		fread(&numpts, sizeof(numpts), 1, fp);
		for (int i = 0; i < numpts; i++)
			pts[i].Load(fp);
	}
};

class morphcanvas_t : public canvas_t
{
	Polygon start, end, current;
	int done;
public:
	void Reset()
	{
		Clear();
		Foreground(Blue);
		start.Draw();
		Foreground(Green);
		end.Draw();
		Foreground(White);
		current.Empty();
		done = 0;
	}
	morphcanvas_t(int left_in, int top_in, int right_in, int bottom_in,
		unsigned long flags_in = 0l, color_t fillcolor_in = Black)
		: canvas_t(left_in, top_in, right_in, bottom_in, 
				flags_in, fillcolor_in)
	{
		Reset();
	}
	int HandleEvent(event_t *e);
	void MorphPolys(Polygon *polys, int first, int mid, int last);
	void MorphPoints(int frames);
	void Command(cmd_t cmd)
	{
		switch(cmd)
		{
			case Start:
				start = current;
				break;
			case Finish:
				end = current;
				break;
			case Morph:
				MorphPoints(10);
				break;
		}
	}
	void Save(FILE *fp)
	{
		start.Save(fp);
		end.Save(fp);
	}
	void Load(FILE *fp)
	{
		start.Load(fp);
		end.Load(fp);
	}
};

void morphcanvas_t::MorphPolys(Polygon *polys, int first, int mid,
	int last)
{
	if (mid == first || mid == last) return;
	polys[mid].Morph(polys[first], polys[last]);
	MorphPolys(polys, first, first + (mid-first)/2, mid);
	MorphPolys(polys, mid, mid + (last-mid)/2, last);
}

void morphcanvas_t::MorphPoints(int frames)
{
	if (start.Points() < 2 || end.Points() < 2 ||
		start.Points() != end.Points())
			return;
	Polygon *polys = new Polygon[frames];
	polys[0] = start;
	polys[frames-1] = end;
	MorphPolys(polys, 0, frames/2, frames-1);
	while (!kbhit())
	{
		for (int f = 0; f < frames; f++)
		{
			Clear();
			polys[f].Draw();
		}
		while (--f >= 0)
		{
			Clear();
			polys[f].Draw();
		}
	}
	delete [] polys;
}

int morphcanvas_t::HandleEvent(event_t *e)
{
	int x, y; mouse.Position(x, y); x -= Left(); y -= Top();
	if (e->Type() == ButtonReleased)
	{
		if (done) Reset();
		current.AddPoint(x, y);
		if (e->Arg1()!=0) // not left button
		{
			current.Close();
			done = 1;
		}
	}
	return 1;
}

class morphapp_t : public application_t
{
	morphcanvas_t *canvas;
	menu_t *menu;
public:
	morphapp_t()
		: application_t(Blue)
	{
		int wl = textwidth("MenuWidth");
		canvas = new morphcanvas_t(wl+50, 1,
			getmaxx() - 1, (getmaxy()<=400) ? getmaxy()-1 : 400,
			W_BORDER);
		assert(canvas);
		Canvas = canvas; // hax
		canvas->Clear();
		menu = new menu_t(1, 1, 5);
		assert(menu);
		menu->AddEntry("File",  FileMenu, (void*)File);
		menu->AddEntry("Start",  CommandHook, (void*)Start);
		menu->AddEntry("Finish",  CommandHook, (void*)Finish);
		menu->AddEntry("Morph",  CommandHook, (void*)Morph);
		menu->AddEntry("Quit", Quit);
		menu->Foreground(Black);
		menu->Background(LightBlue);
	}
	void Command(cmd_t cmd)
	{
		canvas->Command(cmd);
	}
	int Execute();
	void Reset()
	{
		canvas->Reset();
	}
	void Save(FILE *fp)
	{
		canvas->Save(fp);
	}
	void Load(FILE *fp)
	{
		canvas->Load(fp);
	}
	~morphapp_t()
	{
		assert(canvas && menu);
		delete menu;
		delete canvas;
	}
};

int morphapp_t::Execute()
{
	menu->Show();
	while (GetEvent())
		if (HandleEvent()==0)
			break;
	return 0;
}

morphapp_t *app;

int DoSave(void *arg)
{
	(void)arg;
	FILE *fp = fopen("saved.pol", "w");
	app->Save(fp);
	fclose(fp);
	return 0;
}

int DoLoad(void *arg)
{
	(void)arg;
	FILE *fp = fopen("saved.pol", "r");
	app->Load(fp);
	fclose(fp);
	app->Reset();
	return 0;
}

int FileMenu(void *arg)
{
	(void)arg;
	menu_t *m = new menu_t(140, 1, 3);
	assert(m);
	m->AddEntry("Load", DoLoad);
	m->AddEntry("Save", DoSave);
	m->AddEntry("Quit", Quit);
	m->Foreground(Black);
	m->Background(Blue);
	m->Execute();
	delete m;
	return 1;
}

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

int CommandHook(void *arg)
{
	app->Command((cmd_t)arg);
	return 1;
}

int main()
{
	app = new morphapp_t;
	assert(app);
	int rtn = app->Execute();
	delete app;
	return rtn;
}


