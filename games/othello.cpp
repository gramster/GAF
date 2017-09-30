#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "movegame.h"
#include "app.h"

#define MAXTRY		400
#define MINBRDSZ	6
#define MAXBRDSZ	19

#ifndef max
#define max(a,b)	(((a)>(b)) ? (a) : (b))
#define min(a,b)	(((a)>(b)) ? (b) : (a))
#endif

#ifndef Abs
#define Abs(a)		(((a)>=0) ? (a) : -(a))
#endif

// hooks to window in gafgame.cpp

extern void ShowInfo(int ln, char *msg);
extern void ShowMessage(char *msg);
extern void MarkWhite(int r, int c);
extern void MarkBlack(int r, int c);
extern void LabelButton(int r, int c, int v,color_t color);
extern color_t whitecolor;
extern color_t blackcolor;

//----------------------------------------------------------------

class othellomove_t : public boardmove_t
{
	int captures;
public:
	othellomove_t(short to_in = -1)
		: boardmove_t(-1,to_in)
	{ }
	int Captures()
	{
		return captures;
	}
	void Captures(int captures_in)
	{
		captures = captures_in;
	}
	void Write(FILE *fp)
	{
		assert(from == -1);
		fprintf(fp, " %d ", to);
	}
	int Read(FILE *fp)
	{
		return (fscanf(fp, "%d", &to)==1);
		from = -1;
	}
};

//-------------------------------------------------------------------
// Move generator class

int ValidateHook(board_t* &b, int player, int from, int to);

class othellogen_t : public movegen_t
{
public:
	othellogen_t *parent;
	othellogen_t(const board_t* &b_in, int movenum_in, int player_in)
		: movegen_t(b_in, movenum_in, player_in)
	{ to = 0; }
	move_t *NextMove();
};

move_t *othellogen_t::NextMove()
{
	while (to < ((gridboard_t *)b)->Size())
	{
		if (Get(to) == EMPTY)
		{
			if (ValidateHook(b,player,-1,to))
			{
				move_t *rtn = (move_t *)new othellomove_t(to++);
				assert(rtn);
				return rtn;
			}
		}
		to++;
	}
	return NULL;
}

//-------------------------------------------------------------------------

class othello_t : public placegame_t
{
	int piececnt[3];
	void ShowStatus();
	int CanCapture(gridboard_t& bd, int player, int r, int c,
		int dr, int dc);
	int DoCapture(gridboard_t& bd, int player, int r, int c,
		int dr, int dc);
	int  RemoveCaptives(gridboard_t &b, int player, int r, int c);
public:
	int  CanMove(gridboard_t *b_in, int player);
	int  Validate(const board_t* &bd, int player, int &from, int &to);
	void Init();
	othello_t(int bsize, int first, int argc = 0, char **argv = NULL)
		: placegame_t(bsize, first, argc, argv)
	{
		Init();
	}
	generator_t *Generator(const board_t* &b_in, int movenum_in,
		int player_in, generator_t *pgen)
	{
		(void)pgen;
		generator_t *rtn =
			(generator_t *) (new othellogen_t(b_in, movenum_in,
				player_in));
		assert(rtn);
		return rtn;
	}
	move_t *NewMove(move_t *m = NULL)
	{
		move_t *rtn = new othellomove_t;
		assert(rtn);
		if (m) CopyMove(rtn, m);
		return rtn;
	}
	void Undo(board_t *b_in, move_t *m_in) // undo last move
	{
		othellomove_t *G = (othellomove_t *)m_in;
		piececnt[3-nextplayer]-=G->Captures()+1;
		piececnt[nextplayer] += G->Captures();
		gridgame_t::Undo(b_in, m_in);
		ShowStatus();
	}
	int Weight(const board_t* &b_in, int player);
	void Useage(const char *genargs)
	{
		closegraph();
		fprintf(stderr,"Useage: othello %s\n", genargs);
		exit(0);
	}
	int ApplyMove(board_t* &b_in, move_t *m, int mvnum,
		int player, moverecord_t **mr = NULL);
};

othello_t *othello;

//-------------------------------------------------------------------------
// Initialise a new game

void othello_t::Init()
{
	// probably unnecessary
	brd.Empty();
	brd.set(brd.Rows()/2-1,brd.Cols()/2-1,HUMAN);
	brd.set(brd.Rows()/2,brd.Cols()/2,HUMAN);
	brd.set(brd.Rows()/2-1,brd.Cols()/2,COMPUTER);
	brd.set(brd.Rows()/2,brd.Cols()/2-1,COMPUTER);
	for (int i = 0; i < 3; i++)
		piececnt[i] = 2;
#if 0
	if (userblack)
	{
		whitecolor= Black;
		blackcolor= White;
	}
	else
	{
		whitecolor=White;
		blackcolor=Black;
	}
#endif
}

//-------------------------------------------------------------------------
// Show the number of pieces, etc. OK

void othello_t::ShowStatus()
{
	char msg[60];
	ShowInfo(0, "          You       Me:");
	sprintf(msg,"PIECES    %3d       %3d",
		piececnt[HUMAN], piececnt[COMPUTER]);
	ShowInfo(1,msg);
}

//-------------------------------------------------------------------
// OK

