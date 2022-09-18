/****************************************************************************
               Witchaven

               WHFX.C

               Rafael Paiz

****************************************************************************/

#include "icorp.h"
#include <memcheck.h>

#pragma aux copybuf =\
   "rep movsd",\
   parm [esi][edi][ecx]\

#pragma aux klabs =\
   "test eax, eax",\
   "jns skipnegate",\
   "neg eax",\
   "skipnegate:",\
   parm [eax]\

#define FLOORUNIT    16384L

int  justwarpedfx = 0;
int  lastbat = -1;

extern int difficulty;
extern long selectedgun;


extern long angvel,
     svel,
     vel;

extern int justteleported;

extern short lavadrylandsector[32];
extern short lavadrylandcnt;

extern short revolvesector[],
     revolveang[],
     revolveclip[],
     revolvecnt;
extern long revolvex[16][16],
     revolvey[16][16];
extern long revolvepivotx[],
     revolvepivoty[];

extern short xpanningsectorlist[],
     xpanningsectorcnt;
extern short ypanningwalllist[],
     ypanningwallcnt;
extern short floorpanninglist[],
     floorpanningcnt;
extern short skypanlist[],
     skypancnt;
extern short crushsectorlist[],
     crushsectorcnt;
extern short crushsectoranim[],
     crushsectordone[];

extern short warpsectorlist[],
     warpsectorcnt;

char revolvesyncstat;
short revolvesyncang,
     revolvesyncrotang;
long revolvesyncx,
     revolvesyncy;

extern int mapon;

extern short brightness,
     gbrightness;

extern int displaytime;



void 
panningfx(void)
{

     int  i,
          s;
     short dasector;
     short startwall,
          endwall;
     short whichdir;
     struct player *plr;
     long lavax,
          lavay;

     plr = &player[pyrn];


     //
     // n ne e se s sw w nw
     // 0  1 2  3 4  5 6  7
     //
     for (i = 0; i < floorpanningcnt; i++) {
          whichdir = sector[floorpanninglist[i]].lotag - 80;

          switch (whichdir) {
          case 0:
               sector[floorpanninglist[i]].floorypanning = ((lockclock >> 2) & 255);
               break;
          case 1:
               sector[floorpanninglist[i]].floorxpanning = -((lockclock >> 2) & 255);
               sector[floorpanninglist[i]].floorypanning = ((lockclock >> 2) & 255);
               break;
          case 2:
               sector[floorpanninglist[i]].floorxpanning = -((lockclock >> 2) & 255);
               break;
          case 3:
               sector[floorpanninglist[i]].floorxpanning = -((lockclock >> 2) & 255);
               sector[floorpanninglist[i]].floorypanning = -((lockclock >> 2) & 255);
               break;
          case 4:
               sector[floorpanninglist[i]].floorypanning = -((lockclock >> 2) & 255);
               break;
          case 5:
               sector[floorpanninglist[i]].floorxpanning = ((lockclock >> 2) & 255);
               sector[floorpanninglist[i]].floorypanning = -((lockclock >> 2) & 255);
               break;
          case 6:
               sector[floorpanninglist[i]].floorxpanning = ((lockclock >> 2) & 255);
               break;
          case 7:
               sector[floorpanninglist[i]].floorxpanning = ((lockclock >> 2) & 255);
               sector[floorpanninglist[i]].floorypanning = ((lockclock >> 2) & 255);
               break;
          default:
               sector[floorpanninglist[i]].floorxpanning = 0;
               sector[floorpanninglist[i]].floorypanning = 0;
               break;
          }
     }

     for (i = 0; i < xpanningsectorcnt; i++) {
          dasector = xpanningsectorlist[i];
          startwall = sector[dasector].wallptr;
          endwall = startwall + sector[dasector].wallnum - 1;
          for (s = startwall; s <= endwall; s++)
               wall[s].xpanning = ((lockclock >> 2) & 255);
     }

     for (i = 0; i < ypanningwallcnt; i++)
          wall[ypanningwalllist[i]].ypanning = ~(lockclock & 255);

}


//redo this whole thing
/*
     goalz=sector[nextsectorneighborz(s,sector[s].floorz,-1,-1)].ceilingz;
     goalz=sector[nextsectorneighborz(s,sector[s].ceilingz,1,1)].floorz;
*/



