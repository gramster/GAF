#ifndef __GRIDGAME_H
#define __GRIDGAME_H

#include <stdio.h>
#include "game.h"

#define EMPTY		0
#define BITVECTOR // space-efficient boards

// we use shorts for MS-DOS; more efficient in 16-bit OS

class gridboard_t : public board_t
{
	int rows;
	int cols;
	int dim, size;
#ifdef BITVECTOR
	unsigned short *is_empty;
	unsigned short *is_human;
#else
	unsigned char *brd;
#endif
	void Create(int rows_in, int cols_in);
public:
	void Copy(gridboard_t* b);
	gridboard_t(int rows_in, int cols_in);
	gridboard_t(gridboard_t &b);
	gridboard_t(board_t* &b);
	virtual ~gridboard_t();
	gridboard_t& operator=(const gridboard_t& rhs)
	{
		if (this != &rhs)
		{
			assert(rows == rhs.rows && cols == rhs.cols);
			Copy((gridboard_t *)&rhs);
		}
		return *this;
	}
	void SetAll(int p);
	inline void Empty()
		{ SetAll(EMPTY); }
#ifdef BITVECTOR
	void set(int o, int p);
#else
	void set(int o, int p)
	{
		brd[o] = p;
	}
#endif
	inline void set(int r, int c, int p)
	{
		set(r*cols+c, p);
	}
	inline int get(int o) const
	{
#ifdef BITVECTOR
		// This routine gets called more than any, so it is inline and
		// a bit hackish for speed reasons
		register int e = o>>4, b = o&0xF;
		return ((is_empty[e]>>b) & 1) ? 0 : (1 + ((is_human[e]>>b)&1));
#else
		return brd[o];
#endif
	}
	inline int get(int r, int c) const
	{
		return get(r*cols+c);
	}
	int full() const;
	inline int Rows() const		{ return rows;						}
	inline int Cols() const		{ return cols;						}
	inline int Size() const		{ return size;						}
	inline int inrow(int r)		{ return ((unsigned)r)<rows;		}
	inline int incol(int c)		{ return ((unsigned)c)<cols;		}
	inline int inpos(int r, int c)	{ return (inrow(r) && incol(c));	}
	inline int inpos(int p)		{ return ((unsigned)p)<size;		}
	int Count(int type) const;
	void Count(int &empty, int &human, int &computer) const;
	int CountRow(int r, int type) const;
	void CountRow(int r, int &empty, int &human, int &computer) const;
	void Print(FILE *fp = stdout) const;
	GetTestLayout();
};

//------------------------------------------------------------------------

class gridgame_t : public game_t
{
	int placeonly;
public:
	gridboard_t brd;
	gridgame_t(int bsize, int first, int argc = 0, char **argv = NULL,
		int placeonly_in = 0)
		: brd(bsize,bsize)
		, game_t(&brd, first, argc, argv)
	{
		placeonly = placeonly_in;
	}
	int Weight(board_t* &b_in, int player)
	{
		return Weight((const board_t* &)b_in, player);
	}
	virtual int ApplyMove(move_t *m, int movenum,
		int player, moverecord_t **mr)
	{
#ifndef __BORLANDC__
		printf("%s ", (player == HUMAN) ? "You" : "I");
		m->Print();
#endif
#ifdef DEBUG
		if (debug)
		{
		    fprintf(debug, "%s ", (player == HUMAN) ? "You" : "I");
			m->Print(debug);
		}
#endif
		return ApplyMove(b, m, movenum, player, mr);
	}
	virtual void Undo(board_t *b_in, move_t *m_in) // undo last move
	{
		brd = *((gridboard_t *)b_in);
		(void)m_in;
		brd.Print();
	}
	virtual board_t* NewBoard(const board_t* &b_in)
	{
		board_t *rtn = (board_t *)new gridboard_t((board_t *)b_in);
		assert(rtn);
		return rtn;
	}
	inline void PrintMove(move_t* &m, char *msg = "", FILE *fp = stdout)
	{
		fputs(msg, fp);
		fputc(' ', fp);
		m->Print(fp);
	}
	inline int GameOver()
	{
		int w = Weight((const board_t*&)b, nextplayer);
		return ((w<=FIRSTLOSE || w>=FIRSTWIN || w==DRAW) && w!=UNKNOWN);
	}
	inline int ReplaceAllowed()
	{
		return !placeonly; // distinguishes place games (go/othello/tictactoe)
				// from slide games (hexapawn, dodgem) and combo games (ovid)
	}
	virtual void Init()
	{
		brd.Empty();
	}
	virtual void Useage(const char *genericargs) = 0;
	virtual generator_t *Generator(const board_t* &b_in,
		int movenum_in, int player_in, generator_t *pgen_in = NULL) = 0;
	virtual move_t *NewMove(move_t *m = NULL) = 0;
	virtual void CopyMove(move_t* &dest, move_t* &src) = 0;
	virtual int  ApplyMove(board_t* &b_in, move_t *m, int mvnum,
		int player, moverecord_t **mr = NULL) = 0;
	virtual move_t *TranslateMove(char *buf) = 0;
	virtual int Weight(const board_t* &b_in, int player) = 0;
};

#endif

