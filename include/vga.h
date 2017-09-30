/* VGA/EGA Registers */

#define VREG_HT		0x00	/* Horizontal total			*/
#define VREG_HDEE	0x01	/* Horizontal Display Enable End	*/
#define VREG_SHB	0x02	/* Start horizontal blanking		*/
#define VREG_EHB	0x03	/* End horizontal blanking		*/
#define VREG_SHR	0x04	/* Start horizontal retrace		*/
#define VREG_EHR	0x05	/* End horizontal retrace		*/
#define VREG_VT		0x06	/* Vertical total			*/
#define VREG_O		0x07	/* Overflow				*/
#define VREG_PRS	0x08	/* Preset row scan			*/
#define VREG_MSLA	0x09	/* Maximum scan line address		*/
#define VREG_CS		0x0A	/* Cursor start				*/
#define VREG_CE		0x0B	/* Cursor end				*/
#define VREG_SAH	0x0C	/* Start address high			*/
#define VREG_SAL	0x0D	/* Start address low			*/
#define VREG_CLH	0x0E	/* Cursor location high			*/
#define VREG_CLL	0x0F	/* Cursor location low			*/
#define VREG_VRS	0x10	/* Vertical retrace start		*/
#define VREG_VRE	0x11	/* Vertical retrace end			*/
#define VREG_VDEE	0x12	/* Vertical display enable end		*/
#define VREG_LLW	0x13	/* Logical line width			*/
#define VREG_UL		0x14	/* Underline location			*/
#define VREG_SVB	0x15	/* Start vertical blanking		*/
#define VREG_EVB	0x16	/* End vertical blanking		*/
#define VREG_MC		0x17	/* Mode control				*/
#define VREG_LC		0x18	/* Line compare				*/

/* I/O Ports */

#define VPRT_MSTATUS	0x3BA	/* CRTC Status in mono modes		*/
#define VPRT_CSTATUS	0x3DA	/* CRTC Status in colour modes		*/
#define VPRT_SEQADDR	0x3C4	/* Sequencer register select		*/
#define VPRT_SEQDATA	0x3C5	/* Sequencer data			*/
#define VPRT_GCADDR	0x3CE	/* Graphics controller register select	*/
#define VPRT_GCDATA	0x3CF	/* Graphics controller data		*/
#define VPRT_ATTRIB	0x3C0	/* Attribute controller register	*/

/* Sequencer registers */

#define VSREG_RESET	1	/* Reset		*/
#define VSREG_CLKMODE	2	/* Clocking mode	*/
#define VSREG_MAPMASK	3	/* Map mask		*/
#define VSREG_CHARMAP	4	/* Character map select	*/
#define VSREG_MEMMODE	5	/* Memory mode		*/

/* Graphics controller registers */

#define VCREG_SETRESET	0	/* Set/reset			*/
#define VCREG_ENBLSR	1	/* Enable set/reset		*/
#define VCREG_COLORCMP	2	/* Color compare		*/
#define VCREG_ROT	3	/* Data rotate/function select	*/
#define VCREG_RMAPSEL	4	/* Read map select		*/
#define VCREG_GMODE	5	/* Graphics mode		*/
#define VCREG_MISC	6	/* Miscellaneous		*/
#define VCREG_CXXX	7	/* Color don't care		*/
#define VCREG_BMASK	8	/* Bit mask			*/

/* Standard VGA Modes */

#define MODE_640x400	0
#define MODE_720x400	1
#define MODE_640x480	2
#define MODE_640x350	3

typedef struct
{
	int	mode;		/* Mode number			*/
	int	width;
	int	height;
	int	colors;
	unsigned short segment;	/* Memory segment		*/
	int	crtc_status;	/* Status port			*/
	float	bandwidth;	/* Video bandwidth in MHz	*/
	float	horiz;		/* Horizontal scan rate in KHz	*/
	float	vert;		/* Vertical scn rate in Hz	*/
} ModeInfo;