void 
revolvefx(void)
{

     short startwall,
          endwall,
          wallfind[2];
     long i,
          j,
          k,
          s,
          nexti,
          good,
          cnt,
          datag;
     long dax,
          day,
          daz,
          dax2,
          day2,
          daz2,
          centx,
          centy;
     long xvect,
          yvect;
     struct player *plr;


     plr = &player[pyrn];

     for (i = 0; i < revolvecnt; i++) {

          startwall = sector[revolvesector[i]].wallptr;
          endwall = startwall + sector[revolvesector[i]].wallnum - 1;

          revolveang[i] = ((revolveang[i] + 2048 - (((long) synctics) << 1)) & 2047);
          for (k = startwall; k <= endwall; k++) {
               rotatepoint(revolvepivotx[i], revolvepivoty[i], revolvex[i][k - startwall], revolvey[i][k - startwall], revolveang[i], &dax, &day);
               dragpoint(k, dax, day);
          }
          if (plr->sector == revolvesector[i]) {
               revolvesyncang = plr->ang;
               revolvesyncrotang = 0;
               revolvesyncx = plr->x;
               revolvesyncy = plr->y;
               revolvesyncrotang = ((revolvesyncrotang + 2048 - (((long) synctics) << 1)) & 2047);
               rotatepoint(revolvepivotx[i], revolvepivoty[i], revolvesyncx, revolvesyncy, revolvesyncrotang, &plr->x, &plr->y);
               plr->ang = ((revolvesyncang + revolvesyncrotang) & 2047);
          }

     }


}

extern bobbingsectorcnt,
     bobbingsectorlist[];

void 
bobbingsector(void)
{

     short dasector;
     struct player *plr;
     int  i;

     plr = &player[pyrn];

     for (i = 0; i < bobbingsectorcnt; i++) {
          dasector = bobbingsectorlist[i];
          sector[dasector].floorz += (sintable[(lockclock << 4) & 2047] >> 6);
     }

}


#define NEXTLEVEL 1
#define ENDOFDEMO 2

void 
teleporter(void)
{
     extern void fadeout(int start, int end, int red, int green, int blue, int steps); // JEFF
     short dasector;
     short startwall,
          endwall;
     int  i,
          j;
     struct player *plr;
     int  s;
     short daang;

     plr = &player[pyrn];

     for (i = 0; i < warpsectorcnt; i++) {
          dasector = warpsectorlist[i];
          j = ((lockclock & 127) >> 2);
          if (j >= 16)
               j = 31 - j;
          {
               sector[dasector].ceilingshade = j;
               sector[dasector].floorshade = j;
               startwall = sector[dasector].wallptr;
               endwall = startwall + sector[dasector].wallnum - 1;
               for (s = startwall; s <= endwall; s++)
                    wall[s].shade = j;
          }
     }

     if (sector[plr->sector].lotag == 10) {
          if (plr->sector != plr->oldsector) {
               daang = (short) plr->ang;
               warpfxsprite(plr->spritenum);
               warp(&plr->x, &plr->y, &plr->z, &daang, &plr->sector);
               warpfxsprite(plr->spritenum);
               plr->ang = (int) daang;
               justwarpedfx = 48;
//JSA PLUTO
               playsound_loc(S_WARP, plr->x, plr->y);
               setsprite(plr->spritenum, plr->x, plr->y, plr->z + (32 << 8));
          }
     }

     if (sector[plr->sector].lotag == 4002) {
          if (plr->treasure[8] == 1) {
               plr->treasure[8] = 0;
               if (plr->sector != plr->oldsector) {
                    switch (sector[plr->sector].hitag) {
                    case NEXTLEVEL:
                         fadeout(0,255,0,0,0,64);           // JEFF
                         smkplayseq(2);
                         justteleported = 1;
                         mapon++;
                         vel = 0;
                         angvel = 0;
                         svel = 0;
                         playertorch = 0;
                         playsound_loc(S_WARP, plr->x, plr->y);
                         loadnewlevel(mapon);
                         warpfxsprite(plr->spritenum);
                         // cleans up the keys (ivorykey etc)
                         plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = plr->treasure[17] = 0;
                         plr->treasure[4] = plr->treasure[5] = 0;
                         SND_CheckLoops();
                         break;
                    case ENDOFDEMO:
                         playsound_loc(S_THUNDER1, plr->x, plr->y);
                         justteleported = 1;
                         victory(1);
                         SND_CheckLoops();
                         break;
                    }
               }
          }
          else {
               // player need pentagram to teleport
               strcpy(displaybuf, "ITEM NEEDED");
               displaytime = 360;
          }
     }

}

