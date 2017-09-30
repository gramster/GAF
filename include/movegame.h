#ifndef __MOVEGAM_H
#define __MOVEGAM_H

// Replacement games a la hexapawn
// Require Validate, Init, Generator and Weight methods.

#include <assert.h>
#include "gridgame.h"

//---------------------------------------------------------------------
// board moves

class boardmove_t : public move_t
{
public:
	short from, to;
	boardmove_t(short from_in = 0, short to_in = 0)
	{
		from = from_in;
		to = to_in;
	}
	~boardmove_t()
	{ }
	void Print(FILE *fp = stdout)
	{
		if (from<0)
			fprintf(fp, "move to %d\n", to);
		else if (to<0)
			fprintf(fp, "move off from %d\n", from);
		else
			fprintf(fp, "move from %d to %d\n", from, to);
	}
	virtual void Write(FILE *fp)
	{
		fprintf(fp, "%d %d ", from, to);
	}
	virtual int Read(FILE *fp)
	{
		return (fscanf(fp, "%d%d", &from, &to)==2);
	}
};

//-----------------------------------------------------------------
// placement move (to x) generator

class placegen_t : public generator_t
{
	int pos;
public:
	placegen_t(const board_t* &b_in, int movenum_in, int player_in)
		: generator_t(b_in, movenum_in, player_in)
	{
		pos = -1;
	}
	inline int Get(int o) const
	{
		return ((gridboard_t *)b)->get(o);
	}
	move_t *NextMove();
};

//-----------------------------------------------------------------
// Move from x to y move generators

class movegen_t : public generator_t
{
public:
	int from, to, cnt, sz, cols;
	movegen_t(const board_t* &b_in, int movenum_in, int player_in)
		: generator_t(b_in, movenum_in, player_in)
	{
		from = cnt = 0;
		sz = ((gridboard_t *)b_in)->Size();
		cols = ((gridboard_t *)b_in)->Cols();
		to = ((player==HUMAN) ? -cols : cols) - 1;
	}
	inline int Get(int o) const
	{
		return ((gridboard_t *)b)->get(o);
	}
	virtual move_t *NextMove() = 0;
};

//-----------------------------------------------------------------
// Pawn move generator (with taking)

class pawngen_t : public movegen_t
{
public:
	pawngen_t(const board_t* &b_in, int movenum_in, int player_in)
		: movegen_t(b_in, movenum_in, player_in)
	{ }
	virtual move_t *NextMove();
};

// Validate pawn moves

int CheckPawnMove(const board_t* &bd, int player, int from, int to);
int CheckPawnMove(const board_t* &bd, int player, move_t *m);

// Orthogonal move to empty square move generator

int CheckOrthoMove(const board_t* &bd, int player, int from, int to);
int CheckOrthoMove(const board_t* &bd, int player, move_t *m);

class orthogen_t : public movegen_t
{
	int lim; // number of moves checked per piece; usually 4 unless xtra
public:
	orthogen_t(const board_t* &b_in, int movenum_in, int player_in, int xtra = 0)
		: movegen_t(b_in, movenum_in, player_in)
	{ lim = xtra ? 5 : 4; }
	virtual move_t *NextMove();
	virtual int Validate(int &from, int &to); // fails xtra moves
};

// Validate king moves

int CheckKingMove(const board_t* &bd, int player, int from, int to);
int CheckKingMove(const board_t* &bd, int player, move_t *m);

// King move to empty square generator

class kinggen_t : public movegen_t
{
public:
	kinggen_t(const board_t* &b_in, int movenum_in, int player_in)
		: movegen_t(b_in, movenum_in, player_in)
	{ }
	virtual move_t *NextMove();
};

//---------------------------------------------------------------------
// Simple board games

