#ifndef __WINDOW_H
#define __WINDOW_H

#include <graphics.h>
#include <string.h>
#include "debug.h"
#include "event.h"

typedef enum
{
	Black,		Blue,		Green,		Cyan,
	Red,		Magenta,	Brown,		LightGray,
	DarkGray,	LightBlue,	LightGreen,	LightCyan,
	LightRed,	LightMagenta,	Yellow,	White,
	DefaultColor
} color_t;

typedef enum
{
    EmptyFill,	SolidFill,		LineFill,		LtSlashFill,
    SlashFill,	BkslashFill,	LtBkslashFill,	HatchFill,
    XHatchFill,	InterleaveFill,	WideDotFill,	CloseDotFill,
    UserFill,		DefaultFill
} fillstyle_t;

typedef enum
{
    SolidLine, DottedLine, CenterLine, DashedLine, UserLine
} linestyle_t;

typedef enum
{
	CopyPut, XOrPut, OrPut, AndPut, NotPut, SpritePut
} paintmode_t;

//-------------------------------------------------------------------
// Bitmaps and 8-bit Pixmaps

class pixmap_t MBUG1
{
	short width, height;
	char *pixels;
public:
	pixmap_t(int width_in, int height_in, unsigned char* pixels_in);
	~pixmap_t();
	void Show(int left, int top, paintmode_t mode = CopyPut);
	friend class bitmap_t;
};

class bitmap_t : public pixmap_t
{
	color_t fg, bg;
public:
	bitmap_t(int width_in, int height_in, unsigned char* bits_in,
		color_t fg_in = White, color_t bg_in = Black);
	void Show(int left, int top, paintmode_t mode = CopyPut);
};

//-------------------------------------------------------------------
// Window flags

#define W_BORDER	1

class window_t MBUG1
{
private:
	int	top, left, bottom, right;	// size//position info
	unsigned long flags;
	int handle;
	int visible;
	char *savedimg;
	unsigned savesize;
public:
	window_t(int left_in, int top_in, int right_in, int bottom_in,
		unsigned long flags_in = 0l);
	virtual ~window_t();
	void Move(int new_left, int new_top)
	{
		right += (new_left-left);
		left = new_left;
		bottom+= (new_top - top);
		top = new_top;
//		SendEvent(MovedWindow, handle);
	}
	void Resize(int width, int height)
	{
		right = left + width;
		bottom = top + height;
//		SendEvent(SizedWindow, handle);
	}
	inline int Top()
	{
		return top;
	}
	inline int Left()
	{
		return left;
	}
	inline int Bottom()
	{
		return bottom;
	}
	inline int Right()
	{
		return right;
	}
	void Clear(color_t color, fillstyle_t style);
	inline void Show()
	{
		visible = 1;
//		SendEvent(ExposeWindow, handle);
	}
	inline void Hide()
	{
		visible = 0;
//		SendEvent(HideWindow, handle);
	}
	inline int Inside(int x, int y)
	{
		return (x>=left && x<=right && y>=top && y<=bottom);
	}
	inline int Handle()
	{
		return handle;
	}
	inline int Width()
	{
		return right-left+1;
	}
	inline int Height()
	{
		return bottom-top+1;
	}
	void ViewPort();
	void Save();
	void Restore();
	int Intersect(const window_t *w);
	virtual int HandleEvent(event_t *e) = 0;
};

//------------------------------------------------------------------------

