// Classes for games based on square/rectangular grids, with
// one piece per square, and placement or replacement moves

#include <stdlib.h>
#include <assert.h>
#include "gridgame.h"

#if __BORLANDC__
#include <dos.h>

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
#endif

// hackish just to avoid a #include

extern void DrawBlack(int r, int c, int mode = 0);
extern void DrawWhite(int r, int c, int mode = 0);
extern void DrawGray(int r, int c, int mode = 0, int dark = 0);
extern void DrawEmpty(int r, int c, int mode = 0);

void ShowNode(int r, int c, int p, int gray)
{
	if (p && gray)
		DrawGray(r,c,0,(p==COMPUTER));
	else switch(p)
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
		default: 
			DrawGray(r,c);
			break;
	}
}

// ShowBoard does two passes, allowing it to both optimise
// the display and to show pieces changed since last move in gray.

static gridboard_t *shown = NULL, *lastshown = NULL;

void FreeBoards()
{
	if (shown) delete shown;
	if (lastshown) delete lastshown;
	shown = lastshown = NULL;
}

void ShowBoard(const gridboard_t *b, int drawall)
{
	int r, c;
	if (shown==NULL)
	{
		shown = new gridboard_t(b->Rows(), b->Cols());
		atexit(FreeBoards);
	}
	assert(shown);
	if (lastshown)
	{
		for (r=0;r<b->Rows();r++)
		{
			for (c=0;c<b->Cols();c++)
			{
				int p = lastshown->get(r,c);
				if (drawall || shown->get(r,c) != p)
				{
					ShowNode(r,c,p,0);
					shown->set(r,c,p);
				}
			}
		}
	}
	else lastshown = new gridboard_t(b->Rows(), b->Cols());
	assert(lastshown);
	for (r=0;r<b->Rows();r++)
	{
		for (c=0;c<b->Cols();c++)
		{
			int p = b->get(r,c);
			if (lastshown->get(r,c) != p)
			{
				ShowNode(r,c,p,1);
				lastshown->set(r,c,p);
			}
		}
	}
}

//-------------------------------------------------------------------
// Grid board class

void gridboard_t::Create(int rows_in, int cols_in)
{
#ifdef BITVECTOR
	dim = (rows_in*cols_in+15)/16;
	is_empty = new unsigned short[dim];
	assert(is_empty);
	is_human = new unsigned short[dim];
	assert(is_human);
#else
	dim = rows_in*cols_in;
	brd = new unsigned char[dim];
	assert(brd);
#endif
	rows = rows_in;
	cols = cols_in;
	size = rows*cols;
}

gridboard_t::gridboard_t(int rows_in, int cols_in) // basically 2 2d bit vectors
{
	Create(rows_in, cols_in);
	Empty();
}

void gridboard_t::SetAll(int p)
{
	for (int i = 0; i < dim; i++)
	{
#ifdef BITVECTOR
		is_empty[i] = p ? 0 : 0xFFFF;
		is_human[i] = (p==HUMAN) ? 0xFFFF : 0;
#else
		brd[i] = p;
#endif
	}
}

void gridboard_t::Copy(gridboard_t* b)
{
	for (int i = 0; i < dim; i++)
	{
#ifdef BITVECTOR
		is_empty[i] = b->is_empty[i];
		is_human[i] = b->is_human[i];
#else
		brd[i] = b->brd[i];
#endif
	}
}

gridboard_t::gridboard_t(gridboard_t &b)
{
	Create(b.rows, b.cols);
	Copy(&b);
}

gridboard_t::gridboard_t(board_t* &b_in)
{
	gridboard_t *b = (gridboard_t *)b_in;
	Create(b->rows, b->cols);
	Copy(b);
}

gridboard_t::~gridboard_t()
{
#ifdef BITVECTOR
	delete [dim] is_empty;
	delete [dim] is_human;
#else
	delete [dim] brd;
#endif
}

#ifdef BITVECTOR
void gridboard_t::set(int o, int p)
{
	int e = o/16, b = o%16;
	if (p==EMPTY)
		is_empty[e] |= (1 << b);
	else
	{
		is_empty[e] &= ~(1 << b);
		if (p==HUMAN)
			is_human[e] |= 1 << b;
		else 
			is_human[e] &= ~(1 << b);
	}
}
#endif

//-------------------------------------------------------------
// These should never need changing

int gridboard_t::full() const
{
	return (Count(EMPTY)==0);
}

void gridboard_t::Print(FILE *fp) const
{
	int r, c;
//	static int first = 1; // only if we have a bitmap for empty square
	static int first = 0;
	if (fp == stdout)
	{
		ShowBoard(this, first);
		first = 0;
	}
	else for (r = 0; r<rows; r++)
	{
		for (c = 0; c<cols; c++)
		{
			switch (get(r,c))
			{
			case EMPTY: fputc('.',fp); break;
			case COMPUTER: fputc('X',fp); break;
			case HUMAN: fputc('O',fp); break;
			}
		}
		fputc('\n', fp);
	}
}

int gridboard_t::Count(int type) const
{
	int r, c, rtn = 0;
	for (r = 0; r<rows; r++)
		for (c = 0; c<cols; c++)
			if (get(r,c)==type)
				rtn++;
	return rtn;
}

void gridboard_t::Count(int &empty, int &human, int &computer) const
{
	empty = human = computer = 0;
	for (int r = 0; r<rows; r++)
		for (int c = 0; c<cols; c++)
			switch(get(r,c))
			{
				case EMPTY:
					empty++; break;
				case HUMAN:
					human++; break;
				case COMPUTER:
					computer++; break;
			}
}

int gridboard_t::CountRow(int r, int type) const
{
	int rtn = 0;
	for (int c = 0; c<cols; c++)
		if (get(r,c)==type)
			rtn++;
	return rtn;
}

void gridboard_t::CountRow(int r, int &empty, int &human, int &computer) const
{
	empty = human = computer = 0;
	for (int c = 0; c<cols; c++)
		switch(get(r,c))
		{
			case EMPTY:
				empty++; break;
			case HUMAN:
				human++; break;
			case COMPUTER:
				computer++; break;
		}
}


int gridboard_t::GetTestLayout()
{
	char buf[80];
	puts("Layout?");
	scanf("%s",buf);
	if (buf[0]=='q') return 0;
	for (int p = 0; p<Size(); p++)
		set(p, (int)(buf[p]-'0'));
	Print(stderr);
	return 1;
}