void 
warp(long *x, long *y, long *z, short *daang, short *dasector)
{

     short startwall,
          endwall,
          s;
     long i,
          dax,
          day;

     for (i = 0; i < warpsectorcnt; i++) {
          if (sector[warpsectorlist[i]].hitag == sector[*dasector].hitag && warpsectorlist[i] != *dasector) {
               *dasector = warpsectorlist[i];
               break;
          }
     }

     startwall = sector[*dasector].wallptr;
     endwall = startwall + sector[*dasector].wallnum - 1;
     dax = 0L, day = 0L;
     for (s = startwall; s <= endwall; s++) {
          dax += wall[s].x, day += wall[s].y;
          if (wall[s].nextsector >= 0)
               i = s;
     }
     *x = dax / (endwall - startwall + 1);
     *y = day / (endwall - startwall + 1);
     *z = sector[*dasector].floorz - (32 << 8);
     updatesector(*x, *y, dasector);
     dax = ((wall[i].x + wall[wall[i].point2].x) >> 1);
     day = ((wall[i].y + wall[wall[i].point2].y) >> 1);
     *daang = getangle(dax - *x, day - *y);

}

void 
warpsprite(short spritenum)
{

     short dasectnum;

     dasectnum = sprite[spritenum].sectnum;

     warpfxsprite(spritenum);
     warp(&sprite[spritenum].x, &sprite[spritenum].y, &sprite[spritenum].z,
          &sprite[spritenum].ang, &dasectnum);

     warpfxsprite(spritenum);
     setsprite(spritenum, sprite[spritenum].x, sprite[spritenum].y, sprite[spritenum].z);

}

#define GRATEROT   16
#define GRATEVERT  512

// no one is using this so don't forget to remove it
// this function controls iron gates that swing open or go up
void
ironbars(void)
{

     long i;
     short temp;
     long spritenum;
     long ironbarmove;
     long x1,
          x2,
          y1,
          y2;

     for (i = 0; i < ironbarscnt; i++) {
          if (ironbarsdone[i] == 1) {
               spritenum = ironbarsanim[i];
               switch (sprite[ironbarsanim[i]].hitag) {
               case 1:
                    sprite[ironbarsanim[i]].ang += synctics << 1;
                    if (sprite[ironbarsanim[i]].ang > 2047)
                         sprite[ironbarsanim[i]].ang -= 2047;
                    ironbarmove = ironbarsgoal[i] += synctics << 1;
                    setsprite((short) spritenum, sprite[spritenum].x, sprite[spritenum].y, sprite[spritenum].z);
                    if (ironbarsgoal[i] > 512) {
                         ironbarsgoal[i] = 0;
                         sprite[ironbarsanim[i]].hitag = 2;
                         ironbarsdone[i] = 0;
                    }
                    break;
               case 2:
                    sprite[ironbarsanim[i]].ang -= synctics << 1;
                    if (sprite[ironbarsanim[i]].ang < 0)
                         sprite[ironbarsanim[i]].ang += 2047;
                    ironbarsgoal[i] += synctics << 1;
                    setsprite(spritenum, sprite[spritenum].x, sprite[spritenum].y, sprite[spritenum].z);
                    if (ironbarsgoal[i] > 512) {
                         ironbarsgoal[i] = 0;
                         sprite[ironbarsanim[i]].hitag = 1;
                         ironbarsdone[i] = 0;
                    }
                    break;
               case 3:
                    sprite[ironbarsanim[i]].z -= synctics << 4;
                    if (sprite[ironbarsanim[i]].z < ironbarsgoal[i]) {
                         sprite[ironbarsanim[i]].z = ironbarsgoal[i];
                         sprite[ironbarsanim[i]].hitag = 4;
                         ironbarsdone[i] = 0;
                         ironbarsgoal[i] = sprite[ironbarsanim[i]].z + 6000;
                    }
                    setsprite((short) spritenum, sprite[spritenum].x, sprite[spritenum].y, sprite[spritenum].z);
                    break;
               case 4:
                    sprite[ironbarsanim[i]].z += synctics << 4;
                    if (sprite[ironbarsanim[i]].z > ironbarsgoal[i]) {
                         sprite[ironbarsanim[i]].z = ironbarsgoal[i];
                         sprite[ironbarsanim[i]].hitag = 3;
                         ironbarsdone[i] = 0;
                         ironbarsgoal[i] = sprite[ironbarsanim[i]].z - 6000;
                    }
                    setsprite((short) spritenum, sprite[spritenum].x, sprite[spritenum].y, sprite[spritenum].z);
                    break;
               }
          }
     }

}


