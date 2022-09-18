/***************************************************************************
 *   WHOBJ.C  - object animation code for Witchaven game                   *
 *                                                                         *
 ***************************************************************************/

#include "icorp.h"
#include <memcheck.h>

#pragma aux mulscale =\
    "imul ebx",\
    "shrd eax, edx, cl",\
    parm [eax][ebx][ecx]\
    modify [edx]\

extern int treasuresfound,
     kills;


int  shieldtype = 0;
int  vampiretime = 0;

long justwarpedcnt = 0;
extern justwarpedfx;

extern int difficulty;

extern int cartsnd;

extern int mapon;

extern int spellbooktics;
extern int spellbook;
extern int spellbookframe;
extern int spellbookflip;

extern int pressedkey;
extern long pageoffset;
extern long selectedgun;
extern int currentpotion;

short torchpattern[38] = {2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 4, 4, 6, 6, 4, 4, 6, 6, 4, 4, 6, 6, 4, 4, 6, 6, 5, 5, 4, 4, 3, 3, 3, 2, 2, 2};

char flashflag = 0x00;

char displaybuf[50];
char scorebuf[50];
char healthbuf[50];
char armorbuf[50];
char potionbuf[50];

int  displaytime;
int  shadowtime;
int  helmettime;

int  potiontilenum;
int  debuginfo;

int  scoretime;
extern int strongtime,
     invincibletime,
     manatime,
     invisibletime,
     nightglowtime;

extern
long qsetmode;

void
monitor(void)
{

     int  i,
          j;
     int  val;
     struct player *plr;
     char buffer[20];
     long dist,
          mindist;
     short lockx,
          locky;
     short scrx,
          scry;
     char temph[30];

     plr = &player[0];

     if (justwarpedfx > 0) {
          justwarpedfx -= synctics;
          justwarpedcnt += synctics << 6;
          rotatesprite(320 << 15, 200 << 15, justwarpedcnt << 9, 0, ANNIHILATE, 0, 0, 1 + 2, windowx1, windowy1, windowx2, windowy2);
          if (justwarpedfx <= 0)
               justwarpedcnt = 0;
     }

     if (poisoned == 1) {
          if (poisontime >= 0) {
               poisontime -= synctics;
               healthpic(0);
               if (poisontime < 0) {
                    startredflash(50);
                    healthpic(-10);
                    poisontime = 7200;
               }
          }
     }

     if (vampiretime > 0)
          vampiretime -= synctics;

     if (shockme >= 0)
          shockme -= synctics;

     if (helmettime > 0) {
          helmettime -= synctics;
          if (svga == 1)
               rotatesprite(270L << 16, (long) (tilesizy[HELMET] >> 1) << 16, 32768L, 0, HELMET, 0, 0, 0x02, windowx1, windowy1, windowx2, windowy2);
          else
               rotatesprite(300L << 16, (long) (tilesizy[HELMET] >> 1) << 16, 32768L, 0, HELMET, 0, 0, 0x02, windowx1, windowy1, windowx2, windowy2);
     }

     if (displaytime > 0) {
          fancyfontscreen(18, 24, THEFONT, displaybuf);
          displaytime -= ((long) synctics);
     }

     if (shadowtime >= 0)
          shadowtime -= synctics;

     if (nightglowtime >= 0) {
          nightglowtime -= synctics;
          visibility = 256;
          if (nightglowtime < 0)
               visibility = 1024;
     }

     if (strongtime >= 0) {
          strongtime -= synctics;
          startwhiteflash(10);
     }

     if (invisibletime >= 0) {
          invisibletime -= synctics;
     }

     if (manatime >= 0) {
          manatime -= synctics;
     }

     if (displaytime <= 0) {
          if (manatime > 0) {
               if (manatime < 512) {
                    if ((manatime%64) > 32) {
                         return;
                    }
               }
               fancyfontscreen(18, 24, THEFONT, "FIRE RESISTANCE");
          }
          else if (poisoned == 1) {
               fancyfontscreen(18, 24, THEFONT, "POISONED");
          }
          else if (plr->orbactive[5] > 0) {
               if (plr->orbactive[5] < 512) {
                    if ((plr->orbactive[5]%64) > 32) {
                         return;
                    }
               }
               fancyfontscreen(18, 24, THEFONT, "FLYING");
          }
          else if (vampiretime > 0) {
               fancyfontscreen(18, 24, THEFONT, "ORNATE HORN");
          }
     }
     return;
}

extern
int  droptheshield;

extern void
fadeout(int start, int end, int red, int green, int blue, int steps); // JEFF

extern
int  spikeme;

