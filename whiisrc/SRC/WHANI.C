
/***************************************************************************
 *   WHANI.C  - object animation code for Witchaven game                   *
 *                                                                         *
 ***************************************************************************/
#include "icorp.h"
#include <memcheck.h>

#pragma aux klabs =\
    "test eax, eax",\
    "jns skipnegate",\
    "neg eax",\
    "skipnegate:",\
    parm [eax]\

extern short lastbat;
extern int difficulty;

extern int cartsnd;
extern int batsnd;

extern int mapon;
extern int spellbooktics;
extern int spellbook;
extern int spellbookframe;
extern int spellbookflip;

extern int pressedkey;
extern long pageoffset;
extern long selectedgun;
extern int currentpotion;

extern short torchpattern[];

extern char flashflag;

extern int displaytime;
extern int shadowtime;
extern int helmettime;

extern int potiontilenum;
extern int debuginfo;

extern int scoretime;
extern int strongtime,
     invincibletime,
     manatime,
     invisibletime,
     nightglowtime;


void
animateobjs(struct player * plr)
{

     long hiz,
          hihit,
          loz,
          lohit;
     short osectnum,
          hitobject,
          hitdamage;
     long i,
          nexti,
          dax,
          day,
          daz,
          j,
          k;
     short daang;
     int  movestat;
     int  speed;
     long heat,
          ironbarmove;

     spritetype *spr,
         *tspr;
     long x1,
          x2,
          y1,
          y2;
     short startwall,
          endwall;

     long dax2;
     long dist,
          olddist;
     long xdist,
          ydist;
     int  found;

     long dx,
          dy,
          dz,
          dh;
     long nextj;

     long x,
          y,
          z;
     short belongs;

     long mindist = 0x7fffffff;
     short target;


     if (plr->sector < 0 || plr->sector >= numsectors) {
          return;
     }

     // CHILL
     i = headspritestat[CHILL];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          sprite[i].lotag -= synctics;
          if (sprite[i].lotag < 0) {
               sprite[i].picnum++;
               sprite[i].lotag = 18;
               if (sprite[i].picnum == HANGMAN + 10) {
                    sprite[i].picnum = SKELETON;
                    newstatus(i, FACE);
               }
          }
          i = nexti;
     }

     i = headspritestat[SHARDOFGLASS];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          switch (spr->extra) {
          case 1:
               spr->zvel += synctics << 3;
               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                     spr->zvel,
                                     4L << 8, 4L << 8, 0);
               break;
          case 2:
               spr->zvel += synctics << 5;
               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 1,
                                (((long) sintable[spr->ang]) * synctics) << 1,
                                     spr->zvel,
                                     4L << 8, 4L << 8, 0);
               break;
          case 3:
               spr->zvel -= synctics << 5;
               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 2,
                                (((long) sintable[spr->ang]) * synctics) << 2,
                                     spr->zvel,
                                     4L << 8, 4L << 8, 0);
               if (spr->lotag < 0) {
                    spr->lotag = 30;
                    spr->extra = 2;
               }
               break;
          }
          if (spr->lotag < 0) {
               deletesprite((short) i);
          }
          i = nexti;
     }

     i = headspritestat[LAND];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               spr->lotag = 12;
               spr->picnum++;
          }

          switch (spr->picnum) {
          case GONZOHMJUMPEND:
               spr->picnum = GONZOGSH;
               sprite[i].hitag = adjusthp(100);
               newstatus((short) i, FACE);
               break;
          case GONZOSHJUMPEND:
               spr->picnum = GONZOGSH;
               sprite[i].hitag = adjusthp(100);
               newstatus((short) i, FACE);
               break;
          }
          i = nexti;
     }

     i = headspritestat[AMBUSH];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          switch (spr->extra) {
          case 1:                  // forward

               dax = spr->x;       // Back up old x&y if stepping off cliff
               day = spr->y;
               daz = spr->z;

               osectnum = spr->sectnum;
               spr->zvel += synctics << 3;

               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                     spr->zvel,
                                     4L << 8, 4L << 8, 0);

               spr->lotag -= synctics;

               if (globloz <= spr->z && spr->lotag < 0) {
                    spr->z = globloz;
                    changespritestat((short) i, LAND);
               }

               if ((movestat & 0xc000) == 49152) {     // Bullet hit a sprite
                    k = (movestat & 4095);
                    for (j = 0; j < 15; j++) {
                         shards(k, 1);
                    }
                    hitobject = damageactor(movestat, i);
               }


               break;
          case 2:                  // fall
               dax = spr->x;       // Back up old x&y if stepping off cliff
               day = spr->y;
               daz = spr->z;

               osectnum = spr->sectnum;
               spr->zvel += synctics << 4;

               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 1,
                                (((long) sintable[spr->ang]) * synctics) << 1,
                                     spr->zvel,
                                     4L << 8, 4L << 8, 0);

               spr->lotag -= synctics;

               if (globloz <= spr->z && spr->lotag < 0) {
                    spr->z = globloz;
                    changespritestat((short) i, LAND);
               }

               break;
          case 3:                  // jumpup

               dax = spr->x;
               day = spr->y;
               daz = spr->z;

               osectnum = spr->sectnum;
               spr->zvel -= synctics << 4;

               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                     spr->zvel,
                                     4L << 8, 4L << 8, 0);

               spr->lotag -= synctics;

               setsprite((short) i, spr->x, spr->y, spr->z);

               if (spr->lotag < 0) {
                    spr->extra = 2;
                    spr->lotag = 20;
               }

               break;
          }
          i = nexti;
     }

     i = headspritestat[SPARKSUP];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];

          osectnum = spr->sectnum;

          spr->lotag -= synctics;
          if (spr->lotag < -100)
               spr->lotag = -100;
          if (spr->lotag < 0) {
               daz = spr->zvel -= synctics << 4;
               spr->ang = (spr->ang + (synctics << 2)) & 2047;

               hitobject = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                      daz, 4L << 8, 4L << 8, 1);

               if (osectnum != spr->sectnum) {
                    spr->x = sparksx;
                    spr->y = sparksy;
                    spr->z = sparksz;
                    spr->ang = (rand() % 2047) & 2047;
                    spr->zvel = 0;
                    setsprite((short) i, spr->x, spr->y, spr->z);

               }
          }

          i = nexti;
     }

     i = headspritestat[SPARKSDN];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];

          osectnum = spr->sectnum;

          spr->lotag -= synctics;
          if (spr->lotag < -100)
               spr->lotag = -100;

          if (spr->lotag < 0) {

               daz = spr->zvel += synctics << 4;
               spr->ang = (spr->ang + (synctics << 2)) & 2047;

               hitobject = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                      daz, 4L << 8, 4L << 8, 1);

               if (osectnum != spr->sectnum) {
                    spr->x = sparksx;
                    spr->y = sparksy;
                    spr->z = sparksz;
                    spr->ang = (rand() % 2047) & 2047;
                    spr->zvel = 0;
                    setsprite((short) i, spr->x, spr->y, spr->z);

               }
          }


          i = nexti;
     }

     i = headspritestat[SPARKS];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];

          osectnum = spr->sectnum;
          spr->lotag -= synctics;
          if (spr->lotag < -100)
               spr->lotag = -100;

          if (spr->lotag < 0) {

               daz = 0;
               spr->ang = (spr->ang + (synctics << 2)) & 2047;

               hitobject = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                      daz, 4L << 8, 4L << 8, 1);

               if (osectnum != spr->sectnum) {
                    spr->x = sparksx;
                    spr->y = sparksy;
                    spr->z = sparksz;
                    spr->ang = (rand() % 2047) & 2047;
                    spr->zvel = 0;
                    setsprite((short) i, spr->x, spr->y, spr->z);

               }
          }

          i = nexti;
     }

     i = headspritestat[STONETOFLESH];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               switch (spr->picnum) {
               case STONEGONZOCHM:
                    spr->picnum = GONZOGHM;
                    sprite[i].xrepeat = 24;
                    sprite[i].yrepeat = 24;
                    sprite[i].clipdist = 32;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(100);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 0;
                    break;
               case STONEGONZOBSH:
                    spr->picnum = GONZOGSH;
                    sprite[i].xrepeat = 24;
                    sprite[i].yrepeat = 24;
                    sprite[i].clipdist = 32;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(100);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 0;
                    break;
               case STONEGONZOGSH:
                    spr->picnum = GONZOGSH;
                    sprite[i].xrepeat = 24;
                    sprite[i].yrepeat = 24;
                    sprite[i].clipdist = 32;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(100);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 0;
                    break;
               case STONEGRONDOVAL:
                    spr->picnum = GRONHAL;
                    sprite[i].xrepeat = 30;
                    sprite[i].yrepeat = 30;
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(300);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 4;
                    break;
               case STONEGONZOBSW2:
               case STONEGONZOBSW:
                    spr->picnum = NEWGUY;
                    sprite[i].xrepeat = 26;
                    sprite[i].yrepeat = 26;
                    sprite[i].clipdist = 48;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(100);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 30;
                    break;
               }
          }
          i = nexti;
     }

     i = headspritestat[SHADE];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               spr->picnum = GONZOBSHDEAD;
               sprite[i].shade = 31;
               sprite[i].cstat = 0x303;
               sprite[i].pal = 0;
               spr->extra = 12;
               newstatus((short) i, EVILSPIRIT);
          }
          i = nexti;
     }

     i = headspritestat[EVILSPIRIT];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          if (spr->picnum >= (GONZOBSHDEAD - 8))
               spr->picnum--;
          spr->extra--;
          if (spr->picnum < (GONZOBSHDEAD - 8)) {
               j = insertsprite(spr->sectnum, FACE);
               sprite[j].x = spr->x;
               sprite[j].y = spr->y;
               sprite[j].z = spr->z;
               sprite[j].cstat = 0x303;
               sprite[j].picnum = GONZOGSH;
               sprite[j].shade = 31;
               sprite[j].pal = 0;
               sprite[j].xrepeat = spr->xrepeat;
               sprite[j].yrepeat = spr->yrepeat;
               sprite[j].owner = 0;
               sprite[j].lotag = 40;
               sprite[j].hitag = 0;
          }
          if (spr->extra < 0)
               deletesprite((short) i);
          i = nexti;
     }


     i = headspritestat[PATROL];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          dax = spr->x;            // Back up old x&y if stepping off cliff
          day = spr->y;
          daz = spr->z;
          osectnum = spr->sectnum;
          movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                0L, 4L << 8, 4L << 8, 0);
          if (globloz > spr->z + (48 << 8)) {
               setsprite((short) i, spr->x, spr->y, spr->z);
               movestat = 1;
          }
          else {
               spr->z = globloz;
          }
          j = headspritesect[spr->sectnum];
          while (j != -1) {
               nextj = nextspritesect[j];
               tspr = &sprite[j];
               if (tspr->picnum == PATROLPOINT) {
                    dx = labs(spr->x - tspr->x);  // x distance to sprite
                    dy = labs(spr->y - tspr->y);  // y distance to sprite
                    dz = labs((spr->z >> 8) - (tspr->z >> 8));   // z distance to sprite
                    dh = tilesizy[tspr->picnum] >> 4;  // height of sprite
                    if (dx + dy < PICKDISTANCE && dz - dh <= PICKHEIGHT) {
                         spr->ang = tspr->ang;
                         // checkhit((short)i,(short)j):
                    }
               }
               j = nextj;
          }
          if (sintable[(spr->ang + 2560) & 2047] * (plr->x - spr->x) + sintable[(spr->ang + 2048) & 2047] * (plr->y - spr->y) >= 0) {
               if (cansee(plr->x, plr->y, plr->z, plr->sector, spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1) {
                    newstatus((short) i, CHASE);
               }
          }
          else if (movestat != 0) {
               if ((movestat & 0xc000) == 32768) {     // hit a wall
                    actoruse(i);
               }
               newstatus((short) i, FINDME);
          }

          i = nexti;
     }

     i = headspritestat[PULLTHECHAIN];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               spr->picnum++;
               spr->lotag = 24;
               if (spr->picnum == PULLCHAIN3
                   || spr->picnum == SKULLPULLCHAIN3) {
                    spr->lotag = 0;
                    changespritestat(i, 0);
               }
          }
          i = nexti;
     }

     i = headspritestat[ANIMLEVERDN];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               spr->picnum++;
               spr->lotag = 24;
               if (spr->picnum == LEVERDOWN) {
                    spr->lotag = 60;
                    changespritestat(i, 0);
               }
          }
          i = nexti;
     }

     i = headspritestat[ANIMLEVERUP];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               spr->picnum--;
               spr->lotag = 24;
               if (spr->picnum == LEVERUP) {
                    spr->lotag = 1;
                    changespritestat(i, 0);
               }
          }
          i = nexti;
     }

     i = headspritestat[WARPFX];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               spr->lotag = 12;
               spr->picnum++;
               if (spr->picnum >= ANNIHILATE + 5)
                    deletesprite((short) i);
          }
          i = nexti;
     }

     i = headspritestat[NUKED];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          if (spr->picnum == ZFIRE) {
               spr->lotag-=synctics;
               if (spr->lotag <= 0) {
                    deletesprite((short) i);
               }
          }
          else {
               chunksofmeat(plr, i, spr->x, spr->y, spr->z, spr->sectnum,
                            spr->ang);
               trailingsmoke(i, 2);
               newstatus((short) i, DIE);
          }
          i = nexti;
     }
     i = headspritestat[FROZEN];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               spr->pal = 0;
               newstatus((short) i, FACE);
               switch (spr->picnum) {

               case GONZOCSWPAIN:
                    spr->picnum = GONZOCSW;
                    break;
               case GONZOGSWPAIN:
                    spr->picnum = GONZOGSW;
                    break;
               case GONZOGHMPAIN:
                    spr->picnum = GONZOGHM;
                    break;
               case GONZOGSHPAIN:
                    spr->picnum = GONZOGSH;
                    break;

               case GRONHALDIE:
                    spr->picnum = GRONHAL;
                    break;
               case GRONMUDIE:
                    spr->picnum = GRONMU;
                    break;
               case GRONSWDIE:
                    spr->picnum = GRONSW;
                    break;
               case KOBOLDDIE:
                    spr->picnum = KOBOLD;
                    break;
               case DEVILDIE:
                    spr->picnum = DEVIL;
                    break;
               case SKELETONDIE:
                    spr->picnum = SKELETON;
                    break;
               case IMPDIE:
                    spr->picnum = IMP;
                    break;
               case MINOTAURDIE:
                    spr->picnum = MINOTAUR;
                    break;
               }
          }
          i = nexti;
     }



     // PAIN
     i = headspritestat[PAIN];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;

          if (spr->lotag < 0) {
               switch (spr->picnum) {
               case DEMON - 1:
                    spr->picnum = DEMON;
                    break;

               case GONZOCSWPAIN:
                    spr->picnum = GONZOCSW;
                    break;
               case GONZOGSWPAIN:
                    spr->picnum = GONZOGSW;
                    break;
               case GONZOGHMPAIN:
                    spr->picnum = GONZOGHM;
                    break;
               case GONZOGSHPAIN:
                    spr->picnum = GONZOGSH;
                    break;

               case NEWGUYPAIN:
                    spr->picnum = NEWGUY;
                    break;

               case GRONHALPAIN:
                    spr->picnum = GRONHAL;
                    break;
               case GRONSWPAIN:
                    spr->picnum = GRONSW;
                    break;
               case GRONMUPAIN:
                    spr->picnum = GRONMU;
                    break;
               case KOBOLDDIE:
                    spr->picnum = KOBOLD;
                    break;
               case KATIEPAIN:
                    spr->picnum = KATIE;
                    break;
               case DEVILPAIN:
                    spr->picnum = DEVIL;
                    break;
               case IMPPAIN:
                    spr->picnum = IMP;
                    break;
               case MINOTAURPAIN:
                    spr->picnum = MINOTAUR;
                    break;
               }
               spr->ang = (short) plr->ang;
               newstatus((short) i, FLEE);
               break;
          }
          else {
               dax = spr->x;       // Back up old x&y if stepping off cliff
               day = spr->y;
               daz = spr->z;

               osectnum = spr->sectnum;
               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                     0L, 4L << 8, 4L << 8, 0);

               if (globloz > spr->z + (48 << 8)) {
                    spr->x = dax;
                    spr->y = day;
                    spr->z = daz;
                    setsprite((short) i, spr->x, spr->y, spr->z);
                    movestat = 1;
               }
               else {
                    spr->z = globloz;
               }
          }

          j = headspritesect[spr->sectnum];
          while (j != -1) {
               nextj = nextspritesect[j];
               tspr = &sprite[j];
               dx = labs(spr->x - tspr->x);  // x distance to sprite
               dy = labs(spr->y - tspr->y);  // y distance to sprite
               dz = labs((spr->z >> 8) - (tspr->z >> 8));   // z distance to sprite
               dh = tilesizy[tspr->picnum] >> 1;  // height of sprite
               if (dx + dy < PICKDISTANCE && dz - dh <= PICKHEIGHT) {
                    switch (spr->picnum) {
                    case SKELETON:
                    case KOBOLDDIE:
                         // case FREDPAIN:
                         switch (tspr->picnum) {
                         case EXPLO2:
                         case SMOKEFX:
                         case MONSTERBALL:
                              spr->hitag -= synctics << 2;
                              if (spr->hitag < 0) {
                                   newstatus((short) i, DIE);
                              }
                              break;
                         }
                         break;
                    case GONZOBSHPAIN:
                    case GONZOCSWPAIN:
                    case GONZOGHMPAIN:
                    case GONZOGSHPAIN:

                    case GRONHALPAIN:
                    case GRONSWPAIN:
                    case GRONMUPAIN:
                         switch (tspr->picnum) {
                         case EXPLO2:
                         case MONSTERBALL:
                              spr->hitag -= synctics << 2;
                              if (spr->hitag < 0) {
                                   newstatus((short) i, DIE);
                              }
                              break;
                         }
                         break;
                    }
               }
               j = nextj;
          }
          i = nexti;
     }

     // FLOCKSPAWN
     i = headspritestat[FLOCKSPAWN];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               spr->extra--;
               spr->lotag = krand() & 48 + 24;
               bats(i);
               if (spr->extra == 0)
                    changespritestat(i, 0);
          }
          i = nexti;
     }

     // FLOCK
     i = headspritestat[FLOCK];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          switch (spr->extra) {
          case 0:                  // going out of the cave
               if (spr->lotag < 0) {
                    spr->extra = 1;
                    spr->lotag = 512;
               }
               else {
                    movestat = movesprite((short) i, (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3, (((long) sintable[spr->ang]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 0);

                    if (movestat != 0)
                         spr->ang = rand() & 2047;
               }
               break;
          case 1:                  // flying in circles
               if (spr->lotag < 0) {
                    spr->extra = 2;
                    spr->lotag = 512;
                    spr->ang = ((getangle(sprite[spr->hitag].x - spr->x, sprite[spr->hitag].y - spr->y) & 2047) - 1024) & 2047;
               }
               else {
                    spr->z -= synctics << 4;
                    spr->ang = (spr->ang + (synctics << 2)) & 2047;
                    movestat = movesprite((short) i, (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3, (((long) sintable[spr->ang]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 0);

                    if (movestat != 0)
                         spr->ang = rand() & 2047;
               }
               break;
          case 2:                  // fly to roof and get deleted
               if (spr->lotag < 0) {
                    if (i == lastbat && batsnd != -1) {
                         SND_StopLoop(batsnd);
                         batsnd = -1;
                    }
                    deletesprite((short) i);
               }
               else {
                    movestat = movesprite((short) i, (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3, (((long) sintable[spr->ang]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 0);

                    if ((movestat & 0xc000) == 16384) {     // Hits a ceiling /
                                                            // floor
                         if (i == lastbat && batsnd != -1) {
                              SND_StopLoop(batsnd);
                              batsnd = -1;
                         }
                         deletesprite((short) i);
                    }
                    if (movestat != 0)
                         spr->ang = (rand() & 2047);
               }
               break;
          }
          i = nexti;
     }


     // SKIRMISH
     i = headspritestat[SKIRMISH];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;

          if (spr->lotag < 0) {
               newstatus((short) i, FACE);
          }

          dax = spr->x;            // Back up old x&y if stepping off cliff
          day = spr->y;
          daz = spr->z;

          osectnum = spr->sectnum;
          movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                0L, 4L << 8, 4L << 8, 0);

          if (globloz > spr->z + (48 << 8)) {
               spr->x = dax;
               spr->y = day;
               spr->z = daz;
               setsprite((short) i, spr->x, spr->y, spr->z);
               movestat = 1;
          }
          else {
               spr->z = globloz;
          }

          if (movestat != 0) {
               // spr->ang=(getangle(plr->x-spr->x,plr->y-spr->y)&2047);
               // newstatus((short)i,FACE);

               j = headspritestat[APATROLPOINT];
               while (j != -1) {
                    nextj = nextspritestat[j];
                    tspr = &sprite[j];
                    dist = klabs(tspr->x - spr->x) + klabs(tspr->y - spr->y);
                    if (dist < mindist) {
                         mindist = dist;
                         target = j;
                    }
                    j = nextj;
               }
               tspr = &sprite[target];
               if (cansee(tspr->x,
                          tspr->y,
                          tspr->z,
                          tspr->sectnum,
                          spr->x,
                          spr->y,
                          spr->z - (tilesizy[spr->picnum] << 7),
                          spr->sectnum) == 1) {
                    spr->ang = (getangle(tspr->x - spr->x, tspr->y - spr->y) & 2047);
                    // newstatus((short)i,PATROL);
               }

          }
          if ((spr->sectnum != osectnum) && (sector[spr->sectnum].lotag == 10)) {
               warpsprite((short) i);
          }
          switch (sector[spr->sectnum].floorpicnum) {
          case FLOORMIRROR:
          case WATER:
          case SLIME:
               if (spr->picnum == WILLOW
                   || spr->picnum == DEMON
                   || spr->picnum == GUARDIAN
                   || (spr->picnum >= GUARDIANATTACK
                       && spr->picnum <= GUARDIANATTACK + 6)) {
                    spr->z = sector[spr->sectnum].floorz - (16 << 8);
               }
               else {
                    spr->z = sector[spr->sectnum].floorz + (16 << 8);
                    if (rand() % 100 > 60) {
                         makemonstersplash(SPLASHAROO, i);
                    }
               }
               break;
          case LAVA:
               trailingsmoke(i, 1);
               makemonstersplash(LAVASPLASH, i);
               break;
          }
          if (sector[spr->sectnum].floorz - (32 << 8) < sector[spr->sectnum].ceilingz) {
               if (sector[spr->sectnum].lotag == 6) {
                    newstatus((short) i, DIE);
               }
               else {
                    deletesprite((short) i);
               }
          }
          j = headspritesect[spr->sectnum];
          while (j != -1) {
               nextj = nextspritesect[j];
               tspr = &sprite[j];
               dx = labs(spr->x - tspr->x);  // x distance to sprite
               dy = labs(spr->y - tspr->y);  // y distance to sprite
               dz = labs((spr->z >> 8) - (tspr->z >> 8));   // z distance to sprite
               dh = tilesizy[tspr->picnum] >> 1;  // height of sprite
               if (dx + dy < PICKDISTANCE && dz - dh <= PICKHEIGHT) {
                    switch (spr->picnum) {
                    case SKELETON:
                    case KOBOLD:


                         switch (tspr->picnum) {
                         case EXPLO2:
                         case SMOKEFX:
                         case MONSTERBALL:
                              spr->hitag -= synctics << 2;
                              if (spr->hitag < 0) {
                                   newstatus((short) i, DIE);
                              }
                              break;
                         }
                         break;
                    case GONZOCSW:
                    case GONZOGSW:
                    case GONZOGHM:
                    case GONZOGSH:
                    case GRONHAL:
                    case GRONMU:
                    case GRONSW:
                         switch (tspr->picnum) {
                         case EXPLO2:
                         case MONSTERBALL:
                              spr->hitag -= synctics << 2;
                              if (spr->hitag < 0) {
                                   newstatus((short) i, DIE);
                              }
                              break;
                         }
                         break;
                    }
               }
               j = nextj;
          }

          i = nexti;
     }

     // FINDME
     i = headspritestat[FINDME];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->picnum == RAT) {
               spr->ang = ((rand() & 512 - 256) + spr->ang + 1024) & 2047; // NEW
               newstatus((short) i, FLEE);
               goto findout;
          }
          if (invisibletime > 0) {
               newstatus((short) i, FACE);
               break;
          }
          monsternoise((short) i);

          dax = spr->x;            // Back up old x&y if stepping off cliff
          day = spr->y;
          daz = spr->z;

          osectnum = spr->sectnum;
          movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                0L, 4L << 8, 4L << 8, 0);

          if (globloz > spr->z + (48 << 8)) {
               spr->x = dax;
               spr->y = day;
               spr->z = daz;
               setsprite((short) i, spr->x, spr->y, spr->z);
               movestat = 1;
          }
          else {
               spr->z = globloz;
          }

          if (checkdist((short) i, plr->x, plr->y, plr->z)) {
               if (shadowtime > 0) {
                    spr->ang = ((rand() & 512 - 256) + spr->ang + 1024) & 2047;
                    newstatus((short) i, FLEE);
               }
               else {
                    newstatus((short) i, ATTACK);
               }
               break;
          }
          if (movestat != 0) {
               if ((movestat & 0xc000) == 32768) {     // hit a wall
                    actoruse(i);
               }
               if (cansee(plr->x, plr->y, plr->z, plr->sector, spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1 && spr->lotag < 0) {
                    spr->ang = (spr->ang + 1024) & 2047;
                    newstatus((short) i, FLEE);
                    goto findout;
               }

               if (spr->lotag < 0) {
                    if (rand() % 100 > 50) {
                         spr->ang = (spr->ang + ((rand() % 5) * 512)) & 2047;
                    }
                    else {
                         spr->ang = (spr->ang + 1024) & 2047;
                    }

                    spr->lotag = 30;
               }
               else {
                    spr->ang += (synctics << 4) & 2047;
               }

          }
          if (cansee(plr->x, plr->y, plr->z, plr->sector, spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1 && movestat == 0 && spr->lotag < 0) {
               newstatus((short) i, FACE);
               break;
          }
          if ((spr->sectnum != osectnum) && (sector[spr->sectnum].lotag == 10)) {
               warpsprite((short) i);
          }

          switch (sector[spr->sectnum].floorpicnum) {
          case FLOORMIRROR:
          case WATER:
          case SLIME:
               if (spr->picnum == WILLOW
                   || spr->picnum == DEMON
                   || spr->picnum == GUARDIAN
                   || (spr->picnum >= GUARDIANATTACK
                       && spr->picnum <= GUARDIANATTACK + 6)) {
                    spr->z = sector[spr->sectnum].floorz - (16 << 8);
               }
               else {
                    spr->z = sector[spr->sectnum].floorz + (16 << 8);
                    if (rand() % 100 > 60) {
                         makemonstersplash(SPLASHAROO, i);
                    }
               }
               break;
          case LAVA:
               trailingsmoke(i, 1);
               makemonstersplash(LAVASPLASH, i);
               break;
          }
          if (sector[spr->sectnum].floorz - (32 << 8) < sector[spr->sectnum].ceilingz) {
               if (sector[spr->sectnum].lotag == 6) {
                    newstatus((short) i, DIE);
               }
               else {
                    deletesprite((short) i);
               }
          }

          j = headspritesect[spr->sectnum];
          while (j != -1) {
               nextj = nextspritesect[j];
               tspr = &sprite[j];
               if (isvalidactor((short) i)) {
                    dx = labs(spr->x - tspr->x);  // x distance to sprite
                    dy = labs(spr->y - tspr->y);  // y distance to sprite
                    dz = labs((spr->z >> 8) - (tspr->z >> 8));   // z distance to sprite
                    dh = tilesizy[tspr->picnum] >> 1;  // height of sprite
                    if (dx + dy < PICKDISTANCE && dz - dh <= PICKHEIGHT) {
                         switch (spr->picnum) {
                         case SKELETON:
                         case KOBOLD:


                              switch (tspr->picnum) {
                              case EXPLO2:
                              case SMOKEFX:
                              case MONSTERBALL:
                                   spr->hitag -= synctics << 2;
                                   if (spr->hitag < 0) {
                                        newstatus((short) i, DIE);
                                   }
                                   break;
                              }
                              break;
                         case GONZOCSW:
                         case GONZOGSW:
                         case GONZOGHM:
                         case GONZOGSH:
                         case GRONHAL:
                         case GRONMU:
                         case GRONSW:
                              switch (sprite[j].picnum) {
                              case EXPLO2:
                              case MONSTERBALL:
                                   spr->hitag -= synctics << 2;
                                   if (spr->hitag < 0) {
                                        newstatus((short) i, DIE);
                                   }
                                   break;
                              }
                              break;
                         }
                    }
               }
               j = nextj;
          }
findout:
          i = nexti;
     }

     // TORCHFRONT
     i = headspritestat[TORCHFRONT];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          playertorch = spr->lotag -= synctics;
          if (selectedgun > 4) {
               playertorch = spr->lotag = -1;
          }
          setsprite((short) i, plr->x, plr->y, plr->z);
          osectnum = spr->sectnum;
          j = (torchpattern[lockclock % 38]);
          sector[osectnum].ceilingshade = (sector[osectnum].ceilingshade + j) >> 1;
          sector[osectnum].floorshade = (sector[osectnum].floorshade + j) >> 1;
          startwall = sector[osectnum].wallptr;
          endwall = startwall + sector[osectnum].wallnum - 1;
          for (k = startwall; k <= endwall; k++) {
               wall[k].shade = (wall[k].shade + j) >> 1;
          }
          movestat = movesprite((short) i,
                (((long) sintable[(plr->ang + 512) & 2047]) << synctics) << 8,
                                (((long) sintable[plr->ang]) << synctics) << 8,
                                0L,
                                4L << 8,
                                4L << 8,
                                0);

          spr->cstat |= 0x8000;
          show2dsprite[i >> 3] &= ~(1 << (i & 7));

          if (osectnum != spr->sectnum) {
               // sector[osectnum].ceilingshade=ceilingshadearray[osectnum];
               // sector[osectnum].floorshade=floorshadearray[osectnum];
               // startwall=sector[osectnum].wallptr;
               // endwall=startwall+sector[osectnum].wallnum-1;
               // for(k=startwall;k<=endwall;k++) {
               // wall[k].shade=wallshadearray[k];
               // }
               sector[osectnum].ceilingshade = j;;
               sector[osectnum].floorshade = j;
               startwall = sector[osectnum].wallptr;
               endwall = startwall + sector[osectnum].wallnum - 1;
               for (k = startwall; k <= endwall; k++) {
                    wall[k].shade = j;
               }
          }
          if (spr->lotag < 0) {
               deletesprite((short) i);
               // set back to normall
               sector[plr->sector].ceilingshade = ceilingshadearray[plr->sector];
               sector[plr->sector].floorshade = floorshadearray[plr->sector];
               startwall = sector[plr->sector].wallptr;
               endwall = startwall + sector[plr->sector].wallnum - 1;
               for (k = startwall; k <= endwall; k++) {
                    wall[k].shade = wallshadearray[k];
               }
               sector[plr->oldsector].ceilingshade = ceilingshadearray[plr->oldsector];
               sector[plr->oldsector].floorshade = floorshadearray[plr->oldsector];
               startwall = sector[plr->oldsector].wallptr;
               endwall = startwall + sector[plr->oldsector].wallnum - 1;
               for (k = startwall; k <= endwall; k++) {
                    wall[k].shade = wallshadearray[k];
               }
               sector[osectnum].ceilingshade = ceilingshadearray[osectnum];
               sector[osectnum].floorshade = floorshadearray[osectnum];
               startwall = sector[osectnum].wallptr;
               endwall = startwall + sector[osectnum].wallnum - 1;
               for (k = startwall; k <= endwall; k++) {
                    wall[k].shade = wallshadearray[k];
               }
               // osectnum=spr->sectnum;
               osectnum = plr->oldsector;
               sector[osectnum].ceilingshade = ceilingshadearray[osectnum];
               sector[osectnum].floorshade = floorshadearray[osectnum];
               startwall = sector[osectnum].wallptr;
               endwall = startwall + sector[osectnum].wallnum - 1;
               for (k = startwall; k <= endwall; k++) {
                    wall[k].shade = wallshadearray[k];
               }

          }
          i = nexti;
     }

     // TORCHLIGHT
     i = headspritestat[TORCHLIGHT];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          osectnum = spr->sectnum;
          j = (torchpattern[lockclock % 38]);
          sector[osectnum].ceilingshade = j;
          sector[osectnum].floorshade = j;
          startwall = sector[osectnum].wallptr;
          endwall = startwall + sector[osectnum].wallnum - 1;
          for (k = startwall; k <= endwall; k++)
               wall[k].shade = j;
          i = nexti;
     }

     // GLOWLIGHT
     i = headspritestat[GLOWLIGHT];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          osectnum = spr->sectnum;
          j = (torchpattern[lockclock % 38]);
          sector[osectnum].floorshade = j;
          startwall = sector[osectnum].wallptr;
          endwall = startwall + sector[osectnum].wallnum - 1;
          for (k = startwall; k <= endwall; k++)
               wall[k].shade = j;
          startredflash(j);
          i = nexti;
     }

     // BOB
     i = headspritestat[BOB];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->z += (sintable[(lockclock << 4) & 2047] >> 6);
          i = nexti;
     }

     // RESURECT
     i = headspritestat[RESURECT];

     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];

          spr->lotag -= synctics;

          if (spr->lotag <= 0) {
               newstatus((short) i, FACE);
               switch (spr->picnum) {
               case GONZOCSWDEAD:
                    spr->picnum = GONZOCSW;
                    spr->hitag = adjusthp(50);
                    break;
               case GONZOGSWDEAD:
                    spr->picnum = GONZOGSW;
                    spr->hitag = adjusthp(100);
                    break;
               case GONZOGHMDEAD:
                    spr->picnum = GONZOGHM;
                    spr->hitag = adjusthp(40);
                    break;
               case GONZOGSHDEAD:
                    spr->picnum = GONZOGSH;
                    spr->hitag = adjusthp(50);
                    break;
               case NEWGUYDEAD:
                    j = krand() % 3;
                    switch (j) {
                    case 0:
                         sprite[i].extra = 30;
                         sprite[i].hitag = adjusthp(85);
                         break;
                    case 1:
                         sprite[i].extra = 20;
                         sprite[i].hitag = adjusthp(85);
                         break;
                    case 2:
                         sprite[i].extra = 10;
                         sprite[i].hitag = adjusthp(45);
                         break;
                    case 3:
                         sprite[i].extra = 0;
                         sprite[i].hitag = adjusthp(15);
                         break;
                    }
                    sprite[i].xrepeat = NEWGUYXR;
                    sprite[i].yrepeat = NEWGUYYR;
                    sprite[i].picnum = NEWGUY;
                    break;
               case GRONDEAD:
                    j = krand() % 3;
                    switch (j) {
                    case 0:
                         spr->picnum = GRONHAL;
                         spr->hitag = adjusthp(65);
                         spr->extra = 3;
                         break;
                    case 1:
                         spr->picnum = GRONSW;
                         spr->hitag = adjusthp(60);
                         spr->extra = 0;
                         break;
                    case 2:
                         spr->picnum = GRONMU;
                         spr->hitag = adjusthp(70);
                         spr->extra = 2;
                         break;
                    }
                    break;
               case KOBOLDDEAD:
                    spr->picnum = KOBOLD;
                    spr->hitag = adjusthp(60);
                    break;
               case KATIEDEAD:
                    spr->picnum = KATIE;
                    spr->hitag = adjusthp(200);
                    break;
               case DEVILDEAD:
                    spr->picnum = DEVIL;
                    spr->hitag = adjusthp(60);
                    break;
               case SKELETONDEAD:
                    spr->picnum = SKELETON;
                    spr->hitag = adjusthp(25);
                    break;
               case IMPDEAD:
                    spr->picnum = IMP;
                    spr->hitag = adjusthp(20);
                    break;
               case MINOTAURDEAD:
                    spr->picnum = MINOTAUR;
                    spr->hitag = adjusthp(80);
                    break;
               }
               spr->lotag = 100;
               spr->cstat |= 1;
          }
          i = nexti;
     }


     // LIFT UP
     i = headspritestat[LIFTUP];

     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          switch (spr->lotag) {
          case 1821:
               spr->z -= (synctics << 6);
               setsprite((short) i, spr->x, spr->y, spr->z);
               if (spr->z <= sector[spr->sectnum].ceilingz + 32768) {

                    SND_StopLoop(cartsnd);
                    cartsnd = -1;
                    playsound_loc(S_CLUNK, spr->x, spr->y);

                    changespritestat(i, 0);
                    spr->lotag = 1820;
                    spr->z = sector[spr->sectnum].ceilingz + 32768;
               }
               break;
          case 1811:
               spr->z -= (synctics << 6);
               setsprite((short) i, spr->x, spr->y, spr->z);
               if (spr->z <= sector[spr->sectnum].ceilingz + 65536) {
                    changespritestat(i, 0);
                    spr->lotag = 1810;
                    spr->z = sector[spr->sectnum].ceilingz + 65536;
               }
               break;
          case 1801:
               spr->z -= (synctics << 6);
               setsprite((short) i, spr->x, spr->y, spr->z);
               if (spr->z <= sector[spr->sectnum].ceilingz + 65536) {
                    changespritestat(i, 0);
                    spr->lotag = 1800;
                    spr->z = sector[spr->sectnum].ceilingz + 65536;
               }
               break;
          }
          i = nexti;
     }

     // LIFT DN
     i = headspritestat[LIFTDN];

     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];

          ironbarmove = 0;
          switch (spr->lotag) {
          case 1820:
               spr->z += ironbarmove = synctics << 6;
               setsprite((short) i, spr->x, spr->y, spr->z);
               if (spr->z >= (sector[spr->sectnum].floorz - 32768)) {
                    SND_StopLoop(cartsnd);
                    cartsnd = -1;
                    playsound_loc(S_CLUNK, spr->x, spr->y);
                    changespritestat(i, 0);
                    spr->lotag = 1821;
                    spr->z = sector[spr->sectnum].floorz - 32768;
               }
               break;
          case 1810:
               spr->z += ironbarmove = synctics << 6;
               setsprite((short) i, spr->x, spr->y, spr->z);
               if (spr->z >= sector[spr->sectnum].floorz) {
                    changespritestat(i, 0);
                    spr->lotag = 1811;
                    spr->z = sector[spr->sectnum].floorz;
               }
               break;
          case 1800:
               spr->z += ironbarmove = synctics << 6;
               setsprite((short) i, spr->x, spr->y, spr->z);
               if (spr->z >= sector[spr->sectnum].floorz) {
                    changespritestat(i, 0);
                    spr->lotag = 1801;
                    spr->z = sector[spr->sectnum].floorz;
               }
               break;
          }
          i = nexti;
     }

     // WITCHSIT

     i = headspritestat[WITCHSIT];

     while (i >= 0 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->ang = (getangle(plr->x - spr->x, plr->y - spr->y) & 2047);
          if (cansee(plr->x, plr->y, plr->z, plr->sector, spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1) {
               spr->lotag -= synctics;
               if (spr->lotag < 0) {
                    spr->picnum++;
                    spr->lotag = 12;
               }
          }
          i = nexti;
     }


     // FACE
     i = headspritestat[FACE];
     while (i >= 0 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->ang = (getangle(plr->x - spr->x, plr->y - spr->y) & 2047);
          if (cansee(plr->x, plr->y, plr->z, plr->sector,
                     spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1
              && invisibletime < 0) {
               if (shadowtime > 0) {
                    spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                    newstatus((short) i, FLEE);
               }
               else {
                    spr->owner = plr->spritenum;
                    if (spr->picnum == RAT) {
                         spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                         newstatus((short) i, FLEE);
                    }
                    else {
                         newstatus((short) i, CHASE);
                    }
               }
          }
          else {
               if (spr->owner == plr->spritenum) {
                    spr->ang = ((krand() & 512 - 256) + spr->ang) & 2047;
                    newstatus((short) i, FINDME);
               }
          }
          if (checkdist((short) i, plr->x, plr->y, plr->z)) {
               if (spr->picnum == RAT) {
                    spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                    newstatus((short) i, FLEE);
               }
               else {
                    newstatus((short) i, ATTACK);
               }
          }
          j = headspritesect[spr->sectnum];
          while (j != -1) {
               nextj = nextspritesect[j];
               tspr = &sprite[j];
               if (isvalidactor((short) i)) {
                    dx = labs(spr->x - tspr->x);  // x distance to sprite
                    dy = labs(spr->y - tspr->y);  // y distance to sprite
                    dz = labs((spr->z >> 8) - (tspr->z >> 8));   // z distance to sprite
                    dh = tilesizy[tspr->picnum] >> 1;  // height of sprite
                    if (dx + dy < PICKDISTANCE && dz - dh <= PICKHEIGHT) {
                         switch (spr->picnum) {
                         case SKELETON:
                         case KOBOLD:


                              switch (tspr->picnum) {
                              case EXPLO2:
                              case SMOKEFX:
                              case MONSTERBALL:
                                   spr->hitag -= synctics << 2;
                                   if (spr->hitag < 0) {
                                        newstatus((short) i, DIE);
                                   }
                                   break;
                              }
                              break;
                         case GONZOCSW:
                         case GONZOGSW:
                         case GONZOGHM:
                         case GONZOGSH:

                         case GRONHAL:
                         case GRONMU:
                         case GRONSW:
                              switch (tspr->picnum) {
                              case EXPLO2:
                              case MONSTERBALL:
                                   spr->hitag -= synctics << 2;
                                   if (spr->hitag < 0) {
                                        newstatus((short) i, DIE);
                                   }
                                   break;
                              }
                              break;
                         }
                    }
               }
               j = nextj;
          }
          i = nexti;
     }

     // MASPLASH

     i = headspritestat[MASPLASH];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          spr->z = sector[spr->sectnum].floorz + (tilesizy[spr->picnum] << 8);

          if (spr->lotag <= 0) {
               if ((spr->picnum >= SPLASHAROO && spr->picnum < LASTSPLASHAROO)
                   || (spr->picnum >= LAVASPLASH
                       && spr->picnum < LASTLAVASPLASH)
                   || (spr->picnum >= SLIMESPLASH
                       && spr->picnum < LASTSLIMESPLASH)) {
                    spr->picnum++;
                    spr->lotag = 8;
               }
               else {
                    deletesprite((short) i);
               }
          }
          i = nexti;
          j++;
     }

     // ATTACK2
     i = headspritestat[ATTACK2];
     while (i >= 0 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               if (cansee(plr->x, plr->y, plr->z, plr->sector,
                          spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1
                   && invisibletime < 0) {
                    newstatus((short) i, CAST);
               }
               else {
                    newstatus((short) i, CHASE);
               }
               break;
          }
          else {
               spr->ang = (getangle(plr->x - spr->x, plr->y - spr->y) & 2047);
          }
          if (sector[spr->sectnum].floorz - (32 << 8) < sector[spr->sectnum].ceilingz) {
               if (sector[spr->sectnum].lotag == 6) {
                    newstatus((short) i, DIE);
               }
               else {
                    deletesprite((short) i);
               }
          }
          i = nexti;
     }

     // ATTACK SEQUENCE

     i = headspritestat[ATTACK];
     while (i >= 0 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          if (netgame) {
               i = nexti;
               continue;
          }

          if (attacktheme == 0) {
               startsong((rand() % 2) + 2);
               attacktheme = 1;
          }

          switch (spr->picnum) {
               // WANGO
          case KURTREADY:
               spr->lotag -= synctics;
               if (spr->lotag < 0) {
                    spr->picnum++;
                    spr->lotag = 24;
               }
               break;
          case KURTREADY + 1:
               spr->lotag -= synctics;
               if (spr->lotag < 0) {
                    spr->picnum = KURTAT;
                    spr->lotag = 64;
               }
               break;

          case NEWGUYCAST:
          case NEWGUYBOW:

          case GONZOCSWAT:

          case GRONHALATTACK:
          case GRONMUATTACK:
          case KATIEAT:
          case DEVILATTACK:
          case DEMON:
          case GUARDIANATTACK:
               spr->lotag -= synctics;
               if (spr->picnum == GUARDIANATTACK) {
                    if (plr->z < spr->z) {
                         spr->z -= synctics << 8;
                    }
                    if (plr->z > spr->z) {
                         spr->z += synctics << 8;
                    }
               }
               else if (spr->picnum == DEMON) {
                    if (plr->z < spr->z) {
                         spr->z -= synctics << 8;
                    }
                    if (plr->z > spr->z) {
                         spr->z += synctics << 8;
                    }
               }

               if (spr->lotag < 0) {
                    if (cansee(plr->x, plr->y, plr->z, plr->sector,
                               spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1) {
                         newstatus((short) i, CAST);
                    }
                    else {
                         newstatus((short) i, CHASE);
                    }
                    break;
               }
               else {
                    spr->ang = (getangle(plr->x - spr->x, plr->y - spr->y) & 2047);
               }
               break;
          case WILLOW:
               spr->lotag -= synctics;
               if (spr->picnum == WILLOW) {
                    if (plr->z < spr->z) {
                         spr->z -= synctics << 8;
                    }
                    if (plr->z > spr->z) {
                         spr->z += synctics << 8;
                    }
               }
               if (spr->lotag < 0) {
                    if (cansee(plr->x, plr->y, plr->z, plr->sector,
                               spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1)
                         if (checkdist((short) i, plr->x, plr->y, plr->z)) {
                              if (shockme < 0) {
                                   if (krand() & 100 > 95) {
                                        shockme = 120;
                                        strcpy(displaybuf, "Shocked");
                                        displaytime = 360;
                                        updatepics();
                                   }
                              }
                         }
                         else {
                              newstatus((short) i, DRAIN);
                         }
                    else {
                         newstatus((short) i, CHASE);
                    }
                    break;
               }
               else {
                    spr->ang = (getangle(plr->x - spr->x, plr->y - spr->y) & 2047);
               }
               break;

          case NEWGUYMACE:
          case NEWGUYPUNCH:

          case KURTAT:
          case KURTPUNCH:
          case KOBOLDATTACK:

          case SKELETONATTACK:
          case IMPATTACK:
          case MINOTAURATTACK:


               // case GONZOCSWAT:
          case GONZOGSWAT:
          case GONZOGHMAT:
          case GONZOGSHAT:

          case GRONSWATTACK:
               if (sector[spr->sectnum].floorpicnum == WATER
                   || sector[spr->sectnum].floorpicnum == SLIME
                   || sector[spr->sectnum].floorpicnum == FLOORMIRROR) {
                    spr->z = sector[spr->sectnum].floorz + (16 << 8);
               }
               if (spr->lotag >= 64) {
                    if (checksight((short) i, &daang)) {
                         if (checkdist((short) i, plr->x, plr->y, plr->z)) {
                              spr->ang = daang;
                              dax = 0L;
                              day = 0L;
                              attack((short) i);
                         }
                    }
               }
               else if (spr->lotag < 0) {
                    if (shadowtime > 0) {
                         spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;     // NEW
                         newstatus((short) i, FLEE);
                    }
                    else {
                         newstatus((short) i, CHASE);
                    }
               }
               spr->lotag -= synctics;
               break;
          }
          if (sector[spr->sectnum].floorz - (32 << 8) < sector[spr->sectnum].ceilingz) {
               if (sector[spr->sectnum].lotag == 6) {
                    newstatus((short) i, DIE);
               }
               else {
                    deletesprite((short) i);
               }
          }
          i = nexti;
     }

// SHATTER

     i = headspritestat[SHATTER];

     while (i != -1) {

          nexti = nextspritestat[i];
          spr = &sprite[i];

          spr->lotag -= synctics;

          if (spr->lotag < 0) {
               spr->picnum++;
               spr->lotag = 12;
          }
          switch (spr->picnum) {
          case FSHATTERBARREL + 2:
               changespritestat(i, 0);
               break;
          }

          i = nexti;
     }

// FIRE

     i = headspritestat[FIRE];

     while (i != -1) {

          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;

          if (spr->z < sector[spr->sectnum].floorz)
               spr->z += (long) synctics << 8;
          if (spr->z > sector[spr->sectnum].floorz)
               spr->z = sector[spr->sectnum].floorz;


          if (spr->lotag < 0) {
               switch (spr->picnum) {
                    /* case LFIRE: spr->picnum=SFIRE; spr->lotag=2047; break;
                     * case SFIRE: deletesprite((short)i); break; */
               }
          }
          if (checkdist((short) i, plr->x, plr->y, plr->z)) {
               healthpic(-1);
               flashflag = 1;
               startredflash(10);
          }

          i = nexti;
     }



// FALL

     i = headspritestat[FALL];

     while (i != -1) {

          nexti = nextspritestat[i];
          spr = &sprite[i];

          if (spr->z < sector[spr->sectnum].floorz)
               daz = spr->zvel += (synctics << 9);

          hitobject = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                 (((long) sintable[spr->ang]) * synctics) << 3,
                                 daz, 4L << 8, 4L << 8, 0);

          setsprite((short) i, spr->x, spr->y, spr->z);

          if (spr->picnum == FBARRELFALL || spr->picnum >= BOULDER && spr->picnum <= BOULDER + 3
              && (checkdist((short) i, plr->x, plr->y, plr->z))) {
               healthpic(-50);
               startredflash(50);
          }

          if ((hitobject & 0xc0000) == 16384) {
               if (sector[spr->sectnum].floorpicnum == WATER
                   || sector[spr->sectnum].floorpicnum == FLOORMIRROR) {
                    makemonstersplash(SPLASHAROO, i);
               }
               switch (spr->picnum) {
               case FBARRELFALL:
                    newstatus((short) i, SHATTER);
                    spr->lotag = 12;
                    break;
               case TORCH:
                    for (k = 0; k < 16; k++)
                         makeafire(i, 0);
                    deletesprite((short) i);
                    break;
               default:
                    changespritestat(i, 0);
                    break;
               }
               spr->hitag = 0;
          }

          i = nexti;
     }


// SHOVE
     i = headspritestat[SHOVE];

     while (i != -1) {

          nexti = nextspritestat[i];
          spr = &sprite[i];
          if (spr->z < sector[spr->sectnum].floorz)
               daz = spr->zvel += (synctics << 5);

          hitobject = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                 (((long) sintable[spr->ang]) * synctics) << 3,
                                 daz, 4L << 8, 4L << 8, 0);

          setsprite((short) i, spr->x, spr->y, spr->z);

          if (spr->z >= sector[spr->sectnum].floorz) {
               if (sector[spr->sectnum].floorpicnum == WATER
                   || sector[spr->sectnum].floorpicnum == FLOORMIRROR) {
                    makemonstersplash(SPLASHAROO, i);
               }
               newstatus((short) i, BROKENVASE);
               goto outashove;
          }

          if ((hitobject & 0xc000) == 16384) {
               newstatus((short) i, BROKENVASE);
               goto outashove;
          }

          if ((hitobject & 0xc000) == 49152) {    // Bullet hit a sprite

               if (spr->owner != hitobject) {
                    hitdamage = damageactor(hitobject, i);
                    if (hitdamage) {
                         newstatus((short) i, BROKENVASE);
                         goto outashove;
                    }
               }

          }


outashove:
          i = nexti;
     }


// PUSH
     i = headspritestat[PUSH];

     while (i != -1) {

          nexti = nextspritestat[i];

          spr = &sprite[i];

          spr->lotag -= synctics;

          osectnum = spr->sectnum;

          switch (spr->picnum) {
          case BOULDER:
               speed = 9;
               break;
          case BARREL:
               speed = 9;
               break;
          }

          if (spr->z < sector[spr->sectnum].floorz)
               daz = spr->zvel += (synctics << 1);
          // clip type was 1
          hitobject = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                 (((long) sintable[spr->ang]) * synctics) << 3, daz, 4L << 8, 4L << 8, 0);

          setsprite((short) i, spr->x, spr->y, spr->z);

          if (spr->lotag < 0 || (hitobject & 0xc000) == 32768) {
               spr->lotag = 0;
               changespritestat(i, 0);
               if (spr->z < sector[spr->sectnum].floorz) {
                    spr->zvel += 256L;
                    changespritestat(i, FALL);
               }
          }

          i = nexti;
     }