//JSA BLORB
void
sectorsounds(void)
{

#if 0

     struct player *plr;
     unsigned short sec;
     int  index;

     if (!SoundMode)
          return;

     plr = &player[pyrn];

     sec = sector[plr->sector].extra;

     if (sec) {
          if (sec & 32768) {       // loop on/off sector

               if (sec & 1) {      // turn loop on if lsb is 1
                    index = ((sec ^ 32769) >> 1); // (32768|(sprite[i].lotag<<1)
                                                  // |1)
                    if (ambsoundarray[index].hsound == -1)
                         ambsoundarray[index].hsound = SND_PlaySound(ambsoundarray[index].soundnum, 0, 0, 0, -1);
               }

               else {              // turn loop off if lsb is 0 and its playing
                    index = ((sec ^ 32768) >> 1);
                    if (ambsoundarray[index].hsound != -1) {
                         SND_StopLoop(ambsoundarray[index].hsound);
                         ambsoundarray[index].hsound = -1;
                    }
               }
          }
          else {
               if (plr->z <= sector[plr->sector].floorz - (8 << 8))
                    playsound_loc(sec, plr->x, plr->y);
          }

     }
#endif
}


long scarytime = -1;
long scarysize = 0;

void 
scary(void)
{

     if (rand() > 32500 && rand() > 32500 && scarytime < 0) {
          scarytime = 180;
          scarysize = 30;
          SND_PlaySound(S_SCARYDUDE, 0, 0, 0, 0);
     }
     if (scarytime >= 0) {
          scarytime -= synctics << 1;
          scarysize += synctics << 1;
          if (scarytime > 140 && scarytime < 180)
               rotatesprite(320L << 15, 200L << 15, scarysize << 9, 0, SCARY, 0, 0, 1 + 2, windowx1, windowy1, windowx2, windowy2);
          if (scarytime > 120 && scarytime < 139)
               rotatesprite(320L << 15, 200L << 15, scarysize << 9, 0, SCARY + 1, 0, 0, 1 + 2, windowx1, windowy1, windowx2, windowy2);
          if (scarytime > 100 && scarytime < 119)
               rotatesprite(320L << 15, 200L << 15, scarysize << 9, 0, SCARY + 2, 0, 0, 1 + 2, windowx1, windowy1, windowx2, windowy2);
          if (scarytime > 0 && scarytime < 99)
               rotatesprite(320L << 15, 200L << 15, scarysize << 9, 0, SCARY + 3, 0, 0, 1 + 2, windowx1, windowy1, windowx2, windowy2);
     }

}

void 
dofx(void)
{

     scary();
     if (revolvecnt > 0)
          revolvefx();
     panningfx();
     teleporter();
     bobbingsector();
     if (ironbarscnt > 0)
          ironbars();
     thesplash();
     thunder();
     weaponpowerup();


}

