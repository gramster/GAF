// see winning ways p.685.

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <graphics.h>
#include "movegame.h"

// Dodgem game

// Hook for making game's move validator accesible to the
// generator. Must be a more elegant way.

int ValidateHook(const board_t* &b, int player, int &from, int &to);

class dgen_t : public orthogen_t
{
	int first;
public:
	dgen_t(const board_t* &b_in, int movenum_in, int player_in)
		: orthogen_t(b_in, movenum_in, player_in, 1)
	{ first = 1; }
	int Validate(int &from, int &to)
	{
		return ValidateHook((const board_t*)b, player, from, to);
	}
};

class dodgem_t : public movegame_t
{
public:
	dodgem_t(int bsize, int first, int argc = 0, char **argv = NULL)
		: movegame_t(bsize, first, argc, argv)
	{ }
	void Init();
	generator_t *Generator(const board_t* &b_in, int movenum_in,
		int player_in, generator_t *pgen)
	{
		(void)pgen;
		generator_t *rtn =
			(generator_t *) (new dgen_t(b_in, movenum_in, player_in));
		assert(rtn);
		return rtn;
	}
	int Validate(const board_t* &bd, int player, int &from, int &to);
	int Weight(const board_t* &b_in, int player);
	void Useage(const char *genargs)
	{
		closegraph();
		fprintf(stderr,"Useage: dodgem %s [<size>]\n", genargs);
		exit(0);
	}
};

// setup board

void dodgem_t::Init()
{
	int p;
	movegame_t::Init();
	for (p=0; p < (brd.Rows()-1); p++)
		brd.set(p,0,COMPUTER);
	for (p=1; p < brd.Cols(); p++)
		brd.set(brd.Rows()-1,p,HUMAN);
}

// Trivial evaluator. This could be pruned down a lot using 
// Count, etc.

int dodgem_t::Weight(const board_t* &b, int player)
{
	gridboard_t *B = (gridboard_t *)b;
	int baggage = 0; // extra weight
	for (int p = 1; p<=2; p++)
	{
		int canmove=0, pcs = 0;
		int r, c;
		int brick = ((p==HUMAN) ? (FIRSTLOSE/4) : (FIRSTWIN/4))-1;
		int w8 = 0;
		for (int o=0; o<B->Size(); o++)
		{
			if (B->get(o)==p)
			{
				if (p==HUMAN)
					w8 += B->Cols() - o/B->Cols();
				else
					w8 += o%B->Cols();
				pcs++;
				if (p==player)
				{
					// can it move?
					int to;
					if (Validate(b, p, o, to = -1))
						canmove=1;
					else if (o>=B->Cols() && Validate(b, p, o, to = o-B->Cols()))
						canmove=1;
					else
					{
						// if human player, move ahead is blocked
						// this is considered bad...
						if (p==HUMAN) baggage-=brick*2/B->Size();
						if (Validate(b, p, o, to = o+1))
							canmove=1;
						else
						{
							// similarly for computer
							if (p==COMPUTER) baggage-=brick*2/B->Size();
							if (player==HUMAN)
							{
								if (o>0 && Validate(b, p, o, to = o-1))
									canmove=1;
							}
							else if (Validate(b, p, o, to = o+B->Cols()))
								canmove=1;
						}
					}
				}
			}
		}
		if (pcs==0 || (p==player && !canmove))
			goto win;
		// Weigh more toward top right of board and less pieces
		baggage += (brick + brick*w8/B->Cols()) / pcs;
	}
	if (baggage==DRAW) baggage=0; // no draw possible and our weight
		// function could inadvertently create one! Must make
		// this cleaner, probably by making the Win/Draw/Lose
		// methods virtual.
	return baggage; // game not over 
win:
	return ((p==COMPUTER) ? MAXWEIGHT : MINWEIGHT) + baggage;
}

int dodgem_t::Validate(const board_t* &bd, int player, int &from, int &to)
{
	gridboard_t *B = (gridboard_t *)bd;
	if (!B->inpos(from) || B->get(from)!=player)
		return 0;
	if (to == -1)
	{
		if (player==HUMAN)
		{
			if (from>=B->Cols()) return 0;
		}
		else if ((from%B->Cols()) != (B->Cols()-1)) return 0;
	}
	else if (!B->inpos(to) ||  B->get(to)!=EMPTY)
		return 0;
	else if (to == (from+1))
	{
		if ((to % B->Cols())==0) return 0;
	}
	else if (to == (from-1))
	{
		if (player!=HUMAN || (from % B->Cols())==0) return 0;
	}
	else if (to == (from+B->Cols()))
	{
		if (player!=COMPUTER) return 0;
	}
	else if (to != (from-B->Cols()))
		return 0;
	return 1;
}

dodgem_t *dg;

int ValidateHook(const board_t* &bd, int player, int &from, int &to)
{
	return dg->Validate(bd, player, from, to);
}

game_t *Game(int sz, int &vertices, int argc, char *argv[], int &infolines,
	int &menuentries)
{
	vertices = infolines = menuentries = 0;
	dg = new dodgem_t(sz, HUMAN, argc, argv);
	assert(dg);
	return dg;
}

