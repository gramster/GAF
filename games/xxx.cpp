#include <stdlib.h>
#include <stdio.h>
#include <graphics.h> // for closegraph
#include <assert.h>
#include "movegame.h"

class tictactoe_t : public placegame_t
{
public:
	tictactoe_t(int bsize, int first, int argc = 0, char **argv = NULL)
		: placegame_t(bsize, first, argc, argv)
	{ }
	int Weight(const board_t* &b_in, int player);
	void Useage(const char *genargs)
	{
		closegraph();
		fprintf(stderr,"Useage: xxx %s\n", genargs);
		exit(0);
	}
};

int tictactoe_t::Weight(const board_t* &b, int player)
{
	gridboard_t *B = (gridboard_t *)b;
	int sz = B->Cols();
	(void)player; // has no influence in xxx
	for (int p = 1; p<=2; p++)
	{
		int r, c;
		// check for win across
		for (r=0; r<sz; r++)
		{
			for (c=0; c<sz; c++)
				if (B->get(r,c)!=p)
					break;
			if (c==sz) goto win;
		}
		// check for win down
		for (c=0; c<sz; c++)
		{
			for (r=0; r<sz; r++)
				if (B->get(r,c)!=p)
					break;
			if (r==sz) goto win;
		}
		// check for diagonal win
		for (r=0; r<sz; r++)
			if (B->get(r,r)!=p)
				break;
		if (r==sz) goto win;
		for (r=0; r<sz; r++)
			if (B->get(r,sz-r-1)!=p)
				break;
		if (r==sz) goto win;
	}
	// check for draw
	return (B->full() ? DRAW : EVEN);
win:
	return (p==COMPUTER) ? MAXWEIGHT : MINWEIGHT;
}

game_t *Game(int sz, int &vertices, int argc, char *argv[], int &infolines,
	int &menuentries)
{
	vertices = infolines = menuentries = 0;
	game_t *rtn = (game_t *)new tictactoe_t(sz, HUMAN, argc, argv);
	assert(rtn);
	return rtn;
}