void
processobjs(struct player * plr)
{

     long dh,
          dx,
          dy,
          dz,
          nexti;
     short i;
     int  j;
     spritetype *spr;

     if (plr->sector < 0 || plr->sector >= numsectors) {
          return;
     }
     i = headspritesect[plr->sector];
     while (i != -1) {
          nexti = nextspritesect[i];
          spr = &sprite[i];
          dx = labs(plr->x - spr->x);   // x distance to sprite
          dy = labs(plr->y - spr->y);   // y distance to sprite
          dz = labs((plr->z >> 8) - (spr->z >> 8));    // z distance to sprite
          dh = tilesizy[spr->picnum] >> 1;   // height of sprite
          if (dx + dy < PICKDISTANCE && dz - dh <= PICKHEIGHT) {
               switch (spr->picnum) {
               case SILVERBAG:
               case SILVERCOINS:
                    strcpy(displaybuf, "Silver!");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(rand() % 100 + 10);
                    break;
               case GOLDBAG:
               case GOLDBAG2:
               case GOLDCOINS:
               case GOLDCOINS2:
                    strcpy(displaybuf, "Gold!");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(rand() % 100 + 10);
                    break;
               case HELMET:
                    strcpy(displaybuf, "Hero Time");
                    displaytime = 360;
                    deletesprite((short) i);
                    helmettime = 7200;
                    SND_Sound(S_STING1 + rand() % 2);
                    score(10);
                    treasuresfound++;
                    break;
               case PLATEARMOR:
                    if (plr->armor <= 149) {
                         strcpy(displaybuf, "Plate Armor");
                         displaytime = 360;
                         deletesprite((short) i);
                         plr->armortype = 3;
                         plr->armor = 0;
                         armorpic(150);
                         SND_Sound(S_POTION1);
                         score(40);
                         treasuresfound++;
                    }
                    break;
               case CHAINMAIL:
                    if (plr->armor <= 99) {
                         strcpy(displaybuf, "Chain Mail");
                         displaytime = 360;
                         deletesprite((short) i);
                         plr->armortype = 2;
                         plr->armor = 0;
                         armorpic(100);
                         SND_Sound(S_POTION1);
                         score(20);
                         treasuresfound++;
                    }
                    break;
               case LEATHERARMOR:
                    if (plr->armor <= 49) {
                         strcpy(displaybuf, "Leather Armor");
                         displaytime = 360;
                         deletesprite((short) i);
                         plr->armortype = 1;
                         plr->armor = 0;
                         armorpic(50);
                         SND_Sound(S_POTION1);
                         score(10);
                         treasuresfound++;
                    }
                    break;
               case GIFTBOX:
                    treasuresfound++;
                    playsound_loc(S_TREASURE1, spr->x, spr->y);
                    j = rand() % 7;
                    switch (j) {
                    case 0:
                         switch (rand() % 5) {
                         case 0:
                              if (!potionspace(0)) {
                                   break;
                              }
                              strcpy(displaybuf, "Health Potion");
                              displaytime = 360;
                              updatepotion(HEALTHPOTION);
                              potionpic(0);
                              currentpotion = 0;
                              SND_Sound(S_POTION1);
                              score(10);
                              break;
                         case 1:
                              if (!potionspace(1)) {
                                   break;
                              }
                              strcpy(displaybuf, "Strength Potion");
                              displaytime = 360;
                              updatepotion(STRENGTHPOTION);
                              potionpic(1);
                              currentpotion = 1;
                              SND_Sound(S_POTION1);
                              score(20);
                              break;
                         case 2:
                              if (!potionspace(2)) {
                                   break;
                              }
                              strcpy(displaybuf, "Cure Poison Potion");
                              displaytime = 360;
                              updatepotion(ARMORPOTION);
                              potionpic(2);
                              currentpotion = 2;
                              SND_Sound(S_POTION1);
                              score(15);
                              break;
                         case 3:
                              if (!potionspace(3)) {
                                   break;
                              }
                              strcpy(displaybuf, "Resist Fire Potion");
                              displaytime = 360;
                              updatepotion(FIREWALKPOTION);
                              potionpic(3);
                              currentpotion = 3;
                              SND_Sound(S_POTION1);
                              score(15);
                              break;
                         case 4:
                              if (!potionspace(4)) {
                                   break;
                              }
                              strcpy(displaybuf, "Invisibility Potion");
                              displaytime = 360;
                              updatepotion(INVISIBLEPOTION);
                              potionpic(4);
                              currentpotion = 4;
                              SND_Sound(S_POTION1);
                              score(30);
                              break;
                         }
                         spr->picnum = OPENCHEST;
                         break;
                    case 1:
                         switch (rand() % 8) {
                         case 0:
                              if (plr->orbammo[0] < 10) {
                                   plr->orb[0] = 1;
                                   plr->orbammo[0]++;
                                   strcpy(displaybuf, "Scare Scroll");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                              }
                              break;
                         case 1:
                              if (plr->orbammo[1] < 10) {
                                   plr->orb[1] = 1;
                                   plr->orbammo[1]++;
                                   strcpy(displaybuf, "Night Vision Scroll");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                              }
                              break;
                         case 2:
                              if (plr->orbammo[2] < 10) {
                                   plr->orb[2] = 1;
                                   plr->orbammo[2]++;
                                   strcpy(displaybuf, "Freeze Scroll");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                              }
                              break;
                         case 3:
                              if (plr->orbammo[3] < 10) {
                                   plr->orb[3] = 1;
                                   plr->orbammo[3]++;
                                   strcpy(displaybuf, "Magic Arrow Scroll");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                              }
                              break;
                         case 4:
                              if (plr->orbammo[4] < 10) {
                                   plr->orb[4] = 1;
                                   plr->orbammo[4]++;
                                   strcpy(displaybuf, "Open Door Scroll");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                              }
                              break;
                         case 5:
                              if (plr->orbammo[5] < 10) {
                                   plr->orb[5] = 1;
                                   plr->orbammo[5]++;
                                   strcpy(displaybuf, "Fly Scroll");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                              }
                              break;
                         case 6:
                              if (plr->orbammo[6] < 10) {
                                   plr->orb[6] = 1;
                                   plr->orbammo[6]++;
                                   strcpy(displaybuf, "Fireball Scroll");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                              }
                              break;
                         case 7:
                              if (plr->orbammo[7] < 10) {
                                   plr->orb[7] = 1;
                                   plr->orbammo[7]++;
                                   strcpy(displaybuf, "Nuke Scroll");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                              }
                              break;
                         }
                         spr->picnum = OPENCHEST;
                         break;
                    case 2:
                         spr->picnum = OPENCHEST;
                         score((rand() % 400) + 100);
                         strcpy(displaybuf, "Treasure Chest");
                         displaytime = 360;
                         SND_Sound(S_POTION1);
                         break;
                    case 3:
                         switch ((rand() % 5) + 1) {
                         case 1:
                              if (plr->ammo[1] < 12) {
                                   plr->weapon[1] = 1;
                                   plr->ammo[1] = 40;
                                   strcpy(displaybuf, "Dagger");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                                   autoweaponchange(1);
                                   score(10);
                              }
                              break;
                         case 2:
                              if (plr->ammo[3] < 12) {
                                   plr->weapon[3] = 1;
                                   plr->ammo[3] = 55;
                                   strcpy(displaybuf, "Morning Star");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                                   autoweaponchange(3);
                                   score(20);
                              }
                              break;
                         case 3:
                              if (plr->ammo[2] < 12) {
                                   plr->weapon[2] = 1;
                                   plr->ammo[2] = 30;
                                   strcpy(displaybuf, "Short Sword");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                                   autoweaponchange(2);
                                   score(10);
                              }
                              break;
                         case 4:
                              if (plr->ammo[5] < 12) {
                                   plr->weapon[5] = 1;
                                   plr->ammo[5] = 100;
                                   strcpy(displaybuf, "Battle axe");
                                   displaytime = 360;
                                   SND_Sound(S_POTION1);
                                   autoweaponchange(5);
                                   score(30);
                              }
                              break;
                         case 5:
                              if (plr->weapon[7] == 1) {
                                   plr->weapon[7] = 2;
                                   plr->ammo[7] = 1;
                                   strcpy(displaybuf, "Pike axe");
                                   displaytime = 360;
                                   deletesprite((short) i);
                                   SND_Sound(S_POTION1);
                                   score(30);
                              }
                              if (plr->weapon[7] == 2) {
                                   plr->weapon[7] = 2;
                                   plr->ammo[7]++;
                                   strcpy(displaybuf, "Pike axe");
                                   displaytime = 360;
                                   deletesprite((short) i);
                                   SND_Sound(S_POTION1);
                                   score(30);
                              }
                              if (plr->weapon[7] < 1) {
                                   if (plr->ammo[7] < 12) {
                                        plr->weapon[7] = 1;
                                        plr->ammo[7] = 30;
                                        strcpy(displaybuf, "Pike axe");
                                        displaytime = 360;
                                        deletesprite((short) i);
                                        SND_Sound(S_POTION1);
                                        autoweaponchange(7);
                                        score(30);
                                   }
                              }
                              break;
                         }
                         spr->picnum = OPENCHEST;
                         break;
                    case 4:
                         // random armor
                         switch (krand() & 4) {
                         case 0:
                              strcpy(displaybuf, "Hero Time");
                              displaytime = 360;
                              armorpic(10);
                              helmettime = 7200;
                              SND_Sound(S_STING1 + rand() % 2);
                              break;
                         case 1:
                              if (plr->armor <= 149) {
                                   strcpy(displaybuf, "Plate Armor");
                                   displaytime = 360;
                                   plr->armortype = 3;
                                   plr->armor = 0;
                                   armorpic(150);
                                   SND_Sound(S_POTION1);
                                   score(40);
                              }
                              break;
                         case 2:
                              if (plr->armor <= 99) {
                                   strcpy(displaybuf, "Chain Mail");
                                   displaytime = 360;
                                   plr->armortype = 2;
                                   plr->armor = 0;
                                   armorpic(100);
                                   SND_Sound(S_POTION1);
                                   score(20);
                              }
                              break;
                         case 3:
                              if (plr->armor <= 49) {
                                   strcpy(displaybuf, "Leather Armor");
                                   displaytime = 360;
                                   plr->armortype = 1;
                                   plr->armor = 0;
                                   armorpic(50);
                                   SND_Sound(S_POTION1);
                                   score(20);
                              }
                              break;
                         }
                         spr->picnum = OPENCHEST;
                         break;
                    case 5:
                         // poison chest
                         if (krand() & 2 == 0) {
                              poisoned = 1;
                              poisontime = 7200;
                              healthpic(-10);
                              strcpy(displaybuf, "Poisoned Chest");
                              displaytime = 360;
                         }
                         else {
                              deletesprite((short) i);
                              score((krand() & 400) + 100);
                              strcpy(displaybuf, "Treasure Chest");
                              displaytime = 360;
                              SND_Sound(S_POTION1);
                         }
                         break;
                    case 6:
                         explosion(i, spr->x, spr->y, spr->z, spr->owner);
                         playsound_loc(S_EXPLODE, spr->x, spr->y);
                         deletesprite((short) i);
                         break;
                    default:
                         spr->picnum = OPENCHEST;
                         score((rand() % 400) + 100);
                         strcpy(displaybuf, "Experience Gained");
                         displaytime = 360;
                         SND_Sound(S_POTION1);
                         break;
                    }
                    break;

               case FLASKBLUE:
                    if (!potionspace(0))
                         break;
                    strcpy(displaybuf, "Health Potion");
                    displaytime = 360;
                    updatepotion(HEALTHPOTION);
                    potionpic(0);
                    currentpotion = 0;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(10);
                    treasuresfound++;
                    break;
               case FLASKRED:
                    if (!potionspace(3))
                         break;
                    strcpy(displaybuf, "Resist Fire Potion");
                    displaytime = 360;
                    updatepotion(FIREWALKPOTION);
                    potionpic(3);
                    currentpotion = 3;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(20);
                    treasuresfound++;
                    break;
               case FLASKGREEN:
                    if (!potionspace(1))
                         break;
                    strcpy(displaybuf, "Strength Potion");
                    displaytime = 360;
                    updatepotion(STRENGTHPOTION);
                    potionpic(1);
                    currentpotion = 1;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(15);
                    treasuresfound++;
                    break;
               case FLASKOCHRE:
                    if (!potionspace(2))
                         break;
                    strcpy(displaybuf, "Cure Poison Potion");
                    displaytime = 360;
                    updatepotion(ARMORPOTION);
                    potionpic(2);
                    currentpotion = 2;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(15);
                    treasuresfound++;
                    break;
               case FLASKTAN:
                    if (!potionspace(4))
                         break;
                    strcpy(displaybuf, "Invisibility Potion");
                    displaytime = 360;
                    updatepotion(INVISIBLEPOTION);
                    potionpic(4);
                    currentpotion = 4;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(30);
                    treasuresfound++;
                    break;
               case DIAMONDRING:
                    plr->treasure[0] = 1;
                    strcpy(displaybuf, "DIAMOND RING");
                    displaytime = 360;
                    plr->armor = 0;
                    armorpic(200);
                    plr->armortype = 3;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(25);
                    treasuresfound++;
                    break;
               case SHADOWAMULET:
                    plr->treasure[1] = 1;
                    strcpy(displaybuf, "SHADOW AMULET");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    shadowtime = 7500;
                    score(50);
                    treasuresfound++;
                    break;
               case GLASSSKULL:
                    plr->treasure[2] = 1;
                    strcpy(displaybuf, "GLASS SKULL");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    switch (plr->lvl) {
                    case 1:
                         plr->score = 2300;
                         break;
                    case 2:
                         plr->score = 4550;
                         break;
                    case 3:
                         plr->score = 9050;
                         break;
                    case 4:
                         plr->score = 18050;
                         break;
                    case 5:
                         plr->score = 36050;
                         break;
                    case 6:
                         plr->score = 75050;
                         break;
                    case 7:
                         plr->score = 180500;
                         break;
                    case 8:
                         plr->score = 280500;
                         break;
                    }
                    score(10);
                    treasuresfound++;
                    break;
               case AHNK:
                    plr->treasure[3] = 1;
                    strcpy(displaybuf, "ANKH");
                    displaytime = 360;
                    plr->health = 0;
                    healthpic(250);
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(100);
                    treasuresfound++;
                    break;
               case BLUESCEPTER:
                    plr->treasure[4] = 1;
                    strcpy(displaybuf, "Water walk scepter");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(10);
                    treasuresfound++;
                    break;
               case YELLOWSCEPTER:
                    plr->treasure[5] = 1;
                    strcpy(displaybuf, "Fire walk scepter");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(10);
                    treasuresfound++;
                    break;
               case ADAMANTINERING:
                    // ring or protection +5
                    plr->treasure[6] = 1;
                    strcpy(displaybuf, "ADAMANTINE RING");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(30);
                    treasuresfound++;
                    break;
               case ONYXRING:
                    // protection from missile
                    // anit-missile for level only
                    // dont forget to cleanup values
                    plr->treasure[7] = 1;
                    strcpy(displaybuf, "ONYX RING");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(35);
                    treasuresfound++;
                    break;
               case PENTAGRAM:
                    if (sector[plr->sector].lotag == 4002)
                         break;
                    else {
                         plr->treasure[8] = 1;
                         strcpy(displaybuf, "PENTAGRAM");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                    }
                    score(100);
                    treasuresfound++;
                    break;
               case CRYSTALSTAFF:
                    plr->treasure[9] = 1;
                    strcpy(displaybuf, "CRYSTAL STAFF");
                    displaytime = 360;
                    plr->health = 0;
                    healthpic(250);
                    plr->armortype = 2;
                    plr->armor = 0;
                    armorpic(300);
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(150);
                    treasuresfound++;
                    break;
               case AMULETOFTHEMIST:
                    plr->treasure[10] = 1;
                    strcpy(displaybuf, "AMULET OF THE MIST");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    invisibletime = 3200;
                    score(75);
                    treasuresfound++;
                    break;
               case THEHORN:
                    plr->treasure[12] = 1;
                    strcpy(displaybuf, "Ornate Horn");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    vampiretime = 7200;
                    // gain 5-10 hp when you kill something
                    // for 60 seconds
                    score(350);
                    treasuresfound++;
                    break;
               case SAPHIRERING:
                    plr->treasure[13] = 1;
                    strcpy(displaybuf, "SAPPHIRE RING");
                    displaytime = 360;
                    plr->armortype = 3;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    score(25);
                    treasuresfound++;
                    break;
               case BRASSKEY:
                    plr->treasure[14] = 1;
                    strcpy(displaybuf, "BRASS KEY");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    keyspic();
                    score(15);
                    treasuresfound++;
                    break;
               case BLACKKEY:
                    plr->treasure[15] = 1;
                    strcpy(displaybuf, "BLACK KEY");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    keyspic();
                    score(15);
                    treasuresfound++;
                    break;
               case GLASSKEY:
                    plr->treasure[16] = 1;
                    strcpy(displaybuf, "GLASS KEY");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    keyspic();
                    score(15);
                    treasuresfound++;
                    break;
               case IVORYKEY:
                    plr->treasure[17] = 1;
                    strcpy(displaybuf, "IVORY KEY");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    keyspic();
                    score(15);
                    treasuresfound++;
                    break;
               case SCROLLSCARE:
                    if (plr->orbammo[0] < 10) {
                         plr->orb[0] = 1;
                         plr->orbammo[0]+=5;
                         if (plr->orbammo[0] >= 10) {
                              plr->orbammo[0] = 9;
                         }
                         strcpy(displaybuf, "Scare Scroll");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         treasuresfound++;
                         keystatus[0x3B] = 1;
                    }
                    break;
               case SCROLLNIGHT:
                    if (plr->orbammo[1] < 10) {
                         plr->orb[1] = 1;
                         plr->orbammo[1] += 5;
                         if (plr->orbammo[1] >= 10) {
                              plr->orbammo[1] = 9;
                         }
                         strcpy(displaybuf, "Night Vision Scroll");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         treasuresfound++;
                         keystatus[0x3B+1] = 1;
                    }
                    break;
               case SCROLLFREEZE:
                    if (plr->orbammo[2] < 10) {
                         plr->orb[2] = 1;
                         plr->orbammo[2] += 5;
                         if (plr->orbammo[2] >= 10) {
                              plr->orbammo[2] = 9;
                         }
                         strcpy(displaybuf, "Freeze Scroll");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         treasuresfound++;
                         keystatus[0x3B+2] = 1;
                    }
                    break;
               case SCROLLMAGIC:
                    if (plr->orbammo[3] < 10) {
                         plr->orb[3] = 1;
                         plr->orbammo[3] += 5;
                         if (plr->orbammo[3] >= 10) {
                              plr->orbammo[3] = 9;
                         }
                         strcpy(displaybuf, "Magic Arrow Scroll");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         treasuresfound++;
                         keystatus[0x3B+3] = 1;
                    }
                    break;
               case SCROLLOPEN:
                    if (plr->orbammo[4] < 10) {
                         plr->orb[4] = 1;
                         plr->orbammo[4] += 5;
                         if (plr->orbammo[4] >= 10) {
                              plr->orbammo[4] = 9;
                         }
                         strcpy(displaybuf, "Open Door Scroll");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         treasuresfound++;
                         keystatus[0x3B+4] = 1;
                    }
                    break;
               case SCROLLFLY:
                    if (plr->orbammo[5] < 10) {
                         plr->orb[5] = 1;
                         plr->orbammo[5] += 5;
                         if (plr->orbammo[5] >= 10) {
                              plr->orbammo[5] = 9;
                         }
                         strcpy(displaybuf, "Fly Scroll");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         treasuresfound++;
                         keystatus[0x3B+5] = 1;
                    }
                    break;
               case SCROLLFIREBALL:
                    if (plr->orbammo[6] < 10) {
                         plr->orb[6] = 1;
                         plr->orbammo[6] += 5;
                         if (plr->orbammo[6] >= 10) {
                              plr->orbammo[6] = 9;
                         }
                         strcpy(displaybuf, "Fireball Scroll");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         treasuresfound++;
                         keystatus[0x3B+6] = 1;
                    }
                    break;
               case SCROLLNUKE:
                    if (plr->orbammo[7] < 10) {
                         plr->orb[7] = 1;
                         plr->orbammo[7] += 5;
                         if (plr->orbammo[7] >= 10) {
                              plr->orbammo[7] = 9;
                         }
                         strcpy(displaybuf, "Nuke Scroll");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         treasuresfound++;
                         keystatus[0x3B+7] = 1;
                    }
                    break;
               case QUIVER:
                    if (plr->ammo[6] < 100) {
                         plr->ammo[6] += 25;
                         if (plr->ammo[6] > 100)
                              plr->ammo[6] = 100;
                         strcpy(displaybuf, "Quiver of arrows");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         score(10);
                         levelpic();
                    }
                    break;
               case WALLBOW:
               case BOW:
                    plr->weapon[6] = 1;
                    plr->ammo[6] += 10;
                    if (plr->ammo[6] > 100)
                         plr->ammo[6] = 100;
                    strcpy(displaybuf, "long bow");
                    displaytime = 360;
                    deletesprite((short) i);
                    SND_Sound(S_POTION1);
                    autoweaponchange(6);
                    score(10);
                    break;
               case WEAPON1:
                    if (plr->ammo[1] < 12) {
                         plr->weapon[1] = 1;
                         plr->ammo[1] = 40;
                         strcpy(displaybuf, "Dagger");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         autoweaponchange(1);
                         score(10);
                    }
                    break;
               case WEAPON1A:
                    if (plr->ammo[1] < 12) {
                         plr->weapon[1] = 3;
                         plr->ammo[1] = 80;
                         strcpy(displaybuf, "Jeweled Dagger");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         autoweaponchange(1);
                         score(30);
                    }
                    break;
               case GOBWEAPON:
                    if (plr->ammo[2] < 12) {
                         plr->weapon[2] = 1;
                         plr->ammo[2] = 20;
                         strcpy(displaybuf, "Short sword");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         autoweaponchange(2);
                         score(10);
                    }
                    break;
               case WEAPON2:
                    if (plr->ammo[3] < 12) {
                         plr->weapon[3] = 1;
                         plr->ammo[3] = 55;
                         strcpy(displaybuf, "Morning Star");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         autoweaponchange(3);
                         score(20);
                    }
                    break;
               case WALLSWORD:
               case WEAPON3A:
                    if (plr->ammo[4] < 12) {
                         plr->weapon[4] = 1;
                         plr->ammo[4] = 160;
                         strcpy(displaybuf, "Broad Sword");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         autoweaponchange(4);
                         score(60);
                    }
               case WEAPON3:
                    if (plr->ammo[4] < 12) {
                         plr->weapon[4] = 1;
                         plr->ammo[4] = 80;
                         strcpy(displaybuf, "Broad Sword");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         autoweaponchange(4);
                         score(30);
                    }
                    break;
               case WALLAXE:
               case WEAPON4:
                    if (plr->ammo[5] < 12) {
                         plr->weapon[5] = 1;
                         plr->ammo[5] = 100;
                         strcpy(displaybuf, "Battle axe");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         autoweaponchange(5);
                         score(30);
                    }
                    break;
               case THROWHALBERD:
               case WEAPON5:
                    if (plr->ammo[9] < 12) {
                         plr->weapon[9] = 1;
                         plr->ammo[9] = 30;
                         strcpy(displaybuf, "Halberd");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         autoweaponchange(9);
                         score(30);
                    }
                    break;
               case GONZOBSHIELD:
               case GONZOCSHIELD:
               case GONZOGSHIELD:
                    if (shieldpoints < 100) {
                         shieldtype = 2;
                         shieldpoints = 200;
                         droptheshield = 0;
                         deletesprite((short) i);
                         strcpy(displaybuf, "Magic Shield");
                         displaytime = 360;
                         SND_Sound(S_POTION1);
                         score(50);
                    }
                    break;
               case SHIELD:
                    if (shieldpoints < 100) {
                         shieldtype = 1;
                         shieldpoints = 100;
                         droptheshield = 0;
                         deletesprite((short) i);
                         strcpy(displaybuf, "Shield");
                         displaytime = 360;
                         SND_Sound(S_POTION1);
                         score(10);
                    }
                    break;
               case WEAPON5B:
                    if (plr->ammo[9] < 12) {
                         deletesprite((short) i);
                    }
                    break;
               case WALLPIKE:
               case THROWPIKE + 1:
                    if (plr->weapon[7] == 1) {
                         plr->weapon[7] = 2;
                         plr->ammo[7] = 2;
                         strcpy(displaybuf, "Pike axe");
                         displaytime = 360;
                         deletesprite((short) i);
                         if ((rand()%10) > 6)
                              SND_Sound(S_PICKUPAXE);
                         score(30);
                         levelpic();
                    }
                    if (plr->weapon[7] == 2) {
                         plr->weapon[7] = 2;
                         plr->ammo[7]++;
                         strcpy(displaybuf, "Pike axe");
                         displaytime = 360;
                         deletesprite((short) i);
                         if ((rand()%10) > 6)
                              SND_Sound(S_PICKUPAXE);
                         // score(30);
                         levelpic();
                    }
                    if (plr->weapon[7] < 1) {
                         if (plr->ammo[7] < 12) {
                              plr->weapon[7] = 1;
                              plr->ammo[7] = 30;
                              strcpy(displaybuf, "Pike axe");
                              displaytime = 360;
                              deletesprite((short) i);
                              SND_Sound(S_POTION1);
                              autoweaponchange(7);
                              score(30);
                         }
                    }

                    break;
               case WEAPON6:
                    if (plr->weapon[7] == 1) {
                         plr->weapon[7] = 2;
                         plr->ammo[7] = 25;
                         strcpy(displaybuf, "Pike axe");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         score(30);
                         levelpic();
                    }
                    if (plr->weapon[7] == 2) {
                         plr->weapon[7] = 2;
                         plr->ammo[7] += 25;
                         strcpy(displaybuf, "Pike axe");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         // score(30);
                         levelpic();
                    }
                    if (plr->weapon[7] < 1) {
                         if (plr->ammo[7] < 12) {
                              plr->weapon[7] = 2;
                              plr->ammo[7] = 25;
                              strcpy(displaybuf, "Pike axe");
                              displaytime = 360;
                              deletesprite((short) i);
                              SND_Sound(S_POTION1);
                              autoweaponchange(7);
                              score(30);
                         }
                    }
                    break;
                    // case WEAPON7:
               case WEAPON8:
                    if (plr->ammo[8] < 12) {
                         plr->weapon[8] = 1;
                         plr->ammo[8] = 250;
                         strcpy(displaybuf, "Two Handed Sword");
                         displaytime = 360;
                         deletesprite((short) i);
                         SND_Sound(S_POTION1);
                         autoweaponchange(8);
                         score(30);
                    }
                    break;
               case SPIKEBLADE:
                    if (!godmode)
                         if (invincibletime <= 0) {
                              spikeme = 1;
                         }
                    break;
               case SPIKE:
                    if (!godmode)
                         if (invincibletime <= 0) {
                              spikeme = 1;
                         }
                    break;
               case SPIKEPOLE:
                    if (!godmode)
                         if (invincibletime <= 0) {
                              spikeme = 1;
                         }
                    break;
               case MONSTERBALL:
                    if (sprite[i].owner != 4096)
                         if (manatime < 1)
                              healthpic(-1);
                    break;
               case HORNEDSKULL:
                    fadeout(0,255,0,0,0,64);           // JEFF
                    victory(0);
               }
               if (sprite[i].picnum >= EXPLOSTART
                   && sprite[i].picnum <= EXPLOEND
                   && sprite[i].owner != 4096)
                    if (manatime < 1)
                         healthpic(-1);

          }
          i = nexti;
     }

}

