#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <string.h>

typedef enum
{
	Black,		Blue,		Green,		Cyan,
	Red,		Magenta,	Brown,		LightGray,
	DarkGray,	LightBlue,	LightGreen,	LightCyan,
	LightRed,	LightMagenta,	Yellow,	White,
	DefaultColor
} color_t;

typedef enum
{
	CopyPut, XOrPut, OrPut, AndPut, NotPut, SpritePut
} paintmode_t;

// Red-green-blue tuples

class RGB
{
public:
	char R, G, B;
	int operator!=(const char &rhs)
	{
		return (((R<<10)|(G<<5)|B) != rhs); // check order!!
	}
	int operator!=(const RGB &rhs)
	{
		return (R != rhs.R || G != rhs.G || B != rhs.B);
	}
};

//---------------------------------------------------------------

class display_t
{
	int		maxx,
			maxy;
	long 	dim,
			colors;
	short	segment;
	int		mode;
	char far *vscreen;
public:
	display_t(int x_in, int y_in, int colors_in, int mode_in = -1);
	~display_t();
	RGB GetPaletteEntry(int color);
	RGB *GetPalette();
	void SetPaletteEntry(int color, char R, char G, char B);
	void SetPaletteEntry(int color, RGB &rgb);
	void SetPalette(RGB *pal);
	void Blackout(char R, char G, char B);
	void FadeIn();
	void FadeOut();
	void Flush();	// make buffer and screen identical
	void Sleep();
	void Clear(int color);
	void Point(int x, int y, int color);
	void Line(int startx, int starty, int endx, int endy, int color);
	//void Rectangle(int startx, int starty, int endx, int endy);
	//void FilledRectangle(int startx, int starty, int endx, int endy);
	//void Arc(int centerx, int centery, int start, int end, int radius);
	void Circle(int centerx, int centery, int radius, int color);
	//void FilledCircle(int centerx, int centery, int radius);
};

#endif