class canvas_t : public window_t
{
private:
	color_t fgcol, bgcol;			// current colours
	linestyle_t linesty;			// current line style
	fillstyle_t fillsty;			// current fill style
	color_t fillcolor;				// and color
	paintmode_t pmode;
public:
	canvas_t(int left_in, int top_in, int right_in, int bottom_in,
		unsigned long flags_in = 0l, color_t fillcolor_in = Black);
	void Clear(color_t color = DefaultColor, fillstyle_t style = DefaultFill);
	inline void Show()
	{
		Clear();
	}
	inline void Foreground(color_t fg_in)
	{
		fgcol = fg_in;
		setcolor(fgcol);
	}
	inline void Color(color_t fg_in)
	{
		fgcol = fg_in;
		setcolor(fgcol);
	}
	inline void Background(color_t bg_in)
	{
		bgcol = bg_in;
//	setbkcolor(bgcol);
	}
	inline color_t Foreground()
	{
		return fgcol;
	}
	inline color_t Background()
	{
		return bgcol;
	}
	inline void LineStyle(linestyle_t style)
	{
		linesty = style;
	}
	inline void FillStyle(fillstyle_t style)
	{
		fillsty = style;
	}
	inline void FillColor(color_t fcolor_in)
	{
		fillcolor = fcolor_in;
	}
	inline void PaintMode(paintmode_t mode)
	{
		setwritemode(pmode = mode);
	}
	inline void PaintMode()
	{
		setwritemode(pmode);
	}
	void ViewPort();
	void Text(int x, int y, char *txt);
	void Point(int x, int y);
	void Line(int startx, int starty, int endx, int endy);
	void Rectangle(int startx, int starty, int endx, int endy);
	void FilledRectangle(int startx, int starty, int endx, int endy);
	void Arc(int centerx, int centery, int start, int end, int radius);
	void Circle(int centerx, int centery, int radius);
	void FilledCircle(int centerx, int centery, int radius);
	void Spiral(int centerx, int centery, int radius, int nturns, int angle);
	void Twist(int centerx, int centery, int radius, int nturns, int angle);
	void CircleLattice(int centerx, int centery, int radius, int npoints);
	void Grid(int left, int top, int dim, int siz);
	void Button(int x, int y, int radius, color_t fg, color_t bg);
	void FillPoly(int npts, int *pts, color_t color);
	void NestPoly(int centerx, int centery, int npts, int *pts, int cnt,
			float ratio);
	inline void Blit(int left, int top, bitmap_t &bm, paintmode_t mode = CopyPut)
	{
		ViewPort();
		bm.Show(left, top, mode);
	}
	inline void Blit(int left, int top, pixmap_t &pm, paintmode_t mode = CopyPut)
	{
		ViewPort();
		pm.Show(left, top, mode);
	}
	virtual int HandleEvent(event_t *e);
};

//------------------------------------------------------------------------

typedef int (*menu_elt_handler_t)(void *arg);

class menu_elt_t MBUG1
{
private:
	menu_elt_handler_t handler;
	char	*text;
	void	*arg;
public:
	void Set(char *text_in = NULL, menu_elt_handler_t handler_in = NULL,
		void *arg_in = NULL);
	menu_elt_t(char *text_in = NULL, menu_elt_handler_t handler_in = NULL,
		void *arg_in = NULL);
	~menu_elt_t();
	friend class menu_t;
};

class menu_t : public canvas_t
{
	menu_elt_t *elts;
	int nelts, eltnow;
	int maxw, laste;
	int width, height;
	int cancel; // value returned on right button click
	char *underimg;		// should be part of a popup class
	unsigned undersize;
public:
	menu_t(int x_in, int y_in, int nelts_in, int cancel_in = 0);
	~menu_t();
	void AddEntry(char *text_in = NULL, menu_elt_handler_t handler_in = NULL,
		void *arg_in = NULL);
	void Outline(int elt, color_t topcolor, color_t bottomcolor);
	void Show();
	void Execute();
	int HandleEvent(event_t *e);
};

//------------------------------------------------------------------------

class winlistelt_t MBUG1
{
public:
	window_t *w;
	winlistelt_t *next;
	winlistelt_t(window_t *w_in, winlistelt_t *next_in = NULL)
	{
		w = w_in;
		next = next_in;
	}
};

class winmgr_t MBUG1
{
	winlistelt_t *firstwin;
	int nexthandle;
	int focush;
public:
	winmgr_t();
	~winmgr_t();
	window_t *Find(int handle);
	int Register(window_t *w);
	void Deregister(int handle);
	void Raise(int handle);
	void Lower(int handle);
	int  Focus() { return focush; }
	void SetFocus(int handle);
	void ReleaseFocus(int handle);
	int DispatchEvent(event_t *event);
};

extern winmgr_t WindowManager;
#endif