int
potionspace(int vial)
{

     struct player *plr;

     plr = &player[0];

     if (plr->potion[vial] > 9)
          return (0);
     else
          return (1);
}

void
updatepotion(int vial)
{

     struct player *plr;

     plr = &player[0];

     switch (vial) {
     case HEALTHPOTION:
          plr->potion[0]++;
          break;
     case STRENGTHPOTION:
          plr->potion[1]++;
          break;
     case ARMORPOTION:
          plr->potion[2]++;
          break;
     case FIREWALKPOTION:
          plr->potion[3]++;
          break;
     case INVISIBLEPOTION:
          plr->potion[4]++;
          break;
     }
     potionpic(currentpotion);

}

extern
int  debuganalyzespritesflag;

int  tspritelistcnt;

spritetype tspritelist[MAXSPRITESONSCREEN];

void
transformactors(struct player * plr)
{

     long i,
          j,
          k;
     point3d *ospr;
     spritetype *tspr;

     tspritelistcnt=spritesortcnt;
     for (i = 0, tspr = &tsprite[0]; i < spritesortcnt; i++, tspr++) {
          memmove(&tspritelist[i],tspr,sizeof(spritetype));
          switch (tspr->picnum) {
          case GRONHAL:
          case GRONSW:
          case GRONSWATTACK:
          case GRONMU:
          case DEVILSTAND:
          case DEVIL:
          case DEVILATTACK:
          case MINOTAUR:
          case MINOTAURATTACK:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k << 2);
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) << 2);
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               break;
          case SKELETON:
          case IMP:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k * 6);
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) * 6);
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               break;
          case KURTAT:
          case KOBOLD:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k * 5);
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) * 5);
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               break;
          case KATIE:
          case KATIEAT:

               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k * 5);
                    // tspr->cstat &= ~4;   //clear x-flipping bit
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) * 5);
                    // tspr->cstat |= 4;    //set x-flipping bit
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               break;

          case NEWGUY:
          case NEWGUYBOW:
          case NEWGUYMACE:

          case GONZOCSW:
          case GONZOCSWAT:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k * 6);
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) * 6);
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               break;

               break;
          case GONZOGSW:
          case GONZOGHM:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k * 6);
                    // tspr->cstat &= ~4;   //clear x-flipping bit
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) * 6);
                    // tspr->cstat |= 4;    //set x-flipping bit
                    tspr->cstat &= ~4;  // clear x-flipping bit

               }
               break;

          case GONZOGSH:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;

               tspr->picnum += (k * 6);
               tspr->cstat |= 4;   // set x-flipping bit
               break;


          case GRONMUATTACK:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k * 6);
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) * 6);
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               break;

          case GRONHALATTACK:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k * 7);
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) * 7);
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               break;
          case NEWGUYCAST:
          case NEWGUYPUNCH:
          case KURTPUNCH:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k * 3);
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) * 3);
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               break;
          case RAT:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += (k * 2);
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               else {
                    tspr->picnum += ((8 - k) * 2);
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               break;
          case NEWGUYSTAND:
          case NEWGUYKNEE:
          case KURTSTAND:
          case KURTKNEE:
          case GUARDIAN:
               k = getangle(tspr->x - plr->x, tspr->y - plr->y);
               k = (((tspr->ang + 3072 + 128 - k) & 2047) >> 8) & 7;
               if (k <= 4) {
                    tspr->picnum += k;
                    tspr->cstat &= ~4;  // clear x-flipping bit
               }
               else {
                    tspr->picnum += (8 - k);
                    tspr->cstat |= 4;   // set x-flipping bit
               }
               break;
          }
          if (debuganalyzespritesflag) {
               sprintf(tempbuf,"%d: K=%d PIC=%04d CSTAT=%04X SHADE=%02d "
                              "SECT=%05d STAT=%04d",i,k,tspr->picnum,
                              tspr->cstat,tspr->shade,tspr->sectnum,
                              tspr->statnum);
               printext256(0L,(i*8L),31,-1,tempbuf,1);
          }
     }
}

void
newstatus(short sn, int seq)
{

     struct player *plr;
     spritetype *spr;
     int  j;
     long k;

     spr = &sprite[sn];

     switch (seq) {
     case CHILL:
          if (netgame) {
               break;
          }
          sprite[sn].lotag = 60;
          changespritestat(sn, CHILL);
          break;
     case AMBUSH:
          if (netgame) {
               break;
          }
          changespritestat(sn, AMBUSH);
          break;
     case LAND:
          if (netgame) {
               break;
          }
          changespritestat(sn, LAND);
          break;
     case EVILSPIRIT:
          if (netgame) {
               break;
          }
          changespritestat(sn, EVILSPIRIT);
          spr->lotag = 120 + (krand() & 64);
          break;
     case PATROL:
          if (netgame) {
               break;
          }
          changespritestat(sn, PATROL);
          break;
     case WARPFX:
          if (netgame) {
               break;
          }
          changespritestat(sn, WARPFX);
          spr->lotag = 12;
          break;
     case NUKED:
          if (netgame) {
               break;
          }
          changespritestat(sn, NUKED);
          break;
     case BROKENVASE:
          changespritestat(sn, BROKENVASE);
          switch (spr->picnum) {
          case VASEA:
               playsound_loc(S_GLASSBREAK1 + (rand() % 3), spr->x, spr->y);
               spr->picnum = SHATTERVASE;
               break;
          case VASEB:
               playsound_loc(S_GLASSBREAK1 + (rand() % 3), spr->x, spr->y);
               spr->picnum = SHATTERVASE2;
               break;
          case VASEC:
               playsound_loc(S_GLASSBREAK1 + (rand() % 3), spr->x, spr->y);
               spr->picnum = SHATTERVASE3;
               break;
          case STAINGLASS1:
          case STAINGLASS2:
          case STAINGLASS3:
          case STAINGLASS4:
          case STAINGLASS5:
          case STAINGLASS6:
          case STAINGLASS7:
          case STAINGLASS8:
          case STAINGLASS9:
               spr->picnum++;
               SND_Sound(S_BIGGLASSBREAK1 + (rand() % 3));
               break;
          case FBARRELFALL:
          case BARREL:
               playsound_loc(S_BARRELBREAK, spr->x, spr->y);
               spr->picnum = FSHATTERBARREL;
               break;
          }
          spr->lotag = 12;
          spr->cstat &= ~3;
          break;
     case DRAIN:
          if (netgame) {
               break;
          }
          changespritestat(sn, DRAIN);
          spr->lotag = 24;
          spr->pal = 7;
          break;
     case ANIMLEVERDN:
          playsound_loc(S_PULLCHAIN1, spr->x, spr->y);
          spr->picnum = LEVERUP;
          changespritestat(sn, ANIMLEVERDN);
          spr->lotag = 24;
          break;
     case ANIMLEVERUP:
          playsound_loc(S_PULLCHAIN1, spr->x, spr->y);
          spr->picnum = LEVERDOWN;
          changespritestat(sn, ANIMLEVERUP);
          spr->lotag = 24;
          break;
     case SKULLPULLCHAIN1:
     case PULLTHECHAIN:
          playsound_loc(S_PULLCHAIN1, spr->x, spr->y);
          changespritestat(sn, PULLTHECHAIN);
          SND_PlaySound(S_CHAIN1, 0, 0, 0, 0);
          spr->lotag = 24;
          break;
     case FROZEN:
          if (netgame) {
               break;
          }
          playsound_loc(S_FREEZE, spr->x, spr->y);
          changespritestat(sn, FROZEN);
          spr->lotag = 3600;
          break;
     case DEVILFIRE:
          if (netgame) {
               break;
          }
          changespritestat(sn, DEVILFIRE);
          spr->lotag = krand() & 120 + 360;
          break;
     case DRIP:
          if (netgame) {
               break;
          }
          changespritestat(sn, DRIP);
          break;
     case BLOOD:
          if (netgame) {
               break;
          }
          changespritestat(sn, BLOOD);
          break;
     case WAR:
          if (netgame) {
               break;
          }
          changespritestat(sn, WAR);
          break;
     case PAIN:
          spr->lotag = 36;
          switch (spr->picnum) {
          case SKELETON:
          case SKELETONATTACK:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               spr->picnum = SKELETON;
               break;
          case DEMON:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               playsound_loc(S_GUARDIANPAIN1 + (rand() % 2), spr->x, spr->y);
               spr->picnum = DEMON - 1;
               changespritestat(sn, PAIN);
               break;
          case GUARDIAN:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               playsound_loc(S_GUARDIANPAIN1 + (rand() % 2), spr->x, spr->y);
               // needs pain
               spr->picnum = GUARDIAN;
               changespritestat(sn, PAIN);
               break;

          case GONZOBSHPAIN:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               spr->picnum = GONZOBSHPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_GRONPAINA+(rand()%3),spr->x,spr->y);
               break;

          case NEWGUYSTAND:
          case NEWGUYKNEE:
          case NEWGUY:
          case NEWGUYBOW:
          case NEWGUYPUNCH:
          case NEWGUYMACE:
          case NEWGUYCAST:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               spr->picnum = NEWGUYPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_AGM_PAIN1,spr->x,spr->y);
               break;

          case KURTSTAND:
          case KURTKNEE:
          case KURTAT:
          case KURTPUNCH:
          case KURTREADY:

          case GONZOCSW:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               spr->picnum = GONZOCSWPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_GRONPAINA+(rand()%3),spr->x,spr->y);
               break;

          case GONZOGSW:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               spr->picnum = GONZOGSWPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_GRONPAINA+(rand()%3),spr->x,spr->y);
               break;
          case GONZOGHM:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               spr->picnum = GONZOGHMPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_GRONPAINA+(rand()%3),spr->x,spr->y);
               break;
          case GONZOGSH:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               spr->picnum = GONZOGSHPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_GRONPAINA, spr->x, spr->y);
               break;

          case GRONHALPAIN:
          case GRONHAL:
          case GRONHALATTACK:
               spr->lotag = 24;
               spr->picnum = GRONHALPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_GRONPAINA, spr->x, spr->y);
               break;
          case GRONSWPAIN:
          case GRONSW:
          case GRONSWATTACK:
               spr->lotag = 24;
               spr->picnum = GRONSWPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_GRONPAINA+(rand()%3),spr->x,spr->y);
               break;
          case GRONMUPAIN:
          case GRONMU:
          case GRONMUATTACK:
               spr->lotag = 24;
               spr->picnum = GRONMUPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_GRONPAINA, spr->x, spr->y);
               break;
          case KOBOLD:
          case KOBOLDATTACK:
               if (netgame) {
                    break;
               }
               spr->picnum = KOBOLDDIE;
               changespritestat(sn, PAIN);
               playsound_loc(S_KPAIN1 + (rand() % 2), spr->x, spr->y);
               break;
          case KATIE:
          case KATIEAT:
               if (netgame) {
                    break;
               }
//               playsound_loc(S_MPAIN1, spr->x, spr->y);
               spr->picnum = KATIEPAIN;
               changespritestat(sn, PAIN);
               break;
          case DEVILSTAND:
          case DEVIL:
          case DEVILATTACK:
               if (netgame) {
                    break;
               }
               playsound_loc(S_MPAIN1, spr->x, spr->y);
               spr->picnum = DEVILPAIN;
               changespritestat(sn, PAIN);
               break;
          case IMP:
          case IMPATTACK:
               if (netgame) {
                    break;
               }
               spr->lotag = 24;
               spr->picnum = IMPPAIN;
               changespritestat(sn, PAIN);
// BIKINI
//             playsound_loc(S_GOBPAIN1+(rand()%2),spr->x,spr->y);

               break;
          case MINOTAUR:
          case MINOTAURATTACK:
          case MINOTAURPAIN:
               if (netgame) {
                    break;
               }
               spr->picnum = MINOTAURPAIN;
               changespritestat(sn, PAIN);
               playsound_loc(S_MPAIN1, spr->x, spr->y);
               break;
          default:
               if (netgame) {
                    break;
               }
               changespritestat(sn, FLEE);
               break;
          }
          break;
     case FLOCKSPAWN:
          if (netgame) {
               break;
          }
          spr->lotag = 36;
          spr->extra = 10;
          changespritestat(sn, FLOCKSPAWN);
          break;
     case FLOCK:
          if (netgame) {
               break;
          }
          spr->lotag = 128;
          spr->extra = 0;
          spr->pal = 0;
          changespritestat(sn, FLOCK);
          break;
     case FINDME:
          if (netgame) {
               break;
          }
          spr->lotag = 360;
          if (spr->picnum == RAT) {
               spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;     // NEW
               changespritestat(sn, FLEE);
          }
          else {
               changespritestat(sn, FINDME);
          }
          break;
     case SKIRMISH:
          if (netgame) {
               break;
          }
          spr->lotag = 60;
          if (spr->picnum == RAT) {
               spr->ang = ((krand() & 512 - 256) + spr->ang + 1024) & 2047;     // NEW
               changespritestat(sn, FLEE);
          }
          else {
               changespritestat(sn, SKIRMISH);
          }
          break;
     case WITCHSIT:
          if (netgame) {
               break;
          }
          spr->lotag = 12;
          changespritestat(sn, WITCHSIT);
          break;
     case FLEE:
          if (netgame) {
               break;
          }
          switch (spr->picnum) {
          case NEWGUYSTAND:
          case NEWGUYKNEE:
               spr->picnum = NEWGUY;
               break;
          case KURTSTAND:
          case KURTKNEE:
               spr->picnum = GONZOCSW;
               break;
          }
          changespritestat(sn, FLEE);
          spr->lotag = 60;
          break;
     case BOB:
          changespritestat(sn, BOB);
          break;
     case LIFTUP:
