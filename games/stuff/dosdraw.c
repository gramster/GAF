#include <dos.h>

gridboard_t *shown;

void beep(int freq, int duration)
{
	long count = 1193280l/(long)freq;
	outportb(67,182);
	outportb(66,(char)(count%256));
	outportb(66,(char)(count/256));
	outportb(97,inportb(97)|3);
	delay(duration);
	outportb(97,inportb(97)&0xFC);
}

#ifdef USE_GAF

#include "window.h"

// hackish just to avoid a #include

extern void DrawBlack(int r, int c, int mode = 0);
extern void DrawWhite(int r, int c, int mode = 0);
extern void DrawEmpty(int r, int c, int mode = 0);

void ShowBoard(const gridboard_t *b, int drawall)
{
	int r, c;
	if (shown==NULL)
		shown = new gridboard_t(b->Rows(), b->Cols());
	for (r=0;r<b->Rows();r++)
	{
		for (c=0;c<b->Cols();c++)
		{
			if (drawall || b->get(r,c) != shown->get(r,c))
			{
				switch(b->get(r,c))
				{
					case EMPTY:
						DrawEmpty(r,c);
						break;
					case HUMAN:
						DrawWhite(r,c);
						break;
					case COMPUTER:
						DrawBlack(r,c);
						break;
				}
				shown->set(r,c,b->get(r,c));
			}
		}
	}
}

#else

#include <conio.h>

#define BOARDATTR	0x71	/* blue on white */

union REGS inregs, outregs;

/* Put character and attribute at screen cursor position */

void put_screen_ch(int c)
{
	inregs.h.al = (char)c;
	inregs.h.bl = (char)BOARDATTR;
	inregs.x.cx = 1;
	inregs.h.ah=9;
	int86(16, &inregs, &outregs);
}

#define putboard(r,c,ch) gotoxy(c,r); put_screen_ch(ch);

void putline(int w, int r,int left, int right, int mid, int join)
{
	int c;
	putboard(r,10,left);
	for (c=1;c<w;c++)
	{
		putboard(r,7+4*c,mid);
		putboard(r,8+4*c,mid);
		putboard(r,9+4*c,mid);
		putboard(r,10+4*c,join);
	}
	putboard(r,7+4*c,mid);
	putboard(r,8+4*c,mid);
	putboard(r,9+4*c,mid);
	putboard(r,10+4*c,right);
}

void ShowGrid(int rows, int cols)
{
	int r;
	clrscr();
	putline(cols, 1,201,187,205,209);
	for (r=0;;r++)
	{
		putline(cols, 2+r*2,186,186,/*219*/' ',179);
		if (r==(rows-1)) break;
		putline(cols, 3+r*2,199,182,196,197);
	}
	putline(cols, 3+r*2,200,188,205,207);
}

void ShowBoard(const gridboard_t *b)
{
	if (shown==NULL)
	{
		ShowGrid(b->Rows(), b->Cols());
		shown = new gridboard_t(b->Rows(), b->Cols());
	}
	int r, c;
	for (r=0;r<b->Rows();r++)
	{
		for (c=0;c<b->Cols();c++)
		{
			if (b->get(r,c) != shown->get(r,c))
			{
				gotoxy(11+4*c, 2+2*r);
				put_screen_ch(' ');
				gotoxy(12+4*c, 2+2*r);
				switch(b->get(r,c))
				{
					case EMPTY:
						put_screen_ch(' ');
						break;
					case HUMAN:
						put_screen_ch('O');
						break;
					case COMPUTER:
						put_screen_ch('X');
						break;
				}
				gotoxy(13+4*c, 2+2*r);
				put_screen_ch(' ');
				shown->set(r,c,b->get(r,c));
			}
		}
	}
}

#endif

