/***************************************************************************
 *   WHPLR.C  - code for player character                                  *
 *                                                                         *
 ***************************************************************************/

#include "icorp.h"
#include <memcheck.h>

int  dropshieldcnt = 0;
int  droptheshield = 0;

int  enchantedsoundhandle=-1;

extern int shieldtype;
extern short gametype;
extern int godmode;

#define EMPTY   0

int  spelltime;

extern short torchpattern[];
extern int vampiretime;

extern int loadedgame;
extern int goreon;
extern int helmettime;
extern int scoretime;
extern int difficulty;
int  lives;
int  madeahit = 0;
extern int lavasnd;
extern int expgained;

int  pyrn;
int  dahand = 0;
int  mapon = 1;

long neartagdist,
     neartaghitdist;

short neartagsector,
     neartagsprite,
     neartagwall;

struct player player[MAXPLAYERS];

extern int justteleported;
extern int gameactivated;
extern long selectedgun;
extern short oldmousestatus;
extern long oldhoriz;

extern int displaytime;

extern int weapontimex;
extern int weapontimey;
extern int weapontilenum;

extern int potiontilenum;
extern int shadowtime;

extern int playerdie;
extern int frames;
extern long vel;

int  hasshot;
int  orbshot;

int  weapondrop;
int  weapondropgoal;
int  weaponraise;

int  currweapon;
int  currweapontics;
int  currweaponanim;
short currweaponframe;
int  currweaponfired;
int  currweaponattackstyle;
int  currweaponflip;

int  spellbooktics;
int  spellbook;
int  spellbookframe;
int  spellbookflip;

int  spiketics;
int  spikeframe;
int  currspikeframe;
int  spiked;


int  strongtime,
     invincibletime,
     manatime,
     invisibletime = -1,
     nightglowtime;

int  arrowcnt;

short arrowsprite[ARROWCOUNTLIMIT];

#define   THROWPIKELIMIT      100

int  throwpikecnt;

short throwpikesprite[THROWPIKELIMIT];

#define MAXFRAMES 12

struct daweapons {
     int  daweapontics;
     short daweaponframe;
     long currx;
     long curry;
};

struct daweapons spikeanimtics[5] =
{{10, DIESPIKE, 136, 145},
{10, DIESPIKE + 1, 136, 124},
{10, DIESPIKE + 2, 136, 100},
{10, DIESPIKE + 3, 136, 70},
{10, DIESPIKE + 4, 136, 50}};

//
struct daweapons spellbookanim[MAXNUMORBS][9] =
{
     // SCARE
     {{8, SPELLBOOK8, 121, 161}, {8, SPELLBOOK8 + 1, 121, 161},
     {8, SPELLBOOK8 + 2, 121, 156}, {8, SPELLBOOK8 + 3, 121, 158},
     {8, SPELLBOOK8 + 4, 121, 159}, {8, SPELLBOOK8 + 5, 121, 161},
     {8, SPELLBOOK8 + 6, 121, 160}, {8, SPELLBOOK8 + 7, 121, 161},
     {8, SPELLBOOK8 + 7, 121, 161}
     },
     // NIGHT VISION
     {{8, SPELLBOOK6, 121, 161}, {8, SPELLBOOK6 + 1, 121, 161},
     {8, SPELLBOOK6 + 2, 121, 156}, {8, SPELLBOOK6 + 3, 121, 158},
     {8, SPELLBOOK6 + 4, 121, 159}, {8, SPELLBOOK6 + 5, 121, 161},
     {8, SPELLBOOK6 + 6, 121, 160}, {8, SPELLBOOK6 + 7, 121, 161},
     {8, SPELLBOOK6 + 7, 121, 161}
     },
     // FREEZE
     {{8, SPELLBOOK3, 121, 161}, {8, SPELLBOOK3 + 1, 121, 161},
     {8, SPELLBOOK3 + 2, 121, 156}, {8, SPELLBOOK3 + 3, 121, 158},
     {8, SPELLBOOK3 + 4, 121, 159}, {8, SPELLBOOK3 + 5, 120, 161},
     {8, SPELLBOOK3 + 6, 120, 160}, {8, SPELLBOOK3 + 7, 120, 161},
     {8, SPELLBOOK3 + 7, 121, 161}
     },
     // MAGIC ARROW
     {{8, SPELLBOOKBLANK, 121, 161}, {8, SPELLBOOKBLANK + 1, 121, 161},
     {8, SPELLBOOKBLANK + 2, 121, 156}, {8, SPELLBOOKBLANK + 3, 121, 158},
     {8, SPELLBOOKBLANK + 4, 121, 159}, {8, SPELLBOOKBLANK + 5, 121, 161},
     {8, SPELLBOOKBLANK + 6, 120, 160}, {8, SPELLBOOKBLANK + 7, 121, 161},
     {8, SPELLBOOKBLANK + 7, 122, 161}
     },
     // OPEN DOORS
     {{8, SPELLBOOK7, 121, 161}, {8, SPELLBOOK7 + 1, 121, 161},
     {8, SPELLBOOK7 + 2, 121, 156}, {8, SPELLBOOK7 + 3, 121, 158},
     {8, SPELLBOOK7 + 4, 121, 159}, {8, SPELLBOOK7 + 5, 121, 161},
     {8, SPELLBOOK7 + 6, 121, 160}, {8, SPELLBOOK7 + 7, 121, 161},
     {8, SPELLBOOK7 + 7, 121, 161}
     },
     // FLY
     {{8, SPELLBOOK2, 121, 161}, {8, SPELLBOOK2 + 1, 121, 161},
     {8, SPELLBOOK2 + 2, 121, 156}, {8, SPELLBOOK2 + 3, 121, 158},
     {8, SPELLBOOK2 + 4, 121, 159}, {8, SPELLBOOK2 + 5, 121, 161},
     {8, SPELLBOOK2 + 6, 121, 160}, {8, SPELLBOOK2 + 7, 121, 161},
     {8, SPELLBOOK2 + 7, 121, 161}
     },
     // FIRE BALL
     {{8, SPELLBOOK4, 121, 161}, {8, SPELLBOOK4 + 1, 121, 161},
     {8, SPELLBOOK4 + 2, 121, 156}, {8, SPELLBOOK4 + 3, 121, 158},
     {8, SPELLBOOK4 + 4, 121, 159}, {8, SPELLBOOK4 + 5, 121, 161},
     {8, SPELLBOOK4 + 6, 121, 160}, {8, SPELLBOOK4 + 7, 121, 161},
     {8, SPELLBOOK4 + 7, 121, 161}
     },
     // NUKE!
     {{8, SPELLBOOK5, 121, 161}, {8, SPELLBOOK5 + 1, 121, 161},
     {8, SPELLBOOK5 + 2, 121, 156}, {8, SPELLBOOK5 + 3, 121, 158},
     {8, SPELLBOOK5 + 4, 121, 159}, {8, SPELLBOOK5 + 5, 121, 161},
     {8, SPELLBOOK5 + 6, 121, 160}, {8, SPELLBOOK5 + 7, 121, 161},
     {8, SPELLBOOK5 + 7, 121, 161}
     }

};

struct daweapons sspellbookanim[MAXNUMORBS][9] =
{
     // SCARE
     {{8, SSPELLBOOK8, 121, 389}, {8, SSPELLBOOK8 + 1, 121, 377},
     {8, SSPELLBOOK8 + 2, 121, 383}, {8, SSPELLBOOK8 + 3, 121, 385},
     {8, SSPELLBOOK8 + 4, 121, 389}, {8, SSPELLBOOK8 + 5, 121, 387},
     {8, SSPELLBOOK8 + 6, 121, 389}, {8, SSPELLBOOK8 + 7, 121, 389},
     {8, SSPELLBOOK8 + 7, 121, 389}
     },
     // NIGHT VISION
     {{8, SSPELLBOOK6, 121, 389}, {8, SSPELLBOOK6 + 1, 121, 377},
     {8, SSPELLBOOK6 + 2, 121, 383}, {8, SSPELLBOOK6 + 3, 121, 385},
     {8, SSPELLBOOK6 + 4, 121, 389}, {8, SSPELLBOOK6 + 5, 121, 387},
     {8, SSPELLBOOK6 + 6, 121, 389}, {8, SSPELLBOOK6 + 7, 121, 389},
     {8, SSPELLBOOK6 + 7, 121, 389}
     },
     // FREEZE
     {{8, SSPELLBOOK3, 121, 389}, {8, SSPELLBOOK3 + 1, 121, 377},
     {8, SSPELLBOOK3 + 2, 121, 383}, {8, SSPELLBOOK3 + 3, 121, 385},
     {8, SSPELLBOOK3 + 4, 121, 389}, {8, SSPELLBOOK3 + 5, 120, 387},
     {8, SSPELLBOOK3 + 6, 120, 389}, {8, SSPELLBOOK3 + 7, 120, 389},
     {8, SSPELLBOOK3 + 7, 121, 389}
     },
     // MAGIC ARROW
     {{8, SSPELLBOOKBLANK, 121, 389}, {8, SSPELLBOOKBLANK + 1, 121, 377},
     {8, SSPELLBOOKBLANK + 2, 121, 383}, {8, SSPELLBOOKBLANK + 3, 121, 385},
     {8, SSPELLBOOKBLANK + 4, 121, 389}, {8, SSPELLBOOKBLANK + 5, 121, 387},
     {8, SSPELLBOOKBLANK + 6, 120, 389}, {8, SSPELLBOOKBLANK + 7, 121, 389},
     {8, SSPELLBOOKBLANK + 7, 122, 389}
     },
     // OPEN DOORS
     {{8, SSPELLBOOK7, 121, 389}, {8, SSPELLBOOK7 + 1, 121, 377},
     {8, SSPELLBOOK7 + 2, 121, 383}, {8, SSPELLBOOK7 + 3, 121, 385},
     {8, SSPELLBOOK7 + 4, 121, 389}, {8, SSPELLBOOK7 + 5, 121, 387},
     {8, SSPELLBOOK7 + 6, 121, 389}, {8, SSPELLBOOK7 + 7, 121, 389},
     {8, SSPELLBOOK7 + 7, 121, 389}
     },
     // FLY
     {{8, SSPELLBOOK2, 121, 389}, {8, SSPELLBOOK2 + 1, 121, 377},
     {8, SSPELLBOOK2 + 2, 121, 383}, {8, SSPELLBOOK2 + 3, 121, 385},
     {8, SSPELLBOOK2 + 4, 121, 389}, {8, SSPELLBOOK2 + 5, 121, 387},
     {8, SSPELLBOOK2 + 6, 121, 389}, {8, SSPELLBOOK2 + 7, 121, 389},
     {8, SSPELLBOOK2 + 7, 121, 389}
     },
     // FIRE BALL
     {{8, SSPELLBOOK4, 121, 389}, {8, SSPELLBOOK4 + 1, 121, 377},
     {8, SSPELLBOOK4 + 2, 121, 383}, {8, SSPELLBOOK4 + 3, 121, 385},
     {8, SSPELLBOOK4 + 4, 121, 389}, {8, SSPELLBOOK4 + 5, 121, 387},
     {8, SSPELLBOOK4 + 6, 121, 389}, {8, SSPELLBOOK4 + 6, 121, 389},
     {8, SSPELLBOOK4 + 6, 121, 389}
     },
     // NUKE!
     {{8, SSPELLBOOK5, 121, 389}, {8, SSPELLBOOK5 + 1, 121, 377},
     {8, SSPELLBOOK5 + 2, 121, 383}, {8, SSPELLBOOK5 + 3, 121, 385},
     {8, SSPELLBOOK5 + 4, 121, 389}, {8, SSPELLBOOK5 + 5, 121, 387},
     {8, SSPELLBOOK5 + 6, 121, 389}, {8, SSPELLBOOK5 + 6, 121, 389},
     {8, SSPELLBOOK5 + 6, 121, 389}
     }

};


