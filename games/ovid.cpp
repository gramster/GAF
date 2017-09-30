#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <graphics.h>
#include "movegame.h"

// Ovid's Game. Uncomment either of these for variations

//#define MORRIS	// can do kings moves
#define LES_PENDUS	// can pick up and replace. No generator yet!

// Modified placegen generator to create boardmoves.

class oplacegen_t : public movegen_t
{
public:
	oplacegen_t(const board_t* &b_in, int movenum_in, int player_in)
		: movegen_t(b_in, movenum_in, player_in)
	{ to = 0; }
	move_t *NextMove();
};

move_t *oplacegen_t::NextMove()
{
	while (to < ((gridboard_t *)b)->Size())
	{
		if (Get(to) == EMPTY)
		{
			move_t *rtn = (move_t *)new boardmove_t(-1, to++);
			assert(rtn);
			return rtn;
		}
		to++;
	}
	return NULL;
}

class ovid_t : public movegame_t
{
public:
	ovid_t(int bsize, int first, int argc = 0, char **argv = NULL)
		: movegame_t(bsize, first, argc, argv)
	{ }
	void Init()
	{
		movegame_t::Init();
	}
	generator_t *Generator(const board_t* &b_in, int movenum_in,
		int player_in, generator_t *pgen)
	{
		(void)pgen;
		generator_t *rtn;
		gridboard_t *B = (gridboard_t *)b_in;
		if (B->Count(EMPTY) == ( B->Size() - 2 * B->Cols() ))
#ifdef MORRIS
			rtn = (generator_t *) (new kinggen_t(b_in, movenum_in, player_in));
#else
			rtn = (generator_t *) (new orthogen_t(b_in, movenum_in, player_in));
#endif
		else
			rtn = (generator_t *) (new oplacegen_t(b_in, movenum_in, player_in));
		assert(rtn);
		return rtn;
	}
	int Validate(const board_t* &bd, int player, int &from, int &to);
	int Weight(const board_t* &b_in, int player);
	void Useage(const char *genargs)
	{
		closegraph();
		fprintf(stderr,"Useage: ovid %s [<size>]\n", genargs);
		exit(0);
	}
};

// Trivial evaluator, same as tic-tac-toe but no draw

int ovid_t::Weight(const board_t* &b, int player)
{
	gridboard_t *B = (gridboard_t *)b;
	(void)player; // has no influence in xxx
	for (int p = 1; p<=2; p++)
	{
		int r, c;
		// check for win across
		for (r=0; r<B->Rows(); r++)
		{
			for (c=0; c<B->Cols(); c++)
				if (B->get(r,c)!=p)
					break;
			if (c==B->Cols()) goto win;
		}
		// check for win down
		for (c=0; c<B->Cols(); c++)
		{
			for (r=0; r<B->Rows(); r++)
				if (B->get(r,c)!=p)
					break;
			if (r==B->Rows()) goto win;
		}
		// check for diagonal win
		for (r=0; r<B->Rows(); r++)
			if (B->get(r,r)!=p)
				break;
		if (r==B->Rows()) goto win;
		for (r=0; r<B->Rows(); r++)
			if (B->get(r,B->Cols()-r-1)!=p)
				break;
		if (r==B->Rows()) goto win;
	}
	return EVEN;
win:
	return (p==COMPUTER) ? MAXWEIGHT : MINWEIGHT;
}

int ovid_t::Validate(const board_t* &bd, int player, int &from, int &to)
{
	gridboard_t *B = (gridboard_t *)bd;
	int cnt;
	if (to<0 || to>=B->Size() || from>=B->Size()|| B->get(to) != EMPTY)
		return 0;
	cnt = B->Count(EMPTY);
	if (cnt==B->Size()) // first move?
	{
		if (to==((B->Size()+1)/2-1)) // middle not allowed
			return 0;
	}
	else if (cnt==(B->Size()-2*B->Cols())) // all down?
	{
#ifdef LES_PENDUS
		if (from<0 || B->get(from)!=player)
			return 0;
#else
#ifdef MORRIS // must fix up to be orthogonal
		return CheckOrthoMove((const board_t *)b, player, from, to);
#else
		return CheckKingMove((const board_t *)b, player, from, to);
#endif
#endif
	}
	return 1;
}

game_t *Game(int sz, int &vertices, int argc, char *argv[], int &infolines,
	int &menuentries)
{
	vertices = infolines = menuentries = 0;
	game_t *rtn = (game_t *)new ovid_t(sz, HUMAN, argc, argv);
	assert(rtn);
	rtn->blockwins = 1;
	return rtn;
}

