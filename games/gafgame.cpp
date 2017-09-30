#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <graphics.h>
#include <ctype.h>
#include <assert.h>
#include "debug.h"
#include "app.h"
#include "mouse.h"
#include "movegame.h"

extern unsigned _stklen = 16000;

extern game_t *Game(int sz, int &vertices, int argc, char *argv[],
	int &infolines, int &menuentries);
extern int showsearch;
gridgame_t *game;
int vertices = 0, optionentries = 0;
color_t whitecolor=White;
color_t blackcolor=Black;


#include "app.h"

static unsigned char whitestone_bits[] = {
   0xc0, 0x03, 0xf0, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xfe, 0x7f,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xef, 0xfe, 0x77, 0xfe, 0x7b,
   0xfc, 0x3c, 0xf8, 0x1f, 0xf0, 0x0f, 0xc0, 0x03};

static unsigned char black_bits[] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0};

static unsigned char blackstone_bits[] = {
   0xc0, 0x03, 0x30, 0x0c, 0x08, 0x10, 0x04, 0x20, 0x02, 0x40, 0x02, 0x40,
   0x01, 0x80, 0x01, 0x80, 0x01, 0x90, 0x01, 0x90, 0x02, 0x48, 0x02, 0x44,
   0x04, 0x23, 0x08, 0x10, 0x30, 0x0c, 0xc0, 0x03};

static unsigned char graystone_bits[] = {
   0xc0, 0x03, 0xb0, 0x0e, 0x58, 0x15, 0xac, 0x2a, 0x56, 0x55, 0xaa, 0x6a,
   0x55, 0xd5, 0xab, 0xaa, 0x55, 0xd5, 0xab, 0xba, 0x56, 0x5d, 0xaa, 0x6e,
   0x54, 0x37, 0xa8, 0x1a, 0x70, 0x0d, 0xc0, 0x03};

static unsigned char grid_bits[] = {
   0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
   0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
   0xff, 0xff, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
   0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00
};

static unsigned char wood_bits[] = {
	0xf6, 0xbb, 0x60, 	/* #### ## # ### ## ## */
	0xdf, 0xeb, 0xe0, 	/* ## ######## # ##### */
	0xbf, 0xbf, 0xe0, 	/* # ####### ######### */
	0xff, 0xfb, 0x60, 	/* ############# ## ## */
	0xdb, 0xee, 0xe0, 	/* ## ## ##### ### ### */
	0xfe, 0xff, 0xe0, 	/* ####### ########### */
	0xdf, 0xf6, 0xe0, 	/* ## ######### ## ### */
	0xfe, 0xfd, 0xe0, 	/* ####### ###### #### */
	0xdb, 0xb7, 0xe0, 	/* ## ## ### ## ###### */
	0xda, 0xff, 0xa0, 	/* ## ## # ######### # */
	0xff, 0xdd, 0xe0, 	/* ########## ### #### */
	0xaf, 0x7f, 0xa0, 	/* # # #### ######## # */
	0xff, 0xfd, 0xe0, 	/* ############## #### */
	0xd7, 0x7e, 0xe0, 	/* ## # ### ###### ### */
	0xff, 0xff, 0xe0, 	/* ################### */
	0x6b, 0x77, 0xe0, 	/*  ## # ## ### ###### */
	0xaf, 0xb7, 0x60, 	/* # # ##### ## ### ## */
	0xfd, 0xef, 0xe0, 	/* ###### #### ####### */
	0xfb, 0xbf, 0xe0  	/* ##### ### ######### */
} ;

bitmap_t blackstone(16, 16, blackstone_bits);
bitmap_t whitestone(16, 16, whitestone_bits);
bitmap_t grid(16, 16, grid_bits);
bitmap_t graystone(16, 16, graystone_bits);
bitmap_t blacksquare(16, 16, black_bits);
bitmap_t woodsquare(20, 20, wood_bits, LightGreen, Green);

void DrawBlack(int r, int c, int mode = (int)CopyPut);
void DrawWhite(int r, int c, int mode = (int)CopyPut);
void DrawEmpty(int r, int c, int mode = (int)CopyPut);
void ShowResult();
void ClearResult();