struct daweapons throwanimtics[MAXNUMORBS][MAXFRAMES + 1] =
{
#if 0
     // MUTWOHANDS  - scare spell
     {{10, MUTWOHANDS, 19, 155}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUTWOHANDS  - scare spell
     {{1, EMPTY, 127, 170}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUMEDUSA    - night vision
     {{10, ZLIGHT, 0, 177}, {10, ZLIGHT + 1, 0, 137}, {10, ZLIGHT + 2, 48, 82},
     {10, ZLIGHT + 3, 127, 41}, {10, ZLIGHT + 4, 210, 9}, {10, ZLIGHT + 5, 284, 26},
     {10, ZLIGHT + 6, 213, 63}, {10, ZLIGHT + 7, 147, 99}, {10, ZLIGHT + 8, 91, 136},
     {10, ZLIGHT + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUTWOHANDS  - night vision
     {{10, ZFIREBALL, 0, 177}, {10, ZFIREBALL + 1, 0, 137}, {10, ZFIREBALL + 2, 48, 82},
     {10, ZFIREBALL + 3, 127, 41}, {10, ZFIREBALL + 4, 210, 9}, {10, ZFIREBALL + 5, 284, 26},
     {10, ZFIREBALL + 6, 213, 63}, {10, ZFIREBALL + 7, 147, 99}, {10, ZFIREBALL + 8, 91, 136},
     {10, ZFIREBALL + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUTWOHANDS  - freeze
     {{10, ZFREEZE, 0, 101}, {10, ZFREEZE + 1, 0, 51}, {10, ZFREEZE + 2, 0, 71},
     {10, ZFREEZE + 3, 4, 85}, {10, ZFREEZE + 4, 32, 78}, {10, ZFREEZE + 5, 51, 80},
     {10, ZFREEZE + 6, 50, 80}, {10, ZFREEZE + 7, 49, 89}, {10, ZFREEZE + 8, 49, 89},
     {10, ZFREEZE + 9, 49, 98}, {10, ZFREEZE + 10, 47, 105}, {10, ZFREEZE + 11, 48, 121},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUTWOHANDS  - freeze
     {{1, EMPTY, 127, 170}, {10, ZFREEZE + 1, 0, 51}, {10, ZFREEZE + 2, 0, 71},
     {10, ZFREEZE + 3, 4, 85}, {10, ZFREEZE + 4, 32, 78}, {10, ZFREEZE + 5, 51, 80},
     {10, ZFREEZE + 6, 50, 80}, {10, ZFREEZE + 7, 49, 89}, {10, ZFREEZE + 8, 49, 89},
     {10, ZFREEZE + 9, 49, 98}, {10, ZFREEZE + 10, 47, 105}, {10, ZFREEZE + 11, 48, 121},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUTWOHANDS  - magic arrow
     {{10, MUTWOHANDS, 19, 155}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUTWOHANDS  - magic arrow
     {{1, EMPTY, 127, 170}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
     // MUTWOHANDS  - open door
     {{15, MUTWOHANDS, 19, 155}, {15, MUTWOHANDS + 1, 0, 128}, {15, MUTWOHANDS + 2, 0, 93},
     {15, MUTWOHANDS + 3, 0, 83}, {15, MUTWOHANDS + 4, 0, 72}, {15, MUTWOHANDS + 5, 0, 83},
     {15, MUTWOHANDS + 6, 10, 96}, {15, MUTWOHANDS + 7, 43, 109}, {15, MUTWOHANDS + 8, 69, 113},
     {15, MUTWOHANDS + 9, 65, 115}, {15, MUTWOHANDS + 10, 64, 117}, {15, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUMEDUSA    - fly
     {{10, MUMEDUSA, 0, 177}, {10, MUMEDUSA + 1, 0, 137}, {10, MUMEDUSA + 2, 48, 82},
     {10, MUMEDUSA + 3, 127, 41}, {10, MUMEDUSA + 4, 210, 9}, {10, MUMEDUSA + 5, 284, 26},
     {10, MUMEDUSA + 6, 213, 63}, {10, MUMEDUSA + 7, 147, 99}, {10, MUMEDUSA + 8, 91, 136},
     {10, MUMEDUSA + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUMEDUSA    - fly
     {{10, ZLIGHT, 0, 177}, {10, ZLIGHT + 1, 0, 137}, {10, ZLIGHT + 2, 48, 82},
     {10, ZLIGHT + 3, 127, 41}, {10, ZLIGHT + 4, 210, 9}, {10, ZLIGHT + 5, 284, 26},
     {10, ZLIGHT + 6, 213, 63}, {10, ZLIGHT + 7, 147, 99}, {10, ZLIGHT + 8, 91, 136},
     {10, ZLIGHT + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUTWOHANDS  - fireball
     {{10, ZFIREBALL, 0, 177}, {10, ZFIREBALL + 1, 0, 137}, {10, ZFIREBALL + 2, 48, 82},
     {10, ZFIREBALL + 3, 127, 41}, {10, ZFIREBALL + 4, 210, 9}, {10, ZFIREBALL + 5, 284, 26},
     {10, ZFIREBALL + 6, 213, 63}, {10, ZFIREBALL + 7, 147, 99}, {10, ZFIREBALL + 8, 91, 136},
     {10, ZFIREBALL + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUTWOHANDS  - fireball
     {{1, EMPTY, 127, 170}, {10, ZFIREBALL + 1, 0, 137}, {10, ZFIREBALL + 2, 48, 82},
     {10, ZFIREBALL + 3, 127, 41}, {10, ZFIREBALL + 4, 210, 9}, {10, ZFIREBALL + 5, 284, 26},
     {10, ZFIREBALL + 6, 213, 63}, {10, ZFIREBALL + 7, 147, 99}, {10, ZFIREBALL + 8, 91, 136},
     {10, ZFIREBALL + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUTWOHANDS  - nuke
     {{10, MUTWOHANDS, 19, 155}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     }
#endif
     // MUTWOHANDS  - nuke
     {{1, EMPTY, 127, 170}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     }
};

struct daweapons lefthandanimtics[5][MAXFRAMES] =
{
     {{10, RFIST, 15, 121}, {10, RFIST + 1, 17, 114}, {10, RFIST + 2, 54, 131},
     {10, RFIST + 3, 76, 152}, {10, RFIST + 4, 31, 126}, {10, RFIST + 5, 26, 135},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // KNIFE
     {{8, KNIFEATTACK2, 0, 113}, {8, KNIFEATTACK2 + 1, 44, 111}, {8, KNIFEATTACK2 + 2, 119, 137},
     {8, KNIFEATTACK2 + 3, 187, 159}, {16, EMPTY, 136, 100}, {8, KNIFEATTACK2 + 3, 187, 159},
     {8, KNIFEATTACK2 + 2, 119, 137}, {8, KNIFEATTACK2 + 1, 44, 111}, {8, KNIFEATTACK2, 0, 113},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // GOBLINATTACK
     {{10, GOBSWORDATTACK, 243, 92}, {10, GOBSWORDATTACK + 1, 255, 68}, {10, GOBSWORDATTACK + 2, 279, 65},
     {10, GOBSWORDATTACK + 3, 238, 55}, {10, GOBSWORDATTACK + 4, 153, 52}, {10, GOBSWORDATTACK + 5, 129, 152},
     {10, GOBSWORDATTACK + 6, 90, 184}, {1, EMPTY, 297, 169}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     },
     // MORNINGATTACK2

     {{12, MORNINGATTACK2, 38, 141}, {12, MORNINGATTACK2 + 1, 0, 111}, {12, MORNINGATTACK2 + 2, 0, 91},
     {12, MORNINGATTACK2 + 3, 0, 47}, {12, MORNINGATTACK2 + 4, 0, 24}, {1, EMPTY, 0, 24},
     {1, EMPTY, 0, 24}, {1, EMPTY, 0, 24}, {1, EMPTY, 0, 24},
     {1, EMPTY, 0, 24}, {1, EMPTY, 0, 24}, {1, EMPTY, 0, 24}

     },

     // GOBLINATTACK2
     {{10, GOBSWORDATTACK2, 236, 99}, {10, GOBSWORDATTACK2 + 1, 202, 24}, {10, GOBSWORDATTACK2 + 2, 181, 0},
     {10, GOBSWORDATTACK2 + 3, 52, 12}, {10, GOBSWORDATTACK2 + 4, 72, 72}, {10, GOBSWORDATTACK2 + 5, 134, 139},
     {10, GOBSWORDATTACK2 + 6, 297, 169}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     }

};


#define LFIST   EMPTY

struct daweapons cockanimtics[MAXFRAMES + 1] =
{
     {24, EMPTY, 10, 10},
     {12, BOWREADYEND + 1, 101, 115}, {12, BOWREADYEND + 2, 112, 0},
     {12, BOWREADYEND + 3, 115, 0}, {12, BOWREADYEND + 4, 75, 13}
};

struct daweapons readyanimtics[MAXWEAPONS][MAXFRAMES + 1] =
{
     // FIST
     {{10, RFIST, 216, 180}, {10, RFIST, 216, 170}, {10, RFIST, 216, 160},
     {10, RFIST, 216, 150}, {10, RFIST, 216, 140}, {10, RFIST, 216, 130},
     {10, RFIST, 216, 124}, {1, RFIST, 216, 124}, {1, RFIST, 216, 124},
     {1, RFIST, 216, 122}, {1, RFIST, 216, 122}, {1, RFIST, 216, 122},
     {1, EMPTY, 147, 76}
     },
     // KNIFE
     {{10, KNIFEREADY, 69, 171}, {10, KNIFEREADY + 1, 11, 146}, {10, KNIFEREADY + 2, 25, 146},
     {10, KNIFEREADY + 3, 35, 158}, {10, KNIFEREADY + 4, 38, 158}, {10, KNIFEREADY + 5, 16, 157},
     {10, KNIFEREADY + 6, 37, 102}, {10, KNIFEREADY + 7, 239, 63}, {10, KNIFEREADY + 8, 214, 85},
     {10, KNIFEREADY + 9, 206, 110}, {10, KNIFEREADY + 10, 217, 108}, {10, KNIFEREADY + 11, 204, 95},
     {1, EMPTY, 147, 76}
     },
     // GOBSWORD
     {{12, GOBSWORDPULL, 79, 169}, {12, GOBSWORDPULL + 1, 95, 115}, {12, GOBSWORDPULL + 2, 94, 93},
     {12, GOBSWORDPULL + 3, 156, 77}, {12, GOBSWORDPULL + 4, 218, 64}, {12, GOBSWORDPULL + 5, 224, 57},
     {8, GOBSWORDPULL + 6, 251, 54}, {1, GOBSWORDPULL + 7, 243, 92}, {1, GOBSWORDPULL + 7, 243, 92},
     {1, GOBSWORDPULL + 7, 243, 92}, {1, GOBSWORDPULL + 7, 243, 92}, {1, GOBSWORDPULL + 7, 243, 92},
     {1, EMPTY, 147, 76}
     },
     // MORNINGSTAR
     {{6, MORNINGSTAR, 193, 190}, {6, MORNINGSTAR, 193, 180}, {6, MORNINGSTAR, 193, 170},
     {6, MORNINGSTAR, 193, 160}, {6, MORNINGSTAR, 193, 150}, {6, MORNINGSTAR, 193, 140},
     {6, MORNINGSTAR, 193, 130}, {6, MORNINGSTAR, 193, 120}, {6, MORNINGSTAR, 193, 110},
     {6, MORNINGSTAR, 193, 100}, {6, MORNINGSTAR, 193, 90}, {1, MORNINGSTAR, 193, 90},
     {1, EMPTY, 147, 76}
     },
     // SWORD
     {{10, SWORDPULL, 58, 160}, {10, SWORDPULL + 1, 81, 111}, {10, SWORDPULL + 2, 19, 88},
     {10, SWORDPULL + 3, 0, 93}, {10, SWORDPULL + 4, 104, 0}, {10, SWORDPULL + 5, 169, 0},
     {10, SWORDPULL + 6, 244, 38}, {6, SWORDPULL + 7, 225, 121}, {1, SWORDPULL + 7, 225, 121},
     {1, SWORDPULL + 7, 225, 121}, {1, SWORDPULL + 7, 225, 121}, {1, SWORDPULL + 7, 225, 121},
     {1, EMPTY, 147, 76}
     },
     {{12, BIGAXEDRAW, 71, 108}, {12, BIGAXEDRAW + 1, 17, 58}, {12, BIGAXEDRAW + 2, 0, 56},
     {12, BIGAXEDRAW + 3, 0, 71}, {12, BIGAXEDRAW + 4, 0, 102}, {12, BIGAXEDRAW + 5, 0, 11},
     {12, BIGAXEDRAW + 6, 33, 0}, {12, BIGAXEDRAW + 7, 69, 0}, {12, BIGAXEDRAW + 8, 75, 20},
     {12, BIGAXEDRAW9, 150, 92}, {12, BIGAXEDRAW10, 182, 116}, {1, EMPTY, 200, 122},
     {1, EMPTY, 147, 76}
     },
     // BOW
     {{12, BOWREADY, 0, 0}, {12, BOWREADY + 1, 0, 20}, {12, BOWREADY + 2, 0, 46},
     {12, BOWREADY + 3, 0, 26}, {12, BOWREADY + 4, 0, 0}, {12, BOWREADY + 5, 71, 0},
     {8, BOWREADYEND, 77, 23}, {1, BOWREADYEND, 77, 23}, {1, BOWREADYEND, 77, 23},
     {1, BOWREADYEND, 77, 23}, {1, BOWREADYEND, 77, 23}, {1, BOWREADYEND, 77, 23},
     {1, EMPTY, 147, 76}
     },
     {{8, PIKEDRAW, 0, 156}, {8, PIKEDRAW + 1, 15, 98}, {8, PIKEDRAW + 2, 83, 49},
     {8, PIKEDRAW + 3, 144, 66}, {8, PIKEDRAW + 4, 197, 99}, {8, PIKEDRAW + 5, 216, 131},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 147, 76}
     },
     {{12, EXCALDRAW, 167, 130}, {12, EXCALDRAW + 1, 70, 117}, {12, EXCALDRAW + 2, 0, 128},
     {12, EXCALDRAW + 3, 0, 150}, {12, EXCALDRAW + 4, 4, 72}, {12, EXCALDRAW + 5, 38, 81},
     {12, EXCALDRAW + 6, 0, 44}, {12, EXCALDRAW + 7, 112, 0}, {12, EXCALDRAW + 8, 224, 0},
     {12, EXCALDRAW + 9, 198, 84}, {12, EXCALDRAW + 10, 186, 120}, {12, EXCALDRAW + 11, 188, 123},
     {1, EMPTY, 147, 76}
     },
     {{12, HALBERDDRAW, 183, 62}, {12, HALBERDDRAW + 1, 166, 10}, {12, HALBERDDRAW + 2, 173, 29},
     {12, HALBERDDRAW + 3, 114, 35}, {1, HALBERDATTACK1, 245, 22}, {1, HALBERDATTACK1, 245, 22},
     {1, HALBERDATTACK1, 245, 22}, {1, HALBERDATTACK1, 245, 22}, {1, HALBERDATTACK1, 245, 22},
     {1, HALBERDATTACK1, 245, 22}, {1, HALBERDATTACK1, 245, 22}, {1, HALBERDATTACK1, 245, 22},
     {1, EMPTY, 147, 76}
     }

};



struct daweapons weaponanimtics[MAXWEAPONS][MAXFRAMES] =
{
     // FIST
     {{10, RFIST, 216, 120}, {10, RFIST + 1, 166, 113}, {10, RFIST + 2, 156, 129},
     {10, RFIST + 3, 169, 151}, {10, RFIST + 4, 153, 124}, {10, RFIST + 5, 224, 133},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // KNIFE
     {{8, KNIFEATTACK, 189, 52}, {8, KNIFEATTACK + 1, 254, 68}, {8, EMPTY, 147, 76},
     {8, EMPTY, 80, 41}, {8, KNIFEATTACK + 2, 254, 69}, {8, KNIFEATTACK + 3, 218, 80},
     {8, KNIFEATTACK + 4, 137, 83}, {8, KNIFEATTACK + 5, 136, 100}, {8, KNIFEATTACK + 6, 126, 140},
     {8, KNIFEATTACK + 5, 136, 100}, {8, KNIFEATTACK + 4, 137, 83}, {8, KNIFEATTACK, 189, 52}
     },
     // GOBLINATTACK
     {{10, GOBSWORDATTACK, 243, 92}, {10, GOBSWORDATTACK + 1, 255, 68}, {10, GOBSWORDATTACK + 2, 279, 65},
     {10, GOBSWORDATTACK + 3, 238, 55}, {10, GOBSWORDATTACK + 4, 153, 52}, {10, GOBSWORDATTACK + 5, 129, 152},
     {10, GOBSWORDATTACK + 6, 90, 184}, {1, EMPTY, 297, 169}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     },
     // MORNINGSTAR
     {{12, MORNINGSTAR, 193, 90}, {12, MORNINGSTAR + 1, 102, 133}, {12, MORNINGSTAR + 2, 77, 164},
     {12, MORNINGSTAR + 3, 239, 86}, {12, EMPTY, 299, 86}, {12, EMPTY, 107, 52},
     {12, MORNINGSTAR + 4, 197, 24}, {12, MORNINGSTAR + 5, 125, 124}, {12, MORNINGSTAR + 6, 109, 191},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // SWORD
     {{8, SWORDATTACK, 229, 123}, {8, SWORDATTACK + 1, 221, 87}, {8, SWORDATTACK + 2, 193, 21},
     {8, SWORDATTACK + 3, 173, 0}, {8, SWORDATTACK + 4, 61, 0}, {8, SWORDATTACK + 5, 33, 48},
     {8, SWORDATTACK + 6, 126, 131}, {8, SWORDATTACK + 7, 297, 164}, {3, EMPTY, 147, 76},
     {3, EMPTY, 80, 41}, {3, EMPTY, 107, 52}, {3, EMPTY, 147, 76}
     },
     {{12, BIGAXEATTACK, 184, 123}, {12, BIGAXEATTACK + 1, 223, 112}, {12, BIGAXEATTACK + 2, 63, 151},
     {12, BIGAXEATTACK + 3, 91, 133}, {12, BIGAXEATTACK + 4, 127, 138}, {12, BIGAXEATTACK + 5, 106, 128},
     {12, BIGAXEATTACK + 6, 117, 49}, {12, BIGAXEATTACK + 7, 140, 0}, {12, BIGAXEATTACK + 8, 152, 47},
     {12, BIGAXEATTACK + 9, 166, 143}, {12, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // BOW
     {{8, BOWWALK, 75, 13}, {8, BOWWALK + 1, 90, 0}, {8, BOWWALK + 2, 70, 0},
     {8, BOWWALK + 3, 70, 0}, {6, BOWWALK + 4, 70, 0}, {4, BOWWALK + 5, 70, 0},
     {1, EMPTY, 126, 131}, {1, EMPTY, 297, 164}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     {{10, PIKEDRAW + 5, 216, 131}, {10, EMPTY, 80, 41}, {10, EMPTY, 107, 52}, {10, EMPTY, 147, 76},
     {10, PIKEATTACK1, 0, 47}, {10, PIKEATTACK1 + 1, 0, 0}, {10, PIKEATTACK1 + 2, 0, 0},
     {10, PIKEATTACK1 + 3, 73, 0}, {10, PIKEATTACK1 + 4, 130, 27}, {10, PIKEATTACK1 + 5, 138, 125},
     {12, EMPTY, 80, 41}, {1, EMPTY, 107, 52}
     },
     {{8, EXCALATTACK1, 98, 133}, {8, EXCALATTACK1 + 1, 123, 130}, {8, EXCALATTACK1 + 2, 125, 128},
     {8, EXCALATTACK1 + 3, 115, 82}, {8, EXCALATTACK1 + 4, 115, 6}, {8, EXCALATTACK1 + 5, 178, 0},
     {8, EXCALATTACK1 + 6, 155, 0}, {8, EXCALATTACK1 + 7, 143, 0}, {8, EXCALATTACK1 + 8, 90, 91},
     {8, EXCALATTACK1 + 9, 30, 159}, {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}
     },
     {{12, HALBERDATTACK1, 245, 22}, {12, EMPTY, 107, 52}, {12, EMPTY, 147, 76},
     {12, HALBERDATTACK1 + 1, 249, 45}, {12, HALBERDATTACK1 + 2, 161, 60}, {12, HALBERDATTACK1 + 3, 45, 88},
     {12, EMPTY, 80, 41}, {12, HALBERDATTACK1 + 3, 45, 88}, {12, HALBERDATTACK1 + 2, 161, 60},
     {12, HALBERDATTACK1 + 1, 249, 45}, {12, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     }
};


struct daweapons weaponanimtics2[MAXWEAPONS][MAXFRAMES] =
{
     // FIST
     {{10, RFIST, 216, 120}, {10, RFIST + 1, 166, 113}, {10, RFIST + 2, 156, 129},
     {10, RFIST + 3, 169, 151}, {10, RFIST + 4, 153, 124}, {10, RFIST + 5, 224, 133},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // KNIFE
     {{8, KNIFEATTACK, 189, 52}, {8, KNIFEATTACK + 1, 254, 68}, {16, EMPTY, 147, 76},
     {8, KNIFEATTACK2, 206, 114}, {8, KNIFEATTACK2 + 1, 107, 112}, {8, KNIFEATTACK2 + 2, 22, 138},
     {8, KNIFEATTACK2 + 3, 0, 161}, {16, EMPTY, 136, 100}, {8, KNIFEATTACK2 + 3, 0, 161},
     {8, KNIFEATTACK2 + 2, 22, 138}, {8, KNIFEATTACK2 + 1, 107, 112}, {8, KNIFEATTACK2, 206, 114}
     },
     // GOBLINATTACK
     {{10, GOBSWORDATTACK2, 236, 99}, {10, GOBSWORDATTACK2 + 1, 202, 24}, {10, GOBSWORDATTACK2 + 2, 181, 0},
     {10, GOBSWORDATTACK2 + 3, 52, 12}, {10, GOBSWORDATTACK2 + 4, 72, 72}, {10, GOBSWORDATTACK2 + 5, 134, 139},
     {10, GOBSWORDATTACK2 + 6, 297, 169}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     },
     // MORNINGATTACK2
     {{12, MORNINGATTACK2, 85, 136}, {12, MORNINGATTACK2 + 1, 34, 110}, {12, MORNINGATTACK2 + 2, 32, 91},
     {12, MORNINGATTACK2 + 3, 186, 47}, {12, MORNINGATTACK2 + 4, 275, 24}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     },
     // SWORD
     {{8, SWORDATTACK2 + 1, 195, 63}, {8, SWORDATTACK2 + 2, 250, 54}, {8, SWORDATTACK2 + 3, 275, 37},
     {16, EMPTY, 61, 0}, {8, SWORDATTACK2 + 4, 229, 66}, {8, SWORDATTACK2 + 5, 185, 0},
     {8, SWORDATTACK2 + 6, 158, 115}, {8, SWORDATTACK2 + 7, 57, 163}, {1, EMPTY, 57, 163},
     {1, EMPTY, 57, 163}, {1, EMPTY, 57, 163}, {1, EMPTY, 57, 163}
     },
     {{12, BIGAXEATTACK2, 200, 111}, {12, BIGAXEATTACK2 + 1, 5, 136}, {12, BIGAXEATTACK2 + 2, 69, 162},
     {12, BIGAXEATTACK2 + 3, 147, 164}, {12, BIGAXEATTACK2 + 4, 76, 152}, {12, BIGAXEATTACK2 + 5, 33, 95},
     {12, BIGAXEATTACK2 + 6, 0, 91}, {12, BIGAXEATTACK2 + 7, 0, 98}, {12, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // BOW
     {{8, BOWWALK, 75, 13}, {8, BOWWALK + 1, 90, 0}, {8, BOWWALK + 2, 70, 0},
     {8, BOWWALK + 3, 70, 0}, {6, BOWWALK + 4, 70, 0}, {4, BOWWALK + 5, 70, 0},
     {1, EMPTY, 126, 131}, {1, EMPTY, 297, 164}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     {{10, PIKEATTACK2, 266, 147}, {10, PIKEATTACK2 + 1, 182, 117}, {10, PIKEATTACK2 + 2, 123, 84},
     {10, PIKEATTACK2 + 3, 7, 48}, {10, PIKEATTACK2 + 4, 0, 83}, {10, PIKEATTACK2 + 5, 0, 158},
     {10, PIKEATTACK2 + 6, 25, 117}, {10, PIKEATTACK2 + 7, 139, 93}, {10, PIKEATTACK2 + 8, 234, 75},
     {8, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     {{8, EXCALATTACK2, 0, 143}, {8, EXCALATTACK2 + 1, 0, 103}, {8, EXCALATTACK2 + 2, 0, 70},
     {8, EXCALATTACK2 + 3, 48, 0}, {8, EXCALATTACK2 + 4, 67, 0}, {8, EXCALATTACK2 + 5, 78, 21},
     {8, EXCALATTACK2 + 6, 165, 107}, {8, EXCALATTACK2 + 7, 260, 168}, {1, EMPTY, 130, 27},
     {1, EMPTY, 138, 125}, {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}
     },
     {{12, HALBERDATTACK1, 245, 22}, {12, HALBERDATTACK2, 114, 35}, {12, HALBERDATTACK2 + 1, 105, 87},
     {12, HALBERDATTACK2 + 2, 54, 107}, {12, HALBERDATTACK2 + 3, 48, 102}, {1, HALBERDATTACK2 + 3, 48, 102},
     {1, HALBERDATTACK2 + 3, 48, 102}, {12, HALBERDATTACK2 + 2, 54, 107}, {12, HALBERDATTACK2 + 1, 105, 87},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     }
};

// THE NEW WEAPONS

// SCOTT
struct daweapons zlefthandanimtics[5][MAXFRAMES] =
{
     {{10, RFIST, 15, 121}, {10, RFIST + 1, 17, 114}, {10, RFIST + 2, 54, 131},
     {10, RFIST + 3, 76, 152}, {10, RFIST + 4, 31, 126}, {10, RFIST + 5, 26, 135},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // KNIFE
     {{8, KNIFEATTACK2, 0, 113}, {8, KNIFEATTACK2 + 1, 44, 111}, {8, KNIFEATTACK2 + 2, 119, 137},
     {8, KNIFEATTACK2 + 3, 187, 159}, {16, EMPTY, 136, 100}, {8, KNIFEATTACK2 + 3, 187, 159},
     {8, KNIFEATTACK2 + 2, 119, 137}, {8, KNIFEATTACK2 + 1, 44, 111}, {8, KNIFEATTACK2, 0, 113},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // ZSHORTATTACK
     {{10, ZSHORTATTACK, 255, 50}, {10, ZSHORTATTACK + 1, 279, 66}, {10, ZSHORTATTACK + 2, 238, 52},
     {10, ZSHORTATTACK + 3, 181, 49}, {10, ZSHORTATTACK + 4, 129, 141}, {10, ZSHORTATTACK + 5, 70, 93},
     {10, EMPTY, 90, 184}, {1, EMPTY, 297, 169}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     },
     // ZSTARATTACK2
     {{12, ZSTARATTACK2, 38, 141}, {12, ZSTARATTACK2 + 1, 0, 111}, {12, ZSTARATTACK2 + 2, 0, 91},
     {12, ZSTARATTACK2 + 3, 0, 47}, {12, EMPTY, 0, 24}, {1, EMPTY, 0, 24},
     {1, EMPTY, 0, 24}, {1, EMPTY, 0, 24}, {1, EMPTY, 0, 24},
     {1, EMPTY, 0, 24}, {1, EMPTY, 0, 24}, {1, EMPTY, 0, 24}
     },
     // ZSHORTATTACK2
     {{10, ZSHORTATTACK2, 238, 99}, {10, ZSHORTATTACK2 + 1, 202, 11}, {10, ZSHORTATTACK2 + 2, 182, 0},
     {10, ZSHORTATTACK2 + 3, 79, 13}, {10, ZSHORTATTACK2 + 4, 79, 13}, {10, ZSHORTATTACK2 + 5, 119, 123},
     {10, ZSHORTATTACK2 + 6, 295, 179}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     }

};

//SCOTT
struct daweapons zcockanimtics[MAXFRAMES + 1] =
{
     {24, EMPTY, 10, 10},
     {12, BOWREADYEND + 1, 101, 115}, {12, BOWREADYEND + 2, 112, 0},
     {12, BOWREADYEND + 3, 115, 0}, {12, ZBOWWALK, 75, 13}

//    {24,EMPTY,10,10},
//    {12,BOWREADYEND+1,101,115},{12,BOWREADYEND+2,112,0},
//    {12,BOWREADYEND+3,115,0},{12,BOWREADYEND+4,75,13}

};

//SCOTT
struct daweapons zreadyanimtics[MAXWEAPONS][MAXFRAMES + 1] =
{
     // FIST
     {{10, RFIST, 216, 180}, {10, RFIST, 216, 170}, {10, RFIST, 216, 160},
     {10, RFIST, 216, 150}, {10, RFIST, 216, 140}, {10, RFIST, 216, 130},
     {10, RFIST, 216, 124}, {1, RFIST, 216, 124}, {1, RFIST, 216, 124},
     {1, RFIST, 216, 122}, {1, RFIST, 216, 122}, {1, RFIST, 216, 122},
     {1, EMPTY, 147, 76}
     },
     // ZKNIFE
     {{10, ZKNIFEREADY, 69, 171}, {10, ZKNIFEREADY + 1, 11, 146}, {10, ZKNIFEREADY + 2, 25, 146},
     {10, ZKNIFEREADY + 3, 35, 158}, {10, ZKNIFEREADY + 4, 38, 158}, {10, ZKNIFEREADY + 5, 16, 157},
     {10, ZKNIFEREADY + 6, 37, 102}, {10, ZKNIFEREADY + 7, 239, 63}, {10, ZKNIFEREADY + 8, 214, 85},
     {10, ZKNIFEREADY + 9, 206, 110}, {10, ZKNIFEREADY + 10, 217, 108}, {10, ZKNIFEREADY + 11, 204, 95},
     {1, EMPTY, 147, 76}
     },
     // ZSHORTREADY
     {{12, ZSHORTREADY, 79, 169}, {12, ZSHORTREADY + 1, 95, 115}, {12, ZSHORTREADY + 2, 94, 93},
     {12, ZSHORTREADY + 3, 156, 77}, {12, ZSHORTREADY + 4, 218, 64}, {12, ZSHORTREADY + 5, 224, 57},
     {8, ZSHORTREADY + 6, 251, 54}, {1, ZSHORTREADY + 7, 243, 92}, {1, ZSHORTREADY + 7, 243, 92},
     {1, ZSHORTREADY + 7, 243, 92}, {1, ZSHORTREADY + 7, 243, 92}, {1, ZSHORTREADY + 7, 243, 92},
     {1, EMPTY, 147, 76}
     },
     // ZSTARATTACK
     {{6, ZSTARATTACK, 194, 195}, {6, ZSTARATTACK, 194, 185}, {6, ZSTARATTACK, 194, 175},
     {6, ZSTARATTACK, 194, 165}, {6, ZSTARATTACK, 194, 155}, {6, ZSTARATTACK, 194, 145},
     {6, ZSTARATTACK, 194, 135}, {6, ZSTARATTACK, 194, 125}, {6, ZSTARATTACK, 194, 115},
     {6, ZSTARATTACK, 194, 105}, {6, ZSTARATTACK, 194, 95}, {1, ZSTARATTACK, 194, 85},
     {1, EMPTY, 147, 76}
     },
     // SWORD
     {{10, SWORDPULL, 58, 160}, {10, SWORDPULL + 1, 81, 111}, {10, SWORDPULL + 2, 19, 88},
     {10, SWORDPULL + 3, 0, 93}, {10, SWORDPULL + 4, 104, 0}, {10, SWORDPULL + 5, 169, 0},
     {10, SWORDPULL + 6, 244, 38}, {6, SWORDPULL + 7, 225, 121}, {1, SWORDPULL + 7, 225, 121},
     {1, SWORDPULL + 7, 225, 121}, {1, SWORDPULL + 7, 225, 121}, {1, SWORDPULL + 7, 225, 121},
     {1, EMPTY, 147, 76}
     },
     // ZAXE
     {{8, ZAXEREADY, 0, 108}, {8, ZAXEREADY + 1, 0, 58}, {8, ZAXEREADY + 2, 0, 57},
     {8, ZAXEREADY + 3, 0, 69}, {8, ZAXEREADY + 4, 0, 100}, {8, ZAXEREADY + 5, 0, 9},
     {8, ZAXEREADY + 6, 33, 0}, {8, ZAXEREADY + 7, 61, 0}, {8, ZAXEREADY + 8, 73, 20},
     {8, ZAXEREADY + 9, 179, 117}, {8, EMPTY, 182, 116}, {1, EMPTY, 200, 122},
     {1, EMPTY, 147, 76}
     },
     // ZBOW
     {{12, ZBOWREADY, 0, 0}, {12, ZBOWREADY + 1, 0, 20}, {12, ZBOWREADY + 2, 0, 46},
     {12, ZBOWREADY + 3, 0, 26}, {12, ZBOWREADY + 4, 0, 0}, {12, ZBOWREADY + 5, 71, 0},
     {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13},
     {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13},
     {1, EMPTY, 147, 76}
     },
     // ZPIKE
     {{8, ZPIKEREADY, 0, 150}, {8, ZPIKEREADY + 1, 0, 94}, {8, ZPIKEREADY + 2, 47, 45},
     {8, ZPIKEREADY + 3, 138, 62}, {8, ZPIKEREADY + 4, 194, 95}, {8, ZPIKEREADY + 5, 59, 121},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 147, 76}
     },
     {{12, ZTWOHANDREADY, 167, 131}, {12, ZTWOHANDREADY + 1, 71, 117}, {12, ZTWOHANDREADY, 0, 128},
     {12, ZTWOHANDREADY + 3, 0, 150}, {12, ZTWOHANDREADY + 4, 10, 74}, {12, ZTWOHANDREADY + 5, 44, 81},
     {12, ZTWOHANDREADY + 6, 0, 53}, {12, ZTWOHANDREADY + 7, 112, 0}, {12, ZTWOHANDREADY + 8, 220, 0},
     {12, ZTWOHANDREADY + 9, 198, 84}, {12, ZTWOHANDREADY + 10, 186, 120}, {12, ZTWOHANDREADY + 11, 188, 124},
     {1, EMPTY, 147, 76}
     },
     // ZHALBERD
     // { { 12,ZHALBERDREADY,183,62
     // },{12,ZHALBERDREADY+1,166,10},{12,ZHALBERDREADY+2,173,29},
     // { 12,ZHALBERDATTACK,173,29
     // },{1,ZHALBERDATTACK,245,22},{1,ZHALBERDATTACK,245,22},
     // { 1,ZHALBERDATTACK,245,22
     // },{1,ZHALBERDATTACK,245,22},{1,ZHALBERDATTACK,245,22},
     // { 1,ZHALBERDATTACK,245,22
     // },{1,ZHALBERDATTACK,245,22},{1,ZHALBERDATTACK,245,22},
     // { 1,EMPTY,147,76}
     // }
     {{12, HALBERDDRAW, 183, 62}, {12, HALBERDDRAW + 1, 166, 10}, {12, HALBERDDRAW + 2, 173, 29},
     {12, HALBERDDRAW + 3, 114, 35}, {1, HALBERDATTACK1, 245, 22}, {1, HALBERDATTACK1, 245, 22},
     {1, HALBERDATTACK1, 245, 22}, {1, HALBERDATTACK1, 245, 22}, {1, HALBERDATTACK1, 245, 22},
     {1, HALBERDATTACK1, 245, 22}, {1, HALBERDATTACK1, 245, 22}, {1, ZHALBERDATTACK, 245, 30},
     {1, EMPTY, 147, 76}
     }

};


//SCOTT
struct daweapons zweaponanimtics[MAXWEAPONS][MAXFRAMES] =
{
     // FIST
     {{10, RFIST, 216, 120}, {10, RFIST + 1, 166, 113}, {10, RFIST + 2, 156, 129},
     {10, RFIST + 3, 169, 151}, {10, RFIST + 4, 153, 124}, {10, RFIST + 5, 224, 133},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // ZKNIFE
     {{8, ZKNIFEATTACK, 189, 52}, {8, ZKNIFEATTACK + 1, 254, 68},
     {16, EMPTY, 147, 76}, {8, ZKNIFEATTACK + 1, 254, 68},
     {8, ZKNIFEATTACK + 2, 218, 80}, {8, ZKNIFEATTACK + 3, 137, 83},
     {8, ZKNIFEATTACK + 4, 136, 100}, {8, ZKNIFEATTACK + 5, 126, 140},
     {8, ZKNIFEATTACK + 4, 136, 100}, {8, ZKNIFEATTACK + 3, 137, 83},
     {8, ZKNIFEATTACK + 2, 218, 80}, {1, ZKNIFEATTACK, 189, 52}
     },
     // ZSHORTATTACK
     {{10, ZSHORTATTACK, 243, 68}, {10, ZSHORTATTACK + 4, 255, 50},
     {10, ZSHORTATTACK + 5, 279, 66}, {10, ZSHORTATTACK + 6, 238, 52},
     {10, ZSHORTATTACK + 7, 181, 49}, {10, ZSHORTATTACK + 8, 129, 141},
     {1, EMPTY, 90, 184}, {1, EMPTY, 297, 169}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     },
     // ZSTARATTACK
     {{12, ZSTARATTACK, 193, 90}, {12, ZSTARATTACK + 3, 102, 128},
     {12, ZSTARATTACK + 4, 77, 159}, {12, ZSTARATTACK + 5, 239, 79},
     {12, EMPTY, 299, 86}, {12, EMPTY, 107, 52},
     {12, ZSTARATTACK + 6, 175, 19}, {12, ZSTARATTACK + 7, 125, 124},
     {12, ZSTARATTACK + 8, 109, 187},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // SWORD
     {{8, SWORDATTACK, 229, 123}, {8, SWORDATTACK + 1, 221, 87}, {8, SWORDATTACK + 2, 193, 21},
     {8, SWORDATTACK + 3, 173, 0}, {8, SWORDATTACK + 4, 61, 0}, {8, SWORDATTACK + 5, 33, 48},
     {8, SWORDATTACK + 6, 126, 131}, {8, SWORDATTACK + 7, 297, 164}, {3, EMPTY, 147, 76},
     {3, EMPTY, 80, 41}, {3, EMPTY, 107, 52}, {3, EMPTY, 147, 76}
     },
     // ZAXEATTACK
     {{6, ZAXEATTACK, 179, 117}, {6, ZAXEATTACK + 6, 217, 107},
     {6, ZAXEATTACK + 7, 106, 146}, {6, ZAXEATTACK + 8, 94, 128},
     {6, ZAXEATTACK + 9, 123, 132}, {6, ZAXEATTACK + 10, 102, 134},
     {6, ZAXEATTACK + 11, 112, 45}, {6, ZAXEATTACK + 12, 102, 0},
     {6, ZAXEATTACK + 13, 68, 42}, {6, ZAXEATTACK + 14, 42, 138},
     {6, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // ZBOW
     {{8, ZBOWWALK, 75, 13}, {8, ZBOWATTACK, 90, 0}, {8, ZBOWATTACK + 1, 70, 0},
     {8, ZBOWATTACK + 2, 70, 0}, {6, ZBOWATTACK + 3, 70, 0}, {4, ZBOWATTACK + 4, 70, 0},
     {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13},
     {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13}, {1, EMPTY, 147, 76}
     },
     // ZPIKE
     {{10, ZPIKEREADY + 5, 210, 127}, {10, EMPTY, 80, 41}, {10, EMPTY, 107, 52}, {10, EMPTY, 147, 76},
     {10, ZPIKEATTACK, 0, 43}, {10, ZPIKEATTACK + 1, 0, 0}, {10, ZPIKEATTACK + 2, 0, 0},
     {10, ZPIKEATTACK + 3, 45, 0}, {10, ZPIKEATTACK + 4, 51, 23}, {10, ZPIKEATTACK + 5, 59, 121},
     {12, EMPTY, 80, 41}, {1, EMPTY, 107, 52}
     },
     {{8, ZTWOHANDATTACK, 98, 133}, {8, ZTWOHANDATTACK + 6, 115, 130},
     {8, ZTWOHANDATTACK + 7, 125, 128}, {8, ZTWOHANDATTACK + 8, 116, 82},
     {8, ZTWOHANDATTACK + 9, 117, 9}, {8, ZTWOHANDATTACK + 10, 180, 0},
     {8, ZTWOHANDATTACK + 11, 174, 0}, {8, ZTWOHANDATTACK + 12, 166, 0},
     {8, ZTWOHANDATTACK + 13, 125, 90}, {8, ZTWOHANDATTACK + 14, 83, 166},
     {1, EMPTY, 30, 159}, {1, EMPTY, 80, 41}
     },
     // { { 12,ZHALBERDATTACK,173,29 },{6,EMPTY,107,52},
     // { 6,EMPTY,147,76},{ 12,ZHALBERDATTACK+3,117,0 },
     // { 12,ZHALBERDATTACK+4,245,0},{12,ZHALBERDATTACK+5,237,0},
     // { 6,EMPTY,80,41 },{12,ZHALBERDATTACK+6,115,0},
     // { 12,ZHALBERDATTACK+7,9,0 },{ 12,ZHALBERDATTACK+8,0,42 },
     // { 1,ZHALBERDATTACK,173,29 },{ 1,EMPTY,147,76 }
     // }
     {{12, ZHALBERDATTACK, 245, 30}, {12, EMPTY, 107, 52}, {12, EMPTY, 147, 76},
     {12, HALBERDATTACK1 + 1, 249, 45}, {12, HALBERDATTACK1 + 2, 161, 60}, {12, HALBERDATTACK1 + 3, 45, 88},
     {12, EMPTY, 80, 41}, {12, HALBERDATTACK1 + 3, 45, 88}, {12, HALBERDATTACK1 + 2, 161, 60},
     {12, HALBERDATTACK1 + 1, 249, 45}, {12, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     }


};


//SCOTT
struct daweapons zweaponanimtics2[MAXWEAPONS][MAXFRAMES] =
{
     // FIST
     {{10, RFIST, 216, 120}, {10, RFIST + 1, 166, 113}, {10, RFIST + 2, 156, 129},
     {10, RFIST + 3, 169, 151}, {10, RFIST + 4, 153, 124}, {10, RFIST + 5, 224, 133},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // ZKNIFE
     {{8, ZKNIFEATTACK, 189, 52}, {8, ZKNIFEATTACK + 1, 254, 68}, {16, EMPTY, 147, 76},
     {8, ZKNIFEATTACK2, 206, 114}, {8, ZKNIFEATTACK2 + 1, 107, 112}, {8, ZKNIFEATTACK2 + 2, 22, 138},
     {8, ZKNIFEATTACK2 + 3, 0, 161}, {16, EMPTY, 136, 100}, {8, KNIFEATTACK2 + 3, 0, 161},
     {8, ZKNIFEATTACK2 + 2, 22, 138}, {8, ZKNIFEATTACK2 + 1, 107, 112}, {8, KNIFEATTACK2, 206, 114}
     },
     // ZSHORTATTACK2
     {{10, ZSHORTATTACK2, 238, 99}, {10, ZSHORTATTACK2 + 1, 202, 11},
     {10, ZSHORTATTACK2 + 2, 182, 0}, {10, ZSHORTATTACK2 + 3, 79, 13},
     {10, ZSHORTATTACK2 + 4, 40, 45}, {10, ZSHORTATTACK2 + 5, 119, 123},
     {10, ZSHORTATTACK2 + 6, 295, 179}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     },
     // ZSTARATTACK2
     {{12, ZSTARATTACK2, 44, 110}, {12, ZSTARATTACK2 + 1, 26, 91}, {12, ZSTARATTACK2 + 2, 177, 38},
     {12, ZSTARATTACK2 + 3, 262, 11}, {12, EMPTY, 275, 24}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24},
     {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}, {1, EMPTY, 275, 24}
     },
     // SWORD
     {{8, SWORDATTACK2 + 1, 195, 63}, {8, SWORDATTACK2 + 2, 250, 54}, {8, SWORDATTACK2 + 3, 275, 37},
     {16, EMPTY, 61, 0}, {8, SWORDATTACK2 + 4, 229, 66}, {8, SWORDATTACK2 + 5, 185, 0},
     {8, SWORDATTACK2 + 6, 158, 115}, {8, SWORDATTACK2 + 7, 57, 163}, {1, EMPTY, 57, 163},
     {1, EMPTY, 57, 163}, {1, EMPTY, 57, 163}, {1, EMPTY, 57, 163}
     },
     // ZAXEATTACK2
     {{6, ZAXEATTACK2, 200, 111}, {6, ZAXEATTACK2 + 1, 5, 136}, {6, ZAXEATTACK2 + 2, 69, 162},
     {6, ZAXEATTACK2 + 3, 147, 164}, {6, ZAXEATTACK2 + 4, 76, 152}, {6, ZAXEATTACK2 + 5, 33, 95},
     {6, ZAXEATTACK2 + 6, 0, 91}, {6, ZAXEATTACK2 + 7, 0, 98}, {6, EMPTY, 147, 76},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // ZBOW
     // { { 8,ZBOWWALK,75,13},{8,ZBOWWALK+1,90,0},{8,ZBOWWALK+2,70,0},
     // { 8,ZBOWWALK+3,70,0},{6,ZBOWWALK+4,70,0},{4,ZBOWWALK+5,70,0},
     // { 1,EMPTY,126,131},{1,EMPTY,297,164},{1,EMPTY,147,76},
     // { 1,EMPTY,80,41 },{1,EMPTY,107,52},{1,EMPTY,147,76}
     // },
     {{8, ZBOWWALK, 75, 13}, {8, ZBOWATTACK, 90, 0}, {8, ZBOWATTACK + 1, 70, 0},
     {8, ZBOWATTACK + 2, 70, 0}, {6, ZBOWATTACK + 3, 70, 0}, {4, ZBOWATTACK + 4, 70, 0},
     {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13},
     {1, ZBOWWALK, 75, 13}, {1, ZBOWWALK, 75, 13}, {1, EMPTY, 147, 76}
     },

     // ZPIKEATTACK2
     {{10, ZPIKEATTACK2, 266, 147}, {10, ZPIKEATTACK2 + 1, 182, 117}, {10, ZPIKEATTACK2 + 2, 123, 84},
     {10, ZPIKEATTACK2 + 3, 7, 48}, {10, ZPIKEATTACK2 + 4, 0, 83}, {10, ZPIKEATTACK2 + 5, 0, 158},
     {10, ZPIKEATTACK2 + 6, 25, 117}, {10, ZPIKEATTACK2 + 7, 139, 93}, {10, ZPIKEATTACK2 + 8, 234, 75},
     {8, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     },
     // ZTWOHANDATTACK2
     {{8, ZTWOHANDATTACK2, 0, 143}, {8, ZTWOHANDATTACK2 + 1, 0, 103}, {8, ZTWOHANDATTACK2 + 2, 0, 70},
     {8, ZTWOHANDATTACK2 + 3, 41, 0}, {8, ZTWOHANDATTACK2 + 4, 54, 0}, {8, ZTWOHANDATTACK2 + 5, 166, 21},
     {8, ZTWOHANDATTACK2 + 6, 242, 108}, {8, EMPTY, 260, 168}, {1, EMPTY, 130, 27},
     {1, EMPTY, 138, 125}, {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}
     },
     // ZHALBERDATTACK2
     // { { 12,ZHALBERDATTACK2,118,0 },{ 12,ZHALBERDATTACK2+1,80,0},
     // { 12,ZHALBERDATTACK2+2,29,0},{ 12,ZHALBERDATTACK2+3,42,1},
     // { 12,ZHALBERDATTACK2+1,80,0},{1,ZHALBERDATTACK2,118,0},
     // {  1,EMPTY,173,29},{1,EMPTY,173,29},
     // {  1,EMPTY,166,10},{1,EMPTY,80,41 },
     // {  1,EMPTY,107,52},{1,EMPTY,147,76}
     // }
     {{12, HALBERDATTACK1, 245, 22}, {12, HALBERDATTACK2, 114, 35}, {12, HALBERDATTACK2 + 1, 105, 87},
     {12, HALBERDATTACK2 + 2, 54, 107}, {12, HALBERDATTACK2 + 3, 48, 102}, {1, HALBERDATTACK2 + 3, 48, 102},
     {1, HALBERDATTACK2 + 3, 48, 102}, {12, HALBERDATTACK2 + 2, 54, 107}, {12, HALBERDATTACK2 + 1, 105, 87},
     {1, EMPTY, 80, 41}, {1, EMPTY, 107, 52}, {1, EMPTY, 147, 76}
     }

};

struct daweapons zthrowanimtics[MAXNUMORBS][MAXFRAMES + 1] =
{
#if 0
     // MUTWOHANDS  - scare
     {{10, MUTWOHANDS, 19, 155}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUTWOHANDS  - scare
     {{1, EMPTY, 127, 170}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUMEDUSA    - night vis
     {{10, ZLIGHT, 0, 177}, {10, ZLIGHT + 1, 0, 137}, {10, ZLIGHT + 2, 48, 82},
     {10, ZLIGHT + 3, 127, 41}, {10, ZLIGHT + 4, 210, 9}, {10, ZLIGHT + 5, 284, 26},
     {10, ZLIGHT + 6, 213, 63}, {10, ZLIGHT + 7, 147, 99}, {10, ZLIGHT + 8, 91, 136},
     {10, ZLIGHT + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUTWOHANDS  - night vis
     {{10, ZFIREBALL, 0, 177}, {10, ZFIREBALL + 1, 0, 137}, {10, ZFIREBALL + 2, 48, 82},
     {10, ZFIREBALL + 3, 127, 41}, {10, ZFIREBALL + 4, 210, 9}, {10, ZFIREBALL + 5, 284, 26},
     {10, ZFIREBALL + 6, 213, 63}, {10, ZFIREBALL + 7, 147, 99}, {10, ZFIREBALL + 8, 91, 136},
     {10, ZFIREBALL + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#if 0
     // BMUTWOHANDS - freeze
     {{10, ZFREEZE, 0, 101}, {10, ZFREEZE + 1, 0, 51}, {10, ZFREEZE + 2, 0, 71},
     {10, ZFREEZE + 3, 4, 85}, {10, ZFREEZE + 4, 32, 78}, {10, ZFREEZE + 5, 51, 80},
     {10, ZFREEZE + 6, 50, 80}, {10, ZFREEZE + 7, 49, 89}, {10, ZFREEZE + 8, 49, 89},
     {10, ZFREEZE + 9, 49, 98}, {10, ZFREEZE + 10, 47, 105}, {10, ZFREEZE + 11, 48, 121},
     {1, EMPTY, 127, 170}
     },
#endif
     // BMUTWOHANDS - freeze
     {{1, EMPTY, 127, 170}, {10, ZFREEZE + 1, 0, 51}, {10, ZFREEZE + 2, 0, 71},
     {10, ZFREEZE + 3, 4, 85}, {10, ZFREEZE + 4, 32, 78}, {10, ZFREEZE + 5, 51, 80},
     {10, ZFREEZE + 6, 50, 80}, {10, ZFREEZE + 7, 49, 89}, {10, ZFREEZE + 8, 49, 89},
     {10, ZFREEZE + 9, 49, 98}, {10, ZFREEZE + 10, 47, 105}, {10, ZFREEZE + 11, 48, 121},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUTWOHANDS  - magic arrow
     {{10, MUTWOHANDS, 19, 155}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUTWOHANDS  - magic arrow
     {{1, EMPTY, 127, 170}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
     // MUTWOHANDS  - open door
     {{15, MUTWOHANDS, 19, 155}, {15, MUTWOHANDS + 1, 0, 128}, {15, MUTWOHANDS + 2, 0, 93},
     {15, MUTWOHANDS + 3, 0, 83}, {15, MUTWOHANDS + 4, 0, 72}, {15, MUTWOHANDS + 5, 0, 83},
     {15, MUTWOHANDS + 6, 10, 96}, {15, MUTWOHANDS + 7, 43, 109}, {15, MUTWOHANDS + 8, 69, 113},
     {15, MUTWOHANDS + 9, 65, 115}, {15, MUTWOHANDS + 10, 64, 117}, {15, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUMEDUSA    - fly
     {{10, MUMEDUSA, 0, 177}, {10, MUMEDUSA + 1, 0, 137}, {10, MUMEDUSA + 2, 48, 82},
     {10, MUMEDUSA + 3, 127, 41}, {10, MUMEDUSA + 4, 210, 9}, {10, MUMEDUSA + 5, 284, 26},
     {10, MUMEDUSA + 6, 213, 63}, {10, MUMEDUSA + 7, 147, 99}, {10, MUMEDUSA + 8, 91, 136},
     {10, MUMEDUSA + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUMEDUSA    - fly
     {{10, ZLIGHT, 0, 177}, {10, ZLIGHT + 1, 0, 137}, {10, ZLIGHT + 2, 48, 82},
     {10, ZLIGHT + 3, 127, 41}, {10, ZLIGHT + 4, 210, 9}, {10, ZLIGHT + 5, 284, 26},
     {10, ZLIGHT + 6, 213, 63}, {10, ZLIGHT + 7, 147, 99}, {10, ZLIGHT + 8, 91, 136},
     {10, ZLIGHT + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUTWOHANDS  - fireball
     {{10, ZFIREBALL, 0, 177}, {10, ZFIREBALL + 1, 0, 137}, {10, ZFIREBALL + 2, 48, 82},
     {10, ZFIREBALL + 3, 127, 41}, {10, ZFIREBALL + 4, 210, 9}, {10, ZFIREBALL + 5, 284, 26},
     {10, ZFIREBALL + 6, 213, 63}, {10, ZFIREBALL + 7, 147, 99}, {10, ZFIREBALL + 8, 91, 136},
     {10, ZFIREBALL + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#endif
     // MUTWOHANDS  - fireball
     {{1, EMPTY, 127, 170}, {10, ZFIREBALL + 1, 0, 137}, {10, ZFIREBALL + 2, 48, 82},
     {10, ZFIREBALL + 3, 127, 41}, {10, ZFIREBALL + 4, 210, 9}, {10, ZFIREBALL + 5, 284, 26},
     {10, ZFIREBALL + 6, 213, 63}, {10, ZFIREBALL + 7, 147, 99}, {10, ZFIREBALL + 8, 91, 136},
     {10, ZFIREBALL + 9, 46, 183}, {1, EMPTY, 127, 170}, {1, EMPTY, 127, 170},
     {1, EMPTY, 127, 170}
     },
#if 0
     // MUTWOHANDS  - nuke
     {{10, MUTWOHANDS, 19, 155}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     }
#endif
     // MUTWOHANDS  - nuke
     {{1, EMPTY, 127, 170}, {10, MUTWOHANDS + 1, 0, 128}, {10, MUTWOHANDS + 2, 0, 93},
     {10, MUTWOHANDS + 3, 0, 83}, {10, MUTWOHANDS + 4, 0, 72}, {10, MUTWOHANDS + 5, 0, 83},
     {10, MUTWOHANDS + 6, 10, 96}, {10, MUTWOHANDS + 7, 43, 109}, {10, MUTWOHANDS + 8, 69, 113},
     {10, MUTWOHANDS + 9, 65, 115}, {10, MUTWOHANDS + 10, 64, 117}, {10, MUTWOHANDS + 11, 63, 117},
     {1, EMPTY, 127, 170}
     }
};


// END OF THE NEW WEAPONS

#define MAXSTATUSBAR    3

int  heartics = 0;
int  heartincr = 0;
int  showstatusbar = 0;
int  showheart = 0;

int  showmana = 0;

int  showbook = 0;
int  showbooktype = 0;
int  showbookflip;
int  showbookanim;
int  currentorb;
int  currentpotion;

void
loadinglevelmsg(void)
{
#if 0
     int  i,j;

     if (svga) {
          rotatesprite(0L << 16, 0L << 16, 65536L, 0, VMAIN, 0,
                       0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
     }
     else {
          rotatesprite(0L << 16, 0L << 16, 65536L, 0, VMAINBLANK, 0,
                       0, 8 + 16, 0L, 0L, xdim - 1L, ydim - 1L);
     }
     strcpy(tempbuf,"LOADING LEVEL");
     for (i=0,j=0 ; i < strlen(tempbuf) ; i++) {
          j += tilesizx[DAFONT];
     }
     fancyfont((xdim>>1)-(j>>1), (ydim>>1), DAFONT, tempbuf, 1);
     nextpage();
#endif
}

extern
int  quicksaveflag;

void
playerdead(struct player * plr)
{

     int  i,j,loadlast=0;
     int  spellbookpage;
     long clockgoal,
          hiz,hihit,loz,lohit,
          goalz;
     long goaltime;
     int  exit = 0;

     if (playerdie == 1) {
          return;
     }
     if (plr->potion[0] > 0 && !spiked) {
          i=currentpotion;
          currentpotion=0;
          usapotion(plr);
          currentpotion=i;
          updatepics();
          return;
     }
     clockgoal = totalclock + 240;

     playerdie = 1;
     currspikeframe = 0;

     if (spiked == 1) {
          spiketics = spikeanimtics[0].daweapontics;
          playsound_loc(S_GORE1, plr->x, plr->y);
          SND_Sound(S_HEARTBEAT);
     }

     SND_PlaySound(S_PLRDIE1, 0, 0, 0, 0);

     netsendmove();

//     while (totalclock < clockgoal) {
     keystatus[keys[KEYFIRE]] = 0;
     keystatus[keys[KEYUSE]] = 0;
     keystatus[1] = 0;
     startredflash(60);
     while (keystatus[keys[KEYFIRE]] == 0 && keystatus[keys[KEYUSE]] == 0
            && keystatus[1] == 0 && keystatus[0x43] == 0) {
          getzrange(plr->x, plr->y, plr->z, plr->sector,
                    &hiz, &hihit, &loz, &lohit, 128L, 0);
          if (plr->horiz < 100 + (YDIM >> 1)) {
               plr->horiz += (synctics << 1);
          }
          if (plr->z < loz-(4<<8)) {
               plr->z += (synctics<<8);
          }
          if (goreon && ((rand()%10) > 8)) {
               startredflash(10);
               chunksofmeat(plr,plr->spritenum,plr->x,plr->y,plr->z,
                            plr->sector,(short)plr->ang);
          }
          drawscreen(plr);
          animateobjs(plr);
          animatetags(plr);
          doanimations((long) synctics);
          dodelayitems((long) synctics);
          nextpage();
     }
     if (keystatus[0x43]) {
          loadlast=1;
          keystatus[0x43]=0;
     }
     loadinglevelmsg();

     goaltime = totalclock + 240;

     while (!exit) {
          if (totalclock > goaltime)
               exit = 1;
          if (keystatus[0x39] > 0)
               exit = 1;
     }

     // check loops wando
     SND_CheckLoops();

     keystatus[0x39] = 0;

     attacktheme = 0;
     vampiretime = 0;
     playertorch = 0;
     shieldtype = 0;
     shieldpoints = 0;
     playerdie = 0;
     spiked = 0;
     shockme = -1;
     poisoned = 0;
     poisontime = 0;
     currspikeframe = 0;
     spellbookflip = 0;
     displaytime = 0;

     plr->oldsector = plr->sector;
     plr->horiz = 100;
     plr->zoom = 256;
     plr->dimension = 3;
     plr->height = PLAYERHEIGHT;

/*
    for(i=0;i<=9;i++) {
     plr->ammo[i]=0;
     plr->weapon[i]=0;
          if( i < 9 ) {
               plr->orb[i]=0;
               plr->orbammo[i]=0;
          }
     }

     plr->weapon[2]=1;
     plr->ammo[2]=50;


     plr->weapon[0]=plr->weapon[1]=1;
     plr->ammo[0]=32000;
     
     for(i=0;i<MAXPOTIONS;i++)
          plr->potion[i]=0;

     for(i=0;i<MAXTREASURES;i++)
          plr->treasure[i]=0;
*/

     selectedgun = 4;

     for (i = 0; i <= 9; i++) {
          if (i < 5) {
               plr->ammo[i] = 40;
               plr->weapon[i] = 1;
          }
          else {
               plr->ammo[i] = 0;
               plr->weapon[i] = 0;
          }
          if (i < 9) {
               plr->orb[i] = 0;
               plr->orbammo[i] = 0;
          }
     }

     if (difficulty > 1) {
          plr->weapon[0] = plr->weapon[1] = 1;
          plr->ammo[0] = 32000;
          plr->ammo[1] = 45;
     }

     for (i = 0; i < MAXPOTIONS; i++)
          plr->potion[i] = 0;

     for (i = 0; i < MAXTREASURES; i++)
          plr->treasure[i] = 0;

     plr->potion[0] = 3;
     plr->potion[3] = 1;

     hasshot = 0;
     orbshot = 0;
     oldmousestatus = 0;

     plr->lvl = 1;
     plr->score = 0;
     plr->health = 100;
     plr->maxhealth = 100;
     plr->armor = 0;
     plr->armortype = 0;
     currentorb = 0;
     currentpotion = 0;

     for (i = 0; i < MAXNUMORBS; i++)
          plr->orbactive[i] = -1;

     lockclock = totalclock;

     currweapon = selectedgun = 4;

     currweaponfired = 3;
     currweaponflip = 0;
     currweaponanim = 0;

     helmettime = -1;

     if (svga == 0) {
          if (plr->screensize <= 320) {
               spellbookpage = spellbookanim[currentorb][8].daweaponframe;
               itemtoscreen(121L, 161L, spellbookpage, 0, 0);

          }
     }
     if (svga == 1) {
          if (plr->screensize == 320) {
               spellbookpage = sspellbookanim[currentorb][8].daweaponframe;
               rotatesprite((121L << 1) << 16,
                            389L,
                            65536L,
                            0,
                            spellbookpage,
                            0,
                            0,
                            8 + 16,
                            0L,
                            0L,
                            xdim - 1L,
                            ydim - 1L);

          }
     }


     justteleported = 1;

     if (netgame == 0) {
          startsong(rand() % 2);
          if (loadlast) {
               quicksaveflag=1;
               loadgame(plr);
               quicksaveflag=0;
               updatestatusbar();
               strcpy(displaybuf,"quick save loaded");
               displaytime = 360;
          }
          else {
               loadnewlevel(mapon);
          }
     }
     else {
          startsong(rand() % 2);
          netrestartplayer(plr);
     }

     displaytime = -1;
     shadowtime = -1;
     helmettime = -1;
     nightglowtime = -1;
     strongtime = -1;
     invisibletime = -1;
     svgahealth = -1;

//     updatepics();

}


void
spikeheart(struct player * plr)
{
     char dabits = 0x02;
     char dashade = sector[plr->sector].ceilingshade;
     long dax,
          day;
     static long spikeclock;

     plr = &player[pyrn];
     spiketics -= (totalclock-spikeclock);
     if (spiketics < 0) {
          spikeclock=totalclock;
          currspikeframe++;
          if (currspikeframe > 4)
               currspikeframe = 4;
          spiketics = spikeanimtics[currspikeframe].daweapontics;
     }
     else
          currweaponframe = spikeanimtics[currspikeframe].daweaponframe;

     spikeframe = spikeanimtics[currspikeframe].daweaponframe;
     dax = spikeanimtics[currspikeframe].currx;
     day = spikeanimtics[currspikeframe].curry;
     overwritesprite(dax, day, spikeframe, dashade, dabits, 0);
     startredflash(10);
}

void
updateloadedplayer(int i)
{

     struct player *plr;
     plr = &player[pyrn];

     playerdie = 0;
     spiked = 0;
     lives = 0;

     plr->oldsector = plr->sector;
     plr->horiz = 100;
     plr->zoom = 256;
     plr->dimension = 3;
     plr->height = PLAYERHEIGHT;
     plr->spritenum = i;

     sprite[plr->spritenum].x = plr->x;
     sprite[plr->spritenum].y = plr->y;
     sprite[plr->spritenum].z = plr->z + (plr->height << 8);
     sprite[plr->spritenum].cstat = 1 + 256;
     sprite[plr->spritenum].picnum = GRONSW;
     sprite[plr->spritenum].shade = 0;
     sprite[plr->spritenum].xrepeat = 36;
     sprite[plr->spritenum].yrepeat = 36;
     sprite[plr->spritenum].ang = plr->ang;
     sprite[plr->spritenum].xvel = 0;
     sprite[plr->spritenum].yvel = 0;
     sprite[plr->spritenum].zvel = 0;
     sprite[plr->spritenum].owner = 4096;
     sprite[plr->spritenum].lotag = 0;
     sprite[plr->spritenum].hitag = 0;
     // sprite[plr->spritenum].pal=1;
     sprite[plr->spritenum].pal = 10;
     sprite[plr->spritenum].sectnum = plr->sector;

     setsprite((short) i, sprite[i].x, sprite[i].y, sprite[i].z);

}

void
initplayersprite(void)
{

     struct player *plr;
     int  i;
     int  spellbookpage;

     plr = &player[pyrn];

     attacktheme = 0;
     spiked = 0;
     shockme = -1;
     lives = 0;

     plr->oldsector = plr->sector;
     plr->horiz = 100;
     plr->zoom = 256;
     plr->dimension = 3;
     plr->height = PLAYERHEIGHT;
     plr->z = sector[plr->sector].floorz - (plr->height << 8);

     plr->spritenum = insertsprite(plr->sector, 0);

     plr->onsomething = 1;

     sprite[plr->spritenum].x = plr->x;
     sprite[plr->spritenum].y = plr->y;
     sprite[plr->spritenum].z = plr->z + (plr->height << 8);
     sprite[plr->spritenum].cstat = 1 + 256 + 32768;
     sprite[plr->spritenum].picnum = GRONSW;
     sprite[plr->spritenum].shade = 0;
     sprite[plr->spritenum].xrepeat = 36;
     sprite[plr->spritenum].yrepeat = 36;
     sprite[plr->spritenum].ang = plr->ang;
     sprite[plr->spritenum].xvel = 0;
     sprite[plr->spritenum].yvel = 0;
     sprite[plr->spritenum].zvel = 0;
     sprite[plr->spritenum].owner = 4096;
     sprite[plr->spritenum].lotag = 0;
     sprite[plr->spritenum].hitag = 0;
     sprite[plr->spritenum].pal = 10;
     sprite[plr->spritenum].clipdist = 48;

     if (loadedgame == 0 && justteleported == 0) {
          vampiretime = 0;
          playertorch = 0;
          shieldtype = 0;
          shieldpoints = 0;
          playerdie = 0;
          poisoned = 0;
          poisontime = -1;

          selectedgun = 4;

          for (i = 0; i <= 9; i++) {
               if (i < 5) {
                    plr->ammo[i] = 40;
                    plr->weapon[i] = 1;
               }
               else {
                    plr->ammo[i] = 0;
                    plr->weapon[i] = 0;
               }
               if (i < 9) {
                    plr->orb[i] = 0;
                    plr->orbammo[i] = 0;
               }
          }

          if (difficulty > 1) {
               plr->weapon[0] = plr->weapon[1] = 1;
               plr->ammo[0] = 32000;
               plr->ammo[1] = 45;
          }

          for (i = 0; i < MAXPOTIONS; i++)
               plr->potion[i] = 0;

          for (i = 0; i < MAXTREASURES; i++)
               plr->treasure[i] = 0;

          plr->potion[0] = 3;
          plr->potion[3] = 1;

          plr->lvl = 1;
          plr->score = 0;
          plr->health = 100;
          plr->maxhealth = 100;
          plr->armor = 0;
          plr->armortype = 0;
          currentorb = 0;
          currentpotion = 0;

          currweapon = selectedgun = 4;

          currweaponfired = 3;
          currweaponflip = 0;


          for (i = 0; i < MAXNUMORBS; i++)
               plr->orbactive[i] = -1;

          lockclock = totalclock;

          spellbookflip = 0;

          if (svga == 0) {
               if (plr->screensize <= 320) {
                    spellbookpage = spellbookanim[currentorb][8].daweaponframe;
                    itemtoscreen(121L, 161L, spellbookpage, 0, 0);
               }
          }
          if (svga == 1) {
               if (plr->screensize == 320) {
                    // spellbookpage=sspellbookanim[currentorb][8].daweaponframe
                    // ;
                    // itemtoscreen(121L<<1,389L,spellbookpage,0,0);
                    spellbookpage = sspellbookanim[currentorb][8].daweaponframe;
                    rotatesprite((121L << 1) << 16,
                                 389L << 16,
                                 65536L,
                                 0,
                                 spellbookpage,
                                 0,
                                 0,
                                 8 + 16,
                                 0L,
                                 0L,
                                 xdim - 1L,
                                 ydim - 1L);


               }
          }

          invincibletime = manatime = -1;
          hasshot = 0;
          orbshot = 0;
          oldmousestatus = 0;

          displaytime = -1;
          shadowtime = -1;
          helmettime = -1;
          nightglowtime = -1;
          strongtime = -1;
          invisibletime = -1;
          svgahealth = -1;
     }

     updatepics();

}

// JOE    - NOTE
//        weapon[1] = dagger
//        weapon[2] = short sword
//        weapon[3] = morning star
//        weapon[4] = long sword / broad sword
//        weapon[5] = battle axe
//        weapon[6] = pike axe
//        weapon[7] = pike axe (throwpike)
//        weapon[8] = magic sword
//        weapon[9] = halberd

void
autoweaponchange(int dagun)
{
     struct player *plr;

     if (currweaponanim > 0)
          return;

     plr=&player[pyrn];

     if (dagun > selectedgun) {
          selectedgun = dagun;
          hasshot = 0;
          currweaponfired = 2;     // drop weapon
          if (enchantedsoundhandle != -1) {
               SND_StopLoop(enchantedsoundhandle);
               enchantedsoundhandle=-1;
          }
          switch (selectedgun) {
          case 1:
               weapondropgoal = 100;
               weapondrop = 0;
               break;
          case 2:
               if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                    enchantedsoundhandle=SND_PlaySound(S_FIREWEAPONLOOP,0,0,0,-1);
               }
               weapondropgoal = 40;
               weapondrop = 0;
               levelpic();
               break;
          case 3:
               if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                    enchantedsoundhandle=SND_PlaySound(S_ENERGYWEAPONLOOP,0,0,0,-1);
               }
               weapondropgoal = 100;
               weapondrop = 0;
               levelpic();
               break;
          case 4:
               weapondropgoal = 40;
               weapondrop = 0;
               levelpic();
               break;
          case 5:
               if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                    enchantedsoundhandle=SND_PlaySound(S_ENERGYWEAPONLOOP,0,0,0,-1);
               }
               weapondropgoal = 40;
               weapondrop = 0;
               levelpic();
               break;
          case 6:
               if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                    enchantedsoundhandle=SND_PlaySound(S_FIREWEAPONLOOP,0,0,0,-1);
               }
               weapondropgoal = 40;
               weapondrop = 0;
               levelpic();
               break;
          case 7:
               if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                    enchantedsoundhandle=SND_PlaySound(S_ENERGYWEAPONLOOP,0,0,0,-1);
               }
               weapondropgoal = 40;
               weapondrop = 0;
               levelpic();
               break;
          case 8:
               if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                    enchantedsoundhandle=SND_PlaySound(S_FIREWEAPONLOOP,0,0,0,-1);
               }
               weapondropgoal = 40;
               weapondrop = 0;
               levelpic();
               break;
          case 9:
               weapondropgoal = 40;
               weapondrop = 0;
               levelpic();
               break;
          }
     }

}

void
weaponchange(void)
{

     long i;
     int  j;
     struct player *plr;
     char temp[50];

     plr = &player[pyrn];

     if (currweaponanim == 0 && currweaponflip == 0)
          for (i = 0x2; i <= 0xb; i++) {
               if (keystatus[i] > 0 && plr->weapon[i - 0x2] > 0) {
                    selectedgun = i - 0x2;
                    keystatus[i] = 0;
                    hasshot = 0;
                    currweaponfired = 2;     // drop weapon
                    levelpic();
                    if (enchantedsoundhandle != -1) {
                         SND_StopLoop(enchantedsoundhandle);
                         enchantedsoundhandle=-1;
                    }
                    switch (selectedgun) {
                    case 0:
                         weapondropgoal = 40;
                         weapondrop = 0;
                         break;
                    case 1:
                         weapondropgoal = 100;
                         weapondrop = 0;
                         break;
                    case 2:
                         if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_FIREWEAPONLOOP,0,0,0,-1);
                         }
                         weapondropgoal = 100;
                         weapondrop = 0;
                         break;
                    case 3:
                         if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_ENERGYWEAPONLOOP,0,0,0,-1);
                         }
                         weapondropgoal = 100;
                         weapondrop = 0;
                         break;
                    case 4:
                         weapondropgoal = 40;
                         weapondrop = 0;
                         break;
                    case 5:
                         if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_ENERGYWEAPONLOOP,0,0,0,-1);
                         }
                         weapondropgoal = 40;
                         weapondrop = 0;
                         break;
                    case 6:
                         if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_FIREWEAPONLOOP,0,0,0,-1);
                         }
                         weapondropgoal = 40;
                         weapondrop = 0;
                         break;
                    case 7:
                         if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_ENERGYWEAPONLOOP,0,0,0,-1);
                         }
                         weapondropgoal = 40;
                         weapondrop = 0;
                         break;
                    case 8:
                         if (enchantedsoundhandle == -1 && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_ENERGYWEAPONLOOP,0,0,0,-1);
                         }
                         weapondropgoal = 40;
                         weapondrop = 0;
                         break;
                    case 9:
                         weapondropgoal = 40;
                         weapondrop = 0;
                         break;
                    }
               }
          }

     if (currweaponflip == 0) {
          for (i = 0x3b; i <= 0x42; i++) {
               if (keystatus[i] > 0) {
#if 0
                    if (selectedgun > 0) {
                         if (i != 0x3B+2 && i != 0x3B+3 && i != 0x3B+7) {
                              hasshot = 0;
                              currweaponfired = 2;     // drop weapon
                              weapondropgoal = 100;
                              weapondrop = 0;
                              selectedgun = 0;
                         }
                         // selectedgun=spellcasthands
                         levelpic();
                    }
#endif
                    currentorb = i - 0x3b;
                    if (spellbookflip == 0) {
                         spellbook = 0;
                         spellbooktics = 10;
                         spellbookflip = 1;
                         // JSA_NEW
                         SND_PlaySound(S_PAGE, 0, 0, 0, 0);
                         displayspelltext();
                         spelltime = 360;
                    }
                    orbshot = 0;
               }
               keystatus[i] = 0;
          }
          orbpic(currentorb);
     }


     for (j = 0; j < MAXNUMORBS; j++) {
          if (plr->orbactive[j] > -1) {
               plr->orbactive[j] -= synctics;
          }
     }

     if (keystatus[0x1a] > 0) {
          currentpotion--;
          if (currentpotion < 0)
               currentpotion = 4;
          keystatus[0x1a] = 0;
          // JSA_NEW
          SND_PlaySound(S_BOTTLES, 0, 0, 0, 0);
          potionpic(currentpotion);
          potiontext();
     }
     if (keystatus[0x1b] > 0) {
          currentpotion++;
          if (currentpotion > 4)   // MAXPOTIONS
               currentpotion = 0;
          keystatus[0x1B] = 0;
          // JSA_NEW
          SND_PlaySound(S_BOTTLES, 0, 0, 0, 0);
          potionpic(currentpotion);
          potiontext();
     }


}


void 
potiontext(void)
{

     struct player *plr;

     plr = &player[pyrn];

     if (plr->potion[currentpotion] > 0)
          switch (currentpotion) {
          case 0:
               strcpy(displaybuf, "Health Potion");
               displaytime = 240;
               break;
          case 1:
               strcpy(displaybuf, "Strength Potion");
               displaytime = 240;
               break;
          case 2:
               strcpy(displaybuf, "Cure Poison Potion");
               displaytime = 240;
               break;
          case 3:
               strcpy(displaybuf, "Resist Fire Potion");
               displaytime = 240;
               break;
          case 4:
               strcpy(displaybuf, "Invisiblity Potion");
               displaytime = 240;
               break;
          }
}

void 
swingdacrunch(int daweapon)
{


     struct player *plr;

     plr = &player[pyrn];

     switch (daweapon) {
     case 0:                       // fist
          playsound_loc(S_SOCK1 + (rand() % 4), plr->x, plr->y);
          break;
     case 1:                       // dagger
          if (rand() % 2)
               playsound_loc(S_GORE1 + (rand() % 4), plr->x, plr->y);
          break;
     case 2:                       // short sword
          playsound_loc(S_SWORD2 + (rand() % 3), plr->x, plr->y);
          break;
     case 3:                       // morningstar
          playsound_loc(S_SOCK1 + (rand() % 4), plr->x, plr->y);
          break;
     case 4:                       // broad sword
          playsound_loc(S_SWORD1 + (rand() % 3), plr->x, plr->y);
          break;
     case 5:                       // battle axe
          if (rand() % 2)
               playsound_loc(S_SOCK1 + (rand() % 4), plr->x, plr->y);
          else
               playsound_loc(S_SWORD1 + (rand() % 3), plr->x, plr->y);
          break;
     case 6:                       // bow

          break;
     case 7:                       // pike
          if (rand() % 2)
               playsound_loc(S_SOCK1 + (rand() % 4), plr->x, plr->y);
          else
               playsound_loc(S_SWORD1 + (rand() % 3), plr->x, plr->y);
          break;
     case 8:                       // two handed sword
          playsound_loc(S_SWORD1 + (rand() % 2), plr->x, plr->y);
          break;
     case 9:                       // halberd
          if (rand() % 2)
               playsound_loc(S_SOCK1 + (rand() % 4), plr->x, plr->y);
          else
               playsound_loc(S_SWORD1 + (rand() % 3), plr->x, plr->y);
          break;
     }

}

void 
swingdasound(int daweapon)
{

     switch (daweapon) {
     case 0:                       // fist
          SND_PlaySound(S_PLRWEAPON0, 0, 0, 0, 0);
          break;
     case 1:                       // knife
          SND_PlaySound(S_PLRWEAPON1, 0, 0, 0, 0);
          break;
     case 2:                       // short sword
          if(enchantedsoundhandle!=-1)
               SND_PlaySound(S_FIRESWING, 0, 0, 0, 0);
          else
               SND_PlaySound(S_PLRWEAPON4, 0, 0, 0, 0);
          break;
     case 3:                       // mace
          if(enchantedsoundhandle!=-1) {
               SND_PlaySound(S_FIRESWING, 0, 0, 0, 0);
               SND_PlaySound(S_FIREBALL, 0, 0, 0, 0);
          }
          else
               SND_PlaySound(S_PLRWEAPON2, 0, 0, 0, 0);
          break;
     case 4:                       //
          SND_PlaySound(S_PLRWEAPON4, 0, 0, 0, 0);
          break;
     case 5:                       // sword
          SND_PlaySound(S_PLRWEAPON4, 0, 0, 0, 0);
          break;
     case 6:                       // bow
          SND_PlaySound(S_PLRWEAPON3, 0, 0, 0, 0);
          break;
     case 7:                       //
          if(enchantedsoundhandle!=-1)
               SND_PlaySound(S_ENERGYSWING, 0, 0, 0, 0);
          else
               SND_PlaySound(S_PLRWEAPON4, 0, 0, 0, 0);
          break;
     case 8:                       //
          if(enchantedsoundhandle!=-1)
               SND_PlaySound(S_ENERGYSWING, 0, 0, 0, 0);
          else
               SND_PlaySound(S_PLRWEAPON4, 0, 0, 0, 0);
          break;
     case 9:                       //
          SND_PlaySound(S_PLRWEAPON4, 0, 0, 0, 0);
          break;
     }


}

short monsterangle[MAXSPRITESONSCREEN],
     monsterlist[MAXSPRITESONSCREEN];

long shootgunzvel;

extern
spritetype tspritelist[];

extern
int  tspritelistcnt;

void
lockon(struct player *plr,short numshots,short shootguntype)
{
     short daang,i,k,n,s;
     spritetype *spr;

     spr=&tspritelist[0];
     for (i=0,n=0 ; i < tspritelistcnt && n < numshots ; i++,spr++) {
          if (cansee(plr->x, plr->y, plr->z, plr->sector,
                     spr->x, spr->y, spr->z - (tilesizy[spr->picnum] << 7),
                     spr->sectnum) == 1) {
               switch (spr->picnum) {
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
                    monsterangle[n]=getangle(tspritelist[i].x-plr->x,
                                             tspritelist[i].y-plr->y);
                    monsterlist[n]=i;
                    n++;
                    break;
               }
          }
     }
     daang=plr->ang-((numshots*(128/numshots))>>1);
     for (k=0,s=0 ; k < numshots ; k++) {
          if (n > 0) {
               spr=&tspritelist[monsterlist[s]];
               daang=monsterangle[s];
               shootgunzvel=((spr->z-(32<<8)-plr->z)<<8)/
                             ksqrt((spr->x-plr->x)*(spr->x-plr->x)+
                                   (spr->y-plr->y)*(spr->y-plr->y));
               s=(s+1)%n;
          }
          else {
               daang+=(128/numshots);
          }
          shootgun(plr, daang, shootguntype);
     }
}

void
swingdaweapon(struct player * plr)
{
     short daang,k,n,s;

     daang = plr->ang;

     if (currweaponframe == BOWWALK + 5 && plr->ammo[6] > 0) {
          currweaponfired = 5;
          oldmousestatus = 0;      // NEW
          currweaponanim = 0;
     }
     else if (currweaponframe == ZBOWATTACK + 4 && plr->ammo[6] > 0) {
          currweaponfired = 5;
          oldmousestatus = 0;      // NEW
          currweaponanim = 0;
     }

     if (currweaponframe == BOWWALK + 5 && plr->ammo[6] <= 0) {
          currweaponfired = 0;
          oldmousestatus = 0;
          currweaponanim = 0;
          return;
     }
     else if (currweaponframe == ZBOWATTACK + 4 && plr->ammo[6] <= 0) {
          currweaponfired = 0;
          oldmousestatus = 0;
          currweaponanim = 0;
          return;
     }

     if (currweaponframe == PIKEATTACK1 + 4
         && plr->weapon[7] == 2
         && plr->ammo[7] > 0) {
          shootgun(plr, daang, 10);
          playsound_loc(S_GENTHROW, plr->x, plr->y);
          hasshot = 1;
          return;
     }
     else if (currweaponframe == ZPIKEATTACK + 4
              && plr->weapon[7] == 3
              && plr->ammo[7] > 0) {
          lockon(plr,3,10);
          playsound_loc(S_GENTHROW, plr->x, plr->y);
          hasshot = 1;
          return;
     }


     switch (selectedgun) {
     case 0:                       // fist  & close combat weapons
          shootgun(plr, daang, 0);
          hasshot = 1;
          break;
     case 1:                       // knife
          shootgun(plr, daang, 0);
          hasshot = 1;
          break;
     case 2:                       // shortsword
          shootgun(plr, daang, 0);
          hasshot = 1;
          break;
     case 3:                       // morningstar
          shootgun(plr, daang, 0);
          if (plr->weapon[selectedgun] == 3) {
               lockon(plr,1,3);
          }
          hasshot = 1;
          break;
     case 4:                       // sword
          shootgun(plr, daang, 0);
          hasshot = 1;
          break;
     case 5:                       // battleaxe
          if (enchantedsoundhandle != -1) {
               SND_PlaySound(S_ENERGYSWING, 0, 0, 0, 0);
          }
          shootgun(plr, daang, 0);
          hasshot = 1;
          break;
     case 6:                       // bow
          if(enchantedsoundhandle!=-1) {
               SND_PlaySound(S_FIREBALL, 0, 0, 0, 0);
               SND_PlaySound(S_PLRWEAPON3, 0, 0, 0, 0);
          }
          shootgun(plr, daang, 1);
          hasshot = 1;
          break;
     case 7:                       // pike
          shootgun(plr, daang, 0);
          hasshot = 1;
          break;
     case 8:                       // two handed
          shootgun(plr, daang, 0);
          hasshot = 1;
          break;
     case 9:                       // halberd
          shootgun(plr, daang, 0);
          hasshot = 1;
          break;

     }

}

extern
int  nobreakflag;

int  weaponuseless = 0;

void
plrfireweapon(struct player * plr)
{

     char guntype;
     long x,
          y,
          z;
     long dasectnum,
          daz;
     short daang,
          dahoriz;
     short k;
     int  i;
     int  bestweapon;


     if (currweaponfired == 4) {
#if 0
          if (currentorb == 0 || currentorb == 2 || currentorb == 3
              || currentorb == 6 || currentorb == 7) {
               return;   // don't do hands
          }
#endif
          if (plr->weapon[currweapon] == 1) {
               currweapontics = throwanimtics[currentorb][0].daweapontics;
          }
          else {
               currweapontics = zthrowanimtics[currentorb][0].daweapontics;
          }
          return;
     }

     if (plr->ammo[selectedgun] <= 0) {
          if (currweapon == 6) {
               for (i = 0; i < MAXWEAPONS; i++) {
                    if (plr->ammo[i] > 0 && plr->weapon[i] == 1) {
                         selectedgun = i;
                         hasshot = 0;
                         currweaponfired = 2;     // drop weapon
                         weapondropgoal = 100;
                         weapondrop = 0;
                         levelpic();
                    }
               }
          }
          return;
     }
     else {
          madeahit = 0;
          if (nobreakflag == 0) {
               plr->ammo[selectedgun]--;
          }
          if (plr->weapon[selectedgun] == 3) {
               if (plr->ammo[selectedgun] == 0) {
                    plr->weapon[selectedgun] = plr->preenchantedweapon[selectedgun];
                    plr->ammo[selectedgun] = plr->preenchantedammo[selectedgun];
                    if (enchantedsoundhandle != -1) {
                         SND_StopLoop(enchantedsoundhandle);
                         enchantedsoundhandle=-1;
                    }
               }
          }
          if (plr->ammo[selectedgun] <= 0 || plr->ammo[selectedgun] == 10) {
               switch (selectedgun) {
               case 0:             // fist
                    plr->ammo[0] = 9999;
                    break;
               case 1:             // knife
                    if (plr->ammo[selectedgun] == 10) {
                         strcpy(displaybuf, "Dagger is damaged");
                         displaytime = 360;
                    }
                    if (plr->ammo[selectedgun] <= 0) {
                         plr->ammo[1] = 0;
                         plr->weapon[1] = 0;
                         strcpy(displaybuf, "Dagger is Useless");
                         displaytime = 360;
                         weaponuseless = 1;
                    }
                    break;
               case 2:             // short sword
                    if (plr->ammo[selectedgun] == 10) {
                         strcpy(displaybuf, "Short Sword is damaged");
                         displaytime = 360;
                    }
                    if (plr->ammo[selectedgun] <= 0) {
                         plr->ammo[2] = 0;
                         plr->weapon[2] = 0;
                         strcpy(displaybuf, "Short Sword is Useless");
                         displaytime = 360;
                         weaponuseless = 1;
                    }
                    break;
               case 3:             // mace
                    if (plr->ammo[selectedgun] == 10) {
                         strcpy(displaybuf, "Morning Star is damaged");
                         displaytime = 360;
                    }
                    if (plr->ammo[selectedgun] <= 0) {
                         plr->ammo[3] = 0;
                         plr->weapon[3] = 0;
                         strcpy(displaybuf, "Morning Star is Useless");
                         displaytime = 360;
                         weaponuseless = 1;
                    }
                    break;

               case 4:             // sword
                    if (plr->ammo[selectedgun] == 10) {
                         strcpy(displaybuf, "Sword is damaged");
                         displaytime = 360;
                    }
                    if (plr->ammo[selectedgun] <= 0) {
                         plr->ammo[4] = 0;
                         plr->weapon[4] = 0;
                         strcpy(displaybuf, "Sword is Useless");
                         displaytime = 360;
                         weaponuseless = 1;
                    }
                    break;
               case 5:             // battle axe
                    if (plr->ammo[selectedgun] == 10) {
                         strcpy(displaybuf, "Battle axe is damaged");
                         displaytime = 360;
                    }
                    if (plr->ammo[selectedgun] <= 0) {
                         plr->ammo[5] = 0;
                         plr->weapon[5] = 0;
                         strcpy(displaybuf, "Battle axe is Useless");
                         displaytime = 360;
                         weaponuseless = 1;
                    }
                    break;
               case 6:             // bow
                    break;
               case 7:             // pike
                    if (plr->weapon[7] == 1) {
                         if (plr->ammo[selectedgun] == 10) {
                              strcpy(displaybuf, "Pike is damaged");
                              displaytime = 360;
                         }
                         if (plr->ammo[selectedgun] <= 0) {
                              plr->ammo[7] = 0;
                              plr->weapon[7] = 0;
                              strcpy(displaybuf, "Pike is Useless");
                              displaytime = 360;
                              weaponuseless = 1;
                         }
                    }
                    if (plr->weapon[7] == 2 && plr->ammo[7] <= 0) {
                         plr->weapon[7] = 1;
                         plr->ammo[7] = 30;
                    }
                    break;
               case 8:             // two handed sword
                    if (plr->ammo[selectedgun] == 10) {
                         strcpy(displaybuf, "Magic Sword is damaged");
                         displaytime = 360;
                    }
                    if (plr->ammo[selectedgun] <= 0) {
                         plr->ammo[8] = 0;
                         plr->weapon[8] = 0;
                         strcpy(displaybuf, "Magic Sword is Useless");
                         displaytime = 360;
                         weaponuseless = 1;
                    }
                    break;
               case 9:             // halberd
                    if (plr->ammo[selectedgun] == 10) {
                         strcpy(displaybuf, "Halberd is damaged");
                         displaytime = 360;
                    }
                    if (plr->ammo[selectedgun] <= 0) {
                         plr->ammo[9] = 0;
                         plr->weapon[9] = 0;
                         strcpy(displaybuf, "Halberd is Useless");
                         displaytime = 360;
                         weaponuseless = 1;
                    }
                    break;
               }
          }
     }

     if (weaponuseless == 1)
          for (i = 0; i < MAXWEAPONS; i++) {
               if (plr->weapon[i] > 0 && plr->ammo[i] > 0) {
                    currweapon = selectedgun = i;
                    // hasshot=0;
                    // currweaponfired=2; // drop weapon
                    currweaponfired = 3;     // ready weapon
                    // weapondropgoal=100;
                    // weapondrop=0;
                    currweaponflip = 0;
                    weaponuseless = 0;
                    // autoweaponchange(i);
                    levelpic();
               }
          }
     else
          currweaponfired = 1;


     if (currweapon == 6 || selectedgun == 6)
          levelpic();

     if (currweapon == 7 || selectedgun == 7)
          levelpic();

     currweapon = selectedgun;

     // start from the beginning to cycle
     // currweaponfired=1;

     currweaponattackstyle = krand() % 2;

     if (plr->weapon[7] == 2 && currweapon == 7) {
          currweaponattackstyle = 0;
     }
     else if (plr->weapon[7] == 3 && currweapon == 7) {
          currweaponattackstyle = 0;
     }

     if (currweapon == 9) {
          if (krand() % 100 > 80)
               currweaponattackstyle = 0;
          else
               currweaponattackstyle = 1;
     }

     if (currweaponanim > 11) {
          if (plr->weapon[currweapon] == 1)
               currweapontics = weaponanimtics[currweapon][0].daweapontics;
          else
               currweapontics = zweaponanimtics[currweapon][0].daweapontics;
     }


}

void
activatedaorb(struct player * plr)
{


     if (plr->orbammo[currentorb] <= 0)
          return;

     switch (currentorb) {
     case 0:                       // SCARE
          // shadowtime=1200+(plr->lvl*120);
          break;
     case 1:                       // NIGHT VISION
          // nightglowtime=2400+(plr->lvl*600);
          break;
     case 2:                       // FREEZE
          plr->orbactive[currentorb] = -1;
          break;
     case 3:                       // MAGIC ARROW
          plr->orbactive[currentorb] = -1;
          break;
     case 4:                       // OPEN DOORS
          plr->orbactive[currentorb] = -1;
          break;
     case 5:                       // FLY
          // plr->orbactive[currentorb]=3600+(plr->lvl*600);
          break;
     case 6:                       // FIREBALL
          plr->orbactive[currentorb] = -1;
          break;
     case 7:                       // NUKE
          plr->orbactive[currentorb] = -1;
          break;
     }

     if (plr->orbammo[currentorb] <= 0) {
          plr->orb[currentorb] = 0;
          return;
     }
     else
          plr->orbammo[currentorb]--;

     currweaponfired = 4;
#if 0
     if (currentorb == 0 || currentorb == 2 || currentorb == 3
         || currentorb == 6 || currentorb == 7) {
          return;   // no hand animations
     }
#endif
     if (plr->weapon[currweapon] == 1)
          currweapontics = throwanimtics[currentorb][0].daweapontics;
     else
          currweapontics = zthrowanimtics[currentorb][0].daweapontics;

}

void
plruse(struct player * plr)
{

     short hitsect,
          hitwall,
          hitsprite,
          daang2,
          daang;
     long i,
          daz2,
          hitx,
          hity,
          hitz;
     long hit,
          dax,
          day;

     neartag(plr->x, plr->y, plr->z, plr->sector, plr->ang,
      &neartagsector, &neartagwall, &neartagsprite, &neartaghitdist, 1024, 3);
     if (neartagsector >= 0) {
          if (sector[neartagsector].hitag == 0) {
               operatesector(neartagsector);
          }
          else {
               daang = plr->ang;
               daang2 = daang + 2048;
               daz2 = (100 - plr->horiz) * 2000;
               hitscan(plr->x, plr->y, plr->z, plr->sector, // Start position
                       sintable[(daang2 + 2560) & 2047],    // X vector of 3D ang
                       sintable[(daang2 + 2048) & 2047],    // Y vector of 3D ang
                       daz2,       // Z vector of 3D ang
                       &hitsect, &hitwall, &hitsprite, &hitx, &hity, &hitz);
               if (hitwall >= 0) {
                    if ((labs(plr->x - hitx) + labs(plr->y - hity) < 512) && (labs((plr->z >> 8) - ((hitz >> 8) - (64))) <= (512 >> 3))) {
                         switch (wall[hitwall].picnum) {
                         case PENTADOOR1:
                         case PENTADOOR2:
                         case PENTADOOR3:
                         case PENTADOOR4:
                         case PENTADOOR5:
                         case PENTADOOR6:
                         case PENTADOOR7:
                              strcpy(displaybuf, "find door trigger");
                              displaytime = 360;
                              break;
                         }
                    }
               }
               playsound_loc(S_PUSH1 + (rand() % 2), plr->x, plr->y);
          }

     }
     if (neartagsprite >= 0) {
          if (sprite[neartagsprite].lotag == 1) {
               switch (sprite[neartagsprite].picnum) {
               case SKULLPULLCHAIN1:
               case PULLCHAIN1:
                    sprite[neartagsprite].lotag = 0;
                    newstatus(neartagsprite, PULLTHECHAIN);
                    break;
               case LEVERUP:
                    sprite[neartagsprite].lotag = 0;
                    newstatus(neartagsprite, ANIMLEVERUP);
                    break;
               }
               for (i = 0; i < numsectors; i++)
                    if (sector[i].hitag == sprite[neartagsprite].hitag)
                         operatesector(i);
          }
          else
               operatesprite(neartagsprite);
     }

}

void 
loadnewlevel(int mapon)
{
     struct player *plr;
     char mapbuf[_MAX_FNAME+_MAX_EXT];

     plr = &player[pyrn];

     strcpy(mapbuf, "level");
     itoa(mapon, tempbuf, 10);
     strcat(mapbuf, tempbuf);
     strcat(mapbuf, ".map");

//JSA_DEMO2
     // SND_SongFlush();
     // SND_StartMusic(mapon-1);
//JSA_ENDS
     loadinglevelmsg();
     setupboard(mapbuf);
     initplayersprite();
     updatepics();
     lockclock = totalclock;
}

void 
victory(int demoflag)
{
     long goaltime;
     int  exit;

     if (!demoflag) {
          smkplayseq(1);
          smkplayseq(3);
          smkplayseq(4);
          shutdown();
     }
     goaltime = totalclock + 360;
     victor = 1;

     if (svga == 1) {
          keystatus[0x39] = 0;
          keystatus[1] = 0;
          SND_Sound(S_PICKUPFLAG);
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
          SND_Sound(S_PICKUPFLAG);
          itemtoscreen(0L, 0L, THEORDER, 0, 0);
          nextpage();
          exit = 0;
          while (!exit) {
               if (keystatus[0x39] > 0 || keystatus[1] > 0)
                    exit = 1;
          }
          keystatus[0x39] = 0;
          keystatus[1] = 0;
     }
     shutdown();
}

extern
unsigned hSOSDriverHandles[];

void
drawweapons(struct player * plr)
{

     long dax,
          day;
     long snakex = 0;
     long snakey = 0;
     char dabits;
     char dashade;
     char dapalnum;
     short startwall,
          endwall;
     short rotateit;
     int  j,
          k;


     if (spelltime > 0) {
          spelltime -= synctics;
     }

     if (spellbook == 8 && spelltime > 0 && plr->screensize > 320) {
          if (svga == 1) {
               spellbookframe = sspellbookanim[currentorb][8].daweaponframe;
               dax = sspellbookanim[currentorb][8].currx;
               day = sspellbookanim[currentorb][8].curry;
               itemtoscreen(dax << 1, day, spellbookframe, 0, 0);
               itoa(plr->orbammo[currentorb], tempbuf, 10);
               fancyfont(126L << 1, 439L, SSCOREFONT - 26, tempbuf, 0);
          }
          else {
               spellbookframe = spellbookanim[currentorb][8].daweaponframe;
               dax = spellbookanim[currentorb][8].currx;
               day = spellbookanim[currentorb][8].curry;
               itemtoscreen(dax, day, spellbookframe, 0, 0);
               itoa(plr->orbammo[currentorb], tempbuf, 10);
               fancyfont(126L, 181L, SCOREFONT - 26, tempbuf, 0);
          }
     }


     if (shadowtime > 0) {
          dashade = 31, dapalnum = 0;
     }
     else {
          if (plr->weapon[currweapon] == 3) {
               if (enchantedsoundhandle == -1) {
                    switch (currweapon) {
                    case 2:
                         if (enchantedsoundhandle == -1
                             && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_FIREWEAPONLOOP,
                                                                 0,0,0,-1);
                         }
                         break;
                    case 3:
                    case 5:
                         if (enchantedsoundhandle == -1
                             && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_ENERGYWEAPONLOOP,
                                                                 0,0,0,-1);
                         }
                         break;
                    case 6:
                         if (enchantedsoundhandle == -1
                             && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_FIREWEAPONLOOP,
                                                                 0,0,0,-1);
                         }
                         break;
                    case 7:
                         if (enchantedsoundhandle == -1
                             && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_ENERGYWEAPONLOOP,
                                                                 0,0,0,-1);
                         }
                         break;
                    case 8:
                         if (enchantedsoundhandle == -1
                             && plr->weapon[selectedgun] == 3) {
                              enchantedsoundhandle=SND_PlaySound(S_FIREWEAPONLOOP,
                                                                 0,0,0,-1);
                         }
                         break;
                    }
               }
               dashade = 0;
               dapalnum = 0;
          }
          else {
               dashade = sector[plr->sector].ceilingshade;
               dapalnum = 0;
          }
     }

     if (invisibletime > 0)
          // dabits=0x06;
          dabits = 2 + 4;
     else
          // dabits=0x02;
          dabits = 2;

     if (currweaponflip == 1) {
          // dabits+=0x08;
          dabits |= 2;
          dabits |= 4;
     }

     if (currweapon == 0 && dahand == 0)
          if (rand() % 2 == 0)
               dahand = 1;
          else
               dahand = 2;


     switch (currweaponfired) {
     case 6:

          switch (currweapon) {
          case 1:                  // knife
               if (currweaponframe == KNIFEATTACK2 + 1)
                    if ((currweaponanim == 2 || currweaponanim == 10) && currweapontics == 8)
                         swingdasound(currweapon);
               break;
          case 3:                  // morning
               if (currweaponframe == MORNINGATTACK2 + 3)
                    if (currweaponanim == 3 && currweapontics == 12)
                         swingdasound(currweapon);
               break;
          }
          switch (currweaponframe) {
          case RFIST + 5:
          case KNIFEATTACK + 6:
          case ZKNIFEATTACK + 5:   // new
          case MORNINGSTAR + 5:
          case SWORDATTACK + 7:
          case BOWWALK + 5:
          case ZBOWATTACK + 4:
          case KNIFEATTACK2 + 2:
          case ZKNIFEATTACK2 + 2:
          case SWORDATTACK2 + 6:
          case MORNINGATTACK2 + 3:
          case HALBERDATTACK1 + 3:
          case HALBERDATTACK2 + 3:
          case BIGAXEATTACK + 7:
          case BIGAXEATTACK2 + 6:
          case PIKEATTACK1 + 4:
          case PIKEATTACK2 + 4:
          case EXCALATTACK1 + 7:
          case EXCALATTACK2 + 5:
          case GOBSWORDATTACK2 + 4:
          case GOBSWORDATTACK + 4:
          case ZSHORTATTACK + 7:
          case ZSHORTATTACK2 + 4:
          case ZSTARATTACK + 7:
          case ZSTARATTACK2 + 3:
          case ZAXEATTACK + 12:
          case ZAXEATTACK2 + 6:
          case ZPIKEATTACK + 4:
          case ZPIKEATTACK2 + 4:
          case ZTWOHANDATTACK + 12:
          case ZTWOHANDATTACK2 + 5:
          case ZHALBERDATTACK + 4:
          case ZHALBERDATTACK2 + 3:
               // here
               swingdaweapon(plr);
               break;
          }

          currweapontics -= synctics;

          if (helmettime > 0)
               currweapontics--;

          if (currweapontics < 0) {

               currweaponanim++;

               if (currweaponanim > 11) {
                    currweaponanim = 0;
                    currweaponfired = 0;
                    currweaponflip = 0;
                    currweapon = selectedgun;
                    oldmousestatus = 0;

                    if (dahand > 0)
                         dahand = 0;

               }

               if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                    currweapontics = lefthandanimtics[currweapon][currweaponanim].daweapontics;
                    currweaponframe = lefthandanimtics[currweapon][currweaponanim].daweaponframe;
                    dax = (long) lefthandanimtics[currweapon][currweaponanim].currx;
                    day = (long) lefthandanimtics[currweapon][currweaponanim].curry + 8;
               }
               else {
                    currweapontics = zlefthandanimtics[currweapon][currweaponanim].daweapontics;
                    currweaponframe = zlefthandanimtics[currweapon][currweaponanim].daweaponframe;
                    dax = (long) zlefthandanimtics[currweapon][currweaponanim].currx;
                    day = (long) zlefthandanimtics[currweapon][currweaponanim].curry + 8;
               }
          }

          else {
               if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                    currweaponframe = lefthandanimtics[currweapon][currweaponanim].daweaponframe;
                    dax = (long) lefthandanimtics[currweapon][currweaponanim].currx;
                    day = (long) lefthandanimtics[currweapon][currweaponanim].curry + 8;
               }
               else {
                    currweaponframe = zlefthandanimtics[currweapon][currweaponanim].daweaponframe;
                    dax = (long) zlefthandanimtics[currweapon][currweaponanim].currx;
                    day = (long) zlefthandanimtics[currweapon][currweaponanim].curry + 8;
               }
          }



          if (currweapon == 0 && currweaponframe != EMPTY) {
               if (dahand == 1)
                    rotatesprite(dax,
                                 day,
                                 65536L,
                                 0,
                                 currweaponframe,
                                 0,
                                 0,
                                 dabits,
                                 windowx1,
                                 windowy1,
                                 windowx2,
                                 windowy2);
               else if (dahand == 2) {
                    dax = (long) lefthandanimtics[0][currweaponanim].currx;
                    day = (long) lefthandanimtics[0][currweaponanim].curry + 8;
                    overwritesprite(dax, day + 5, currweaponframe + 6, dashade, dabits, dapalnum);
               }
          }
          else {
               if (currweaponframe != EMPTY) {

                    if (plr->weapon[currweapon] == 1)
                         dax = lefthandanimtics[currweapon][currweaponanim].currx;
                    else
                         dax = zlefthandanimtics[currweapon][currweaponanim].currx;

                    rotatesprite((dax + (tilesizx[currweaponframe] >> 1)) << 16,
                               (day + (tilesizy[currweaponframe] >> 1)) << 16,
                                 65536L,
                                 1024,
                                 currweaponframe,
                                 0,
                                 0,
                                 dabits,
                                 windowx1, windowy1, windowx2, windowy2);

               }
          }

          if (currweapon == 0 && currweaponframe == EMPTY) {
               dahand = 0;
               oldmousestatus = 0;
               currweaponanim = 0;
               currweaponfired = 0;
          }

          if (selectedgun == 4 && currweaponframe == EMPTY) {
               currweaponanim = 0;
               currweaponfired = 0;
               currweaponflip = 0;
               currweapon = selectedgun;
               oldmousestatus = 0;
          }

          break;


     case 1:                       // fire

          switch (currweapon) {
          case 0:                  // fist
               if (currweaponframe == RFIST + 5)
                    if (currweaponanim == 5 && currweapontics == 10)
                         swingdasound(currweapon);
               break;
          case 1:                  // knife
               if (currweaponframe == KNIFEATTACK + 6
                   || currweaponframe == ZKNIFEATTACK + 5)
                    if (currweaponanim == 8 && currweapontics == 8)
                         swingdasound(currweapon);
               if (currweaponframe == KNIFEATTACK2 + 2
                   || currweaponframe == ZKNIFEATTACK2 + 2)
                    if ((currweaponanim == 5 || currweaponanim == 9) && currweapontics == 8)
                         swingdasound(currweapon);
               break;
          case 2:                  // shortsword
               if (currweaponframe == GOBSWORDATTACK + 4
                   || currweaponframe == ZSHORTATTACK + 7)
                    if (currweaponanim == 4 && currweapontics == 10)
                         swingdasound(currweapon);
               if (currweaponframe == GOBSWORDATTACK2 + 4
                   || currweaponframe == ZSHORTATTACK2 + 4)
                    if (currweaponanim == 4 && currweapontics == 10)
                         swingdasound(currweapon);
               break;
          case 3:                  // morning
               if (currweaponframe == MORNINGSTAR + 5
                   || currweaponframe == ZSTARATTACK + 7)
                    if (currweaponanim == 7 && currweapontics == 12)
                         swingdasound(currweapon);
               if (currweaponframe == MORNINGATTACK2 + 3
                   || currweaponframe == ZSTARATTACK2 + 3)
                    if (currweaponanim == 3 && currweapontics == 12)
                         swingdasound(currweapon);
               break;
          case 4:                  // sword
               if (currweaponframe == SWORDATTACK + 7)
                    if (currweaponanim == 7 && currweapontics == 8)
                         swingdasound(currweapon);
               if (currweaponframe == SWORDATTACK2 + 6)
                    if (currweaponanim == 6 && currweapontics == 8)
                         swingdasound(currweapon);
               break;
          case 5:                  // battleaxe
               if (currweaponframe == BIGAXEATTACK + 7
                   || currweaponframe == ZAXEATTACK + 12)
                    if (currweaponanim == 7 && currweapontics == 12)
                         swingdasound(currweapon);
               if (currweaponframe == BIGAXEATTACK2 + 6
                   || currweaponframe == ZAXEATTACK2 + 6)
                    if (currweaponanim == 6 && currweapontics == 12)
                         swingdasound(currweapon);
               break;
          case 6:                  // bow
               if (currweaponframe == BOWWALK + 4)
                    if (currweaponanim == 4 && currweapontics == 6)
                         swingdasound(currweapon);
               if (currweaponframe == ZBOWATTACK + 4)
                    if (currweaponanim == 4 && currweapontics == 6)
                         swingdasound(currweapon);
               break;
          case 7:                  // pike
               if (currweaponframe == PIKEATTACK1 + 4
                   || currweaponframe == ZPIKEATTACK + 4)
                    if (currweaponanim == 8 && currweapontics == 10)
                         swingdasound(currweapon);
               if (currweaponframe == PIKEATTACK2 + 4
                   || currweaponframe == ZPIKEATTACK2 + 4)
                    if (currweaponanim == 4 && currweapontics == 10)
                         swingdasound(currweapon);
               break;
          case 8:                  // two handed sword
               if (currweaponframe == EXCALATTACK1 + 7
                   || currweaponframe == ZTWOHANDATTACK + 12)
                    if (currweaponanim == 7 && currweapontics == 8)
                         swingdasound(currweapon);
               if (currweaponframe == EXCALATTACK2 + 5
                   || currweaponframe == ZTWOHANDATTACK2 + 5)
                    if (currweaponanim == 5 && currweapontics == 8)
                         swingdasound(currweapon);
               break;
          case 9:                  // halberd
               if (currweaponframe == HALBERDATTACK1 + 3
                   || currweaponframe == ZHALBERDATTACK + 4)
                    if (currweaponanim == 7 && currweapontics == 12)
                         swingdasound(currweapon);
               if (currweaponframe == HALBERDATTACK2 + 3
                   || currweaponframe == ZHALBERDATTACK2 + 3)
                    if (currweaponanim == 4 && currweapontics == 12)
                         swingdasound(currweapon);
               break;
          }

          switch (currweaponframe) {
          case RFIST + 5:
          case KNIFEATTACK + 6:
          case ZKNIFEATTACK + 5:   // new
          case MORNINGSTAR + 5:
          case SWORDATTACK + 7:
          case BOWWALK + 5:
          case ZBOWATTACK + 4:
          case KNIFEATTACK2 + 2:
          case ZKNIFEATTACK2 + 2:
          case SWORDATTACK2 + 6:
          case MORNINGATTACK2 + 3:
          case HALBERDATTACK1 + 3:
          case HALBERDATTACK2 + 3:
          case BIGAXEATTACK + 7:
          case BIGAXEATTACK2 + 6:
          case PIKEATTACK1 + 4:
          case PIKEATTACK2 + 4:
          case EXCALATTACK1 + 7:
          case EXCALATTACK2 + 5:
          case GOBSWORDATTACK2 + 4:
          case GOBSWORDATTACK + 4:
          case ZSHORTATTACK + 7:
          case ZSHORTATTACK2 + 4:
          case ZSTARATTACK + 7:
          case ZSTARATTACK2 + 3:
          case ZAXEATTACK + 12:
          case ZAXEATTACK2 + 6:
          case ZPIKEATTACK + 4:
          case ZPIKEATTACK2 + 4:
          case ZTWOHANDATTACK + 12:
          case ZTWOHANDATTACK2 + 5:
          case ZHALBERDATTACK + 4:
          case ZHALBERDATTACK2 + 3:
               swingdaweapon(plr);
               break;
          }

          currweapontics -= ((long) synctics);
          if (helmettime > 0)
               currweapontics--;

          if (shieldpoints <= 0)
               droptheshield = 1;

          if ((currweaponframe == SWORDATTACK + 7
               || currweaponframe == SWORDATTACK2 + 7)
              && currweapontics < 0 && droptheshield == 1) {

               if (rand() % 100 > 50) {
                    if (plr->ammo[1] > 0 && plr->weapon[3] == 0) {
                         currweapon = 1;
                         currweapontics = 6;
                         currweaponanim = -1;
                         currweaponfired = 6;
                         currweaponflip = 1;
                    }
                    if (plr->ammo[3] > 0) {
                         currweapon = 3;
                         currweapontics = 6;
                         currweaponanim = -1;
                         currweaponfired = 6;
                         currweaponflip = 1;
                    }
               }
          }
          if (currweapontics < 0) {
               currweaponanim++;
               if (currweaponanim > 11) {
                    // impact=0;
                    currweaponanim = 0;
                    currweaponfired = 0;
                    oldmousestatus = 0; // NEW
                    if (dahand > 0)
                         dahand = 0;
               }
               if (currweaponattackstyle == 0) {
                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                         currweapontics = weaponanimtics[currweapon][currweaponanim].daweapontics;
                         currweaponframe = weaponanimtics[currweapon][currweaponanim].daweaponframe;
                         dax = weaponanimtics[currweapon][currweaponanim].currx + 8;
                         day = weaponanimtics[currweapon][currweaponanim].curry;
                    }
                    else {
                         currweapontics = zweaponanimtics[currweapon][currweaponanim].daweapontics;
                         currweaponframe = zweaponanimtics[currweapon][currweaponanim].daweaponframe;
                         dax = zweaponanimtics[currweapon][currweaponanim].currx + 8;
                         day = zweaponanimtics[currweapon][currweaponanim].curry;
                    }
               }
               else {
                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                         currweapontics = weaponanimtics2[currweapon][currweaponanim].daweapontics;
                         currweaponframe = weaponanimtics2[currweapon][currweaponanim].daweaponframe;
                         dax = weaponanimtics2[currweapon][currweaponanim].currx + 8;
                         day = weaponanimtics2[currweapon][currweaponanim].curry;
                    }
                    else {
                         currweapontics = zweaponanimtics2[currweapon][currweaponanim].daweapontics;
                         currweaponframe = zweaponanimtics2[currweapon][currweaponanim].daweaponframe;
                         dax = zweaponanimtics2[currweapon][currweaponanim].currx + 8;
                         day = zweaponanimtics2[currweapon][currweaponanim].curry;
                    }
               }
          }
          else {
               if (currweaponattackstyle == 0) {
                    // flip
                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                         currweaponframe = weaponanimtics[currweapon][currweaponanim].daweaponframe;
                         dax = weaponanimtics[currweapon][currweaponanim].currx;
                         day = weaponanimtics[currweapon][currweaponanim].curry;
                    }
                    else {
                         currweaponframe = zweaponanimtics[currweapon][currweaponanim].daweaponframe;
                         dax = zweaponanimtics[currweapon][currweaponanim].currx;
                         day = zweaponanimtics[currweapon][currweaponanim].curry;
                    }
               }
               else {
                    // flip
                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                         currweaponframe = weaponanimtics2[currweapon][currweaponanim].daweaponframe;
                         dax = weaponanimtics2[currweapon][currweaponanim].currx;
                         day = weaponanimtics2[currweapon][currweaponanim].curry;
                    }
                    else {
                         currweaponframe = zweaponanimtics2[currweapon][currweaponanim].daweaponframe;
                         dax = zweaponanimtics2[currweapon][currweaponanim].currx;
                         day = zweaponanimtics2[currweapon][currweaponanim].curry;
                    }
               }

          }
          if (currweapon == 0 && currweaponframe != EMPTY) {
               if (dahand == 1)
                    overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);

               else if (dahand == 2) {
                    dax = lefthandanimtics[0][currweaponanim].currx;
                    day = lefthandanimtics[0][currweaponanim].curry + 8;
                    overwritesprite(dax, day + 5, currweaponframe + 6, dashade, dabits, dapalnum);
               }
          }
          else {
               if (currweaponframe != EMPTY) {
                    if (currweaponattackstyle == 0)
                         // flip
                         if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2)
                              dax = weaponanimtics[currweapon][currweaponanim].currx;
                         else
                              dax = zweaponanimtics[currweapon][currweaponanim].currx;
                    else
                         // flip
                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2)
                         dax = weaponanimtics2[currweapon][currweaponanim].currx;
                    else
                         dax = zweaponanimtics2[currweapon][currweaponanim].currx;

                    overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
               }
          }

          if (currweapon == 0 && currweaponframe == EMPTY) {
               dahand = 0;
               oldmousestatus = 0; // NEW
               currweaponanim = 0;
               currweaponfired = 0;
          }
          break;


     case 0:                       // walking

          if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2)
               currweapontics = weaponanimtics[currweapon][0].daweapontics;
          else
               currweapontics = zweaponanimtics[currweapon][0].daweapontics;

          if (currweapon == 6 && plr->ammo[6] <= 0) {
               // wango
               if (plr->weapon[currweapon] == 1)
                    currweaponframe = BOWREADYEND;
               else
                    currweaponframe = ZBOWWALK;
          }
          else {
               if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2)
                    currweaponframe = weaponanimtics[currweapon][0].daweaponframe;
               else
                    currweaponframe = zweaponanimtics[currweapon][0].daweaponframe;
          }

          if (vel != 0) {
               snakex = (sintable[(lockclock << 4) & 2047] >> 12);
               snakey = (sintable[(totalclock << 4) & 2047] >> 12);

               if (plr->screensize <= 320) {
                    /* if( currweaponframe == BOWREADYEND ) { if
                     * (plr->weapon[currweapon] == 1) {
                     * day=readyanimtics[currweapon][6].curry+snakey+16;
                     * dax=readyanimtics[currweapon][6].currx+snakex+16; } else
                     * { day=zreadyanimtics[currweapon][6].curry+snakey+16;
                     * dax=zreadyanimtics[currweapon][6].currx+snakex+16; } }
                     * else { if (plr->weapon[currweapon] == 1 ||
                     * plr->weapon[7] == 2) {
                     * day=weaponanimtics[currweapon][0].curry+snakey+16;
                     * dax=weaponanimtics[currweapon][0].currx+snakex+16; }
                     * else {
                     * day=zreadyanimtics[currweapon][6].curry+snakey+16;
                     * dax=zreadyanimtics[currweapon][6].currx+snakex+16; } } */
                    if (currweaponframe == BOWREADYEND) {
                         if (plr->weapon[currweapon] == 1) {
                              day = readyanimtics[currweapon][6].curry + snakey + 8;
                              dax = readyanimtics[currweapon][6].currx + snakex + 8;
                         }
                         else {
                              day = zreadyanimtics[currweapon][6].curry + snakey + 8;
                              dax = zreadyanimtics[currweapon][6].currx + snakex + 8;
                         }
                    }
                    else {
                         if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                              day = weaponanimtics[currweapon][0].curry + snakey + 8;
                              dax = weaponanimtics[currweapon][0].currx + snakex + 8;
                         }
                         else {
                              day = zweaponanimtics[currweapon][0].curry + snakey + 8;
                              dax = zweaponanimtics[currweapon][0].currx + snakex + 8;
                         }
                    }

               }
               else {
                    if (currweaponframe == BOWREADYEND) {
                         if (plr->weapon[currweapon] == 1) {
                              day = readyanimtics[currweapon][6].curry + snakey + 8;
                              dax = readyanimtics[currweapon][6].currx + snakex + 8;
                         }
                         else {
                              day = zreadyanimtics[currweapon][6].curry + snakey + 8;
                              dax = zreadyanimtics[currweapon][6].currx + snakex + 8;
                         }
                    }
                    else {
                         if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                              day = weaponanimtics[currweapon][0].curry + snakey + 8;
                              dax = weaponanimtics[currweapon][0].currx + snakex + 8;
                         }
                         else {
                              day = zweaponanimtics[currweapon][0].curry + snakey + 8;
                              dax = zweaponanimtics[currweapon][0].currx + snakex + 8;
                         }
                    }
               }
          }
          else {
               if (currweaponframe == BOWREADYEND) {
                    if (plr->weapon[currweapon] == 1) {
                         day = readyanimtics[currweapon][6].curry + 3;
                         dax = readyanimtics[currweapon][6].currx + 3;
                    }
                    else {
                         day = zreadyanimtics[currweapon][6].curry + 3;
                         dax = zreadyanimtics[currweapon][6].currx + 3;
                    }
               }
               else {
                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                         dax = weaponanimtics[currweapon][0].currx + 3;
                         day = weaponanimtics[currweapon][0].curry + 3;
                    }
                    else {
                         dax = zweaponanimtics[currweapon][0].currx + 3;
                         day = zweaponanimtics[currweapon][0].curry + 3;
                    }
               }
          }
          if (currweapon == 0 && currweaponframe != EMPTY) {
               overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
               overwritesprite(0, day + 8, currweaponframe + 6, dashade, dabits, dapalnum);
          }
          else {
               if (currweaponframe != EMPTY) {
                    overwritesprite(dax + snakex, day, currweaponframe, dashade, dabits, dapalnum);
               }
          }
          break;
     case 2:                       // unready
          if (currweapon == 1)
               weapondrop += synctics << 1;
          else
               weapondrop += synctics;
          if (weapondrop > weapondropgoal) {
               currweaponfired = 3;
               weaponraise = 40;
               currweapon = selectedgun;
               weaponuseless = 0;
               // hasshot=0;//just in case of bugg
               // make hasshot=1;
          }

          if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2)
               currweapontics = weaponanimtics[currweapon][0].daweapontics;
          else
               currweapontics = zweaponanimtics[currweapon][0].daweapontics;

          if (currweapon == 6 && plr->ammo[6] <= 0) {
               if (plr->weapon[currweapon] == 1)
                    currweaponframe = BOWREADYEND;
               else
                    // currweaponframe=ZBOWREADYEND;
                    currweaponframe = ZBOWWALK;
          }
          else {
               if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2)
                    currweaponframe = weaponanimtics[currweapon][0].daweaponframe;
               else
                    currweaponframe = zweaponanimtics[currweapon][0].daweaponframe;
          }

          if (currweaponframe == BOWREADYEND) {
               day = readyanimtics[currweapon][6].curry + (weapondrop);
               dax = readyanimtics[currweapon][6].currx;
          }
          // else if (currweaponframe == ZBOWREADYEND) {
          else if (currweaponframe == ZBOWWALK) {
               day = zreadyanimtics[currweapon][6].curry + (weapondrop);
               dax = zreadyanimtics[currweapon][6].currx;
          }
          else {
               if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                    dax = weaponanimtics[currweapon][0].currx;
                    day = weaponanimtics[currweapon][0].curry + (weapondrop);
               }
               else {
                    dax = zweaponanimtics[currweapon][0].currx;
                    day = zweaponanimtics[currweapon][0].curry + (weapondrop);
               }
          }

          if (currweapon == 0 && currweaponframe != EMPTY) {
               overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
               overwritesprite(0, day, currweaponframe + 6, dashade, dabits, dapalnum);
          }

          else {
               if (currweaponframe != EMPTY) {
                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2)
                         dax = weaponanimtics[currweapon][0].currx;
                    else
                         dax = weaponanimtics[currweapon][0].currx;

                    overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
               }
          }
          break;
     case 3:                       // ready
          currweapontics -= ((long) synctics);
          if (currweapontics < 0) {
               currweaponanim++;
               if (currweaponanim == 12) {
                    currweaponanim = 0;
                    currweaponfired = 0;

                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                         currweaponframe = readyanimtics[currweapon][11].daweaponframe;
                         dax = readyanimtics[currweapon][11].currx;
                         day = readyanimtics[currweapon][11].curry + 8;
                    }
                    else {
                         currweaponframe = zreadyanimtics[currweapon][11].daweaponframe;
                         dax = zreadyanimtics[currweapon][11].currx;
                         day = zreadyanimtics[currweapon][11].curry + 8;
                    }

                    if (currweapon == 0 && currweaponframe != EMPTY) {
                         overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
                         overwritesprite(0, day, currweaponframe + 6, dashade, dabits, dapalnum);
                    }
                    else {
                         if (currweaponframe != EMPTY) {
                              overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
                         }
                    }
                    break;
                    if (dahand > 0)
                         dahand = 0;
               }
               if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                    currweapontics = readyanimtics[currweapon][currweaponanim].daweapontics;
                    currweaponframe = readyanimtics[currweapon][currweaponanim].daweaponframe;
                    dax = readyanimtics[currweapon][currweaponanim].currx;
                    day = readyanimtics[currweapon][currweaponanim].curry + 8;
               }
               else {
                    currweapontics = zreadyanimtics[currweapon][currweaponanim].daweapontics;
                    currweaponframe = zreadyanimtics[currweapon][currweaponanim].daweaponframe;
                    dax = zreadyanimtics[currweapon][currweaponanim].currx;
                    day = zreadyanimtics[currweapon][currweaponanim].curry + 8;
               }
          }
          else {
               if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                    currweaponframe = readyanimtics[currweapon][currweaponanim].daweaponframe;
                    dax = readyanimtics[currweapon][currweaponanim].currx;
                    day = readyanimtics[currweapon][currweaponanim].curry + 8;
               }
               else {
                    currweaponframe = zreadyanimtics[currweapon][currweaponanim].daweaponframe;
                    dax = zreadyanimtics[currweapon][currweaponanim].currx;
                    day = zreadyanimtics[currweapon][currweaponanim].curry + 8;
               }

          }
          if (currweapon == 0 && currweaponframe != EMPTY) {
               overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
               overwritesprite(0, day, currweaponframe + 6, dashade, dabits, dapalnum);
          }
          else {
               if (currweaponframe != EMPTY) {
                    overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
               }
          }
          break;

     case 5:                       // cock
          currweapontics -= ((long) synctics);
          if (currweapontics < 0) {
               currweaponanim++;
               if (currweaponanim == 4) {
                    currweaponanim = 0;
                    currweaponfired = 0;

                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                         currweaponframe = cockanimtics[3].daweaponframe;
                         dax = cockanimtics[3].currx + 3;
                         day = cockanimtics[3].curry + 3;
                    }
                    else {
                         if (plr->weapon[currweapon] == 3) {
                              currweaponframe = zcockanimtics[4].daweaponframe;
                              dax = zcockanimtics[4].currx + 3;
                              day = zcockanimtics[4].curry + 3;
                         }
                         else {
                              currweaponframe = cockanimtics[4].daweaponframe;
                              dax = cockanimtics[4].currx + 3;
                              day = cockanimtics[4].curry + 3;
                         }
                    }

                    if (currweaponframe != EMPTY)
                         overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
                    break;
               }

               if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                    currweapontics = cockanimtics[currweaponanim].daweapontics;
                    currweaponframe = cockanimtics[currweaponanim].daweaponframe;
                    dax = cockanimtics[currweaponanim].currx;
                    day = cockanimtics[currweaponanim].curry + 8;
               }
               else {
                    if (plr->weapon[currweapon] == 3) {
                         currweapontics = zcockanimtics[currweaponanim].daweapontics;
                         currweaponframe = zcockanimtics[currweaponanim].daweaponframe;
                         dax = zcockanimtics[currweaponanim].currx;
                         day = zcockanimtics[currweaponanim].curry + 8;
                    }
                    else {
                         currweapontics = cockanimtics[currweaponanim].daweapontics;
                         currweaponframe = cockanimtics[currweaponanim].daweaponframe;
                         dax = cockanimtics[currweaponanim].currx;
                         day = cockanimtics[currweaponanim].curry + 8;
                    }
               }
          }
          else {
               if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                    currweaponframe = cockanimtics[currweaponanim].daweaponframe;
                    dax = cockanimtics[currweaponanim].currx;
                    day = cockanimtics[currweaponanim].curry + 8;
               }
               else {
                    if (plr->weapon[currweapon] == 3) {
                         currweaponframe = zcockanimtics[currweaponanim].daweaponframe;
                         dax = zcockanimtics[currweaponanim].currx;
                         day = zcockanimtics[currweaponanim].curry + 8;
                    }
                    else {
                         currweaponframe = zcockanimtics[currweaponanim].daweaponframe;
                         dax = zcockanimtics[currweaponanim].currx;
                         day = zcockanimtics[currweaponanim].curry + 8;
                    }
               }
          }
          if (currweaponframe != EMPTY)
               overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);

          break;

     case 4:                       // throw the orb

          if (currweaponframe == EMPTY || (currentorb == 0 || currentorb == 2
                                           || currentorb == 3
                                           || currentorb == 6
                                           || currentorb == 7)) {
               castaorb(plr);
               currweaponanim = 13;
               currweapontics = -1;
          }

          currweapontics -= ((long) synctics);
          if (currweapontics < 0) {
               currweaponanim++;
               if (currweaponanim > 12) {
                    currweaponanim = 0;
                    currweaponfired = 0;
                    orbshot = 0;
                    //
                    if (plr->weapon[currweapon] == 1 || plr->weapon[7] == 2) {
                         currweaponframe = throwanimtics[currentorb][currweaponanim].daweaponframe;
                         dax = throwanimtics[currentorb][currweaponanim].currx;
                         day = throwanimtics[currentorb][currweaponanim].curry + 8;
                    }
                    else {
                         currweaponframe = zthrowanimtics[currentorb][currweaponanim].daweaponframe;
                         dax = zthrowanimtics[currentorb][currweaponanim].currx;
                         day = zthrowanimtics[currentorb][currweaponanim].curry + 8;
                    }

                    if (currweapon == 0 && currweaponframe != EMPTY) {
                         overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
                    }
                    else {
                         if (currweaponframe != EMPTY) {
                              overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
                         }
                    }
                    //

                    break;
               }
               if (plr->weapon[currweapon] == 1) {
                    currweapontics = throwanimtics[currentorb][currweaponanim].daweapontics;
                    currweaponframe = throwanimtics[currentorb][currweaponanim].daweaponframe;
                    dax = throwanimtics[currentorb][currweaponanim].currx;
                    day = throwanimtics[currentorb][currweaponanim].curry + 8;
               }
               else {
                    currweapontics = zthrowanimtics[currentorb][currweaponanim].daweapontics;
                    currweaponframe = zthrowanimtics[currentorb][currweaponanim].daweaponframe;
                    dax = zthrowanimtics[currentorb][currweaponanim].currx;
                    day = zthrowanimtics[currentorb][currweaponanim].curry + 8;
               }
          }
          else {
               if (plr->weapon[currweapon] == 1) {
                    currweaponframe = throwanimtics[currentorb][currweaponanim].daweaponframe;
                    dax = throwanimtics[currentorb][currweaponanim].currx;
                    day = throwanimtics[currentorb][currweaponanim].curry + 8;
               }
               else {
                    currweaponframe = zthrowanimtics[currentorb][currweaponanim].daweaponframe;
                    dax = zthrowanimtics[currentorb][currweaponanim].currx;
                    day = zthrowanimtics[currentorb][currweaponanim].curry + 8;
               }
          }
          if (currweaponframe != EMPTY) {
               overwritesprite(dax, day, currweaponframe, dashade, dabits, dapalnum);
          }
          break;
     }

     if (shieldpoints > 0
         && (currweaponfired == 0
             || currweaponfired == 1)
         && selectedgun > 0
         && selectedgun < 5
         && droptheshield == 0) {

          if (currweaponfired == 1) {
               snakex = (sintable[(lockclock << 4) & 2047] >> 12);
               if (droptheshield == 1) {
                    dropshieldcnt += synctics << 1;
                    snakey = (sintable[(totalclock << 4) & 2047] >> 12) + dropshieldcnt;
               }
               else {
                    snakey = (sintable[(totalclock << 4) & 2047] >> 12);
               }
          }
          if (dropshieldcnt > 200) {
               dropshieldcnt = 0;
               droptheshield = 1;
          }

          if (shieldtype == 1) {
               if (shieldpoints > 75) {
                    overwritesprite(-40 + snakex, 100 + snakey, GRONSHIELD, dashade, dabits, dapalnum);
               }
               else if (shieldpoints > 50 && shieldpoints < 76) {
                    overwritesprite(-40 + snakex, 100 + snakey, GRONSHIELD + 1, dashade, dabits, dapalnum);
               }
               else if (shieldpoints > 25 && shieldpoints < 51) {
                    overwritesprite(-40 + snakex, 100 + snakey, GRONSHIELD + 2, dashade, dabits, dapalnum);
               }
               else {
                    overwritesprite(-40 + snakex, 100 + snakey, GRONSHIELD + 3, dashade, dabits, dapalnum);
               }
          }
          else {
               if (shieldpoints > 150) {
                    overwritesprite(-40 + snakex, 100 + snakey, ROUNDSHIELD, dashade, dabits, dapalnum);
               }
               else if (shieldpoints > 100 && shieldpoints < 151) {
                    overwritesprite(-40 + snakex, 100 + snakey, ROUNDSHIELD + 1, dashade, dabits, dapalnum);
               }
               else if (shieldpoints > 50 && shieldpoints < 101) {
                    overwritesprite(-40 + snakex, 100 + snakey, ROUNDSHIELD + 2, dashade, dabits, dapalnum);
               }
               else {
                    overwritesprite(-40 + snakex, 100 + snakey, ROUNDSHIELD + 3, dashade, dabits, dapalnum);
               }
          }

     }


     //
     // spellbook
     //

     if (spellbookflip == 1) {
          if (svga == 0) {
               if (plr->screensize <= 320)
                    itemtoscreen(122L, 155L, SPELLBOOKBACK, 0, 0);
          }
          if (svga == 1) {
               if (plr->screensize == 320)
                    itemtoscreen(120L << 1, 372L, SSPELLBACK, 0, 0);
          }

          spellbooktics -= synctics;
          if (spellbooktics < 0) {
               spellbook++;
               if (spellbook > 8)
                    spellbook = 8;
               if (spellbook == 8) {
                    spellbooktics = spellbookanim[currentorb][8].daweapontics;
                    if (svga == 1) {
                         spellbookframe = sspellbookanim[currentorb][8].daweaponframe;
                         dax = sspellbookanim[currentorb][8].currx;
                         day = sspellbookanim[currentorb][8].curry;
                         itemtoscreen(dax << 1, day, spellbookframe, 0, 0);
                    }
                    else {
                         spellbookframe = spellbookanim[currentorb][8].daweaponframe;
                         dax = spellbookanim[currentorb][8].currx;
                         day = spellbookanim[currentorb][8].curry;
                         itemtoscreen(dax, day, spellbookframe, 0, 0);
                    }
                    spellbookflip = 0;
                    return;
               }
               else {
                    spellbooktics = spellbookanim[currentorb][spellbook].daweapontics;

                    if (svga == 1) {
                         spellbookframe = sspellbookanim[currentorb][spellbook].daweaponframe;
                         dax = sspellbookanim[currentorb][spellbook].currx;
                         day = sspellbookanim[currentorb][spellbook].curry;
                         itemtoscreen(dax << 1, day, spellbookframe, 0, 0);
                    }
                    else {
                         spellbookframe = spellbookanim[currentorb][spellbook].daweaponframe;
                         dax = spellbookanim[currentorb][spellbook].currx;
                         day = spellbookanim[currentorb][spellbook].curry;
                         itemtoscreen(dax, day, spellbookframe, 0, 0);
                    }
               }
          }
          else {

               if (svga == 1) {
                    spellbookframe = sspellbookanim[currentorb][spellbook].daweaponframe;
                    dax = sspellbookanim[currentorb][spellbook].currx;
                    day = sspellbookanim[currentorb][spellbook].curry;
                    itemtoscreen(dax << 1, day, spellbookframe, 0, 0);
               }
               else {
                    spellbookframe = spellbookanim[currentorb][spellbook].daweaponframe;
                    dax = spellbookanim[currentorb][spellbook].currx;
                    day = spellbookanim[currentorb][spellbook].curry;
                    itemtoscreen(dax, day, spellbookframe, 0, 0);
               }

          }
     }


}

void 
castaorb(struct player * plr)
{

     int  i, k, n, s;
     short daang;
     spritetype *spr;

     switch (currentorb) {
     case 0:                       // SCARE
          playsound_loc(S_GENERALMAGIC4,plr->x,plr->y);
          shadowtime = ((plr->lvl + 1) * 120) << 4;
          break;
     case 1:                       // NIGHTVISION
          nightglowtime = 3600 + (plr->lvl * 120);
          break;
     case 2:                       // FREEZE
          playsound_loc(S_GENERALMAGIC3,plr->x,plr->y);
          daang = plr->ang;
          shootgun(plr, daang, 6);
          // playsound_loc(S_SPELL1,plr->x,plr->y);
          break;
     case 3:                       // MAGIC ARROW
          lockon(plr,10,2);
          playsound_loc(S_GENERALMAGIC2,plr->x,plr->y);
          break;
     case 4:                       // OPEN DOORS
          playsound_loc(S_DOORSPELL,plr->x,plr->y);
          daang = plr->ang;
          shootgun(plr, daang, 7);
          break;
     case 5:                       // FLY
          playsound_loc(S_GENERALMAGIC1,plr->x,plr->y);
          plr->orbactive[currentorb] = 3600 + (plr->lvl * 120);
          break;
     case 6:                       // FIREBALL
          lockon(plr,3,3);
          playsound_loc(S_FIRESPELL,plr->x,plr->y);
          break;
     case 7:                       // NUKE
          playsound_loc(S_NUKESPELL,plr->x,plr->y);
          daang = plr->ang;
          shootgun(plr, daang, 4);
          break;
     }

}

void
chunksofmeat(struct player * plr, short hitsprite, long hitx, long hity, long hitz, short hitsect, short daang)
{

     short j;
     short k;
     short zgore;
     int  chunk = REDCHUNKSTART;
     int  newchunk;

     if (goreon == 0)
          return;

     if (sprite[hitsprite].shade > 30)
          return;

     switch (selectedgun) {
     case 1:
     case 2:
          zgore = 1;
          break;
     case 3:
     case 4:
          zgore = 2;
          break;
     case 5:
          zgore = 3;
          break;
     case 6:
          zgore = 1;
          break;
     case 7:
          zgore = 2;
          break;
     case 8:
     case 9:
          zgore = 3;
          break;
     }
     if (sprite[hitsprite].statnum == NUKED) {
          zgore = 32;
     }
     if (sprite[hitsprite].picnum == RAT)
          zgore = 1;

     if (sprite[hitsprite].picnum == WILLOW
         || sprite[hitsprite].picnum == WILLOWEXPLO
         || sprite[hitsprite].picnum == WILLOWEXPLO + 1
         || sprite[hitsprite].picnum == WILLOWEXPLO + 2
         || sprite[hitsprite].picnum == GUARDIAN
         || sprite[hitsprite].picnum == GUARDIANATTACK
         || sprite[hitsprite].picnum == DEMON)
          return;

     if (sprite[hitsprite].picnum == SKELETON
         || sprite[hitsprite].picnum == SKELETONATTACK
         || sprite[hitsprite].picnum == SKELETONDIE) {
          playsound_loc(S_SKELHIT1 + (rand() % 2), sprite[hitsprite].x, sprite[hitsprite].y);
     }
     else {
          if (rand() % 100 > 60)
               playsound_loc(S_GORE1 + (rand() % 4), sprite[hitsprite].x, sprite[hitsprite].y);
     }

     if ((hitsprite >= 0) && (sprite[hitsprite].statnum < MAXSTATUS)) {
          for (k = 0; k < zgore; k++) {
               newchunk = 0;
               j = insertsprite(hitsect, CHUNKOMEAT);
               sprite[j].x = hitx;
               sprite[j].y = hity;
               sprite[j].z = hitz;
               sprite[j].cstat = 0;
               if (rand() % 100 > 50) {
                    switch (sprite[hitsprite].picnum) {
                    case GRONHAL:
                    case GRONHALATTACK:
                    case GRONHALPAIN:
                    case GRONMU:
                    case GRONMUATTACK:
                    case GRONMUPAIN:
                    case GRONSW:
                    case GRONSWATTACK:
                    case GRONSWPAIN:
                         chunk = REDCHUNKSTART + (rand() % 8);
                         break;

                    case KOBOLD:
                    case KOBOLDATTACK:
                         if (sprite[hitsprite].pal == 0)
                              chunk = BROWNCHUNKSTART + (rand() % 8);
                         if (sprite[hitsprite].pal == 4)
                              chunk = GREENCHUNKSTART + (rand() % 8);
                         if (sprite[hitsprite].pal == 7)
                              chunk = REDCHUNKSTART + (rand() % 8);
                         break;
                    case DEVILSTAND:
                    case DEVIL:
                    case DEVILATTACK:
                         chunk = REDCHUNKSTART + (rand() % 8);
                         break;
                    case IMP:
                    case IMPATTACK:
                    case IMPPAIN:
                         if (sprite[hitsprite].pal == 0)
                              chunk = GREENCHUNKSTART + (rand() % 8);
                         break;
                    case MINOTAUR:
                    case MINOTAURATTACK:
                    case MINOTAURPAIN:
                         chunk = TANCHUNKSTART + (rand() % 8);
                         break;
                    }
               }
               else {
                    newchunk = 1;
//                    chunk = NEWCHUNK + (rand() % 9);
                    chunk = REDCHUNKSTART + (rand() % 8);
               }
               if (sprite[hitsprite].picnum == SKELETON ||
                   sprite[hitsprite].picnum == SKELETONATTACK ||
                   sprite[hitsprite].picnum == SKELETONDIE) {
                    chunk = BONECHUNK1 + (rand() % 9);
               }
               if (plr->weapon[2] == 3 && currweapon == 2) {
                    sprite[j].picnum = ARROWFLAME;
               }
               else {
                    sprite[j].picnum = chunk; // = REDCHUNKSTART + (rand() % 8);
               }

               sprite[j].shade = -16;
               sprite[j].xrepeat = 64;
               sprite[j].yrepeat = 64;
               sprite[j].clipdist = 16;
               sprite[j].ang = ((rand() & 1023) - 1024) & 2047;
               sprite[j].xvel = ((rand() & 1023) - 512);
               sprite[j].yvel = ((rand() & 1023) - 512);
               sprite[j].zvel = ((rand() & 1023) - 512);
               if (newchunk == 1)
                    sprite[j].zvel <<= 1;
               sprite[j].owner = 4096;
               sprite[j].lotag = 512;
               sprite[j].hitag = 0;
               sprite[j].pal = 0;
               movesprite((short) j, (((long) sintable[(sprite[j].ang + 512) & 2047]) * synctics) << 3, (((long) sintable[sprite[j].ang]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 0);
          }
     }

}

void
swingdapunch(int daweapon)
{

     switch (daweapon) {
 case 0:                          // hands
//JSA SPOOGE
          SND_Sound(S_SOCK4);
          SND_Sound(S_PLRPAIN1 + (rand() % 2));
          healthpic(-1);
          startredflash(10);
          break;
     case 1:                       // knife
     case 2:                       // mace
     case 4:                       // sword
          SND_PlaySound(S_WALLHIT1, 0, 0, 0, 0);
          break;
     case 3:                       // arrow
          break;
     case 5:
     case 6:
     case 7:
     case 8:
          SND_PlaySound(S_WALLHIT1, 0, 0, 0, 0);
          break;

     }


}

void
shootgun(struct player * plr, short daang, char guntype)
{

     short hitsect,
          hitwall,
          hitsprite,
          daang2,
          daang3,
          k;
     long i,
          j,
          daz2,
          hitx,
          hity,
          hitz;
     int  chunk;
     long hit,
          dax,
          day;
     int  bolts;
     int  cnt;

     switch (guntype) {
     case 0:
          daang2 = daang + 2048;
          daz2 = (100 - plr->horiz) * 2000;
          hitscan(plr->x, plr->y, plr->z, plr->sector, // Start position
                  sintable[(daang2 + 2560) & 2047],    // X vector of 3D ang
                  sintable[(daang2 + 2048) & 2047],    // Y vector of 3D ang
                  daz2,            // Z vector of 3D ang
                  &hitsect, &hitwall, &hitsprite, &hitx, &hity, &hitz);

          if (hitsprite >= 0)
               madeahit = 1;

          if (hitwall >= 0) {
               if ((labs(plr->x - hitx) + labs(plr->y - hity) < 512) && (labs((plr->z >> 8) - ((hitz >> 8) - (64))) <= (512 >> 3))) {
                    madeahit = 1;
                    switch (currweapon) {
                    case 0:        // fist
                         if (currweaponframe == RFIST + 5)
                              if (currweaponanim == 5 && currweapontics == 10)
                                   swingdapunch(currweapon);
                         break;
                    case 1:        // knife
                         if (currweaponframe == KNIFEATTACK + 6)
                              if (currweaponanim == 8 && currweapontics == 8)
                                   swingdapunch(currweapon);
                         if (currweaponframe == KNIFEATTACK2 + 2)
                              if (currweaponanim == 5 || currweaponanim == 9 && currweapontics == 8)
                                   swingdapunch(currweapon);
                         break;
                    case 2:        // short sword
                         if (currweaponframe == GOBSWORDATTACK + 4
                             || currweaponframe == ZSHORTATTACK + 7)
                              if (currweaponanim == 4 && currweapontics == 10)
                                   swingdapunch(currweapon);
                         if (currweaponframe == GOBSWORDATTACK + 4
                             || currweaponframe == ZSHORTATTACK + 4)
                              if (currweaponanim == 4 && currweapontics == 10)
                                   swingdapunch(currweapon);
                         break;
                    case 3:        // morning
                         if (currweaponframe == MORNINGSTAR + 5
                             || currweaponframe == ZSTARATTACK + 7)
                              if (currweaponanim == 7 && currweapontics == 12)
                                   swingdapunch(currweapon);
                         if (currweaponframe == MORNINGATTACK2 + 3
                             || currweaponframe == ZSTARATTACK + 3)
                              if (currweaponanim == 3 && currweapontics == 12)
                                   swingdapunch(currweapon);

                         if (plr->weapon[currweapon] == 3) {
                              explosion(hitsprite, hitx, hity, hitz, 4096);
                         }
                         break;
                    case 4:        // sword
                         if (currweaponframe == SWORDATTACK + 7)
                              if (currweaponanim == 7 && currweapontics == 8) {
                                   swingdapunch(currweapon);
                                   madenoise(2, plr->x, plr->y, plr->z);
                              }
                         if (currweaponframe == SWORDATTACK2 + 6)
                              if (currweaponanim == 6 && currweapontics == 8) {
                                   swingdapunch(currweapon);
                                   madenoise(2, plr->x, plr->y, plr->z);
                              }
                         break;
                    case 5:        // battleaxe
                         if (currweaponframe == BIGAXEATTACK + 7
                             || currweaponframe == ZAXEATTACK + 12)
                              if (currweaponanim == 7 && currweapontics == 12)
                                   swingdapunch(currweapon);
                         if (currweaponframe == BIGAXEATTACK2 + 6
                             || currweaponframe == ZAXEATTACK2 + 6)
                              if (currweaponanim == 6 && currweapontics == 12)
                                   swingdapunch(currweapon);
                         break;
                    case 6:        // bow
                         if (currweaponframe == BOWWALK + 4)
                              if (currweaponanim == 4 && currweapontics == 6)
                                   swingdapunch(currweapon);
                         if (currweaponframe == ZBOWATTACK + 4)
                              if (currweaponanim == 4 && currweapontics == 6)
                                   swingdapunch(currweapon);
                         break;
                    case 7:        // pike
                         if (currweaponframe == PIKEATTACK1 + 4
                             || currweaponframe == ZPIKEATTACK + 4)
                              if (currweaponanim == 8 && currweapontics == 10)
                                   swingdapunch(currweapon);
                         if (currweaponframe == PIKEATTACK2 + 4
                             || currweaponframe == ZPIKEATTACK2 + 4)
                              if (currweaponanim == 4 && currweapontics == 10)
                                   swingdapunch(currweapon);
                         break;
                    case 8:        // two handed sword
                         if (currweaponframe == EXCALATTACK1 + 7
                             || currweaponframe == ZTWOHANDATTACK + 12)
                              if (currweaponanim == 7 && currweapontics == 8)
                                   swingdapunch(currweapon);
                         if (currweaponframe == EXCALATTACK2 + 5
                             || currweaponframe == ZTWOHANDATTACK2 + 5)
                              if (currweaponanim == 5 && currweapontics == 8)
                                   swingdapunch(currweapon);
                         break;
                    case 9:        // halberd
                         if (currweaponframe == HALBERDATTACK1 + 3
                             || currweaponframe == ZHALBERDATTACK + 4)
                              if (currweaponanim == 6 && currweapontics == 12)
                                   swingdapunch(currweapon);
                         if (currweaponframe == HALBERDATTACK2 + 3
                             || currweaponframe == ZHALBERDATTACK2 + 3)
                              if (currweaponanim == 4 && currweapontics == 12)
                                   swingdapunch(currweapon);
                         break;
                    }
               }
          }
          if (checkweapondist(hitsprite, plr->x, plr->y, plr->z, guntype)) {
               switch (sprite[hitsprite].picnum) {
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
                    newstatus(hitsprite, BROKENVASE);
                    break;

               case GRONHAL:
               case GRONHALATTACK:
               case GRONHALPAIN:
               case GRONMU:
               case GRONMUATTACK:
               case GRONMUPAIN:
               case GRONSW:
               case GRONSWATTACK:
               case GRONSWPAIN:
               case KOBOLD:
               case KOBOLDATTACK:
               case DEVILSTAND:
               case DEVIL:
               case DEVILATTACK:
               case SKELETON:
               case SKELETONATTACK:
               case IMP:
               case IMPATTACK:
                    // case IMPPAIN:
               case MINOTAUR:
               case MINOTAURATTACK:
               case MINOTAURPAIN:
               case FISH:
               case RAT:
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
                    // case GONZOCSWPAIN:

               case GONZOGSW:
               case GONZOGSWAT:
                    // case GONZOGSWPAIN:

               case GONZOGHM:
               case GONZOGHMAT:
                    // case GONZOGHMPAIN:

               case GONZOGSH:
               case GONZOGSHAT:
                    // case GONZOGSHPAIN:

               case KATIE:
               case KATIEAT:
                    if (netgame) {
                         netshootgun(hitsprite, currweapon);
                    }

                    if (currweapon == 3)
                         if (plr->weapon[currweapon] == 3) {
                              explosion(hitsprite, hitx, hity, hitz, 4096);
                         }


                    if (invisibletime > 0)
                         if (krand() & 32 > 15)
                              invisibletime = -1;
                    switch (selectedgun) {
                    case 0:        // fist
                         k = (krand() % 5) + 1;
                         break;
                    case 1:        // dagger
                         k = (krand() % 5) + 5;
                         break;
                    case 2:        // short sword
                         k = (krand() % 10) + 5;
                         break;
                    case 3:        // morning star
                         k = (krand() % 15) + 5;
                         break;
                    case 4:        // broad sword
                         k = (krand() % 20) + 5;
                         break;
                    case 5:        // battle axe
                         k = (krand() % 25) + 5;
                         switch (sprite[hitsprite].picnum) {
                         case GRONHAL:
                         case GRONHALATTACK:
                         case GRONHALPAIN:
                         case GRONMU:
                         case GRONMUATTACK:
                         case GRONMUPAIN:
                         case GRONSW:
                         case GRONSWATTACK:
                         case GRONSWPAIN:

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
                         case GONZOGSW:
                         case GONZOGSWAT:
                         case GONZOGHM:
                         case GONZOGHMAT:
                         case GONZOGSH:
                         case GONZOGSHAT:
                              k += k >> 1;
                         }
                         break;
                    case 6:        // bow
                         k = (krand() % 30) + 5;
                         break;
                    case 7:        // pike axe
                         k = (krand() % 35) + 5;
                         break;
                    case 8:        // two handed sword
                         k = (krand() % 40) + 5;
                         break;
                    case 9:        // halberd
                         k = (krand() % 45) + 5;
                         break;
                    }
                    k += plr->lvl;
                    if (plr->weapon[currweapon] == 3) {
                         k <<= 1;
                    }
                    if (vampiretime > 0) {
                         if (plr->health <= plr->maxhealth)
                              healthpic((rand() % 10) + 1);
                    }
                    if (helmettime > 0)
                         k <<= 1;
                    if (strongtime > 0) {
                         k += k >> 1;

                         switch (currweapon) {
                         case 0:   // fist
                              if (currweaponframe == RFIST + 5)
                                   if (currweaponanim == 5 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              break;
                         case 1:   // knife
                              if (currweaponframe == KNIFEATTACK + 6)
                                   if (currweaponanim == 8 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == KNIFEATTACK2 + 2)
                                   if (currweaponanim == 5 || currweaponanim == 9 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              break;
                         case 2:   // short sword
                              if (currweaponframe == GOBSWORDATTACK + 4
                                  || currweaponframe == ZSHORTATTACK + 7)
                                   if (currweaponanim == 4 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == GOBSWORDATTACK2 + 4
                                  || currweaponframe == ZSHORTATTACK2 + 4)
                                   if (currweaponanim == 4 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              break;
                         case 3:   // morning
                              if (currweaponframe == MORNINGSTAR + 5
                                  || currweaponframe == ZSTARATTACK + 7)
                                   if (currweaponanim == 7 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == MORNINGATTACK2 + 3
                                  || currweaponframe == ZSTARATTACK2 + 3)
                                   if (currweaponanim == 3 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              break;
                         case 4:   // sword
                              if (currweaponframe == SWORDATTACK + 7)
                                   if (currweaponanim == 7 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == SWORDATTACK2 + 6)
                                   if (currweaponanim == 6 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              break;
                         case 5:   // battleaxe
                              if (currweaponframe == BIGAXEATTACK + 7
                                  || currweaponframe == ZAXEATTACK + 12)
                                   if (currweaponanim == 7 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == BIGAXEATTACK2 + 6
                                  || currweaponframe == ZAXEATTACK2 + 6)
                                   if (currweaponanim == 6 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              break;
                         case 6:   // bow
                              if (currweaponframe == BOWWALK + 4)
                                   if (currweaponanim == 4 && currweapontics == 6)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == ZBOWATTACK + 4)
                                   if (currweaponanim == 4 && currweapontics == 6)
                                        swingdacrunch(currweapon);
                              break;
                         case 7:   // pike
                              if (currweaponframe == PIKEATTACK1 + 4
                                  || currweaponframe == ZPIKEATTACK + 4)
                                   if (currweaponanim == 8 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == PIKEATTACK2 + 4
                                  || currweaponframe == ZPIKEATTACK2 + 4)
                                   if (currweaponanim == 4 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              break;
                         case 8:   // two handed sword
                              if (currweaponframe == EXCALATTACK1 + 7
                                  || currweaponframe == ZTWOHANDATTACK + 12)
                                   if (currweaponanim == 7 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == EXCALATTACK2 + 5
                                  || currweaponframe == ZTWOHANDATTACK2 + 5)
                                   if (currweaponanim == 5 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              break;
                         case 9:   // halberd
                              if (currweaponframe == HALBERDATTACK1 + 3
                                  || currweaponframe == ZHALBERDATTACK + 4)
                                   if (currweaponanim == 6 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == HALBERDATTACK2 + 3
                                  || currweaponframe == ZHALBERDATTACK2 + 3)
                                   if (currweaponanim == 4 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              break;
                         }
                         sprite[hitsprite].hitag -= (k << 1);
                         // freeze sword
#if 0
                         if (plr->weapon[currweapon] == 3 && currweapon == 4) {
                              if (sprite[hitsprite].picnum == KATIE
                                  || sprite[hitsprite].picnum == KATIEPAIN
                                  || sprite[hitsprite].picnum == KATIEAT
                                  || sprite[hitsprite].shade > 30) {
                                   // do nothing
                                   // a laughing sound
                              }
                              else {
                                   turntostone(hitsprite);
                                   break;
                              }
                         }
#endif
                         if (plr->weapon[currweapon] == 3
                             && currweapon == 8
                             && sprite[hitsprite].pal != 6) {
                              if (sprite[hitsprite].hitag <= 0) {
                                   sprite[hitsprite].hitag = 1;
                              }
                              if (rand() % 100 > 50)
                                   medusa(hitsprite);
                              break;
                         }
                         else if (currweapon != 0) {
//JSA GORE1 you have strong time
                              if (rand() % 100 > 50) {
                                   if (sprite[hitsprite].picnum == SKELETON
                                       || sprite[hitsprite].picnum == SKELETONATTACK
                                   || sprite[hitsprite].picnum == SKELETONDIE)
                                        playsound_loc(S_SKELHIT1 + (rand() % 2), sprite[hitsprite].x, sprite[hitsprite].y);
                              }

                              // HERE
                              switch (currweapon) {
                              case 0:   // fist
                                   break;
                              case 1:   // knife
                                   if (currweaponframe == KNIFEATTACK + 6)
                                        if (currweaponanim == 8 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == KNIFEATTACK2 + 2)
                                        if (currweaponanim == 5 || currweaponanim == 9 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 2:   // short sword
                                   if (currweaponframe == GOBSWORDATTACK + 4
                                       || currweaponframe == ZSHORTATTACK + 7)
                                        if (currweaponanim == 4 && currweapontics == 10)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == GOBSWORDATTACK2 + 4
                                       || currweaponframe == ZSHORTATTACK + 4)
                                        if (currweaponanim == 4 && currweapontics == 10)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 3:   // morning
                                   if (currweaponframe == MORNINGSTAR + 5
                                       || currweaponframe == ZSTARATTACK + 7)
                                        if (currweaponanim == 7 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == MORNINGATTACK2 + 3
                                       || currweaponframe == ZSTARATTACK2 + 3)
                                        if (currweaponanim == 3 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 4:   // sword
                                   if (currweaponframe == SWORDATTACK + 7)
                                        if (currweaponanim == 7 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == SWORDATTACK2 + 6)
                                        if (currweaponanim == 6 && currweapontics == 8)
                                             break;
                                   chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                              case 5:   // battleaxe
                                   if (currweaponframe == BIGAXEATTACK + 7
                                       || currweaponframe == ZAXEATTACK + 12)
                                        if (currweaponanim == 7 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == BIGAXEATTACK2 + 6
                                       || currweaponframe == ZAXEATTACK2 + 6)
                                        if (currweaponanim == 6 && currweapontics == 12)
                                             break;
                                   chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                              case 6:   // bow
                                   if (currweaponframe == BOWWALK + 4)
                                        if (currweaponanim == 4 && currweapontics == 6)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == ZBOWATTACK + 4)
                                        if (currweaponanim == 4 && currweapontics == 6)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 7:   // pike
                                   if (currweaponframe == PIKEATTACK1 + 4
                                       || currweaponframe == ZPIKEATTACK + 4)
                                        if (currweaponanim == 8 && currweapontics == 10)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == PIKEATTACK2 + 4
                                       || currweaponframe == ZPIKEATTACK2 + 4)
                                        if (currweaponanim == 4 && currweapontics == 10)
                                             break;
                                   chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                              case 8:   // two handed sword
                                   if (currweaponframe == EXCALATTACK1 + 7
                                    || currweaponframe == ZTWOHANDATTACK + 12)
                                        if (currweaponanim == 7 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == EXCALATTACK2 + 5
                                    || currweaponframe == ZTWOHANDATTACK2 + 5)
                                        if (currweaponanim == 5 && currweapontics == 8)
                                             break;
                                   chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                              case 9:   // halberd
                                   if (currweaponframe == HALBERDATTACK1 + 3
                                     || currweaponframe == ZHALBERDATTACK + 4)
                                        if (currweaponanim == 6 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == HALBERDATTACK2 + 3
                                    || currweaponframe == ZHALBERDATTACK2 + 3)
                                        if (currweaponanim == 4 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              }
                              // ENDOFHERE
                              // chunksofmeat(plr, hitsprite, hitx, hity, hitz,
                              // hitsect, daang);
                         }
                    }
                    else {
                         switch (currweapon) {
                         case 0:   // fist
                              if (currweaponframe == RFIST + 5)
                                   if (currweaponanim == 5 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              break;
                         case 1:   // knife
                              if (currweaponframe == KNIFEATTACK + 6)
                                   if (currweaponanim == 8 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == KNIFEATTACK2 + 2)
                                   if (currweaponanim == 5 || currweaponanim == 9 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              break;
                         case 2:   // SHORT SWORD
                              if (currweaponframe == GOBSWORDATTACK + 4
                                  || currweaponframe == ZSHORTATTACK + 7)
                                   if (currweaponanim == 4 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == GOBSWORDATTACK2 + 4
                                  || currweaponframe == ZSHORTATTACK2 + 4)
                                   if (currweaponanim == 4 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              break;
                         case 3:   // morning
                              if (currweaponframe == MORNINGSTAR + 5
                                  || currweaponframe == ZSTARATTACK + 7)
                                   if (currweaponanim == 7 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == MORNINGATTACK2 + 3
                                  || currweaponframe == ZSTARATTACK2 + 3)
                                   if (currweaponanim == 3 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              break;
                         case 4:   // sword
                              if (currweaponframe == SWORDATTACK + 7)
                                   if (currweaponanim == 7 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == SWORDATTACK2 + 6)
                                   if (currweaponanim == 6 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              break;
                         case 5:   // battleaxe
                              if (currweaponframe == BIGAXEATTACK + 7
                                  || currweaponframe == ZAXEATTACK + 12)
                                   if (currweaponanim == 7 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == BIGAXEATTACK2 + 6
                                  || currweaponframe == ZAXEATTACK2 + 6)
                                   if (currweaponanim == 6 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              break;
                         case 6:   // bow
                              if (currweaponframe == BOWWALK + 4)
                                   if (currweaponanim == 4 && currweapontics == 6)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == ZBOWATTACK + 4)
                                   if (currweaponanim == 4 && currweapontics == 6)
                                        swingdacrunch(currweapon);
                              break;

                         case 7:   // pike
                              if (currweaponframe == PIKEATTACK1 + 4
                                  || currweaponframe == ZPIKEATTACK + 4)
                                   if (currweaponanim == 8 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == PIKEATTACK2 + 4
                                  || currweaponframe == ZPIKEATTACK2 + 4)
                                   if (currweaponanim == 4 && currweapontics == 10)
                                        swingdacrunch(currweapon);
                              break;
                         case 8:   // two handed sword
                              if (currweaponframe == EXCALATTACK1 + 7
                                  || currweaponframe == ZTWOHANDATTACK + 12)
                                   if (currweaponanim == 7 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == EXCALATTACK2 + 5
                                  || currweaponframe == ZTWOHANDATTACK2 + 5)
                                   if (currweaponanim == 5 && currweapontics == 8)
                                        swingdacrunch(currweapon);
                              break;
                         case 9:   // halberd
                              if (currweaponframe == HALBERDATTACK1 + 3
                                  || currweaponframe == ZHALBERDATTACK + 4)
                                   if (currweaponanim == 6 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              if (currweaponframe == HALBERDATTACK2 + 3
                                  || currweaponframe == ZHALBERDATTACK2 + 3)
                                   if (currweaponanim == 4 && currweapontics == 12)
                                        swingdacrunch(currweapon);
                              break;
                         }
                         sprite[hitsprite].hitag -= k;
                         // freeze sword
#if 0
                         if (plr->weapon[currweapon] == 3 && currweapon == 4) {
                              if (sprite[hitsprite].picnum == KATIE
                                  || sprite[hitsprite].picnum == KATIEPAIN
                                  || sprite[hitsprite].picnum == KATIEAT
                                  || sprite[hitsprite].shade > 30) {
                                   // do nothing
                                   // a laughing sound
                              }
                              else {
                                   turntostone(hitsprite);
                                   break;
                              }
                         }
#endif
                         if (plr->weapon[currweapon] == 3
                             && currweapon == 8
                             && sprite[hitsprite].pal != 6) {
                              if (sprite[hitsprite].hitag <= 0) {
                                   sprite[hitsprite].hitag = 1;
                              }
                              if (rand() % 100 > 75)
                                   medusa(hitsprite);
                              break;
                         }

                         if (currweapon != 0) {
//JSA GORE normal
                              if (rand() % 100 > 50) {
                                   if (sprite[hitsprite].picnum == SKELETON
                                       || sprite[hitsprite].picnum == SKELETONATTACK
                                   || sprite[hitsprite].picnum == SKELETONDIE)
                                        playsound_loc(S_SKELHIT1 + (rand() % 2), sprite[hitsprite].x, sprite[hitsprite].y);
                              }
                              // HERE
                              switch (currweapon) {
                              case 0:   // fist
                                   break;
                              case 1:   // knife
                                   if (currweaponframe == KNIFEATTACK + 6)
                                        if (currweaponanim == 8 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == KNIFEATTACK2 + 2)
                                        if (currweaponanim == 5 || currweaponanim == 9 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 2:   // short sword
                                   if (currweaponframe == GOBSWORDATTACK + 4
                                       || currweaponframe == ZSHORTATTACK + 7)
                                        if (currweaponanim == 4 && currweapontics == 10)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == GOBSWORDATTACK2 + 4
                                       || currweaponframe == ZSHORTATTACK2 + 4)
                                        if (currweaponanim == 4 && currweapontics == 10)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 3:   // morning
                                   if (currweaponframe == MORNINGSTAR + 5
                                       || currweaponframe == ZSTARATTACK + 7)
                                        if (currweaponanim == 7 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == MORNINGATTACK2 + 3
                                       || currweaponframe == ZSTARATTACK2 + 3)
                                        if (currweaponanim == 3 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 4:   // sword
                                   if (currweaponframe == SWORDATTACK + 7)
                                        if (currweaponanim == 7 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == SWORDATTACK2 + 6)
                                        if (currweaponanim == 6 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 5:   // battleaxe
                                   if (currweaponframe == BIGAXEATTACK + 7
                                       || currweaponframe == ZAXEATTACK + 12)
                                        if (currweaponanim == 7 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == BIGAXEATTACK2 + 6
                                       || currweaponframe == ZAXEATTACK2 + 6)
                                        if (currweaponanim == 6 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 6:   // bow
                                   if (currweaponframe == BOWWALK + 4)
                                        if (currweaponanim == 4 && currweapontics == 6)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == ZBOWATTACK + 4)
                                        if (currweaponanim == 4 && currweapontics == 6)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 7:   // pike
                                   if (currweaponframe == PIKEATTACK1 + 4
                                       || currweaponframe == ZPIKEATTACK + 4)
                                        if (currweaponanim == 8 && currweapontics == 10)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == PIKEATTACK2 + 4
                                       || currweaponframe == ZPIKEATTACK2 + 4)
                                        if (currweaponanim == 4 && currweapontics == 10)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 8:   // two handed sword
                                   if (currweaponframe == EXCALATTACK1 + 7
                                    || currweaponframe == ZTWOHANDATTACK + 12)
                                        if (currweaponanim == 7 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == EXCALATTACK2 + 5
                                    || currweaponframe == ZTWOHANDATTACK2 + 5)
                                        if (currweaponanim == 5 && currweapontics == 8)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              case 9:   // halberd
                                   if (currweaponframe == HALBERDATTACK1 + 3
                                     || currweaponframe == ZHALBERDATTACK + 4)
                                        if (currweaponanim == 6 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   if (currweaponframe == HALBERDATTACK2 + 3
                                    || currweaponframe == ZHALBERDATTACK2 + 3)
                                        if (currweaponanim == 4 && currweapontics == 12)
                                             chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                                   break;
                              }
                              // ENDOFHERE

                              // chunksomeat(plr, hitsprite, hitx, hity, hitz,
                              // hitsect, daang);
                         }
                    }

                    if (netgame) {
                         break;
                    }

                    if (sprite[hitsprite].hitag <= 0) {
                         if (selectedgun > 1) {
//JSA GORE on death ?
//RAF ans:death
                              chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                              if (sprite[hitsprite].picnum == SKELETON
                                  || sprite[hitsprite].picnum == SKELETONATTACK
                                  || sprite[hitsprite].picnum == SKELETONDIE)
                                   playsound_loc(S_SKELHIT1 + (rand() % 2), sprite[hitsprite].x, sprite[hitsprite].y);
                         }
                         newstatus(hitsprite, DIE);
                    }
                    sprite[hitsprite].ang = plr->ang + ((krand() & 32) - 64);
                    if (sprite[hitsprite].hitag > 0)
                         newstatus(hitsprite, PAIN);
                    break;
               case GONZOBSHPAIN:

               case GONZOCSWPAIN:

               case GONZOGSWPAIN:
               case GONZOGHMPAIN:
               case GONZOGSHPAIN:

               case GRONHALDIE:
               case GRONMUDIE:
               case GRONSWDIE:
               case KOBOLDDIE:
               case DEVILDIE:
               case SKELETONDIE:
               case IMPDIE:
               case MINOTAURDIE:

               case NEWGUYPAIN:
               case KATIEPAIN:
                    if (sprite[hitsprite].pal == 6) {
                         // JSA_NEW
                         SND_PlaySound(S_SOCK1 + (rand() % 4), 0, 0, 0, 0);
                         playsound_loc(S_FREEZEDIE, hitx, hity);
                         for (k = 0; k < 16; k++) {
                              icecubes(hitsprite, hitx, hity, hitz, hitsprite);
                         }
                         deletesprite((short) hitsprite);
                    }
               }                   // switch
          }                        // if weapondist
          if (madeahit == 0) {
               plr->ammo[currweapon]++;
               madeahit = 1;
          }
          break;
     case 1:

          daang2 = (daang + 2048) & 2047;
          daz2 = (100 - plr->horiz) * 2000;
          hitscan(plr->x, plr->y, plr->z, plr->sector, // Start position
                  sintable[(daang2 + 2560) & 2047],    // X vector of 3D ang
                  sintable[(daang2 + 2048) & 2047],    // Y vector of 3D ang
                  daz2,            // Z vector of 3D ang
                  &hitsect, &hitwall, &hitsprite, &hitx, &hity, &hitz);


          if (hitwall >= 0) {

               arrowcnt=(arrowcnt+1)%ARROWCOUNTLIMIT;
               if (arrowsprite[arrowcnt] != -1) {
                    deletesprite((short) arrowsprite[arrowcnt]);
                    arrowsprite[arrowcnt]=-1;
               }

               neartag(hitx, hity, hitz, hitsect, daang,
                       &neartagsector,
                       &neartagwall,
                       &neartagsprite,
                       &neartaghitdist, 1024, 3);


               if (sector[neartagsector].lotag < 6 ||
                   sector[neartagsector].lotag > 8) {

                    j = insertsprite(hitsect, 0);
                    arrowsprite[arrowcnt]=j;
                    sprite[j].x = hitx;
                    sprite[j].y = hity;
                    sprite[j].z = hitz + (8 << 8);
                    sprite[j].cstat = 17;    // was16
                    sprite[j].picnum = WALLARROW;
                    sprite[j].shade = 0;
                    sprite[j].pal = 0;
                    sprite[j].xrepeat = 16;
                    sprite[j].yrepeat = 48;
                    sprite[j].ang = ((daang + 2048) - 512 + (rand() & 128 - 64)) & 2047;
                    sprite[j].xvel = 0;
                    sprite[j].yvel = 0;
                    sprite[j].zvel = 0;
                    sprite[j].owner = 4096;
                    sprite[j].lotag = 32;
                    sprite[j].hitag = 0;
                    playsound_loc(S_ARROWHIT, sprite[j].x, sprite[j].y);

                    if (plr->weapon[6] == 3 && currweapon == 6) {
                         j = insertsprite(hitsect, FIRECHUNK);
                         sprite[j].x = hitx;
                         sprite[j].y = hity;
                         sprite[j].z = hitz + (14 << 8);
                         sprite[j].cstat = 0;
                         sprite[j].picnum = ARROWFLAME;
                         sprite[j].shade = 0;
                         sprite[j].pal = 0;
                         sprite[j].xrepeat = 64;
                         sprite[j].yrepeat = 64;
                         sprite[j].ang = 0;
                         sprite[j].xvel = 0;
                         sprite[j].yvel = 0;
                         sprite[j].zvel = 0;
                         sprite[j].owner = 0;
                         sprite[j].lotag = 1200;
                         sprite[j].hitag = 0;
                    }

               }



               if (netgame) {
                    netshootgun(-1, 5);
               }

          }


          if ((hitsprite >= 0) && (sprite[hitsprite].statnum < MAXSTATUS)) {
               switch (sprite[hitsprite].picnum) {
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
                    newstatus(hitsprite, BROKENVASE);
                    break;
               case GRONHAL:
               case GRONHALATTACK:
               case GRONHALPAIN:
               case GRONMU:
               case GRONMUATTACK:
               case GRONMUPAIN:
               case GRONSW:
               case GRONSWATTACK:
               case GRONSWPAIN:


               case KOBOLD:
               case KOBOLDATTACK:
               case DEVILSTAND:
               case DEVIL:
               case DEVILATTACK:
               case SKELETON:
               case SKELETONATTACK:
               case IMP:
               case IMPATTACK:
                    // case IMPPAIN:

               case MINOTAUR:
               case MINOTAURATTACK:
               case MINOTAURPAIN:
               case FISH:
               case RAT:
               case WILLOW:
               case DEMON:
               case GUARDIAN:
               case GUARDIANATTACK:
               case KURTSTAND:
               case KURTKNEE:
               case KURTAT:
               case KURTPUNCH:
               case KURTREADY:
               case NEWGUYSTAND:
               case NEWGUYKNEE:
               case NEWGUY:
               case NEWGUYCAST:
               case NEWGUYBOW:
               case NEWGUYMACE:
               case NEWGUYPUNCH:

               case GONZOCSW:
               case GONZOCSWAT:
               case GONZOGSW:
               case GONZOGSWAT:
               case GONZOGHM:
               case GONZOGHMAT:
               case GONZOGSH:
               case GONZOGSHAT:
               case KATIE:
               case KATIEAT:

                    if (netgame) {
                         netshootgun(hitsprite, currweapon);
                         break;
                    }

                    sprite[hitsprite].hitag -= (krand() & 30) + 15;
                    if (sprite[hitsprite].hitag <= 0) {
                         newstatus(hitsprite, DIE);
                         if (sprite[hitsprite].picnum == RAT)
                              chunksofmeat(plr, hitsprite, hitx, hity, hitz, hitsect, daang);
                    }
                    else {
                         sprite[hitsprite].ang = (getangle(plr->x - sprite[hitsprite].x, plr->y - sprite[hitsprite].y) & 2047);
                         newstatus(hitsprite, PAIN);
                    }
                    break;
                    // SHATTER FROZEN CRITTER
               case GRONHALDIE:
               case GRONMUDIE:
               case GRONSWDIE:
               case KOBOLDDIE:
               case DEVILDIE:
               case SKELETONDIE:
               case IMPDIE:
               case MINOTAURDIE:
               case KATIEPAIN:
                    if (sprite[hitsprite].pal == 6) {
                         // JSA_NEW
                         SND_PlaySound(S_SOCK1 + (rand() % 4), 0, 0, 0, 0);
                         playsound_loc(S_FREEZEDIE, hitx, hity);
                         for (k = 0; k < 32; k++)
                              icecubes(hitsprite, hitx, hity, hitz, hitsprite);
                         deletesprite((short) hitsprite);
                    }
               }                   // switch
          }
          break;

     case 6:                       // MEDUSA
          for (i = 0; i < MAXSPRITES; i++) {
               // cansee
               if (i != plr->spritenum)
                    if (sprite[i].picnum == KOBOLD
                        || sprite[i].picnum == IMP
                        || sprite[i].picnum == MINOTAUR
                        || sprite[i].picnum == SKELETON
                        || sprite[i].picnum == GRONHAL
                        || sprite[i].picnum == GRONMU
                        || sprite[i].picnum == GRONSW
                        || sprite[i].picnum == GONZOCSW
                        || sprite[i].picnum == GONZOGSW
                        || sprite[i].picnum == GONZOGHM
                        || sprite[i].picnum == GONZOGSH
                        || sprite[i].picnum == KURTSTAND
                        || sprite[i].picnum == KURTKNEE
                        || sprite[i].picnum == KURTAT
                        || sprite[i].picnum == KURTPUNCH
                        || sprite[i].picnum == KURTREADY
                        || sprite[i].picnum == NEWGUYSTAND
                        || sprite[i].picnum == NEWGUYKNEE
                        || sprite[i].picnum == NEWGUY
                        || sprite[i].picnum == NEWGUYCAST
                        || sprite[i].picnum == NEWGUYBOW
                        || sprite[i].picnum == NEWGUYMACE
                        || sprite[i].picnum == NEWGUYPUNCH)
                         if (cansee(plr->x, plr->y, plr->z, plr->sector,
                                    sprite[i].x, sprite[i].y,
                                    sprite[i].z - (tilesizy[sprite[i].picnum] << 7),
                                    sprite[i].sectnum) == 1) {
                              if (checkmedusadist(i, plr->x, plr->y, plr->z, plr->lvl))
                                   medusa(i);
                         }
          }
          break;
     case 7:                       // KNOCKSPELL

          daang2 = (daang + 2048) & 2047;
          daz2 = (100 - plr->horiz) * 2000;

          hitscan(plr->x, plr->y, plr->z, plr->sector, // Start position
                  sintable[(daang2 + 2560) & 2047],    // X vector of 3D ang
                  sintable[(daang2 + 2048) & 2047],    // Y vector of 3D ang
                  daz2,            // Z vector of 3D ang
                  &hitsect, &hitwall, &hitsprite, &hitx, &hity, &hitz);

          if (hitsect < 0 && hitsprite < 0 || hitwall >= 0) {

               neartag(hitx, hity, hitz, hitsect, daang,
                       &neartagsector, &neartagwall, &neartagsprite, &neartaghitdist, 1024, 3);

               if (neartagsector >= 0) {
                    if (sector[neartagsector].lotag >= 60 &&
                        sector[neartagsector].lotag <= 69) {
                         sector[neartagsector].lotag = 6;
                         sector[neartagsector].hitag = 0;
                    }
                    if (sector[neartagsector].lotag >= 70 &&
                        sector[neartagsector].lotag <= 79) {
                         sector[neartagsector].lotag = 7;
                         sector[neartagsector].hitag = 0;
                    }
                    operatesector(neartagsector);
               }

          }
          break;
     case 10:                      // throw a pike axe
          if (currweaponframe == PIKEATTACK1 + 4
              || currweaponframe == ZPIKEATTACK + 4) {
               if (currweaponanim == 8 && currweapontics == 10) {

                    if (netgame) {
                         netshootgun(-1, 15);
                    }

                    throwpikecnt=(throwpikecnt+1)%THROWPIKELIMIT;
                    if (throwpikesprite[throwpikecnt] != -1) {
                         deletesprite((short) throwpikesprite[throwpikecnt]);
                         throwpikesprite[throwpikecnt]=-1;
                    }

                    if (plr->weapon[currweapon] == 3) {
                         j = insertsprite(plr->sector, MISSILE);
                         throwpikesprite[throwpikecnt]=j;
                         sprite[j].x = plr->x;
                         sprite[j].y = plr->y;
                         sprite[j].z = plr->z + (24 << 8);
                         sprite[j].cstat = 21;
                         sprite[j].picnum = THROWPIKE;
                         sprite[j].ang = ((daang + 2048 + 96) - 512) & 2047;
                         sprite[j].xrepeat = 24;
                         sprite[j].yrepeat = 24;
                         sprite[j].clipdist = 32;
                         sprite[j].extra = daang;
                         sprite[j].shade = -15;
                         sprite[j].xvel = ((krand() & 256) - 128);
                         sprite[j].yvel = ((krand() & 256) - 128);
                         if (shootgunzvel != 0) {
                              sprite[j].zvel = shootgunzvel;
                              shootgunzvel = 0L;
                         }
                         else {
                              sprite[j].zvel = ((100 - plr->horiz) << 4);
                         }
                         sprite[j].owner = 4096;
                         sprite[j].lotag = 1024;
                         sprite[j].hitag = 0;
                         sprite[j].pal = 0;
                         movesprite((short) j, (((long) sintable[(sprite[j].extra + 512) & 2047]) * synctics) << 3, (((long) sintable[sprite[j].extra]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 0);
                         setsprite((short) j, sprite[j].x, sprite[j].y, sprite[j].z);
                    }
                    else {

                         throwpikecnt=(throwpikecnt+1)%THROWPIKELIMIT;
                         if (throwpikesprite[throwpikecnt] != -1) {
                              deletesprite((short) throwpikesprite[throwpikecnt]);
                              throwpikesprite[throwpikecnt]=-1;
                         }

                         j = insertsprite(plr->sector, MISSILE);
                         throwpikesprite[throwpikecnt]=j;
                         sprite[j].x = plr->x;
                         sprite[j].y = plr->y;
                         sprite[j].z = plr->z + (24 << 8);
                         sprite[j].cstat = 21;
                         sprite[j].picnum = THROWPIKE;
                         sprite[j].ang = ((plr->ang + 2048 + 96) - 512) & 2047;
                         sprite[j].xrepeat = 24;
                         sprite[j].yrepeat = 24;
                         sprite[j].clipdist = 32;
                         sprite[j].extra = plr->ang;
                         sprite[j].shade = -15;
                         sprite[j].xvel = ((krand() & 256) - 128);
                         sprite[j].yvel = ((krand() & 256) - 128);
                         sprite[j].zvel = ((100 - plr->horiz) << 4);
                         sprite[j].owner = 4096;
                         sprite[j].lotag = 1024;
                         sprite[j].hitag = 0;
                         sprite[j].pal = 0;
                    }

               }
          }
          if (currweaponframe == PIKEATTACK2 + 4) {
               if (currweaponanim == 4 && currweapontics == 10) {

                    throwpikecnt=(throwpikecnt+1)%THROWPIKELIMIT;
                    if (throwpikesprite[throwpikecnt] != -1) {
                         deletesprite((short) throwpikesprite[throwpikecnt]);
                         throwpikesprite[throwpikecnt]=-1;
                    }

                    if (plr->weapon[currweapon] == 3) {
#if 0
                         for (i = 0; i < 3; i++) {
#endif
                              j = insertsprite(plr->sector, MISSILE);
                              throwpikesprite[throwpikecnt]=j;
                              sprite[j].x = plr->x;
                              sprite[j].y = plr->y;
                              sprite[j].z = plr->z + (24 << 8);
                              sprite[j].cstat = 21;
                              sprite[j].picnum = THROWPIKE;
                              sprite[j].ang = daang;
#if 0
                              sprite[j].ang = ((plr->ang + 2048 + 96) - 512) & 2047;
#endif
                              sprite[j].xrepeat = 24;
                              sprite[j].yrepeat = 24;
                              sprite[j].clipdist = 32;
                              sprite[j].extra = daang;
#if 0
                              switch (i) {
                              case 0:
                                   sprite[j].extra = plr->ang;
                                   break;
                              case 1:
                                   sprite[j].extra = (plr->ang - 64) & 2047;
                                   break;
                              case 2:
                                   sprite[j].extra = (plr->ang + 64) & 2047;
                                   break;
                              }
#endif
                              sprite[j].shade = -15;
                              sprite[j].xvel = ((krand() & 256) - 128);
                              sprite[j].yvel = ((krand() & 256) - 128);
                              sprite[j].zvel = ((100 - plr->horiz) << 4);
                              sprite[j].owner = 4096;
                              sprite[j].lotag = 1024;
                              sprite[j].hitag = 0;
                              sprite[j].pal = 0;
                              movesprite((short) j, (((long) sintable[(sprite[j].extra + 512) & 2047]) * synctics) << 3, (((long) sintable[sprite[j].extra]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 0);
                              setsprite((short) j, sprite[j].x, sprite[j].y, sprite[j].z);
#if 0
                         }
#endif
                    }
                    else {
                         j = insertsprite(plr->sector, MISSILE);
                         throwpikesprite[throwpikecnt]=j;
                         sprite[j].x = plr->x;
                         sprite[j].y = plr->y;
                         sprite[j].z = plr->z + (24 << 8);
                         sprite[j].cstat = 21;
                         sprite[j].picnum = THROWPIKE;
                         sprite[j].ang = ((plr->ang + 2048 + 96) - 512) & 2047;
                         sprite[j].xrepeat = 24;
                         sprite[j].yrepeat = 24;
                         sprite[j].clipdist = 32;
                         sprite[j].extra = plr->ang;
                         sprite[j].shade = -15;
                         sprite[j].xvel = ((krand() & 256) - 128);
                         sprite[j].yvel = ((krand() & 256) - 128);
                         sprite[j].zvel = ((100 - plr->horiz) << 4);
                         sprite[j].owner = 4096;
                         sprite[j].lotag = 1024;
                         sprite[j].hitag = 0;
                         sprite[j].pal = 0;
                    }

               }
          }
          break;
     case 2:                       // parabolic trajectory

          if (netgame) {
               netshootgun(-1, 12);
          }

          j = insertsprite(plr->sector, MISSILE);
          sprite[j].x = plr->x;
          sprite[j].y = plr->y;
          sprite[j].z = plr->z + (8 << 8) + ((krand() & 10) << 8);
          sprite[j].cstat = 0;
          sprite[j].picnum = PLASMA;
          sprite[j].shade = -32;
          sprite[j].pal = 0;
          sprite[j].xrepeat = 16;
          sprite[j].yrepeat = 16;
          sprite[j].ang = daang;
          sprite[j].xvel = (sintable[(daang + 2560) & 2047] >> 5);
          sprite[j].yvel = (sintable[(daang + 2048) & 2047] >> 5);
          if (shootgunzvel != 0) {
               sprite[j].zvel = shootgunzvel;
               shootgunzvel = 0L;
          }
          else {
               sprite[j].zvel = ((100 - plr->horiz) << 4);
          }
          sprite[j].owner = 4096;
          sprite[j].lotag = 256;
          sprite[j].hitag = 0;
          sprite[j].clipdist = 48;

          // movesprite
          // setsprite
          movesprite((short) j, (((long) sintable[(sprite[j].ang + 512) & 2047]) * synctics) << 3, (((long) sintable[sprite[j].ang]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 0);
          setsprite((short) j, sprite[j].x, sprite[j].y, sprite[j].z);

          break;
     case 3:

          if (netgame) {
               netshootgun(-1, 13);
          }

          j = insertsprite(plr->sector, MISSILE);
          sprite[j].x = plr->x;
          sprite[j].y = plr->y;
          sprite[j].z = plr->z + (8 << 8);
          sprite[j].cstat = 0;     // Hitscan does not hit other bullets
          sprite[j].picnum = MONSTERBALL;
          sprite[j].shade = -32;
          sprite[j].pal = 0;
          sprite[j].xrepeat = 64;
          sprite[j].yrepeat = 64;
          // sprite[j].ang=plr->ang;
          sprite[j].ang = daang;
          sprite[j].xvel = (sintable[(daang + 2560) & 2047] >> 7);
          sprite[j].yvel = (sintable[(daang + 2048) & 2047] >> 7);
          if (shootgunzvel != 0L) {
               sprite[j].zvel = shootgunzvel;
               shootgunzvel = 0L;
          }
          else {
               sprite[j].zvel = ((100 - plr->horiz) << 4);
          }
          sprite[j].owner = 4096;
          sprite[j].lotag = 256;
          sprite[j].hitag = 0;
          sprite[j].clipdist = 64;
          movesprite((short) j, (((long) sintable[(sprite[j].ang + 512) & 2047]) * synctics) << 3, (((long) sintable[sprite[j].ang]) * synctics) << 3, 0L, 4L << 8, 4L << 8, 0);
          setsprite((short) j, sprite[j].x, sprite[j].y, sprite[j].z);


          break;
     case 4:

          if (netgame) {
               netshootgun(-1, 14);
          }
          for (j=0 ; j < MAXSPRITES ; j++) {
               switch (sprite[j].picnum) {
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
                    if (cansee(plr->x, plr->y, plr->z, plr->sector,
                               sprite[j].x, sprite[j].y,
                               sprite[j].z - (tilesizy[sprite[j].picnum] << 7),
                               sprite[j].sectnum) == 1) {
                         if (sprite[j].owner != 4096) {
                              nukespell(j);
                         }
                    }
                    break;
               }
          }
/*
           j=insertsprite(plr->sector,HEATSEEKER);
           sprite[j].x=plr->x;
           sprite[j].y=plr->y;
           sprite[j].z=plr->z+(8<<8);
           sprite[j].cstat=0;        //Hitscan does not hit other bullets
           sprite[j].picnum=PLASMA;
           sprite[j].shade=-32;
           sprite[j].pal=0;
           sprite[j].xrepeat=16;
           sprite[j].yrepeat=16;
           sprite[j].ang=plr->ang;
           sprite[j].xvel=(sintable[(daang+2560)&2047]>>7);
           sprite[j].yvel=(sintable[(daang+2048)&2047]>>7);
           sprite[j].zvel=((100-plr->horiz)<<4);
           sprite[j].owner=4096;
           sprite[j].lotag=1024;
           sprite[j].hitag=0;
           sprite[j].clipdist=128;
          
          movesprite((short)j,(((long)sintable[(sprite[j].ang+512)&2047])*synctics)<<3,(((long)sintable[sprite[j].ang])*synctics)<<3,0L,4L<<8,4L<<8,0);
          setsprite((short)j,sprite[j].x,sprite[j].y,sprite[j].z);

*/

          break;
     }


}




void
singleshot(short bstatus)
{

     hasshot = 0;
     return;
     if (selectedgun != 1 && oldmousestatus == 0) {
          keystatus[keys[KEYFIRE]] = keystatus[0x9d] = 0;
          hasshot = 0;
     }

}

void
potionpic(int currentpotion)
{

     struct player *plr;
     int  i;
     int  tilenum;

     if (playerdie == 1) {
          return;
     }

     plr = &player[pyrn];

     if (netgame)
          return;

     if (svga == 1) {
          itemtoscreen(260L << 1, 387L, SPOTIONBACKPIC, 0, 0);
          itemtoscreen((260L << 1) - 4L, 380L, SPOTIONARROW + currentpotion, 0, 0);
     }
     else {
          itemtoscreen(260L, 161L, POTIONBACKPIC, 0, 0);
          itemtoscreen(258L, 157L, POTIONARROW + currentpotion, 0, 0);
     }

     for (i = 0; i < MAXPOTIONS; i++) {
          if (plr->potion[i] < 0)
               plr->potion[i] = 0;
          if (plr->potion[i] > 0) {
               switch (i) {
               case 0:
                    if (svga == 1)
                         tilenum = SFLASKBLUE;
                    else
                         tilenum = FLASKBLUE;
                    break;
               case 1:
                    if (svga == 1)
                         tilenum = SFLASKGREEN;
                    else
                         tilenum = FLASKGREEN;
                    break;
               case 2:
                    if (svga == 1)
                         tilenum = SFLASKOCHRE;
                    else
                         tilenum = FLASKOCHRE;
                    break;
               case 3:
                    if (svga == 1)
                         tilenum = SFLASKRED;
                    else
                         tilenum = FLASKRED;
                    break;
               case 4:
                    if (svga == 1)
                         tilenum = SFLASKTAN;
                    else
                         tilenum = FLASKTAN;
                    break;
               }

               potiontilenum = tilenum;

               if (svga == 1)
                    // overwritesprite((262<<1)+(i*20),406,potiontilenum,0,0,0);
                    //
                    itemtoscreen((long) ((262 << 1) + (i * 20)), 406L, potiontilenum, 0, 0);
               /* rotatesprite((long)((262<<1)+(i*20))<<16, 406L<<16, 65536L,
                * 0, potiontilenum, 0, 0, 8+16, 0L,0L,xdim-1,ydim-1); */
               else
                    // overwritesprite((long)(262+(i*10)),169L,potiontilenum,0,0
                    // ,0);
                    itemtoscreen((long) (262 + (i * 10)), 169L, potiontilenum, 0, 0);

               itoa(plr->potion[i], potionbuf, 10);

               if (svga == 1)
                    fancyfont((long) ((266 << 1) + (i * 20)), 394L, SPOTIONFONT - 26, potionbuf, 0);
               else
                    fancyfont((long) (266 + (i * 10)), 164L, SMFONT - 26, potionbuf, 0);

          }
          else {
               if (svga == 1)
                    // overwritesprite((262<<1)+(i*20),406,SFLASKBLACK,0,0,0);
                    // itemtoscreen((long)((262<<1)+(i*10)),406L,SFLASKBLACK,0,0
                    // ,0);
                    rotatesprite((long) ((262 << 1) + (i * 20)) << 16,
                                 406L << 16,
                                 65536L,
                                 0,
                                 SFLASKBLACK,
                                 0,
                                 0,
                                 8 + 16,
                                 0L, 0L, xdim - 1, ydim - 1);

               else
                    // overwritesprite((long)(262+(i*10)),169L,FLASKBLACK,0,0,0)
                    // ;
                    itemtoscreen((long) (262 + (i * 10)), 169L, FLASKBLACK, 0, 0);
          }
     }


}


void 
usapotion(struct player * plr)
{
     if (playerdie == 1) {
          return;
     }

     if (currentpotion == 0 && plr->health >= plr->maxhealth)
          return;

     if (currentpotion == 2 && poisoned == 0)
          return;

     if (plr->potion[currentpotion] <= 0)
          return;
     else
          plr->potion[currentpotion]--;

     switch (currentpotion) {
     case 0:                       // health potion
          if (plr->health + 25 > plr->maxhealth) {
               plr->health = plr->maxhealth;
               SND_PlaySound(S_DRINK, 0, 0, 0, 0);
               healthpic(0);
          }
          else {
               SND_PlaySound(S_DRINK, 0, 0, 0, 0);
               healthpic(25);
          }
          startblueflash(10);
          break;
     case 1:                       // strength
          strongtime = 3200;
          SND_PlaySound(S_DRINK, 0, 0, 0, 0);
          startblueflash(10);
          break;
     case 2:                       // anti venom
          SND_PlaySound(S_DRINK, 0, 0, 0, 0);
          poisoned = 0;
          poisontime = 0;
          startwhiteflash(10);
          strcpy(displaybuf, "poison cured");
          healthpic(0);
          displaytime = 360;
          break;
     case 3:                       // fire resist
          SND_PlaySound(S_DRINK, 0, 0, 0, 0);
          manatime = 3200;
          startwhiteflash(10);
          if (lavasnd != -1) {
               SND_StopLoop(lavasnd);
               lavasnd = -1;
          }
          armorpic(0);
          break;
     case 4:                       // invisi
          SND_PlaySound(S_DRINK, 0, 0, 0, 0);
          invisibletime = 3200;
          startgreenflash(10);
          break;
     }

     potionpic(currentpotion);

}

void
orbpic(int currentorb)
{

     int  tilenum;
     int  spellbookpage;
     struct player *plr;
     long dax,
          day;

     plr = &player[pyrn];

     if (playerdie == 1) {
          return;
     }

     if (plr->orbammo[currentorb] < 0)
          plr->orbammo[currentorb] = 0;

     itoa(plr->orbammo[currentorb], tempbuf, 10);

     if (svga == 1)
          spellbookpage = sspellbookanim[currentorb][8].daweaponframe;
     else
          spellbookpage = spellbookanim[currentorb][8].daweaponframe;

     if (svga == 1) {
          itemtoscreen(120L << 1, 372L, SSPELLBACK, 0, 0);
          spellbookframe = sspellbookanim[currentorb][8].daweaponframe;
          dax = sspellbookanim[currentorb][8].currx;
          day = sspellbookanim[currentorb][8].curry;
          itemtoscreen(dax << 1, day, spellbookframe, 0, 0);
          itoa(plr->orbammo[currentorb], tempbuf, 10);
          fancyfont(126L << 1, 439L, SSCOREFONT - 26, tempbuf, 0);
     }
     else {
          itemtoscreen(122L, 155L, SPELLBOOKBACK, 0, 0);
          itemtoscreen(121L, 161L, spellbookpage, 0, 0);
          fancyfont(126L, 181L, SCOREFONT - 26, tempbuf, 0);
     }

}

void
healthpic(int hp)
{

     int  i,tilenum;
     struct player *plr;

     plr = &player[pyrn];

     if (playerdie == 1) {
          return;
     }
     if (godmode == 1) {
          plr->health = 200;
          if (svga == 0) {
               if (plr->screensize <= 320) {
                    itemtoscreen(72L, 168L, HEALTHBACKPIC, 0, 0);
                    fancyfont(74L, 170L, BGZERO - 26, healthbuf, 0);
               }
          }
          else if (svga == 1) {
               if (plr->screensize == 320) {
                    itemtoscreen(74L << 1, 406L, SHEALTHBACK, 0, 0);
                    fancyfont(76L << 1, 409L, SHEALTHFONT - 26, healthbuf, 0);
               }
          }
          return;
     }
     switch (difficulty) {
     case 1:
          hp >>= 1;
          break;
     case 3:
          hp <<= 1;
          break;
     }
     plr->health += hp;

     if (plr->health < 0) {
          plr->health = 0;
     }

     itoa(plr->health, healthbuf, 10);

     if (svga == 0) {
          if (plr->screensize <= 320) {
#if 0
               if (poisoned == 1) {
                    // overwritesprite(72,168,HEALTHBACKPIC,0,0,6);
                    itemtoscreen(72L, 168L, HEALTHBACKPIC, 0, 6);
                    fancyfont(74L, 170L, BGZERO - 26, healthbuf, 6);
               }
               else {
#endif
                    // overwritesprite(72,168,HEALTHBACKPIC,0,0,0);
                    itemtoscreen(72L, 168L, HEALTHBACKPIC, 0, 0);
                    fancyfont(74L, 170L, BGZERO - 26, healthbuf, 0);
#if 0
               }
#endif
          }
     }
     else if (svga == 1) {
          if (plr->screensize == 320) {
#if 0
               if (poisoned == 1) {
                    // overwritesprite(74<<1,406,SHEALTHBACK,0,0,6);
                    itemtoscreen(74L << 1, 406L, SHEALTHBACK, 0, 6);
                    fancyfont(76L << 1, 409L, SHEALTHFONT - 26, healthbuf, 6);
               }
               else {
#endif
                    // overwritesprite(74<<1,406,SHEALTHBACK,0,0,0);
                    itemtoscreen(74L << 1, 406L, SHEALTHBACK, 0, 0);
                    fancyfont(76L << 1, 409L, SHEALTHFONT - 26, healthbuf, 0);
#if 0
               }
#endif
          }
     }


}


void
armorpic(int arm)
{

     int  tilenum;
     struct player *plr;

     plr = &player[pyrn];

     if (playerdie == 1) {
          return;
     }

     plr->armor += arm;

     if (plr->armor < 0) {
          plr->armor = 0;
          plr->armortype = 0;
     }

     itoa(plr->armor, armorbuf, 10);

     if (svga == 0) {
          if (plr->screensize <= 320) {
//               if (manatime > 0) {
//                    itemtoscreen(197L, 168L, HEALTHBACKPIC, 0, 8);
//                    fancyfont(201L, 170L, BGZERO - 26, armorbuf, 8);
//               }
//               else {
                    itemtoscreen(197L, 168L, HEALTHBACKPIC, 0, 0);
                    fancyfont(201L, 170L, BGZERO - 26, armorbuf, 0);
//               }
          }
     }
     else {
          if (plr->screensize == 320) {
//               if (manatime > 0) {
//                    itemtoscreen(200L << 1, 406L, SHEALTHBACK, 0, 8);
//                    fancyfont(204L << 1, 409L, SHEALTHFONT - 26, armorbuf, 8);
//               }
//               else {
                    itemtoscreen(200L << 1, 406L, SHEALTHBACK, 0, 0);
                    fancyfont(204L << 1, 409L, SHEALTHFONT - 26, armorbuf, 0);
//               }
          }
     }


}

void
levelpic(void)
{

     struct player *plr;
     int  x,
          y;
     int  tilenum;
     char temp[20];
     char temp2[20];

     plr = &player[pyrn];

     if (playerdie == 1) {
          return;
     }

     if (selectedgun == 6) {
          if (plr->ammo[6] < 0)
               plr->ammo[6] = 0;
          itoa(plr->ammo[6], temp, 10);
          strcpy(temp2, temp);
          if (svga == 0) {
               // overwritesprite(3,181,ARROWS,0,0,0);
               itemtoscreen(3L, 181L, ARROWS, 0, 0);
               fancyfont(42, 183, SCOREFONT - 26, temp2, 0);
          }
          else {
               // overwritesprite(3<<1,434,SARROWS,0,0,0);
               itemtoscreen(3L << 1, 434L, SARROWS, 0, 0);
               fancyfont(42 << 1, 439, SSCOREFONT - 26, temp2, 0);
          }
     }
     else if (selectedgun == 7 && plr->weapon[7] == 2) {
          if (plr->ammo[7] < 0)
               plr->ammo[7] = 0;
          itoa(plr->ammo[7], temp, 10);
          strcpy(temp2, temp);
          // need pike pic
          if (svga == 0) {
               // overwritesprite(3,181,PIKES,0,0,0);
               itemtoscreen(3L, 181L, PIKES, 0, 0);
               fancyfont(42, 183, SCOREFONT - 26, temp2, 0);
          }
          else {
               // overwritesprite(3<<1,434,SPIKES,0,0,0);
               itemtoscreen(3L << 1, 434L, SPIKES, 0, 0);
               fancyfont(42 << 1, 439, SSCOREFONT - 26, temp2, 0);
          }
     }
     else {

          if (svga == 0)
               tilenum = PLAYERLVL + (plr->lvl - 1);
          else
               tilenum = SPLAYERLVL + (plr->lvl - 1);

          if (svga == 0)
               // overwritesprite(3,181,tilenum,0,0,0);
               itemtoscreen(3L, 181L, tilenum, 0, 0);
          else
               // overwritesprite(3<<1,436,tilenum,0,0,0);
               itemtoscreen(3L << 1, 436L, tilenum, 0, 0);

     }

}


void 
score(int score)
{

     int  x,
          y;
     int  tilenum;
     struct player *plr;

     plr = &player[pyrn];

     if (playerdie == 1) {
          return;
     }

     plr->score += score;
     expgained += score;

     tilenum = SCOREFONT - 26;

     itoa(plr->score, tempbuf, 10);

     if (svga == 1)
          // overwritesprite(6,394,SSCOREBACKPIC,0,0,0);
          itemtoscreen(6L, 394L, SSCOREBACKPIC, 0, 0);
     // itemtoscreen(6L,163L,SSCOREBACKPIC,0,0,0);
     else
          // overwritesprite(29,163,SCOREBACKPIC,0,0,0);
          itemtoscreen(29L, 163L, SCOREBACKPIC, 0, 0);

     strcpy(scorebuf, tempbuf);

     if (svga == 1) {
          if (plr->screensize == 320)
               fancyfont(60, 396 + 1, SSCOREFONT - 26, scorebuf, 0);
     }
     else {
          if (plr->screensize <= 320)
               fancyfont(30, 165, SCOREFONT - 26, scorebuf, 0);
     }

     goesupalevel(plr);

}

void
goesupalevel(struct player * plr)
{
     switch (plr->lvl) {
     case 0:
     case 1:
          if (plr->score > 9999) {
               strcpy(displaybuf, "thou art a warrior");
               displaytime = 360;
               plr->lvl = 2;
               plr->maxhealth = 120;
               levelpic();
          }
          break;
     case 2:
          if (plr->score > 19999) {
               strcpy(displaybuf, "thou art a swordsman");
               displaytime = 360;
               plr->lvl = 3;
               plr->maxhealth = 140;
               levelpic();
          }
          break;
     case 3:
          if (plr->score > 29999) {
               strcpy(displaybuf, "thou art a hero");
               displaytime = 360;
               plr->lvl = 4;
               plr->maxhealth = 160;
               levelpic();
          }
          break;
     case 4:
          if (plr->score > 39999) {
               strcpy(displaybuf, "thou art a champion");
               displaytime = 360;
               plr->lvl = 5;
               plr->maxhealth = 180;
               levelpic();
          }
          break;
     case 5:
          if (plr->score > 49999) {
               strcpy(displaybuf, "thou art a superhero");
               displaytime = 360;
               plr->lvl = 6;
               plr->maxhealth = 200;
               levelpic();
          }
          break;
     case 6:
          if (plr->score > 59999) {
               strcpy(displaybuf, "thou art a lord");
               displaytime = 360;
               plr->lvl = 7;
               levelpic();
          }
     }
}


int
checkweapondist(short i, long x, long y, long z, char guntype)
{

     int  length;

     if (selectedgun == 0) {
          length = 1024;
     }
     else {
          switch (selectedgun) {
          case 1:
               length = 1024;
               break;
          case 2:
          case 3:
          case 4:
          case 5:
               length = 1536;
               break;
          case 6:
               length = 2048;
               break;
          case 7:
               length = 1024;
               break;
          case 8:
          case 9:
               length = 2048;
               break;
          }
     }
     if ((labs(x - sprite[i].x) + labs(y - sprite[i].y) < length) && (labs((z >> 8) - ((sprite[i].z >> 8) - (tilesizy[sprite[i].picnum] >> 1))) <= (length >> 3)))
          return (1);
     else
          return (0);

}


void 
updatepics(void)
{

     score(0);                     // OK
     if (netgame != 0) {
          if (gametype >= 1)
               captureflagpic();
          else
               fragspic();
     }
     else {
          potionpic(currentpotion);// OK
     }
     levelpic();                   // ok
     healthpic(0);                 // OK
     armorpic(0);                  // OK
     orbpic(currentorb);
     keyspic();                    // OK

}

extern short teamscore[];
extern short teaminplay[];

struct capt {
     int  x;
     int  y;
     int  palnum;
};

void 
captureflagpic(void)
{

     int  i;

     struct capt flag[4] = {{260, 161, 0},
     {286, 161, 10},
     {260, 176, 11},
     {286, 176, 12}};

     struct capt sflag[4] = {{260, 387, 0},
     {286, 387, 10},
     {260, 417, 11},
     {286, 417, 12}};

     if (svga == 1) {
          // overwritesprite(260<<1,387,SPOTIONBACKPIC,0,0,0);
          itemtoscreen(260L << 1, 387L, SPOTIONBACKPIC, 0, 0);
     }
     else {
          // overwritesprite(260,161,POTIONBACKPIC,0,0,0);
          itemtoscreen(260L, 161L, POTIONBACKPIC, 0, 0);
     }

     for (i = 0; i < 4; i++) {
          if (svga == 1) {
               if (teaminplay[i]) {
                    overwritesprite((sflag[i].x << 1) + 6, sflag[i].y + 8, STHEFLAG, 0, 0, sflag[i].palnum);
                    itoa(teamscore[i], tempbuf, 10);
                    fancyfont((sflag[i].x << 1) + 16, sflag[i].y + 16, SPOTIONFONT - 26, tempbuf, 0);
               }
          }
          else {
               if (teaminplay[i]) {
                    rotatesprite((flag[i].x+3) << 16, (flag[i].y+3) << 16,
                                 65536L, 0, THEFLAG, 0, flag[i].palnum, 8 + 16,
                                 0L, 0L, xdim - 1L, ydim - 1L);
//                    overwritesprite(flag[i].x + 3, flag[i].y + 3, THEFLAG, 0, 0, flag[i].palnum);
                    itoa(teamscore[i], tempbuf, 10);
                    fancyfont(flag[i].x + 6, flag[i].y + 6, SMFONT - 26,
                              tempbuf, 0);
               }
          }
     }
}

void 
fragspic(void)
{

     struct player *plr;

     plr = &player[pyrn];

     if (svga == 1) {
          if (plr->screensize == 320) {
               // overwritesprite(260<<1,387,SPOTIONBACKPIC,0,0,0);
               itemtoscreen(260L << 1, 387L, SPOTIONBACKPIC, 0, 0);
               itoa(teamscore[pyrn], tempbuf, 10);
               fancyfont((260 << 1) + 10, 387 + 10, SHEALTHFONT - 26, tempbuf, 0);
          }
     }
     else {
          if (plr->screensize <= 320) {
               // overwritesprite(260,161,POTIONBACKPIC,0,0,0);
               itemtoscreen(260L, 161L, POTIONBACKPIC, 0, 0);
               itoa(teamscore[pyrn], tempbuf, 10);
               fancyfont(260 + 15, 161 + 5, BGZERO - 26, tempbuf, 0);
          }
     }

}


void 
keyspic(void)
{

     struct player *plr;

     plr = &player[pyrn];

     if (svga == 1) {
          if (plr->treasure[14] == 1)
               // overwritesprite(242<<1,387,SKEYBRASS,0,0,0);
               itemtoscreen(242L << 1, 387L, SKEYBRASS, 0, 0);
          else
               // overwritesprite(242<<1,387,SKEYBLANK,0,0,0);
               itemtoscreen(242L << 1, 387L, SKEYBLANK, 0, 0);

          if (plr->treasure[15] == 1)
               // overwritesprite(242<<1,408,SKEYBLACK,0,0,0);
               itemtoscreen(242L << 1, 408L, SKEYBLACK, 0, 0);
          else
               // overwritesprite(242<<1,408,SKEYBLANK,0,0,0);
               itemtoscreen(242 << 1, 408, SKEYBLANK, 0, 0);

          if (plr->treasure[16] == 1)
               // overwritesprite(242<<1,430,SKEYGLASS,0,0,0);
               itemtoscreen(242L << 1, 430L, SKEYGLASS, 0, 0);
          else
               // overwritesprite(242<<1,430,SKEYBLANK,0,0,0);
               itemtoscreen(242L << 1, 430L, SKEYBLANK, 0, 0);

          if (plr->treasure[17] == 1)
               // overwritesprite(242<<1,452,SKEYIVORY,0,0,0);
               itemtoscreen(242L << 1, 452L, SKEYIVORY, 0, 0);
          else
               // overwritesprite(242<<1,452,SKEYBLANK,0,0,0);
               itemtoscreen(242L << 1, 452L, SKEYBLANK, 0, 0);
     }
     else {
          if (plr->treasure[14] == 1)
               // overwritesprite(242,160,KEYBRASS,0,0,0);
               itemtoscreen(242L, 160L, KEYBRASS, 0, 0);
          else
               // overwritesprite(242,160,KEYBLANK,0,0,0);
               itemtoscreen(242L, 160L, KEYBLANK, 0, 0);

          if (plr->treasure[15] == 1)
               // overwritesprite(242,169,KEYBLACK,0,0,0);
               itemtoscreen(242L, 169L, KEYBLACK, 0, 0);
          else
               // overwritesprite(242,169,KEYBLANK,0,0,0);
               itemtoscreen(242L, 169L, KEYBLANK, 0, 0);

          if (plr->treasure[16] == 1)
               // overwritesprite(242,178,KEYGLASS,0,0,0);
               itemtoscreen(242L, 178L, KEYGLASS, 0, 0);
          else
               // overwritesprite(242,178,KEYBLANK,0,0,0);
               itemtoscreen(242L, 178L, KEYBLANK, 0, 0);

          if (plr->treasure[17] == 1)
               // overwritesprite(242,187,KEYIVORY,0,0,0);
               itemtoscreen(242L, 187L, KEYIVORY, 0, 0);
          else
               // overwritesprite(242,187,KEYBLANK,0,0,0);
               itemtoscreen(242L, 187L, KEYBLANK, 0, 0);
     }

}


int 
adjustscore(int score)
{

     float factor;

     factor = (krand() % 20) / 100;

     if (krand() % 100 > 50)
          return (score * (factor + 1));
     else
          return (score - (score * (factor)));

}

int 
lvlspellcheck(struct player * plr)
{

     int  legal = 0;

     legal = 1;
#if 0
     switch (currentorb) {
     case 0:
          legal = 1;
          break;
     case 1:
          legal = 1;
          break;
     case 2:
          if (plr->lvl > 1) {
               legal = 1;
          }
          else {
               strcpy(displaybuf, "must attain 2nd level");
               displaytime = 360;
          }
          break;
     case 3:
          if (plr->lvl > 1) {
               legal = 1;
          }
          else {
               strcpy(displaybuf, "must attain 2nd level");
               displaytime = 360;
          }
          break;
     case 4:
          if (plr->lvl > 2) {
               legal = 1;
          }
          else {
               strcpy(displaybuf, "must attain 3rd level");
               displaytime = 360;
          }
          break;
     case 5:
          if (plr->lvl > 2) {
               legal = 1;
          }
          else {
               strcpy(displaybuf, "must attain 3rd level");
               displaytime = 360;
          }
          break;
     case 6:
          if (plr->lvl > 3) {
               legal = 1;
          }
          else {
               strcpy(displaybuf, "must attain 4th level");
               displaytime = 360;
          }
          break;
     case 7:
          if (plr->lvl > 4) {
               legal = 1;
          }
          else {
               strcpy(displaybuf, "must attain 5th level");
               displaytime = 360;
          }
          break;
     }
#endif
     return (legal);
}

void 
gronmissile(int s)
{

     struct player *plr;
     short k,
          j,
          daang;
     long discrim;

     plr = &player[pyrn];

     daang = (sprite[s].ang - 36) & 2047;

     for (k = 0; k < 10; k++) {

          daang = (daang + (k << 1)) & 2047;

          j = insertsprite(sprite[s].sectnum, MISSILE);
          sprite[j].x = sprite[s].x;
          sprite[j].y = sprite[s].y;
          sprite[j].z = sprite[s].z + (8 << 8) + ((krand() & 10) << 8);
          sprite[j].cstat = 0;
          sprite[j].picnum = PLASMA;
          sprite[j].shade = -32;
          sprite[j].pal = 0;
          sprite[j].xrepeat = 16;
          sprite[j].yrepeat = 16;
          sprite[j].ang = daang;
          sprite[j].xvel = (sintable[(daang + 2560) & 2047] >> 5);
          sprite[j].yvel = (sintable[(daang + 2048) & 2047] >> 5);
          discrim = ksqrt((plr->x - sprite[s].x) * (plr->x - sprite[s].x)
                          + (plr->y - sprite[s].y) * (plr->y - sprite[s].y));
          if (discrim == 0)
               discrim = 1;
          sprite[j].zvel = (((plr->z + (8 << 8)) - sprite[s].z) << 7) / discrim;
          sprite[j].owner = s;
          sprite[j].lotag = 256;
          sprite[j].hitag = 0;
          sprite[j].clipdist = 48;

     }

}


void 
displayspelltext(void)
{
     switch (currentorb) {
 case 0:
          strcpy(displaybuf, "scare spell");
          displaytime = 360;
          break;
     case 1:
          strcpy(displaybuf, "night vision spell");
          displaytime = 360;
          break;
     case 2:
          strcpy(displaybuf, "freeze spell");
          displaytime = 360;
          break;
     case 3:
          strcpy(displaybuf, "magic arrow spell");
          displaytime = 360;
          break;
     case 4:
          strcpy(displaybuf, "open door spell");
          displaytime = 360;
          break;
     case 5:
          strcpy(displaybuf, "fly spell");
          displaytime = 360;
          break;
     case 6:
          strcpy(displaybuf, "fireball spell");
          displaytime = 360;
          break;
     case 7:
          strcpy(displaybuf, "nuke spell");
          displaytime = 360;
          break;
     }

}


void
painsound(long xplc, long yplc)
{
     playsound_loc(S_BREATH1 + (rand() % 6), xplc, yplc);
}

#if 0
void
turntostone(short i)
{

     switch (sprite[i].picnum) {

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
          sprite[i].picnum++;
          sprite[i].pal = 8;
          sprite[i].shade = 15;
          changespritestat(i, 0);
          break;
     }

}
#endif

int
inView(struct player *plr,int i)
{
     short a;

     a = getangle(sprite[i].x - plr->x, sprite[i].y - plr->y);
     if ((a < plr->ang && plr->ang - a < 256) ||
         (a >= plr->ang && ((plr->ang + a) & 2047) < 256)) {
          return(1);
     }
     return(0);
}

