#include <dos.h>
#include <assert.h>
#include <alloc.h>

#include "display.h"

#define abs(x)	(((x) < 0) ? (-(x)) : (x) )

int cos_table[256] =
{
     512,  511,  511,  510,  509,  508,  506,  504, 
     502,  499,  496,  493,  489,  486,  482,  477, 
     473,  468,  462,  457,  451,  445,  439,  432, 
     425,  418,  411,  403,  395,  387,  379,  370, 
     362,  353,  343,  334,  324,  315,  305,  295, 
     284,  274,  263,  252,  241,  230,  219,  207, 
     196,  184,  172,  160,  148,  136,  124,  112, 
     100,   87,   75,   63,   50,   38,   25,   12,    
       0,  -12,  -24,  -37,  -49,  -62,  -74,  -87, 
     -99, -111, -123, -136, -148, -160, -172, -183, 
    -195, -207, -218, -229, -240, -251, -262, -273, 
    -283, -294, -304, -314, -324, -333, -343, -352, 
    -361, -370, -378, -387, -395, -403, -410, -418, 
    -425, -432, -438, -445, -451, -457, -462, -467, 
    -472, -477, -481, -485, -489, -493, -496, -499, 
    -502, -504, -506, -508, -509, -510, -511, -511, 
    -511, -511, -511, -510, -509, -508, -506, -504, 
    -502, -499, -496, -493, -490, -486, -482, -478, 
    -473, -468, -463, -457, -451, -445, -439, -433, 
    -426, -419, -411, -404, -396, -388, -380, -371, 
    -362, -353, -344, -335, -325, -315, -305, -295, 
    -285, -274, -264, -253, -242, -231, -219, -208, 
    -196, -185, -173, -161, -149, -137, -125, -113, 
    -101,  -88,  -76,  -63,  -51,  -38,  -26,  -13, 
      -1,   11,   23,   36,   48,   61,   73,   86, 
      98,  110,  123,  135,  147,  159,  171,  183, 
     194,  206,  217,  229,  240,  251,  262,  272, 
     283,  293,  303,  313,  323,  333,  342,  352, 
     361,  369,  378,  386,  394,  402,  410,  417, 
     424,  431,  438,  444,  450,  456,  462,  467, 
     472,  477,  481,  485,  489,  493,  496,  499, 
     501,  504,  506,  507,  509,  510,  511,  511
};

#pragma inline

void MemCopy(unsigned short dseg, unsigned short doff,
	unsigned short sseg, unsigned short soff, int cnt)
{
	asm mov ax, dseg
	asm mov es, ax
	asm mov di, doff
	asm push ds
	asm mov si, soff
	asm mov ax, sseg
	asm mov ds, ax
	asm mov cx, cnt
	asm shr cx, 1
	asm rep movsw
	asm pop ds
}

void MemSet(unsigned short dseg, unsigned short doff,
	char v, int cnt)
{
	asm mov ax, dseg
	asm mov es, ax
	asm mov di, doff
	asm mov ah, v
	asm mov al, ah
	asm mov cx, cnt
	asm shr cx, 1
	asm rep stosw
}

long Sine(int a)
{
	return cos_table[(a+192)&255];
}

long Cosine(int a)
{
	return cos_table[a&255];
}

display_t::display_t(int x_in, int y_in, int colors_in, int mode_in)
{
	// check the mode
	maxx = x_in;
	maxy = y_in;
	dim = maxx * maxy;
	colors = colors_in;
	if (mode_in>=0) mode = mode_in;
	else switch (colors_in)
	{
	case 16:
		switch(x_in)
		{
		case 320:
			assert(y_in == 200);
			mode = 0x0D;
			break;
		case 640:
			if (y_in == 200) mode = 0x0E;
			else if (y_in == 350) mode = 0x10;
			else if (y_in == 480) mode = 0x12;
			else assert(0);
			break;
		default: assert(0);
		}
		break;
	case 256:
		assert(x_in == 320);
		assert(y_in == 200);
		mode = 0x13;
		break;
	default: assert(0);
	}
	vscreen = (char far *)farcalloc(dim, 1);
	// set the mode
	_AH = 0;
	_AL = mode;
	geninterrupt(0x10);
	segment = 0xA000;
}

display_t::~display_t()
{
	if (vscreen) farfree(vscreen);
	// back to text mode
	_AH = 0;
	_AL = 3;
	geninterrupt(0x10);
}

void display_t::Clear(int color)
{
	if (dim < 65536l)
		if (vscreen)
			MemSet(FP_SEG(vscreen), FP_OFF(vscreen), color, (int)dim);
		else
			MemSet(segment, 0, color, (int)dim);
}

void display_t::Sleep() // wait for retrace
{
	while (inportb(0x3DA) & 8);
	while ((inportb(0x3DA) & 8)==0);
}

void display_t::Flush()
{
	if (vscreen)
		if (dim < 65536l)
		{
			Sleep();
			MemCopy(segment, 0, FP_SEG(vscreen), FP_OFF(vscreen), (int)dim);
		}
}

inline void display_t::Point(int x, int y, int color)
{
	if (vscreen)
		vscreen[x + y * maxx] = color;
	else
		*((char far *)MK_FP(segment, x+y*maxx)) = color;
}