int Brd2Pxl(int bpos) // xlate board pos to pixel pos
{
	return bpos*20 + (vertices?2:12);
}

int Pxl2Brd(int ppos) // xlate pixel pos to board pos
{
	int rtn = ppos - (vertices?0:10);
	if (rtn<0) return -1;
	else return rtn/20;
}

static int newnest = 0; // hax!

int Cancel(void *arg);
int Confirm(void *arg);
int Pass(void *arg);
int DoUndo(void *arg);
//int Quit(void *arg);
int NewGame(void *arg = NULL);
int OptionMenu(void *arg);
int FileMenu(void *arg);
int AutoPlayMenu(void *arg);

void AutoMove()
{
	mouse.SetCursor(HourglassCursor);
	game->AutoMove();
	ShowResult();
	mouse.SetCursor(ArrowCursor);
}

class gamecanvas_t : public canvas_t
{
public:
	int boardedit;
	int bsize;
	gamecanvas_t(int left_in, int top_in, int bsize_in,
		unsigned long flags_in = 0l, color_t fillcolor_in = Black)
		: canvas_t(left_in, top_in,
			left_in+(bsize_in-vertices)*20+20,
			top_in+(bsize_in-vertices)*20+20, 
			flags_in, fillcolor_in)
	{
		bsize = bsize_in;
		boardedit = 0;
	}
	int HandleEvent(event_t *e);
};

int gamecanvas_t::HandleEvent(event_t *e)
{
	static int oldpiece = 0, oldpcr, oldpcc, 
			down = 0, startx, starty, oldx, oldy;
	int er, ec;
	int x, y; mouse.Position(x, y); x -= Left(); y -= Top();
	ViewPort();
	if (down && e->Type() == MouseMoved)
	{
			// move moving piece
			Blit(oldx-8, oldy-8, graystone, XOrPut);
			Blit(x-8, y-8, graystone, XOrPut);
			oldx = x; oldy = y;
	}
	else if (e->Type() == ButtonPressed && (e->Arg1()==0 || boardedit))
	{
		if (!down)
		{
			down = 1;
			startx = oldx = x;
			starty = oldy = y;
			// delete old piece, if any
			int sr = Pxl2Brd(starty);
			int sc = Pxl2Brd(startx);
			oldpiece = 0;
			if (!boardedit && game->ReplaceAllowed() && sc>=0
				&& sr>=0 && sc<bsize && sr<bsize)
			{
				oldpiece = (game->brd.get(sr,sc) == HUMAN);
				if (oldpiece)
					DrawEmpty(oldpcr = sr, oldpcc = sc);
			}
			// draw moving piece
			Blit(oldx-8, oldy-8, graystone, XOrPut);
		}
	}
	else if (e->Type() == ButtonReleased)
	{
		if (down)
		{
			int sr = Pxl2Brd(starty);
			int sc = Pxl2Brd(startx);
			int er = Pxl2Brd(y);
			int ec = Pxl2Brd(x);
			Blit(oldx-8, oldy-8, graystone, XOrPut);
			down = 0;
			if (er<0 || er>=bsize) er = -1;
			if (ec<0 || ec>=bsize) ec = -1;

			if (boardedit)
			{
				if (er>=0 && ec>=0)
				{
					int p = game->brd.get(er,ec);
					if (p) game->brd.set(er,ec,EMPTY);
					else if (e->Arg1()==0)
						game->brd.set(er,ec,HUMAN);
					else
						game->brd.set(er,ec,COMPUTER);
					game->brd.Print();
				}
			}
			else if (e->Arg1()==0)
			{
				char mvbuf[80];
				if (sr!=er || sc!=ec)
					if (er<0 || ec<0)
						sprintf(mvbuf, "%c%c ZZ", sr+'A',sc+'A');
					else
						sprintf(mvbuf, "%c%c %c%c",
							sr+'A',sc+'A',er+'A',ec+'A');
				else
					sprintf(mvbuf, "%c%c", sr+'A',sc+'A');
				if (game->HumanMove(mvbuf) == 0)
				{
					ShowResult();
					if (!game->GameOver() && game->passcnt<2)
					{
						AutoMove();
						ShowResult();
					}
					mouse.Flush();
				}
				else if (oldpiece) // restore piece
				{
					DrawWhite(oldpcr, oldpcc);
					oldpiece = 0;
				}
			}
		}
	}
	return 1;
}

