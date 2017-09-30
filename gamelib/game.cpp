#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "game.h"

// Game class. This primarily provides a command-line arg processor
// for generic args, a main driver routine, and a generic minimax 
// routine.

#if __BORLANDC__
#include <dos.h>
#include <conio.h>
extern void beep(int freq, int duration);
#endif

#define ABPRUNE // enable alpha-beta pruning

#ifdef DEBUG
FILE *debug;
int showsearch = 0;
#endif

generator_t::generator_t(const board_t* &b_in, const int move_in,
	const int p_in)
{
	b = (board_t *)b_in;
	pos = 0;
	player = p_in;
	movenum = move_in;
}

game_t::~game_t()
{
#ifdef DEBUG
	if (debug)
	{
		fclose(debug);
		debug = NULL;
	}
#endif
	while (gamerecord)
	{
		moverecord_t *tmr = gamerecord;
		gamerecord = gamerecord->Next();
		delete tmr;
	}
}

const char *genargs =
"[-d] [(-c | -h)] [-f] [ -a[<limit>] [-p] ] [-T] [-B] [level]\n"
"where:\n\t-d enables debugging trace\n\t-c specifies computer start\n"
"\t-h specifies human start\n\t-f uses a faster search\n"
"\t-a causes autoplay for the specified number of moves\n"
"\t-p waits for a keypress after each move (non-graphics only)\n"
"\t-T enters test mode\n\t-B makes human black\n"
"\t<level> specifies the tree search level (default 6)\n";

game_t::game_t(board_t *b_in, int firstplayer_in, int argc, char **argv)
{
	b = b_in;
	gamerecord = NULL;
	maxprune = paws = test = errorstate = movenum = 0;
	automoves = autoplay = autolog = blockwins = blockloses = 0;
	canpass = userblack = 0;
	level = 6;
	nextplayer = firstplayer = firstplayer_in;
	for (int arg = 1; arg < argc; arg++)
	{
		if (argv[arg][0]=='-')
		{
			switch(argv[arg][1])
			{
#ifdef DEBUG
			case 'd': 
				debug = fopen("debug", "w");
				break;
#endif
			case 'h': // human starts
				nextplayer = firstplayer = HUMAN;
				break;
			case 'c': // computer starts
				nextplayer = firstplayer = COMPUTER;
				break;
			case 'f': // fast
				maxprune = 1;
				break;
			case 'p': // paws
				paws = 1;
				break;
			case 'a': // autoplay
				automoves = 500;
				if (isdigit(argv[arg][2]))
					automoves = atoi(&argv[arg][2]);
				if (automoves==0)
				{
					automoves = 9999;
					autolog = 1;
				}
				break;
			case 'T': // test mode
				test = 1;
				break;
			case 'B': // user plays black
				userblack = 1;
				break;
			default: errorstate = 1;
			}
		}
		else if (isdigit(argv[arg][0]))
			level = atoi(argv[arg]);
		else errorstate = 1;
	}
	printf("Playing at level %d\n", level);
}

//--------------------------------------------------------------
// Game save and restore

void game_t::SaveGame(FILE *fp)
{
	moverecord_t *mr = gamerecord;
	fprintf(fp,"First %d\n", firstplayer);
	if (mr) mr = mr->Next(); // skip dummy entry
	int mcnt = movenum;
	while (mr && --mcnt>0)
	{
		mr->Move()->Write(fp);
		fputc('\n', fp);
		mr = mr->Next();
	}
}

void game_t::LoadGame(FILE *fp)
{
	assert(gamerecord); // must have first move
	Init();
	fscanf(fp,"First %d", &firstplayer);
	nextplayer = firstplayer;
	(void)RecordMove(0, NewBoard((const board_t *)b), NULL);
	movenum = 1;
	move_t *m = NewMove();
	while (m->Read(fp))
	{
		(void)ApplyMove(m, movenum++, nextplayer);
		nextplayer = 3-nextplayer;
		b->Print();
	}
	delete m;
}

//--------------------------------------------------------------
// Game recorder and move undoer

