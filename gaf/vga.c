/* VGA routines by Gram */

int is_mono = 0;

/* Info for each mode */

ModeInfo mode_info[] =
{
/*	{ 640, 400, 25.175, 31.5, 70.	},
	{ 720, 400, 28.322, 31.5, 70.	},
	{ 640, 480, 25.175, 31.5, 60.	},
	{ 640, 350, 25.175, 31.5, 70.	}
*/
	{  4, 320, 200,  4, 0xB800, -1, ?, ?, ? },
	{  6, 640, 200,  2, 0xB800, -1, ?, ?, ? },
	{ 13, 320, 200, 16, 0xA000, -1, ?, ?, ? },
	{ 14, 640, 200, 16, 0xA000, -1, ?, ?, ? },
	{ 15, 640, 350,  2, 0xA000, -1, ?, ?, ? },
	{ 16, 640, 350,  4, 0xA000, -1, 25.175, 31.5, 70. },
	{ 17, 640, 480,  2, 0xA000, -1, 25.175, 31.5, 60. },
	{ 18, 640, 480, 16, 0xA000, -1, 25.175, 31.5, 60. },
	{ 19, 320, 200,256, 0xA000, -1, ?, ?, ? },
};

ModeInfo *mode = NULL;

/* Get CRTC status */

inline unsigned char VGetStatus(void)
{
	return (unsigned char)inportb(mode->crtc_status);
}

/* Test display enabled bit */

inline int VDisplayEnabled(void)
{
	return ((VGetStatus() & 1) == 0);
}

/* Test vertical sync */

inline int VSynchingVert(void)
{
	return ((VGetStatus() & 8) == 1);
}

/* Write to a sequencer register */

inline void VSeqWrite(int reg, unsigned value)
{
	outportb(VPRT_SEQADDR, reg);
	outportb(VPRT_SEQDATA, value);
}

/* Write to a graphics controller register */

inline void VGrxWrite(int reg, unsigned value)
{
	outportb(VPRT_GCADDR, reg);
	outportb(VPRT_GCDATA, value);
}

/* Write to an attribute controller register */

inline void VAttribWrite(int reg, unsigned value)
{
	(void)VGetStatus(); /* Reset flip flop */
	outportb(VPRT_ATTRIB, reg);
	outportb(VPRT_ATTRIB, value);
}

/* Read from an attribute controller register */

inline unsigned char VAttribRead(int reg)
{
	outportb(VPRT_ATTRIB, reg);
	return (unsigned char)intportb(VPRT_ATTRIB+1);
}

inline void VSetMode(VMode m)
{
	_AH = mode->mode;
	geninterrupt(0x10);
	/* Get the status port from the video info area */
	mode->crtc_status = *((char far *)MK_FP(0x40, 0x63);
}