void
weaponpowerup(void)
{
     int  i;
     int  ok = 0;
     int  which = 0;
     struct player *plr;
     long j;
     long nextj;

     plr = &player[0];

     if (sector[plr->sector].lotag == 50 && sector[plr->sector].hitag > 0) {
          sprintf(displaybuf, "weapons enchanted");
          displaytime = 360;
          for (i = 0; i < 10; i++) {
               if (plr->weapon[i] != 0 && plr->weapon[i] != 3) {
                    plr->preenchantedweapon[i] = plr->weapon[i];
                    plr->preenchantedammo[i] = plr->ammo[i];
                    plr->weapon[i] = 3;
                    switch (difficulty) {
                    case 0:
                         plr->ammo[i] = 25;
                         break;
                    case 1:
                         plr->ammo[i] = 20;
                         break;
                    case 2:
                    case 3:
                         plr->ammo[i] = 10;
                         break;
                    }
                    sector[plr->sector].hitag--;
                    if (sector[plr->sector].hitag == 0) {
                         j=headspritesect[plr->sector];
                         while (j != -1) {
                              nextj=nextspritesect[j];
                              if (sprite[j].picnum == CONE) {
                                   deletesprite((short) j);
                              }
                              else if (sprite[j].picnum == SPARKBALL) {
                                   deletesprite((short) j);
                              }
                              j=nextj;
                         }
                    }
#if 0
                    sector[plr->sector].hitag--;
                    if (sector[plr->sector].hitag == 0) {
                         for (j = 0; j < MAXSPRITES; j++) {
                              if (sprite[j].picnum == CONE)
                                   deletesprite((short) j);
                              if (sprite[j].picnum == SPARKBALL)
                                   deletesprite((short) j);
                         }
                    }
#endif
               }
          }
     }
}

int  thunderflash;
int  thundertime;

void
thunder(void)
{

     int  val;

     if (thunderflash == 0) {
          if ((gotpic[SKY >> 3] & (1 << (SKY & 7))) > 0) {
               gotpic[SKY >> 3] &= ~(1 << (SKY & 7));
               if (waloff[SKY] != -1) {
                    visibility = 1024;
                    if (rand() > 32700) {
                         thunderflash = 1;
                         thundertime = 120;
                    }
               }
          }
          else if ((gotpic[SKY2 >> 3] & (1 << (SKY2 & 7))) > 0) {
               gotpic[SKY2 >> 3] &= ~(1 << (SKY2 & 7));
               if (waloff[SKY2] != -1) {

                    visibility = 768;
                    if (rand() > 32700) {
                         thunderflash = 1;
                         thundertime = 120;
                    }
               }
          }
          else if ((gotpic[SKY3 >> 3] & (1 << (SKY3 & 7))) > 0) {
               gotpic[SKY3 >> 3] &= ~(1 << (SKY3 & 7));
               if (waloff[SKY3] != -1) {
                    visibility = 512;
                    if (rand() > 32700) {
                         thunderflash = 1;
                         thundertime = 120;
                    }
               }
          }
          else if ((gotpic[SKY4 >> 3] & (1 << (SKY4 & 7))) > 0) {
               gotpic[SKY4 >> 3] &= ~(1 << (SKY4 & 7));
               if (waloff[SKY4] != -1) {
                    visibility = 512;
                    if (rand() > 32700) {
                         thunderflash = 1;
                         thundertime = 120;
                    }
               }
          }
          else if ((gotpic[SKY5 >> 3] & (1 << (SKY5 & 7))) > 0) {
               gotpic[SKY5 >> 3] &= ~(1 << (SKY5 & 7));
               if (waloff[SKY5] != -1) {
                    visibility = 1024;
                    if (rand() > 32700) {
                         thunderflash = 1;
                         thundertime = 120;
                    }
               }
          }
          else if ((gotpic[SKY6 >> 3] & (1 << (SKY6 & 7))) > 0) {
               gotpic[SKY6 >> 3] &= ~(1 << (SKY6 & 7));
               if (waloff[SKY6] != -1) {
                    visibility = 512;
                    if (rand() > 32700) {
                         thunderflash = 1;
                         thundertime = 120;
                    }
               }
          }
          else if ((gotpic[SKY7 >> 3] & (1 << (SKY7 & 7))) > 0) {
               gotpic[SKY7 >> 3] &= ~(1 << (SKY7 & 7));
               if (waloff[SKY7] != -1) {
                    visibility = 512;
                    if (rand() > 32700) {
                         thunderflash = 1;
                         thundertime = 120;
                    }
               }
          }
          else {
               visibility = 1024;
          }
     }
     else {
          thundertime -= synctics;
          if (thundertime < 0) {
               thunderflash = 0;
               brightness = gbrightness;
               // setbrightness(brightness);
               setbrightness(brightness, (char *) &palette[0]);
               SND_Sound(S_THUNDER1 + (rand() % 4));
               visibility = 1024;
          }
     }

     if (thunderflash == 1) {
          if (waloff[SKY] != -1) {
               val = rand() % 4;
               brightness += val;
               switch (val) {
               case 0:
                    visibility = 2048;
                    break;
               case 1:
                    visibility = 1024;
                    break;
               case 2:
                    visibility = 512;
                    break;
               case 3:
                    visibility = 256;
                    break;
               default:
                    visibility = 4096;
                    break;
               }
               if (brightness > 8) {
                    brightness = 0;
               }
               // setbrightness(brightness);
               setbrightness(brightness, (char *) &palette[0]);
          }
     }
}