moverecord_t *game_t::GetMove(int movenum_in)
{
	moverecord_t *m = gamerecord;
	while (movenum_in-->0)
	{
		assert(m);
		m = m->Next();
	}
	assert(m);
	return m;
}

moverecord_t *game_t::RecordMove(int movenum_in, board_t *board_in, move_t *move_in)
{
	moverecord_t *m = gamerecord, *p = NULL,
		*newm = new moverecord_t(board_in, move_in, NULL);
	assert(newm);
	while (movenum_in-->0)
	{
		assert(m);
		p = m;
		m = m->Next();
	}
	if (m) // Replacing existing entries? Delete 'em...
	{
		if (p) p->Link(NULL);
		while (m)
		{
			moverecord_t *tm = m;
			m = m->Next();
			delete tm;
		}
	}
	if (p) p->Link(newm);
	else gamerecord = newm; // first move (guaranteed by assert)
	return lastmove = newm;
}

void game_t::EraseMove(int movenum_in)
{
	moverecord_t *m = gamerecord, *p = NULL;
	assert(movenum_in>0);
	while (movenum_in-->0)
	{
		assert(m);
		p = m;
		m = m->Next();
	}
	assert(p);
	lastmove = p;
	p->Link(NULL);
	while (m)
	{
		moverecord_t *tm = m;
		m = m->Next();
		delete tm;
	}
}

//---------------------------------------------------------------
// Simple minimaxer

#ifdef DEBUG
int id = 0;
#endif
#ifdef DRAWTREE
long Xpositions[10];
int drawtree = 0;
extern void DrawTreeNode(long x, int y, const board_t* &b, int val);
extern void DrawLink(int ypos, long px, long cx);
#endif