class movegame_t : public gridgame_t
{
public:
	movegame_t(int bsize, int first, int argc = 0, char **argv = NULL,
		int placeonly_in = 0)
		: gridgame_t(bsize, first, argc, argv, placeonly_in)
	{ }
	virtual void CopyMove(move_t* &dest, move_t* &src)
	{
		((boardmove_t *)dest)->from = ((boardmove_t *)src)->from;
		((boardmove_t *)dest)->to = ((boardmove_t *)src)->to;
	}
	virtual move_t *NewMove(move_t *m = NULL)
	{
		move_t *rtn = new boardmove_t;
		assert(rtn);
		if (m) CopyMove(rtn, m);
		return rtn;
	}
	virtual move_t *TranslateMove(char *buf);
	virtual int ApplyMove(board_t* &b_in, move_t *m, int mvnum,
		int player, moverecord_t **mr)
	{
		gridboard_t *B = (gridboard_t *)b_in;
		int from = ((boardmove_t *)m)->from;
		if (from>=0 && from<B->Size())
			B->set(from, EMPTY);
		int to = ((boardmove_t *)m)->to;
		if (to>=0 && to<B->Size())
			B->set(to, player);
		moverecord_t *tmpmr = RecordMove(mvnum, NewBoard((const board_t *)b_in), NewMove(m));
		if (mr) *mr = tmpmr;
		return 1;
	}
	virtual int Validate(const board_t* &bd, int player, move_t *m)
	{
		int from, to;
		return Validate(bd, player, from = ((boardmove_t *)m)->from,
					to = ((boardmove_t *)m)->to);
	}
	virtual void Init()
	{
		gridgame_t::Init();
	}
	virtual void Useage(const char *genericargs) = 0;
	virtual int Validate(const board_t* &bd, int player, int &from, int &to) = 0;
	virtual generator_t *Generator(const board_t* &b_in, int movenum_in,
		int player_in, generator_t *pgen = NULL) = 0;
	virtual int Weight(const board_t* &b_in, int player) = 0;
};

//---------------------------------------------------------------------
// Placement game (e.g. tic-tac-toe)

class placegame_t : public movegame_t
{
public:
	placegame_t(int bsize, int first, int argc = 0, char **argv = NULL)
		: movegame_t(bsize, first, argc, argv, 1)
	{ }
	virtual int Validate(const board_t* &bd, int player, int &from, int &to)
	{
#ifdef USE_GAF
		from = -1;
#else
		assert(from==-1);
#endif
		return (to>=0 && to<((gridboard_t *)bd)->Size() &&
			((gridboard_t *)bd)->get(to) == EMPTY);
	}
	virtual generator_t *Generator(const board_t* &b_in, int movenum_in,
		int player_in, generator_t *pgen = NULL)
	{
		(void)pgen;
		generator_t *rtn =
			(generator_t *) (new placegen_t(b_in, movenum_in, player_in));
		assert(rtn);
		return rtn;
	}
	virtual void Useage(const char *genericargs) = 0;
	virtual int Weight(const board_t* &b_in, int player) = 0;
};

//---------------------------------------------------------------------
// Pawn games

#define CANADVANCE	1
#define CANTAKE		2
#define CANMOVE	(CANADVANCE|CANTAKE)

class pawngame_t : public movegame_t
{
public:
	pawngame_t(int bsize, int first, int argc = 0, char **argv = NULL)
		: movegame_t(bsize, first, argc, argv, 0)
	{ }
	int LocatePawn(const board_t* &b, int player, int flags);
	virtual generator_t *Generator(const board_t* &b_in, int movenum_in,
		int player_in, generator_t *pgen = NULL)
	{
		(void)pgen;
		generator_t *rtn =
			(generator_t *) (new pawngen_t(b_in, movenum_in, player_in));
		assert(rtn);
		return rtn;
	}
	virtual int Validate(const board_t* &bd, int player, int &from, int &to)
	{
		return CheckPawnMove(bd, player, from, to);
	}
	virtual void Useage(const char *genericargs) = 0;
	virtual void Init() = 0;
	virtual int Weight(const board_t* &b_in, int player) = 0;
};

#endif


