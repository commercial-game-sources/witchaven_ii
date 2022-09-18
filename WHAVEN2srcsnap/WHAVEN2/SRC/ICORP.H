/***************************************************************************
 *   ICORP.H   - include file for Whaven game                              *
 *                                                                         *
 *                                                     12/15/94 Les Bird   *
 ***************************************************************************/

#include <fcntl.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <setjmp.h>
#include <stdarg.h>
#include <process.h>

#include "build.h"
#include "names.h"
#include "sndmod.h"
#include "les.h"
#include "jeff.h"
#include "raf.h"
#include "whdefs.h"
#include "keydefs.h"
#include "extern.h"
#include "pragmas.h"
#include "wh2cntrl.h"

#ifndef _PROTOTYPING_
     #include "whaven.def"
     #include "whinp.def"
     #include "whtag.def"
     #include "whobj.def"
     #include "whplr.def"
     #include "whfx.def"
     #include "whsndmod.def"
     #include "whani.def"
     #include "whnet.def"
     #include "cdrom.def"
     #include "whctm.def"
     #include "whsmk.def"
     #include "readcfg.def"
     #include "profile.def"
#endif

#define   GONZOGSHXR     35
#define   GONZOGSHYR     35
#define   GONZOGHMXR     35
#define   GONZOGHMYR     35
#define   GONZOGSWXR     35
#define   GONZOGSWYR     35
#define   NEWGUYXR       35
#define   NEWGUYYR       35
#define   GONZOCSWXR     35
#define   GONZOCSWYR     35
#define   GONZOCSWATXR   35
#define   GONZOCSWATYR   35
#define   KOBOLDXR       60
#define   KOBOLDYR       60
#define   DEVILXR        50
#define   DEVILYR        50
#define   IMPXR          25
#define   IMPYR          25
#define   MINOTAURXR     64
#define   MINOTAURYR     64
#define   SKELETONXR     35
#define   SKELETONYR     35
#define   GRONHALXR      35
#define   GRONHALYR      35
#define   GRONSWXR       35
#define   GRONSWYR       35
#define   GRONMUXR       35
#define   GRONMUYR       35
#define   WILLOWXR       35
#define   WILLOWYR       35
#define   DEMONXR        38
#define   DEMONYR        41
#define   GUARDIANXR     35
#define   GUARDIANYR     35
#define   KURTXR         35
#define   KURTYR         35
#define   KATIEXR        35
#define   KATIEYR        35

extern int  swingcnt;

struct swingdoor {
     int  wall[8];
     int  sector;
     int  angopen;
     int  angclosed;
     int  angopendir;
     int  ang;
     int  anginc;
     long x[8];
     long y[8];
};

extern
struct swingdoor swingdoor[MAXSWINGDOORS];

//
// plr->flags defines
//

//
// plr->equipment flag values
//


struct player {
     long x,y,z;
     long ang;
     long horiz;
     long zoom;
     long height;
     long hvel;               
     short sector,oldsector;
     short screensize;
     short spritenum;
     char dimension;
     unsigned long flags;
     int  weapon[MAXWEAPONS],preenchantedweapon[MAXWEAPONS];
     int  ammo[MAXWEAPONS],preenchantedammo[MAXWEAPONS];
     int  orbammo[MAXNUMORBS];
     int  treasure[MAXTREASURES];
     int  orbactive[MAXNUMORBS];
     int  orb[MAXNUMORBS];
     int  potion[MAXPOTIONS];
     int  lvl;
     long score;
     int  health;
     int  maxhealth;
     int  armor;
     int  armortype;
     char onsomething;
     long fallz;
};

extern struct player player[MAXPLAYERS];

struct delayitem {
     void (*func)(int);
     int  item;
     int  timer;
};

extern
struct delayitem delayitem[MAXSECTORS];

struct sectoreffect {
     unsigned long sectorflags;
     long animate;
     int  hi,lo;
     int  delay,delayreset;
};

extern
int  secnt,
     sexref[];

extern
struct sectoreffect sectoreffect[MAXSECTORS];

extern
int  pyrn;

extern
long angvel,
     svel,
     vel;

extern
long *animateptr[],
     animategoal[],
     animatevel[],
     animatecnt,
     neartagdist,
     neartaghitdist;

extern
unsigned long flags32[];

extern
short neartagsector,
     neartagwall,
     neartagsprite;

extern
long lockclock;

void lb_printf(int,int,char *,...);

extern
char displaybuf[50],
     tempbuf[576],
     scorebuf[50],
     healthbuf[50],
     armorbuf[50],
     potionbuf[50],
     boardname[80],
     tempboard[80],
     loadgamename[80],
     scantoasc[128],
     scantoascwithshift[128],
     typemessage[162],
     fancy[41];

#define   ARROWCOUNTLIMIT     100
#define   THROWPIKELIMIT      100

extern
short arrowsprite[ARROWCOUNTLIMIT],
     throwpikesprite[THROWPIKELIMIT];

