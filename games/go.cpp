// Go game - adapted from GnuGo approach.

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "movegame.h"
#include "app.h"

#define MAXTRY		400
#define MINBRDSZ	7
#define MAXBRDSZ	19
#define MAXBREADTH	8

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
extern void ShowResult();
extern void MarkWhite(int r, int c);
extern void MarkBlack(int r, int c);
extern void LabelButton(int r, int c, int v,color_t color);
extern color_t whitecolor;
extern color_t blackcolor;

//----------------------------------------------------------------

class gomove_t : public boardmove_t
{
	int captures, korow, kocol;
public:
	int Captures()
	{
		return captures;
	}
	int KORow()
	{
		return korow;
	}
	int KOCol()
	{
		return kocol;
	}
	void Captures(int captures_in)
	{
		captures = captures_in;
	}
	void KORow(int korow_in)
	{
		korow = korow_in;
	}
	void KOCol(int kocol_in)
	{
		kocol = kocol_in;
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

//----------------------------------------------------------------
// The generation algorithm makes a table of moves

typedef struct
{
	int r, c, val;
} genrecord_t;

genrecord_t genrecord[MAXBREADTH];
int movecnt=0;
int minval, minentry;
int breadth = MAXBREADTH;

void ClearRecord()
{
	movecnt = 0;
}

void AddMove(int r, int c, int val)
{
	// see if we have it
	for (int i = 0; i < movecnt; i++)
	{
		if (genrecord[i].r == r && genrecord[i].c == c)
		{
			if (val > genrecord[i].val)
				genrecord[i].val = val;
			return;
		}
	}
	if (movecnt < breadth)
	{
		genrecord[movecnt].r = r;
		genrecord[movecnt].c = c;
		genrecord[movecnt].val = val;
		movecnt++;
	}
	else
	{
		minval = genrecord[0].val;
		minentry = 0;
		for (i = 1; i < breadth; i++)
		{
			if (genrecord[i].val < minval)
			{
				minval = genrecord[i].val;
				minentry = i;
			}
		}
		genrecord[minentry].r = r;
		genrecord[minentry].c = c;
		genrecord[minentry].val = val;
	}
}

//-------------------------------------------------------------------
// Move generator class

class gogen_t : public movegen_t
{
public:
	genrecord_t mvrecord[MAXBREADTH];
	int mvcnt, mvnow;
//	int KOrow[3], KOcol[3];
	gogen_t *parent;
	gogen_t(const board_t* &b_in, int movenum_in, int player_in,
		generator_t *pgen);
	move_t *NextMove();
};

//-------------------------------------------------------------------------

class go_t : public placegame_t
{
	int gameover, winner;
	int capturerow[3], capturecol[3], capturecnt[3], piececnt[3],
		turfcnt[3];
	int liberties[MAXBRDSZ][MAXBRDSZ];
	void Count2(gridboard_t &b, int &lib, int r, int c, int player, gridboard_t &markbd);
	void Count(gridboard_t &b, int &lib, int r, int c, int player, gridboard_t &markbd);
	void CountLib(gridboard_t &b, int &lib, int r, int c, int player);
	void ShowStatus();
	void CountTurf();
	int  RemoveCaptives(gridboard_t &b, int player);
	int  AddOpen(gridboard_t &b, int r, int c, int rows[], int cols[],
		int player, int minlib, int &cnt, gridboard_t &markbd);
	int  FindOpen(gridboard_t &b, int r, int c, int rows[], int cols[],
		int player, int minlib, int &cnt, gridboard_t &markbd);
	int  FindDirPlayer(int r, int c, int dr, int dc);
	int  FindPlayer(int r, int c);
	int  CheckNeighbour(gridboard_t &b, int r, int c, int player,
		int &val, int &rr, int &rc, int minlib, gridboard_t &markbd);
	int  FindNextMove(gridboard_t &b, int r, int c, int player,
		int &rr, int &rc, int &val, int minlib, gridboard_t &markbd);
	int  CompareLiberties(int newlib, int minlib);
	int  FindDefense(gridboard_t &b, int &r, int &c, int player, int &val);
	int  FindAttack(gridboard_t &b, int &r, int &c, int player, int &val);
	void SetHandicap(int hcap, int player);
	int  Suicide(gridboard_t &b, int r, int c, int player);
	int  Own2(gridboard_t &b, int r1, int c1, int r2, int c2, int player);
	int  Own3(gridboard_t &b, int r1, int c1, int r2, int c2, int r3, int c3, int player);
	int  IsSurrounded(gridboard_t &b, const int r, const int c, int player);
	int  Opening(gridboard_t &b, int &r, int &c, int player, int type);
	int  IsRegionEmpty(gridboard_t &b, int r1, int c1, int r2, int c2);
	int  MatchPattern(gridboard_t &b, int r, int c, int player, int &rr, int &cc, int &val);
	int  FindPattern(gridboard_t &b, int &r, int &c, int player, int &val);
	void RemoveDeadGroup(gridboard_t &b, int r, int c, int player);
	void ShowDeadMessage();
	void MarkDiagonal(gridboard_t &b, int cnum, int r, int c,
		int rr, int cc, int p, gridboard_t &markbd);
	void MarkChain(gridboard_t &b, int cnum, int r, int c, int p,
		gridboard_t &markbd);
	void FindEmptyRegion(gridboard_t &b, int r, int c, int &ch, int &sz,
		gridboard_t &markbd);
	void EyeSpace(gridboard_t &b, int cnum, int &neyes, int &eyespace);
	void SpreadInfluence();
	void Influence();
	int CheckPos(gridboard_t &b, int r, int c, int f);
	int CheckCorners(gridboard_t &b, int r, int c, int f);
	int TakeCorner(gridboard_t &b, int player);
public:
	gridboard_t *atari;
	int handicap;
	int showatari;
	int chain[MAXBRDSZ][MAXBRDSZ];
	int influence[MAXBRDSZ][MAXBRDSZ];
	void EndGame();
	void Eval(gridboard_t &b, int player);
	void GenMove(gridboard_t &b, int player);
	int  Validate(const board_t* &bd, int player, int &from, int &to);
	void Init();
	go_t(int bsize, int first, int argc = 0, char **argv = NULL)
		: placegame_t(bsize, first, argc, argv)
	{
		atari = new gridboard_t(bsize,bsize);
		assert(atari);
		handicap = 0;
		showatari = 1;
		Init();
	}
	~go_t()
	{
		delete atari;
	}
	generator_t *Generator(const board_t* &b_in, int movenum_in,
		int player_in, generator_t *pgen)
	{
		generator_t *rtn =
			(generator_t *) (new gogen_t(b_in, movenum_in,
				player_in, pgen));
		assert(rtn);
		return rtn;
	}
	move_t *NewMove(move_t *m = NULL)
	{
		move_t *rtn = new gomove_t;
		assert(rtn);
		if (m) CopyMove(rtn, m);
		return rtn;
	}
	void Undo(board_t *b_in, move_t *m_in) // undo last move
	{
		gomove_t *G = (gomove_t *)m_in;
		capturecnt[3-nextplayer] -= G->Captures();
		piececnt[nextplayer] += G->Captures();
		piececnt[3-nextplayer]--;
		gridgame_t::Undo(b_in, m_in);
		ShowStatus();
	}
	int Influence(int r, int c);
	int FindChains(gridboard_t &b);
	int TestResult(gridboard_t &b, int r, int c, int p);
	int Weight(const board_t* &b_in, int player);
	void Useage(const char *genargs)
	{
		closegraph();
		fprintf(stderr,"Useage: go %s\n", genargs);
		exit(0);
	}
	int ApplyMove(board_t* &b_in, move_t *m, int mvnum,
		int player, moverecord_t **mr = NULL);
	void AddEntries(void *m); // for menu entries using GAF
	void AddOptions(void *m);
	void HandlePass()
	{
		game_t::HandlePass();
		if (autoplay) EndGame();
		else if (passcnt>1) ShowDeadMessage();
		else ShowMessage(nextplayer==HUMAN?"You pass":"I pass");
	}
};

go_t *go;

//-------------------------------------------------------------------------

gogen_t::gogen_t(const board_t* &b_in, int movenum_in, int player_in,
	generator_t *pgen)
	: movegen_t(b_in, movenum_in, player_in)
{
	parent = (gogen_t *)pgen;
	go->GenMove(*((gridboard_t *)b), player);
	int v = -1, i, j;
	for (i = 0; i < movecnt; i++)
	{
		if (genrecord[i].val>v)
			v = genrecord[i].val;
	}
	for (i = j = 0; i < movecnt; i++)
		if (genrecord[i].val==v)
			mvrecord[j++] = genrecord[i];
	mvcnt = j;
	mvnow = 0;
#ifdef DEBUG
	if (debug)
	{
		fprintf(debug, "Generator for board:\n");
		b_in->Print(debug);
		fprintf(debug, "produced moves:\n");
		for (i = 0; i < mvcnt; i++)
			fprintf(debug, "%d,%d : %d\n",
				mvrecord[i].r,
				mvrecord[i].c,
				mvrecord[i].val);
	}
#endif
}

move_t *gogen_t::NextMove()
{
	move_t *rtn = NULL;
	if (mvnow<mvcnt)
	{
		rtn = new boardmove_t(-1,
			mvrecord[mvnow].r*((gridboard_t *)b)->Cols()+
			mvrecord[mvnow].c);
		assert(rtn);
		mvnow++;
	}
	return rtn;
}

//-------------------------------------------------------------------------
// Liberty counting

void go_t::Count2(gridboard_t &b, int &lib, int r, int c, int player,
	gridboard_t &markbd)
{
	if (markbd.inpos(r,c) && markbd.get(r, c) != EMPTY)
	{
		if (b.get(r, c) == EMPTY)
		{
		    ++lib;
		    markbd.set(r,c,EMPTY);
		}
		else if (b.get(r, c) == player)
			Count(b, lib, r, c, player, markbd);
	}
}

void go_t::Count(gridboard_t &b, int &lib, int r, int c, int player,
	gridboard_t &markbd)
{
	markbd.set(r,c,EMPTY); // mark piece
	Count2(b, lib, r-1, c,   player, markbd); // count north
	Count2(b, lib, r  , c-1, player, markbd); // count west
	Count2(b, lib, r  , c+1, player, markbd); // count east
	Count2(b, lib, r+1, c  , player, markbd); // count south
}

// work out liberties for a board position

void go_t::CountLib(gridboard_t &b, int &lib, int r, int c, int player)
{
	gridboard_t markbd(brd.Rows(),brd.Cols());
	markbd.SetAll(player);
	Count(b, lib, r, c, player, markbd);
}

// Work out liberties for player

void go_t::Eval(gridboard_t &b, int player)
{
	for (int r = 0; r < b.Rows(); r++)
	{
		for (int c = 0; c < b.Cols(); c++)
		{
			if (b.get(r,c)==player)
			{
				int lib = 0;
				CountLib(b, lib,r,c,player);
				liberties[r][c] = lib;
			}
		}
	}
}

//---------------------------------------------------------------
// Find open space around a coordinate
//
// This recursively explores around a group of pieces, stopping
// when an opponent piece or empty square is found. In the latter
// case, if this is not a previous KO square, the square is saved in
// a vector. If the vector count reaches the liberty count of the 
// group, FindOpen returns 1.

int go_t::AddOpen(gridboard_t &b, int r, int c, int rows[], int cols[],
	int player, int minlib, int &cnt, gridboard_t &markbd)
{
	if (!markbd.inpos(r,c))
		return 0;
	gomove_t *m = (gomove_t *)LastMove();
	if (b.get(r,c) == EMPTY
		&& (m->Captures()!=1 || r != m->KORow() || c != m->KOCol()))
	{
	    rows[cnt] = r;
	    cols[cnt] = c;
	    if (++cnt == minlib) return 1;
	}
	else if (b.get(r,c) == player && markbd.get(r,c)==EMPTY)
		if (FindOpen(b, r, c, rows, cols, player, minlib, cnt, markbd)
			&& (cnt == minlib))
				return 1;
	return 0;
}

//--------------------------------------------------------------------

int go_t::FindOpen(gridboard_t &b, int r, int c, int rows[], int cols[],
	int player, int minlib, int &cnt, gridboard_t &markbd)
{
	markbd.set(r,c,player);
	if (AddOpen(b, r-1,c,rows,cols,player,minlib,cnt,markbd) ||
	    AddOpen(b, r+1,c,rows,cols,player,minlib,cnt,markbd) ||
	    AddOpen(b, r,c-1,rows,cols,player,minlib,cnt,markbd) ||
	    AddOpen(b, r,c+1,rows,cols,player,minlib,cnt,markbd))
		return 1;
	return 0; // no space
}

//-----------------------------------------------------------

int go_t::CheckNeighbour(gridboard_t &b, int r, int c, int player, int &val,
	int &rr, int &rc, int minlib, gridboard_t &markbd)
{
	int found = 0, tr, tc, tval = -1;
	if (!markbd.inpos(r,c)) return 0;
	if (b.get(r,c) == EMPTY)
	{
		int lib = 0;
		tr = r;
		tc = c;
		CountLib(b, lib, r, c, player);
		tval = CompareLiberties(lib, minlib);
		found = 1;
	}
	else if (b.get(r,c) == player && markbd.get(r,c)==EMPTY)
		if (FindNextMove(b, r, c, player, tr, tc, tval, minlib, markbd))
			found = 1;
	if (found)
	{
		found = 0;
		if (tval > val)
		{
			val = tval;
			rr = tr;
			rc = tc;
		}
		if (minlib == 1)
			return 1;
	}
	return 0;
}

//-----------------------------------------------------------------
// Find a new move from group containing r, c; return in rr,cc.

int go_t::FindNextMove(gridboard_t &b, int r, int c, int player,
	int &rr, int &rc, int &val, int minlib, gridboard_t &markbd)
{
	rr = rc = val = -1;
	markbd.set(r,c,player);
	if (CheckNeighbour(b, r-1,c,player,val,rr,rc,minlib,markbd) ||
	    CheckNeighbour(b, r+1,c,player,val,rr,rc,minlib,markbd) ||
	    CheckNeighbour(b, r,c-1,player,val,rr,rc,minlib,markbd) ||
	    CheckNeighbour(b, r,c+1,player,val,rr,rc,minlib,markbd))
		return 1;
	return 0;
}

//-------------------------------------------------------------------
// Compare liberties

int go_t::CompareLiberties(int newlib, int minlib)
{
	return (newlib <= minlib) ? -1 : 
	       (40 + ((newlib-minlib) - 1) * 50 / (minlib * minlib * minlib));
}

//-------------------------------------------------------------------
// Save threatened pieces

int go_t::FindDefense(gridboard_t &b, int &r, int &c, int player, int &val)
{
	int ti, tj, tval;
	r = c = val = -1;
	for (int minlib = 1; minlib < 4; minlib++)
	{
		for (int rr = 0; rr < b.Rows(); rr++)
		{
			for (int cc = 0; cc < b.Cols(); cc++)
			{
				if (b.get(rr,cc) == player &&
				    liberties[rr][cc] == minlib)
				{
					int tr, tc;
					gridboard_t markbd(b.Rows(), b.Cols());
					if (FindNextMove(b, rr, cc, player, tr, tc, tval, minlib, markbd))
					{
#ifdef DEBUG
						if (debug)
							fprintf(debug, "Found defense %d,%d weight %d\n",tr,tc,tval);
#endif
						if (tval > val)
						{
							val = tval;
//							r = tr;
//							c = tc;
						}
						AddMove(tr,tc,tval);
					}
				}
			}
		}
	}
	return (val > 0); // found move?
}

//-------------------------------------------------------------------
// find opponent piece to capture or attack
// algorithm outline:
//
// For each opponent piece with liberty less than four
//    Find the pieces giving opponent liberty (vector tr,tc)
//    if opponent group has only one liberty and val<120
//       (r,c,val) = (tr[0],tr[0],120)
//    else 
//       for u = 0 .. liberties
//         for v = 0 .. liberties
//            if (u != v)
//               if liberties of (tr[u],tc[u]) not zero
//                  apply move, find resulting liberties of (tr[v],tc[v])
//                     (i.e. liberties of group if opponent plays this)
//                  reject move if my liberty one and opponent's non-zero
//                  else value = 120 - 20 * opponent liberties
//                  if this is a new high value, save (tr[u],tc[u])
//                  undo applied move
//
// This has been slightly modified for efficiency and to maximise
// opponents reply properly.

int go_t::FindAttack(gridboard_t &b, int &r, int &c, int player, int &val)
{
	int rr, cc, tr[3], tc[3], tval, cnt, u, v, lib1;

	r = c = val = -1;
	// find opponent with liberty less than four
	for (rr = 0; rr< b.Rows(); rr++)
	{
		for (cc = 0; cc < b.Cols(); cc++)
		{
			if (b.get(rr,cc) == 3-player && liberties[rr][cc] < 4)
			{
				gridboard_t markbd(b.Rows(), b.Cols());
				cnt = 0;
				if (FindOpen(b, rr, cc, tr, tc, 3-player, liberties[rr][cc], cnt, markbd))
				{
					if (liberties[rr][cc] == 1) // kill it?
					{
#ifdef DEBUG
						if (debug)
							fprintf(debug, "Found deadly attack %d,%d weight 120\n", tr[0],tc[0]);
#endif
						if (val < 120)
						{
							val = 120;
//							r = tr[0];
//							c = tc[0];
						}
						AddMove(tr[0],tc[0],val);
					}
					else
					{
						for (u = 0; u < liberties[rr][cc]; u++)
						{
							int lib1 = 0, maxl = 0;
							b.set(tr[u],tc[u],player);
							CountLib(b, lib1, tr[u], tc[u], player);
							if (lib1 > 0)	/* valid move */
							{
								for (v = 0; v < liberties[rr][cc]; v++)
								{
									if (u != v)
									{
										// look ahead opponent move
										int lib = 0;
										CountLib(b, lib, tr[v], tc[v], 3-player);
										if (lib>maxl)
											maxl = lib;
				    					}
								}
								if (lib1 == 1 && maxl > 0)
									tval = 0;
								else
									tval = 120 - 20 * maxl;
#ifdef DEBUG
								if (debug)
									fprintf(debug, "Found non-deadly attack %d,%d weight %d\n", tr[u],tc[u],tval);
#endif
								if (val < tval)
								{
									val = tval;
//									r = tr[u];
//									c = tc[u];
								}
								AddMove(tr[u],tc[u],tval);
							}
							b.set(tr[u], tc[u], EMPTY);
						}
					}
				}
			}
		}
	}
	return (val > 0); // found move?
}

//-------------------------------------------------------------------
// check suicide move of opponent at (r,c)

int go_t::Suicide(gridboard_t &b, int r, int c, int player)
{
	int rr, cc, kill;
	int lib = 0;
	CountLib(b, lib, r, c, player);
	if (lib == 0)
	// new move is suicide then check if kill my pieces and Ko possibility
	{
		/* assume alive */
		b.set(r, c, player);
		/* check oppoenets liberties */
		Eval(b, 3-player);
		for (rr = kill = 0; rr < b.Rows(); rr++)
			for (cc = 0; cc < b.Cols(); cc++)
				// count how many pieces will be killed
				if (b.get(rr,cc)==(3-player) && liberties[rr][cc]==0)
					kill++;
		gomove_t *m = (gomove_t *)LastMove();
		if (kill == 0 ||
		   (kill == 1 && m->Captures()==1 &&
			r == m->KORow() && c==m->KOCol()))
		// either no effect on my pieces or an illegal Ko so undo
		{
			b.set(r,c,EMPTY);
			return 1;
		}
	}
	return 0;
}

//-------------------------------------------------------------------
// these are all OK

int go_t::Own2(gridboard_t &b, int r1, int c1, int r2, int c2, int player)
{
	return (b.get(r1,c1)==player && b.get(r2,c2)==player);
}

int go_t::Own3(gridboard_t &b, int r1, int c1, int r2, int c2, int r3, int c3, int player)
{
	return (b.get(r1,c1)==player && b.get(r2,c2)==player &&
		b.get(r3,c3)==player);
}

int go_t::IsSurrounded(gridboard_t &b, const int r, const int c, int player)
{
	if (r == 0) // check top edge
	{
		if (c == 0 && Own2(b, r+1,c,r,c+1,player))
			return 1;
		if (c == (b.Cols()-1) && Own2(b, r+1,c,r,c-1,player))
			return 1;
		return Own3(b, r+1,c,r,c-1,r,c+1,player);
	}
	if (r == (b.Rows()-1)) // check bottom edge
	{
		if (c == 0 && Own2(b, r-1,c,r,c+1,player))
			return 1;
		if (c == (b.Cols()-1) && Own2(b, r-1,c,r,c-1,player))
			return 1;
		return Own3(b, r-1,c,r,c-1,r,c+1,player);
	}
	if (c == 0) // check left edge
		return Own3(b, r,c+1,r-1,c,r+1,c,player);
	else if (c == (b.Cols()-1)) // check right edge
		return Own3(b, r,c-1,r-1,c,r+1,c,player);
	else return (Own2(b, r,c-1,r,c+1,player) && Own2(b, r-1,c,r+1,c,player));
}

//-------------------------------------------------------------------

void go_t::GenMove(gridboard_t &b, int player)
{
	int tval, tr, tc, lib;
	tr = tc = tval = -1;
	if (passcnt>1) return;
	ClearRecord();
	Eval(b, player); // work out my liberties
	Eval(b, 3-player); // work out opponents liberties
	if (TakeCorner(b, player)) ;
	if (FindAttack(b, tr, tc, player, tval)) ;
//		AddMove(tr,tc,tval);
	if (FindDefense(b, tr, tc, player, tval)) ;
//		AddMove(tr,tc,tval);
	/* try match local play pattern for new move */
	if (FindPattern(b, tr, tc, player, tval)) ;
//		AddMove(tr,tc,tval);
	// no urgent move then do random move, avoiding edges and center,
	// liberty one, fill of own eye, and suicide.
	int Try = 0;
	if (tval < 0)
	{
#ifdef DEBUG
		if (debug)
			fprintf(debug,"Finding a random move for %s\n",
					player==HUMAN?"You":"Me");
#endif
		do
		{
			int retry = 10; // arbitrary
			while (retry--)
			{
				tr = random(b.Rows());
				/* avoid low line and center region */
				if (tr >= 2 && tr < (b.Rows()-2) && (tr <= 5 || tr > b.Rows()-5))
					break;
				if (retry < 5 && tr >= 2 && tr < (b.Rows()-2))
					break;
			}
			retry = 10; // arbitrary
			while (retry--)
			{
				tc = random(b.Cols());
				/* avoid low line and center region */
				if (tc >= 2 && tc < (b.Cols()-2) && (tc <= 5 || tc > b.Cols()-5))
					break;
				if (retry < 5 && tc >= 2 && tc < (b.Cols()-2))
					break;
			}
			lib = 0;
			CountLib(b, lib, tr, tc, player);
		}
		// avoid liberty of 1 and fill of own eye
		while (++Try < MAXTRY &&
			(b.get(tr,tc) != EMPTY || lib < 2
			|| IsSurrounded(b, tr, tc, player)
			|| Suicide(b, tr, tc, player)));
	}
	if (Try >= MAXTRY) tr = tc = -1; // pass
#if 1
	AddMove(tr,tc,0);
#endif
}

//-------------------------------------------------------------------
// Opening move. Tend to stick away from edges and center, and
// avoid placing adjacent to an existing piece of same colour

int go_t::Opening(gridboard_t &b, int &r, int &c, int player, int type)
{
	int cnt = 9;
	while (cnt--)
	{
		r = 2 + random(2);
		c = 2 + random(2);
		if ((type == 1) || (type == 3))
			r = b.Rows() - r - 1;
		if ((type == 2) || (type == 3))
			c = b.Cols() - c - 1;
		if (b.get(r,c)!=EMPTY) { cnt++; continue; }
		if (b.get(r+1,c)!=player
		    && b.get(r-1,c)!=player
		    && b.get(r,c-1)!=player
		    && b.get(r,c+1)!=player)
			break;
	}
	return 1;
}

//-------------------------------------------------------------------
// check if region bounded by a rectangle is empty

int go_t::IsRegionEmpty(gridboard_t &b, int r1, int c1, int r2, int c2)
{
	int maxr = max(r1,r2);
	int maxc = max(c1,c2);
	for (int r = min(r1,r2); r <= maxr; r++)
		for (int c = min(c1,c2); c <= maxc; c++)
			if (b.get(r,c) != EMPTY)
				return 0;
	return 1;
}

//-------------------------------------------------------------------
// match pattern and get next move

#define MAXPC 16
#define PATNO 24

#include "patterns.cpp"

int go_t::MatchPattern(gridboard_t &b, int r, int c, int player,
	int &rr, int &cc, int &val)
{
	#define line(r)		(b.Rows() - abs(b.Rows()-r))
	#define column(c)	(b.Cols() - abs(b.Cols()-c))

	int pr, pc;	// pattern coords
	rr = cc = val = -1;
	// loop thru patterns
	for (int pat = 0; pat < PATNO; pat++)
	{
		// loop thru matrix transforms
		for (int l = 0; l < pattern[pat].trfno; l++)
		{
			int cont = 1;
			for (int k = 0; k != pattern[pat].patlen && cont; k++)
			{
				// transform coordinates
				int trc, trr;
				trc = c + trf[l][0][0] * pattern[pat].patn[k].x
				    + trf[l][0][1] * pattern[pat].patn[k].y;
				if (!b.incol(trc))
				{
				    cont = 0;
				    break;
				}
				trr = r + trf[l][1][0] * pattern[pat].patn[k].x
				    + trf[l][1][1] * pattern[pat].patn[k].y;
				if (!b.inrow(trr))
				{
				    cont = 0;
				    break;
				}
				int p = b.get(trr, trc);
				int a = pattern[pat].patn[k].att;
				if (a<3)
				{
					int p2 = a ? (a==2 ? player : (3-player)) : 0;
					if (p != p2) cont = 0;
				}
				else if (a==3)
				{
					if (p == EMPTY)	/* open for new move */
					{
						int lib = 0;
						CountLib(b, lib, trr, trc, player);
						if (lib > 1)
						{
							pr = trr;
							pc = trc;
						}
						else cont = 0;
					}
					else cont = 0;
				}
				else
				{
					int p2 = (a>4) ? (a==6 ? player : (3-player)) : 0;
					if (p != p2) cont = 0;
					if (trr != 0 && trr != (b.Rows()-1)
					  && trc != 0 && trc != (b.Cols()-1))
						cont = 0; // not on edge
				}
			}
			if (cont) // found a pattern; check weight
			{
				int tval = pattern[pat].patwt;
				if ((pat >= 8) && (pat <= 13))
				{
					// patterns for expand region have rewards/penalties
					if (line(pr) < 2)	tval--;
					else if (line(pr) < 4)	tval++;
					if (column(pc) < 2)	tval--;
					else if (column(pc) < 4)tval++;
				}
				if (tval > val)
				{
					val = tval;
					rr = pr;
					cc = pc;
				}
			}
		}
	}
	return (val > 0); // found pattern
}

//-------------------------------------------------------------------
// NEEDS SOME MAJOR HAX FOR LOOKAHEAD!!

int go_t::FindPattern(gridboard_t &b, int &r, int &c, int player, int &val)
{
	int             tr, tc, tval;
	val = -1;
#if 0 // this is problematic with lookahead and is out for now
	if (mtype==0 && mcnt==3 && !IsRegionEmpty(0, 0, 4, 4))
		mcnt=0;
#endif
	for (;;)
	{
#if 0
		if (mcnt) // continue filling current region
		{
			mcnt--;
			if (Opening(b, r, c, player, mtype))
				goto ok;
		}
		else if (mtype<3)
		{
			// Find a new open region to start filling
			mtype++;
			int tr = b.Rows()/5-1;
			int lc = b.Cols()/5-1;
			if ((mtype == 1) || (mtype == 3))
				tr = b.Rows() - tr - 5;
			if ((mtype == 2) || (mtype == 3))
				lc = b.Cols() - lc - 5;
			if (IsRegionEmpty(b, tr, lc, tr+4, lc+4)) mcnt=3;
		}
		else if (mtype<8) // occupy edges
		{
			mtype++;
			if (mtype==4)
			{
				r = 2;
				c = b.Cols()/2;
			}
			else if (mtype==5)
			{
				r = b.Rows() - 3;
				c = b.Cols()/2;
			}
			else if (mtype==6)
			{
				r = b.Rows()/2;
				c = 2;
			}
			else
			{
				r = b.Rows()/2;
				c = b.Cols() - 3;
			}
			if (IsRegionEmpty(r-2, c-2, r+2, c+2))
				goto ok;
		}
		else
#endif
#if 1
		// Exhausted opening moves; look for patterns
		for (int rr = 0; rr < b.Rows(); rr++)
		{
			for (int cc = 0; cc < b.Cols(); cc++)
			{
			 	if (b.get(rr,cc)==player &&
					MatchPattern(b, rr, cc, player, tr, tc, tval) &&
					tval > val)
				{
					val = tval;
					r = tr;
					c = tc;
				}
			}
		}
#endif
		return (val>0); // found a pattern
	}
ok:
	val = 80;
	return 1;
}

//-------------------------------------------------------------------------
// Get user to do the dirty work.

void go_t::ShowDeadMessage()
{
	ShowInfo(0, "PLEASE CLICK ON DEAD GROUPS");
	ShowInfo(1, "CLICK ON END/FILL WHEN DONE");
	ShowInfo(2,"");
	ShowInfo(3,"");
}

//-------------------------------------------------------------------------
// Show the number of pieces, etc.

void go_t::ShowStatus()
{
	char msg[60];
	ShowInfo(0, "          You       Me:");
	sprintf(msg,"CAPTURES  %3d       %3d",
		capturecnt[HUMAN], capturecnt[COMPUTER]);
	ShowInfo(1,msg);
	sprintf(msg,"PIECES    %3d       %3d",
		piececnt[HUMAN], piececnt[COMPUTER]);
	ShowInfo(2,msg);
	if (turfcnt[HUMAN] || turfcnt[COMPUTER])
	{
		sprintf(msg,"TURF      %3d       %3d",
			turfcnt[HUMAN], turfcnt[COMPUTER]);
		ShowInfo(3,msg);
		int h = turfcnt[HUMAN]+capturecnt[HUMAN];
		int c = turfcnt[COMPUTER]+capturecnt[COMPUTER];
		if (h>c) winner = HUMAN;
		else if (h<c) winner = COMPUTER;
		else winner = DRAW;
		if (autolog)
		{
			FILE *fp = fopen("autolog", "a");
			fprintf(fp,"Captures %3d %3d  Turf %3d %3d  Score %3d %3d   ",
				capturecnt[HUMAN], capturecnt[COMPUTER],
				turfcnt[HUMAN], turfcnt[COMPUTER], h, c);
			if (h>c) fprintf(fp,"You win\n");
			else if (h<c) fprintf(fp,"I win\n");
			else fprintf(fp,"We drew\n");
			fclose(fp);
			closegraph();
			exit(0);
		}
	}
	else ShowInfo(3,"");
}

//-------------------------------------------------------------------------
// Initialise a new game

void go_t::Init()
{
	// probably unnecessary
	brd.Empty();
	SetHandicap(handicap, HUMAN);
	atari->Empty();
	for (int i = 0; i < 3; i++)
	{
		capturerow[i] = capturecol[i] = -1;
		capturecnt[i] = piececnt[i] = turfcnt[i] = 0;
	}
	movenum = gameover = 0;
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
}

//---------------------------------------------------------------
// Search for a nonempty square in a certain direction. Called
// when computing score at end; hence no board parameter.

int go_t::FindDirPlayer(int r, int c, int dr, int dc)
{
	while (brd.get(r,c)==EMPTY && brd.inpos(r+dr,c+dc))
	{
		r += dr;
		c += dc;
	}
	return brd.get(r,c);
}

//-----------------------------------------------------------
// Try to find the owner of the empty squares
// Called when computing score at end; hence no board parameter.

int go_t::FindPlayer(int r, int c)
{
#if 0
	int player1, player2;
   	player1 = FindDirPlayer(r,c,-1,0);
	player2 = FindDirPlayer(r,c,+1,0);
	if (player1)
		return ((player1 == player2) || (player2 == 0)) ? player1 : 0;
	else if (player2)
		return player2;
	player1 = FindDirPlayer(r,c,0,-1);
	player2 = FindDirPlayer(r,c,0,+1);
	if (player1)
		return ((player1 == player2) || (player2 == 0)) ? player1 : 0;
	else if (player2)
		return player2;
	return 0; // indeterminate
#else // my version
	int p = 0;
	int pn = FindDirPlayer(r,c,-1,0); if (pn) p = pn;
	int ps = FindDirPlayer(r,c,+1,0);
	if (ps)
	{
		if (p && p!=ps) return 0;
		p = ps;
	}
	int pw = FindDirPlayer(r,c,0,-1);
	if (pw)
	{
		if (p && p!=pw) return 0;
		p = pw;
	}
	int pe = FindDirPlayer(r,c,0,+1); if (pe) p = pe;
	if (pe)
	{
		if (p && p!=pe) return 0;
		p = pe;
	}
	return p;
#endif
}

//-------------------------------------------------------------------------

void go_t::CountTurf()
{
	int r, c;
	// We first remove any chains with only one liberty
	Eval(brd, COMPUTER);
	Eval(brd, HUMAN);
	for (r = 0; r < brd.Rows(); r++)
	{
		for (c = 0; c < brd.Cols(); c++)
		{
			if (brd.get(r,c) && liberties[r][c]<2)
			{
				capturecnt[brd.get(r,c)]++;
				brd.set(r,c,EMPTY);
			}
		}
	}
	// We then remove any single pieces that are completely surrounded
	for (r = 0; r < brd.Rows(); r++)
	{
		for (c = 0; c < brd.Cols(); c++)
		{
			if (brd.get(r,c) && IsSurrounded(brd,r,c,3-brd.get(r,c)))
			{
				capturecnt[brd.get(r,c)]++;
				brd.set(r,c,EMPTY);
			}
		}
	}
	if (userblack)
	{
		whitecolor=DarkGray;
		blackcolor=LightGray;
	}
	else
	{
		whitecolor=LightGray;
		blackcolor=DarkGray;
	}
	int change = 1;
	while (change)
	{
		change = 0;
		for (r = 0; r < brd.Rows(); r++)
		{
			for (c = 0; c < brd.Cols(); c++)
			{
				if (brd.get(r,c)==EMPTY)
				{
					int p = FindPlayer(r,c);
					if (p)
					{
						brd.set(r, c, p);
						brd.Print();
						turfcnt[p]++;
						change = 1;
#ifdef DEBUG
						if (debug)
							fprintf(debug,"Set %d,%d to %s\n",
								r,c, p==HUMAN?"you":"me");
#endif
					}
				}
				//else piececnt[brd.get(r,c)]++;
			}
		}
	}
	brd.Print();
}

void go_t::EndGame() // called after both pass
{
	CountTurf();
	ShowStatus();
	gameover = 1;
}

//-------------------------------------------------------------------
// OK

void go_t::SetHandicap(int hcap, int player)
{
	int mid = brd.Cols()/2;
	int left = 2;
	int right = brd.Cols() - 3;
	if (hcap) { hcap--; brd.set(left, left, player); }
	if (hcap) { hcap--; brd.set(right, right, player); }
	if (hcap) { hcap--; brd.set(left, right, player); }
	if (hcap) { hcap--; brd.set(right, left, player); }
	if (hcap)
	{
		if (hcap%2) brd.set(mid,mid,player);
		if (hcap>1) { brd.set(mid, right,player); brd.set(mid,left,player); }
		if (hcap>3) { brd.set(left,mid,player); brd.set(right,mid,player); }
	}
}

//-------------------------------------------------------------------

int go_t::Weight(const board_t* &b, int player)
{
	int rtn;
	(void)player;
	if (gameover)
		return (winner == HUMAN)? FIRSTLOSE
					: (winner ? FIRSTWIN : DRAW);
	rtn = FindChains(*((gridboard_t *)b));
	assert(rtn>FIRSTLOSE && rtn<FIRSTWIN);
	if (rtn==DRAW) rtn++;
	return rtn;
}

//-------------------------------------------------------------------

int go_t::Validate(const board_t* &bd, int player, int &from, int &to)
{
	gridboard_t *B = (gridboard_t *)bd;
	if (from != -1) return 0;
	if (passcnt>1)
		return (player==HUMAN && !autoplay); // only pruning OK
	int rt = to/B->Cols(), ct = to%B->Cols();
	if (!B->inpos(to) || B->get(to) != EMPTY || Suicide(*B,rt,ct,player))
		return 0;
	return 1;
}

//-----------------------------------------------------------

int go_t::RemoveCaptives(gridboard_t &b, int player)
{
	int captures = 0;
	if (gameover) return 0;
	Eval(b, player);
	capturerow[player] = -1;
	capturecol[player] = -1;
	for (int r = 0; r < b.Rows(); r++)
	{
		for (int c = 0; c < b.Rows(); c++)
		{
			if (b.get(r,c)==player && liberties[r][c]==0)
			{
				b.set(r,c,EMPTY);
				// save position for KO check
				capturerow[player] = r;
				capturecol[player] = c;
				captures++;
			}
		}
	}
	if (captures>1) // no KO possible
	{
		capturerow[player] = -1;
		capturecol[player] = -1;
	}
	return captures;
}

//-------------------------------------------------------------------------
// Remove a group; called when user clicks on dead groups at end

void go_t::RemoveDeadGroup(gridboard_t &b, int r, int c, int player)
{
	if(b.inpos(r,c) && b.get(r, c) == player)
	{
		b.set(r,c,EMPTY);
		RemoveDeadGroup(b,r-1,c,player);
		RemoveDeadGroup(b,r,c-1,player);
		RemoveDeadGroup(b,r+1,c,player);
		RemoveDeadGroup(b,r,c+1,player);
	}
}

//-------------------------------------------------------------------

int go_t::ApplyMove(board_t* &b_in, move_t *m, int mvnum,
		int player, moverecord_t **mr)
{
	gridboard_t *B = (gridboard_t *)b_in;
	boardmove_t *M = (boardmove_t *)m;
	int r = M->to/B->Cols(), c = M->to%B->Cols();
#ifdef DEBUG
	if (debug)
	{
		fprintf(debug,"Applying %s move %d,%d\n",
			player==HUMAN?"You":"I",r,c);
	}
#endif
	if (passcnt>1)
	{
		if (!autoplay && player==HUMAN) // dead group click
		{
			if (B->inpos(r,c) && B->get(r,c) != EMPTY)
				RemoveDeadGroup(*B,r,c,B->get(r,c));
			B->Print();
			nextplayer = COMPUTER; // make it human!
		}
	}
	else
	{
		moverecord_t *tmpmr;
		(void)movegame_t::ApplyMove(b_in, m, mvnum, player, &tmpmr);
		int catchcnt = RemoveCaptives(*B, 3-player); // remove dead opponents
		((gomove_t *)tmpmr->Move())->Captures(catchcnt);
		((gomove_t *)tmpmr->Move())->KORow(capturerow[3-player]);
		((gomove_t *)tmpmr->Move())->KOCol(capturecol[3-player]);
		tmpmr->Board(NewBoard((const board_t *)b_in));
		if (mr) *mr = tmpmr;
		if (b_in == &brd) // real move
		{
			if (!brd.inpos( ((boardmove_t *)m)->to ))
			{
				HandlePass();
				nextplayer = 3-nextplayer; // Undo side effect
			}
			else
			{
				passcnt = 0;
				capturecnt[player] += catchcnt;
				piececnt[3-player] -= catchcnt;
				piececnt[player]++;
				ShowStatus();
				ShowResult();
				// show ataris
				B->Print();
				Eval(*B,HUMAN);
				for (int r = 0; r < B->Rows(); r++)
				{
					for (int c = 0; c < B->Cols(); c++)
					{
						if (B->get(r,c)==HUMAN)
						{
							if (showatari && liberties[r][c]!=atari->get(r,c))
							{
								if (liberties[r][c]==1)
									MarkBlack(r,c);
								else if (atari->get(r,c)==1)
									MarkWhite(r,c);
							}
							atari->set(r,c,liberties[r][c]);
						}
					}
				}
			}
//			movenum++;
		}
	}
	return 0; // don't print board; already done!
}

//---------------------------------------------------------------

int Dummy(void *arg)
{
	(void)arg;
	return 0;
}

int SetHandicap(void *arg)
{
	go->handicap = (int)arg;
	return 0;
}

int HandicapMenu(void *arg)
{
	(void)arg;
	menu_t *m = new menu_t(150, 50, 11);
	assert(m);
	m->AddEntry("None", SetHandicap, (void *)0);
	m->AddEntry("One", SetHandicap, (void *)1);
	m->AddEntry("Two", SetHandicap, (void *)2);
	m->AddEntry("Three", SetHandicap, (void *)3);
	m->AddEntry("Four", SetHandicap, (void *)4);
	m->AddEntry("Five", SetHandicap, (void *)5);
	m->AddEntry("Six", SetHandicap, (void *)6);
	m->AddEntry("Seven", SetHandicap, (void *)7);
	m->AddEntry("Eight", SetHandicap, (void *)8);
	m->AddEntry("Nine", SetHandicap, (void *)9);
	m->AddEntry("Cancel", Dummy, NULL);
	m->Foreground(Black);
	m->Background(Blue);
	m->Execute();
	delete m;
	return 0;
}

int SetAtari(void *arg)
{
	(void)arg;
	go->showatari = 1-go->showatari;
	go->Eval(go->brd,HUMAN);
	for (int r = 0; r < go->brd.Rows(); r++)
	{
		for (int c = 0; c < go->brd.Cols(); c++)
		{
			if (go->brd.get(r,c)==HUMAN)
			{
				if (go->atari->get(r,c)==1)
					if (go->showatari)
						MarkBlack(r,c);
					else
						MarkWhite(r,c);
			}
		}
	}
	return 1;
}

extern void ShowBoard(const gridboard_t *b, int drawall);

int ShowChains(void *arg)
{
	*(int *)arg = 1;
	go->FindChains(go->brd);
	for (int r = 0; r < go->brd.Rows(); r++)
		for (int c = 0; c < go->brd.Cols(); c++)
		{
			if (go->brd.get(r,c)==HUMAN)
				LabelButton(r,c,go->chain[r][c], Black);
			else if (go->brd.get(r,c)==COMPUTER)
				LabelButton(r,c,go->chain[r][c], White);
			else if (go->influence[r][c]<0)
					LabelButton(r,c,-go->influence[r][c], White);
				else if (go->influence[r][c]>0)
					LabelButton(r,c,go->influence[r][c], Black);
		}
	menu_t *m = new menu_t(1, 20, 1);
	assert(m);
	m->AddEntry("Hide chains", Dummy);
	m->Foreground(Black);
	m->Background(Red);
	m->Execute();
	delete m;
	ShowBoard(&go->brd,1);
	return 0;
}

int EndGameMenu(void *arg)
{
	(void)arg;
	go->EndGame();
	ShowResult();
	return 1;
}

void go_t::AddEntries(void *m)
{
	menu_t *M = (menu_t *)m;
	M->AddEntry("End/Fill", EndGameMenu);
}

void go_t::AddOptions(void *m)
{
	menu_t *M = (menu_t *)m;
	M->AddEntry(showatari ? "Don't Show Ataris" : "Show Ataris", SetAtari);
	M->AddEntry("Show Chains", ShowChains);
	M->AddEntry("Handicap", HandicapMenu);
}

extern int optionentries;

game_t *Game(int sz, int &vertices, int argc, char *argv[],
	int &infolines, int &menuentries)
{
	vertices = 1;
	infolines = 4;
	menuentries = 1;
	optionentries = 3;
	// set lookahead to 1
	if (sz<MINBRDSZ) sz=MINBRDSZ;
	go = new go_t(sz, HUMAN, argc, argv);
	assert(go);
	assert(go->brd.Rows() >= MINBRDSZ && go->brd.Rows() <= MAXBRDSZ);
	go->CanPass(1);
	if (go->level>3) go->level = 3;
	breadth = go->level;
	return go;
}

//---------------------------------------------------------------
// Experimental stuff

void go_t::MarkDiagonal(gridboard_t &b, int cnum, int r, int c, int rr, int cc,
	int p, gridboard_t &markbd)
{
	if (markbd.inpos(rr,cc) && b.get(r,cc)!=3-p && b.get(rr,c)!=3-p)
		MarkChain(b,cnum,rr,cc,p,markbd);
}

void go_t::MarkChain(gridboard_t &b, int cnum, int r, int c, int p,
	gridboard_t &markbd)
{
	if (markbd.inpos(r,c) && markbd.get(r,c)==0 && b.get(r,c)==p)
	{
		markbd.set(r,c,1);
		chain[r][c] = cnum;
		MarkChain(b,cnum,r-1,c,p,markbd);
		MarkChain(b,cnum,r+1,c,p,markbd);
		MarkChain(b,cnum,r,c-1,p,markbd);
		MarkChain(b,cnum,r,c+1,p,markbd);
		// check diagonals
		MarkDiagonal(b,cnum,r,c,r-1,c-1,p,markbd);
		MarkDiagonal(b,cnum,r,c,r+1,c-1,p,markbd);
		MarkDiagonal(b,cnum,r,c,r-1,c+1,p,markbd);
		MarkDiagonal(b,cnum,r,c,r+1,c+1,p,markbd);
	}
}

void go_t::FindEmptyRegion(gridboard_t &b, int r, int c, int &ch, int &sz,
	gridboard_t &markbd)
{
	if (markbd.inpos(r,c))
	{
		if (!markbd.get(r,c) && b.get(r,c)==EMPTY)
		{
			markbd.set(r,c,1);
			sz++;
			FindEmptyRegion(b,r-1,c,ch,sz,markbd);
			FindEmptyRegion(b,r+1,c,ch,sz,markbd);
			FindEmptyRegion(b,r,c-1,ch,sz,markbd);
			FindEmptyRegion(b,r,c+1,ch,sz,markbd);
		}
		else if (chain[r][c])
		{
			if (ch==0) ch = chain[r][c];
			else if (ch != chain[r][c]) ch=-1; // not enclosed
		}
	}
}

void go_t::EyeSpace(gridboard_t &b, int cnum, int &neyes, int &eyespace)
{
	gridboard_t markbd(brd.Rows(), brd.Cols());
	neyes = eyespace = 0;
	for (int r = 0; r < b.Rows(); r++)
	{
		for (int c = 0; c < b.Cols(); c++)
		{
			if (markbd.get(r,c)==0)
			{
				if (b.get(r,c)==EMPTY)
				{
					int ch=0, sz=0;
					FindEmptyRegion(b,r,c,ch,sz,markbd);
					if (ch==cnum)
					{
						neyes++;
						eyespace+=sz;
					}
				}
			}
		}
	}
}

int go_t::FindChains(gridboard_t &b)
{
	int *cw, *cl, i, cnum = 1, r, c, maxi=0, rtn;
	gridboard_t markbd(brd.Rows(), brd.Cols());
	for (r = 0; r < b.Rows(); r++)
	{
		for (c = 0; c < b.Cols(); c++)
		{
			if (markbd.get(r,c)==0)
			{
				if (b.get(r,c))
					MarkChain(b, cnum++,r,c,b.get(r,c),markbd);
				else
				{
					markbd.set(r,c,1);
					chain[r][c] = 0;
				}
			}
		}
	}
	// compute the weight of each chain
	Eval(b, COMPUTER);
	Eval(b, HUMAN);
	cw = new int[cnum];
	assert(cw);
	cl = new int[cnum];
	assert(cl);
	for (i=0;i<cnum;i++) cw[i] = cl[i] = 0;
	for (r = 0; r < b.Rows(); r++)
	{
		for (c = 0; c < b.Cols(); c++)
		{
			if (chain[r][c])
			{
				cl[chain[r][c]]++;
				cw[chain[r][c]]+=liberties[r][c];
			}
		}
	}
	for (i=1;i<cnum;i++)
	{
		// the weight of the chain should now be scaled by its
		// strength (dependent on number and size of eyes)
		// should also divide by number of blocks in chain!
		int neyes, eyespace;
		EyeSpace(b, i, neyes,eyespace);
		if (neyes>2) neyes = 2;
		cw[i] *= (10+50*neyes) / cl[i];
		if (neyes) cw[i] /= (cl[i]-eyespace);
	}
	// compute the influence of chains on board
	for (r = 0; r < b.Rows(); r++)
	{
		for (c = 0; c < b.Cols(); c++)
		{
			if (chain[r][c])
				if (b.get(r,c)==HUMAN)
					influence[r][c] = -cw[chain[r][c]];
				else
					influence[r][c] = cw[chain[r][c]];
			else influence[r][c] = 0;
		}
	}
	// temp hax to show weights
	for (r = 0; r < b.Rows(); r++)
	{
		for (c = 0; c < b.Cols(); c++)
		{
			if (chain[r][c])
				chain[r][c] = cw[chain[r][c]];
		}
	}

	delete [cnum] cw;
	delete [cnum] cl;
#if 0
	// ten iterations of averaging with neighbours
	for (i = 0; i < 10; i++)
	{
		int w[MAXBRDSZ][MAXBRDSZ];
		for (r = 0; r < b.Rows(); r++)
		{
			for (c = 0; c < b.Cols(); c++)
			{
				int nb = 0;
				w[r][c] = 0;
				if (liberties[r][c] && b.get(r,c)!=EMPTY)
					continue;
				if (b.inpos(r-1,c))
				{
					nb++;
					w[r][c] += influence[r-1][c];
				}
				if (b.inpos(r+1,c))
				{
					nb++;
					w[r][c] += influence[r+1][c];
				}
				if (b.inpos(r,c-1))
				{
					nb++;
					w[r][c] += influence[r][c-1];
				}
				if (b.inpos(r,c+1))
				{
					nb++;
					w[r][c] += influence[r][c+1];
				}
				assert(nb);
				w[r][c] /= nb;
			}
		}
		for (r = 0; r < b.Rows(); r++)
			for (c = 0; c < b.Cols(); c++)
				influence[r][c] += w[r][c];
	}
#else
	int w[MAXBRDSZ][MAXBRDSZ];
	for (r = 0; r < b.Rows(); r++)
	{
		for (c = 0; c < b.Cols(); c++)
		{
			w[r][c] = 0;
			if (liberties[r][c] && b.get(r,c)!=EMPTY)
				continue;
			assert(influence[r][c]==0);
			for (int rr = 0; rr < b.Rows(); rr++)
			{
				for (int cc = 0; cc < b.Cols(); cc++)
				{
					if (influence[rr][cc])
					{
						int dist = Abs(r-rr)+Abs(c-cc);
						w[r][c] += influence[rr][cc]/(dist*dist);
					}
				}
			}
		}
	}
	for (r = 0; r < b.Rows(); r++)
		for (c = 0; c < b.Cols(); c++)
			influence[r][c] += w[r][c];

#endif
	// normalise to -99..99
	for (r = 0; r < b.Rows(); r++)
		for (c = 0; c < b.Cols(); c++)
			if (influence[r][c]>maxi || influence[r][c]<-maxi)
				maxi = Abs(influence[r][c]);
	rtn = 0;
	if (maxi==0) maxi=1; // avoid div by zero
	for (r = 0; r < b.Rows(); r++)
		for (c = 0; c < b.Cols(); c++)
		{
			influence[r][c] = (int)(99l*(long)influence[r][c] / (long)maxi);
			if (b.get(r,c)==EMPTY || liberties[r][c]==0)
				rtn += influence[r][c]/10;
		}
	return rtn; // weight of board
}

int go_t::TestResult(gridboard_t &b, int r, int c, int p)
{
	b.set(r,c,p);
	int rtn = FindChains(b);
	b.set(r,c,EMPTY);
	return p==HUMAN ? -rtn : rtn;
}

//-------------------------------------------------------------------------
// AmiGo inspired experimental stuff

void go_t::SpreadInfluence()
{
	int temp[MAXBRDSZ][MAXBRDSZ], r, c;
	for (r = 0; r < brd.Rows(); r++)
		for (c = 0; c < brd.Cols(); c++)
			temp[r][c] = 0;
	for (r = 0; r < brd.Rows(); r++)
		for (c = 0; c < brd.Cols(); c++)
		{
			int v = 0;
			temp[r][c] += influence[r][c];
			if (influence[r][c]>0) v = 1;
			else if (influence[r][c]>0) v = -1;
			if (brd.inrow(r-1)) temp[r-1][c]+=v;
			if (brd.inrow(r+1)) temp[r+1][c]+=v;
			if (brd.incol(c-1)) temp[r][c-1]+=v;
			if (brd.incol(c+1)) temp[r][c+1]+=v;
		}
	for (r = 0; r < brd.Rows(); r++)
		for (c = 0; c < brd.Cols(); c++)
			influence[r][c] = temp[r][c];
}

void go_t::Influence()
{
	for (int r = 0; r < brd.Rows(); r++)
	{
		for (int c = 0; c < brd.Cols(); c++)
		{
			switch (brd.get(r,c))
			{
				case HUMAN:
					influence[r][c] = -50;
					break;
				case COMPUTER:
					influence[r][c] = 50;
					break;
				case EMPTY:
					influence[r][c] = 0;
					break;
			}
		}
	}
	for (int i = 0; i < 4; i++)
		SpreadInfluence();
}

int go_t::CheckPos(gridboard_t &b, int r, int c, int f)
{
	int rtn = 
		(
		    (f == 0 && influence[r][c] == 0)
		 || (f > 0 && influence[r][c] >= 0 && influence[r][c] <= f)
		 || (f < 0 && influence[r][c] <= 0 && influence[r][c] >= f)
		)
		&& b.get(r,c)==EMPTY
		&& b.get(r-1,c)==EMPTY
		&& b.get(r+1,c)==EMPTY
		&& b.get(r,c-1)==EMPTY
		&& b.get(r,c+1)==EMPTY;
	if (rtn) AddMove(r,c,120);
	return rtn;
}

int go_t::CheckCorners(gridboard_t &b, int r, int c, int f)
{
	int lastcol = b.Cols()-1;
#if 0
	return (CheckPos(b,r,c,f) ||
		CheckPos(b,lastcol-r,c,f) ||
		CheckPos(b,r,lastcol-c,f) ||
		CheckPos(b,lastcol-r,lastcol-c,f));
#else
	CheckPos(b,r,c,f);
	CheckPos(b,lastcol-r,c,f);
	CheckPos(b,r,lastcol-c,f);
	CheckPos(b,lastcol-r,lastcol-c,f);
#endif
}
	
int go_t::TakeCorner(gridboard_t &b, int player)
{
	int delta = (player==HUMAN) ? -1 : 1, field = -1;
	Influence();
	for (int i = 0; i < 3; i++)
	{
		if (field == -1) field = 0;
		else if (field == 0) field = 4*delta;
		else field = -4*delta;
#if 0
		if (CheckCorners(b, 1, 2, field)
		 || CheckCorners(b, 1, 3, field)
		 || CheckCorners(b, 2, 1, field)
		 || CheckCorners(b, 3, 1, field))
			return 1;
#else
		CheckCorners(b, 1, 2, field);
		CheckCorners(b, 1, 3, field);
		CheckCorners(b, 2, 1, field);
		CheckCorners(b, 3, 1, field);
#endif
	}
	return 0;
}