// FLEE

     i = headspritestat[FLEE];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;

          switch (spr->picnum) {

          case NEWGUY:
          case NEWGUYCAST:
          case NEWGUYMACE:
          case NEWGUYPUNCH:

          case KURTAT:
          case KURTPUNCH:
          case GONZOCSWAT:

          case GONZOGSWAT:
          case GONZOGHMAT:
          case GONZOGSHAT:

          case GONZOCSW:

          case GONZOGSW:
          case GONZOGHM:
          case GONZOGSH:

          case GRONHAL:
          case GRONHALATTACK:
          case GRONMU:
          case GRONMUATTACK:
          case GRONSW:
          case GRONSWATTACK:
          case KATIE:
          case KATIEAT:
          case DEVIL:
          case DEVILATTACK:
          case KOBOLD:
          case KOBOLDATTACK:
          case MINOTAUR:
          case MINOTAURATTACK:
          case SKELETON:
          case SKELETONATTACK:
          case IMP:
          case IMPATTACK:
          case DEMON:
          case GUARDIAN:
          case GUARDIANATTACK:
          case RAT:
          case WILLOW:
               if (spr->picnum == DEMON || spr->picnum == GUARDIAN || spr->picnum == GUARDIANATTACK) {
                    spr->z -= synctics << 8;

                    osectnum = spr->sectnum;
                    movestat = movesprite((short) i,
                                          (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                          0L, 4L << 8, 4L << 8, 0);

                    if (totalclock % 100 > 70) {
                         trailingsmoke(i, 1);
                    }
               }
               else {

                    dax = spr->x;  // Back up old x&y if stepping off cliff
                    day = spr->y;
                    daz = spr->z;

                    osectnum = spr->sectnum;
                    movestat = movesprite((short) i,
                                          (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                          0L, 4L << 8, 4L << 8, 0);

                    if (globloz > spr->z + (48 << 8)) {
                         spr->x = dax;
                         spr->y = day;
                         spr->z = daz;
                         setsprite((short) i, spr->x, spr->y, spr->z);
                         movestat = 1;
                    }
                    else {
                         spr->z = globloz;
                    }
               }
               if (movestat != 0) {
                    spr->ang = (getangle(plr->x - spr->x, plr->y - spr->y) & 2047);
                    newstatus((short) i, FACE);
               }
               if (spr->lotag < 0) {
                    newstatus((short) i, FACE);
               }
               break;
          default:
               break;
          }

          if ((spr->sectnum != osectnum) && (sector[spr->sectnum].lotag == 10)) {
               warpsprite((short) i);
          }
          if (sector[spr->sectnum].floorz - (32 << 8) < sector[spr->sectnum].ceilingz) {
               if (sector[spr->sectnum].lotag == 6) {
                    newstatus((short) i, DIE);
               }
               else {
                    deletesprite((short) i);
               }
          }
          if (sector[spr->sectnum].floorpicnum == WATER
              || sector[spr->sectnum].floorpicnum == LAVA
              || sector[spr->sectnum].floorpicnum == SLIME
              || sector[spr->sectnum].floorpicnum == FLOORMIRROR) {
               if (sector[spr->sectnum].floorpicnum == WATER
                   || sector[spr->sectnum].floorpicnum == SLIME
                   || sector[spr->sectnum].floorpicnum == FLOORMIRROR) {
                    if (rand() % 100 > 60) {
                         makemonstersplash(SPLASHAROO, i);
                    }
                    if (spr->picnum == RAT) {
                         spr->hitag--;
                         if (spr->hitag < 0) {
                              newstatus((short) i, DIE);
                         }
                    }
               }
               else {
                    trailingsmoke(i, 1);
                    makemonstersplash(LAVASPLASH, i);
                    if (spr->picnum == RAT) {
                         spr->hitag--;
                         if (spr->hitag < 0) {
                              newstatus((short) i, DIE);
                         }
                    }
               }
          }
          if (                     /* sector[osectnum].floorpicnum == LAVA ||
                                    * sector[osectnum].floorpicnum == ANILAVA
                  || */ sector[osectnum].floorpicnum == SLIME
              || sector[osectnum].floorpicnum == WATER
              || sector[osectnum].floorpicnum == FLOORMIRROR) {

               if (spr->picnum == WILLOW
                   || spr->picnum == DEMON
                   || spr->picnum == GUARDIAN
                   || (spr->picnum >= GUARDIANATTACK
                       && spr->picnum <= GUARDIANATTACK + 6)) {
                    spr->z -= synctics << 8;
               }
               else {
                    spr->z = sector[spr->sectnum].floorz + (16 << 8);
               }
          }


          j = headspritesect[spr->sectnum];
          while (j != -1) {
               nextj = nextspritesect[j];
               tspr = &sprite[j];
               if (isvalidactor((short) i)) {
                    dx = labs(spr->x - tspr->x);  // x distance to sprite
                    dy = labs(spr->y - tspr->y);  // y distance to sprite
                    dz = labs((spr->z >> 8) - (tspr->z >> 8));   // z distance to sprite
                    dh = tilesizy[tspr->picnum] >> 1;  // height of sprite
                    if (dx + dy < PICKDISTANCE && dz - dh <= PICKHEIGHT) {
                         switch (spr->picnum) {
                         case SKELETON:
                         case KOBOLD:


                              switch (tspr->picnum) {
                              case EXPLO2:
                              case SMOKEFX:
                              case MONSTERBALL:
                                   spr->hitag -= synctics << 2;
                                   if (spr->hitag < 0) {
                                        newstatus((short) i, DIE);
                                   }
                                   break;
                              }
                              break;
                         case GRONHAL:
                         case GRONMU:
                         case GRONSW:
                              switch (tspr->picnum) {
                              case EXPLO2:
                              case SMOKEFX:
                              case MONSTERBALL:
                                   spr->hitag -= synctics << 2;
                                   if (spr->hitag < 0) {
                                        newstatus((short) i, DIE);
                                   }
                                   break;
                              }
                              break;
                         }
                    }
               }
               j = nextj;
          }

          i = nexti;
     }


// CHASE

     i = headspritestat[CHASE];
     while (i != -1 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->picnum == RAT) {
               newstatus((short) i, FLEE);
               goto chaseout;
          }
          if (spr->lotag < 0) {
               spr->lotag = 250;
          }
          osectnum = spr->sectnum;
          switch (spr->picnum) {
          case DEMON:
          case GUARDIAN:
          case WILLOW:
               speed = 8;
               if (plr->z < spr->z) {
                    spr->z -= synctics << 8;
               }
               if (plr->z > spr->z) {
                    spr->z += synctics << 8;
               }
               if (krand() % 63 == 0) {
                    if (cansee(plr->x, plr->y, plr->z, plr->sector, spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1 && invisibletime < 0) {
                         newstatus((short) i, ATTACK);
                    }
                    break;
               }
               else {
                    // spr->z=sector[spr->sectnum].floorz-(32<<8);
                    checksight((short) i, &daang);
                    if (spr->picnum == GUARDIAN) {
                         if (totalclock % 100 > 70) {
                              trailingsmoke(i, 1);
                         }
                    }
                    else if (spr->picnum == DEMON) {
                         if (totalclock % 100 > 70) {
                              trailingsmoke(i, 1);
                         }
                    }

                    if (!checkdist((short) i, plr->x, plr->y, plr->z)) {

                         movestat = movesprite((short) i,
                                               (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 2,
                                (((long) sintable[spr->ang]) * synctics) << 2,
                                               0L, 4L << 8, 4L << 8, 0);

                    }
                    else {
                         if (krand() % 8 == 0) {
                              newstatus((short) i, ATTACK);
                         }
                         else {
                              spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                              newstatus((short) i, CHASE);
                         }
                    }
               }
               break;
          case GRONHAL:
          case GRONMU:
          case KATIE:
          case DEVIL:
               speed = 8;
               if (krand() % 63 == 0) {
                    if (cansee(plr->x, plr->y, plr->z, plr->sector, spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1) { // && invisibletime < 0)
                                                                                                                                                 // {
                         newstatus((short) i, ATTACK);
                         goto chaseout;
                    }
               }
               else {
                    checksight((short) i, &daang);
                    if (!checkdist((short) i, plr->x, plr->y, plr->z)) {

                         dax = spr->x;  // Back up old x&y if stepping off
                                        // cliff
                         day = spr->y;
                         daz = spr->z;

                         osectnum = spr->sectnum;
                         movestat = movesprite((short) i,
                                               (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                               0L, 4L << 8, 4L << 8, 0);

                         if (globloz > spr->z + (48 << 8)) {
                              spr->x = dax;
                              spr->y = day;
                              spr->z = daz;
                              setsprite((short) i, spr->x, spr->y, spr->z);
                              movestat = 1;
                         }
                         else {
                              spr->z = globloz;
                         }
                         if (spr->picnum == KATIE) {
                              if (osectnum != spr->sectnum) {
                                   spr->x = dax;
                                   spr->y = day;
                                   spr->z = daz;
                                   setsprite((short) i, spr->x, spr->y, spr->z);
                                   movestat = 1;
                              }
                         }
                    }
                    else {
                         if (krand() % 8 == 0) {
                              newstatus((short) i, ATTACK);
                         }
                         else {
                              spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;     // NEW
                              newstatus((short) i, FLEE);
                         }
                         break;
                    }
               }
               break;

          case GONZOGHM:
          case GONZOGSH:
               monsternoise((short) i);
               speed = 8;
               if (cansee(plr->x, plr->y, plr->z, plr->sector, spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1 && invisibletime < 0) {
                    if (checkdist((short) i, plr->x, plr->y, plr->z)) {
                         if (shadowtime > 0) {
                              spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                              newstatus((short) i, FLEE);
                         }
                         else {
                              newstatus((short) i, ATTACK);
                         }
                         break;
                    }
                    else if (krand() & 32 == 0) {
                         spr->ang = ((krand() & 128 - 256) + spr->ang + 1024) & 2047;
                         newstatus((short) i, FLEE);
                    }
                    if (krand() % 63 > 60) {
                         spr->ang = ((krand() & 128 - 256) + spr->ang + 1024) & 2047;
                         newstatus((short) i, FLEE);
                         break;
                    }

                    dax = spr->x;  // Back up old x&y if stepping off cliff
                    day = spr->y;
                    daz = spr->z;

                    osectnum = spr->sectnum;
                    movestat = movesprite((short) i,
                                          (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                          0L, 4L << 8, 4L << 8, 0);

                    if (globloz > spr->z + (48 << 8)) {
                         spr->x = dax;
                         spr->y = day;
                         spr->z = daz;
                         setsprite((short) i, spr->x, spr->y, spr->z);
                         movestat = 1;

                         if (rand() % 100 > 80 && sector[plr->sector].lotag == 25) {
                              newstatus((short) i, AMBUSH);
                              sprite[i].z -= (PLAYERHEIGHT << 6);
                              sprite[i].lotag = 60;
                              sprite[i].extra = 1;
                              sprite[i].picnum = GONZOHMJUMP;
                              goto chaseout;

                         }

                    }
                    else {
                         spr->z = globloz;
                    }

                    if ((movestat & 0xc000) == 32768
                        && sector[plr->sector].lotag == 25) {
                         newstatus((short) i, AMBUSH);
                         sprite[i].z -= (PLAYERHEIGHT << 6);
                         sprite[i].lotag = 90;
                         sprite[i].extra = 3;
                         sprite[i].picnum = GONZOHMJUMP;
                         goto chaseout;
                    }

                    if (movestat != 0) {
                         if (movestat & 4095 != plr->spritenum) {
                              if (krand() & 2 == 0) {
                                   daang = (spr->ang + 256) & 2047;
                                   spr->ang = daang;
                              }
                              else {
                                   daang = (spr->ang - 256) & 2047;
                                   spr->ang = daang;
                              }
                              if (shadowtime > 0) {
                                   spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                                   newstatus((short) i, FLEE);
                              }
                              else {
                                   newstatus((short) i, SKIRMISH);
                              }
                         }
                         else {
                              spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                              newstatus((short) i, SKIRMISH);
                         }
                    }
                    break;
               }
               else {
                    // spr->ang=((krand()&512-256)+spr->ang+1024)&2047;
                    // newstatus((short)i,FLEE);
                    // newstatus((short)i,FINDME);

                    j = headspritestat[APATROLPOINT];
                    while (j != -1) {
                         nextj = nextspritestat[j];
                         tspr = &sprite[j];
                         dist = klabs(tspr->x - spr->x) + klabs(tspr->y - spr->y);
                         if (dist < mindist) {
                              mindist = dist;
                              target = j;
                         }
                         j = nextj;
                    }
                    tspr = &sprite[target];
                    if (cansee(tspr->x,
                               tspr->y,
                               tspr->z,
                               tspr->sectnum,
                               spr->x,
                               spr->y,
                               spr->z - (tilesizy[spr->picnum] << 7),
                               spr->sectnum) == 1) {
                         spr->ang = (getangle(tspr->x - spr->x, tspr->y - spr->y) & 2047);
                         newstatus((short) i, PATROL);
                    }


                    // break;
               }

               break;

          case KOBOLD:
          case IMP:
          case MINOTAUR:
          case SKELETON:
          case GRONSW:
          case NEWGUY:
          case GONZOCSW:
          case GONZOGSW:

               monsternoise((short) i);
               speed = 8;
               if (cansee(plr->x, plr->y, plr->z, plr->sector, spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum) == 1 && invisibletime < 0) {
                    if (checkdist((short) i, plr->x, plr->y, plr->z)) {
                         if (shadowtime > 0) {
                              spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                              newstatus((short) i, FLEE);
                         }
                         else {
                              newstatus((short) i, ATTACK);
                         }
                         break;
                    }
                    else if (krand() & 32 == 0) {
                         spr->ang = ((krand() & 128 - 256) + spr->ang + 1024) & 2047;
                         newstatus((short) i, FLEE);
                    }
                    if (krand() % 63 > 60) {
                         spr->ang = ((krand() & 128 - 256) + spr->ang + 1024) & 2047;
                         newstatus((short) i, FLEE);
                         break;
                    }

                    dax = spr->x;  // Back up old x&y if stepping off cliff
                    day = spr->y;
                    daz = spr->z;

                    osectnum = spr->sectnum;
                    movestat = movesprite((short) i,
                                          (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                          0L, 4L << 8, 4L << 8, 0);

                    if (globloz > spr->z + (48 << 8)) {
                         spr->x = dax;
                         spr->y = day;
                         spr->z = daz;
                         setsprite((short) i, spr->x, spr->y, spr->z);
                         movestat = 1;

                    }
                    else {
                         spr->z = globloz;
                    }

                    if (movestat != 0) {
                         if (movestat & 4095 != plr->spritenum) {
                              if (krand() & 2 == 0) {
                                   daang = (spr->ang + 256) & 2047;
                                   spr->ang = daang;
                              }
                              else {
                                   daang = (spr->ang - 256) & 2047;
                                   spr->ang = daang;
                              }
                              if (shadowtime > 0) {
                                   spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                                   newstatus((short) i, FLEE);
                              }
                              else {
                                   newstatus((short) i, SKIRMISH);
                              }
                         }
                         else {
                              spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                              newstatus((short) i, SKIRMISH);
                         }
                    }
                    break;
               }
               else {
                    j = headspritestat[APATROLPOINT];
                    while (j != -1) {
                         nextj = nextspritestat[j];
                         tspr = &sprite[j];
                         dist = klabs(tspr->x - spr->x) + klabs(tspr->y - spr->y);
                         if (dist < mindist) {
                              mindist = dist;
                              target = j;
                         }
                         j = nextj;
                    }
                    tspr = &sprite[target];
                    if (cansee(tspr->x,
                               tspr->y,
                               tspr->z,
                               tspr->sectnum,
                               spr->x,
                               spr->y,
                               spr->z - (tilesizy[spr->picnum] << 7),
                               spr->sectnum) == 1) {
                         spr->ang = (getangle(tspr->x - spr->x, tspr->y - spr->y) & 2047);
                         newstatus((short) i, PATROL);
                    }


                    // break;
               }
          }
          if ((spr->sectnum != osectnum) && (sector[spr->sectnum].lotag == 10)) {
               warpsprite((short) i);
          }
          if (sector[spr->sectnum].floorz - (32 << 8) < sector[spr->sectnum].ceilingz) {
               if (sector[spr->sectnum].lotag == 6) {
                    newstatus((short) i, DIE);
               }
               else {
                    deletesprite((short) i);
               }
          }
          switch (sector[spr->sectnum].floorpicnum) {
          case FLOORMIRROR:
          case WATER:
          case SLIME:
               if (spr->picnum == WILLOW
                   || spr->picnum == DEMON
                   || spr->picnum == GUARDIAN
                   || (spr->picnum >= GUARDIANATTACK
                       && spr->picnum <= GUARDIANATTACK + 6)) {
                    spr->z = sector[spr->sectnum].floorz - (16 << 8);
               }
               else {
                    spr->z = sector[spr->sectnum].floorz + (16 << 8);
                    if (rand() % 100 > 60) {
                         makemonstersplash(SPLASHAROO, i);
                    }
               }
               break;
          case LAVA:
               trailingsmoke(i, 1);
               makemonstersplash(LAVASPLASH, i);
               break;
          }
          if (sector[osectnum].lotag == KILLSECTOR && spr->z + (8 << 8) >= sector[osectnum].floorz) {
               spr->hitag--;       // HI
               if (spr->hitag < 0) {
                    newstatus((short) i, DIE);
               }
          }
          if (sector[spr->sectnum].floorpicnum == LAVA ||
              sector[spr->sectnum].floorpicnum == LAVA1 ||
              sector[spr->sectnum].floorpicnum == ANILAVA
              && spr->z + (8 << 8) >= sector[spr->sectnum].floorz) {
               switch (spr->picnum) {
               case SKELETON:
               case KOBOLD:
               case MINOTAUR:

                    spr->hitag--;
                    break;
               }
               if (spr->hitag < 0) {
                    newstatus((short) i, DIE);
               }
          }

          j = headspritesect[spr->sectnum];
          while (j != -1) {
               nextj = nextspritesect[j];
               tspr = &sprite[j];
               if (isvalidactor((short) i)) {
                    dx = labs(spr->x - tspr->x);  // x distance to sprite
                    dy = labs(spr->y - tspr->y);  // y distance to sprite
                    dz = labs((spr->z >> 8) - (tspr->z >> 8));   // z distance to sprite
                    dh = tilesizy[tspr->picnum] >> 1;  // height of sprite
                    if (dx + dy < PICKDISTANCE && dz - dh <= PICKHEIGHT) {
                         switch (spr->picnum) {
                         case SKELETON:
                         case KOBOLD:


                              switch (tspr->picnum) {
                              case EXPLO2:
                              case SMOKEFX:
                              case MONSTERBALL:
                                   spr->hitag -= synctics << 2;
                                   if (spr->hitag < 0) {
                                        newstatus((short) i, DIE);
                                   }
                                   break;
                              }
                              break;
                         case GRONHAL:
                         case GRONMU:
                         case GRONSW:
                              switch (tspr->picnum) {
                              case EXPLO2:
                              case SMOKEFX:
                              case MONSTERBALL:
                                   spr->hitag -= synctics << 2;
                                   if (spr->hitag < 0) {
                                        newstatus((short) i, DIE);
                                   }
                                   break;
                              }
                              break;
                         }
                    }
               }
               j = nextj;
          }
chaseout:
          i = nexti;
     }


     // DRAIN
     i = headspritestat[DRAIN];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               playsound_loc(S_FIREBALL, spr->x, spr->y);
               castspell(i);
               newstatus((short) i, CHASE);
          }
          i = nexti;
     }

     // CAST
     i = headspritestat[CAST];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;

          if (spr->picnum >= GUARDIANATTACK && spr->picnum <= GUARDIANATTACK + 6) {
               if (plr->z < spr->z) {
                    spr->z -= synctics << 8;
               }
               if (plr->z > spr->z) {
                    spr->z += synctics << 8;
               }
          }
          else if (spr->picnum == DEMON) {
               if (plr->z < spr->z) {
                    spr->z -= synctics << 8;
               }
               if (plr->z > spr->z) {
                    spr->z += synctics << 8;
               }
          }

          if (spr->lotag < 0) {

               if (spr->picnum == GONZOCSWAT) {
                    spr->extra--;
                    playsound_loc(S_GENTHROW, spr->x, spr->y);
                    gonzopike(i);
                    newstatus((short) i, CHASE);
                    goto outchase;

               }
               if (spr->picnum == GRONHALATTACK) {
                    spr->extra--;
                    playsound_loc(S_GENTHROW, spr->x, spr->y);
                    throwhalberd(i);
                    newstatus((short) i, CHASE);
                    goto outchase;
               }

               if (spr->picnum == GRONMUATTACK) {
                    spr->extra--;
                    playsound_loc(S_WISP, spr->x, spr->y);
                    castspell(i);
                    newstatus((short) i, CHASE);
                    goto outchase;
               }
               else if (spr->picnum == DEMON) {
                    castspell(i);
                    newstatus((short) i, CHASE);
                    goto outchase;
               }
               else {
                    spr->picnum++;
                    spr->lotag = 12;
               }

          }

          if (spr->picnum == KATIEAT + 2) {
               if (spr->extra >= 2) {
                    if (rand() % 100 > 50) {
                         for (j = 0; j < 4; j++) {
                              spawnabaddy((short) i, GONZOGSH);
                         }
                    }
                    else {
                         for (j = 0; j < 6; j++) {
                              spawnabaddy((short) i, GRONSW);
                         }
                    }
                    spr->picnum = KATIE;
                    spr->extra--;
                    playsound_loc(S_FIREBALL, spr->x, spr->y);
                    newstatus((short) i, CHASE);
               }
          }
          if (spr->picnum == KATIEAT + 6) {
               if (spr->extra == 1) {
                    k = 0;
                    for (j = 0; j < MAXSPRITES; j++) {
                         if (sprite[j].pal == 8) {
                              sprite[j].picnum--;
                              sprite[j].pal = 0;
                              sprite[j].shade = 0;
                              changespritestat((short) j, FACE);
                         }
                    }
                    spr->picnum = KATIE;
                    playsound_loc(S_FIREBALL, spr->x, spr->y);
                    newstatus((short) i, CHASE);
                    spr->extra--;
               }
          }
          if (spr->picnum == KATIEAT + 16) {
               spr->picnum = KATIE;
               playsound_loc(S_FIREBALL, spr->x, spr->y);
               castspell(i);
               newstatus((short) i, CHASE);
               spr->extra++;
          }

          switch (spr->picnum) {
          case DEVILATTACK + 2:
               spr->picnum = DEVIL;
               playsound_loc(S_FIREBALL, spr->x, spr->y);
               castspell(i);
               newstatus((short) i, CHASE);
               break;

          case NEWGUYCAST + 2:
               spr->extra--;
               spr->picnum = NEWGUY;
               playsound_loc(S_WISP, spr->x, spr->y);
               skullycastspell((short) i);
               newstatus((short) i, CHASE);
               break;

          case NEWGUYBOW + 2:
               spr->extra--;
               spr->picnum = NEWGUY;
               playsound_loc(S_PLRWEAPON3, spr->x, spr->y);
               newguyarrow((short) i);
               newstatus((short) i, CHASE);
               break;

          case GUARDIANATTACK + 6:
               spr->picnum = GUARDIAN;
               playsound_loc(S_FIREBALL, spr->x, spr->y);
               castspell(i);
               newstatus((short) i, CHASE);
               break;
          }
          if (sector[spr->sectnum].floorz - (32 << 8) < sector[spr->sectnum].ceilingz) {
               if (sector[spr->sectnum].lotag == 6)
                    newstatus((short) i, DIE);
               else {
                    deletesprite((short) i);
               }
          }
outchase:
          i = nexti;
     }

     // DIE
     i = headspritestat[DIE];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          switch (spr->picnum) {
          case RAT:
          case FISH:
               deletesprite((short) i);
               goto dieout;
               break;
          case GRONHALDIE:
          case GRONSWDIE:
          case GRONMUDIE:
               if (spr->lotag < 0) {
                    spr->picnum = GRONDIE;
                    spr->lotag = 20;
               }
               else {
                    goto dieout;
               }
               break;
          }
          if (spr->lotag <= 0) {
               spr->picnum++;
               spr->lotag = 20;
               if (spr->picnum == WILLOWEXPLO
                   || spr->picnum == WILLOWEXPLO + 1
                   || spr->picnum == WILLOWEXPLO + 2)
                    spr->xrepeat = spr->yrepeat <<= 1;
               switch (spr->picnum) {
               case WILLOWEXPLO + 2:

               case GONZOBSHDEAD:

               case GONZOCSWDEAD:

               case GONZOGSWDEAD:
               case GONZOGHMDEAD:
               case GONZOGSHDEAD:
               case NEWGUYDEAD:
               case KOBOLDDEAD:
               case KATIEDEAD:
               case DEVILDEAD:
               case SKELETONDEAD:
               case IMPDEAD:
               case MINOTAURDEAD:
               case GRONDEAD:
                    if (difficulty == 4) {
                         newstatus((short) i, RESURECT);
                    }
                    else {
                         newstatus((short) i, DEAD);
                    }
                    break;
               }
          }
dieout:
          i = nexti;
     }

     //
     // STAND
     //
     i = headspritestat[STAND];
     while (i >= 0 && netgame == 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          if (sintable[(spr->ang + 512) & 2047] * (plr->x - spr->x) + sintable[spr->ang & 2047] * (plr->y - spr->y) >= 0)
               if (cansee(spr->x,
                          spr->y,
                          spr->z - (tilesizy[spr->picnum] << 7), spr->sectnum, plr->x, plr->y, plr->z, plr->sector) == 1
                   && invisibletime < 0) {
                    switch (spr->picnum) {
                    case HANGMAN:
                         newstatus((short) i, CHILL);
// BIKINI
//                        playsound_loc(S_SKELSEE,spr->x,spr->y);
                         break;
                    case SKELETON:
                    case KOBOLD:
                    case IMP:

                    case MINOTAUR:
                    case FISH:
                    case RAT:
                         if (shadowtime > 0) {
                              spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;     // NEW
                              newstatus((short) i, FLEE);
                         }
                         else
                              newstatus((short) i, CHASE);
                         break;
                    case KATIE:
                    case DEVIL:
                    case DEMON:
                    case GUARDIAN:
                    case WILLOW:
                         newstatus((short) i, FACE);
                         break;
                    case NEWGUYSTAND:
                    case NEWGUYKNEE:
                    case KURTSTAND:
                    case KURTKNEE:
                         if (shadowtime > 0) {
                              spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;
                              newstatus((short) i, FLEE);
                         }
                         else
                              newstatus((short) i, CHASE);
                         break;
                    }
               }
          if (sector[spr->sectnum].floorz - (32 << 8) < sector[spr->sectnum].ceilingz) {
               if (sector[spr->sectnum].lotag == 6)
                    newstatus((short) i, DIE);
               else {
                    deletesprite((short) i);
               }
          }
          i = nexti;
     }

     i = headspritestat[ACTIVE];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          osectnum = spr->sectnum;
          j = (torchpattern[lockclock % 38]);
          sector[osectnum].ceilingshade = j;
          sector[osectnum].floorshade = j;
          startwall = sector[osectnum].wallptr;
          endwall = startwall + sector[osectnum].wallnum - 1;
          for (k = startwall; k <= endwall; k++)
               wall[k].shade = j;
          if (spr->lotag < 0) {
               sector[osectnum].ceilingshade = ceilingshadearray[osectnum];
               sector[osectnum].floorshade = floorshadearray[osectnum];
               startwall = sector[osectnum].wallptr;
               endwall = startwall + sector[osectnum].wallnum - 1;
               for (k = startwall; k <= endwall; k++) {
                    wall[k].shade = wallshadearray[k];
               }
               newstatus((short) i, DORMANT);
          }
          i = nexti;
     }

     i = headspritestat[DORMANT];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          osectnum = spr->sectnum;
          j = (torchpattern[lockclock % 38]);
          sector[osectnum].ceilingshade = j;
          sector[osectnum].floorshade = j;
          startwall = sector[osectnum].wallptr;
          endwall = startwall + sector[osectnum].wallnum - 1;
          for (k = startwall; k <= endwall; k++)
               wall[k].shade = j;
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               newstatus((short) i, ACTIVE);
          }
          i = nexti;
     }

     // New missile code
     i = headspritestat[MISSILE];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          switch (spr->picnum) {
          case THROWPIKE:
          case FATSPANK:
          case MONSTERBALL:
               // case FIREBALL:
          case PLASMA:
               if (spr->picnum == MONSTERBALL && rand() % 100 > 90) {
                    if (spr->lotag < 200)
                         trailingsmoke(i, 2);
               }
               spr->z += spr->zvel;
               if (spr->z < sector[spr->sectnum].ceilingz + (4 << 8)) {
                    spr->z = sector[spr->sectnum].ceilingz + (4 << 8);
                    spr->zvel = -(spr->zvel >> 1);
               }
               if (spr->z > sector[spr->sectnum].floorz - (4 << 8)) {
                    spr->z = sector[spr->sectnum].floorz - (4 << 8);
                    if (sector[spr->sectnum].floorpicnum == WATER
                        || sector[spr->sectnum].floorpicnum == SLIME
                        || sector[spr->sectnum].floorpicnum == FLOORMIRROR)
                         if (rand() % 100 > 60)
                              makemonstersplash(SPLASHAROO, i);
                    deletesprite((short) i);
                    goto bulletisdeletedskip;
               }
               dax = spr->xvel;
               day = spr->yvel;
               daz = ((((long) spr->zvel) * synctics) >> 3);
               break;
          case BULLET:
               dax = spr->xvel;
               day = spr->yvel;
               daz = spr->zvel;
               break;
          }                        // switch
          osectnum = spr->sectnum;
          if (spr->picnum == THROWPIKE) {
               spr->cstat = 0;
               hitobject = movesprite((short) i,
               (((long) sintable[(spr->extra + 512) & 2047]) * synctics) << 6,
                              (((long) sintable[spr->extra]) * synctics) << 6,
                                      daz, 4L << 8, 4L << 8, 1);
               spr->cstat = 21;
          }
          else {
               hitobject = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 6,
                                (((long) sintable[spr->ang]) * synctics) << 6,
                                      daz, 4L << 8, 4L << 8, 1);
          }
          if ((hitobject & 0xc000) == 16384) {    // Hits a ceiling / floor
               if (spr->picnum == MONSTERBALL) {
                    if (spr->owner == 4096) {
                         explosion2(i, spr->x, spr->y, spr->z, i);
                    }
                    else {
                         explosion(i, spr->x, spr->y, spr->z, i);
                    }
               }

               switch (spr->picnum) {
               default:
                    deletesprite((short) i);
                    goto bulletisdeletedskip;
               }
          }
          else if ((hitobject & 0xc000) == 32768) {    // hit a wall
               if (spr->picnum == MONSTERBALL) {
                    if (spr->owner == 4096) {
                         explosion2(i, spr->x, spr->y, spr->z, i);
                    }
                    else {
                         explosion(i, spr->x, spr->y, spr->z, i);
                    }
               }

               if (spr->picnum == THROWPIKE) {
                    spr->picnum++;
                    changespritestat(i, 0);
                    goto bulletisdeletedskip;
               }
               deletesprite((short) i);
               goto bulletisdeletedskip;

          }
          else if (spr->lotag < 0 && spr->picnum == PLASMA) {
               hitobject = 1;
          }
          if ((hitobject & 0xc000) == 49152) {    // Bullet hit a sprite
               if (spr->picnum == MONSTERBALL) {
                    if (spr->owner == 4096) {
                         explosion2(i, spr->x, spr->y, spr->z, i);
                    }
                    else {
                         explosion(i, spr->x, spr->y, spr->z, i);
                    }
               }
               if (spr->owner != hitobject) {
                    hitdamage = damageactor(hitobject, i);
               }
               if (hitdamage) {
                    deletesprite((short) i);
                    goto bulletisdeletedskip;
               }
          }
          if (hitobject != 0 || spr->lotag < 0) {
               x = spr->x;
               y = spr->y;
               z = spr->z;
               belongs = spr->owner;
               switch (spr->picnum) {
               case THROWPIKE:
               case PLASMA:
               case FATSPANK:
               case MONSTERBALL:
                    // case FIREBALL:
               case BULLET:
                    deletesprite((short) i);
                    goto bulletisdeletedskip;
                    break;
               }
          }