class gameapp_t : public application_t
{
public:
	menu_t *menu;
	gamecanvas_t *canvas;
	canvas_t *reswin;
	canvas_t *infowin;
	gameapp_t(int bsize, int infolines, int menuentries);
	int Execute();
	void ShowInfo(int ln, char *msg);
	~gameapp_t()
	{
		assert(canvas && menu);
		delete menu;
		delete canvas;
		delete reswin;
		if (infowin) delete infowin;
	}
};

gameapp_t::gameapp_t(int bsize, int infolines, int menuentries)
	: application_t(Black)
{
	int wl = textwidth("MenuWidthXX");
	canvas = new gamecanvas_t(240, 1, bsize, W_BORDER);
	assert(canvas);
	int rtop = (menuentries+8)*20;
	reswin = new canvas_t(120-wl/2, rtop, 120+wl/2, rtop+20, W_BORDER);
	assert(reswin);
	if (infolines)
	{
		infowin = new canvas_t(2, rtop+40, 238, rtop+40+infolines*20, W_BORDER);
		assert(infowin);
		infowin->Foreground(Black);
		infowin->Background(Green);
		infowin->Clear();
	}
	else infowin = NULL;
	reswin->Foreground(White);
	reswin->Background(Black);
	reswin->Clear();
	canvas->Background(Green);
	canvas->Clear();
	canvas->Foreground(Black);
	canvas->Grid(10, 10, bsize-vertices, 20);
	canvas->Foreground(White);
	if (game->CanPass())
		menu = new menu_t(120-wl/2, 1, 5+menuentries,1);
	else
		menu = new menu_t(120-wl/2, 1, 4+menuentries,1);
	assert(menu);
	menu->AddEntry("File", FileMenu);
	menu->AddEntry("Undo", DoUndo);
	menu->AddEntry("Options", OptionMenu);
	menu->AddEntry("Autoplay", AutoPlayMenu);
	if (game->CanPass())
		menu->AddEntry("Pass", Pass);
	menu->Foreground(Black);
	menu->Background(Brown);
}

void gameapp_t::ShowInfo(int ln, char *msg)
{
	if (infowin)
	{
		infowin->Color(Green);
		infowin->FilledRectangle(10,5+20*ln,
			infowin->Width()-10,15+20*ln);
		infowin->Color(Black);
		infowin->Text(10,5+20*ln,msg);
	}
}

int gameapp_t::Execute()
{
	menu->Show();
	NewGame();
	while (GetEvent())
		if (HandleEvent()==0)
			break;
	return 0;
}

gameapp_t *app;

//--------------------------------------------------------------------
#ifdef DRAWTREE

extern int drawtree;
extern int Xpositions[];
extern void ShowBoard(const gridboard_t *b, int drawall);
int zoom = 1, displacex = 0, displacey = 0, xspace, yspace;

extern void DrawLink(int ypos, long px, long cx)
{
	int cy = (ypos+1) * yspace + displacey - 5;
	px = px * xspace/6 + (xspace-10)/2 + displacex;
	cx = cx * xspace/6 + (xspace-10)/2 + displacex;
	line(px,cy-(zoom>2? 30 : 50),cx,cy);
}

