/*#######################################################################\
#
#	Synergistic Software
#	(c) Copyright 1995  All Rights Reserved.
#   Written 10/5/95 by Craig B. Seastrom
#
\#######################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <io.h>
#include <process.h>


/* Defines. */

#define USERINTR1		0x69
#define USERINTR2		0x6A
#define USERINTR3		0x6B

#define STOPCOMMAND		0x80000001
#define PLAYTRACKCMD	0x80000002
#define FINDTRACKCMD	0x80000003
#define CHECKBUSYCMD	0x80000004


/* Prototypes. */

void device_request (void *);
short get_cd_drive_handle (void);
void cd_status (void);
void play_audio (unsigned long, unsigned long);
void stop_audio (void);
void audio_disk_info (void);
void audio_track_info (unsigned char);
void play_track (unsigned char);
unsigned long Red_to_HighSierra (unsigned long);
void Print_to_Mono (unsigned long);
// TODO: (fire lizard) uncomment
//void interrupt CDPlayHandler (void);
//void interrupt (* oldinthandler1) (void);
//void interrupt (* oldinthandler2) (void);
//void interrupt (* oldinthandler3) (void);


/* Global variables. */

//union	REGS inregs, outregs;
//struct	SREGS sregs;
short	cd_drive_handle;
unsigned short cd_error;
short	monopos;


/* Structures. */

typedef struct
	{
	unsigned char  length;		// Length in bytes of request header
	unsigned char  subunit;		// Subunit code for minor devices
	unsigned char  comcode;		// Command code field
	unsigned short status;		// Status
	unsigned char  unused[8];	// Reserved
	} request_header;

struct
	{
	request_header header;
	unsigned char  media;		// Media descriptor byte from BPB
	unsigned long  address;		// Transfer address
	unsigned short number;		// Number of bytes to transfer
	unsigned short sector;		// Starting sector number
	unsigned long  volid;		// DWORD ptr to requested vol ID if error 0Fh
	} IOCTL1;

struct
	{
	unsigned char code;			// Control block code
	unsigned long status;		// Device parameters
	} device_status;

struct
	{
	unsigned char  code;		// Control block code
	unsigned char  lowest;		// Lowest track number
	unsigned char  highest;		// Highest track number
	unsigned long  leadout;		// Starting point of the lead-out track
	} DiskInfo;

struct
	{
	unsigned char  code;		// Control block code
	unsigned char  track;		// Track number
	unsigned long  start;		// Starting point of the track
	unsigned char  ctrlinfo;	// Track control information
	} TnoInfo;

struct
	{
	request_header header;
	unsigned char  mode;		// Media descriptor byte from BPB
	unsigned long  sector;		// Starting sector number
	unsigned long  number;		// Number of sectors to read
	} PlayReq;

struct
	{
	request_header header;
	} StopPlayReq;

int main (int argc, char *argv[])
{
	int end_code = 0;

	if (get_cd_drive_handle() == 1)
	{
		printf ("No CD-ROM drive found.\n");
		exit(1);
	}

	/* Get the address of the old interrupt. */
	//oldinthandler1 = _dos_getvect (USERINTR1);
	//oldinthandler2 = _dos_getvect (USERINTR2);
	//oldinthandler3 = _dos_getvect (USERINTR3);

	/* Install the new interrupt handler. */
	//_dos_setvect (USERINTR1, CDPlayHandler);

	/* Set the return value to something we can check for on initialization */
	//poke (0x0000, 0x01a8, 0xC0DE);
	//poke (0x0000, 0x01aa, 0xDEAD);

	/* Spawn the program indicated as a parameter, with its parms. */
	if (argc == 1)
	{
		end_code = spawnv (P_WAIT, "NOVA", NULL);
	}
	else
	{
		end_code = spawnv (P_WAIT, "NOVA", &argv[0]);
	}

	/* Restore the original interrupt handler. */
	//_dos_setvect (USERINTR1, oldinthandler1);
	//_dos_setvect (USERINTR2, oldinthandler2);
	//_dos_setvect (USERINTR3, oldinthandler3);

	return (end_code);
}


void device_request (void *block)
{
	//inregs.x.ax = 0x1510;
	//inregs.x.cx = cd_drive_handle;
	//inregs.x.bx = FP_OFF (block);
	//sregs.es    = FP_SEG (block);
	//int86x (0x2f, &inregs, &outregs, &sregs);
}


short get_cd_drive_handle (void)
{
	//inregs.h.ah = 0x15;
	//inregs.h.al = 0x00;
	//inregs.x.bx = 0;
	//int86 (0x2f, &inregs, &outregs);

	//if (outregs.x.bx != 0)		// number of drives not 0
	{
	//	cd_drive_handle = outregs.x.cx;
	//	return (0);
	}

	return (1);
}


void cd_status (void)
{
	IOCTL1.header.length = sizeof (IOCTL1);
	IOCTL1.header.subunit = 0;
	IOCTL1.header.comcode = 3;

	IOCTL1.media = 0;
	IOCTL1.address = (unsigned long)&device_status;
	IOCTL1.number = 5;
	IOCTL1.sector = 0;
	IOCTL1.volid = 0;

	device_status.code = 6;
	device_request (&IOCTL1);
}


void play_audio (unsigned long sector, unsigned long number_of_sectors)
{
	PlayReq.header.length = sizeof (PlayReq);
	PlayReq.header.subunit = 0;
	PlayReq.header.comcode = 132;

	PlayReq.mode = 0;
	PlayReq.sector = sector;
	PlayReq.number = number_of_sectors;

	device_request (&PlayReq);

	if ((PlayReq.header.status & 0x8000))
	{
		cd_error = PlayReq.header.status;
	}
}