bulletisdeletedskip:
          i = nexti;
     }

     i = headspritestat[JAVLIN];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          switch (spr->picnum) {
          case THROWPIKE:
          case WALLARROW:
          case DART:
          case HORIZSPIKEBLADE:
          case THROWHALBERD:
               spr->z -= spr->zvel;
               if (spr->z < sector[spr->sectnum].ceilingz + (4 << 8)) {
                    spr->z = sector[spr->sectnum].ceilingz + (4 << 8);
                    spr->zvel = -(spr->zvel >> 1);
               }
               if (spr->z > sector[spr->sectnum].floorz - (4 << 8)) {
                    spr->z = sector[spr->sectnum].floorz - (4 << 8);
                    if (sector[spr->sectnum].floorpicnum == WATER
                        || sector[spr->sectnum].floorpicnum == SLIME
                        || sector[spr->sectnum].floorpicnum == FLOORMIRROR)
                         if (rand() % 100 > 60) {
                              makemonstersplash(SPLASHAROO, i);
                         }
                    deletesprite((short) i);
                    goto bulletisdeletedskip;
               }
               dax = spr->xvel;
               day = spr->yvel;
               // daz=((((long)spr->zvel)*synctics)>>3);
               // daz=(long)((spr->zvel*synctics)>>3);
               daz = spr->zvel;
               break;
          }
          osectnum = spr->sectnum;
          spr->cstat = 0;
          hitobject = movesprite((short) i,
               (((long) sintable[(spr->extra + 512) & 2047]) * synctics) << 6,
                              (((long) sintable[spr->extra]) * synctics) << 6,
                                 daz, 4L << 8, 4L << 8, 0);
          if (spr->picnum == WALLARROW || spr->picnum == THROWHALBERD) {
               spr->cstat = 0x11;
          }
          else if (spr->picnum == DART) {
               spr->cstat = 0x10;
          }
          else {
               spr->cstat = 0x15;
          }
          if ((hitobject & 0xc000) == 16384) {    // Hits a ceiling / floor
               if (spr->picnum == THROWPIKE) {
                    spr->picnum++;
               }
               changespritestat(i, 0);
               goto javlinskip;
          }
          else if ((hitobject & 0xc000) == 32768) {    // hit a wall
               if (spr->picnum == THROWPIKE) {
                    spr->picnum++;
               }
               changespritestat(i, 0);
               goto javlinskip;
          }
          if ((hitobject - 49152) >= 0 || hitobject & 0xc000 == 49152) {   // Bullet hit a sprite
               j = (hitobject & 4095);  // j is the spritenum that the bullet
                                        // (spritenum i) hit
               // if (spr->owner != hitobject) {
               hitdamage = damageactor(hitobject, i);
               if (hitdamage)
                    goto javlinskip;
               // }
               if (!hitdamage) {
                    switch (sprite[j].picnum) {
                    case THROWPIKE:
                    case WALLARROW:
                    case DART:
                    case HORIZSPIKEBLADE:
                    case THROWHALBERD:
                         deletesprite((short) i);
                         goto javlinskip;
                         break;
                    }
               }
          }
          if (hitobject != 0) {
               deletesprite((short) i);
               goto javlinskip;
          }
