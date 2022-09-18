/*********************************************************************
*
*   WHMENU.C - menu code for Witchaven game
*
*********************************************************************/

#include "icorp.h"
//#define NOTFIXED return;
#define NOTFIXED
#define FULLSCREEN 0

#define SCOTTSPEED 20L


//** Les 01/17/96   - START
// Raf for swift support extern declarations
#include "swift.h"
#include <memcheck.h>
//** Les 01/17/96   - END

//** Les 01/17/96   - START
// Raf extern vals from whinp.c
extern int wingmanflag,
     wingmanyawsens,
     wingmanpitchsens,
     wingmanrollsens,
     wingmanzsens,
     wingmanzreverse;

extern SWIFT_3DStatus wingmaninfo;

extern short wingmankeys[];
//** Les 01/17/96   - END


int  loopinstuff;                  // here it is
extern int mousxspeed,
     mousyspeed;

extern int vampiretime;
extern int musiclevel;
extern int digilevel;

extern short gbrightness;

//JSA 4_27 cart and elevator sound variables
extern int cartsnd,
     gratesnd,
     lavasnd,
     batsnd;

extern int gameactivated;
extern int escapetomenu;

extern int difficulty;
extern int mapon;

extern int currweapon;
extern int selectedgun;
extern int currentpotion;
extern int helmettime;
extern int shadowtime;
extern int nightglowtime;
extern short brightness;
extern int strongtime;
extern int invisibletime;
extern int manatime;
extern int thunderflash;
extern int thundertime;
extern int currentorb;
extern int currweaponfired;
extern int currweaponanim;
extern int currweaponattackstyle;
extern int currweaponflip;

int  goreon = 1;
int  loadedgame = 0;
int  loadgo = 0;
int  musicoverride = -1;

char typemessage[162],
     typemessageleng = 0,
     typemode = 0;

