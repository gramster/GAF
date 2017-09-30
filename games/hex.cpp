#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <graphics.h>
#include <ctype.h>

#include "movegame.h"

// n-by-n Hexapawn game

class hexapawn_t : public pawngame_t
{
public:
	hexapawn_t(int bsize, int first, int argc = 0, char **argv = NULL)
		: pawngame_t(bsize, first, argc, argv)
	{ }
	void Init();
	int Weight(const board_t* &b_in, int player);
	void Useage(const char *genargs)
	{
		closegraph();
		fprintf(stderr,"Useage: hex %s [<size>]\n", genargs);
		exit(0);
	}
};

// setup board

void hexapawn_t::Init()
{
	int c;
	gridgame_t::Init();
	for (c=0; c<brd.Cols();c++)
	{
		brd.set(0,c,COMPUTER);
		brd.set(brd.Rows()-1,c,HUMAN);
	}
}

// Trivial evaluator

int hexapawn_t::Weight(const board_t* &b, int player)
{
	const gridboard_t *B = (const gridboard_t *)b;
	int empty, human, computer, p;
	B->Count(empty, human, computer);
	// check for all captured or pawn in last row
	if (human==0 || B->CountRow(B->Rows()-1, COMPUTER)>0)
		p = COMPUTER;
	else if (computer==0 || B->CountRow(0, HUMAN)>0)
		p = HUMAN;
	// check if player can move
	else if (LocatePawn(b, player, CANMOVE)>=0)
		return EVEN;
	else p = 3 - player;
	return (p==COMPUTER) ? MAXWEIGHT : MINWEIGHT;
}

game_t *Game(int sz, int &vertices, int argc, char *argv[], int &infolines,
	int &menuentries)
{
	vertices = infolines = menuentries = 0;
	game_t *rtn = (game_t *)new hexapawn_t(sz, HUMAN, argc, argv);
	assert(rtn);
	return rtn;
}