javlinskip:
          i = nexti;
     }
     i = headspritestat[FIRECHUNK];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               deletesprite((short) i);
          }
          i = nexti;
     }

     // CHUNK O WALL
     i = headspritestat[CHUNKOWALL];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          dax = spr->xvel >> 3;
          day = spr->yvel >> 3;
          daz = spr->zvel -= synctics << 2;
          movestat = movesprite((short) i, (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3, (((long) sintable[spr->ang]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 1);
          setsprite((short) i, spr->x, spr->y, spr->z);
          if (spr->extra == 0) {
               if (spr->lotag < 0) {
                    spr->lotag = 8;
                    spr->picnum++;
                    if (spr->picnum == SMOKEFX + 3) {
                         deletesprite((short) i);
                    }
               }
          }
          else {
               if (spr->lotag < 0) {
                    deletesprite((short) i);
               }
          }
          i = nexti;
     }

     // CHUNK O MEAT
     i = headspritestat[CHUNKOMEAT];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          spr->z += spr->zvel;
          daz = spr->zvel += synctics << 4;
          if (spr->picnum == BONECHUNK1
              && spr->picnum == BONECHUNKEND) {
               daz >>= 1;
               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 2,
                                (((long) sintable[spr->ang]) * synctics) << 2,
                                     daz, 4L << 8, 4L << 8, 1);
          }
          else {
               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                     daz, 4L << 8, 4L << 8, 1);
          }
          if (movestat >= 16384 && movestat < 32768) {
               if (sector[spr->sectnum].floorpicnum == WATER
                   || sector[spr->sectnum].floorpicnum == SLIME
                   || sector[spr->sectnum].floorpicnum == FLOORMIRROR) {
                    if (rand() % 100 > 60) {
                         makemonstersplash(SPLASHAROO, i);
                    }
                    spr->lotag = -1;
               }
               else {
                    if (spr->picnum >= BONECHUNK1 && spr->picnum <= BONECHUNKEND) {
                         deletesprite((short) i);
                    }
                    else {
                         spr->cstat|=0x0020;
                         spr->lotag = 1200;
                         newstatus((short) i, BLOOD);
                    }
               }
          }
          else if (movestat >= 32768 && movestat < 49152) {
               deletesprite((short) i);
#if 0
               if (spr->picnum >= BONECHUNK1 && spr->picnum <= BONECHUNKEND) {
                    deletesprite((short) i);
               }
               else {
//                    k=movestat-32768;
//                    dax=(wall[k].x-wall[wall[k].point2].x)/2;
//                    day=(wall[k].y-wall[wall[k].point2].y)/2;
//                    spr->ang=getangle(spr->x-dax,spr->y-day);
                    spr->cstat|=0x0010;
                    spr->lotag = 1200;
                    newstatus((short) i, BLOOD);
               }
#endif
          }
          if (spr->lotag < 0) {
               deletesprite((short) i);
          }
          i = nexti;
     }

     i = headspritestat[BLOOD];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               if (spr->z < sector[spr->sectnum].floorz) {
                    spr->lotag = 600;
                    spr->zvel = 0;
                    newstatus((short) i, DRIP);
               }
               else {
                    deletesprite((short) i);
               }
          }
          i = nexti;
     }

     i = headspritestat[DEVILFIRE];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          if (invisibletime < 0) {
               spr->lotag -= synctics;
               if (spr->lotag < 0) {
                    spr->lotag = krand() & 120 + 360;
                    if (cansee(plr->x,
                               plr->y,
                               plr->z,
                               plr->sector,
                               spr->x,
                               spr->y,
                               spr->z -
                               (tilesizy[spr->picnum] << 7),
                               spr->sectnum) == 1) {
                         // JSA_NEW
                         playsound_loc(S_FIREBALL, spr->x, spr->y);
                         castspell(i);
                    }
               }
          }
          i = nexti;
     }

     i = headspritestat[DRIP];
     while (i != -1) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          spr->z += spr->zvel;
          dax = 0L;
          day = 0L;
          daz = spr->zvel += synctics << 1;
          daz = ((((long) spr->zvel) * synctics) << 1);
          movestat = movesprite((short) i, dax, day, daz, 4L << 8, 4L << 8, 1);
          if ((movestat & 0xc000) == 16384) {
               spr->lotag = 1200;
               newstatus((short) i, BLOOD);
          }
          if (spr->lotag < 0)
               deletesprite((short) i);
          i = nexti;
     }

     i = headspritestat[SMOKE];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