//JSA_DEMO3
          if (cartsnd == -1) {
               playsound_loc(S_CLUNK, spr->x, spr->y);
               cartsnd = SND_PlaySound(S_CHAIN1, spr->x, spr->y, 0, 5);
          }

          changespritestat(sn, LIFTUP);
          break;
     case LIFTDN:
//JSA_DEMO3
          if (cartsnd == -1) {
               playsound_loc(S_CLUNK, spr->x, spr->y);
               cartsnd = SND_PlaySound(S_CHAIN1, spr->x, spr->y, 0, 5);
          }

          changespritestat(sn, LIFTDN);
          break;
     case SHOVE:
          spr->lotag = 128;
          changespritestat(sn, SHOVE);
          break;
     case SHATTER:
          changespritestat(sn, SHATTER);
          switch (spr->picnum) {
          case FBARRELFALL:
               spr->picnum = FSHATTERBARREL;
               break;
          }
          break;
     case YELL:
          if (netgame) {
               break;
          }
          changespritestat(sn, YELL);
          spr->lotag = 12;
          break;
     case ATTACK:
          if (netgame) {
               break;
          }
          spr->lotag = 64;
          changespritestat(sn, ATTACK);
          switch (spr->picnum) {
          case NEWGUY:
               if (spr->extra > 20) {
                    spr->picnum = NEWGUYCAST;
                    spr->lotag = 24;
               }
               else if (spr->extra > 10)
                    spr->picnum = NEWGUYBOW;
               else if (spr->extra > 0)
                    spr->picnum = NEWGUYMACE;
               else
                    spr->picnum = NEWGUYPUNCH;
               break;
          case GONZOCSW:
               if (spr->extra > 10)
                    spr->picnum = GONZOCSWAT;
               else if (spr->extra > 0) {
                    spr->picnum = KURTREADY;
                    spr->lotag = 12;
               }
               else
                    spr->picnum = KURTPUNCH;
               break;
          case GONZOGSW:
               spr->picnum = GONZOGSWAT;
               break;
          case GONZOGHM:
               spr->picnum = GONZOGHMAT;
               break;
          case GONZOGSH:
               spr->picnum = GONZOGSHAT;
               break;

          case GRONHAL:
               spr->picnum = GRONHALATTACK;
               break;
          case GRONMU:
               spr->picnum = GRONMUATTACK;
               break;
          case GRONSW:
               spr->picnum = GRONSWATTACK;
               break;
          case KOBOLD:
               spr->picnum = KOBOLDATTACK;
               if (rand() % 10 > 4)
                    playsound_loc(S_KSNARL1 + (rand() % 4), spr->x, spr->y);
               break;
          case KATIE:
               if ((rand() % 10) > 4) {
                    playsound_loc(S_JUDY1, spr->x, spr->y);
               }
               spr->picnum = KATIEAT;
               break;
          case DEVILSTAND:
          case DEVIL:
               if ((rand() % 10) > 4) {
                    playsound_loc(S_DEMON1 + (rand() % 5), spr->x, spr->y);
               }
               spr->picnum = DEVILATTACK;
               break;
          case SKELETON:
               spr->picnum = SKELETONATTACK;
               break;
          case IMP:
               if ((rand() % 10) > 5)
                    playsound_loc(S_IMPGROWL1+(rand()%3),spr->x,spr->y);
               spr->picnum = IMPATTACK;
               break;
          case MINOTAUR:
               if ((rand() % 10) > 4)
                    playsound_loc(S_MSNARL1 + (rand() % 3), spr->x, spr->y);
               spr->picnum = MINOTAURATTACK;
               break;
          case WILLOW:
               playsound_loc(S_WISP + (rand() % 2), spr->x, spr->y);
               spr->pal = 7;
               break;
          case DEMON:
               playsound_loc(S_GUARDIAN1 + (rand() % 2), spr->x, spr->y);
               spr->picnum = DEMON;
               break;

          case GUARDIAN:
               playsound_loc(S_GUARDIAN1 + (rand() % 2), spr->x, spr->y);
               spr->picnum = GUARDIANATTACK;
               break;
          }
          break;
     case FACE:
          if (netgame) {
               break;
          }
          changespritestat(sn, FACE);
          break;
     case STAND:
          if (netgame) {
               break;
          }
          // changespritestat(sn,STAND);
          changespritestat(sn, FACE);
          spr->lotag = 0;
          break;
     case CHASE:
          if (netgame) {
               break;
          }
          if (spr->picnum == RAT)
               changespritestat(sn, FLEE);
          else
               changespritestat(sn, CHASE);
          spr->lotag = 256;
          switch (spr->picnum) {

          case NEWGUY:
          case NEWGUYSTAND:
          case NEWGUYKNEE:
          case NEWGUYBOW:
          case NEWGUYMACE:
          case NEWGUYPUNCH:
          case NEWGUYCAST:
               spr->picnum = NEWGUY;
               break;

          case KURTSTAND:
          case KURTKNEE:
          case KURTAT:
          case KURTPUNCH:
          case GONZOCSWAT:
               spr->picnum = GONZOCSW;
               break;
          case GONZOGSWAT:
               spr->picnum = GONZOGSW;
               break;
          case GONZOGHMAT:
               spr->picnum = GONZOGHM;
               break;
          case GONZOGSHAT:
               spr->picnum = GONZOGSH;
               break;



          case GRONHALATTACK:
               if (spr->extra > 2)
                    spr->picnum = GRONHAL;
               else
                    spr->picnum = GRONMU;
               break;
          case GRONSWATTACK:
               spr->picnum = GRONSW;
               break;
          case GRONMUATTACK:
               if (spr->extra > 0)
                    spr->picnum = GRONMU;
               else
                    spr->picnum = GRONSW;
               break;
          case KOBOLDATTACK:
               spr->picnum = KOBOLD;
               break;
          case KATIEAT:
               spr->picnum = KATIE;
               break;
          case DEVILATTACK:
               spr->picnum = DEVIL;
               break;
          case SKELETONATTACK:
               spr->picnum = SKELETON;
               break;
          case IMPATTACK:
               if (rand() % 10 > 2)
// BIKINI
//                        playsound_loc(S_GOBLIN1+(krand()%3),spr->x,spr->y);
                    spr->picnum = IMP;
               break;
          case MINOTAURATTACK:
//JSA_DEMO3
               playsound_loc(S_MSNARL1 + (rand() % 4), spr->x, spr->y);
               spr->picnum = MINOTAUR;
               break;
          case DEMON:
               spr->picnum = DEMON;
               break;
          case GUARDIANATTACK:
               spr->picnum = GUARDIAN;
               break;
          case WILLOW:
               spr->pal = 6;
               break;
          }
          break;
     case MISSILE:
          changespritestat(sn, MISSILE);
          break;
     case CAST:
          changespritestat(sn, CAST);
          spr->lotag = 12;
          switch (spr->picnum) {
          case GRONHALATTACK:
               spr->lotag = 24;
               break;
          case GRONMUATTACK:
               spr->lotag = 36;
               break;
          case GONZOCSWAT:
               spr->lotag = 24;
               break;
          case NEWGUY:
               spr->lotag = 24;
               break;
          }
          break;
     case FX:
          changespritestat(sn, FX);
          break;
     case DIE:
          spr->cstat &= ~3;
          switch (spr->picnum) {

          case GONZOBSHPAIN:
               if (netgame) {
                    break;
               }
               spr->lotag = 20;
               spr->picnum = GONZOBSHPAIN;
               playsound_loc(S_GRONDEATHA + rand() % 3, spr->x, spr->y);
               if (spr->shade > 30) {
                    trailingsmoke(sn, 2);
                    deletesprite((short) sn);
                    goto outathere;
               }
               kills++;
               break;
          case NEWGUY:
          case NEWGUYSTAND:
          case NEWGUYKNEE:
          case NEWGUYCAST:
          case NEWGUYBOW:
          case NEWGUYMACE:
          case NEWGUYPUNCH:
               if (netgame) {
                    break;
               }
               spr->lotag = 20;
               spr->picnum = NEWGUYDIE;
               playsound_loc(S_AGM_DIE1+(rand()%3),spr->x,spr->y);
               kills++;
               break;
          case KURTSTAND:
          case KURTKNEE:
          case KURTAT:
          case KURTPUNCH:
          case KURTREADY:
          case GONZOCSW:
          case GONZOCSWAT:
          case GONZOCSWPAIN:
               if (netgame) {
                    break;
               }
               spr->lotag = 20;
               spr->picnum = GONZOCSWPAIN;
               playsound_loc(S_GRONDEATHA + rand() % 3, spr->x, spr->y);
               kills++;
               break;
          case GONZOGSW:
          case GONZOGSWAT:
          case GONZOGSWPAIN:
               if (netgame) {
                    break;
               }
               spr->lotag = 20;
               spr->picnum = GONZOGSWPAIN;
               playsound_loc(S_GRONDEATHA + rand() % 3, spr->x, spr->y);
               kills++;
               break;
          case GONZOGHM:
          case GONZOGHMAT:
          case GONZOGHMPAIN:
               if (netgame) {
                    break;
               }
               spr->lotag = 20;
               spr->picnum = GONZOGHMPAIN;
               playsound_loc(S_GRONDEATHA + rand() % 3, spr->x, spr->y);
               kills++;
               break;
          case GONZOGSH:
          case GONZOGSHAT:
          case GONZOGSHPAIN:
               if (netgame) {
                    break;
               }
               spr->lotag = 20;
               spr->picnum = GONZOGSHPAIN;
               playsound_loc(S_GRONDEATHA + rand() % 3, spr->x, spr->y);
               kills++;
               break;

          case GRONHALPAIN:
          case GRONHAL:
          case GRONHALATTACK:
               spr->lotag = 20;
               spr->picnum = GRONHALDIE;
               playsound_loc(S_GRONDEATHA + rand() % 3, spr->x, spr->y);
               kills++;
               break;
          case GRONSWPAIN:
          case GRONSW:
          case GRONSWATTACK:
               spr->lotag = 20;
               spr->picnum = GRONSWDIE;
               playsound_loc(S_GRONDEATHA + rand() % 3, spr->x, spr->y);
               kills++;
               break;
          case GRONMUPAIN:
          case GRONMU:
          case GRONMUATTACK:
               spr->lotag = 20;
               spr->picnum = GRONMUDIE;
               playsound_loc(S_GRONDEATHA + rand() % 3, spr->x, spr->y);
               kills++;
               break;
          case FISH:
          case RAT:
               if (netgame) {
                    break;
               }
               spr->lotag = 20;
               break;
          case KOBOLD:
          case KOBOLDATTACK:
               if (netgame) {
                    break;
               }
               playsound_loc(S_KDIE1 + (rand() % 2), spr->x, spr->y);
               spr->lotag = 20;
               spr->picnum = KOBOLDDIE;
               kills++;
               break;
          case KATIE:
          case KATIEAT:
               if (netgame) {
                    break;
               }
               playsound_loc(S_JUDYDIE,spr->x,spr->y);
               spr->lotag = 20;
               spr->picnum = KATIEPAIN;
               spawnhornskull(sn);
               kills++;
               break;
          case DEVILSTAND:
          case DEVIL:
          case DEVILATTACK:
               if (netgame) {
                    break;
               }
               playsound_loc(S_DEMONDIE1 + (rand() % 2), spr->x, spr->y);
               spr->lotag = 20;
               spr->picnum = DEVILDIE;
               kills++;
               break;
          case SKELETON:
          case SKELETONATTACK:
               if (netgame) {
                    break;
               }
               playsound_loc(S_SKELETONDIE, spr->x, spr->y);
               spr->lotag = 20;
               spr->picnum = SKELETONDIE;
               kills++;
               break;
          case IMP:
          case IMPATTACK:
          case IMPDIE:
               if (netgame) {
                    break;
               }
               playsound_loc(S_IMPDIE1+(rand()%2),spr->x,spr->y);
               spr->lotag = 20;
               spr->picnum = IMPDIE;
               kills++;
               break;
          case MINOTAUR:
          case MINOTAURATTACK:
               if (netgame) {
                    break;
               }
               playsound_loc(S_MDEATH1, spr->x, spr->y);
               spr->lotag = 10;
               spr->picnum = MINOTAURDIE;
               kills++;
               break;
          case DEMON:
               if (netgame) {
                    break;
               }
               playsound_loc(S_GUARDIANDIE, spr->x, spr->y);
               kills++;
               explosion(sn, spr->x, spr->y, spr->z, spr->owner);
               deletesprite((short) sn);
               score(1500);
               goto outathere;
               break;
          case GUARDIAN:
          case GUARDIANATTACK:
               if (netgame) {
                    break;
               }
               playsound_loc(S_GUARDIANDIE, spr->x, spr->y);
               explosion(sn, spr->x, spr->y, spr->z, spr->owner);
               deletesprite((short) sn);
               score(1500);
               kills++;
               goto outathere;
               break;
          case WILLOW:
               if (netgame) {
                    break;
               }
               playsound_loc(S_WILLOWDIE, spr->x, spr->y);
               spr->pal = 0;
               spr->lotag = 20;
               spr->picnum = WILLOWEXPLO;
               kills++;
               break;
          }
          changespritestat(sn, DIE);