RGB display_t::GetPaletteEntry(int color)
{
	RGB rtn;
	outportb(0x3C7, color);
	rtn.R = inportb(0x3C9);
	rtn.G = inportb(0x3C9);
	rtn.B = inportb(0x3C9);
	return rtn;
}

RGB *display_t::GetPalette()
{
	RGB *rtn = new RGB[colors];
	for (int i = colors; i--;)
		rtn[i] = GetPaletteEntry(i);
	return rtn;
}

void display_t::SetPaletteEntry(int color, char R, char G, char B)
{
	outportb(0x3C8, color);
	outportb(0x3C9, R);
	outportb(0x3C9, G);
	outportb(0x3C9, B);
}

void display_t::SetPaletteEntry(int color, RGB &rgb)
{
	SetPaletteEntry(color, rgb.R, rgb.G, rgb.B);
}

void display_t::SetPalette(RGB *pal)
{
	for (int i = colors; i--;)
		SetPaletteEntry(i, pal[i]);
}

void display_t::Blackout(char R, char G, char B)
{
	Sleep(); // wait for retrace
	for (int i = colors; i--;)
		SetPaletteEntry(i, R, G, B);
}

void display_t::FadeIn()
{
	assert(colors == 256);
	RGB *palfinal = GetPalette();
	Blackout(0,0,0);
	RGB *palnow = GetPalette();
	for (int i = 0; i < 64; i++)
	{
		Sleep(); // wait for retrace
		for (int p = colors; p--; )
		{
			if (palnow[p] != palfinal[p])
			{
				if (palnow[p].R != palfinal[p].R)
					palnow[p].R++;
				if (palnow[p].G != palfinal[p].G)
					palnow[p].G++;
				if (palnow[p].B != palfinal[p].B)
					palnow[p].B++;
				SetPaletteEntry(p, palnow[p]);
			}
		}
	}
	delete [] palnow;
	delete [] palfinal;
}

void display_t::FadeOut()
{
	assert(colors == 256);
	RGB *pal = GetPalette();
	RGB *oldpal = GetPalette();
	for (int i = 0; i < 64; i++)
	{
		Sleep(); // wait for retrace
		for (int p = colors; p--; )
		{
			if (pal[p] != 0)
			{
				if (pal[p].R) pal[p].R--;
				if (pal[p].G) pal[p].G--;
				if (pal[p].B) pal[p].B--;
				SetPaletteEntry(p, pal[p]);
			}
		}
	}
	Clear(0); Flush();
	SetPalette(oldpal);
	delete [] oldpal;
	delete [] pal;
}

void display_t::Circle(int x, int y, int r, int color)
{
	for (int i = 0; i < 256; i++)
	{
		long dX = (r * Cosine(i))/512l;
		long dY = (r * Sine(i))/512l;
		Point(x+dX, y+dY, color);
	}
}

#define sgn(x) ((x) ? ((x)<0 ? -1 : 1 ) : 0)

// Bresenham's line algorithm; no clipping

void display_t::Line(int startx, int starty, int endx, int endy, int color)
{
	int i, deltax, deltay, numpixels,
	    d, dinc1, dinc2,
	    x, xinc1, xinc2,
	    y, yinc1, yinc2;
	// Calculate deltax and deltay for initialisation
	deltax = abs(endx - startx);
	deltay = abs(endy - starty);
	// Initialize all vars based on which is the independent variable
	if (deltax >= deltay)
	{
		// x is independent variable
		numpixels = deltax + 1;
		dinc1 = deltay << 1;
		d = dinc1 - deltax;
		dinc2 = (deltay - deltax) <<1;
		xinc1 = xinc2 = yinc2 = 1;
		yinc1 = 0;
	}
	else
	{
		// y is independent variable
		numpixels = deltay + 1;
		dinc1 = deltax << 1;
		d = dinc1 - deltay;
		dinc2 = (deltax - deltay) << 1;
		xinc1 = 0;
		xinc2 = 1;
		yinc1 = 1;
		yinc2 = 1;
	}
	// Make sure x and y move in the right directions
	if (startx > endx)
	{
	      xinc1 = - xinc1;
	      xinc2 = - xinc2;
	}
	if (starty > endy)
	{
		yinc1 = - yinc1;
		yinc2 = - yinc2;
	}
	// Start drawing at <startx, starty>
	x = startx;
	y = starty;

	// Draw the pixels
	for (i = numpixels; i-->0; )
	{
		Point(x, y, color);
		if (d < 0)
		{
			d += dinc1;
			x += xinc1;
			y += yinc1;
		}
		else
		{
			d += dinc2;
			x += xinc2;
			y += yinc2;
		}
	}
}

#include <stdio.h>

int main()
{
	const colors = 256;
	display_t d(320, 200, colors); // ONLY USE 320x200x256 for now!!
	d.Clear(0);
	d.Flush();
	for (int i = 0; i < colors; i++)
	{
		d.Line(i + 32, 1, 288 - i, 199, i);
		d.Circle(10+i, 10+i, 2+i/4, i);
		d.Flush();
	}
	d.Flush();
	sleep(2);
	d.FadeOut();
	return 0;
}