//        spr->z-=(synctics<<6);

          if (spr->xrepeat > 1) {
               spr->xrepeat = spr->yrepeat -= synctics;
          }
//        setsprite((short)i,spr->x,spr->y,spr->z);
          if (spr->lotag < 0) {
               deletesprite((short) i);
          }
          i = nexti;
     }

     i = headspritestat[EXPLO];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          spr->picnum++;
          if (spr->lotag < 0) {
               spr->lotag = 12;
          }

          j = headspritesect[spr->sectnum];
          while (j != -1) {
               nextj = nextspritesect[j];
               tspr = &sprite[j];
               dx = labs(spr->x - tspr->x);  // x distance to sprite
               dy = labs(spr->y - tspr->y);  // y distance to sprite
               dz = labs((spr->z >> 8) - (tspr->z >> 8));   // z distance to sprite
               dh = tilesizy[tspr->picnum] >> 1;  // height of sprite
               if (dx + dy < PICKDISTANCE && dz - dh <= PICKHEIGHT) {
                    if (tspr->owner == 4096) {
                         // strcpy(displaybuf,"hit player");
                         // displaytime=120;
                    }
                    else {
                         switch (tspr->picnum) {
                         case SKELETON:
                         case KOBOLD:
                         case IMP:
                              tspr->hitag -= synctics << 4;
                              if (tspr->hitag < 0) {
                                   newstatus((short) j, DIE);
                              }
                              break;
                         case NEWGUYSTAND:
                         case NEWGUYKNEE:
                         case NEWGUYCAST:
                         case NEWGUYPUNCH:
                         case NEWGUYMACE:

                         case KURTSTAND:
                         case KURTKNEE:
                         case KURTAT:
                         case KURTPUNCH:
                         case KURTREADY:



                         case GONZOCSW:
                         case GONZOCSWAT:

                         case GONZOGSW:
                         case GONZOGSWAT:

                         case GONZOGHM:
                         case GONZOGHMAT:

                         case GONZOGSH:
                         case GONZOGSHAT:

                         case GRONHAL:
                         case GRONMU:
                         case GRONSW:
                              tspr->hitag -= synctics << 4;
                              if (tspr->hitag < 0) {
                                   newstatus((short) j, DIE);
                              }
                              break;
                         }
                    }
               }
               j = nextj;
          }

          if (spr->picnum == EXPLOEND) {
               deletesprite((short) i);
          }
          i = nexti;
     }

     i = headspritestat[BROKENVASE];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->lotag < 0) {
               spr->picnum++;
               spr->lotag = 18;
               switch (spr->picnum) {
               case FSHATTERBARREL + 2:
                    randompotion(i);
               case SHATTERVASE + 6:
               case SHATTERVASE2 + 6:
               case SHATTERVASE3 + 6:
                    changespritestat(i, 0);
                    break;
               case STAINGLASS1 + 6:
               case STAINGLASS2 + 6:
               case STAINGLASS3 + 6:
               case STAINGLASS4 + 6:
               case STAINGLASS5 + 6:
               case STAINGLASS6 + 6:
               case STAINGLASS7 + 6:
               case STAINGLASS8 + 6:
               case STAINGLASS9 + 6:
                    changespritestat(i, 0);
                    break;
               }
          }
          i = nexti;
     }

     i = headspritestat[FX];
     while (i >= 0) {
          nexti = nextspritestat[i];
          spr = &sprite[i];
          spr->lotag -= synctics;
          if (spr->picnum == BULLET
              || spr->picnum == EXPLOSION
          // || spr->picnum == FIREBALL
              || spr->picnum == MONSTERBALL
              || spr->picnum == FATSPANK) {
               spr->zvel += (synctics << 5);
               if (spr->z < sector[spr->sectnum].ceilingz + (4 << 8)) {
                    spr->z = sector[spr->sectnum].ceilingz + (4 << 8);
                    spr->zvel = -(spr->zvel >> 1);
               }
               if (spr->z > sector[spr->sectnum].floorz - (4 << 8)
                   && spr->picnum != EXPLOSION) {
                    spr->z = sector[spr->sectnum].floorz - (4 << 8);
                    spr->zvel = 0;
                    spr->lotag = 4;
               }
               dax = ((((long) spr->xvel) * synctics) >> 3);
               day = ((((long) spr->yvel) * synctics) >> 3);
               daz = (((long) spr->zvel) * synctics);
               movestat = movesprite((short) i, dax, day, daz, 4L << 8, 4L << 8, 0);
               setsprite((short) i, spr->x, spr->y, spr->z);
          }

          if (spr->picnum == ICECUBE && spr->z < sector[spr->sectnum].floorz) {
               spr->z += spr->zvel;
               daz = spr->zvel += synctics << 4;
               movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                                (((long) sintable[spr->ang]) * synctics) << 3,
                                     daz, 4L << 8, 4L << 8, 0);
               setsprite((short) i, spr->x, spr->y, spr->z);
          }

          if (spr->lotag < 0 || movestat != 0) {
               if (spr->picnum == PLASMA
                   || spr->picnum == EXPLOSION
               // || spr->picnum == FIREBALL
                   || spr->picnum == MONSTERBALL
                   || spr->picnum == FATSPANK
                   || spr->picnum == ICECUBE) {
                    deletesprite((short) i);
                    goto outathere;
               }
          }

          if (spr->z + (8 << 8) >= sector[spr->sectnum].floorz
              && spr->picnum == ICECUBE) {
               spr->z = sector[spr->sectnum].floorz;
               if (sector[spr->sectnum].floorpicnum == WATER
                   || sector[spr->sectnum].floorpicnum == SLIME
                   || sector[spr->sectnum].floorpicnum == FLOORMIRROR) {
                    if (rand() % 100 > 60) {
                         makemonstersplash(SPLASHAROO, i);
                         deletesprite((short) i);
                    }
               }
               else {
                    if (spr->lotag < 0) {
                         deletesprite((short) i);
                    }
               }
          }