void stop_audio (void)
{
	StopPlayReq.header.length = sizeof (StopPlayReq);
	StopPlayReq.header.subunit = 0;
	StopPlayReq.header.comcode = 133;

	device_request (&StopPlayReq);
}


void audio_disk_info (void)
{
	IOCTL1.header.length = sizeof (IOCTL1);
	IOCTL1.header.subunit = 0;
	IOCTL1.header.comcode = 3;

	IOCTL1.media = 0;
	IOCTL1.address = (unsigned long)&DiskInfo;
	IOCTL1.number = 7;
	IOCTL1.sector = 0;
	IOCTL1.volid = 0;

	DiskInfo.code = 10;
	device_request (&IOCTL1);
}


void audio_track_info (unsigned char track)
{
	IOCTL1.header.length = sizeof (IOCTL1);
	IOCTL1.header.subunit = 0;
	IOCTL1.header.comcode = 3;

	IOCTL1.media = 0;
	IOCTL1.address = (unsigned long)&TnoInfo;
	IOCTL1.number = 7;
	IOCTL1.sector = 0;
	IOCTL1.volid = 0;

	TnoInfo.code = 11;
	TnoInfo.track = track;
	device_request (&IOCTL1);
}


void play_track (unsigned char track)
{
	unsigned long startpos, endpos;

	if (track == 0)				// play whole CD
	{
		audio_track_info (DiskInfo.lowest);
	}
	else
	{
		audio_track_info (track);
	}
	startpos = TnoInfo.start;

	if ((track != 0) && (track < DiskInfo.highest))
	{
		audio_track_info (track + 1);
		endpos = TnoInfo.start;
	}
	else
	{
		endpos = DiskInfo.leadout;
	}

	startpos = Red_to_HighSierra (startpos);
	endpos   = Red_to_HighSierra ( endpos );

	play_audio (startpos, endpos - startpos);
}


unsigned long Red_to_HighSierra (unsigned long position)
{
	unsigned long minute, second, frame;

	minute = (position >> 16) & 0xff;
	second = (position >>  8) & 0xff;
	frame  = (position      ) & 0xff;

	return ((minute * 60 * 75) + (second * 75) + frame - 150);
}


#if 0
void Print_to_Mono (unsigned long number)
{
	number &= 0x000f;
	if (number > 9)
	{
		number += 7;
	}

	poke (0xb000, 0x00a0 * 24 + monopos, 0x4030 + number);
	monopos += 2;
}
#endif


void /*interrupt*/ CDPlayHandler(void)
{
	unsigned long start, length;
	unsigned short startlow, starthigh, lengthlow, lengthhigh;

	cd_error = 0;

	cd_status();

	if (device_status.status & 0x0040)
	{
		cd_error = 0xffff;		// CD drive error.  No disk found.
	}

	if (device_status.status & 0x0001)
	{
		cd_error = 0xfffe;		// CD drive door open.
	}

	if (cd_error == 0)
	{
		audio_disk_info();

 	 	//startlow = peek (0x0000, 0x01a8);	// int vector 6a
 	 	//starthigh = peek (0x0000, 0x01aa);
 	 	start = ((unsigned long)starthigh << 16) + (unsigned long)startlow;

 	 	//lengthlow = peek (0x0000, 0x01ac);	// int vector 6b
 	 	//lengthhigh = peek (0x0000, 0x01ae);
 	 	length = ((unsigned long)lengthhigh << 16) + (unsigned long)lengthlow;

#if 0
		monopos = 80;
		Print_to_Mono (start >> 28);
		Print_to_Mono (start >> 24); 
		Print_to_Mono (start >> 20); 
		Print_to_Mono (start >> 16); 
		Print_to_Mono (start >> 12); 
		Print_to_Mono (start >>  8); 
		Print_to_Mono (start >>  4); 
		Print_to_Mono (start >>  0); 

		monopos = 104;
		Print_to_Mono (length >> 28);
		Print_to_Mono (length >> 24);
		Print_to_Mono (length >> 20);
		Print_to_Mono (length >> 16);
		Print_to_Mono (length >> 12);
		Print_to_Mono (length >>  8);
		Print_to_Mono (length >>  4);
		Print_to_Mono (length >>  0);
#endif

		if (length < 0x80000000)
		{
			if (IOCTL1.header.status & 0x0200)
			{
				cd_error = 0xC000;			// drive busy
			}
			else
			{
				play_audio (start, length);
			}
		}
		else
		{
			switch (length)
			{
				case STOPCOMMAND:
					stop_audio();
					break;

				case PLAYTRACKCMD:
					if ((start >= DiskInfo.lowest) && (start <= DiskInfo.highest))
					{
						if (IOCTL1.header.status & 0x0200)
						{
							cd_error = 0xC000;			// drive busy
						}
						else
						{
							play_track ((unsigned char)start);
						}
					}
					else
					{
						cd_error = 0xA000;			// invalid track
					}
					break;

				case FINDTRACKCMD:
					if ((start >= DiskInfo.lowest) && (start <= DiskInfo.highest))
					{
						audio_track_info (start);
						start = Red_to_HighSierra (TnoInfo.start);
					}
					else
					{
						cd_error = 0x8000;			// invalid track
					}
					break;

				case CHECKBUSYCMD:
					if (IOCTL1.header.status & 0x0200)
					{
						cd_error = 0xC000;			// drive busy
					}
					break;

				default:
					cd_error = 0xfffd;				// invalid command
					break;
			}
		}
	}

	if (cd_error == 0)
	{
		//poke (0x0000, 0x01a8, start);
		//poke (0x0000, 0x01aa, start >> 16);
	}
	else
	{
		//poke (0x0000, 0x01a8, 0);
		//poke (0x0000, 0x01aa, cd_error);
	}
}