char scantoasc[128] = {
     0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
     'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',
     'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 39, '`', 0, 92, 'z', 'x', 'c', 'v',
     'b', 'n', 'm', ',', '.', '/', 0, '*', 0, 32, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
     '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

char scantoascwithshift[128] = {
     0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
     'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S',
     'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 34, '~', 0, '|', 'Z', 'X', 'C', 'V',
     'B', 'N', 'M', '<', '>', '?', 0, '*', 0, 32, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
     '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

extern int frames;
#define MAXSAVEDGAMES 8

struct savedgame {
     char name[20];
};

struct savedgame savedgamenames[MAXSAVEDGAMES];
struct savedgame quicksavedgame;

int  quicksaveflag;

extern int gameactivated;

void
loadsavetoscreen(void)
{
     if (svga == 0)
          permanentwritesprite(0L, 0L, ZLOADSAVE, 0, 0L, 0L, 319L, 199L, 0);
     else {
          rotatesprite(0L << 16, 0L << 16, 65536L, 0, VLOAD, 0, 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
          rotatesprite(0L << 16, 240L << 16, 65536L, 0, VSAVE, 0, 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
     }

}

void
menutoscreen(void)
{
     if (svga == 0)
          permanentwritesprite(0L, 0L, THEMAINMENUWITH, 0, 0L, 0L, 319L, 199L, 0);
     else
          rotatesprite(0L << 16, 0L << 16, 65536L, 0, VMAIN, 0, 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);

}

void
menutoscreenblank(void)
{
     permanentwritesprite(0L, 0L, THEMAINMENU, 0, 0L, 0L, 319L, 199L, 0);
}

void
itemtoscreen(long x, long y, short dapic, signed char dashade, char dapal)
{
     permanentwritesprite(x, y, dapic, dashade, x, y, xdim - 1, ydim - 1, dapal);
}
//
// fancy font
//
// to use this function you will need to name the starting letter
// the function will then scan the string and display its chars


char fancy[41] = {'a', 'b', 'c', 'd', 'e',
     'f', 'g', 'h', 'i', 'j',
     'k', 'l', 'm', 'n', 'o',
     'p', 'q', 'r', 's', 't',
     'u', 'v', 'w', 'x', 'y',
     'z', '0', '1', '2', '3',
     '4', '5', '6', '7', '8',
'9', '!', '?', '-', ':', ' '};


void
fancyfont(long x, long y, short tilenum, char *string, char pal)
{

     int  i,
          j;
     int  len;
     int  incr = 0;
     int  exit = 0;
     int  number;
     char temp[40];

     strlwr(string);
     len = strlen(string);
     strcpy(temp, string);

     for (i = 0; i < len; i++) {
          tempbuf[i] = temp[i];
          for (j = 0; j < 40; j++) {
               if (tempbuf[i] == fancy[j]) {
                    number = j;
               }
          }
          if (i == 0) {
               itemtoscreen(x, y, tilenum + number, 0, pal);
               incr += tilesizx[tilenum + number] + 1;
          }
          else if (tempbuf[i] != ' ') {
               itemtoscreen(x + incr, y, tilenum + number, 0, pal);
               incr += tilesizx[tilenum + number] + 1;
          }
          else {
               incr += 8;
          }
     }

}



void
fancyfontscreen(long x, long y, short tilenum, char *string)
{

     int  i,
          j;
     int  len;
     int  incr = 0;
     int  exit = 0;
     int  number;
     char temp[40];

     strlwr(string);
     len = strlen(string);
     strcpy(temp, string);

     for (i = 0; i < len; i++) {
          tempbuf[i] = temp[i];
          for (j = 0; j < 40; j++) {
               if (tempbuf[i] == fancy[j]) {
                    number = j;
               }
          }
          if (i == 0) {
               overwritesprite(x, y, tilenum + number, 0, 0x02, 7);
               incr += tilesizx[tilenum + number] + 1;
          }
          else if (tempbuf[i] != ' ') {
               overwritesprite(x + incr, y, tilenum + number, 0, 0x02, 7);
               incr += tilesizx[tilenum + number] + 1;
          }
          else
               incr += 8;
     }

}


int
menuscreen(struct player * plr)
{

     struct {
          long x;
          long y;
     }    redpic[5] = {
          {
               94, 8
          },
          {
               88, 39
          },
          {
               95, 67
          },
          {
               114, 101
          },
          {
               112, 120
          }
     };

     struct {
          long x;
          long y;
     }    sredpic[5] = {
          {
               209, 102
          },
          {
               206, 136
          },
          {
               213, 177
          },
          {
               235, 208
          },
          {
               222, 242
          }
     };


     int  exit = 0;
     int  select = 0;
     short redpicnum;
     long goaltime;
     int  i;
     int  cnt = 0;

     // clearview(0L);  //Clear screen to specified color

     if (netgame) {
          return (0);
//        netkillme();
     }

//JSA BLORB
     SND_CheckLoops();

     redpicnum = THENEWGAME;

     keystatus[1] = keystatus[0x1c] = keystatus[0x9c] = 0;

     goaltime = totalclock + SCOTTSPEED;

     if (svga == 1) {
          setview(0L, 0L, 639L, 479L);
          rotatesprite(0L << 16, 0L << 16, 65536L, 0, VMAIN, 0,
                       0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
     }

     while (!exit) {

          menutoscreen();

          if (select < 5) {
               if (svga == 0) {
                    switch (cnt) {
                    case 0:
                         redpicnum = THENEWGAME + select;
                         break;
                    case 1:
                         redpicnum = ZFLASHER1 + select;
                         break;
                    case 2:
                         redpicnum = ZFLASHER2 + select;
                         break;
                    }
                    itemtoscreen(redpic[select].x, redpic[select].y, redpicnum, 0, 0);
               }
               else {
                    switch (cnt) {
                    case 0:
                         redpicnum = VNEW + select;
                         break;
                    case 1:
                         redpicnum = SFLASH1 + select;
                         break;
                    case 2:
                         redpicnum = SFLASH2 + select;
                         break;
                    }
                    rotatesprite((sredpic[select].x) << 16, (sredpic[select].y) << 16, 65536L, 0,
                       redpicnum, 0, 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
               }
          }

          nextpage();
          if (totalclock >= goaltime) {
               goaltime = totalclock + SCOTTSPEED;
               cnt++;
               if (cnt > 2)
                    cnt = 0;
               if (keystatus[0xd0] || keystatus[0x50]) {
                    TEMPSND();
                    select++;
                    if (select > 4)
                         select = 0;
               }
               if (keystatus[0xc8] || keystatus[0x48]) {
                    TEMPSND();
                    select--;
                    if (select < 0)
                         select = 4;
               }
               if (keystatus[1] > 0) {
                    TEMPSND();
                    if (gameactivated == 1) {
                         // select=5;
                         lockclock = totalclock;
                         exit = 1;
                    }
                    else
                         select = 4;
                    keystatus[1] = 0;
               }
               if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    keystatus[0x1c] = keystatus[0x9c] = 0;
                    keystatus[1] = 0;
                    switch (select) {
                    case 0:        // ok
                         gameactivated = 0;
                         srand(totalclock & 30000);
                         // tille maps are finished
                         // if(loadedgame == 0 && netgame == 0)
                         // mapon=1;
#if 0
                         if (netgame) {
                              netpickmonster();
                              exit = 1;
                              break;
                         }
#endif
                         SND_Sound(S_WARP);
                         startnewgame(plr);
                         gameactivated = 1;
                         exit = 1;
                         keystatus[0x1c] = keystatus[0x9c] = 0;
                         keystatus[1] = 0;
                         break;
                    case 1:
                         TEMPSND();
                         loadsave(plr);
                         if (loadgo == 1)
                              exit = 1;
                         loadgo = 0;
                         keystatus[0x1c] = keystatus[0x9c] = 0;
                         keystatus[1] = 0;
                         break;
                    case 2:
                         TEMPSND();
                         optionspage();
                         keystatus[0x1c] = keystatus[0x9c] = 0;
                         keystatus[1] = 0;
                         break;
                    case 3:
                         TEMPSND();
                         help();
                         break;
                    case 4:
                         TEMPSND();
                         quit();
                         break;
                    case 5:
                         lockclock = totalclock;
                         exit = 1;
                         updatepics();
                         break;
                    }              // switch
               }                   // if
          }                        // the delay
     }                             // while

     escapetomenu = 0;

//      if (netgame) {
//          netreviveme();
//      }

     return (0);
}

//JUNE8
#define  MAXHELPSCREENS   10

void
help(void)
{

     int thenames[MAXHELPSCREENS] = {
          ZWEAPON,
          ZSPELL,
          ZPOTION,
          ZMOVE,
          ZMOVE2,
          ZKEN,
          ZCREDITS,
          ZCREDITS2,
          ZARTIST,
          ZCREDITS3
     }, sthenames[MAXHELPSCREENS] = {
          1064,1063,1060,1059,1061,VCREDIT1,1057,1062,1056,1058
     };

     int  select = 0;
     long goaltime;
     int  exit = 0;

     while (!exit) {
          if (totalclock >= goaltime) {

               if (svga == 0) {
                    menutoscreenblank();
                    itemtoscreen(0L, 0L, thenames[select], 0, 0);
               }
               else {
                    rotatesprite(0L << 16, 0L << 16, 65536L, 0, VMAINBLANK, 0,
                                 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
                    rotatesprite(0L << 16, 0L << 16, 65536L, 0,
                                 sthenames[select], 0, 0, 8 + 16, 0L, 0L,
                                 xdim - 1L, ydim - 1L);
               }

               nextpage();
               goaltime = totalclock + SCOTTSPEED;
               if (keystatus[0xd0]
                   || keystatus[keys[KEYRIGHT]]
                   || keystatus[RDN]
                   || keystatus[keys[KEYBACK]]
                   || keystatus[RRIGHT]) {
                    TEMPSND();
                    select++;
                    if (select > 9)
                         select = 9;
               }
               if (keystatus[0xc8]
                   || keystatus[keys[KEYLEFT]]
                   || keystatus[RUP]
                   || keystatus[keys[KEYFWD]]
                   || keystatus[RLEFT]) {
                    TEMPSND();
                    select--;
                    if (select < 0)
                         select = 0;
               }
               if (keystatus[1] > 0) {
                    exit = 1;
                    keystatus[1] = 0;
               }
               if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    exit = 2;
                    keystatus[0x1c] = keystatus[0x9c] = 0;
               }
          }
     }
     keystatus[1] = keystatus[0x1c] = keystatus[0x9c] = 0;

}

//JUNE6
void
loadsave(struct player * plr)
{

     int  exit = 0;
     int  select = 0;
     long goaltime;
     int  cnt = 0;
     short redpicnum;

     struct {
          long x;
          long y;
     }    spic[2] = {{
               229, 152
     },
     {
          233, 190
     }
     };

     goaltime = totalclock + SCOTTSPEED;

     while (!exit) {
          loadsavetoscreen();
          if (svga == 0) {
               if (select == 0) {
                    switch (cnt) {
                    case 0:
                         itemtoscreen(109L, 46L, DALOAD, 0, 0);
                         break;
                    case 1:
                         itemtoscreen(109L, 46L, ZFLASHER1 + 8, 0, 0);
                         break;
                    case 2:
                         itemtoscreen(109L, 46L, ZFLASHER2 + 8, 0, 0);
                         break;
                    }
               }
               else {
                    switch (cnt) {
                    case 0:
                         itemtoscreen(112L, 77L, DASAVE, 0, 0);
                         break;
                    case 1:
                         itemtoscreen(112L, 77L, ZFLASHER1 + 9, 0, 0);
                         break;
                    case 2:
                         itemtoscreen(112L, 77L, ZFLASHER2 + 9, 0, 0);
                         break;
                    }
               }
          }
          else {
               if (select == 0) {
                    switch (cnt) {
                    case 0:
                         redpicnum = SFLASH3 + select;
                         break;
                    case 1:
                         redpicnum = SFLASH1 + 5 + select;
                         break;
                    case 2:
                         redpicnum = SFLASH2 + 5 + select;
                         break;
                    }
               }
               else {
                    switch (cnt) {
                    case 0:
                         redpicnum = SFLASH3 + select;
                         break;
                    case 1:
                         redpicnum = SFLASH1 + 5 + select;
                         break;
                    case 2:
                         redpicnum = SFLASH2 + 5 + select;
                         break;
                    }
               }
               rotatesprite((spic[select].x) << 16, (spic[select].y) << 16, 65536L, 0,
                       redpicnum, 0, 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
          }
          if (totalclock >= goaltime) {
               goaltime = totalclock + SCOTTSPEED;
               cnt++;
               if (cnt > 2)
                    cnt = 0;
               if (keystatus[keys[KEYBACK]] || keystatus[RDN]) {
                    TEMPSND();
                    select = 1;
               }
               if (keystatus[keys[KEYFWD]] || keystatus[RUP]) {
                    TEMPSND();
                    select = 0;
               }
               if (keystatus[1] > 0) {
                    exit = 1;
                    keystatus[1] = 0;
               }
               if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    exit = 2;
                    keystatus[0x1c] = keystatus[0x9c] = 0;
               }
          }
          nextpage();
     }

     keystatus[1] = 0;

     if (exit == 2)
          switch (select) {
          case 0:                  // ok
               loadgame(plr);
               break;
          case 1:                  // ok
               if (gameactivated == 1)
                    savegame(plr);
               break;
          }

}

//JUNE6
void
quit(void)
{

     int  exit = 0;
     long goaltime;
     char temp[20];

     if (svga == 0) {
          menutoscreenblank();
          itemtoscreen(102L, 76L, ZSURE, 0, 0);
     }
     else {
          rotatesprite(0L << 16, 0L << 16, 65536L, 0, VMAINBLANK, 0,
                       0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
          itemtoscreen((xdim>>1)-58, (ydim>>1)-24, ZSURE, 0, 0);
     }

     nextpage();

     while (!exit) {

          if (keystatus[0x9c] > 0 || keystatus[0x1c] > 0 || keystatus[0x15] > 0) {
               exit = 1;
               keystatus[0x1c] = keystatus[0x9c] = 0;
          }


          if (keystatus[1] > 0 || keystatus[0x31] > 0) {
               exit = 2;
               keystatus[1] = 0;
          }
     }

     if (exit == 2) {
          keystatus[1] = 0;
     }
     else {
          if (svga == 1) {
               permanentwritesprite(0, 0, STHEORDER, 0, 0, 0, 639, 239, 0);
               permanentwritesprite(0, 240, STHEORDER + 1, 0, 0, 240, 639, 479, 0);
               nextpage();
               exit = 0;
               while (!exit) {
                    if (keystatus[0x39] > 0 || keystatus[1] > 0)
                         exit = 1;
               }
               keystatus[0x39] = 0;
               keystatus[1] = 0;
          }
          else {
               keystatus[0x39] = 0;
               keystatus[1] = 0;
               exit = 0;
               while (!exit) {
                    if (keystatus[0x39] > 0 || keystatus[1] > 0)
                         exit = 1;
                    // overwritesprite(0,0,ORDER1,0,0,0);
                    // overwritesprite(0,0,ORDER1,0,4,0);
                    itemtoscreen(0L, 0L, THEORDER, 0, 0);
                    nextpage();
               }
               keystatus[0x39] = 0;
               keystatus[1] = 0;
               exit = 0;
               /* while( !exit ){ if(keystatus[0x39] > 0 || keystatus[1] > 0)
                * exit=1; //overwritesprite(0,0,ORDER2,0,0,0);
                * //overwritesprite(0,0,ORDER2,0,4,0);
                * itemtoscreen(0L,0L,ORDER2,0,0); nextpage(); }
                * keystatus[0x39]=0; keystatus[1]=0; exit=0; */
          }
          shutdown();
     }

}

void
thedifficulty(void)
{

     struct {
          int  x;
          int  y;
     }    redpic[4] = {
          {
               152, 131 + 5
          },
          {
               184, 131 + 5
          },
          {
               219, 129 + 5
          },
          {
               261, 128 + 5
          }
     };



     int  exit = 0;
     int  selected;
     int  select;
     int  select2 = 0;
     int  select3 = goreon;
     int  redpicnum;
     int  pickone = 0;
     long goaltime;
     int  cnt = 0;

     struct player *plr;

     plr = &player[0];

     select = difficulty - 1;
     keystatus[1] = 0;
     keystatus[0x1c] = 0;
     keystatus[0x9c] = 0;

     if (svga == 0) {
          permanentwritesprite(0L, 0L, ZZGORE, 0, 0L, 0L, 319L, 199L, 0);
          itemtoscreen(66L, 9L, ZZBLOOD, 0, 0);
     }
     else {
          permanentwritesprite(0L, 0L, VGOREA, 0, 0L, 0L, xdim-1L, ydim-1L, 0);
          permanentwritesprite(0L, 240L, VGOREB, 0, 0L, 0L, xdim-1L, ydim-1L, 0);
          itemtoscreen((xdim>>1)-160L+37L, (ydim>>1)-100L-1L, 3702, 0, 0);
     }

     if (goreon == 1) {
          itemtoscreen((long) ((xdim - tilesizx[NOGORESHADOW]) >> 1),
                       (svga == 0) ? (ydim >> 1) - 100L + 72L :
                                     (ydim >> 1) - 100L + 80L,
                       NOGORESHADOW, 0, 0);
          itemtoscreen((long) ((xdim - tilesizx[GORESOLID]) >> 1),
                       (svga == 0) ? (ydim >> 1) - 100L + 69L :
                                     (ydim >> 1) - 100L + 77L,
                       GORESOLID, 0, 0);
     }
     else {
          itemtoscreen((long) ((xdim - tilesizx[NOGORESOLID]) >> 1),
                       (svga == 0) ? (ydim >> 1) - 100L + 72L :
                                     (ydim >> 1) - 100L + 80L,
                       NOGORESOLID, 0, 0);
          itemtoscreen((long) ((xdim - tilesizx[GORESHADOW]) >> 1),
                       (svga == 0) ? (ydim >> 1) - 100L + 69L :
                                     (ydim >> 1) - 100L + 77L,
                       GORESHADOW, 0, 0);
     }

     goaltime = totalclock + SCOTTSPEED;

     while (!exit) {

          if (totalclock >= goaltime) {
               goaltime = totalclock + SCOTTSPEED;
               cnt++;
               if (cnt > 2)
                    cnt = 0;

               if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
                    TEMPSND();
                    select2 = 0;
               }
               if (keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
                    TEMPSND();
                    select2 = 1;
               }
               if (select2 == 0)
                    pickone = 0;
               else
                    pickone = 1;

               redpicnum = HORNYSKULL1 + select;

               if (svga == 0) {
                    permanentwritesprite(0L, 0L, ZZGORE, 0,
                                         0L, 0L, 319L, 199L, 0);
               }
               else {
                    permanentwritesprite(0L, 0L, VGOREA, 0,
                                         0L, 0L, xdim-1L, ydim-1L, 0);
                    permanentwritesprite(0L, 240L, VGOREB, 0,
                                         0L, 0L, xdim-1L, ydim-1L, 0);
               }

               if (select2 == 0) {
                    switch (cnt) {
                    case 0:
                         itemtoscreen((svga == 0) ? (xdim >> 1) - 160L + 66L :
                                                    (xdim >> 1) - 160L + 37L,
                                      (svga == 0) ? (ydim >> 1) - 100L + 9L :
                                                    (ydim >> 1) - 100L - 1L,
                                      (svga == 0) ? ZZBLOOD : 3690,
                                      0, 0);
                         break;
                    case 1:
                         itemtoscreen((svga == 0) ? (xdim >> 1) - 160L + 66L :
                                                    (xdim >> 1) - 160L + 37L,
                                      (svga == 0) ? (ydim >> 1) - 100L + 9L :
                                                    (ydim >> 1) - 100L - 1L,
                                      (svga == 0) ? ZFLASHER1 + 10 : 3702,
                                      0, 0);
                         break;
                    case 2:
                         itemtoscreen((svga == 0) ? (xdim >> 1) - 160L + 66L :
                                                    (xdim >> 1) - 160L + 37L,
                                      (svga == 0) ? (ydim >> 1) - 100L + 9L :
                                                    (ydim >> 1) - 100L - 1L,
                                      (svga == 0) ? ZFLASHER2 + 10 : 3709,
                                      0, 0);
                         break;
                    }

               }
               else {
                    switch (cnt) {
                    case 0:
                         itemtoscreen((svga == 0) ? (xdim >> 1) - 160L + 72L :
                                                    (xdim >> 1) - 160L + 48L,
                                      (svga == 0) ? (ydim >> 1) - 100L + 74L :
                                                    (ydim >> 1) - 100L + 74L,
                                      (svga == 0) ? ZZDIFFICULTY : 3691,
                                      0, 0);
                         break;
                    case 1:
                         itemtoscreen((svga == 0) ? (xdim >> 1) - 160L + 72L :
                                                    (xdim >> 1) - 160L + 48L,
                                      (svga == 0) ? (ydim >> 1) - 100L + 74L :
                                                    (ydim >> 1) - 100L + 74L,
                                      (svga == 0) ? ZFLASHER1 + 11 : 3703,
                                      0, 0);
                         break;
                    case 2:
                         itemtoscreen((svga == 0) ? (xdim >> 1) - 160L + 72L :
                                                    (xdim >> 1) - 160L + 48L,
                                      (svga == 0) ? (ydim >> 1) - 100L + 74L :
                                                    (ydim >> 1) - 100L + 74L,
                                      (svga == 0) ? ZFLASHER2 + 11 : 3710,
                                      0, 0);
                         break;
                    }
               }

               itemtoscreen((long) ((xdim - tilesizx[HORNYBACK]) >> 1),
                                   (svga == 0) ? (ydim >> 1) - 100L + 136L :
                                                 (ydim >> 1) - 100L + 144L,
                                   HORNYBACK, 0, 0);
               itemtoscreen((long) (xdim >> 1) - 160L + (redpic[select].x - 64),
                                   (svga == 0) ?
                                   (ydim >> 1)-100L+(redpic[select].y+3) :
                                   (ydim >> 1)-100L+(redpic[select].y+3)+8L,
                                   redpicnum, 0, 0);

               if (goreon == 1) {
                    itemtoscreen((long) (xdim >> 1) - 160L + (160 - 64),
                                        (svga == 0) ? (ydim >> 1)-100L+72L :
                                                      (ydim >> 1)-100L+80L,
                                        NOGORESHADOW, 0, 0);
                    itemtoscreen((long) (xdim >> 1) - 160L + (160 + 32),
                                        (svga == 0) ? (ydim >> 1)-100L+69L :
                                                      (ydim >> 1)-100L+77L,
                                        GORESOLID, 0, 0);
               }
               else {
                    itemtoscreen((long) (xdim >> 1) - 160L + (160 - 64),
                                        (svga == 0) ? (ydim >> 1)-100L+72L :
                                                      (ydim >> 1)-100L+80L,
                                        NOGORESOLID, 0, 0);
                    itemtoscreen((long) (xdim >> 1) - 160L + (160 + 32),
                                        (svga == 0) ? (ydim >> 1)-100L+69L :
                                                      (ydim >> 1)-100L+77L,
                                        GORESHADOW, 0, 0);
               }

               if (pickone == 1) {
                    if (keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
                         TEMPSND();
                         select--;
                         if (select < 0)
                              select = 0;
                    }
                    if (keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
                         TEMPSND();
                         select++;
                         if (select > 3)
                              select = 3;
                    }
                    selected = select;
               }
               else {
                    if (keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
                         TEMPSND();
                         select3 = 0;
                    }
                    if (keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
                         TEMPSND();
                         select3 = 1;
                    }
                    if (select3 == 0)
                         goreon = 0;
                    else
                         goreon = 1;
               }
               if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    exit = 1;
                    keystatus[0x1c] = keystatus[0x9c] = 0;
               }
               if (keystatus[1] > 0) {
                    exit = 2;
                    keystatus[1] = 0;
               }
               nextpage();
          }                        // IF

     }                             // WHILE

     if (exit == 1) {
          switch (selected) {
          case 0:
               difficulty = 1;
               break;
          case 1:
               difficulty = 2;
               break;
          case 2:
               difficulty = 3;
               break;
          case 3:
               difficulty = 4;
               break;
          }
     }


}

extern
int  justteleported,
     mapflag,
     nobreakflag,
     show2dobjectsflag,
     show2dmapflag;

void
startnewgame(struct player * plr)
{

     char temp[20];
     char tempshow2dsector[MAXSECTORS >> 3];
     char tempshow2dwall[MAXWALLS >> 3];
     char tempshow2dsprite[MAXSPRITES >> 3];
     int  i;

     justteleported=0;
     if (netgame) {
          // dolevelmusic(0);
          // SND_StartMusic(mapon-1);
          goto skip;
     }
     if (loadedgame == 0) {
          if (mapflag == 0) {
               mapon = 1;
          }
          strcpy(boardname, "level");
          itoa(mapon, temp, 10);
          strcat(boardname, temp);
          strcat(boardname, ".map");
          setupboard(boardname);
          initplayersprite();
          cleanup();
          if (musicoverride == -1) {
               dolevelmusic(mapon - 1);
          }
     }
     else if (loadedgame == 1) {
          setupboard(boardname);
          loadplayerstuff();
          initplayersprite();
#if 0
          for (i = 0; i < (MAXSECTORS >> 3); i++)
               tempshow2dsector[i] = show2dsector[i];
          for (i = 0; i < (MAXWALLS >> 3); i++)
               tempshow2dwall[i] = show2dwall[i];
          for (i = 0; i < (MAXSPRITES >> 3); i++)
               tempshow2dsprite[i] = show2dsprite[i];
          for (i = 0; i < (MAXSECTORS >> 3); i++)
               show2dsector[i] = tempshow2dsector[i];
          for (i = 0; i < (MAXWALLS >> 3); i++)
               show2dwall[i] = tempshow2dwall[i];
          for (i = 0; i < (MAXSPRITES >> 3); i++)
               show2dsprite[i] = tempshow2dsprite[i];
#endif
          loadedgame = 0;
          if (musicoverride == -1) {
               dolevelmusic(mapon - 1);
          }
     }
skip:
     godmode=0;
     nobreakflag=0;
     show2dobjectsflag=0;
     show2dmapflag=0;
     if (plr->screensize < 320)
          permanentwritesprite(0L, 0L, BACKGROUND, 0, 0L, 0L, 319L, 199L, 0);

     if (plr->screensize <= 320) {
          permanentwritesprite(0L, 154L, NEWSTATUSBAR, 0, 0L, 0L, 319L, 199L, 0);
     }
     updatepics();

}

void
loadgame(struct player * plr)
{

     int  select = 0;
     int  exit = 0;
     int  gn,
          i,
          step = 0;
     char temp[20];
     long goaltime;

     if (quicksaveflag) {
          select=10;
          exit=2;
          goto quickloadlabel;
     }

     goaltime = totalclock + SCOTTSPEED;

     for (i = 0; i < MAXSAVEDGAMES; i++)
          if (!savedgamedat(i))
               strcpy(savedgamenames[i].name, "empty");

     while (!exit) {
          if (svga == 0) {
               permanentwritesprite(0L, 0L, ZLOAD, 0, 0L, 0L, 319L, 199L, 0);
          }
          else {
               rotatesprite(0L << 16, 0L << 16,
                            65536L, 0, VMAINBLANK, 0, 0, 8 + 16,
                            0L, 0L, xdim - 1L, ydim - 1L);
               rotatesprite(((xdim>>1)-160L)<<16, ((ydim>>1)-100L)<<16,
                            65536L, 0, ZLOAD, 0, 0, 1 + 16,
                            (xdim>>1)-160L,(ydim>>1)-100L,
                            (xdim>>1)+160L-1L,(ydim>>1)+100L-1L);
          }
          for (i = 0; i < MAXSAVEDGAMES; i++) {
               if (i == select) {
                    rotatesprite(((xdim>>1)-135L)<<16,
                                 (((ydim>>1)-69L)+(i * 19))<<16,
                                 16384L, 0, HELMET + step,
                                 0, 0, 8 + 16,
                                 0L, 0L, xdim-1L, ydim-1L);
               }
               fancyfont((xdim>>1)-83, ((ydim>>1)-69)+(i * 19),
                         DAFONT, savedgamenames[i].name, 0);
          }

          nextpage();

          if (totalclock >= goaltime) {
               goaltime = totalclock + SCOTTSPEED;
               step=(step+1)%8;
          }

          if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
               select--;
               if (select < 0)
                    select = MAXSAVEDGAMES-1;
               keystatus[keys[KEYFWD]] = 0;
               keystatus[RUP] = 0;
          }
          if (keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
               select++;
               if (select > MAXSAVEDGAMES-1)
                    select = 0;
               keystatus[keys[KEYBACK]] = 0;
               keystatus[RDN] = 0;
          }

          if (keystatus[1] > 0) {
               exit = 1;
               keystatus[1] = 0;
          }

          if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
               keystatus[0x1c] = keystatus[0x9c] = 0;
               if (strcmp(savedgamenames[select].name, "empty") == 0)
                    exit = 0;
               else {
                    exit = 2;
                    loadgo = 1;
               }
          }
     }
quickloadlabel:
     if (exit == 2) {
          keystatus[1] = keystatus[0x1c] = keystatus[0x9c] = 0;
          strcpy(boardname, "svgm");
          itoa(select, temp, 10);
          strcat(boardname, temp);
          strcpy(loadgamename, "svgn");
          strcat(loadgamename, temp);
          strcat(loadgamename, ".dat");
          strcat(boardname, ".map");
          if (access(boardname,F_OK) != 0) {
               return;
          }
          if (access(loadgamename,F_OK) != 0) {
               return;
          }
          loadedgame = 1;
          gameactivated = 1;
          startnewgame(plr);
     }
}

void
savegame(struct player * plr)
{

     int  exit = 0;
     int  gn,
          i,
          step = 0;
     int  select = 0;
     char temp[20];
     long goaltime;


     if (quicksaveflag) {
          strcpy(quicksavedgame.name,"QUICKSAVE");
          strcpy(tempbuf,"svgm10.map");
          saveboard(tempbuf, &plr->x, &plr->y, &plr->z, &plr->ang, &plr->sector);
          savedgamename(10);
          return;
     }
     for (i = 0; i < MAXSAVEDGAMES; i++)
          if (!savedgamedat(i))
               strcpy(savedgamenames[i].name, "EMPTY");

     goaltime = totalclock + SCOTTSPEED;

     while (!exit) {
          if (svga == 0) {
               permanentwritesprite(0L, 0L, ZSAVE, 0, 0L, 0L, 319L, 199L, 0);
          }
          else {
               rotatesprite(0L << 16, 0L << 16,
                            65536L, 0, VMAINBLANK, 0, 0, 8 + 16,
                            0L, 0L, xdim - 1L, ydim - 1L);
               rotatesprite(((xdim>>1)-160L)<<16, ((ydim>>1)-100L)<<16,
                            65536L, 0, ZSAVE, 0, 0, 1 + 16,
                            (xdim>>1)-160L,(ydim>>1)-100L,
                            (xdim>>1)+160L-1L,(ydim>>1)+100L-1L);
          }
          for (i = 0; i < MAXSAVEDGAMES; i++) {
               if (i == select) {
                    rotatesprite(((xdim>>1)-135L)<<16,
                                 (((ydim>>1)-69L)+(i * 19))<<16,
                                 16384L, 0, HELMET + step,
                                 0, 0, 8 + 16,
                                 0L, 0L, xdim-1L, ydim-1L);
               }
               fancyfont((xdim>>1)-83,((ydim>>1)-69)+(i * 19),
                         DAFONT, savedgamenames[i].name, 0);
          }

          nextpage();

          if (totalclock >= goaltime) {
               goaltime = totalclock + SCOTTSPEED;

               step = (step + 1) % 8;
          }

          if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
               select--;
               if (select < 0)
                    select = MAXSAVEDGAMES-1;
               keystatus[keys[KEYFWD]] = 0;
               keystatus[RUP] = 0;
          }

          if (keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
               select++;
               if (select > MAXSAVEDGAMES-1)
                    select = 0;
               keystatus[keys[KEYBACK]] = 0;
               keystatus[RDN] = 0;
          }

          if (keystatus[1] > 0) {
               exit = 1;
               keystatus[1] = 0;
          }

          if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
               exit = 2;
               typemessageleng = 0;
               keystatus[0x1c] = keystatus[0x9c] = 0;
               savegametext(select);
          }
     }

}

void 
savegametext(int select)
{

     struct player *plr;

     int  exit = 0;
     int  i,
          len,
          j;
     char temp[40];
     char temp1[40];
     char temp2[40];
     char temp3[40];
     int  typemessageleng = 0;

     plr = &player[pyrn];

     keystatus[0x1c] = keystatus[0x9c] = 0;

     for (i = 0; i < 128; i++)
          keystatus[i] = 0;

     if (strcmp(savedgamenames[select].name,"empty") == 0) {
          strcpy(temp,"");
     }
     else {
          sprintf(temp,"%s",savedgamenames[select].name);
     }
     typemessageleng=strlen(temp);

     while (!exit) {

          if (svga == 0) {
               permanentwritesprite(0L, 0L, ZSAVE, 0, 0L, 0L, 319L, 199L, 0);
          }
          else {
               rotatesprite(0L << 16, 0L << 16,
                            65536L, 0, VMAINBLANK, 0, 0, 8 + 16,
                            0L, 0L, xdim - 1L, ydim - 1L);
               rotatesprite(((xdim>>1)-160L)<<16, ((ydim>>1)-100L)<<16,
                            65536L, 0, ZSAVE, 0, 0, 1 + 16,
                            (xdim>>1)-160L,(ydim>>1)-100L,
                            (xdim>>1)+160L-1L,(ydim>>1)+100L-1L);
          }
          for (i = 0; i < MAXSAVEDGAMES; i++) {
               if (i == select) {
                    fancyfont((xdim>>1)-77, (ydim>>1)-100+31+(i * 19),
                              DAFONT, temp, 7);
               }
               else {
                    strcpy(tempbuf, savedgamenames[i].name);
                    fancyfont((xdim>>1)-77, (ydim>>1)-100+31+(i * 19),
                              DAFONT, tempbuf, 0);
               }
          }

          if (keystatus[0xe] > 0) {// backspace
               if (typemessageleng > 0) {
                    temp[typemessageleng] = '\0';
                    typemessageleng--;
                    temp[typemessageleng] = '\0';
               }
               else {
                    strcpy(temp, "-");
                    typemessageleng = 0;
               }
               keystatus[0xe] = 0;
          }




          if (typemessageleng < 10) {
               for (i = 0; i < 128; i++) {
                    if (keystatus[i] > 0
                        && keystatus[0x0e] == 0   // keypressed not
                        && keystatus[1] == 0 // esc
                        && keystatus[0x1c] == 0   // enter l or r
                        && keystatus[0x9c] == 0) {     // backspace
                         for (j = 0; j < 41; j++) {
                              if (scantoasc[i] == ' ') {
                                   continue;
                              }
                              else if (scantoasc[i] == fancy[j]) {
                                   temp[typemessageleng] = fancy[j];
                                   typemessageleng++;
                                   temp[typemessageleng] = '\0';
                                   keystatus[i] = 0;
                              }
                              else
                                   keystatus[i] = 0;
                         }
                    }
               }
          }
          if (keystatus[1] > 0) {
               exit = 1;
               keystatus[1] = 0;
          }
          if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
               if (typemessageleng > 0) {
                    strcpy(savedgamenames[select].name, temp);
                    strcpy(tempbuf, "svgm");
                    itoa(select, temp2, 10);
                    strcat(tempbuf, temp2);
                    strcat(tempbuf, ".map");
                    saveboard(tempbuf, &plr->x, &plr->y, &plr->z, &plr->ang, &plr->sector);
                    savedgamename(select);
               }
               else {
                    strcpy(tempbuf, "svgm");
                    itoa(select, temp2, 10);
                    strcat(tempbuf, temp2);
                    strcat(tempbuf, ".map");
                    saveboard(tempbuf, &plr->x, &plr->y, &plr->z, &plr->ang, &plr->sector);
                    savedgamename(select);
               }
               exit = 2;
               keystatus[0x1c] = keystatus[0x9c] = 0;
          }
          nextpage();
     }

     if (exit == 2) {
          keystatus[0x1c] = keystatus[0x9c] = 0;
     }

}
#if 0
void
savegametext(int select)
{

     struct player *plr;

     int  exit = 0;
     int  i,
          len,
          j;
     char temp[40];
     char temp1[40];
     char temp2[40];

     int  typemessageleng = 0;

     plr = &player[pyrn];

     keystatus[0x1c] = keystatus[0x9c] = 0;

     for (i = 0; i < 128; i++)
          keystatus[i] = 0;

     strcpy(temp, "-");

     while (!exit) {

          permanentwritesprite(0L, 0L, ZSAVE, 0, 0L, 0L, 319L, 199L, 0);

          for (i = 0; i < MAXSAVEDGAMES; i++) {
               if (i == select) {
                    strcpy(tempbuf, temp);
                    fancyfont(77, 31 + (i * 19), DAFONT, tempbuf, 7);
               }
               else {
                    strcpy(tempbuf, savedgamenames[i].name);
                    fancyfont(77, 31 + (i * 19), DAFONT, tempbuf, 0);
               }
          }
          if (keystatus[0xe] > 0) {// backspace
               if (typemessageleng > 0) {
                    temp[typemessageleng] = '\0';
                    typemessageleng--;
                    temp[typemessageleng] = '\0';
               }
               else {
                    strcpy(temp, "-");
                    typemessageleng = 0;
               }
               keystatus[0xe] = 0;
          }
          if (typemessageleng < 10) {
               for (i = 0; i < 128; i++) {
                    if (keystatus[i] > 0
                        && keystatus[0x0e] == 0   // keypressed not
                        && keystatus[1] == 0 // esc
                        && keystatus[0x1c] == 0   // enter l or r
                        && keystatus[0x9c] == 0) {     // backspace

                         for (j = 0; j < 41; j++) {
                              if (scantoasc[i] == ' ') {
                                   continue;
                              }
                              else if (scantoasc[i] == fancy[j]) {
                                   temp[typemessageleng] = fancy[j];
                                   typemessageleng++;
                                   temp[typemessageleng] = '\0';
                                   keystatus[i] = 0;
                              }
                              else
                                   keystatus[i] = 0;
                         }
                    }
               }
          }
          if (keystatus[1] > 0) {
               exit = 1;
               keystatus[1] = 0;
          }
          if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
               if (typemessageleng > 0) {
                    strcpy(savedgamenames[select].name, temp);
                    strcpy(tempbuf, "svgm");
                    itoa(select, temp2, 10);
                    strcat(tempbuf, temp2);
                    strcat(tempbuf, ".map");
                    saveboard(tempbuf, &plr->x, &plr->y, &plr->z, &plr->ang, &plr->sector);
                    savedgamename(select);
               }
               else {
                    strcpy(tempbuf, "svgm");
                    itoa(select, temp2, 10);
                    strcat(tempbuf, temp2);
                    strcat(tempbuf, ".map");
                    saveboard(tempbuf, &plr->x, &plr->y, &plr->z, &plr->ang, &plr->sector);
                    savedgamename(select);
               }
               exit = 2;
               keystatus[0x1c] = keystatus[0x9c] = 0;
          }
          nextpage();
     }

     if (exit == 2) {
          keystatus[0x1c] = keystatus[0x9c] = 0;
     }

}
#endif

extern
int  treasurescnt,
     treasuresfound,
     killcnt,
     kills,
     expgained;

int
savedgamename(int gn)
{

     struct player *plr;
     int  file;
     int  i;
     char temp[3];

     plr = &player[0];

     if (svga == 1)
          plr->screensize = 320;
     else
          plr->screensize = 320;

     strcpy(tempbuf, "svgn");
     itoa(gn, temp, 10);
     strcat(tempbuf, temp);
     strcat(tempbuf, ".dat");

     file = open(tempbuf, O_BINARY | O_TRUNC | O_CREAT | O_WRONLY, S_IWRITE);

     if (file != -1) {
          if (gn == 10) {
               write(file, quicksavedgame.name, sizeof(struct savedgame));
          }
          else {
               write(file, savedgamenames[gn].name, sizeof(struct savedgame));
          }
          write(file, &player[0], sizeof(struct player));
          write(file, &currweapon, sizeof(currweapon));
          write(file, &selectedgun, sizeof(selectedgun));
          write(file, &currentpotion, sizeof(currentpotion));
          write(file, &helmettime, sizeof(helmettime));
          write(file, &shadowtime, sizeof(shadowtime));
          write(file, &nightglowtime, sizeof(nightglowtime));
          write(file, &visibility, sizeof(visibility));
          write(file, &brightness, sizeof(brightness));
          write(file, &strongtime, sizeof(strongtime));
          write(file, &invisibletime, sizeof(invisibletime));
          write(file, &manatime, sizeof(manatime));
          write(file, &thunderflash, sizeof(thunderflash));
          write(file, &thundertime, sizeof(thundertime));

          write(file, &currentorb, sizeof(currentorb));
          write(file, &currweaponfired, sizeof(currweaponfired));
          write(file, &currweaponanim, sizeof(currweaponanim));
          write(file, &currweaponattackstyle, sizeof(currweaponattackstyle));
          write(file, &currweaponflip, sizeof(currweapon));
          write(file, &mapon, sizeof(mapon));

          write(file, &totalclock, sizeof(totalclock));
          write(file, &lockclock, sizeof(lockclock));
          write(file, &synctics, sizeof(synctics));

          // Warning: only works if all pointers are in sector structures!
          for (i = MAXANIMATES - 1; i >= 0; i--)
               animateptr[i] = (long *) (animateptr[i] - ((long) sector));

          write(file, animateptr, MAXANIMATES << 2);

          for (i = MAXANIMATES - 1; i >= 0; i--)
               animateptr[i] = (long *) (animateptr[i] + ((long) sector));

          write(file, animategoal, MAXANIMATES << 2);
          write(file, animatevel, MAXANIMATES << 2);
          write(file, &animatecnt, 4);

          write(file, show2dsector, MAXSECTORS >> 3);
          write(file, show2dwall, MAXWALLS >> 3);
          write(file, show2dsprite, MAXSPRITES >> 3);
          write(file, &automapping, 1);
          write(file, &shieldpoints, 2);
          write(file, &vampiretime, 2);
          write(file, &poisoned, 2);
          write(file, &poisontime, 2);

          write(file, &sparksx, sizeof(sparksx));
          write(file, &sparksy, sizeof(sparksy));
          write(file, &sparksz, sizeof(sparksz));

          write(file, &treasurescnt, sizeof(treasurescnt));
          write(file, &treasuresfound, sizeof(treasuresfound));
          write(file, &killcnt, sizeof(killcnt));
          write(file, &kills, sizeof(kills));
          write(file, &expgained, sizeof(expgained));

          close(file);
          return 1;
     }
     else
          return 0;



}

int
savedgamedat(int gn)
{

     int  fh = 0,
          nr = 0;
     char fname[20];
     char fsname[20];
     char savedgame[40];
     char temp[20];

     itoa(gn, temp, 10);
     strcpy(fname, "svgm");
     strcat(fname, temp);
     strcat(fname, ".map");

     itoa(gn, temp, 10);
     strcpy(fsname, "svgn");
     strcat(fsname, temp);
     strcat(fsname, ".dat");

     if (access(fname, F_OK) != 0)
          return 0;

     if (access(fsname, F_OK) != 0)
          return 0;

     fh = open(fsname, O_RDONLY | O_BINARY);

     if (fh < 0)
          return 0;

     if (gn == 10) {
          nr = read(fh, quicksavedgame.name, sizeof(struct savedgame));
     }
     else {
          nr = read(fh, savedgamenames[gn].name, sizeof(struct savedgame));
     }

     close(fh);

     if (nr != sizeof(struct savedgame))
          return 0;

     return 1;

}

void
loadplayerstuff(void)
{

     int  fh;
     int  i;
     char temp[40];

     fh = open(loadgamename, O_RDONLY | O_BINARY);

     read(fh, savedgamenames[0].name, sizeof(struct savedgame));
     read(fh, &player[0], sizeof(struct player));
     read(fh, &currweapon, sizeof(currweapon));
     read(fh, &selectedgun, sizeof(selectedgun));
     read(fh, &currentpotion, sizeof(currentpotion));
     read(fh, &helmettime, sizeof(helmettime));
     read(fh, &shadowtime, sizeof(shadowtime));
     read(fh, &nightglowtime, sizeof(nightglowtime));
     read(fh, &visibility, sizeof(visibility));
     read(fh, &brightness, sizeof(brightness));
     read(fh, &strongtime, sizeof(strongtime));
     read(fh, &invisibletime, sizeof(invisibletime));
     read(fh, &manatime, sizeof(manatime));
     read(fh, &thunderflash, sizeof(thunderflash));
     read(fh, &thundertime, sizeof(thundertime));

     read(fh, &currentorb, sizeof(currentorb));
     read(fh, &currweaponfired, sizeof(currweaponfired));
     read(fh, &currweaponanim, sizeof(currweaponanim));
     read(fh, &currweaponattackstyle, sizeof(currweaponattackstyle));
     read(fh, &currweaponflip, sizeof(currweaponflip));
     read(fh, &mapon, sizeof(mapon));

     read(fh, &totalclock, sizeof(totalclock));
     read(fh, &lockclock, sizeof(lockclock));
     read(fh, &synctics, sizeof(synctics));

     // Warning: only works if all pointers are in sector structures!
     for (i = MAXANIMATES - 1; i >= 0; i--)
          animateptr[i] = (long *) (animateptr[i] - ((long) sector));

     read(fh, animateptr, MAXANIMATES << 2);

     for (i = MAXANIMATES - 1; i >= 0; i--)
          animateptr[i] = (long *) (animateptr[i] + ((long) sector));

     read(fh, animategoal, MAXANIMATES << 2);
     read(fh, animatevel, MAXANIMATES << 2);
     read(fh, &animatecnt, 4);


     read(fh, show2dsector, MAXSECTORS >> 3);
     read(fh, show2dwall, MAXWALLS >> 3);
     read(fh, show2dsprite, MAXSPRITES >> 3);
     read(fh, &automapping, 1);
     read(fh, &shieldpoints, 2);
     read(fh, &vampiretime, 2);
     read(fh, &poisoned, 2);
     read(fh, &poisontime, 2);

     read(fh, &sparksx, sizeof(sparksx));
     read(fh, &sparksy, sizeof(sparksy));
     read(fh, &sparksz, sizeof(sparksz));

     read(fh, &treasurescnt, sizeof(treasurescnt));
     read(fh, &treasuresfound, sizeof(treasuresfound));
     read(fh, &killcnt, sizeof(killcnt));
     read(fh, &kills, sizeof(kills));
     read(fh, &expgained, sizeof(expgained));

     close(fh);

}

void
optionspage(void)
{

     int  which = 0;
     int  exit = 0;
     int  i;
     long goaltime;
     int  cnt = 0;

     struct thenames {
          long x;
          long y;
          short thename;
     }    danames[3] = {{
               107L, 35L, DASOUND
     },
     {
          97L, 72L, DAGAMEPLAY
     },
     {
          101L, 108L, DACONTROLS
     }
     };

     struct sthenames {
          long x;
          long y;
          short thename;
     }    sdanames[3] = {{
               254, 145, SFLASH4
     },
     {
          235, 183, SFLASH4 + 1
     },
     {
          253, 242, SFLASH4 + 2
     }
     };

     goaltime = totalclock + SCOTTSPEED;

     while (!exit) {

          if (svga == 0)
               permanentwritesprite(0L, 0L, ZOPS, 0, 0L, 0L, 319L, 199L, 0);
          else {
               rotatesprite(0L << 16, 0L << 16, 65536L, 0, VSOUNDA, 0, 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
               rotatesprite(0L << 16, 240L << 16, 65536L, 0, VSOUNDB, 0, 0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
          }

          for (i = 0; i < 3; i++) {
               if (svga == 0) {
                    if (which == i) {
                         switch (cnt) {
                         case 0:
                              itemtoscreen(danames[which].x, danames[which].y,
                                           danames[which].thename, 0, 0);
                              break;
                         case 1:
                              itemtoscreen(danames[which].x, danames[which].y,
                                           ZFLASHER1 + 5 + which, 0, 0);
                              break;
                         case 2:
                              itemtoscreen(danames[which].x, danames[which].y,
                                           ZFLASHER2 + 5 + which, 0, 0);
                              break;
                         }
                    }
               }
               else {
                    switch (cnt) {
                    case 0:
                         itemtoscreen(sdanames[which].x, sdanames[which].y,
                                      sdanames[which].thename, 0, 0);
                         break;
                    case 1:
                         itemtoscreen(sdanames[which].x, sdanames[which].y,
                                      SFLASH1 + 7 + which, 0, 0);
                         break;
                    case 2:
                         itemtoscreen(sdanames[which].x, sdanames[which].y,
                                      SFLASH2 + 7 + which, 0, 0);
                         break;
                    }
               }
          }

          nextpage();

          if (totalclock >= goaltime) {

               goaltime = totalclock + SCOTTSPEED;
               cnt++;
               if (cnt > 2)
                    cnt = 0;

               if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
                    which--;
                    if (which < 0)
                         which = 2;
               }

               if (keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
                    which++;
                    if (which > 2)
                         which = 0;
               }

               if (keystatus[1] > 0) {
                    exit = 1;
                    keystatus[1] = 0;
               }

               if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    keystatus[0x1c] = keystatus[0x9c] = 0;
                    switch (which) {
                    case 0:
                         TEMPSND();
                         thesound();
                         break;
                    case 1:
                         TEMPSND();
                         thedifficulty();
                         break;
                    case 2:
                         TEMPSND();
                         thecontrols();
                         break;
                    }
               }
          }
     }


}



void
thesound(void)
{

     int  exit = 0;
     long goaltime;
     long bitchtime;
     long waittime;
     int  which = 0;
     int  musicslider = 0,
          fxslider = 0,
          playing = 0,
          stoped = 0,
          shuffle = 0;
     int  forward, track = 0;
     long dax1, dax2;
     long x1,
          y1;
     long cnt = tilesizy[KATIEASS];
     long xbase,ybase,xlen,ylen,xoff,yoff;
     short incr = 0, dapic, pic2;

     struct template {
          long x;
          long y;
          short daname;
     }    dastuff[22] = {{
               66L, 45L, CDMUVOL
     },
     {
          71L, 79L, CDMUVOL + 1
     },
     {
          184L, 23L, CDMUVOL + 2
     },
     {
          229L, 24L, CDMUVOL + 3
     },
     {
          216L, 76L, CDMUVOL + 4
     },
     {
          184L, 40L, CDMUVOL + 5
     },
     {
          225L, 40L, CDMUVOL + 6
     },
     {
          57L, 105L, CDMUVOL + 7
     },
     {
          57L, 117L, CDMUVOL + 8
     },
     {
          57L, 129L, CDMUVOL + 9
     },
     {
          57L, 141L, CDMUVOL + 10
     },
     {
          57L, 153L, CDMUVOL + 11
     },
     {
          142L, 105L, CDMUVOL + 12
     },
     {
          142L, 117L, CDMUVOL + 13
     },
     {
          142L, 129L, CDMUVOL + 14
     },
     {
          142L, 141L, CDMUVOL + 15
     },
     {
          142L, 153L, CDMUVOL + 16
     },
     {
          216L, 105L, CDMUVOL + 17
     },
     {
          216L, 117L, CDMUVOL + 18
     },
     {
          216L, 129L, CDMUVOL + 19
     },
     {
          216L, 141L, CDMUVOL + 20
     },
     {
          216L, 153L, CDMUVOL + 21
     }
     };

     musiclevel = (wMIDIVol >> 3);
     digilevel = (wDIGIVol >> 11);

     dax2 = (long) (digilevel * 5);
     dax1 = (long) (musiclevel * 5);

     goaltime = totalclock + 10L;
     bitchtime = totalclock + 10L;
     waittime = totalclock + 720L;

     dapic = KATIEAT + 2;

     if (svga == 0) {
          pic2 = ZCDPLAYER;
          xlen = tilesizx[ZCDPLAYER];
          ylen = tilesizy[ZCDPLAYER];
          xoff = 0;
          yoff = 0;
     }
     else {
          pic2 = 1228;
          xlen = tilesizx[1228];
          ylen = tilesizy[1228];
          xoff = -40;
          yoff = -16;
     }
     xbase = (xdim >> 1) - (xlen >> 1);
     ybase = (ydim >> 1) - (ylen >> 1);

     while (!exit) {
          rotatesprite(0L << 16, 240L << 16, 65536L, 0, VSOUNDB, 0, 0, 8 + 16,
                       0L, 0L, xdim - 1L, ydim - 1L);
          permanentwritesprite(xbase, ybase, pic2, 0,
                               0L, 0L, xdim-1L, ydim-1L, 0);
          itemtoscreen(xbase + dastuff[which].x + xoff,
                       ybase + dastuff[which].y + yoff,
                       dastuff[which].daname, 0, 0);

          itemtoscreen(xbase + 60L + dax1 + xoff,
                       ybase + 25L + yoff, DASLIDER, 0, 0);
          itemtoscreen(xbase + 60L + dax2 + xoff,
                       ybase + 60L + yoff, DASLIDER, 0, 0);

          if (playing) {
               itemtoscreen(xbase + dastuff[5].x + xoff,
                            ybase + dastuff[5].y + yoff,
                            REDSHUFFLE + 1, 0, 0);
          }
          if (stoped) {
               itemtoscreen(xbase + dastuff[6].x + xoff,
                            ybase + dastuff[6].y + yoff,
                            REDSHUFFLE + 2, 0, 0);
          }
          if (shuffle) {
               itemtoscreen(xbase + dastuff[4].x + xoff,
                            ybase + dastuff[4].y + yoff,
                            REDSHUFFLE, 0, 0);
          }
          if (track) {
               itemtoscreen(xbase + dastuff[track].x + xoff,
                            ybase + dastuff[track].y + yoff,
                            REDSHUFFLE + track - 4, 0, 0);

               if (cnt) {
                    x1 = (long) (((xdim >> 1) - tilesizx[KATIEASS]) >> 1);
                    y1 = (long) (ydim - tilesizy[KATIEASS]);
               }
               else {
                    x1 = (long) (((xdim >> 1) - tilesizx[dapic]) >> 1);
                    y1 = (long) (ydim - tilesizy[dapic]);
               }

               if (which == 9 && track == 9) {
                    if (totalclock > waittime) {
                         if (cnt) {
                              rotatesprite(xbase + (x1 << 16) + xoff,
                                           ybase + ((y1 + cnt) << 16) + yoff,
                                           65536L, 0, KATIEASS + incr, 0,
                                           0, 8 + 16,
                                           0L, 0L, xdim - 1L, ydim - 1L);
                              cnt--;
                              if (cnt % 8 == 0) {
                                   incr++;
                                   if (incr == 5) {
                                        incr = 0;
                                   }
                              }
                         }
                         else {
                              rotatesprite(xbase + (x1 << 16) + xoff,
                                           ybase + (y1 << 16) + yoff,
                                           65536L, 0, dapic, 0,
                                           0, 8 + 16,
                                           0L, 0L, xdim - 1L, ydim - 1L);
                         }
                    }
               }

               if (totalclock > bitchtime) {
                    bitchtime = totalclock + 10L;
                    if (forward) {
                         if (dapic < KATIEAT + 6) {
                              dapic++;
                         }
                         else {
                              forward = 0;
                         }
                    }
                    else {
                         if (dapic == KATIEAT + 2) {
                              forward = 1;
                              dapic = KATIEAT + 2;
                         }
                         else {
                              dapic--;
                         }
                    }
               }
          }

          if (totalclock > goaltime) {
               goaltime = totalclock + 10L;

               switch (which) {
               case 0:
                    musicslider = 1;
                    fxslider = 0;
                    break;
               case 1:
                    musicslider = 0;
                    fxslider = 1;
                    break;
               default:
                    musicslider = 0;
                    fxslider = 0;
                    break;
               }

               if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
                    which--;
                    if (which == 5 || which == 6)
                         which = 4;
                    if (which < 0)
                         which = 21;
               }

               if (keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
                    which++;
                    if (which == 5 || which == 6)
                         which = 7;
                    if (which > 21)
                         which = 0;
               }

               if (fxslider) {
                    if (keystatus[keys[KEYRIGHT]] > 0) {
                         dax2 += 5L;
                         digilevel++;
                         keystatus[keys[KEYRIGHT]] = 0;
                         if (dax2 > 80L)
                              dax2 = 80L;
                         if (digilevel > 16)
                              digilevel = 16;
                         SND_Mixer(0, digilevel);
                         SND_Sound(S_LOUDCHAINWALK);
                    }
                    if (keystatus[keys[KEYLEFT]] > 0) {
                         dax2 -= 5L;
                         digilevel--;
                         keystatus[keys[KEYLEFT]] = 0;
                         if (dax2 < 0L)
                              dax2 = 0L;
                         if (digilevel < 0)
                              digilevel = 0;
                         SND_Mixer(0, digilevel);
                         SND_Sound(S_LOUDCHAINWALK);
                    }
               }

               else if (musicslider) {
                    if (keystatus[keys[KEYRIGHT]] > 0) {
                         dax1 += 5L;
                         musiclevel++;
                         keystatus[keys[KEYRIGHT]] = 0;
                         if (dax1 > 80L)
                              dax1 = 80L;
                         if (musiclevel > 16)
                              musiclevel = 16;
                         SND_Mixer(1, musiclevel);
                    }
                    if (keystatus[keys[KEYLEFT]] > 0) {
                         dax1 -= 5L;
                         musiclevel--;
                         keystatus[keys[KEYLEFT]] = 0;
                         if (dax1 < 0L)
                              dax1 = 0L;
                         if (musiclevel < 0)
                              musiclevel = 0;
                         SND_Mixer(1, musiclevel);
                    }
               }
               if (keystatus[1] > 0) {
                    exit = 1;
               }
               else if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    keystatus[0x1c] = keystatus[0x9c] = 0;
                    switch (which) {
                    case 2:
                         playing = 1;
                         stoped = 0;
                         shuffle = 0;
                         track = 0;
                         break;
                    case 3:
                         playing = 0;
                         stoped = 1;
                         shuffle = 0;
                         track = 0;
                         musicoverride = -1;
                         break;
                    case 4:
                         playing = 0;
                         stoped = 0;
                         shuffle = 1;
                         track = 0;
                         dolevelmusic(rand() % 10);
                         // SND_StartMusic(rand()%25);
                         break;
                    }
                    if (which > 6 && which < 22) {
                         track = which;
                         dolevelmusic(track - 7);
                         forward = 1;
                         // SND_StartMusic(track);
                         if (which != 9) {
                              waittime = totalclock + 720L;
                              cnt = tilesizy[KATIEASS];
                         }
                         musicoverride = track - 7;
                    }
                    else {
                         track = 0;
                         musicoverride = -1;
                    }
               }
          }
          nextpage();
     }

}

void
thecontrols(void)
{
     int  i;
     int  exit = 0;
     int  which = 0;
     long goaltime;
     int  reverse = 0;

     struct dastuff {
          long x;
          long y;
          short daname;
          long val;
          long off;
          long off2;
     }    dacontrols[14] = {{
               25L, 36L, ZCONTROLS + 1, 0L, 77L, 77L
     },
     {
          25L, 52L, ZCONTROLS + 2, 0L, 77L, 77L
     },

     {
          22L, 107L, ZCONTROLS + 3, 0L, 99L, 99L
     },
     {
          26L, 124L, ZCONTROLS + 4, 0L, 99L, 99L
     },
     {
          27L, 138L, ZCONTROLS + 5, 0L, 99L, 99L
     },
     {
          28L, 152L, ZCONTROLS + 6, 0L, 99L, 99L
     },
     {
          28L, 166L, ZCONTROLS + 7, 0L, 99L, 99L
     },
     {
          28L, 180L, ZCONTROLS + 8, 0L, 99L, 99L
     },

     {
          177L, 68L, ZCONTROLS + 9, 0L, 255L, 255L
     },
     {
          177L, 81L, ZCONTROLS + 10, 0L, 255L, 255L
     },
     {
          177L, 95L, ZCONTROLS + 11, 0L, 255L, 255L
     },
     {
          178L, 110L, ZCONTROLS + 12, 0L, 255L, 255L
     },
     {
          235L, 125L, ZZON, 0L, 255L, 255L
     },
     {
          262L, 125L, ZZOFF, 0L, 255L, 255L
     }
     };

     goaltime = totalclock + 10L;

     // set the controls values
     // for the mouse
     dacontrols[0].val = mousxspeed;
     dacontrols[1].val = mousyspeed;
     // for the avenger

     // for the wingman
     if (wingmanflag) {
          dacontrols[8].val = wingmanyawsens;
          dacontrols[9].val = wingmanpitchsens;
          dacontrols[10].val = wingmanrollsens;
          dacontrols[11].val = wingmanzsens;
          dacontrols[12].val = wingmanzreverse;
     }


     // for the mouse
     dacontrols[0].off2 = dacontrols[0].off + (dacontrols[0].val * 3);
     dacontrols[1].off2 = dacontrols[1].off + (dacontrols[1].val * 3);
     // for the avenger

     // for the wingman
     if (wingmanflag) {
          dacontrols[8].off2 = dacontrols[8].off + (dacontrols[8].val * 3);
          dacontrols[9].off2 = dacontrols[9].off + (dacontrols[9].val * 3);
          dacontrols[10].off2 = dacontrols[10].off + (dacontrols[10].val * 3);
          dacontrols[11].off2 = dacontrols[11].off + (dacontrols[11].val * 3);
          dacontrols[12].off2 = dacontrols[12].off + (dacontrols[12].val * 3);
     }

     while (!exit) {
          permanentwritesprite((xdim>>1)-160L, (ydim>>1)-100L,
                               ZCONTROLS, 0,
                               (xdim>>1)-160L, (ydim>>1)-100L,
                               (xdim>>1)+160L-1L, (ydim>>1)+100L-1L, 0);
          for (i = 0; i < 14; i++) {
               if (i == which) {
                    itemtoscreen((xdim>>1)-160L+dacontrols[i].x,
                                 (ydim>>1)-100L+dacontrols[i].y,
                                 dacontrols[i].daname, 0, 0);
               }
               if (i < 12) {
                    itemtoscreen((xdim>>1)-160L+dacontrols[i].off2,
                                 (ydim>>1)-100L+dacontrols[i].y,
                                 ZZSLIDER, 0, 0);
               }
#if 0
               if (reverse == 0) {
                    itemtoscreen(dacontrols[i].x, dacontrols[i].y, ZZON, 0, 0);
               }
               else {
                    itemtoscreen(dacontrols[i].x, dacontrols[i].y, ZZOFF, 0, 0);
               }
#endif
          }
          nextpage();
          if (totalclock > goaltime) {
               goaltime = totalclock + 10L;
               if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
                    which--;
                    if (which < 0)
                         which = 13;
               }
               if (keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
                    which++;
                    if (which > 13)
                         which = 0;
               }
               if (which < 12) {
                    if (keystatus[keys[KEYRIGHT]] > 0) {
                         dacontrols[which].val++;
                         keystatus[keys[KEYRIGHT]] = 0;
                         if (dacontrols[which].val > 10)
                              dacontrols[which].val = 10;
                    }
                    if (keystatus[keys[KEYLEFT]] > 0) {
                         dacontrols[which].val--;
                         keystatus[keys[KEYRIGHT]] = 0;
                         if (dacontrols[which].val < 0)
                              dacontrols[which].val = 0;
                    }
               }
               else {
                    if (keystatus[keys[KEYRIGHT]] > 0) {
                         reverse = 1;
                         keystatus[keys[KEYRIGHT]] = 0;
                    }
                    if (keystatus[keys[KEYLEFT]] > 0) {
                         reverse = 0;
                         keystatus[keys[KEYRIGHT]] = 0;
                    }
               }
               dacontrols[which].off2 = dacontrols[which].off + (dacontrols[which].val * 3);
               if (keystatus[1] > 0) {
                    exit = 1;
               }
               else if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
                    // exit=2;
                    if (which == 12 || which == 13) {

                    }
               }
          }
     }
     mousxspeed = dacontrols[0].val;
     mousyspeed = dacontrols[1].val;

     if (wingmanflag) {
          wingmanyawsens = dacontrols[8].val;
          wingmanpitchsens = dacontrols[9].val;
          wingmanrollsens = dacontrols[10].val;
          wingmanzsens = dacontrols[11].val;
          wingmanzreverse = dacontrols[12].val;
     }


}


/***************************************************************************
 *   TEKERR    - crirical error handler for TEKWAR                         *
 *                                                                         *
 *                                                     12/15/94 Jeff S.    *
 ***************************************************************************/

int  criterr_flag;

int  __far
cehndlr(unsigned deverr, unsigned errcode, unsigned far * devhdr)
{
     criterr_flag = errcode;
     return (_HARDERR_IGNORE);
}


void
installcrerrhndlr(void)
{
     _harderr(cehndlr);

     return;
}



/***************************************************************************
 *   screen effects for TEKWAR follow...fades, palette stuff, etc..        *
 *                                                                         *
 *                                                     12/15/94 Jeff S.    *
 ***************************************************************************/



#define   MESSIAHRANDOMKEY    215
#define   LEDRANDOMKEY        10

char forcemessiah = 0;
long messcnt;
long ledcnt;
char ledflash;
char tremors;
char dofadein = 0;

#define   DEADTIME       2   // # minutes of nothing happening
#define   FLASHINTERVAL  20

long passlock,
     lastastep,
     lastbstep,
     astep,
     bstep;

extern char pee;
extern char flashflag;

void 
screenfx(struct player * plr)
{

     updatepaletteshifts();

     return;
}


#define   MAXSTRLEN      30

//
// (extern asm template)
//
// extern int near asm_main (int *parmeter1, int parameter2);
// Extern must be EXACT prototype.
//
 //  #pragma aux asm_main "*_"             /* Define the auxilary function. */ \
 //    parm caller     [eax] [ebx]         /* Calling parameters.           */ \
 //    value           [eax]               /* Return value.                 */ \
//    modify          [ecx]; // Non parameter registers that will be altered.

extern void near asmwaitvrt(int parm1);
#pragma aux asmwaitvrt  "*_"       \
parm caller[ecx] \
     modify[eax edx];

extern void near asmsetpalette(char *pal);
#pragma aux asmsetpalette "*_"     \
parm caller[esi] \
     modify[eax ecx edx];

void
clearpal(void)
{
     short i;

     outp(PEL_WRITE_ADR, 0);
     for (i = 0; i < 768; i++)
          outp(PEL_DATA, 0x00);

     return;
}


char palette1[256][3],
     palette2[256][3];

void
getpalette(char *palette)
{
     int  i;

     outp(PEL_READ_ADR, 0);
     for (i = 0; i < 768; i++)
          *palette++ = inp(PEL_DATA);
}

void
fillpalette(int red, int green, int blue)
{
     int  i;

     outp(PEL_WRITE_ADR, 0);
     for (i = 0; i < 256; i++) {
          outp(PEL_DATA, red);
          outp(PEL_DATA, green);
          outp(PEL_DATA, blue);
     }
}


char foggy = 0;

void
fadeout(int start, int end, int red, int green, int blue, int steps)
{
     int  i,
          j,
          orig,
          delta;
     char *origptr,
         *newptr;

     asmwaitvrt(1);
     getpalette(&palette1[0][0]);
     memcpy(palette2, palette1, 768);

     for (i = 0; i < steps; i++) {
          origptr = &palette1[start][0];
          newptr = &palette2[start][0];
          for (j = start; j <= end; j++) {
               orig = *origptr++;
               delta = red - orig;
               *newptr++ = orig + delta * i / steps;
               orig = *origptr++;
               delta = green - orig;
               *newptr++ = orig + delta * i / steps;
               orig = *origptr++;
               delta = blue - orig;
               *newptr++ = orig + delta * i / steps;
          }

          asmwaitvrt(1);
          asmsetpalette(&palette2[0][0]);
     }

     if (foggy == 0)
          fillpalette(red, green, blue);

     return;
}


void
fadein(int start, int end, int steps)
{
     int  i,
          j,
          delta;

     if (steps == 0) {
          return;
     }

     asmwaitvrt(1);
     getpalette(&palette1[0][0]);
     memcpy(&palette2[0][0], &palette1[0][0], sizeof(palette1));

     start *= 3;
     end = end * 3 + 2;

     // fade through intermediate frames
     for (i = 0; i < steps; i++) {
          for (j = start; j <= end; j++) {
               delta = palette[j] - palette1[0][j];
               palette2[0][j] = palette1[0][j] + delta * i / steps;
          }

          asmwaitvrt(1);
          asmsetpalette(&palette2[0][0]);
     }

     // final color
     asmsetpalette(palette);

     dofadein = 0;
     // clearkeys();
}


void
fog1(void)
{
     if (foggy == 0) {
          foggy = 1;
          fadeout(1, 254, 8, 8, 10, 2);
     }
     else {
          foggy = 0;
          fadein(0, 255, 2);
     }
}


void
fog2(void)
{
     char *lookptr;

     lookptr = palookup[0];
     palookup[0] = palookup[1];
     palookup[1] = lookptr;
}

void
makefxlookups(void)
{
     char palbuf[256];
     short i;

     for (i = 0; i < 256; i++)
          palbuf[i] = *(palookup[0] + i);

     // makepalookup(1, palbuf, 60,60,60,1);
     makepalookup(1, palbuf, 60, 10, 10, 1);
}



#define   NUMWHITESHIFTS      3
#define   WHITESTEPS          20
#define   WHITETICS           6

#define   NUMREDSHIFTS        4
#define   REDSTEPS            8

#define   NUMGREENSHIFTS      4
#define   GREENSTEPS          8

#define   NUMBLUESHIFTS       4
#define   BLUESTEPS           8

char whiteshifts[NUMREDSHIFTS][768];
char redshifts[NUMREDSHIFTS][768];
char greenshifts[NUMGREENSHIFTS][768];
char blueshifts[NUMBLUESHIFTS][768];

int  redcount,
     whitecount,
     greencount,
     bluecount;
char palshifted;

extern char palette[];

void
initpaletteshifts(void)
{
     char *workptr,
         *baseptr;
     int  i,
          j,
          delta;

     for (i = 1; i <= NUMREDSHIFTS; i++) {
          workptr = (char *) &redshifts[i - 1][0];
          baseptr = &palette[0];
          for (j = 0; j <= 255; j++) {
               delta = 64 - *baseptr;
               *workptr++ = *baseptr++ + delta * i / REDSTEPS;
               delta = -*baseptr;
               *workptr++ = *baseptr++ + delta * i / REDSTEPS;
               delta = -*baseptr;
               *workptr++ = *baseptr++ + delta * i / REDSTEPS;
          }
     }

     for (i = 1; i <= NUMWHITESHIFTS; i++) {
          workptr = (char *) &whiteshifts[i - 1][0];
          baseptr = &palette[0];
          for (j = 0; j <= 255; j++) {
               delta = 64 - *baseptr;
               *workptr++ = *baseptr++ + delta * i / WHITESTEPS;
               delta = 62 - *baseptr;
               *workptr++ = *baseptr++ + delta * i / WHITESTEPS;
               delta = 0 - *baseptr;
               *workptr++ = *baseptr++ + delta * i / WHITESTEPS;
          }
     }

     for (i = 1; i <= NUMGREENSHIFTS; i++) {
          workptr = (char *) &greenshifts[i - 1][0];
          baseptr = &palette[0];
          for (j = 0; j <= 255; j++) {
               delta = -*baseptr;
               *workptr++ = *baseptr++ + delta * i / GREENSTEPS;
               delta = 64 - *baseptr;
               *workptr++ = *baseptr++ + delta * i / GREENSTEPS;
               delta = -*baseptr;
               *workptr++ = *baseptr++ + delta * i / GREENSTEPS;
          }
     }

     for (i = 1; i <= NUMBLUESHIFTS; i++) {
          workptr = (char *) &blueshifts[i - 1][0];
          baseptr = &palette[0];
          for (j = 0; j <= 255; j++) {
               delta = -*baseptr;
               *workptr++ = *baseptr++ + delta * i / BLUESTEPS;
               delta = -*baseptr;
               *workptr++ = *baseptr++ + delta * i / BLUESTEPS;
               delta = 64 - *baseptr;
               *workptr++ = *baseptr++ + delta * i / BLUESTEPS;
          }
     }


     return;
}

void
startgreenflash(int greentime)
{
     greencount = 0;

     NOTFIXED

          greencount += greentime;

     if (greencount < 0) {
          greencount = 0;
     }

}

void
startblueflash(int bluetime)
{
     bluecount = 0;

     NOTFIXED

          bluecount += bluetime;

     if (bluecount < 0) {
          bluecount = 0;
     }

}


void
startredflash(int damage)
{


     redcount = 0;

     NOTFIXED

          redcount += damage;

     if (redcount < 0) {
          redcount = 0;
     }
}

void
startwhiteflash(int bonus)
{

     whitecount = 0;

     NOTFIXED

          whitecount += bonus;

     if (whitecount < 0) {
          whitecount = 0;
     }

}


void
updatepaletteshifts(void)
{
     int  red,
          white,
          green,
          blue;

     if (whitecount) {
          white = whitecount / WHITETICS + 1;
          if (white > NUMWHITESHIFTS)
               white = NUMWHITESHIFTS;
          whitecount -= synctics;
          if (whitecount < 0)
               whitecount = 0;
     }
     else {
          white = 0;
     }

     if (redcount) {
          red = redcount / 10 + 1;
          if (red > NUMREDSHIFTS)
               red = NUMREDSHIFTS;
          redcount -= synctics;
          if (redcount < 0)
               redcount = 0;
     }
     else {
          red = 0;
     }

     if (greencount) {
          green = greencount / 10 + 1;
          if (green > NUMGREENSHIFTS)
               green = NUMGREENSHIFTS;
          greencount -= synctics;
          if (greencount < 0)
               greencount = 0;
     }
     else {
          green = 0;
     }

     if (bluecount) {
          blue = bluecount / 10 + 1;
          if (blue > NUMBLUESHIFTS)
               blue = NUMBLUESHIFTS;
          bluecount -= synctics;
          if (bluecount < 0)
               bluecount = 0;
     }
     else {
          blue = 0;
     }

     if (red) {
          asmwaitvrt(1);
          asmsetpalette(redshifts[red - 1]);
          palshifted = 1;
     }
     else if (white) {
          asmwaitvrt(1);
          asmsetpalette(whiteshifts[white - 1]);
          palshifted = 1;
     }
     else if (green) {
          asmwaitvrt(1);
          asmsetpalette(greenshifts[green - 1]);
          palshifted = 1;
     }
     else if (blue) {
          asmwaitvrt(1);
          asmsetpalette(blueshifts[blue - 1]);
          palshifted = 1;
     }

     else if (palshifted) {
          asmwaitvrt(1);
          asmsetpalette(&palette[0]);   // back to normal
          // setbrightness(gbrightness);
          setbrightness(brightness, (char *) &palette[0]);
          palshifted = 0;
     }

     return;
}


void
finishpaletteshifts(void)
{
     if (palshifted == 1) {
          palshifted = 0;
          asmwaitvrt(1);
          asmsetpalette(&palette[0]);
     }

     return;
}

#if 0
void 
clearkeys(void)
{

     memset(keystatus, 0, sizeof(keystatus));

     // return;
}
#endif


//JUST USING THIS FOR NOW
void 
TEMPSND(void)
{
     SND_Sound(rand() % 145);
}

void 
cleanup(void)
{

     currweapon = 4;
     selectedgun = 4;
     currentpotion = 0;
     helmettime = -1;
     shadowtime = -1;
     nightglowtime = -1;
     strongtime = -1;
     invisibletime = -1;
     manatime = -1;
     currentorb = 0;
     currweaponfired = 3;
     currweaponanim = 0;
     currweaponattackstyle = 0;
     currweaponflip = 0;

}
