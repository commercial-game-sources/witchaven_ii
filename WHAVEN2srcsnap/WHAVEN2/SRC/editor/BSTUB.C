#include "build.h"
#include "names.h"
#include "stdio.h"
#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <setjmp.h>
#include <stdarg.h>
#include <process.h>
#include <memcheck.h>

#define AVERAGEFRAMES 64

static char tempbuf[256];
extern long qsetmode;
extern short searchsector, searchwall, searchstat;
extern long zmode, kensplayerheight;
extern short defaultspritecstat;

extern short temppicnum, tempcstat, templotag, temphitag, tempextra;
extern char tempshade, temppal, tempxrepeat, tempyrepeat;
extern char somethingintab;

static long frameval[AVERAGEFRAMES], framecnt = 0;

#define NUMOPTIONS 8
#define NUMKEYS 19
static long chainxres[4] = {256,320,360,400};
static long chainyres[11] = {200,240,256,270,300,350,360,400,480,512,540};
static long vesares[13][2] = {320,200,360,200,320,240,360,240,320,400,
						360,400,640,350,640,400,640,480,800,600,
						1024,768,1280,1024,1600,1200};
static char option[NUMOPTIONS] = {0,0,0,0,0,0,1,0};
static char keys[NUMKEYS] =
{
	0xc8,0xd0,0xcb,0xcd,0x2a,0x9d,0x1d,0x39,
	0x1e,0x2c,0xd1,0xc9,0x33,0x34,
	0x9c,0x1c,0xd,0xc,0xf,
};

extern char buildkeys[NUMKEYS];

//Detecting 2D / 3D mode:
//   qsetmode is 200 in 3D mode
//   qsetmode is 350/480 in 2D mode
//
//You can read these variables when F5-F8 is pressed in 3D mode only:
//
//   If (searchstat == 0)  WALL        searchsector=sector, searchwall=wall
//   If (searchstat == 1)  CEILING     searchsector=sector
//   If (searchstat == 2)  FLOOR       searchsector=sector
//   If (searchstat == 3)  SPRITE      searchsector=sector, searchwall=sprite
//   If (searchstat == 4)  MASKED WALL searchsector=sector, searchwall=wall
//
//   searchsector is the sector of the selected item for all 5 searchstat's
//
//   searchwall is undefined if searchstat is 1 or 2
//   searchwall is the wall if searchstat = 0 or 4
//   searchwall is the sprite if searchstat = 3 (Yeah, I know - it says wall,
//                                      but trust me, it's the sprite number)
void ExtInit(void)
{
	long i, fil;

	initgroupfile("stuff.dat");
	if ((fil = open("build.dat",O_BINARY|O_RDWR,S_IREAD)) != -1)
	{
		read(fil,&option[0],NUMOPTIONS);
		read(fil,&keys[0],NUMKEYS);
		memcpy((void *)buildkeys,(void *)keys,NUMKEYS);   //Trick to make build use setup.dat keys
		close(fil);
	}
	if (option[4] > 0) option[4] = 0;
	initmouse();

	//switch(option[0])
	//{
	//     case 0: initengine(0,chainxres[option[6]&15],chainyres[option[6]>>4]); break;
	//     case 1: initengine(1,vesares[option[6]&15][0],vesares[option[6]&15][1]); break;
	//     default: initengine(option[0],320L,200L); break;
	//}
	initengine(1,640L,480L);

	readpalettetable();     

		//You can load your own palette lookup tables here if you just
		//copy the right code!
	//for(i=0;i<256;i++)
	//     tempbuf[i] = ((i+32)&255);  //remap colors for screwy palette sectors
	//makepalookup(16,tempbuf,0,0,0,1);

	kensplayerheight = 48;
	zmode = 0;
	defaultspritecstat = 0;
	pskyoff[0] = 0; pskyoff[1] = 0; pskybits = 1;
}

void ExtUnInit(void)
{
	uninitgroupfile();
}