void DrawTreeNode(long x, int y, const board_t* &b, int val)
{
	x = x * xspace/6 + displacex;
	if (x>-100 && x<640)
	{
		gridboard_t *B = (gridboard_t *)b;
		int r, c;
		y = y * yspace + displacey;
		// draw micro board
		for (r = 0; r < B->Rows(); r++)
		{
			for (c = 0; c < B->Cols(); c++)
			{
				int p = B->get(r,c);
				int col = ( (p==EMPTY)?Green: (p==HUMAN?whitecolor:blackcolor));
				int xx = x+c*zoom;
				int yy = y+r*zoom;
				for (int dx=0; dx<zoom; dx++)
					for (int dy=0; dy<zoom; dy++)
						putpixel(xx+dx,yy+dy,col);
				if (col!=Green && zoom>2)
				{
					putpixel(xx,yy,Green);
					putpixel(xx+zoom-1,yy,Green);
					putpixel(xx,yy+zoom-1,Green);
					putpixel(xx+zoom-1,yy+zoom-1,Green);
				}
			}
		}
		// draw board outline
		setcolor(Red);
		r = B->Rows();
		c = B->Cols();
		line(x-1,y-1,x+c*zoom+1,y-1); // across top
		line(x-1,y+r*zoom+1,x+c*zoom+1,y+r*zoom+1); // down right
		line(x-1,y-1,x-1,y+r*zoom+1); // down left
		line(x+c*zoom+1,y-1,x+c*zoom+1,y+r*zoom+1); // across bottom
		// draw weight
		if (zoom>2)
		{
			char wbuf[10];
			sprintf(wbuf,"%d", val);
			outtextxy(x+c*zoom/2,y+r*zoom+10,wbuf);
		}
		setcolor(White); // ready for links
	}
}

int ShowTree(void *arg)
{
	int ch, oldsearch = showsearch;
	showsearch = 0;
	long max;
	(void)arg;
	mouse.Hide();
	displacex = displacey = 0;
	drawtree = 1;
	setviewport(0,0,getmaxx(), getmaxy(),1);
	setlinestyle(SolidLine,0,1);
	settextjustify(CENTER_TEXT,CENTER_TEXT);
	move_t *m = game->NewMove();
redo:
	cleardevice();
	xspace = game->brd.Cols()*zoom+10;
	yspace = game->brd.Rows()*zoom+60;
	(void)game->FindMove(game->level, game->movenum, game->nextplayer,
		(const board_t* &)game->b, m, UNKNOWN);
nextkey:
	switch(getch())
	{
		case '+':	zoom++; break;
		case '-':	if (zoom) zoom--;
				else goto nextkey;
				break;
		case 'R':	displacex -= 320; break;
		case 'L':	displacex += 320; break;
		case 'U':	displacey -= 200; break;
		case 'D':	displacey += 200; break;
		case 'r':	displacex -= 64; break;
		case 'l':	displacex += 64; break;
		case 'u':	displacey -= 40; break;
		case 'd':	displacey += 40; break;
		case 27:	goto done;
		default:	goto nextkey;
	}
	goto redo;
done:
	delete m;
	settextjustify(LEFT_TEXT,TOP_TEXT);
	cleardevice();
	drawtree = 0;
	app->menu->Show();
	ShowResult();
	app->canvas->Clear();
	app->canvas->Foreground(Black);
	app->canvas->Grid(10, 10, game->brd.Cols()-vertices, 20);
	ShowBoard((const gridboard_t *)game->b, 1);
	if (app->infowin) app->infowin->Clear();
	mouse.Show();
	showsearch = oldsearch;
	return 0;
}

#endif

//--------------------------------------------------------------------
// Menu item handlers

int SetLevel(void *arg)
{
	game->level = (int)arg;
	return 0;
}

int SetLevelMenu(void *arg)
{
	(void)arg;
	menu_t *m = new menu_t(150, 50, 6);
	assert(m);
	m->AddEntry("One", SetLevel, (void *)1);
	m->AddEntry("Two", SetLevel, (void *)2);
	m->AddEntry("Three", SetLevel, (void *)3);
	m->AddEntry("Four", SetLevel, (void *)4);
	m->AddEntry("Five", SetLevel, (void *)5);
	m->AddEntry("Six", SetLevel, (void *)6);
	m->Foreground(Black);
	m->Background(Blue);
	m->Execute();
	delete m;
	return 0;
}

int ToggleEdit(void *arg)
{
	(void)arg;
	app->canvas->boardedit ^= 1;
	return 0;
}

int ToggleSearch(void *arg)
{
	(void)arg;
	showsearch ^= 1;
	return 0;
}