void
thesplash(void)
{
     struct player *plr;

     plr = &player[pyrn];

     if (sector[plr->sector].floorpicnum == WATER ||
         sector[plr->sector].floorpicnum == LAVA ||
         sector[plr->sector].floorpicnum == SLIME) {
          if (!plr->onsomething) {
               return;
          }
          if (plr->sector != plr->oldsector) {
               switch (sector[plr->sector].floorpicnum) {
               case WATER:
                    makeasplash(SPLASHAROO, plr);
                    break;
               case SLIME:
                    makeasplash(SLIMESPLASH, plr);
                    break;
               case LAVA:
                    makeasplash(LAVASPLASH, plr);
                    break;
               }
          }
     }
}


void
makeasplash(int picnum, struct player * plr)
{
     short j;
     short movestat;
     long dax,
          day;
     spritetype *spr;

     plr = &player[pyrn];

     j = insertsprite(plr->sector, MASPLASH);
     spr = &sprite[j];

     spr->x = plr->x;
     spr->y = plr->y;
     spr->z = sector[plr->sector].floorz + (tilesizy[picnum] << 8);
     spr->cstat = 0;
     spr->picnum = picnum;
     spr->shade = 0;
     spr->pal = 0;
     spr->xrepeat = 64;
     spr->yrepeat = 64;
     spr->owner = 0;
     spr->clipdist = 16;
     spr->lotag = 8;
     spr->hitag = 0;

     switch (picnum) {
     case SLIMESPLASH:
     case SPLASHAROO:
          playsound_loc(S_SPLASH1 + (krand() % 3), sprite[j].x, sprite[j].y);
          break;
     case LAVASPLASH:
          break;
     }
//JSA ends

     movestat = movesprite((short) j, (((long) sintable[(sprite[j].ang + 512) & 2047]) * synctics) << 3, (((long) sintable[sprite[j].ang]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 0);
}


void
makemonstersplash(int picnum, int i)
{
     short j;
     short movestat;
     long dax,
          day;

     if (sprite[i].picnum == FISH)
          return;

     j = insertsprite(sprite[i].sectnum, MASPLASH);
     sprite[j].x = sprite[i].x;
     sprite[j].y = sprite[i].y;
     sprite[j].z = sector[sprite[i].sectnum].floorz + (tilesizy[picnum] << 8);
     sprite[j].cstat = 0;          // Hitscan does not hit other bullets
     sprite[j].picnum = picnum;
     sprite[j].shade = 0;

     if (sector[sprite[i].sectnum].floorpal == 9)
          sprite[j].pal = 9;
     else
          sprite[j].pal = 0;

     sprite[j].xrepeat = 64;
     sprite[j].yrepeat = 64;
     sprite[j].owner = 0;
     sprite[j].clipdist = 16;
     sprite[j].lotag = 8;
     sprite[j].hitag = 0;

//JSA 5/3 start
     switch (picnum) {
     case SPLASHAROO:
     case SLIME:
          if (rand() % 2) {
               if ((gotpic[WATER >> 3] & (1 << (WATER & 7))) > 0) {
                    gotpic[WATER >> 3] &= ~(1 << (WATER & 7));
                    if (waloff[WATER] != -1)
                         if (rand() % 2)
                              playsound_loc(S_SPLASH1 + (krand() % 3), sprite[j].x, sprite[j].y);
               }
          }
          if (rand() % 2) {
               if ((gotpic[SLIME >> 3] & (1 << (SLIME & 7))) > 0) {
                    gotpic[SLIME >> 3] &= ~(1 << (SLIME & 7));
                    if (waloff[SLIME] != -1)
                         if (rand() % 2)
                              playsound_loc(S_SPLASH1 + (krand() % 3), sprite[j].x, sprite[j].y);
               }
          }
          break;
     case LAVASPLASH:
          break;
     }
//JSA ends
}

void
bats(short k)
{

     short i,
          j;

     j = insertsprite(sprite[k].sectnum, FLOCK);
     sprite[j].x = sprite[k].x;
     sprite[j].y = sprite[k].y;
     sprite[j].z = sprite[k].z;
     sprite[j].cstat = 0;
     sprite[j].picnum = BAT;
     sprite[j].shade = 0;
     sprite[j].xrepeat = 64;
     sprite[j].yrepeat = 64;
     sprite[j].ang = (sprite[k].ang + (krand() & 128 - 256)) & 2047;
     sprite[j].owner = k;
     sprite[j].clipdist = 16;
     sprite[j].lotag = 128;
     sprite[j].hitag = k;
     sprite[j].extra = 0;
     newstatus(j, FLOCK);

     if (sprite[k].extra == 1)
          lastbat = j;

}


void 
warpfxsprite(int s)
{
     struct player *plr;
     long j,
          daz;
     short daang;


     plr = &player[pyrn];

     j = insertsprite(sprite[s].sectnum, WARPFX);

     sprite[j].x = sprite[s].x;
     sprite[j].y = sprite[s].y;
     sprite[j].z = sprite[s].z - (32 << 8);

     sprite[j].cstat = 0;

     sprite[j].picnum = ANNIHILATE;

     if (s == plr->spritenum) {
          daang = (short) plr->ang;
          sprite[j].ang = daang;
     }
     else {
          daang = sprite[s].ang;
          sprite[j].ang = daang;
     }

     sprite[j].xrepeat = 48;
     sprite[j].yrepeat = 48;
     sprite[j].clipdist = 16;

     sprite[j].extra = 0;
     sprite[j].shade = -31;
     sprite[j].xvel = ((krand() & 256) - 128);
     sprite[j].yvel = ((krand() & 256) - 128);
     sprite[j].zvel = ((krand() & 256) - 128);
     sprite[j].owner = s;
     sprite[j].lotag = 12;
     sprite[j].hitag = 0;
     sprite[j].pal = 0;

     daz = ((((long) sprite[j].zvel) * synctics) >> 3);

     movesprite((short) j,
                (((long) sintable[(daang + 512) & 2047]) * synctics) << 3,
                (((long) sintable[daang]) * synctics) << 3,
                daz, 4L << 8, 4L << 8, 1);
}

void
makesparks(short i, int type)
{

     long j;

     switch (type) {
     case 1:
          j = insertsprite(sprite[i].sectnum, SPARKS);
          break;
     case 2:
          j = insertsprite(sprite[i].sectnum, SPARKSUP);
          break;
     case 3:
          j = insertsprite(sprite[i].sectnum, SPARKSDN);
          break;
     }

     sprite[j].x = sprite[i].x;
     sprite[j].y = sprite[i].y;
     sprite[j].z = sprite[i].z;
     sprite[j].cstat = 0;
     sprite[j].picnum = SPARKBALL;
     sprite[j].shade = 0;
     sprite[j].xrepeat = 24;
     sprite[j].yrepeat = 24;
     sprite[j].ang = (krand() % 2047) & 2047;
     sprite[j].owner = 0;
     sprite[j].clipdist = 16;
     sprite[j].lotag = rand() % 100;
     sprite[j].hitag = 0;
     sprite[j].extra = 0;
     sprite[j].pal = 0;
}

void
shards(short i, short type)
{

     short j;

     j = insertsprite(sprite[i].sectnum, SHARDOFGLASS);

     sprite[j].x = sprite[i].x + (((rand() % 512) - 256) << 2);
     sprite[j].y = sprite[i].y + (((rand() % 512) - 256) << 2);
     sprite[j].z = sprite[i].z - (PLAYERHEIGHT << 8) + (((rand() % 48) - 16) << 7);
     sprite[j].zvel = rand() % 256;
     sprite[j].cstat = 0;
     sprite[j].picnum = SHARD + (rand() % 3);
     sprite[j].shade = 0;
     sprite[j].xrepeat = 64;
     sprite[j].yrepeat = 64;
     sprite[j].ang = (sprite[i].ang + ((rand() % 512) - 256) & 2047);
     sprite[j].owner = i;
     sprite[j].clipdist = 16;
     sprite[j].lotag = 120 + (rand() % 100);
     sprite[j].hitag = 0;
     sprite[j].extra = type;
     sprite[j].pal = 0;


}