void ExtPreCheckKeys(void)
{
}

void ExtAnalyzeSprites(void)
{
	long i;
	spritetype *tspr;

	for(i=0,tspr=&tsprite[0];i<spritesortcnt;i++,tspr++)
	{
		tspr->shade += 6;
		if (sector[tspr->sectnum].ceilingstat&1)
			tspr->shade += sector[tspr->sectnum].ceilingshade;
		else
			tspr->shade += sector[tspr->sectnum].floorshade;
	}
}

void ExtCheckKeys(void)
{
	long i;

	if (qsetmode == 200)    //In 3D mode
	{
		i = totalclock;
		if (i != frameval[framecnt])
		{
			sprintf(tempbuf,"%ld",(120*AVERAGEFRAMES)/(i-frameval[framecnt]));
			printext256(0L,0L,31,-1,tempbuf,1);
			frameval[framecnt] = i;
		}
		framecnt = ((framecnt+1)&(AVERAGEFRAMES-1));

		editinput();
	}
	else
	{
	}
}

void ExtCleanUp(void)
{
}

void ExtLoadMap(const char *mapname)
{
}

void ExtSaveMap(const char *mapname)
{
}

const char *ExtGetSectorCaption(short sectnum)
{
	if ((sector[sectnum].lotag|sector[sectnum].hitag) == 0)
	{
		tempbuf[0] = 0;
	}
	else
	{
		sprintf(tempbuf,"%hu,%hu",(unsigned short)sector[sectnum].hitag,
										  (unsigned short)sector[sectnum].lotag);
	}
	return(tempbuf);
}

const char *ExtGetWallCaption(short wallnum)
{
	if ((wall[wallnum].lotag|wall[wallnum].hitag) == 0)
	{
		tempbuf[0] = 0;
	}
	else
	{
		sprintf(tempbuf,"%hu,%hu",(unsigned short)wall[wallnum].hitag,
										  (unsigned short)wall[wallnum].lotag);
	}
	return(tempbuf);
}

const char *ExtGetSpriteCaption(short spritenum)
{
	if ((sprite[spritenum].lotag|sprite[spritenum].hitag) == 0)
	{
		tempbuf[0] = 0;
	}
	else
	{
		sprintf(tempbuf,"%hu,%hu",(unsigned short)sprite[spritenum].hitag,
										  (unsigned short)sprite[spritenum].lotag);
	}
	return(tempbuf);
}

//printext16 parameters:
//printext16(long xpos, long ypos, short col, short backcol,
//           char name[82], char fontsize)
//  xpos 0-639   (top left)
//  ypos 0-479   (top left)
//  col 0-15
//  backcol 0-15, -1 is transparent background
//  name
//  fontsize 0=8*8, 1=3*5

//drawline16 parameters:
// drawline16(long x1, long y1, long x2, long y2, char col)
//  x1, x2  0-639
//  y1, y2  0-143  (status bar is 144 high, origin is top-left of STATUS BAR)
//  col     0-15

void ExtShowSectorData(short sectnum)   //F5
{
	if (qsetmode == 200)    //In 3D mode
	{
	}
	else
	{
		clearmidstatbar16();             //Clear middle of status bar

		sprintf(tempbuf,"Sector %d",sectnum);
		printext16(8,32,11,-1,tempbuf,0);

		printext16(8,48,11,-1,"8*8 font: ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789",0);
		printext16(8,56,11,-1,"3*5 font: ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789",1);

		drawline16(320,68,344,80,4);       //Draw house
		drawline16(344,80,344,116,4);
		drawline16(344,116,296,116,4);
		drawline16(296,116,296,80,4);
		drawline16(296,80,320,68,4);
	}
}

void ExtShowWallData(short wallnum)       //F6
{
	if (qsetmode == 200)    //In 3D mode
	{
	}
	else
	{
		clearmidstatbar16();             //Clear middle of status bar

		sprintf(tempbuf,"Wall %d",wallnum);
		printext16(8,32,11,-1,tempbuf,0);
	}
}