class display_t
{
	int		maxx,
			maxy;
	long 	dim,
			colors;
	short	segment;
	int		mode;
	char far *vscreen;
public:
	display_t(int x_in, int y_in, int colors_in, int mode_in = -1);
	~display_t();
	RGB GetPaletteEntry(int color);
	RGB *GetPalette();
	void SetPaletteEntry(int color, char R, char G, char B);
	void SetPaletr [bp+12]
	out	dx,al
	?debug	L 191
	mov	dx,969
	mov	al,byte ptr [bp+14]
	out	dx,al
	?debug	L 192
	mov	dx,969
	mov	al,byte ptr [bp+16]
	out	dx,al
	?debug	L 193
	pop	bp
	ret	
	?debug	C E60474686973180206000005636F6C6F72040A0A+
	?debug	C 00000152020A0C00000147020A0E00000142020A+
	?debug	C 100000
	?debug	E
	?debug	E
@display_t@SetPaletteEntry$qizczczc	endp
	?debug	L 195
	assume	cs:DISPLAY_TEXT
@display_t@SetPaletteEntry$qim3RGB	proc	far
	?debug	B
	push	bp
	mov	bp,sp
	?debug	C F82240646973706C61795F744053657450616C65+
	?debug	C 747465456E7472792471696D33524742
	?debug	C E31C000400351A00
	?debug	C E6037267621C0A0C000005636F6C6F72040A0A00+
	?debug	C 0004746869731802060000
	?debug	B
	?debug	L 197
	les	bx,dword ptr [bp+12]
	mov	al,byte ptr es:[bx+2]
	push	ax
	les	bx,dword ptr [bp+12]
	mov	al,byte ptr es:[bx+1]
	push	ax
	les	bx,dword ptr [bp+12]
	mov	al,byte ptr es:[bx]
	push	ax
	push	word ptr [bp+10]
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	cs
	call	near ptr @display_t@SetPaletteEntry$qizczczc
	add	sp,12
	?debug	L 198
	pop	bp
	ret	
	?debug	C E60474686973180206000005636F6C6F72040A0A+
	?debug	C 0000037267621C0A0C0000
	?debug	E
	?debug	E
@display_t@SetPaletteEntry$qim3RGB	endp
	?debug	L 200
	assume	cs:DISPLAY_TEXT
@display_t@SetPalette$qn3RGB	proc	far
	?debug	B
	push	bp
	mov	bp,sp
	push	si
	?debug	C F81C40646973706C61795F744053657450616C65+
	?debug	C 74746524716E33524742
	?debug	C E60370616C1B0A0A000004746869731802060000
	?debug	B
	?debug	L 202
	les	bx,dword ptr [bp+6]
	mov	si,word ptr es:[bx+6]
	jmp	short @14@86
@14@58:
	?debug	L 203
	mov	ax,si
	mov	dx,3
	imul	dx
	mov	dx,word ptr [bp+10]
	add	dx,ax
	push	word ptr [bp+12]
	push	dx
	push	si
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	cs
	call	near ptr @display_t@SetPaletteEntry$qim3RGB
	add	sp,10
@14@86:
	?debug	L 202
	dec	si
	jne	short @14@58
	?debug	L 204
	pop	si
	pop	bp
	ret	
	?debug	C E601690404060004746869731802060000037061+
	?debug	C 6C1B0A0A0000
	?debug	E
	?debug	E
@display_t@SetPalette$qn3RGB	endp
	?debug	L 206
	assume	cs:DISPLAY_TEXT
@display_t@Blackout$qzczczc	proc	far
	?debug	B
	push	bp
	mov	bp,sp
	push	si
	?debug	C F81B40646973706C61795F7440426C61636B6F75+
	?debug	C 7424717A637A637A63
	?debug	C E60142020A0E00000147020A0C00000152020A0A+
	?debug	C 000004746869731802060000
	?debug	B
	?debug	L 208
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	cs
	call	near ptr @display_t@Sleep$qv
	pop	cx
	pop	cx
	?debug	L 209
	les	bx,dword ptr [bp+6]
	mov	si,word ptr es:[bx+6]
	jmp	short @15@86
@15@58:
	?debug	L 210
	mov	al,byte ptr [bp+14]
	push	ax
	mov	al,byte ptr [bp+12]
	push	ax
	mov	al,byte ptr [bp+10]
	push	ax
	push	si
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	cs
	call	near ptr @display_t@SetPaletteEntry$qizczczc
	add	sp,12
@15@86:
	?debug	L 209
	dec	si
	jne	short @15@58
	?debug	L 211
	pop	si
	pop	bp
	ret	
	?debug	C E601690404060004746869731802060000015202+
	?debug	C 0A0A00000147020A0C00000142020A0E0000
	?debug	E
	?debug	E
@display_t@Blackout$qzczczc	endp
	?debug	L 213
	assume	cs:DISPLAY_TEXT
@display_t@FadeIn$qv	proc	far
	?debug	B
	push	bp
	mov	bp,sp
	sub	sp,8
	push	si
	push	di
	?debug	C F81440646973706C61795F744046616465496E24+
	?debug	C 7176
	?debug	C E604746869731802060000
	?debug	B
	?debug	L 215
	les	bx,dword ptr [bp+6]
	cmp	word ptr es:[bx+6],256
	jne	short @16@86
	jmp	short @16@114
@16@86:
	mov	ax,215
	push	ax
	push	ds
	mov	ax,offset DGROUP:s@+512
	push	ax
	push	ds
	mov	ax,offset DGROUP:s@+498
	push	ax
	push	ds
	mov	ax,offset DGROUP:s@+458
	push	ax
	call	far ptr ___assertfail
	add	sp,14
@16@114:
	?debug	L 216
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	cs
	call	near ptr @display_t@GetPalette$qv
	pop	cx
	pop	cx
	mov	word ptr [bp-2],dx
	mov	word ptr [bp-4],ax
	?debug	L 217
	mov	al,0
	push	ax
	mov	al,0
	push	ax
	mov	al,0
	push	ax
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	cs
	call	near ptr @display_t@Blackout$qzczczc
	add	sp,10
	?debug	L 218
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	cs
	call	near ptr @display_t@GetPalette$qv
	pop	cx
	pop	cx
	mov	word ptr [bp-6],dx
	mov	word ptr [bp-8],ax
	?debug	L 219
	xor	di,di
	jmp	@16@478
@16@142:
	?debug	L 221
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	cs
	call	near ptr @display_t@Sleep$qv
	pop	cx
	pop	cx
	?debug	B
	?debug	L 222
	les	bx,dword ptr [bp+6]
	mov	si,word ptr es:[bx+6]
	jmp	@16@422
@16@198:
	?debug	L 224
	mov	ax,si
	mov	dx,3
	imul	dx
	mov	dx,word ptr [bp-4]
	add	dx,ax
	push	word ptr [bp-2]
	push	dx
	mov	ax,si
	mov	dx,3
	imul	dx
	mov	dx,word ptr [bp-8]
	add	dx,ax
	push	word ptr [bp-6]
	push	dx
	call	far ptr @@RGB@$bneq$qmx3RGB
	add	sp,8
	or	ax,ax
	jne	@@2
	jmp	@16@422
@@2:
	?debug	L 226
	mov	ax,si
	mov	dx,3
	imul	dx
	les	bx,dword ptr [bp-8]
	add	bx,ax
	mov	al,byte ptr es:[bx]
	push	ax
	mov	ax,si
	mov	dx,3
	imul	dx
	les	bx,dword ptr [bp-4]
	add	bx,ax
	pop	ax
	cmp	al,byte ptr es:[bx]
	je	short @16@282
	?debug	L 227
	mov	ax,si
	mov	dx,3
	imul	dx
	les	bx,dword ptr [bp-8]
	add	bx,ax
	inc	byte ptr es:[bx]
@16@282:
	?debug	L 228
	mov	ax,si
	mov	dx,3
	imul	dx
	les	bx,dword ptr [bp-8]
	add	bx,ax
	mov	al,byte ptr es:[bx+1]
	push	ax
	mov	ax,si
	mov	dx,3
	imul	dx
	les	bx,dword ptr [bp-4]
	add	bx,ax
	pop	ax
	cmp	al,byte ptr es:[bx+1]
	je	short @16@338
	?debug	L 229
	mov	ax,si
	mov	dx,3
	imul	dx
	les	bx,dword ptr [bp-8]
	add	bx,ax
	inc	byte ptr es:[bx+1]
@16@338:
	?debug	L 230
	mov	ax,si
	mov	dx,3
	imul	dx
	les	bx,dword ptr [bp-8]
	add	bx,ax
	mov	al,byte ptr es:[bx+2]
	push	ax
	mov	ax,si
	mov	dx,3
	imul	dx
	les	bx,dword ptr [bp-4]
	add	bx,ax
	pop	ax
	cmp	al,byte ptr es:[bx+2]
	je	short @16@394
	?debug	L 231
	mov	ax,si
	mov	dx,3
	imul	dx
	les	bx,dword ptr [bp-8]
	add	bx,ax
	inc	byte ptr es:[bx+2]
@16@394:
	?debug	L 232
	mov	ax,si
	mov	dx,3
	imul	dx
	mov	dx,word ptr [bp-8]
	add	dx,ax
	push	word ptr [bp-6]
	push	dx
	push	si
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	cs
	c]��NW��؁�?��`�
7�S~��a���\���k�3r&�5Oh�~i� +ړ�$�m��n#�7# �yr�N�c:����P�6R�&n:l#Ub�V<�v;Uj�+=K��@k \��1�pw
�wt�%�D�Q�60��JCNb��)��%$��M	ٸ����<���4��X`<���\B��_Lv)!ӊ�˰�����%df1�[`�W������r���y���,!��J���3U����j]i%IZiށ��?��߃�s�^˥��i �Q( ����
ȿ�qc��P�z�$��b�ݒg%�J�K�"8 �p��[�����#��x`Mp!�%@!^�F��k�j�h�j�/�ݚ��W�@[k��r-?-@"Y��IGH`�z��am���K�Οɧ����ҵ������;�n�5�i��$}�G�@��y1y�%g���F��Q�Q��KS���.ƾƅ���d�I��/�2�5�1�c
FM{�A�wNO����ez�t�vFČc3\f�����+��g�Y�f��uk a��'�vs�kq�od	YVX��`����6�m��r�F�N0n�m;���,�����v�b:�,��v,;�̮߮b��9�s��\���悲�us��c�~�n�{2��<�c�
�0�����}���A{@t������:�w����<������:�r��X��;�q$.@;'�U���Iq����.д-� NMk�oR�߃�P��c�g@±�YU����ā��@�-��{aE��e�RyA\X�<�R*#we��"��bA��ѰI�G���������
�
��Zg5~�R[�k5x\Z+_=��I�C�r�+�4�`WY�Ɇ2�տZU�u�*�ؕ��{�#�Cw��=�C�B(H��
Uzs8	$���)�l>�\���g���$$T��>���3T��!̻؜?Y�R�s_�K�+f_/񽦫�v�aQ���A�2��5���������nN|��a���^���[A��jr��ٵ��,QN���MX��T���*rl�@���+���	���V���Ϙ�I_��u=�:RLRs�C�ܵM�qއ�O{_�~X�R�]P�]������\���dVn�>����g}"�e3���
��Ъi)'�},�_�a��*w���<��u��h�8�p��n�#���C�5��|�O��q���,^��=�E��a}K)�Ĭ�E����4��OYw-Q���2���p��hp��� U8�\s8b�bktc�O��8-5x��#C-�Pf�)�8/���
�����J�Hw��_c���5*|��`M8Ȋ�&��b{�?�O�PQ���'��6�3z;�����G*�\5���ԼSy��&i�����������������O:o��XI�CCao��ֿ3�QB0o}SH�����O�k�,n�S�[�$�Sg��cD�p�޽�)l �6����~xs�:�s�2�5��2��������Ȱ;�Wx�D���V�?�	�^��*�&(j-PѢ�U���������o
�Y:�ה����?�]�}�q���]L�M�q!�ϧvI����,������:kh�C7�{�мN�Q/f��x�����Ϟn�r�m!�\��N�X�����.�`,�'��Mhl�$�3��>4�p��c0��a���b��9�n�=��/���j�%���6��Ffnj�Y|�9TY}>{-ew�&(��!�{=b���l�g��]���m����W��E�:��+�hq�G|�툎k��Z��;im%�
>H�*5����in��1�3�L���#w��
*ɪzu�bVǣYgs���o��p�*8��S�,�d�h7x<ƴql]39Ͷi��l�+����o�� J�č##�jv�ڽDe��G)��3�x��o�J�5f�d3���
-��hJ�$�{���U��yAʈ ,�.ėM�Tz{CH��
NӢR��$0��ع�ϣ�=~���ͦ���o�?�Ɩ��|sW��P)tv��?�޵�����L��7�TFY�O��� �<�-�9*yQAV|k�v���ٜ3��\zB3Pr�>�?������	|�ܬ������Ƽ�E�oK�^��^3L��q�vh,4�@��i\�������G�����	Ϟ�k/s�c�q�6.2q��#�v5�`=i�m�YcEy�ք��}�����֪�Xs;�؇��'�l��`b�r���?�n鎹
�=�A�eK���s� w�R<�z�f���~�m]d�5Z�]h�'yWh(�ʲ����^���0;I�e�|࿳�V��������S�N�{iN.��Z8��1�d*]'*�l���&������=Xhr��7�������#�7���{I���s���z��v�zS�xa敻'=��/�{���w�L�
}`K짝��Hq�����J�-��Z���ᐭ6��6a������/J�\�0��M��&�$M
�K��>:\qo�b>nH9��w�$�s�`�.��,� �՝��|��]��E��-������B��a�>�����@�����x�����`ư9&�'���H$��N2���I-���M�1�4O4T�Y^9T:��jt�A���:�̡�Z�,����z�������<�������y�����Ͳ�̵vE���t�NON���by�����7��jQ�?���!*6�6i�,�;g)�G���:���������2��dގYLO�J[U*��-�':��ڏM��A�	������I���T�_I�/�������� ���X�Ѩ~x��%�>�?����x#��}�,��/�g���TN�_N=>��`n�\7
b<��B�	{0\#ER��S��pG����y�'�NbpgTP��)x� H��Q]�:�6��7�z�BO�P�GA��<LXƉ�������,�k��������'$u7i)�G���4],)��C�$)Ho/&��rק3�v�B��*_#���YG��NǢ�/\��
�q�ds�#�^dy%W����O?(�8ųe����˟8Ys�L��Y(��)V�$���i��\v[Uy�!�\�R0+U�x�gծ�WRnU�y�f;c|�lnt��R:�9���k�� ��j���q�m8~���K�lc(�6/�N��5�B�i;e�{�J%�� ��͎Y������� 0���;�2F����صu�l)��J��L�qe�\�|��= Ϣ!�[Df{�@�ɚ~E��_�&���O���V(�'�a�ɄY.�cS�Q{s�Sd�E��$���.OSUS��\���;��QᕽYǡ!�q�A�8w�VE�~z63�=� ���M�j�����ߩ���k�
>Pd��
 �QM�b��?ٿ ��ހտO���cV���l!�=���?���b���
k�?���_��iU��&�|�i�$J'�S�5��n6��@��4U�צz��D/��#f���=�qzߣ�$'���q|�Y��{����9�6���6�d3c(m��V׶I�����Tm'ڸ��}m��{}_�� �ovH���E)̼}�R$T��j��N�~(G{� �Z_�VV��L�RoT��wO���]��&&=$�e"N�����#���jOH�B�=yX�39rO��� ,ϑ�mǓ��D���nK!RcyX�|������5�||��H�ss4���]��B�""6S_�m�6���M�-N؞�^����&�ł��(�Y��&uuw,��]��a�Q��n������&u���j�6f���kY!�7�^<��ɫ�ќ��0�य͊N��z��||�`�H��j\�8�e�}��K�N�:��8�\v����V���?�<R=rK��:�Fxi����G�Z�Dj6܅�h�a1����}���Ġy��=��1��'�>�6~��.ʉ��b�=JF{$�5b
���>�c�z��/A�b���u^�IL��v�t���~�~�%l(<�����+V�=��#�#R8��Di�+����2���%CE�X
B@M�*�ڡݐ��0]