outathere:
          i = nexti;
     }

}


/*
     spr->cstat |= 0x8000;        //Set invisible bit
     drawrooms(posx,posy,etc.);
     spr->cstat &= ~0x8000;       //Restore invisible bit
*/


int
findapatrolpoint(short i)
{
     short p,
          target;
     long mindist = 0x7fffffff;
     long dist;
     short oldang;
     spritetype *spr,
         *tspr;

     target = 0;

     spr = &sprite[i];

     oldang = spr->ang;

     for (p = 0; p < MAXSPRITES; p++) {
          if (sprite[p].picnum == PATROLPOINT && spr->owner != p) {
               dist = klabs(sprite[p].x - spr->x) + klabs(sprite[p].y - spr->y);
               if (dist < mindist) {
                    mindist = dist;
                    target = p;
               }
          }
     }

     spr->owner = target;
     tspr = &sprite[target];

     if (tspr->cstat != -1) {
          spr->ang = (getangle(spr->x - tspr->x, spr->y - tspr->y) & 2047);

          if (cansee(tspr->x,
                     tspr->y,
                     tspr->z,
                     tspr->sectnum,
                     spr->x,
                     spr->y,
                     spr->z - (tilesizy[spr->picnum] << 7),
                     spr->sectnum) == 1) {

               spr->owner = target;
               return (1);
          }
          else {
               spr->ang = oldang;
               return (0);
          }
     }

     return (0);
}


void
checkhit(short i, short j)
{
     short hitsect,
          hitwall,
          hitsprite,
          daang2,
          daang,
          dahoriz;
     long daz2,
          hitx,
          hity,
          hitz;
     long x,
          y,
          z,
          dasectnum;

     daang = sprite[j].ang;
     x = sprite[j].x;
     y = sprite[j].y;
     z = sprite[j].z;
     dasectnum = sprite[j].sectnum;
     daang2 = ((daang + 2048) & 2047);
     // daz2=((100-dahoriz)*2000)>>1;
     daz2 = 0;

     hitscan(x, y, z, dasectnum,   // Start position
             sintable[(daang2 + 2560) & 2047],    // X vector of 3D ang
             sintable[(daang2 + 2048) & 2047],    // Y vector of 3D ang
             daz2,                 // Z vector of 3D ang
             &hitsect, &hitwall, &hitsprite, &hitx, &hity, &hitz);

     sprite[i].ang = (getangle(hitx - sprite[i].x, hity - sprite[i].y) & 2047);

}
