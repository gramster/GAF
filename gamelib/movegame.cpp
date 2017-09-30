#include "movegame.h"

//-----------------------------------------------------------------
// Placement move generator

move_t *placegen_t::NextMove()
{
	while (++pos < ((gridboard_t *)b)->Size())
	{
		if (Get(pos) == EMPTY)
		{
			move_t *rtn = (move_t *)new boardmove_t(-1, pos);
			assert(rtn);
			return rtn;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------
// pawn move generator & validator

int CheckPawnMove(const board_t* &bd, int player, int from, int to)
{
	const gridboard_t *B = (const gridboard_t *)bd;
	short c = B->Cols(), sz = B->Size();
	if (from<0 || from>=sz || B->get(from)!=player)
		return 0;
	if (to<0 || to>=sz)
		return 0;
	int rf = from/c, cf = from%c,
		rt = to/c, ct = to%c;
	if (player==HUMAN)
	{
		if (rt != (rf - 1)) return 0;
	}
	else
	{
		if (rt != (rf + 1)) return 0;
	}
	if (ct == cf)
		return (B->get(to) == EMPTY);
	else
	{
		// Validate a take move
		if (B->get(to) != 3-player) return 0;
		if (cf==0)
		{
			if (ct!=1) return 0;
		}
		else if (cf==(sz-1))
		{
			if (ct!=(sz-2)) return 0;
		}
		else
		{
			if (ct != (cf-1) && ct != (cf+1))
				return 0;
		}
	}
	return 1;
}

int CheckPawnMove(const board_t* &bd, int player, move_t *m)
{
	return CheckPawnMove(bd, player, ((boardmove_t *)m)->from,
		((boardmove_t *)m)->to);
}

move_t *pawngen_t::NextMove()
{
	for (;;)
	{
		if (cnt>=3 || Get(from)!=player)
		{
			from++;
			if (from>=sz)
				return NULL; // no more moves
			cnt = 0;
			to = from + ((player==HUMAN) ? -cols : cols) - 1;
			continue;
		}
		cnt++;
		if (to>=0 && to<sz)
		{
			if (cnt==2)
			{
				if (Get(to) == EMPTY)
				{
					move_t *rtn = (move_t *)new boardmove_t(from, to++);
					assert(rtn);
					return rtn;
				}
			}
			else if (Get(to) == 3-player)
			{
				// if cnt is 1 mustn't be on left edge
				// if cnt is 3 mustn't be on right edge
				if ((cnt==1 && from%cols) ||
				    (cnt==3 && (from+1)%cols))
				{
					move_t *rtn = (move_t *)new boardmove_t(from, to++);
					assert(rtn);
					return rtn;
				}
			}
		}
		to++;
	}
}

//----------------------------------------------------------------------

int CheckOrthoMove(const board_t* &bd, int player, int from, int to)
{
	const gridboard_t *B = (const gridboard_t *)bd;
	if (!B->inpos(from) || B->get(from)!=player)
		return 0;
	if (!B->inpos(to) || B->get(to)!=EMPTY)
		return 0;
	short c = B->Cols(), r = B->Rows();
	int rf = from/c, cf = from%c,
		rt = to/c, ct = to%c;
	// check if the two are neighbours orthogonally
	if (rf == rt)
	{
		if (cf && ct == (cf-1))
			return 1;
		if (cf<(c-1) && ct == (cf+1))
			return 1;
	}
	else if (cf == ct)
	{
		if (rf && rt == (rf-1))
			return 1;
		if (rf<(r-1) && rt == (rf+1))
			return 1;
	}
	return 0;
}

int CheckOrthoMove(const board_t* &bd, int player, move_t *m)
{
	return CheckOrthoMove(bd, player, ((boardmove_t *)m)->from,
		((boardmove_t *)m)->to);
}

int orthogen_t::Validate(int &from, int &to)
{
	return CheckOrthoMove((const board_t *)b, player, from,to);
}

move_t *orthogen_t::NextMove()
{
	for (;;)
	{
		if (cnt>=lim || Get(from)!=player)
		{
			from++;
			if (from>=sz)
				return NULL; // no more moves
			cnt = 0;
			continue;
		}
		cnt++;
		if (cnt==5) to = -1;
		else if (cnt%2) to=from+2-cnt; // left/right
		else to=from+(3-cnt)*cols; // up/down
		if (Validate(from,to))
		{
			move_t *rtn = (move_t *)new boardmove_t(from, to);
			assert(rtn);
			return rtn;
		}
	}
}

//----------------------------------------------------------------------
// king moves to empty squares only!

int CheckKingMove(const board_t* &bd, int player, int from, int to)
{
	const gridboard_t *B = (const gridboard_t *)bd;
	short c = B->Cols(), r = B->Rows(), sz = B->Size();
	int rf = from/c, cf = from%c, rt = to/c, ct = to%c;
	if (from<0 || from>=sz || B->get(from)!=player)
		return 0;
	if (to<0 || to>=sz || B->get(to)!=EMPTY)
		return 0;
	if (CheckOrthoMove(bd, player, from, to))
		return 1;
	// check if the two are neighbours diagonally
	if (rf && cf && rt==(rf-1) && ct==(cf-1))
		return 1;
	if (rf<(r-2) && cf && rt==(rf+1) && ct==(cf-1))
		return 1;
	if (rf && cf<(c-1) && rt==(rf-1) && ct==(cf+1))
		return 1;
	if (rf<(r-1) && cf<(c-1) && rt==(rf+1) && ct==(cf+1))
		return 1;
	return 0;
}

int CheckKingMove(const board_t* &bd, int player, move_t *m)
{
	return CheckKingMove(bd, player, ((boardmove_t *)m)->from,
		((boardmove_t *)m)->to);
}

move_t *kinggen_t::NextMove()
{
	for (;;)
	{
		if (cnt>=8 || Get(from)!=player)
		{
			from++;
			if (from>=sz)
				return NULL; // no more moves
			cnt = 0;
			continue;
		}
		switch(cnt++)
		{
		case 0:	to = from - cols - 1;		break;
		case 3:	to = from-1;			break;
		case 4:	to = from+1;			break;
		case 5:	to = from + cols - 1;		break;
		default:to++;				break;
		}
		if (CheckKingMove((const board_t *)b, player, from, to))
		{
			move_t *rtn = (move_t *)new boardmove_t(from, to);
			assert(rtn);
			return rtn;
		}
	}
}

//----------------------------------------------------------------------

move_t *movegame_t::TranslateMove(char *buf)
{
	int from, to, cnt;
	cnt = sscanf(buf, "%d %d", &from, &to);
	if (cnt==0) // Try letters; needed for games > size 9; ZZ is off-board
	{
		char c1,c2,c3,c4;
		cnt = sscanf(buf, "%c%c %c%c", &c1, &c2, &c3, &c4);
		if (cnt != 2 && cnt != 4) return NULL;
		if (c1>'Z') c1 -= 'a'; else c1 -= 'A';
		if (c2>'Z') c2 -= 'a'; else c2 -= 'A';
		if (cnt == 2)
		{
			from = -1;
			to = c1 * brd.Cols() + c2;
		}
		else
		{
			from = c1 * brd.Cols() + c2;
			if (c3>'Z') c3 -= 'a'; else c3 -= 'A';
			if (c4>'Z') c4 -= 'a'; else c4 -= 'A';
			if (c3==25 && c4==25)
				to = -1;
			else to = c3 * brd.Cols() + c4;
		}
	}
	else // numbers
	{
		if (cnt==1)
		{
			to = from;
			from = -1;
		}
		else if (cnt!=2) return NULL;
		if (to>=0) to = (to/10-1)*brd.Cols()+(to%10-1);
		if (from>=0) from = (from/10-1)*brd.Cols()+(from%10-1);
	}
	boardmove_t *rtn = new boardmove_t(from, to);
	assert(rtn);
	if (Validate((const board_t *)b, HUMAN, rtn))
		return rtn;
	else delete rtn;
	return NULL;
}

int pawngame_t::LocatePawn(const board_t* &b, int player, int flags)
{
	gridboard_t *B = (gridboard_t *)b;
	for (int p=0; p<B->Size(); p++)
	{
		if (B->get(p)==player)
		{
			int off = (player==HUMAN)? (-B->Cols()) : B->Cols();
			if ((flags & CANADVANCE) && CheckPawnMove(b,player,p, p+off))
				return p;
			if ((flags & CANTAKE) &&
				(CheckPawnMove(b,player,p, p+off-1)
				 || CheckPawnMove(b,player,p, p+off+1)))
				return p;
		}
	}
	return -1;
}

//----------------------------------------------------------------

