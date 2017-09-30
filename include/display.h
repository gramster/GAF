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
	c]ÃÍNW¸ÅØÅ?ÒÅ`ñ
7»S~ëîa¸÷\ûŒ˜k3r&é5Ohü~iš +Ú“Ü$ê‡m²«n#š7# øyr”Nğc:ş±‘ºP¥6R¼&n:l#UböV<¶v;Uj +=KÀ•@k \–î1™pw–éJµaéÉ˜w–^…p‰¾°Ô8(§€|­„Z@ö¿^B* ß(!`7KÈhyN	ù+¶p«„\W@¾]B.ÀrKÈ‡
Èwt¥%äDİQ60£èJCNb¡ü)ºÒ%$©¿M	Ù¸˜¼¤„<£˜ì¿4œX`<»˜ì\BöŸ_Lv)!ÓŠÉË°…ÅÅäå%df1Ù[`“W”×“İÂrî“İy¯˜Ì,!›æJ¥îºÒ3Uä¢²çŸòj]i%IZiŞÆÈ?åßƒğ¸s¥^Ë¥‡ïi —Q( ·æ‘û
È¿°qc´ºPîz°$­äbÉİ’g%ïJÆKº"8 ÊpàÂ[àçãÏÅ#ø½x`Mp!¬%@!^ãˆF·ÀkšjÚh¦j‚/šİš«‰Wˆ@[k¦Ör-?-@"YâIGH`æ”zííam ¯³KçÎŸÉ§ÜÍ×ÍÒµÓ›ôöêÓ;£nè5êië‡éƒ$}ƒGŸ@«y1yì%g†ºFàQ»QÆÔKSÁ©.Æ¾Æ…ÆÀÈd§I…É/À2Í5­1ıc
FM{§AÓwNO¦¿şezût°vFÄŒc3\f‚À™‚™+ÌôgëY‹f­uk aÚÑ'óvsàkqËod	YVX‡Á`­ÕşÙ6ómÀ›r›F›N0n£m;İÖÎ,¶õ°•ØvÚb:Ã,»ùv,;ğÌ®ß®bÎ÷9 sÆ\¿¹×æ‚²¹usçÏcÌ~ónÌ{2ïı<ğcŞ
û0ûãöàš}¹ıûA{@t˜é°ÈÁÃø:ìw¸âğÆ<š¿•âãæ:r¼æXèŞ;şq$.@;'ÜUÄÿçIq‰‹ˆù.Ğ´-µ NMkøoRğßƒâP¯ÿcõg@Â±òYUªàÉÇÄÿà@ä-ÆÊ{aEºße¶RyA\Xƒ<üR*#weÕÔ"®bAÜíÑ°IáGïâï‚ø ÊŞãçæ(8àe-áçJİÖ¾o„Ÿó€¸²†Ü~™­X6~;§ ­úÃhiœÀ×ïùìñÁqg„!¨ğÃj†¤Íj‹ªB8‘Ğåp$]#]^ğÈoã~w†K>}ä·9»Õv”<q‰=4ì˜!àÙeXlÚ•.Ï¿ó®µµÿPkoùÇñÁBûdè»i|VŞÀÆ!>òTxO¼lrüøQ±hbÉ[r/ágÏ4roß7HâÃÀøÕNÂO•:¢ıß(9VáÄ0‚b[OãN¤}šQy‰½[®:ÔúZùîwn#"¶%dŸ<,±…»öP—÷É¶Û›xÙ­t»îFÛ³Æ‚68†Ÿ<eôy×Ç–‘úŸ}_ä-àë¡š0ÕH[g®gPŞ?[¢3ÆSğğ}ÓÇxºªoÃmòÕŠñ,+tZ#†ÛBLí3pKM8áüä-¿ínIÁÛ–µ7‘§}ÉPÿ¸Äşïivs00ö­†®J°7c\±³IÏVØ£uN±0.×;¢Õ÷ñ?¥”ø^çÏXDkBÿV¡ú¤úœtVS;áçù1CµU¿ Ú3uÆÔ=­G´—˜íi]k¾ÉÿIÓ®É¼Ë\m®G1§|$®|7jl|Ğt5%²p;%p¥‘ò6ÊRúØ¼§M”\?£aÿËi4ÈSmä®ğj‚YŸrçó&½´52x<g	|Ïwg„vÌÎt]:—`Tî”{®/hİY5,Â‚&¿àÖ‡£,­|Õ‡@Eøˆƒá'{Ä>ÖD7†áÛB?b~fÈÄ0Vá1ÌhÅš„#´ëy}yvŒ–‘a½ßŒ–1ÅğT£e,¶Aê¾ÃGäÚ")#Úù¡„=¯[0t°±âG† ôñw
Ë
“¹Zg5~ÿR[ìk5x\Z+_=„ IøCµrä+†4ÿ`WY¥É†2êÕ¿ZUã–uº*ÿØ•ÓÉ{#–CwçÎ=CçB(HòãRÁœp˜d:˜	‡µR_x°ÁÂø}×7%)šŞ(VlÃÙ1Üš3*Ó.ï€ó¨÷Ñ×Û}ùÜ»ç9È‰ëHÑŞFñBÎ>9 Ë^é‚‡×/ç†1,¦ñˆ°uŒĞ÷7‰ûäşËoQ<‡8~Ë7r:£¡ Ö-‘l¤X»Q¼±/•SËQ&rüÈ9¡w¾né	8Ì6
Uzs8	$¤¨™)†l>‰\ØÀügö²°$$Tú†>¯Ùº3Tìƒ!Ì»Øœ?Y—R—s_ÎKò+f_/ñ½¦«ºvĞaQØÀ‘A–2ú5¬Šü«ÑÂœœnN|—âaşò^úÖş[Aè˜jr²Ùµ˜²,QN©¨ÖMX­ÅTÁÓü*rlØ@à·—ß+ù±	ª…ÓV­„½Ï˜¯I_ş¾u=ã:RLRsŠC‰ÜµMıqŞ‡½O{_÷~X›Rµ]Pâ]íİäİå­ö\ÓÒï°dVnä>ˆÕé­Æg}"Üe3Æıü
ãîĞªi)'ƒ},å·_á™aßú*w¾îéˆ<­ÓušÉhù8íp¬ön„#ùìÎC¹5÷½|ƒOõÉqŒíª,^Â™ÜÈ=òEÙ™a}K)ğ¡Ä¬ÄE‰…‰Ÿ4µÃOYw-Qôô¾2•éÑpœ¬hp’ªÁ U8û\s8b‡bktc»Oç¬Ô8-5x¼è#C-ˆPfù)í8/•ßØíø‡tãªnTI»q%ëÚuí¼¼›WˆÒWI9³qlIdÂ¥NïÔ…G.Ôv…If£ğØí°óŠŞ’›_®Mô‹­3hiğw‹­÷-âïªµ­Ù8æ[´=Ei[£ö-Y)~ö¼2áœ1µ,Œ_æ$éÑïHïOsÿ°Œå—dÊ8 ù|ğÂd’ìù2–Ià÷Â@y>»=¬fÔ5‹Ñ²úåcF‹óµBó)AS%»µšØ¼ºğ½Ùyİ—SÒ[Tµ¢ÆR¯JØ2Ä¡nm2Ùg=•&ËŞA>clæY #¾›-òFK	@æ
á´äÏ…JóHwßÇ_cŸˆ5*|`M8ÈŠä&‰¶b{Ì?ìO§PQŸáñ'£Ã6ê3z;‹ÈÑêG*¨\5ûÒØÔ¼Sy×ò&iÁ¹ŸòÚóúïğƒÅ–…º…¿ùO:oÿ•XI÷CCaoØÀÖ¿3µQB0o}SHÓî¦ä¦ÉùOİkê¿,nùS§[ä$ùSg¿cDÓpºŞ½ )l í6İë–~xs¹:ısŸ2—5Ğã2ªêè›ÆõõÙê·È°;¤WxÔDÓ×ıV„?«	Ã^Çê*Â&(j-PÑ¢×U¹³¦‹ÚôçÀÿofªDÀ#t_òèfëFÏÌ®íèÄ,¾Î`œ¦°S(ç;óÂ~¸"2¦¥².ŸoÈ@’#naZ’%w4Ê:y8Â*?±u™ı`o•ù7ò¬®‰(~¢qcâ¢¼÷:Ï:İõ`_½­z“,ñ‡ œ)šyâ—z¶ymzJ{|áã¨eBQïÔöŞı?CNÆ8ÿvêŞkFzM“·”ÓÜ]¡fÅh‹o˜X8WSˆ”+é·õŠõ¨UzNæ+Í{læ*UÓVù!'g`Ù`Ñ˜Oïjx¥†ˆÕ?±^Gù†Û½8¾|Ìg4ÄÊèì1Ÿ± +İ™yËºã¼Ì
ÃY:«×”µüõ«?Å]Š}áq—ÏÍ]LÓMıq!öÏ§vIÜíŞÂ,·»Ë´™œ:khÙC7œ{ÖĞ¼N³Q/fµxŒèÍû•ÏnŞr“m!\ºÙNğX™ùæïœ•£¶.Ó`,ã¸'ÖÈMhlå$Ë3¬½>4œp–¼c0õ¬aàâ¶b›ç9ÅnÅ=±°/‰š¶já¾%³±¹6„ÑFfnj”Y|“9TY}>{-ewÎ&(ãö!¯{=b…–lÍgÀ„]î…ÙmÛÃóÓW¤ŠE£:ö+éhq¾G|´íˆkÈÏZ´Ë;im%æ
>H°*5›¾‹•in´š1•3ÃL±ìÔ#wï¬–
*Éªzuî¬bVÇ£Ygsª‘ßo‹ïpØ*8‹­Sõ,ÔdÌh7x<Æ´ql]39Í¶i°Ølº+ş‹ìò‘o²¹ JÒÄ##ÅjvúÚ½DeğØG)øÿ3¡x¸êoÙJ÷5fÓd3ê›ı©TĞã±œã·„ÙÓR×™ÎoM×‹YKxkßn^²óï/Èöí”®]óºşã=·wÍß×%jE§‰;¢–lÍiÕ±4»ÕIÛµcy“WSz” ;*/ª$ª:ª)ª+ê]Ñ5³˜¯ŸìÍb†c:Íb÷~§@£üy?#ŞS|ôşyX’Pê¢N<§ ÌùùÙü§ù€¹&Å„Ò£K¨h_š¾:=0="}oz¯9+JæİH/ıY”?ù>]ãGú°&4”®•=-{Nö’ìÙcxI›é´mÉa+’¶ùÚñÙœé/dßÉö˜7†7¯;Êh8ãøÓ›šä½û2;¡´¢ondN¶6—dÏÈÖÜy6orbGReŞ×¼!¢QO®Ä°Äªd'­dˆXÔbúævKbåçGÇŞŞ<[yñJy®ıoƒ£â$ê½²""2Ôâ*À=}mYY&z0›s{ÃÖ/ÕéƒeãÕºŸóâK}ÊÈuLÎS—„Î«m—»øûošŞ÷º}	ñƒüãš«é¶G[¯´HàsÆa’¶cŸÊh:/-I,I¿ÿ‰®zB3ş<çî9F ÂENÚë¹ğ¾lÊRÙã*ÉÚ'…oGpûø[²90¨Lıƒs.÷eäĞœ²µÜŠ³M7›ı09{Øôª¶Ûàax…€° %âÖ7OÆÂ+jX“Aƒª1è	í8¯®Áq*»6TYw½mpéT7ñ»|»öwI»ò»2ºXæ¬/…İ5¿Ó¿)»ÆÇº[Hè{tôİÏÅ âK´GXŸ¥¯^®^î¥
-ı©hJà$³{›ïŸüUàöyAÊˆ ,î.Ä—MûTz{CHÇÔ
NÓ¢Rªë$0–¾Ø¹ÒÏ£Œ=~œñ»çÍ¦—½¡o“?•Æ–²›|sW¯ŒP)tv°»?•Şµ…Î˜ÄLÕÛ7ıTFY„O•ıó “<Ü-9*yQAV|kìvö—¸Ùœ3ö“\zB3Prû>Ê?·±¶õÁ	|ÓÜ¬Æï‡²ŞêÆ¼ÅEùoKÅ^¨Ö^3LÊÊqÖvh,4Ó@‰‘i\›˜©ºîû¦G€™î	ÏÊk/s¹c˜qË6.2q™²#Ğv5Á`=i§m°YcEyŒÖ„ää}²âĞÏÚÖªëXs;ÄØ‡öÜ'»lßĞ`b¶r§¶µ?Åné¹åç»ïËş9ÃÕÇÎX_,´Ô)ËÚı	Äº#™È2öHÌe€Í±À²
à=ÎAÆeKœô¯sÎ w×R<–z¯f±á~§m]d‹5ZÚ]hë'yWh(ùÊ²¿ÿû^®Úê0;I´eğ|à¿³õV¤‚ğù“SÂNÎ{iN.“ÒZ8çØ1ñd*]'*ËlëÔ¸&ê·ÃÄô§ß=XhrüĞ7şö´›‘ßî·#Ü7äı“{IÖÍìs¥¼§zÏövòzSï˜¸×xaæ•»'=¢ü/Ö{••ˆw–LêK®»Ÿd/[0£’åoVÏÌÛàıwıï”ë¹šU_l9§¨zxjË=(ç–®¦ø«ì\ú£êFï×éãŞ´;Ô¯ÇNy¯Ø›«©s+pI”gøOÑ—Q¹šo÷F-ÍÕ¼s.+âpé‘ñ—‘¹šûl²¶åjFT‡çjúgEC¢—‘bÚ_–å3öÕ¡¹šN_Ä· K©Ô÷;îû;ÜĞ¢ÄÀ_nAáaü÷;÷fåTfFwØ•(¹.8xı³¸ÏA¶OU–ïŒC©ß¡R¾ŸAjİ×Ñ©o>" eä›O¡H÷ÉYBor$p*ÍdÑusêÄw+ºŸıB „ÇFÊö³‘ee	±€PÃ¿ÓÚc	RÆ FBø†RĞ†tçl@æcÌxÛßBfEÿÉÖNßÏŞÇ¼Ó:ØdÙ	aCS›ƒå&"ıt.ÁMHš±bÆ%g—	À§JİPÁ„ä²8Åá„LQôi–†‚Év»´Ûz²÷c#O9‘»ÙX¾œ
}`Kì§®HqÁœğáÉJœ-ùìZŸ²¹á­6ÄÙ6aÊÖ‹ÚÔÚ/J\¹0–µMĞÀ&í„$Mõ”5ÜÀaKJ:êSzŸŠ÷zİ4Hıö:ºœ_û:¶êÙ‡B%?e$·FéÿæHÖŠS×ŠjóëÓmÒ	ÏR–§£ñL¯õé!‚mé­ËS„w—	Îüå8:f[<''G`uúcßÔÇ{Ï¹äÑÜpwNN‘ôñˆæÎ-¹ß·Õ5êÈ…9¹šëo§n¹r1ğ±ôæ“;ylëú¦~©¼vac²ôJ3©9°3ƒ×²-Š€5£¾ÈvœCó7Õ4vºlÃ &rbp(œïÌº1¬Mg–6ˆqCYÚ(W·³ücÑÆãïSfå·rvM—‚É™TRÜÄ ÎÔNlb1ehÂÓĞÅÒívh€†"%‰üBw’¦‰_,&uÜÓnˆ¸£:7kbÉ›ÂÀÊ!ß™¢Gqš²¡óJÖæ‚J-$bpù};QL¾(@Ãn¨Ò“ø¬`ğ/Ïÿš mWá©Ûôç]¾^õ]¾A¤š*Ï7Ä¡/QSÀÇMÚ¢oH%±PÃ¢èˆA~ÇÔ{SfßÏ+/.©*ùVraşBeÁÂ…=%¸Ê½•´ŒúåùƒĞKæãœYºqŒÜ£˜BË–æÃV¥)Ü­%1.îeXñ'«`üæË¨K…áõ&$£·¡V/Ås«—Ê&J»İª9ñù{q(x+,5t(~¿ù˜ÁùdzN2ƒƒAeó^bĞø3½¶Y‡oÇ;¸šZÍ¨>ò³  eAAÍ–j|š·POö[kwƒ_ÒWı£(—œÖtìÓÕ&eÁ¿§m‹?².×Ij†jŒ]×ëâËËƒµùgß-ùr¢nDŒn”O<äZtE²ëQLŠñ®úîs;¦Äùù…#Æ‡ş¤æëwÌ–çƒW(öÊét,M&/öíË©¶}õÄwİìáÊ¾¦‰×æõûòRšô]›ìD	~zÏÿs¨íc‡øËïÏ•¼K…›:ª;~µ~í:]ÃVx,¿?¯G×QİğpN‹Ñü–SŞßÔúÈ˜oœº¡4<øRahÇyÍİ›‡Ë­ŠrÕäšuµZ[ò­™ ¿ºi wÛø£¬İlÿK…ë;Ô~íyW’’>­Ğ3¨ıc0¶Gïé—‚Î¿2ê”‘âŒ—¥/½0–MX,ÀùçctÛrk­ği`/NªµœœşrFğı-ÙµÀÙG´s ±D!ÖW(½k½¿)	ù÷Ú4ğ/ŸêrW¢ÅSó[g”Ì{DŠqñ, ÆÜv’|t/˜ëyßÊç~ÒuÄ8«àû%¾ËL×‰â'Ö?Yo8%À@©6U¾¹ Ääª,Ò|¿Ñ°©²çıó'ŒNÏ bô’5ê*^áiäP…ª{èT¥Ó$wÖ,\ÅÎYoçü×Ã(Îb¾­)Ù9ûiÇéÚ¡ÆT"·#„¦håz ‹«@Y£Á!›{ˆøò î?ógï±˜vĞVŸÚâìa÷Yó¬seÎ—ü¹EcK–ì-%¢¦»p–¾ìv¼k«ÄøYË¾\õÀ€GU¹„’™{¬¡È¯o†¬…
ëKø>:\qoÁb>nH9‹‹wâ$ê•s¤`ñ.œ¤,… ÙÕ¸ä«|ÜÕ]¸·E¶ë-Šœ—–ÉÃB÷Øa’>Àø–°»@Şô¨ú÷x¦ÿÿî`Æ°9&‘'¡ƒÔH$ÉšN2§©„I-¯¤†MÇ1©4O4TêY^9T:¼öjtÒA†¢’:—Ì¡ĞZæ,Ìóûôz¾¿ßåûË<÷ÜÓöÜÓãyÜî·ÛõÍ²òˆÌµvE‘t¤NON¶¸ùby–¨õ¹š7ÛájQ‰?‡¨ø!*6í6iÀ,µ;g)™GÁÖÏ:ş‘½•­º£ë2ÿ¢dŞYLO²J[U*å»Õ-Á':ìï¶ÚM¶õAÌ	¥ã‡ı³ÙŞIêşT»_I‹/ù¾´Ôî¶‰ïİé«÷ €»£X€Ñ¨~x”%’>Û?‡–¡x#§Ê}Ô,ªã/§gûœøTNô_N=>Ëö`nû\7
b<äÜBÜ	{0\#ER‰•Säó§‚pG‹èõŸy¢'ÚNbpgTPš‚)xò H¡ÔQ]:´6óõ7ñz§BOËPÄGA€Ã<LXÆ‰©ÂÒÒÔ—,²k¥É’»ıº'$u7i)„Gîéı4],)¬CÍ$)Ho/&§§r×§3ì¥v“Bßï*_#¨ÚßYG NÇ¢Ú/\óÈ
®q¤dsì#®^dy%W³¨¦òO?(¨8Å³e×ÑµæËŸ8Ysï®L²µY(‡˜)VØ$©Òi©õ\v[Uyú!Ú\ÍR0+UªxÕgÕ®ŸWRnU‹y†f;c|’lnt¬°R:Í9¾€œk­µ •j•¶óqæm8~íîÙK“lc(«6/´NÚÁ5€Bği;e™{ĞJ%ÎÅ É¢ÍY²œøŞÉêù 0Ëõ•;ê2F³ø¿şØµuÕl)–½J¤ëLÔqe‡\Æ|§ğ= Ï¢!Ñ[Df{˜@ÅÉš~Eıó‹_ñ¿&§ ¹O€·úV(ê' aáÉ„Y.écSÄQ{súSdÚE¸$‡¥².OSUS‚È\©à•;áÂQá•½YÇ¡!Ëq®Aã8wVEæ~z63=œ å®¼Mçj£ÆúÿËß©ÈõékÓ
>Pd“º
 ÂQMúböÔ?Ù¿ ÜîŞ€Õ¿O‰›cV§£íl!ğ=Ôï¾ì?ÙéÙb„í a<ÃJ¨)l.ì(8MÔ.4+\tÚ«O´<W[l&&ØçÓV/Ë?ŸM/œíÃ9cX†=TÈÌx{¦0Ax±ôùxû„KC•ÖF€ÍÌ’ªr¡Ô†t©5ûfÁ‡"}—âµbûü½Q—GÅ„*ôP‹=YW¤…ƒ…1UIUG€²\AzÃsväâò¨¾7 ûæÕŠú÷Wú¹¿	a‹%T»§ú6UÎUöù[7Üy^õ±
kŸ?½¡Š_µ¸iUÓÆ&|½i‰$J'½SÑ5šn6áÅ@Óê‡4UÕ×¦zÛóD/–˜#fœ¯œ=Ôqzß£é$'¾‡ïq|ëYñ·ì{âäìĞ9Ç6¯¶ä6ÿd3c(m»İV×¶I©”µÉÛTm'Ú¸öÒ}mõü{}_‹Ó è²ovH¶ÒE)Ì¼}î R$TšÓj”«Nğ~(G{¦ ŒZ_ÍVV«ßL®RoTÿæwOóõÒ]æË&&=$€e"Nƒû”ô#¡‹£jOHüB×=yXÍ39rO‰¨§ ,Ï‘‡mÇ“ñ°DòØñnK!RcyX|ü’†¢°Ü5“||ÖñHïss4Ìş‡]¹¶Bê""6S_m”6“º¾MË-NØ^³àı›&¶Å‚³§(‚YÙ…&uuw,¢]¶èaQ¿‚n°¥§Íùƒ&uÁ¾óƒjÛ6fÁ”ákY!…7Ï^<®É«¬Ñœíé0¿à¤¯ÍŠNõ z—š||ô`áH¯–j\—8œe‘}ì¡ÃK‡N“:•ï‚8\v¬õÑËV•¾à?û<R=rK£ı:°FxiîÚÉ‚GõZÒDj6Ü…¿h·a1ƒ¶€»}¸ŠëÄ y“=úƒ1ÛØ'š>è»6~ê ï.Ê‰›ÿbÌ=JF{$®5b
ü¡ˆ>¢cŠzö¯/AíbÖŠ“u^«IL¶âv…tèïò~û~¤%l(<ºèı««+VÙ=åµ#Œ#R8µ¿Di²+‚ÍæŒ2ÉÁ†%CEX«ËVïêKÏ?Ñ÷_£gBëßGüˆ0©ÓYÏ™ÛøEMö?*&?_ê?8ÒûµÚŸhÆ®tÆ'G„Fà¬B6lkmtç]¶z»®>¢#~ “èc²È7åÏ7©ó¤îxÀâ›Ô_ÍYÇjß…–ªû[!†ê>5áŒ½%ª/ìà†ÄÍìFuø.;”‹y?è­!0¬IlHüÌù–H¤ÇŸdÆopô5˜ë³ííÜw„n@>]Ã&ß³|á"ÜøË!#±-1?í÷YÊ¼øg7ÓùùüÛÔËÜÙ³Mêäç6U4	È…–…K/V Ù”laqúÒSDöÎ_s×¦äO…·n$±]|{©ØEèMx®7Í9ãà­¨¤ìÑlNöeuU|‚²7¢p®ñ÷Áh<Š ™äúĞ¸cP
B@Mù*æÚ¡İø¨0]Ó