int game_t::FindMove(const int lookahead, const int mvnum, const int player,
	const board_t* &b_in, move_t* &move, int pweight, generator_t *pgen)
{
	int rtn = UNKNOWN;
	move_t *nxtmv, *mv;
	generator_t *gen;
#ifdef DEBUG
	int Id = id++, cnt=0;
#endif
#ifdef DRAWTREE
	int ypos;
	long cpos[64];
	if (drawtree)
	{
		if (pgen==NULL)
		{
			for (int i = 0; i < 10; i++)
				Xpositions[i] = 0l;
		}
		ypos = (pgen==NULL) ? 0 : (pgen->ypos+1);
		assert(ypos < 10);
	}
#endif
	if (lookahead <= 0 || GameOver(b_in, player))
	{
#ifdef DEBUG
	if (debug)
		fprintf(debug, "findmove %d lookahead %d player %d pweight %d terminal %d\n",
			Id, lookahead, player, pweight, Weight(b_in, player));
#endif
		rtn = Weight(b_in, player); // terminal node
#ifdef DRAWTREE
		if (drawtree)
		{
			Xpositions[ypos]+=6;
			DrawTreeNode(Xpositions[ypos], ypos, b_in, rtn);
		}
#endif
		return rtn;
	}
	gen = Generator(b_in, mvnum, player, pgen);
#ifdef DRAWTREE
	if (drawtree)
	{
//		gen->xpos = xpos; // should be able to eliminate these;
			// just need depth
		gen->ypos = ypos;
	}
#endif
	for (	mv = gen->NextMove(),
		CopyMove(move, mv)
	      ;	mv
	      ;	mv = gen->NextMove())
	{
		board_t *nb = NewBoard(b_in);
		(void)ApplyMove(nb, mv, mvnum, player);
		if (showsearch) nb->Print();
		move_t *m = NewMove();
#ifdef DEBUG
		if (debug)
		{
			fprintf(debug, "findmove %d lookahead %d player %d pweight %d ",
				Id, lookahead, player, pweight);
			PrintMove(mv, "checking ", debug);
			nb->Print(debug);
		}
#endif

		int wn = FindMove(lookahead-1, mvnum+1,
				(player == COMPUTER) ? HUMAN : COMPUTER,
				(const board_t* &)nb, m, rtn, gen);
		delete m;
		delete nb;

#ifdef DRAWTREE
		if (drawtree)
			cpos[cnt] = Xpositions[ypos+1]; // record child pos
#endif
		cnt++;
		if (player == COMPUTER) // maximising?
		{
#ifdef ABPRUNE
			if (wn >= pweight && pweight!=UNKNOWN)
			{
				rtn = wn;
#ifdef DEBUG
				if (debug)
				{
					fprintf(debug, "findmove %d pruning at weight %d ",
						Id, rtn);
					PrintMove(mv, "", debug);
				}
#endif
				goto done; // prune
			}
#endif
			// The random choice here must only be at lookahead
			// one as pruning confuses it.
#if __BORLANDC__
			if (wn > rtn || rtn==UNKNOWN || (lookahead==1 && wn==rtn && random(2)))
#else
			if (wn > rtn || rtn==UNKNOWN || (lookahead==1 && wn==rtn && random()%2))
#endif
			{
				rtn = wn;
				CopyMove(move, mv);
#ifdef DEBUG
				if (debug)
				{
					fprintf(debug, "findmove %d new best weight %d ",
						Id, rtn);
					PrintMove(mv, "", debug);
				}
#endif
				if (maxprune && wn >= FIRSTWIN)
					goto done; // can't do better!
			}
		}
		else // human move; minimise
		{
#ifdef ABPRUNE
			if (wn <= pweight && pweight!=UNKNOWN)
			{
				rtn = wn;
#ifdef DEBUG
				if (debug)
				{
					fprintf(debug, "findmove %d pruning at weight %d ",
						Id, rtn);
					PrintMove(mv, "", debug);
				}
#endif
				goto done; // prune
			}
#endif
			if (wn < rtn || rtn==UNKNOWN)
			{
				rtn = wn;
				CopyMove(move, mv); // for debugging
#ifdef DEBUG
				if (debug)
				{
					fprintf(debug, "findmove %d new best weight %d ",
						Id, rtn);
					PrintMove(mv, "", debug);
				}
#endif
				if (maxprune && rtn <= FIRSTLOSE) // can't do better
					goto done;
			}
		}
		delete mv;
		EraseMove(mvnum); // take out of record & fix lastmove
	}
done:
	if (mv)
	{
		delete mv;
		EraseMove(mvnum);
	}
	delete gen;
	if (rtn == UNKNOWN) rtn = 0;
	if (rtn < (FIRSTLOSE-25))
		rtn+=25; // prolong the agony; maybe opponent will slip up
	else if (rtn > (FIRSTWIN+25))
		rtn-=25; // win as fast as possible
	if (cnt == 0)
		if (blockwins)
			rtn = (player==HUMAN) ? FIRSTWIN : FIRSTLOSE;
		else if (blockloses)
			rtn = (player==COMPUTER) ? FIRSTWIN : FIRSTLOSE;
#ifdef DEBUG
		else
		{
			fprintf(stderr,"Minimax error (player %d)! Check generator/gameover\n",
				player);
			b_in->Print(stderr);
			assert(0);
		}
	if (debug)
	{
		fprintf(debug, "Leaving findmove %d lookahead %d player %d pweight %d chose (%d)",
				Id, lookahead, player, pweight, rtn);
		PrintMove(move, "", debug);
	}
#endif
#ifdef DRAWTREE
	if (drawtree)
	{
		long lastchild = cpos[cnt-1];
		long mid = cpos[0] + (lastchild - cpos[0])/2l;
		if (mid < (Xpositions[ypos]+6l))
			mid = Xpositions[ypos]+6l; // avoid cramp
		Xpositions[ypos] = mid;
		DrawTreeNode(mid, ypos, b_in, rtn);
		for (int i = 0; i < cnt; i++)
			DrawLink(ypos, mid, cpos[i]);
		// Make sure further nodes are drawn to the right...
		for (i = ypos+1; i < 10; i++)
			if (Xpositions[i] < Xpositions[i-1])
				Xpositions[i] = Xpositions[i-1];

	}
#endif
	return rtn;
}