void ExtShowSpriteData(short spritenum)   //F6
{
	if (qsetmode == 200)    //In 3D mode
	{
	}
	else
	{
		clearmidstatbar16();             //Clear middle of status bar

		sprintf(tempbuf,"Sprite %d",spritenum);
		printext16(8,32,11,-1,tempbuf,0);
	}
}

void ExtEditSectorData(short sectnum)    //F7
{
	short nickdata;

	if (qsetmode == 200)    //In 3D mode
	{
			//Ceiling
		if (searchstat == 1)
			sector[searchsector].ceilingpicnum++;   //Just a stupid example

			//Floor
		if (searchstat == 2)
			sector[searchsector].floorshade++;      //Just a stupid example
	}
	else                    //In 2D mode
	{
		sprintf(tempbuf,"Sector (%ld) Nick's variable: ",sectnum);
		nickdata = 0;
		nickdata = getnumber16(tempbuf,nickdata,65536L);

		printmessage16("");              //Clear message box (top right of status bar)
		ExtShowSectorData(sectnum);
	}
}

void ExtEditWallData(short wallnum)       //F8
{
	short nickdata;

	if (qsetmode == 200)    //In 3D mode
	{
	}
	else
	{
		sprintf(tempbuf,"Wall (%ld) Nick's variable: ",wallnum);
		nickdata = 0;
		nickdata = getnumber16(tempbuf,nickdata,65536L);

		printmessage16("");              //Clear message box (top right of status bar)
		ExtShowWallData(wallnum);
	}
}

void ExtEditSpriteData(short spritenum)   //F8
{
	short nickdata;

	if (qsetmode == 200)    //In 3D mode
	{
	}
	else
	{
		sprintf(tempbuf,"Sprite (%ld) Nick's variable: ",spritenum);
		nickdata = 0;
		nickdata = getnumber16(tempbuf,nickdata,65536L);
		printmessage16("");

		printmessage16("");              //Clear message box (top right of status bar)
		ExtShowSpriteData(spritenum);
	}
}

faketimerhandler()
{
}


	//Just thought you might want my getnumber16 code
/*
getnumber16(char namestart[80], short num, long maxnumber)
{
	char buffer[80];
	long j, k, n, danum, oldnum;

	danum = (long)num;
	oldnum = danum;
	while ((keystatus[0x1c] != 2) && (keystatus[0x1] == 0))  //Enter, ESC
	{
		sprintf(&buffer,"%s%ld_ ",namestart,danum);
		printmessage16(buffer);

		for(j=2;j<=11;j++)                //Scan numbers 0-9
			if (keystatus[j] > 0)
			{
				keystatus[j] = 0;
				k = j-1;
				if (k == 10) k = 0;
				n = (danum*10)+k;
				if (n < maxnumber) danum = n;
			}
		if (keystatus[0xe] > 0)    // backspace
		{
			danum /= 10;
			keystatus[0xe] = 0;
		}
		if (keystatus[0x1c] == 1)   //L. enter
		{
			oldnum = danum;
			keystatus[0x1c] = 2;
			asksave = 1;
		}
	}
	keystatus[0x1c] = 0;
	keystatus[0x1] = 0;
	return((short)oldnum);
}
*/

void 
readpalettetable(void) 
{
	 FILE *fp;
	 int  i,j;
	 char num_tables,lookup_num;

	 if ((fp=fopen("lookup.dat","rb")) == NULL) {
		   return;
	 }
	 num_tables=getc(fp);
	 for (j=0 ; j < num_tables ; j++) {
		   lookup_num=getc(fp);
		   for (i=0 ; i < 256 ; i++) {
				tempbuf[i]=getc(fp);
		   }
		   makepalookup(lookup_num,tempbuf,0,0,0,1);
	 }
	 fclose;
}