int othello_t::Weight(const board_t* &b, int player)
{
	int cnt[3];
	gridboard_t *B = (gridboard_t *)b;
	(void)player;
	cnt[0] = cnt[1] = cnt[2] = 0;
	for (int r = 0; r < B->Rows(); r++)
		for (int c = 0; c < B->Rows(); c++)
			cnt[B->get(r,c)]++;
	if (cnt[EMPTY]==0 || cnt[HUMAN]==0 || cnt[COMPUTER]==0
		|| !CanMove(B, player)) // game over
	{
		if (cnt[HUMAN]==cnt[COMPUTER])
			return DRAW;
		else if (cnt[HUMAN]<cnt[COMPUTER])
			return FIRSTWIN+(cnt[COMPUTER]-cnt[HUMAN])*10;
		else 
			return FIRSTLOSE+(cnt[COMPUTER]-cnt[HUMAN])*10;
	}
	else 
	{
		int rtn = (cnt[COMPUTER]-cnt[HUMAN])*10;
		if (rtn==DRAW) rtn=0;
		return rtn;
	}
}

//-------------------------------------------------------------------
// Validate done

int othello_t::CanCapture(gridboard_t& bd, int player, int r, int c,
	int dr, int dc)
{
	int p;
	r += dr; c += dc;
	if (!bd.inpos(r,c) || bd.get(r,c) != 3-player)
		return 0;
	for (;;)
	{
		r += dr; c += dc;
		if (!bd.inpos(r,c))
			return 0;
		p = bd.get(r,c);
		if (p == EMPTY) return 0;
		else if (p == player) return 1;
	}
}

int othello_t::Validate(const board_t* &bd, int player, int &from, int &to)
{
	gridboard_t *B = (gridboard_t *)bd;
	int rt, ct;
	if (from != -1 || !B->inpos(to) || B->get(to) != EMPTY)
		return 0;
	// The play must have an orthogonally or diagonally adjacent
	// string of opponents pieces terminated by a player piece.
	rt = to/B->Cols(); 
	ct = to%B->Cols(); 
	if (CanCapture(*B,player,rt,ct,-1,0) || CanCapture(*B,player,rt,ct,+1,0) ||
	    CanCapture(*B,player,rt,ct,0,-1) || CanCapture(*B,player,rt,ct,0,+1) ||
	    CanCapture(*B,player,rt,ct,-1,-1)|| CanCapture(*B,player,rt,ct,-1,+1) ||
	    CanCapture(*B,player,rt,ct,+1,-1)|| CanCapture(*B,player,rt,ct,+1,+1))
		return 1;
	return 0;
}

int othello_t::CanMove(gridboard_t *b_in, int player)
{
	int from = -1;
	for (int to = 0; to < b_in->Size(); to++)
		if (Validate((const board_t *)b_in, player, from, to))
			return 1;
	return 0;
}

//-----------------------------------------------------------
// assumes a valid capture

int othello_t::DoCapture(gridboard_t &bd, int player, int r, int c,
	int dr, int dc)
{
	int len = 0;
	for (;;)
	{
		r += dr; c += dc;
		if (bd.get(r,c) == player) return len;
		len++;
		bd.set(r,c,player);
	}
}

int othello_t::RemoveCaptives(gridboard_t &b, int player, int r, int c)
{
	int captures = 0;
	if (CanCapture(b,player,r,c,-1,0))
		captures+=DoCapture(b,player,r,c,-1,0);
	if (CanCapture(b,player,r,c,+1,0))
		captures+=DoCapture(b,player,r,c,+1,0);
	if (CanCapture(b,player,r,c,0,-1))
		captures+=DoCapture(b,player,r,c,0,-1);
	if (CanCapture(b,player,r,c,0,+1))
		captures+=DoCapture(b,player,r,c,0,+1);
	if (CanCapture(b,player,r,c,-1,-1))
		captures+=DoCapture(b,player,r,c,-1,-1);
	if (CanCapture(b,player,r,c,+1,+1))
		captures+=DoCapture(b,player,r,c,+1,+1);
	if (CanCapture(b,player,r,c,-1,+1))
		captures+=DoCapture(b,player,r,c,-1,+1);
	if (CanCapture(b,player,r,c,+1,-1))
		captures+=DoCapture(b,player,r,c,+1,-1);
	return captures;
}

//-----------------------------------------------------------

int othello_t::ApplyMove(board_t* &b_in, move_t *m, int mvnum,
		int player, moverecord_t **mr)
{
	gridboard_t *B = (gridboard_t *)b_in;
	othellomove_t *M = (othellomove_t *)m;
	int r = M->to/B->Cols(), c = M->to%B->Cols();
#ifdef DEBUG
	if (debug)
	{
		fprintf(debug,"Applying %s move %d,%d\n",
			player==HUMAN?"You":"I",r,c);
	}
#endif
	moverecord_t *tmpmr;
	(void)movegame_t::ApplyMove(b_in, m, mvnum, player, &tmpmr);
	int catchcnt = RemoveCaptives(*B, player, r,c); // remove dead opponents
	((othellomove_t *)tmpmr->Move())->Captures(catchcnt);
	tmpmr->Board(NewBoard((const board_t *)b_in));
	if (mr) *mr = tmpmr;
	if (b_in == &brd) // real move
	{
		piececnt[3-player] -= catchcnt;
		piececnt[player] += catchcnt+1;
		ShowStatus();
		ShowResult();
		B->Print();
	}
	return 0; // don't print board; already done!
}

int ValidateHook(board_t* &bd, int player, int from, int to)
{
	return othello->Validate((const board_t *)bd, player, from, to);
}

game_t *Game(int sz, int &vertices, int argc, char *argv[],
	int &infolines, int &menuentries)
{
	vertices = 0;
	infolines = 2;
	menuentries = 0;
	// set lookahead to 1
	if (sz<MINBRDSZ) sz=MINBRDSZ;
	if (sz>MAXBRDSZ) sz=MAXBRDSZ;
	othello = new othello_t(sz, HUMAN, argc, argv);
	assert(othello);
	assert(othello->brd.Rows() >= MINBRDSZ && othello->brd.Rows() <= MAXBRDSZ);
	othello->CanPass(0);
	othello->blockwins = 1;
	return othello;
}