void game_t::TestGenerator()
{
	while (b->GetTestLayout())
	{
		for (int p = 1; p <= 2; p++)
		{
			generator_t *g = Generator((const board_t *)b, 0, p);
			move_t *m;
			while ((m = g->NextMove()) != NULL)
			{
				m->Print(stdout);
				delete m;
			}
			delete g;
		}
		printf("Weight for Computer (1) is %d\n", Weight(b, COMPUTER));
		printf("Weight for Human    (2) is %d\n", Weight(b, HUMAN));
		move_t *m = NewMove();
		(void)FindMove(level, 0, COMPUTER, (const board_t* &)b,
			m, UNKNOWN);
		printf("Computer would "); m->Print(stdout);
		delete m;
		m = NewMove();
		(void)FindMove(level, 0, HUMAN, (const board_t* &)b,
			m, UNKNOWN);
		printf("Human would "); m->Print(stdout);
		delete m;
	}
}

void game_t::AutoMove()
{
	move_t *m = NewMove();
	// Useful hack to test if lookeahead does any good
	if (nextplayer==HUMAN)
		(void)FindMove(1, movenum, nextplayer, (const board_t* &)b,
			m, UNKNOWN);
	else
		(void)FindMove(level, movenum, nextplayer, (const board_t* &)b,
			m, UNKNOWN);
	if (showsearch) b->Print(); // restore
	int prt = ApplyMove(m, movenum, nextplayer);
	delete m;
	if (prt) b->Print();
	nextplayer = 3 - nextplayer;
	movenum++;
}

int game_t::HumanMove(char *mvbuf)
{
	move_t *m;
	if ((m = TranslateMove(mvbuf)) == NULL)
		return -1;
	int prt = ApplyMove(m, movenum, nextplayer);
	delete m;
	if (prt) b->Print();
	nextplayer = 3 - nextplayer;
	movenum++;
	return 0;
}

void game_t::Initialise()
{
	passcnt = 0;
	if (errorstate)
		Useage(genargs);
	Init();
	nextplayer = firstplayer;
	// store initial board as move zero
	RecordMove(0, NewBoard((const board_t *)b), NULL);
	movenum = 1;
#if __BORLANDC__
	randomize();
#endif
	if (test)
		TestGenerator();
	autoplay = automoves;
};

void game_t::ShowResult()
{
#if __BORLANDC__
	gotoxy(1,23);
		cputs("                       ");
	gotoxy(1,23);
	if (Lose())
		cputs("You win");
	else if (Win())
		cputs("I win");
	else
		cputs("We drew");
#else
	if (Lose())
		puts("You win");
	else if (Win())
		puts("I win");
	else
		puts("We drew");
#endif
}

#if 1

void game_t::Play() // stub
{
}

#else

void game_t::Play()
{
	Initialise();
	b->Print();
	for (;;)
	{
		move_t *m;
		if (autoplay)
		{
			autoplay--;
			if (autoplay==0) break;
#if __BORLANDC__
			if (paws) while (!kbhit());
#endif
		}
#if __BORLANDC__
		if (kbhit())
			if (getch()==27)
				break;
#endif
		if (!autoplay && nextplayer==HUMAN)
		{
#if __BORLANDC__
			char _mvbuf[80], *mvbuf = _mvbuf+2;
			gotoxy(1,23);
			cprintf("                                                 ");
			gotoxy(1,23);
			cprintf("Your move? ");
			_mvbuf[0]=10;
			cgets(_mvbuf);
#else
			char mvbuf[80];
			printf("Your move? "); fflush(stdout);
			gets(mvbuf);
#endif
			if (*mvbuf=='q' || *mvbuf=='Q') exit(0);
			if (HumanMove(mvbuf) != 0)
			{
#if __BORLANDC__
				gotoxy(1,24);
				cputs("Invalid move!");
				beep(3000,50);
				sleep(1);
				gotoxy(1,24);
				cputs("             ");
#else
				puts("Invalid move!");
#endif
				continue;
			}
		}
		else AutoMove();
		if (GameOver()) break;
	}
	ShowResult();
}


extern game_t *Game(int sz, int argc, char *argv[], int &);

void main(int argc, char *argv[])
{
	int sz = 3;
	if (argc>=2 && isdigit(argv[argc-1][0]))
	{
		argc--;
		sz = atoi(argv[argc]);
	}
	game_t *game = Game(sz, argc, argv);
	game->Play();
	delete game;
}
#endif