outathere:
          break;

     case RESURECT:
          if (netgame) {
               break;
          }
          spr->lotag = 7200;
          switch (spr->picnum) {

          case GONZOBSHDEAD:
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(85);
               break;
          case NEWGUYDEAD:
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(55);
               break;
          case GONZOCSWDEAD:
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(55);
               break;
          case GONZOGSWDEAD:
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(105);
               break;
          case GONZOGHMDEAD:
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(100);
               break;
          case GONZOGSHDEAD:
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(110);
               break;

          case GRONDEAD:
               spr->picnum = GRONDEAD;
               spr->cstat &= ~3;
               spr->extra = 3;
               changespritestat(sn, RESURECT);
               switch (sprite[sn].pal) {
               case 0:
                    score(125);
                    break;
               case 10:
                    score(90);
                    break;
               case 11:
                    score(115);
                    break;
               case 12:
                    score(65);
                    break;
               }
               break;
          case FISH:
          case RAT:
               spr->picnum = RAT;
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(5);
               break;
          case KOBOLDDEAD:
               spr->picnum = KOBOLDDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               switch (sprite[sn].pal) {
               case 0:
                    score(25);
                    break;
               case 7:
                    score(40);
                    break;
               }
               score(10);
               break;
          case KATIEDEAD:
               trailingsmoke(sn, 1);
               spr->picnum = KATIEDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               spawnhornskull(sn);
               score(5000);
               break;
          case DEVILDEAD:
               trailingsmoke(sn, 1);
               spr->picnum = DEVILDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(70);
               break;
          case SKELETONDEAD:
               spr->picnum = SKELETONDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(20);
               break;
          case IMPDEAD:
               spr->picnum = IMPDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(115);
               break;
          case MINOTAURDEAD:
               spr->picnum = MINOTAURDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, RESURECT);
               score(95);
               break;
          case DEMON:
          case GUARDIAN:
          case GUARDIANATTACK:
               changespritestat(sn, 0);
               deletesprite((short) sn);
               score(120);
               break;
          case WILLOW:
               changespritestat(sn, 0);
               deletesprite((short) sn);
               score(15);
               break;
          }
          break;
     case DEAD:
          switch (spr->picnum) {
          case FISH:
          case RAT:
               if (netgame) {
                    break;
               }
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               score(5);
               break;

          case GONZOBSHDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = GONZOBSHDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               if (spr->pal == 4) {
                    changespritestat(sn, SHADE);
                    deaddude(sn);
               }
               else {
                    changespritestat(sn, DEAD);
                    if (spr->shade < 25)
                         monsterweapon(sn);
               }
               score(85);
               break;
          case GONZOCSWDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = GONZOCSWDEAD;
               spr->cstat &= ~3;
               if (spr->pal == 4) {
                    changespritestat(sn, SHADE);
                    deaddude(sn);
               }
               else {
                    changespritestat(sn, DEAD);
                    monsterweapon(sn);
               }
               score(55);
               break;
          case GONZOGSWDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = GONZOGSWDEAD;
               spr->cstat &= ~3;
               if (spr->pal == 4) {
                    changespritestat(sn, SHADE);
                    deaddude(sn);
               }
               else {
                    changespritestat(sn, DEAD);
                    monsterweapon(sn);
               }
               score(105);
               break;
          case GONZOGHMDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = GONZOGHMDEAD;
               spr->cstat &= ~3;
               if (spr->pal == 4) {
                    changespritestat(sn, SHADE);
                    deaddude(sn);
               }
               else {
                    changespritestat(sn, DEAD);
                    monsterweapon(sn);
               }
               score(100);
               break;
          case NEWGUYDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = NEWGUYDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               monsterweapon(sn);
               score(50);
               break;
          case GONZOGSHDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = GONZOGSHDEAD;
               spr->cstat &= ~3;
               if (spr->pal == 4) {
                    changespritestat(sn, SHADE);
                    deaddude(sn);
               }
               else {
                    changespritestat(sn, DEAD);
                    monsterweapon(sn);
               }
               score(110);
               break;
          case GRONDEAD:
               spr->picnum = GRONDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               switch (sprite[sn].pal) {
               case 0:
                    score(125);
                    break;
               case 10:
                    score(90);
                    break;
               case 11:
                    score(115);
                    break;
               case 12:
                    score(65);
                    break;
               }
               monsterweapon(sn);
               break;
          case KOBOLDDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = KOBOLDDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               switch (sprite[sn].pal) {
               case 0:
                    score(25);
                    break;
               case 7:
                    score(40);
                    break;
               }
               monsterweapon(sn);
               break;
          case KATIEDEAD:
               if (netgame) {
                    break;
               }
               trailingsmoke(sn, 1);
               spr->picnum = DEVILDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               spawnhornskull(sn);
               score(500);
               break;
          case DEVILDEAD:
               if (netgame) {
                    break;
               }
               trailingsmoke(sn, 1);
               spr->picnum = DEVILDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               score(70);
               monsterweapon(sn);
               break;
          case SKELETONDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = SKELETONDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               score(20);
               monsterweapon(sn);
               break;
          case IMPDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = IMPDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               score(115);
               monsterweapon(sn);
               break;
          case MINOTAURDEAD:
               if (netgame) {
                    break;
               }
               spr->picnum = MINOTAURDEAD;
               spr->cstat &= ~3;
               changespritestat(sn, DEAD);
               score(95);
               if (rand() % 100 > 60)
                    monsterweapon(sn);
               break;
          case WILLOWEXPLO + 2:
               if (netgame) {
                    break;
               }
               spr->pal = 0;
               spr->cstat &= ~3;
               changespritestat(sn, 0);
               deletesprite((short) sn);
               score(15);
               break;
          }
          if (sector[spr->sectnum].floorpicnum == WATER
              || sector[spr->sectnum].floorpicnum == SLIME) {
               if (spr->picnum == MINOTAURDEAD) {
                    spr->z += (8 << 8);
                    setsprite((short) sn, spr->x, spr->y, spr->z);
               }
          }
          if (sector[spr->sectnum].floorpicnum == LAVA
              || sector[spr->sectnum].floorpicnum == LAVA1
              || sector[spr->sectnum].floorpicnum == LAVA2) {
               trailingsmoke(sn, 1);
               deletesprite((short) sn);
          }
          break;

     }
     //
     // the control variable for monster release
     //

}







// FIRE BREATH

void
firebreath(short i, int a, int b, int c)
{

     long j;
     int  k;
     long discrim;
     long discrim2;

     struct player *plr;
     spritetype *spr;
     spritetype *tspr;

     plr = &player[pyrn];
     spr = &sprite[i];

     for (k = 0; k <= a; k++) {

          j = insertsprite(spr->sectnum, MISSILE);
          tspr = &sprite[j];

          tspr->x = spr->x;
          tspr->y = spr->y;
          if (c == LOW)
               tspr->z = sector[spr->sectnum].floorz - (32 << 8);
          else
               tspr->z = sector[spr->sectnum].floorz - (tilesizy[spr->picnum] << 7);
          tspr->cstat = 0;
          tspr->picnum = MONSTERBALL;
          tspr->shade = -15;
          tspr->xrepeat = 128;
          tspr->yrepeat = 128;
          tspr->ang = ((((getangle(plr->x - tspr->x, plr->y - tspr->y) + (krand() & 15) - 8) + 2048) + ((b * 22) + (k * 10))) & 2047);
          tspr->xvel = (sintable[(tspr->ang + 2560) & 2047] >> 6);
          tspr->yvel = (sintable[(tspr->ang + 2048) & 2047] >> 6);
          discrim = ksqrt((plr->x - tspr->x) * (plr->x - tspr->x)
                          + (plr->y - tspr->y) * (plr->y - tspr->y));
          if (discrim == 0)
               discrim = 1;
          if (c == HIGH)
               tspr->zvel = ((plr->z + (32 << 8) - tspr->z) << 7) / discrim;
          else
               tspr->zvel = (((plr->z + (8 << 8)) - tspr->z) << 7) / discrim;   // NEW

          tspr->owner = i;
          tspr->clipdist = 16;
          tspr->lotag = 512;
          tspr->hitag = 0;
     }



}

//cast

void
castspell(short i)
{

     short j;
     long discrim;
     long discrim2;

     struct player *plr;
     spritetype *spr;
     spritetype *tspr;

     plr = &player[pyrn];
     spr = &sprite[i];

     j = insertsprite(spr->sectnum, MISSILE);
     tspr = &sprite[j];

     tspr->x = spr->x;
     tspr->y = spr->y;
     if (spr->picnum == SPAWNFIREBALL) {
          tspr->z = spr->z - ((tilesizy[spr->picnum] >> 1) << 8);
     }
     else {
          tspr->z = spr->z - ((tilesizy[spr->picnum] >> 1) << 8);
     }
     tspr->cstat = 0;
     tspr->picnum = MONSTERBALL;
     tspr->shade = -15;
     tspr->xrepeat = 64;
     tspr->yrepeat = 64;
     if (sprite[i].picnum == SPAWNFIREBALL) {
          tspr->ang = ((getangle(plr->x - tspr->x, plr->y - tspr->y) + 2048) & 2047);
     }
     else {
          tspr->ang = (((getangle(plr->x - tspr->x, plr->y - tspr->y) + (krand() & 15) - 8) + 2048) & 2047);
     }
     tspr->xvel = (sintable[(tspr->ang + 2560) & 2047] >> 6);
     tspr->yvel = (sintable[(tspr->ang + 2048) & 2047] >> 6);

     // discrim=ksqrt((plr->x-tspr->x)*(plr->x-tspr->x)+(plr->y-tspr->y)*(plr->y
     // -tspr->y));
     // if(discrim == 0) {
     // discrim=1;
     // }

     tspr->zvel = ((plr->z + (8 << 8) - tspr->z) << 8) /
          ksqrt((plr->x - tspr->x) *
                (plr->x - tspr->x) +
                (plr->y - tspr->y) *
                (plr->y - tspr->y));

     tspr->owner = i;
     tspr->clipdist = 16;
     tspr->lotag = 512;
     tspr->hitag = 0;

}

void
skullycastspell(short i)
{

     short j;
     long discrim;
     long discrim2;

     struct player *plr;
     spritetype *spr;
     spritetype *tspr;

     plr = &player[pyrn];
     spr = &sprite[i];

     j = insertsprite(spr->sectnum, MISSILE);
     tspr = &sprite[j];

     tspr->x = spr->x;
     tspr->y = spr->y;
     if (spr->picnum == SPAWNFIREBALL) {
          tspr->z = spr->z - ((tilesizy[spr->picnum] >> 1) << 8);
     }
     else {
          tspr->z = spr->z - ((tilesizy[spr->picnum] >> 1) << 8);
     }
     tspr->cstat = 0;
     tspr->picnum = PLASMA;
     tspr->shade = -15;
     tspr->xrepeat = 64;
     tspr->yrepeat = 64;
     if (spr->picnum == SPAWNFIREBALL) {
          tspr->ang = ((getangle(plr->x - tspr->x, plr->y - tspr->y) + 2048) & 2047);
     }
     else {
          tspr->ang = (((getangle(plr->x - tspr->x, plr->y - tspr->y) + (krand() & 15) - 8) + 2048) & 2047);
     }
     tspr->xvel = (sintable[(tspr->ang + 2560) & 2047] >> 6);
     tspr->yvel = (sintable[(tspr->ang + 2048) & 2047] >> 6);

     discrim = ksqrt((plr->x - tspr->x) * (plr->x - tspr->x) + (plr->y - tspr->y) * (plr->y - tspr->y));
     if (discrim == 0) {
          discrim = 1;
     }
     tspr->zvel = ((plr->z + (48 << 8) - tspr->z) << 7) / discrim;
     tspr->owner = i;
     tspr->clipdist = 16;
     tspr->lotag = 512;
     tspr->hitag = 0;
     tspr->pal = 7;

}



int
checkheat(short i)
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
     struct player *plr;
     int  trys;

     plr = &player[pyrn];
     hitsprite = 0;

     daang = sprite[i].ang;
     dahoriz = plr->horiz;

     x = sprite[i].x;
     y = sprite[i].y;
     z = sprite[i].z;

     dasectnum = sprite[i].sectnum;

     daang2 = ((daang + 2048 + (krand() & 31) - 16) & 2047);
     daz2 = ((100 - dahoriz) * 2000) + ((krand() - 32768) >> 1);

     hitscan(x, y, z, dasectnum,   // Start position
             sintable[(daang2 + 2560) & 2047],    // X vector of 3D ang
             sintable[(daang2 + 2048) & 2047],    // Y vector of 3D ang
             daz2,                 // Z vector of 3D ang
             &hitsect, &hitwall, &hitsprite, &hitx, &hity, &hitz);

     if (hitsprite > 0 && hitsprite != plr->spritenum) {
          sprite[i].ang = (getangle(sprite[hitsprite].x - sprite[i].x, sprite[hitsprite].y - sprite[i].y) & 2047);
          return (1);
     }
     else {
          sprite[i].ang = plr->ang;
          if (plr->horiz > 100) {
               sprite[i].z -= plr->horiz;
          }
     }
     return (0);
}

int
checkfacing(short i, long x, long y)
{


     if (sintable[(sprite[i].ang + 2560) & 2047] * (x - sprite[i].x)
         + sintable[(sprite[i].ang + 2048) & 2047] * (y - sprite[i].y) >= 0)
          return (1);
     else
          return (0);

}

int
checkmedusadist(short i, long x, long y, long z, int lvl)
{

     long attackdist;

     attackdist = 8192 + (lvl << 9);

     if ((labs(x - sprite[i].x) + labs(y - sprite[i].y) < attackdist)
         && (labs((z >> 8) - ((sprite[i].z >> 8)
         - (tilesizy[sprite[i].picnum] >> 1))) <= 120))
          return (1);
     else
          return (0);

}


int
checkdist(short i, long x, long y, long z)
{

     long attackdist;
     struct player *plr;

     plr = &player[0];

     switch (sprite[i].picnum) {
     case KURTAT:

     case NEWGUY:
     case NEWGUYMACE:
     case NEWGUYCAST:
     case NEWGUYBOW:
          if (sprite[i].extra > 10) {
               attackdist = 2048 << 1;
          }
          else {
               attackdist = 1024 + 256;
          }
          break;

     case KURTPUNCH:
     case NEWGUYPUNCH:

     case GONZOGSW:
     case GONZOGHM:
     case GONZOGSH:


     case GONZOGSWAT:
     case GONZOGHMAT:
     case GONZOGSHAT:
          attackdist = 1024 + 256;
          break;

     case GONZOCSW:
     case GONZOCSWAT:
          if (sprite[i].extra > 10)
               attackdist = 2048 << 1;
          else
               attackdist = 1024 + 256;
          break;

     case GRONHALATTACK:
     case GRONHAL:
          attackdist = 1024 + 512;
          break;
     case GRONMU:
     case GRONMUATTACK:
          attackdist = 2048;
          break;
     case GRONSW:
     case GRONSWATTACK:
          attackdist = 1024 + 256;
          break;
     case MINOTAUR:
     case MINOTAURATTACK:
          attackdist = 1024 + 512;
          break;
     case SKELETONATTACK:
     case IMP:
     case IMPATTACK:
     case KOBOLD:
     case KOBOLDATTACK:
          attackdist = 1024;
          break;
     case DEMON:
          attackdist = 4096 + 2048;
          break;
     case GUARDIAN:
     case GUARDIANATTACK:
          attackdist = 4096;
          break;
     case KATIE:
     case KATIEAT:
     case DEVIL:
     case DEVILATTACK:
          attackdist = 2048;
          break;
     case FISH:
     case RAT:
     case WILLOW:
          attackdist = 512;
          break;
     default:
          attackdist = 512;
     }

     if ((labs(x - sprite[i].x) + labs(y - sprite[i].y) < attackdist)
         && (labs((z >> 8) - ((sprite[i].z >> 8) - (tilesizy[sprite[i].picnum] >> 1))) <= 120))
          return (1);
     else
          return (0);

}

int
checksight(short i, short *daang)
{

     struct player *plr;
     plr = &player[pyrn];
     if (invisibletime > 0) {
          *daang = ((krand() & 512) - 256) & 2047;
          return (0);
     }
     if (cansee(plr->x, plr->y, plr->z, plr->sector,
                sprite[i].x, sprite[i].y, sprite[i].z - (tilesizy[sprite[i].picnum] << 7), sprite[i].sectnum) == 1
         && invisibletime < 0) {

          *daang = (getangle(plr->x - sprite[i].x, plr->y - sprite[i].y) & 2047);

          if (((sprite[i].ang + 2048 - *daang) & 2047) < 1024)
               sprite[i].ang = ((sprite[i].ang + 2048 - (synctics << 1)) & 2047);
          else
               sprite[i].ang = ((sprite[i].ang + (synctics << 1)) & 2047);

          return (1);
     }
     else {
          daang = 0;
     }

     return (0);
}