int OptionMenu(void *arg)
{
	(void)arg;
#ifdef DRAWTREE
	menu_t *m = new menu_t(60, 65, optionentries+4);
#else
	menu_t *m = new menu_t(60, 65, optionentries+3);
#endif
	assert(m);
	m->AddEntry("Level", SetLevelMenu);
	m->AddEntry(app->canvas->boardedit ? "Resume Play" : "Edit Board",
		ToggleEdit);
	m->AddEntry(showsearch ? "Hide Search" : "Show Search",
		ToggleSearch);
#ifdef DRAWTREE
	m->AddEntry("Show Tree", ShowTree);
#endif
	if (optionentries)
		game->AddOptions((void *)m);
	m->Foreground(Black);
	m->Background(Blue);
	m->Execute();
	delete m;
	return 1;
}

int AutoPlay()
{
	while (game->autoplay>0 && !game->GameOver())
	{
		game->autoplay--;
		AutoMove();
		while (app->GotEvent())
		{
			if (app->GetEvent()==0 || app->HandleEvent()==0)
				goto done;
		}
	}
done:
	if (!game->GameOver() && game->nextplayer == COMPUTER)
		AutoMove();
	ShowResult();
	return 1;
}

int NewGame(void *arg)
{
	int rtn;
	if (newnest) return 0; // avoid recursive invocations!
	newnest++;
	(void)arg;
	game->Initialise();
	ClearResult();
	game->brd.Print();
	if (game->autoplay)
		(void)AutoPlayMenu(NULL);
	else if (!game->GameOver() && game->nextplayer == COMPUTER)
		AutoMove();
	newnest--;
	return 0;
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

int Exit(void *arg)
{
	(void)arg;
	return 0;
}

int AutoPlayMenu(void *arg)
{
	menu_t *m = new menu_t(1, 20, 1);
	assert(m);
	m->AddEntry("Stop Autoplay", Exit, arg);
	m->Foreground(Black);
	m->Background(Red);
	m->Show();
	WindowManager.SetFocus(m->Handle());
	if (game->autoplay==0)
		game->autoplay = 500;
	(void)AutoPlay();
	WindowManager.ReleaseFocus(m->Handle());
	game->autoplay = 0;
	delete m;
	return 1;
}

static int quit = 0;

int Quit(void *arg)
{
	int cancelled = 1;
	(void)arg;
	menu_t *m = new menu_t(150, 1, 2);
	assert(m);
	m->AddEntry("Confirm", Confirm, (void *)&cancelled);
	m->AddEntry("Cancel", Cancel, (void *)&cancelled);
	m->Foreground(Black);
	m->Background(Red);
	m->Execute();
	delete m;
	if (!cancelled) quit = 1;
	return 0;
}

int Pass(void *arg)
{
	(void)arg;
	game->HandlePass();
	if (game->passcnt < 2 && !game->GameOver())
		AutoMove();
	ShowResult();
	return 1;
}

int DoUndo(void *arg)
{
	(void)arg;
	if (game->movenum>2 && !game->GameOver())
	{
		game->game_t::Undo();
		game->game_t::Undo();
	}
	ShowResult();
	return 1;
}

int DoSave(void *arg)
{
	(void)arg;
	FILE *fp = fopen("saved.gam", "w");
	game->SaveGame(fp);
	fclose(fp);
	return 0;
}

int DoLoad(void *arg)
{
	(void)arg;
	ClearResult();
	FILE *fp = fopen("saved.gam", "r");
	game->LoadGame(fp);
	ShowResult();
	fclose(fp);
	return 0;
}

int FileMenu(void *arg)
{
	(void)arg;
	menu_t *m = new menu_t(140, 1, 4);
	assert(m);
	m->AddEntry("New", NewGame);
	m->AddEntry("Load", DoLoad);
	m->AddEntry("Save", DoSave);
	m->AddEntry("Quit", Quit);
	m->Foreground(Black);
	m->Background(Blue);
	m->Execute();
	delete m;
	return (quit==0);
}

//--------------------------------------------------------------------

void DrawBlack(int r, int c, int mode)
{
	if (mode == XOrPut)
		app->canvas->Blit(Brd2Pxl(c), Brd2Pxl(r), blackstone, (paintmode_t)mode);
	else
		app->canvas->Button(Brd2Pxl(c)+8,Brd2Pxl(r)+8,9,blackcolor,whitecolor);
}

void DrawWhite(int r, int c, int mode)
{
	if (mode == XOrPut)
		app->canvas->Blit(Brd2Pxl(c), Brd2Pxl(r), whitestone, (paintmode_t)mode);
	else
		app->canvas->Button(Brd2Pxl(c)+8,Brd2Pxl(r)+8,9,whitecolor,blackcolor);
}

void DrawGray(int r, int c, int mode, int dark)
{
	if (mode == XOrPut)
		app->canvas->Blit(Brd2Pxl(c), Brd2Pxl(r), graystone, (paintmode_t)mode);
	else
		app->canvas->Button(Brd2Pxl(c)+8,Brd2Pxl(r)+8,9,
			(dark ^ (whitecolor==Black)) ?DarkGray:LightGray,
			blackcolor);
}

void DrawEmpty(int r, int c, int mode)
{
	assert(mode == CopyPut);
	int cp = Brd2Pxl(c)+8;
	int rp = Brd2Pxl(r)+8;
	color_t fg = app->canvas->Foreground();
#if 1
	app->canvas->Color(app->canvas->Background());
	app->canvas->FilledRectangle(cp-9,rp-9,cp+9,rp+9);
#else
	app->canvas->Blit(cp-9,rp-9,woodsquare, (paintmode_t)mode);
#endif
	if (vertices) // redraw grid
	{
		app->canvas->Color(Black);
		if (c>0) // draw left line
			app->canvas->Line(cp-10, rp, cp, rp);
		if (c < (app->canvas->bsize-1)) // draw right line
			app->canvas->Line(cp, rp, cp+10, rp);
		if (r>0) // draw up line
			app->canvas->Line(cp, rp-10, cp, rp);
		if (r < (app->canvas->bsize-1)) // draw down line
			app->canvas->Line(cp, rp, cp, rp+10);
	}
	app->canvas->Foreground(fg);
}

// hax for go atari

void MarkButton(int r, int c, color_t color)
{
	color_t fg = app->canvas->Foreground();
	int cp = Brd2Pxl(c)+8;
	int rp = Brd2Pxl(r)+8;
	app->canvas->Color(color);
	app->canvas->Line(cp-4,rp-4,cp+4,rp+4);
	app->canvas->Line(cp-4,rp+4,cp+4,rp-4);
	app->canvas->Foreground(fg);
}

void MarkWhite(int r, int c)
{
	MarkButton(r, c, whitecolor);
}

void MarkBlack(int r, int c)
{
	MarkButton(r, c, blackcolor);
}

void LabelButton(int r, int c, int v, color_t color)
{
	color_t fg = app->canvas->Foreground();
	char cnt[4];
	sprintf(cnt,"%d",v);
	app->canvas->Color(color);
	int cp = Brd2Pxl(c)+8 - textwidth(cnt)/2;
	int rp = Brd2Pxl(r)+4;
	app->canvas->Text(cp,rp,cnt);
	app->canvas->Foreground(fg);
}

void ShowMessage(char *msg)
{
	app->reswin->Clear();
	app->reswin->Text(4,4,msg);
}

void ShowResult()
{
	app->reswin->Clear();
	if (game->Win())
		app->reswin->Text(4,4,"I win!");
	else if (game->Lose())
		app->reswin->Text(4,4,"You win!");
	else if (game->Draw())
		app->reswin->Text(4,4,"We drew");
	else
	{
		char buf[10];
		sprintf(buf,"Move %d", game->movenum);
	     	app->reswin->Text(4,4,buf);
	}
}

void ClearResult()
{
	app->reswin->Clear();
}

void ShowInfo(int ln, char *msg)
{
	app->ShowInfo(ln, msg);
}

int main(int argc, char *argv[])
{
	int sz = 3;
	INITDEBUG
	if (argc>=2 && isdigit(argv[argc-1][0]))
	{
		argc--;
		sz = atoi(argv[argc]);
	}
	int infolines, menuentries;
	game = (gridgame_t *)Game(sz, vertices, argc, argv,
			infolines, menuentries);
	if (game->userblack)
	{
		whitecolor=Black;
		blackcolor=White;
	}
	app = new gameapp_t(game->brd.Cols(), infolines, menuentries);
	assert(app);
	if (menuentries)
		game->AddEntries((void *)app->menu);
	int rtn = app->Execute();
	delete app;
	delete game;
	return rtn;
}