void
checkmove(short i, short *movestat)
{

     struct player *plr;
     spritetype *spr;

     plr = &player[pyrn];
     spr = &sprite[i];

     //*movestat=actormovesprite((short)i);

     *movestat = movesprite((short) i,
                 (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
                            (((long) sintable[spr->ang]) * synctics) << 3,
                            0L,
                            4L << 8,
                            4L << 8,
                            2);



     if (*movestat != 0) {
          if (krand() & 1 == 0) {
               spr->ang = (getangle(plr->x - spr->x, plr->y - spr->y) & 2047);
          }
          else
               spr->ang = ((spr->ang + (synctics)) & 2047);
     }


}

extern
int  droptheshield;

void
shieldhit(int hp)
{
     shieldpoints -= hp;
     if (shieldpoints <= 0) {
          strcpy(displaybuf, "Shield useless");
          displaytime = 360;
     }
}

void
attack(short i)
{

     struct player *plr;
     int  a=0,k,s=0;


     plr = &player[pyrn];

     if (invincibletime > 0)
          return;

     if (plr->treasure[6] == 1 && krand() & 32 > 16)
          return;

     if (krand() & 15 < plr->armortype + 10)
          return;

     if (droptheshield == 0 && shieldpoints > 0 && selectedgun < 5) {
          a = getangle(sprite[i].x - plr->x, sprite[i].y - plr->y);
          if ((a < plr->ang && plr->ang - a < 128) ||
              (a > plr->ang && ((plr->ang + a) & 2047) < 128)) {
               if (krand() % 100 > 80) {
                    playsound_loc(S_SWORD1 + rand() % 3, plr->x, plr->y);
                    return;
               }
               else {
                    s = krand() % 50;
                    shieldhit(-s);
                    if (krand() % 100 > 50) {
                         playsound_loc(S_SWORD1 + rand() % 3, plr->x, plr->y);
                         return;
                    }
               }
          }
     }

#if 0
     k = krand() % 100;
     if (k > (plr->armortype << 3))
          k = 15;
     else
          k = 5;
#endif
     switch (sprite[i].picnum) {
     case FISH:
     case RAT:
          break;
     case SKELETONATTACK:          // damage 5 - 10
          playsound_loc(S_RIP1 + (rand() % 3), sprite[i].x, sprite[i].y);
          if (rand() % 2) {
               playsound_loc(S_GORE1 + (rand() % 4), sprite[i].x, sprite[i].y);
          }
          if (rand() % 2) {
               playsound_loc(S_BREATH1 + (rand() % 6), sprite[i].x, sprite[i].y);
          }
          k = (krand() % 5) + 5;
          break;
     case KATIE:                   // damage 5 - 50
     case KATIEAT:
          playsound_loc(S_DEMONTHROW, sprite[i].x, sprite[i].y);
          k = (krand() % 45) + 5;
          break;
     case DEVIL:                   // damage fireball 5 - 25
     case DEVILATTACK:
          playsound_loc(S_DEMONTHROW, sprite[i].x, sprite[i].y);
          break;
     case KOBOLDATTACK:            // damage 5 - 10
          playsound_loc(S_GENSWING, sprite[i].x, sprite[i].y);
          if (rand() % 10 > 4) {
               playsound_loc(S_KOBOLDHIT, plr->x, plr->y);
               playsound_loc(S_BREATH1 + (rand() % 6), plr->x, plr->y);
          }
          k = (krand() % 5) + 5;
          break;
     case IMPATTACK:               // damage 5 - 10
          playsound_loc(S_RIP1 + (rand() % 3), sprite[i].x, sprite[i].y);
          if (rand() % 2) {
               playsound_loc(S_GORE1 + (rand() % 4), sprite[i].x, sprite[i].y);
          }
          if (rand() % 2) {
               playsound_loc(S_BREATH1 + (rand() % 6), sprite[i].x, sprite[i].y);
          }

          k = (krand() % 5) + 5;
          if (k > 8) {
               poisoned = 1;
          }
          break;

     case NEWGUYMACE:              // damage 5 - 20
          playsound_loc(S_PLRWEAPON2,sprite[i].x,sprite[i].y);
          if (rand()%10 > 4) {
               playsound_loc(S_KOBOLDHIT,plr->x,plr->y);
               playsound_loc(S_BREATH1+(rand()%6),plr->x,plr->y);
          }
          k = (krand() % 15) + 5;
          break;
     case NEWGUYPUNCH:             // damage 1 - 5
     case KURTPUNCH:
          playsound_loc(S_GENSWING,sprite[i].x,sprite[i].y);
          if (rand()%10 > 4) {
               playsound_loc(S_SOCK1+(rand()%4),plr->x,plr->y);
               playsound_loc(S_BREATH1+(rand()%6),plr->x,plr->y);
          }
          k = (krand() % 4) + 1;
          break;
     case KURTAT:                  // damage 5 - 15
          playsound_loc(S_GENSWING, sprite[i].x, sprite[i].y);
          if (rand() % 10 > 3) {
               playsound_loc(S_SWORD1 + (rand() % 6), sprite[i].x, sprite[i].y);
          }
          k = (krand() % 10) + 5;
          break;
     case GONZOCSWAT:              // damage 5- 20
     case GONZOGSWAT:
          playsound_loc(S_GENSWING, sprite[i].x, sprite[i].y);
          if (rand() % 10 > 6) {
               playsound_loc(S_SWORD1 + (rand() % 6), sprite[i].x, sprite[i].y);
          }
          k = (krand() % 15) + 5;
          break;

     case GONZOGHMAT:              // damage 5- 15
          playsound_loc(S_GENSWING, sprite[i].x, sprite[i].y);
          if (rand() % 10 > 6) {
               playsound_loc(S_SWORD1 + (rand() % 6), sprite[i].x, sprite[i].y);
          }
          k = (krand() % 10) + 5;
          break;

     case GONZOGSHAT:              // damage 5 - 20
          playsound_loc(S_GENSWING, sprite[i].x, sprite[i].y);
          if (rand() % 10 > 3) {
               playsound_loc(S_SWORD1 + (rand() % 6), sprite[i].x, sprite[i].y);
          }
          k = (krand() % 15) + 5;
          break;

     case GRONSWATTACK:            // damage 5 - 25
          k = (krand() % 20) + 5;
          if (sprite[i].shade > 30) {
               k += krand() % 10;
          }
          playsound_loc(S_GENSWING, sprite[i].x, sprite[i].y);
          if (rand() % 10 > 3) {
               playsound_loc(S_SWORD1 + (rand() % 6), sprite[i].x, sprite[i].y);
          }
          break;

     case MINOTAURATTACK:          // damage 5 - 30
          playsound_loc(S_GENSWING, sprite[i].x, sprite[i].y);
          if (rand() % 10 > 4) {
               playsound_loc(S_SWORD1 + (rand() % 6), sprite[i].x, sprite[i].y);
          }
          k = (krand() % 25) + 5;
          break;
     }

     if (shieldpoints > 0) {
          if (s > k) {
               k = 0;
          }
          else {
               k -= s;
          }
     }

     switch (plr->armortype) {
     case 0:                       // none
          healthpic(-k);
          break;
     case 1:                       // leather
          a = krand() % 5;
          if (a > k) {
               k = 0;
          }
          else {
               k -= a;
          }
          armorpic(-a);
          healthpic(-k);
          break;
     case 2:                       // chain
          a = krand() % 10;
          if (a > k) {
               k = 0;
          }
          else {
               k -= a;
          }
          armorpic(-a);
          healthpic(-k);
          break;
     case 3:                       // plate
          a = krand() % 20;
          if (a > k) {
               k = 0;
          }
          else {
               k -= a;
          }
          armorpic(-a);
          healthpic(-k);
          break;
     }

     startredflash(k);

#if 0
     if (rand() % 100 > 50) {
          svel += (rand() % k) << 3;
          angvel += (rand() % k) << 3;
     }
     else {
          svel -= (rand() % k) << 3;
          angvel += (rand() % k) << 3;
     }
#endif

     if (k == 0)
          k = 1;

     k = rand() % k;
     angvel += k << 3;
     svel += k << 3;
     vel -= k << 3;
     plr->hvel += k << 2;

#if 0
     if (plr->health < 21) {
          strcpy(displaybuf, "health critical");
          displaytime = 30;
     }
#endif

}




void
fireballblast(short i)
{

     long j;
     int  k;
     spritetype *spr;
     spritetype *tspr;

     spr = &sprite[i];

     for (k = 0; k < 8; k++) {

          j = insertsprite(spr->sectnum, FX);
          tspr = &sprite[j];

          tspr->x = spr->x;
          tspr->y = spr->y;
          tspr->z = spr->z;

          tspr->cstat = 2;
          // tspr->picnum=FIREBALL;
          tspr->picnum = FATSPANK;
          tspr->shade = 0;

          tspr->xrepeat = 8;
          tspr->yrepeat = 8;
          tspr->ang = k << 8;
          tspr->xvel = ((krand() & 511) - 256) << 3;
          tspr->yvel = ((krand() & 511) - 256) << 3;
          tspr->zvel = ((krand() & 511) - 256) << 3;

          tspr->owner = spr->owner;
          tspr->lotag = 196;
          tspr->hitag = 0;

     }

}

void
makeafire(short i, int firetype)
{
     long j;
     spritetype *spr,
         *tspr;

     spr = &sprite[i];

     j = insertsprite(spr->sectnum, FIRE);
     tspr = &sprite[j];

     tspr->x = spr->x + (rand() & 1024) - 512;
     tspr->y = spr->y + (rand() & 1024) - 512;
     tspr->z = spr->z;
     tspr->cstat = 0;
     tspr->xrepeat = 64;
     tspr->yrepeat = 64;
     tspr->shade = 0;
     tspr->clipdist = 64;
     tspr->owner = spr->owner;
     tspr->lotag = 2047;
     tspr->hitag = 0;

     changespritestat(j, FIRE);

}

void
explosion(short i, long x, long y, long z, short owner)
{
     long j;
     spritetype *spr,
         *tspr;

     spr = &sprite[i];
     j = insertsprite(spr->sectnum, EXPLO);
     tspr = &sprite[j];

     tspr->x = x;
     tspr->y = y;
     tspr->z = z + (16 << 8);
     tspr->cstat = 0;
     tspr->cstat &= ~3;
     tspr->picnum = EXPLOSTART;
     tspr->shade = -15;
     tspr->xrepeat = 64;
     tspr->yrepeat = 64;
     tspr->ang = krand() & 2047;
     tspr->xvel = ((krand() & 511) - 256);
     tspr->yvel = ((krand() & 511) - 256);
     tspr->zvel = ((krand() & 511) - 256);
     tspr->owner = owner;
     tspr->lotag = 12;
     tspr->hitag = 0;
     tspr->pal = 0;

}

void
explosion2(short i, long x, long y, long z, short owner)
{
     long j;
     spritetype *spr,
         *tspr;

     spr = &sprite[i];

     j = insertsprite(spr->sectnum, EXPLO);
     tspr = &sprite[j];

     tspr->x = x;
     tspr->y = y;
     tspr->z = z + (16 << 8);
     tspr->cstat = 0;
     tspr->cstat &= ~3;
     tspr->picnum = EXPLOSTART;
     tspr->shade = -25;
     tspr->xrepeat = 64;
     tspr->yrepeat = 64;
     tspr->ang = krand() & 2047;
     tspr->xvel = ((krand() & 256) - 128);
     tspr->yvel = ((krand() & 256) - 128);
     tspr->zvel = ((krand() & 256) - 128);
     tspr->owner = spr->owner;
     tspr->lotag = 12;
     tspr->hitag = 0;
     tspr->pal = 0;

}


void
trailingsmoke(short i, short ball)
{
     long j;
     spritetype *spr,
         *tspr;

     spr = &sprite[i];

     j = insertsprite(spr->sectnum, SMOKE);
     tspr = &sprite[j];

     tspr->x = spr->x;
     tspr->y = spr->y;
     tspr->z = spr->z;
     tspr->cstat = 0x03;
     tspr->cstat &= ~3;
     tspr->picnum = SMOKEFX;
     tspr->shade = 0;
     if (ball == 1) {
          tspr->xrepeat = 128;
          tspr->yrepeat = 128;
     }
     else {
          tspr->xrepeat = 32;
          tspr->yrepeat = 32;
     }
     tspr->pal = 0;

     tspr->owner = spr->owner;
     tspr->lotag = 256;
     tspr->hitag = 0;

}

void
icecubes(short i, long x, long y, long z, short owner)
{
     long j;
     spritetype *spr,
         *tspr;

     spr = &sprite[i];

     j = insertsprite(spr->sectnum, FX);
     tspr = &sprite[j];

     tspr->x = x;
     tspr->y = y;
     tspr->z = sector[spr->sectnum].floorz - (PLAYERHEIGHT << 8) + (rand() & 4096);
     tspr->cstat = 0;
     tspr->picnum = ICECUBE;
     tspr->shade = -16;
     tspr->xrepeat = 16;
     tspr->yrepeat = 16;
     tspr->ang = ((rand() & 1023) - 1024) & 2047;
     tspr->xvel = ((rand() & 1023) - 512);
     tspr->yvel = ((rand() & 1023) - 512);
     tspr->zvel = ((rand() & 1023) - 512);
     tspr->pal = 6;
     tspr->owner = spr->owner;
     tspr->lotag = 2048;
     tspr->hitag = 0;

}


int
damageactor(short hitobject, short i)
{
     short osectnum,
          hitdamage,
          j,
          k;
     long nexti,
          dax,
          day,
          daz;
     short daang,
          movestat;
     struct player *plr;
     int  incr;
     spritetype *spr,
         *tspr;

     plr = &player[pyrn];
     spr = &sprite[i];

     j = (hitobject & 4095);
     tspr = &sprite[j];

     // if (tspr->owner == 4096) {
     // return(0);
     // }

     if (j == plr->spritenum && spr->owner == 4096) {
          return (0);
     }

//    if (j == plr->spritenum && spr->owner != 4096) {
     if (spr->owner != 4096) {
          if (invincibletime <= 0 || manatime <= 0) {
               if ((krand() & 9) == 0) {
                    playsound_loc(S_PLRPAIN1 + (rand() % 2), sprite[i].x, sprite[i].y);
               }
               if (manatime > 0 && spr->picnum != FATSPANK && spr->picnum != PLASMA) {
                    deletesprite((short) i);
               }
               else {
                    if (spr->picnum == DART) {
                         poisoned = 1;
                         poisontime = 7200;
                         strcpy(displaybuf, "Poisoned");
                         displaytime = 360;
                    }
                    if (netgame) {
                         netdamageactor(j, i);
                    }
                    else {
                         if (spr->picnum == PLASMA) {
                              healthpic(-((krand() & 15) + 15));
                              vel -= (rand() % 20);
                         }
                         else if (spr->picnum == FATSPANK) {
                              playsound_loc(S_GORE1A + (rand() % 3), plr->x, plr->y);
                              healthpic(-((krand() & 10) + 10));
                              if (rand() % 100 > 90) {
                                   poisoned = 1;
                                   poisontime = 7200;
                                   strcpy(displaybuf, "Poisoned");
                                   displaytime = 360;
                              }
                         }
                         else if (spr->picnum == THROWPIKE) {
                              healthpic(-((krand() % 10) + 5));
                         }
                         else {
                              healthpic(-((krand() % 20) + 5));
                         }
                    }
                    startredflash(10);
               }
          }
     }
     if (j != plr->spritenum && netgame == 0) {   // Les 08/11/95
          if (spr->owner != j) {
               switch (tspr->picnum) {
               case BARREL:
               case VASEA:
               case VASEB:
               case VASEC:
               case STAINGLASS1:
               case STAINGLASS2:
               case STAINGLASS3:
               case STAINGLASS4:
               case STAINGLASS5:
               case STAINGLASS6:
               case STAINGLASS7:
               case STAINGLASS8:
               case STAINGLASS9:
                    if (tspr->hitag == 0) {
                         tspr->hitag = 0;
                         tspr->lotag = 0;
                         newstatus(j, BROKENVASE);
                    }
                    break;


               case NEWGUY:
               case NEWGUYSTAND:
               case NEWGUYKNEE:
               case NEWGUYMACE:
               case NEWGUYCAST:
               case NEWGUYPUNCH:
               case NEWGUYBOW:

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
               case GRONHALATTACK:
               case GRONMU:
               case GRONMUATTACK:
               case GRONSW:
               case GRONSWATTACK:
               case KOBOLD:
               case KOBOLDATTACK:
               case KATIE:
               case KATIEAT:
               case DEVIL:
               case DEVILATTACK:
               case IMP:
               case IMPATTACK:
               case MINOTAUR:
               case MINOTAURATTACK:
               case SKELETON:
               case SKELETONATTACK:
                    switch (spr->picnum) {
                    case PLASMA:
                         tspr->hitag -= 40;
                         break;
                    case FATSPANK:
                         tspr->hitag -= 10;
                         break;
                    case MONSTERBALL:
                         tspr->hitag -= 40;
                         break;
                         // case FIREBALL:
                         // tspr->hitag-=3;
                         // break;
                    case BULLET:
                         tspr->hitag -= 10;
                         break;
                    case DISTORTIONBLAST:
                         tspr->hitag = 10;
                         break;
                    case DART:
                    case WALLARROW:
                    case HORIZSPIKEBLADE:
                    case THROWHALBERD:
                         tspr->hitag -= 30;
                         break;
                    case THROWPIKE:
                         if (rand() % 2) {
                              playsound_loc(S_GORE1A + rand() % 2, sprite[i].x, sprite[i].y);
                         }
                         tspr->hitag -= 30;
                         break;
                    case BARREL:
                         tspr->hitag -= 100;
                         break;
                    }
                    if (tspr->hitag <= 0) {
                         newstatus(j, DIE);
                         deletesprite((short) i);
                         return (1);
                    }
                    else {
                         newstatus(j, PAIN);
                         return (1);
                    }
                    break;

               case GONZOBSHPAIN:
               case GONZOCSWPAIN:
               case GONZOGSWPAIN:
               case GONZOGHMPAIN:
               case GONZOGSHPAIN:
               case NEWGUYPAIN:
               case GRONHALPAIN:
               case GRONMUPAIN:
               case GRONSWPAIN:
               case KOBOLDDIE:
               case KATIEPAIN:
               case DEVILDIE:
               case IMPDIE:
               case MINOTAURDIE:
               case SKELETONDIE:
//JSA_NEW //why is this here it's in whplr
//raf because monsters could shatter a guy thats been frozen
                    if (tspr->pal == 6) {
                         for (k = 0; k < 32; k++) {
                              icecubes(j, tspr->x, tspr->y, tspr->z, j);
                         }
                         deletesprite((short) j);
                         return (1);
                    }
               default:
                    deletesprite((short) i);
                    return (1);
               }
          }
     }
     return (0);
}

void
nukespell(short j)
{

     long k;
     struct player *plr;

     plr = &player[pyrn];

     // dont nuke a shade
     if (sprite[j].shade > 30)
          return;

     newstatus(j, NUKED);
     sprite[j].pal = 0;
     sprite[j].cstat |= 1;
     sprite[j].cstat &= ~3;
     sprite[j].shade = 6;
     sprite[j].lotag = 360;
     sprite[j].ang = plr->ang;
     score(150);

     k = insertsprite(sprite[j].sectnum, NUKED);
     sprite[k].lotag = 360;
     sprite[k].xrepeat = 30;
     sprite[k].yrepeat = 12;
     sprite[k].picnum = ZFIRE;
     sprite[k].pal = 0;
     sprite[k].ang = sprite[j].ang;
     sprite[k].x = sprite[j].x;
     sprite[k].y = sprite[j].y;
     sprite[k].z = sprite[j].z;
     sprite[k].cstat = sprite[j].cstat;

}

void
medusa(short j)
{

     newstatus(j, FROZEN);
     switch (sprite[j].picnum) {

     case NEWGUY:
     case NEWGUYSTAND:
     case NEWGUYKNEE:
     case NEWGUYCAST:
     case NEWGUYBOW:
     case NEWGUYMACE:
     case NEWGUYPUNCH:
          sprite[j].picnum = NEWGUYPAIN;
          break;

     case KURTSTAND:
     case KURTKNEE:
     case KURTAT:
     case KURTPUNCH:
     case KURTREADY:

     case GONZOCSW:
     case GONZOCSWAT:
          sprite[j].picnum = GONZOCSWPAIN;
          break;

     case GONZOGSW:
     case GONZOGSWAT:
          sprite[j].picnum = GONZOGSWPAIN;
          break;

     case GONZOGHM:
     case GONZOGHMAT:
          sprite[j].picnum = GONZOGHMPAIN;
          break;

     case GONZOGSH:
     case GONZOGSHAT:
          sprite[j].picnum = GONZOGHMPAIN;
          break;

     case KOBOLD:
     case KOBOLDATTACK:
          sprite[j].picnum = KOBOLDDIE;
          break;
     case KATIE:
     case KATIEAT:
          sprite[j].picnum = KATIEPAIN;
          break;
     case DEVIL:
     case DEVILATTACK:
          sprite[j].picnum = DEVILDIE;
          break;
     case IMP:
     case IMPATTACK:
          sprite[j].picnum = IMPDIE;
          break;
     case MINOTAUR:
     case MINOTAURATTACK:
          sprite[j].picnum = MINOTAURDIE;
          break;
     case SKELETON:
          sprite[j].picnum = SKELETONDIE;
          break;
     case GRONHAL:
     case GRONHALATTACK:
          sprite[j].picnum = GRONHALDIE;
          break;
     case GRONMU:
     case GRONMUATTACK:
          sprite[j].picnum = GRONMUDIE;
          break;
     case GRONSW:
     case GRONSWATTACK:
          sprite[j].picnum = GRONSWDIE;
          break;
     }
     sprite[j].pal = 6;
     sprite[j].cstat |= 1;
     score(100);

}


 // New movesprite using getzrange.  Note that I made the getzrange
 // parameters global (&globhiz,&globhihit,&globloz,&globlohit) so they
 // don't need to be passed everywhere.  Also this should make this
 // movesprite function compatible with the older movesprite functions.
int
movesprite(short spritenum, long dx, long dy, long dz, long ceildist, long flordist, char cliptype)
{
     long daz,
          zoffs,
          templong;
     short retval,
          dasectnum,
          tempshort;
     spritetype *spr;

     spr = &sprite[spritenum];

     flordist = 32L;

     if ((spr->cstat & 128) == 0)
          zoffs = -((tilesizy[spr->picnum] * spr->yrepeat) << 1);
     else
          zoffs = 0;

     dasectnum = spr->sectnum;     // Can't modify sprite sectors directly
                                   // becuase of linked lists
     daz = spr->z + zoffs;         // Must do this if not using the new
                                   // centered centering (of course)
     retval = clipmove(&spr->x, &spr->y, &daz, &dasectnum, dx, dy,
                   ((long) spr->clipdist) << 2, ceildist, flordist, cliptype);

     if ((dasectnum != spr->sectnum) && (dasectnum >= 0))
          changespritesect(spritenum, dasectnum);

     // Set the blocking bit to 0 temporarly so getzrange doesn't pick up
     // its own sprite
     tempshort = spr->cstat;
     spr->cstat &= ~1;
     getzrange(spr->x, spr->y, spr->z - 1, spr->sectnum,
               &globhiz, &globhihit, &globloz, &globlohit,
               ((long) spr->clipdist) << 2, cliptype);
     spr->cstat = tempshort;

     daz = spr->z + zoffs + dz;
     if ((daz <= globhiz) || (daz > globloz)) {
          if (retval != 0)
               return (retval);
          return (16384 + dasectnum);
     }
     spr->z = daz - zoffs;
     return (retval);
}

#if 0
 // New movesprite using getzrange.  Note that I made the getzrange
 // parameters global (&globhiz,&globhihit,&globloz,&globlohit) so they
 // don't need to be passed everywhere.  Also this should make this
 // movesprite function compatible with the older movesprite functions.
int 
movesprite(short spritenum, long dx, long dy, long dz, long ceildist, long flordist, char cliptype)
{
     long daz,
          zoffs,
          templong;
     short retval,
          dasectnum,
          tempshort;
     spritetype *spr;

     spr = &sprite[spritenum];

     if ((spr->cstat & 128) == 0) {
          zoffs = -((tilesizy[spr->picnum] * spr->yrepeat) << 1);
     }
     else {
          zoffs = 0;
     }

     dasectnum = spr->sectnum;     // Can't modify sprite sectors directly
                                   // becuase of linked lists
     daz = spr->z + zoffs;         // Must do this if not using the new
                                   // centered centering (of course)
     retval = clipmove(&spr->x, &spr->y, &daz, &dasectnum, dx, dy,
                   ((long) spr->clipdist) << 2, ceildist, flordist, cliptype);

     if ((dasectnum != spr->sectnum) && (dasectnum >= 0)) {
          changespritesect(spritenum, dasectnum);
     }

     // Set the blocking bit to 0 temporarly so getzrange doesn't pick up
     // its own sprite
     tempshort = spr->cstat;
     spr->cstat &= ~1;
     getzrange(spr->x, spr->y, spr->z - 1, spr->sectnum,
               &globhiz, &globhihit, &globloz, &globlohit,
               ((long) spr->clipdist) << 2, cliptype);
     spr->cstat = tempshort;
     daz = spr->z + zoffs + dz;
     if ((daz <= globhiz) || (daz > globloz)) {
          if (retval != 0) {
               return (retval);
          }
          return (16384 + dasectnum);
     }
     spr->z = daz - zoffs;
     return (retval);
}
#endif
int
actormovesprite(short spritenum)
{
     long daz,
          zoffs,
          templong;
     short retval,
          dasectnum,
          tempshort;
     spritetype *spr;
     long dx,
          dy,
          dz;

     spr = &sprite[spritenum];

     dx = (((long) sintable[(spr->ang + 512) & 2047]) * synctics) << 3,
          dy = (((long) sintable[spr->ang]) * synctics) << 3,
          dz = 0L;

     if ((spr->cstat & 128) == 0) {
          zoffs = -((tilesizy[spr->picnum] * spr->yrepeat) << 1);
     }
     else {
          zoffs = 0;
     }

     dasectnum = spr->sectnum;
     daz = spr->z + zoffs;
     retval = clipmove(&spr->x, &spr->y, &daz, &dasectnum, dx, dy,
                       ((long) spr->clipdist) << 2, 1024L, 1024L, 2);

     if ((dasectnum != spr->sectnum) && (dasectnum >= 0)) {
          changespritesect(spritenum, dasectnum);
     }
     tempshort = spr->cstat;
     spr->cstat &= ~1;
     getzrange(spr->x, spr->y, spr->z - 1, spr->sectnum,
               &globhiz, &globhihit, &globloz, &globlohit,
               ((long) spr->clipdist) << 2, 2);
     spr->cstat = tempshort;
     daz = spr->z + zoffs;
     if ((daz <= globhiz) || (daz > globloz)) {
          if (retval != 0) {
               return (retval);
          }
          return (16384 + dasectnum);
     }
     spr->z = daz - zoffs;
     return (retval);
}

void 
guardianfire(short i, int k, struct player * plr)
{

     short daang,
          j;
     int  discrim;

     daang = (daang + (k << 2)) & 2047;
     j = insertsprite(sprite[i].sectnum, MISSILE);
     sprite[j].x = sprite[i].x;
     sprite[j].y = sprite[i].y;
     sprite[j].z = sprite[i].z + (8 << 8) + ((krand() & 10) << 8);
     sprite[j].cstat = 0;
     sprite[j].picnum = PLASMA;
     sprite[j].shade = -32;
     sprite[j].pal = 0;
     sprite[j].xrepeat = 16;
     sprite[j].yrepeat = 16;
     sprite[j].ang = daang;
     sprite[j].xvel = (sintable[(daang + 2560) & 2047] >> 5);
     sprite[j].yvel = (sintable[(daang + 2048) & 2047] >> 5);
     // sprite[j].zvel=((100-plr->horiz)<<4);
     discrim = ksqrt((plr->x - sprite[j].x) * (plr->x - sprite[j].x) + (plr->y - sprite[j].y) * (plr->y - sprite[j].y));
     if (discrim == 0)
          discrim = 1;
     sprite[j].zvel = ((plr->z + (48 << 8) - sprite[j].z) << 7) / discrim;

     sprite[j].owner = 4096;
     sprite[j].lotag = 256;
     sprite[j].hitag = 0;
     sprite[j].clipdist = 48;

}


void
gonzopike(short s)
{
     long j,
          daz;
     struct player *plr;

     plr = &player[0];

     j = insertsprite(sprite[s].sectnum, JAVLIN);

     sprite[j].x = sprite[s].x;
     sprite[j].y = sprite[s].y;
     sprite[j].z = sprite[s].z - (40<<8);

     sprite[j].cstat = 21;

     sprite[j].picnum = THROWPIKE;
     sprite[j].ang = ((sprite[s].ang + 2048 + 96) - 512) & 2047;
     sprite[j].xrepeat = 24;
     sprite[j].yrepeat = 24;
     sprite[j].clipdist = 32;

     sprite[j].extra = sprite[s].ang;
     sprite[j].shade = -15;
     sprite[j].xvel = ((krand() & 256) - 128);
     sprite[j].yvel = ((krand() & 256) - 128);
     // sprite[j].zvel=((krand()&256)-128);
     // sprite[j].zvel=((100-plr->horiz)<<4);

     sprite[j].zvel = ((plr->z + (8 << 8) - sprite[s].z) << 7) /
          ksqrt((plr->x - sprite[s].x) *
                (plr->x - sprite[s].x) +
                (plr->y - sprite[s].y) *
                (plr->y - sprite[s].y));

     sprite[j].zvel += ((rand() % 256) - 128);

     sprite[j].owner = s;
     sprite[j].lotag = 1024;
     sprite[j].hitag = 0;
     sprite[j].pal = 0;


}

void
newguyarrow(short s)
{
     long j,
          daz;
     struct player *plr;

     plr = &player[0];

     j = insertsprite(sprite[s].sectnum, JAVLIN);

     sprite[j].x = sprite[s].x;
     sprite[j].y = sprite[s].y;
     sprite[j].z = sprite[s].z - (40<<8);

     sprite[j].cstat = 21;

     sprite[j].picnum = WALLARROW;
     sprite[j].ang = ((sprite[s].ang + 2048 + 96) - 512) & 2047;
     sprite[j].xrepeat = 24;
     sprite[j].yrepeat = 24;
     sprite[j].clipdist = 32;

     sprite[j].extra = sprite[s].ang;
     sprite[j].shade = -15;
     sprite[j].xvel = ((krand() & 256) - 128);
     sprite[j].yvel = ((krand() & 256) - 128);

     sprite[j].zvel = ((plr->z + (8 << 8) - sprite[s].z) << 7) /
          ksqrt((plr->x - sprite[s].x) *
                (plr->x - sprite[s].x) +
                (plr->y - sprite[s].y) *
                (plr->y - sprite[s].y));

     sprite[j].zvel += ((rand() % 256) - 128);

     sprite[j].owner = s;
     sprite[j].lotag = 1024;
     sprite[j].hitag = 0;
     sprite[j].pal = 0;


}

void
trowajavlin(int s)
{

     long j;

     j = insertsprite(sprite[s].sectnum, JAVLIN);

     sprite[j].x = sprite[s].x;
     sprite[j].y = sprite[s].y;
     sprite[j].z = sprite[s].z - (40<<8);

     sprite[j].cstat = 21;

     switch (sprite[s].lotag) {
     case 91:
          sprite[j].picnum = WALLARROW;
          sprite[j].ang = ((sprite[s].ang + 2048) - 512) & 2047;
          sprite[j].xrepeat = 16;
          sprite[j].yrepeat = 48;
          sprite[j].clipdist = 24;
          sprite[j].owner = s;

          break;
     case 92:
          sprite[j].picnum = DART;
          sprite[j].ang = ((sprite[s].ang + 2048) - 512) & 2047;
          sprite[j].xrepeat = 64;
          sprite[j].yrepeat = 64;
          sprite[j].clipdist = 16;
          sprite[j].owner = s;
          break;
     case 93:
          sprite[j].picnum = HORIZSPIKEBLADE;
          sprite[j].ang = ((sprite[s].ang + 2048) - 512) & 2047;
          sprite[j].xrepeat = 16;
          sprite[j].yrepeat = 48;
          sprite[j].clipdist = 32;
          sprite[j].owner = s;

          break;
     case 94:
          sprite[j].picnum = THROWPIKE;
          sprite[j].ang = ((sprite[s].ang + 2048) - 512) & 2047;
          sprite[j].xrepeat = 24;
          sprite[j].yrepeat = 24;
          sprite[j].clipdist = 32;
          sprite[j].owner = s;

          break;
     }

     sprite[j].extra = sprite[s].ang;
     sprite[j].shade = -15;
     sprite[j].xvel = ((krand() & 256) - 128);
     sprite[j].yvel = ((krand() & 256) - 128);
     sprite[j].zvel = ((krand() & 256) - 128);
     // sprite[j].owner=sprite[s].owner;
     sprite[j].owner = 0;
     sprite[j].lotag = 0;
     sprite[j].hitag = 0;
     sprite[j].pal = 0;

}

void 
throwhalberd(int s)
{

     long j,
          daz;

     j = insertsprite(sprite[s].sectnum, JAVLIN);

     sprite[j].x = sprite[s].x;
     sprite[j].y = sprite[s].y;
     sprite[j].z = sprite[s].z - (40 << 8);

     sprite[j].cstat = 17;

     sprite[j].picnum = THROWHALBERD;
     sprite[j].ang = ((sprite[s].ang + 2048) - 512) & 2047;
     sprite[j].xrepeat = 8;
     sprite[j].yrepeat = 16;
     sprite[j].clipdist = 32;

     sprite[j].extra = sprite[s].ang;
     sprite[j].shade = -15;
     sprite[j].xvel = ((krand() & 256) - 128);
     sprite[j].yvel = ((krand() & 256) - 128);
     sprite[j].zvel = ((krand() & 256) - 128);
     sprite[j].owner = s;
     sprite[j].lotag = 0;
     sprite[j].hitag = 0;
     sprite[j].pal = 0;

     sprite[j].cstat = 0;
     daz = ((((long) sprite[j].zvel) * synctics) >> 3);
     movesprite((short) j,
          (((long) sintable[(sprite[j].extra + 512) & 2047]) * synctics) << 7,
                (((long) sintable[sprite[j].extra]) * synctics) << 7,
                daz, 4L << 8, 4L << 8, 1);
     sprite[j].cstat = 21;


}

void
spawnhornskull(short i)
{
     short j;

     j = insertsprite(sprite[i].sectnum, 0);
     sprite[j].x = sprite[i].x;
     sprite[j].y = sprite[i].y;
     sprite[j].z = sprite[i].z - (24 << 8);
     sprite[j].shade = -15;
     sprite[j].cstat = 0;
     sprite[j].cstat &= ~3;
     sprite[j].pal = 0;
     sprite[j].picnum = HORNEDSKULL;
     sprite[j].xrepeat = 64;
     sprite[j].yrepeat = 64;
}

extern
int  treasurescnt;

void
monsterweapon(short i)
{

     short j;
     int  k;

     if (sprite[i].shade > 20)
          return;

     switch (sprite[i].picnum) {
     case SKELETONDEAD:
     case KOBOLDDEAD:
          return;
     }
     if ((rand() % 100) < 75) {
          return;
     }
     
     j = insertsprite(sprite[i].sectnum, 0);

     sprite[j].x = sprite[i].x;
     sprite[j].y = sprite[i].y;
     sprite[j].z = sprite[i].z - (24 << 8);
     sprite[j].shade = -15;
     sprite[j].cstat = 0;
     sprite[j].cstat &= ~3;
     sprite[j].pal = 0;
     sprite[j].picnum = FLASKBLUE + (krand() % 4);

     switch (sprite[i].picnum) {
     case NEWGUYDEAD:
          sprite[j].xrepeat = 25;
          sprite[j].yrepeat = 20;
          if (sprite[j].extra < 20)
               sprite[j].picnum = WEAPON2;
          else
               sprite[j].picnum = QUIVER;

          sprite[j].pal = 0;
          break;

     case MINOTAURDEAD:
          sprite[j].xrepeat = 25;
          sprite[j].yrepeat = 20;
          sprite[j].picnum = WEAPON4;
          sprite[j].pal = 0;
          break;

     case GONZOBSHDEAD:
          sprite[j].picnum = GONZOBSHIELD;
          sprite[j].xrepeat = 12;
          sprite[j].yrepeat = 12;
          break;

     case GONZOCSWDEAD:
          if (sprite[j].extra > 10) {
               sprite[j].picnum = WEAPON6;
               sprite[j].xrepeat = 25;
               sprite[j].yrepeat = 20;
          }
          else if (sprite[j].extra > 0) {
               sprite[j].picnum = GOBWEAPON;
               sprite[j].xrepeat = 25;
               sprite[j].yrepeat = 20;
          }
          else {
               sprite[j].picnum = WEAPON1;
               sprite[j].xrepeat = 25;
               sprite[j].yrepeat = 20;
          }
          break;
     case GONZOCSHDEAD:
          sprite[j].picnum = GONZOCSHIELD;
          sprite[j].xrepeat = 12;
          sprite[j].yrepeat = 12;
          break;

     case GONZOGSWDEAD:
          sprite[j].picnum = WEAPON8;
          sprite[j].xrepeat = 25;
          sprite[j].yrepeat = 20;
          break;
     case GONZOGHMDEAD:
          sprite[j].picnum = PLATEARMOR;
          sprite[j].xrepeat = 26;
          sprite[j].yrepeat = 26;
          break;
     case GONZOGSHDEAD:
          sprite[j].picnum = GONZOGSHIELD;
          sprite[j].xrepeat = 12;
          sprite[j].yrepeat = 12;
          break;

     case GRONDEAD:
          if (netgame) {
               sprite[j].x = sprite[i].x;
               sprite[j].y = sprite[i].y;
               sprite[j].z = sprite[i].z - (24 << 8);
               sprite[j].shade = -15;
               sprite[j].cstat = 0;
               sprite[j].cstat &= ~3;
               sprite[j].xrepeat = 25;
               sprite[j].yrepeat = 20;
               k = krand() % 4;
               switch (k) {
               case 0:
                    sprite[j].picnum = WEAPON3;
                    sprite[j].xrepeat = 25;
                    sprite[j].yrepeat = 20;
                    break;
               case 1:
                    sprite[j].picnum = WEAPON5;
                    sprite[j].xrepeat = 25;
                    sprite[j].yrepeat = 20;
                    break;
               case 2:
                    sprite[j].picnum = WEAPON6;
                    sprite[j].xrepeat = 20;
                    sprite[j].yrepeat = 15;
                    break;
               case 3:
                    sprite[j].picnum = SHIELD;
                    sprite[j].xrepeat = 32;
                    sprite[j].yrepeat = 32;
                    break;
               }
          }
          else {
               switch (sprite[j].pal) {
               case 0:
                    sprite[j].picnum = WEAPON3;
                    sprite[j].xrepeat = 25;
                    sprite[j].yrepeat = 20;
                    break;
               case 10:
                    sprite[j].picnum = WEAPON5;
                    sprite[j].xrepeat = 25;
                    sprite[j].yrepeat = 20;
                    break;
               case 11:
                    sprite[j].picnum = WEAPON6;
                    sprite[j].xrepeat = 20;
                    sprite[j].yrepeat = 15;
                    break;
               case 12:
                    sprite[j].picnum = SHIELD;
                    sprite[j].xrepeat = 32;
                    sprite[j].yrepeat = 32;
                    break;
               }
          }
          sprite[j].pal = 0;
          break;
     default:
          treasurescnt++;
          break;
     }

}

// VAL 1-3
void
madenoise(int val, long x, long y, long z)
{

     long i,
          nexti;

     if (netgame) {
          return;
     }
     i = headspritestat[FACE];
     while (i >= 0) {
          nexti = nextspritestat[i];
          if ((labs(x - sprite[i].x) + labs(y - sprite[i].y) < (val * 4096)))
               newstatus(i, FINDME);
          i = nexti;
     }

}

void
monsternoise(short i)
{
     return;

     switch (sprite[i].picnum) {
     case KOBOLD:
          if (rand() % 100 > 98)
               playsound_loc(S_KSNARL1 + (rand() % 4), sprite[i].x, sprite[i].y);
          break;
     case IMP:
     case MINOTAUR:
     case SKELETON:
          break;
     }

}

void
randompotion(short i)
{

     short j;

     if (rand() % 100 > 20)
          return;

     j = insertsprite(sprite[i].sectnum, 0);

     sprite[j].x = sprite[i].x;
     sprite[j].y = sprite[i].y;
     sprite[j].z = sprite[i].z - (12 << 8);
     sprite[j].shade = -12;
     sprite[j].pal = 0;
     sprite[j].cstat = 0;
     sprite[j].cstat &= ~3;
     sprite[j].xrepeat = 64;
     sprite[j].yrepeat = 64;
     sprite[j].picnum = FLASKBLUE + krand() % 4;

}


void
spawnabaddy(short i, short monster)
{

     short j;

     j = insertsprite(sprite[i].sectnum, FACE);

     sprite[j].x = sprite[i].x + (krand() & 2048) - 1024;
     sprite[j].y = sprite[i].y + (krand() & 2048) - 1024;
     sprite[j].z = sprite[i].z;


     switch (monster) {
     case GRONSW:
          sprite[j].xrepeat = GRONSWXR;
          sprite[j].yrepeat = GRONSWYR;
          sprite[j].clipdist = 64;
          sprite[j].hitag = adjusthp(300);
          sprite[j].lotag = 100;
          sprite[j].cstat = 0;
          sprite[j].cstat |= 0x101;
          sprite[j].extra = 0;
          sprite[j].pal = 0;
          sprite[j].shade = 0;
          sprite[j].picnum = GRONSW;
          break;
     }
     setsprite((short) j, sprite[j].x, sprite[j].y, sprite[j].z);

}



void
spawnapentagram(short sn)
{

     short j;

     j = insertsprite(sprite[sn].sectnum, 0);

     sprite[j].x = sprite[sn].x;
     sprite[j].y = sprite[sn].y;
     sprite[j].z = sprite[sn].z - (8 << 8);
     sprite[j].xrepeat = sprite[j].yrepeat = 64;
     sprite[j].pal = 0;
     sprite[j].shade = -15;
     sprite[j].cstat = 0;
     sprite[j].clipdist = 64;
     sprite[j].lotag = 0;
     sprite[j].hitag = 0;
     sprite[j].extra = 0;
     sprite[j].picnum = PENTAGRAM;

     setsprite((short) j, sprite[j].x, sprite[j].y, sprite[j].z);

}


int
isvalidactor(short i)
{

     int  valid;

     switch (sprite[i].picnum) {
     case SKELETON:
     case KOBOLD:
     case IMP:
     case GRONHAL:
     case GRONMU:
     case GRONSW:
          valid = 1;
          break;
     default:
          valid = 0;
          break;
     }

     return (valid);

}

int
actoruse(short i)
{

     spritetype *spr;

     spr = &sprite[i];

     neartag(spr->x,
             spr->y,
             spr->z,
             spr->sectnum,
             spr->ang,
             &neartagsector,
             &neartagwall,
             &neartagsprite,
             &neartaghitdist,
             1024,
             3);

     if (neartagsector >= 0) {
          if (sector[neartagsector].hitag == 0) {
               if (sector[neartagsector].floorz != sector[neartagsector].ceilingz) {
                    operatesector(neartagsector);
                    return (1);
               }
          }
     }

     return (0);

}




void
deaddude(short sn)
{

     long j;

     j = insertsprite(sprite[sn].sectnum, DEAD);
     sprite[j].x = sprite[sn].x;
     sprite[j].y = sprite[sn].y;
     sprite[j].z = sprite[sn].z;
     sprite[j].cstat &= ~3;
     sprite[j].picnum = GONZOBSHDEAD;
     sprite[j].shade = 0;
     sprite[j].pal = 0;
     sprite[j].xrepeat = sprite[sn].xrepeat;
     sprite[j].yrepeat = sprite[sn].yrepeat;
     sprite[j].owner = 0;
     sprite[j].lotag = 0;
     sprite[j].hitag = 0;

}

int
isActor(int spritenum)
{
     switch (sprite[spritenum].picnum) {
     case KOBOLD:
     case KOBOLDATTACK:
     case DEVIL:
     case DEVILATTACK:
     case IMP:
     case IMPATTACK:
     case MINOTAUR:
     case MINOTAURATTACK:
     case SKELETON:
     case SKELETONATTACK:
     case GRONHAL:
     case GRONHALATTACK:
     case GRONMU:
     case GRONMUATTACK:
     case GRONSW:
     case GRONSWATTACK:
     case DEMON:
     case GUARDIAN:
     case GUARDIANATTACK:
     case WILLOW:
     case NEWGUYSTAND:
     case NEWGUYKNEE:
     case NEWGUY:
     case NEWGUYCAST:
     case NEWGUYBOW:
     case NEWGUYMACE:
     case NEWGUYPUNCH:
     case KURTSTAND:
     case KURTKNEE:
     case KURTAT:
     case KURTPUNCH:
     case KURTREADY:
     case GONZOCSW:
     case GONZOCSWAT:
     case GONZOCHMAT:
     case GONZOGSW:
     case GONZOGSWAT:
     case GONZOGHM:
     case GONZOGHMAT:
     case GONZOGSH:
     case GONZOGSHAT:
     case KATIE:
     case KATIEAT:
          return(1);
     }
     return(0);
}

