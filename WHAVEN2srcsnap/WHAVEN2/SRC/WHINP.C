/***************************************************************************
 *   WHINP.C  - main game code for Apogee engine                           *
 *                                                                         *
 ***************************************************************************/

#include "icorp.h"
#include "keydefs.h"               // Les 07/24/95
#include "jstick.h"                // Les 07/27/95
#include "avlib.h"                 // Les 01/16/96
//** Les 01/17/96   - START
#include "swift.h"
#include <memcheck.h>
//** Les 01/17/96   - END

extern long reccnt;
extern int overtheshoulder;
extern long screentilt;

int  playrec = 0;
int  wallplay = 0;

int  soundcontrol;
int  musiclevel;
int  digilevel;
int  soundtoggle;

int  runlock = 1;

extern int mapon;
extern int record;

int  followmode = 0;
extern long followx;
extern long followy;

//JSA BLORB
extern int lavasnd;
int  justplayed = 0;
int  lopoint = 0;
int  walktoggle = 0;
int  runningtime = 0;

short cybyaw,
     cybpit,
     cybrol;

int  charsperline = 0;
char nettemp[80];
extern char typemessageleng,
     typemode;
int  nettypemode = 0;

//scantoasc

extern int displaytime;

int  mousecalibrate = 0;
int  mousespeed = 3;
int  mousxspeed = 3,
     mousyspeed = 3;

long angvel,
     svel,
     vel;


extern int escapetomenu;
extern int currweaponflip;
extern int invincibletime;
extern int manatime;

long lockclock;

long oldhoriz;

int  pressedkey;
int  testpaleffects = 0;

char joyb,
     oldjoyb,
     oldbstatus,
     butbit[] = {0x10, 0x20, 0x40, 0x80},
     mbutbit[] = {0x01, 0x04, 0x02};    // Les 01/17/96

short jcalibration = 0,
     jctrx,
     jctry,
     jlowx,
     jhighx,
     jlowy,
     jhighy,
     jmovespeed = 16;

int  joyx, joyy,
     joykeys[4],
     jstickenabled = 0,
     jturnspeed = 16;

short mousekeys[3];                // Les 01/17/96

extern int wet;
extern char foggy;
extern char tremors;
extern char flashflag;

extern int currweaponfired;

extern int currentorb;
extern int currentpotion;
extern long selectedgun;
extern int currweapon;
extern int hasshot;
extern int orbshot;
extern int showstatusbar;
extern int showheart;
extern int showbook;

short oldmousestatus,
     brightness,
     gbrightness;
extern short compass;
extern int playerdie;

//** Les 01/16/96   - START
short spaceballon,
     spaceballinitflag,
     spacekeys[6] = {
     KEYFIRE,                      // Spaceball button A (use weap)
     KEYUSE,                       // Spaceball button B (use door)
     KEYRUN,                       // Spaceball button C (run mode)
     KEYCNTR,                      // Spaceball button D (center view)
     KEYJUMP,                      // Spaceball button E (jump up)
     KEYMAP                        // Spaceball button F (map mode)
};

SPW_InputEvent sbpacket;
//** Les 01/16/96   - END

//** Les 01/17/96   - START
int  wingmanflag,
     wingmanyawsens = 4,
     wingmanpitchsens = 5,
     wingmanrollsens = 5,
     wingmanzsens = 1,
     wingmanzreverse = 0;

SWIFT_3DStatus wingmaninfo,
     oldwingmaninfo;

short wingmankeys[9] = {
     KEYUSE,                       // Wingman top button (use door)
     KEYUSEP,                      // Wingman thumb button (use potion)
     KEYFIRE,                      // Wingman trigger button (fire weap)
     0,                            // reserved
     KEYCAST,                      // Wingman middle button (cast spell)
     KEYCYCSPELLDN,                // Wingman 4-way west (select lower weap)
     KEYCYCSPELLUP,                // Wingman 4-way east (select upper weap)
     KEYCYCPOTIONUP,               // Wingman 4-way north (select upper potion)
     KEYCYCPOTIONDN                // Wingman 4-way south (select lower potion)
};
//** Les 01/17/96   - END

void
initjstick(void)
{
     jcalibration = 1;
     jstickenabled = 0;
     if (configJoystick[0] == 1) {
          joykeys[0] = configJoystick[1];
          joykeys[1] = configJoystick[2];
          joykeys[2] = configJoystick[3];
          joykeys[3] = configJoystick[4];
     }
     else if (configGamepad[0] == 1) {
          joykeys[0] = configGamepad[1];
          joykeys[1] = configGamepad[2];
          joykeys[2] = configGamepad[3];
          joykeys[3] = configGamepad[4];
     }
}

void
keytimerstuff(void)
{

     if (followmode == 1) {
          goto skip;
     }
     if (keystatus[keys[KEYSTRAFE]] == 0 && keystatus[keys[KEYSTRAFE]+0x80] == 0) {
          if (keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
               angvel -= 16;
               if (angvel < -128) {
                    angvel = -128;
               }
          }
          if (keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
               angvel += 16;
               if (angvel > 127) {
                    angvel = 127;
               }
          }
     }
     else {
          if (keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
               svel += 8;
               if (svel > 127) {
                    svel = 127;
               }
          }
          if (keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
               svel -= 8;
               if (svel < -128) {
                    svel = -128;
               }
          }
     }

     if (keystatus[keys[KEYSTFL]] != 0) {
          svel += 8;
          if (svel > 127) {
               svel = 127;
          }
     }
     else if (keystatus[keys[KEYSTFR]] != 0) {
          svel -= 8;
          if (svel < -128) {
               svel = -128;
          }
     }

     if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
          vel += 8;
          if (vel > 201) {
               vel = 201;
          }
     }

     if (keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
          vel -= 8;
          if (vel < -201) {
               vel = -201;
          }
     }

skip:
     if (angvel < 0) {
          angvel += 12;
          if (angvel > 0) {
               angvel = 0;
          }
     }

     if (angvel > 0) {
          angvel -= 12;
          if (angvel < 0) {
               angvel = 0;
          }
     }

     if (svel < 0) {
          svel += 2;
          if (svel > 0) {
               svel = 0;
          }
     }

     if (svel > 0) {
          svel -= 2;
          if (svel < 0) {
               svel = 0;
          }
     }

     if (vel < 0) {
          vel += 2;
          if (vel > 0) {
               vel = 0;
          }
     }

     if (vel > 0) {
          vel -= 2;
          if (vel < 0) {
               vel = 0;
          }
     }

}

int  jumphoriz;

void
dophysics(struct player * plr, long goalz, short flyupdn, int v)
{
     if (plr->orbactive[5] > 0) {
          if (v > 0) {
               if (plr->horiz > 125)
                    plr->hvel -= (synctics << 6);
               else if (plr->horiz < 75)
                    plr->hvel += (synctics << 6);
          }
          else {
               if (flyupdn > 0) {
                    plr->hvel -= (synctics << 7);
               }
               if (flyupdn < 0) {
                    plr->hvel += (synctics << 7);
               }
          }
          plr->hvel += (sintable[(lockclock << 4) & 2047] >> 6);
          plr->fallz = 0L;
     }
     else if (plr->z < goalz) {
          plr->hvel += (synctics * GRAVITYCONSTANT);
          plr->onsomething &= ~(GROUNDBIT | PLATFORMBIT);
          plr->fallz += plr->hvel;
     }
     else if (plr->z > goalz) {
          plr->hvel -= ((plr->z - goalz) >> 6);
          plr->onsomething |= GROUNDBIT;
          plr->fallz = 0L;
     }
     else {
          plr->fallz = 0L;
     }
     plr->z += plr->hvel;
     if (plr->hvel > 0 && plr->z > goalz) {
          plr->hvel >>= 2;
     }
     else if (plr->onsomething != 0) {
          if (plr->hvel < 0 && plr->z < goalz) {
               plr->hvel = 0;
               plr->z = goalz;
          }
     }
     if (plr->z < sector[plr->sector].ceilingz) {
          plr->z = sector[plr->sector].ceilingz + (plr->height >> 2);
          plr->hvel = 0;
     }
     else if (plr->z > sector[plr->sector].floorz) {
          plr->z = sector[plr->sector].floorz - (plr->height >> 4);
          plr->hvel = 0;
     }
     jumphoriz = -(plr->hvel >> 8);
}

int  debuganalyzespritesflag,
     debugkeysflag;

extern
int  iglassenabled,
     vfx1enabled;

extern
char vfx1_cyberpuck;

int  cybpuckused;

short pitch,
     roll,
     yaw;

long vrangle,
     vrangle2,
     vrdelta,
     vrpitch;

extern
char puckbuttons;

extern
short puckpitch,
     puckroll,
     puckbutton[];

int  patrolpoints = 0;

extern
int  droptheshield;

extern
int  quicksaveflag;

#if 0
long throwx,
     throwy;
#endif

void
processinput(struct player * plr)
{

     short bstatus,
          mousx,
          mousy;
     long goalz,
          hihit,
          hiz,
          i,
          lohit,
          loz,
          tics,
          xvect,
          yvect;
     long dax,
          dax2,
          day,
          day2,
          odax,
          odax2,
          oday,
          oday2;
     int  a,
          s,
          v;
     static int mv;
     long oldposx,
          oldposy,
          oldposz;
     long dist;
     long feetoffground;
     char *ptr;
     short hitobject,
          onsprite = -1;
     static short tempsectornum;
     short onground;

     a = angvel;
     s = svel;
     v = vel;

     if (keystatus[1] > 0) {
          keystatus[1] = 0;
          if (plr->z < sector[plr->sector].floorz - ((PLAYERHEIGHT + 8) << 8)) {
               strcpy(displaybuf, "must be on the ground");
               displaytime = 360;
          }
          else {
               escapetomenu = 1;
               plr->z = sector[plr->sector].floorz - (PLAYERHEIGHT << 8);
               setsprite(plr->spritenum, plr->x, plr->y, plr->z + (plr->height << 8));
               sprite[plr->spritenum].ang = plr->ang;
               visibility = 1024;
          }
     }

     if (followmode == 1) {
          if (keystatus[keys[KEYLEFT]] > 0 || keystatus[RLEFT] > 0) {
               followx += ((long) sintable[plr->ang]) >> 5;
               followy += ((long) sintable[(plr->ang+1536)&2047]) >> 5;
          }
          if (keystatus[keys[KEYRIGHT]] > 0 || keystatus[RRIGHT] > 0) {
               followx += ((long) sintable[(plr->ang+1024)&2047]) >> 5;
               followy += ((long) sintable[(plr->ang+512)&2047]) >> 5;
          }
          if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
               followx += ((long) sintable[(plr->ang+512)&2047]) >> 5;
               followy += ((long) sintable[plr->ang]) >> 5;
          }
          if (keystatus[keys[KEYBACK]] > 0 || keystatus[RDN] > 0) {
               followx += ((long) sintable[(plr->ang+1536)&2047]) >> 5;
               followy += ((long) sintable[(plr->ang+1024)&2047]) >> 5;
          }
     }


     if (keystatus[0x57] > 0) {    // F11 - brightness
          keystatus[0x57] = 0;
          gbrightness = brightness++;
          if (brightness > 8) {
               brightness = 0;
               gbrightness = 0;
          }
          // setbrightness(brightness);
          setbrightness(brightness, (char *) &palette[0]);

     }

     if (keystatus[0x43] > 0) {    // F9 - quick load
          quicksaveflag=1;
          loadgame(plr);
          quicksaveflag=0;
          updatestatusbar();
          strcpy(displaybuf,"quick save loaded");
          displaytime = 360;
     }

     if (keystatus[0x44] > 0) {    // F10 - quick save
          quicksaveflag=1;
          savegame(plr);
          quicksaveflag=0;
          updatestatusbar();
          strcpy(displaybuf,"quick saved");
          displaytime = 360;
     }

     if (keystatus[56] != 0) {     // ALT-1 to turn on sprite debugging
          if (keystatus[2] != 0) {
               debuganalyzespritesflag^=1;
               keystatus[56]=0;
               keystatus[2]=0;
          }
          else if (keystatus[3] != 0) {
               debugkeysflag^=1;
               keystatus[56]=0;
               keystatus[3]=0;
          }
     }

#if 0
     if (keystatus[0x43] > 0) {    // F9 sound controls
          if (soundtoggle == 1) {
               soundtoggle = 0;
          }
          else {
               soundtoggle = 1;
          }
     }

     if (soundtoggle == 1) {
          dosoundthing();
     }
#endif

     if (keystatus[0x58] != 0) {   // F12 save pcx
          keystatus[0x58] = 0;
          screencapture("captxxxx.pcx", keystatus[0x2A]|keystatus[0x36]);
     }
#if 0
     if (option[3] != 0) {
          if (keystatus[0x58] > 0) {
               if (mousecalibrate == 1) {
                    mousecalibrate = 0;
                    strcpy(displaybuf, "Mouse calibrate off");
                    displaytime = 40;
               }
               else {
                    mousecalibrate = 1;
                    strcpy(displaybuf, "Mouse calibrate on");
                    displaytime = 360;
               }
               keystatus[0x58] = 0;
          }
     }
#endif
#if 0
//
//  moved this to thecontrols(); in whmenu.c
//
     if (mousecalibrate == 1) {
          if (keystatus[keys[KEYRIGHT]] != 0) {
               mousxspeed++;
               if (mousxspeed > 16) {
                    mousxspeed = 16;
               }
               itoa(mousxspeed, tempbuf, 10);
               strcpy(displaybuf, "Mouse Speed x ");
               strcat(displaybuf, tempbuf);
               displaytime = 40;
          }
          if (keystatus[keys[KEYLEFT]] != 0) {
               mousxspeed--;
               if (mousxspeed < 1) {
                    mousxspeed = 1;
               }
               itoa(mousxspeed, tempbuf, 10);
               strcpy(displaybuf, "Mouse Speed x ");
               strcat(displaybuf, tempbuf);
               displaytime = 40;
          }
          if (keystatus[keys[KEYFWD]] != 0) {
               mousyspeed++;
               if (mousyspeed > 16) {
                    mousyspeed = 16;
               }
               itoa(mousyspeed, tempbuf, 10);
               strcpy(displaybuf, "Mouse Speed y ");
               strcat(displaybuf, tempbuf);
               displaytime = 40;
          }
          if (keystatus[keys[KEYBACK]] != 0) {
               mousyspeed--;
               if (mousyspeed < 1) {
                    mousyspeed = 1;
               }
               itoa(mousyspeed, tempbuf, 10);
               strcpy(displaybuf, "Mouse Speed y ");
               strcat(displaybuf, tempbuf);
               displaytime = 40;
          }
     }
#endif

     if (keystatus[0xe] > 0) {
          if (netgame == 0) {
               if (typemode == 1) {
                    typemode = 0;
                    charsperline = 0;
                    typemessageleng = 0;
               }
               else {
                    typemode = 1;
               }
               keystatus[0xe] = 0;
          }
          else {
               if (nettypemode == 1) {
                    nettypemode = 0;
                    charsperline = 0;
                    typemessageleng = 0;
                    strcpy(nettemp, "");
               }
               else {
                    nettypemode = 1;
                    typemessageleng = 0;
               }
               keystatus[0xe] = 0;
          }
     }

     if (typemode == 1) {
          typeletter();
     }

     if (nettypemode == 1) {
          nettypeletter();
     }

     if (jstickenabled) {
          jstick();
          if (keystatus[0x57] != 0) {   // recalibrate joystick (F11)
               jstickenabled = 0;
               jcalibration = 1;
          }

          if (joyx < jlowx) {
               keystatus[keys[KEYLEFT]] = 1;
          }
          else {
               keystatus[keys[KEYLEFT]] = 0;
          }

          if (joyx > jhighx) {
               keystatus[keys[KEYRIGHT]] = 1;
          }
          else {
               keystatus[keys[KEYRIGHT]] = 0;
          }

          if (joyy < jlowy) {
               v += (jmovespeed << 3);
          }
          else if (joyy > jhighy) {
               v -= (jmovespeed << 3);
          }

          for (i = 0; i < 4; i++) {
               if (((joyb & butbit[i]) != butbit[i])   // button is down
                   &&((oldjoyb & butbit[i]) == butbit[i])) {
                    keystatus[keys[joykeys[i]]] = 1;
                    if (joykeys[i] == KEYRUN) {
                         keystatus[keys[KEYFWD]] = 1;
                    }
               }
               else if (((joyb & butbit[i]) != butbit[i])   // button still down
                        &&((oldjoyb & butbit[i]) != butbit[i])) {
                    if (joykeys[i] == KEYUSE // ..one-time actions
                        || joykeys[i] == KEYJUMP
                        || joykeys[i] == KEYMAP
                        || joykeys[i] == KEYUSEP
                        || joykeys[i] == KEYCAST) {
                         keystatus[keys[joykeys[i]]] = 0;
                    }
               }
               else if (((joyb & butbit[i]) == butbit[i])   // button released
                        &&((oldjoyb & butbit[i]) != butbit[i])) {
                    keystatus[keys[joykeys[i]]] = 0;
                    if (joykeys[i] == KEYRUN) {
                         keystatus[keys[KEYFWD]] = 0;
                    }
               }
          }
          oldjoyb = joyb;
     }
     else if (jcalibration) {
          jstick();
          switch (jcalibration) {
          case 1:
               strcpy(displaybuf, "center stick press button");
               displaytime = 360;
               if (((joyb & 0xF0) != 0xF0) && ((oldjoyb & 0xF0) == 0xF0)) {
                    jctrx = joyx;
                    jctry = joyy;
                    jcalibration++;
               }
               break;
          case 2:
               strcpy(displaybuf, "top left press button");
               displaytime = 360;
               if (((joyb & 0xF0) != 0xF0) && ((oldjoyb & 0xF0) == 0xF0)) {
                    jlowx = jctrx - ((jctrx - joyx) / 4);
                    jlowy = jctry - ((jctry - joyy) / 4);
                    jcalibration++;
               }
               break;
          case 3:
               strcpy(displaybuf, "lower right press button");
               displaytime = 360;
               if (((joyb & 0xF0) != 0xF0) && ((oldjoyb & 0xF0) == 0xF0)) {
                    jhighx = jctrx + ((joyx - jctrx) / 4);
                    jhighy = jctry + ((joyy - jctry) / 4);
                    jcalibration = 0;
                    jstickenabled = 1;
               }
               break;
          }
          oldjoyb = joyb;
     }

     if (v < -201) {
          v = -201;
     }
     else if (v > 201) {
          v = 201;
     }

     v += v >> 1;

     if (s < -201) {
          s = -201;
     }
     else if (s > 201) {
          s = 201;
     }

     if (a < -112) {
          a = -112;
     }
     else if (a > 112) {
          a = 112;
     }

     if (vfx1enabled || iglassenabled || cyberenabled) {
          if (iglassenabled || cyberenabled) {
               vio_read(&yaw,&pitch,&roll);
          }
          else {
               vfx1_read(&yaw,&pitch,&roll,&puckpitch,&puckroll,&puckbuttons);
          }
          if (iglassenabled || vfx1enabled) {
               vrangle=(1024-(yaw>>4))&2047;
               vrpitch=100+(pitch/82);
          }
          else {
               vrangle=(1024+(yaw>>4))&2047;
               vrpitch=(pitch/140);
          }
          if (vrpitch < 0) {
               vrpitch=0;
          }
          else if (vrpitch > 200) {
               vrpitch=200;
          }
          plr->ang=(plr->ang+vrangle-vrangle2)&2047;
          plr->horiz=vrpitch;
          vrangle2=vrangle;
          if (configVFX1[0]) {
               for (i=0 ; i < 3 ; i++) {
                    if ((puckbuttons&(1<<i)) != 0) {
                         keystatus[keys[configVFX1[i+1]]]=1;
                         cybpuckused=1;
                    }
                    else if (cybpuckused) {
                         keystatus[keys[configVFX1[i+1]]]=0;
                    }
               }
               if (puckpitch < -1024) {
                    vel=-max(puckpitch>>6,-128);
               }
               else if (puckpitch > 1024) {
                    vel=-min(puckpitch>>6,127);
               }
               if (puckroll < -1024) {
                    svel=-max(puckroll>>5,-128);
               }
               else if (puckroll > 1024) {
                    svel=-min(puckroll>>5,127);
               }
          }
     }

     if (option[3] != 0) {
          getmousevalues(&mousx, &mousy, &bstatus);
          if (keystatus[keys[KEYSTRAFE]] != 0
              || keystatus[keys[KEYSTRAFE]+0x80] != 0) {
               i = s;
               if (mousx < 0) {
                    mousx -= (1 << mousxspeed);
               }
               else if (mousx > 0) {
                    mousx += (1 << mousxspeed);
               }
               i -= mousx;
          }
          else {
               i = a;
               if (mousx < 0) {
                    mousx -= (1 << mousxspeed);
               }
               else if (mousx > 0) {
                    mousx += (1 << mousxspeed);
               }
               i += mousx;
          }

          if (i < -512) {
               i = -512;
          }
          if (i > 511) {
               i = 511;
          }


          if (keystatus[keys[KEYSTRAFE]] != 0
              || keystatus[keys[KEYSTRAFE]+0x80] != 0) {
               s = i;
          }
          else {
               a = i;
          }

// Les START - 07/24/95  - if key KEYLOOKING is held, mouse fwd/back looks up/down
          if (keystatus[keys[KEYLOOKING]]) {
               i = plr->horiz;
               i += (mousy >> 4);
               if (i < 100 - (YDIM >> 1)) {
                    i = 100 - (YDIM >> 1);
               }
               else if (i > 100 + (YDIM >> 1)) {
                    i = 100 + (YDIM >> 1);
               }
               plr->horiz = i;
          }
          else {
               i = v;
               i -= (mousy * mousyspeed);    // Les 07/24/95
               if (i < -128) {
                    i = -128;
               }
               else if (i > 127) {
                    i = 127;
               }
               v = i;
          }
          for (i = 0; i < 3; i++) {// Les 01/17/96
               if (((bstatus & mbutbit[i]) == mbutbit[i])   // button is down
                   &&((oldbstatus & mbutbit[i]) != mbutbit[i])) {
                    keystatus[keys[mousekeys[i]]] = 1;
                    if (mousekeys[i] == KEYRUN) {
                         keystatus[keys[KEYFWD]] = 1;
                    }
               }
               else if (((bstatus & mbutbit[i]) == mbutbit[i])   // button still down
                        &&((oldbstatus & mbutbit[i]) == mbutbit[i])) {
                    if (mousekeys[i] == KEYUSE    // ..one-time actions
                        || mousekeys[i] == KEYJUMP
                        || mousekeys[i] == KEYMAP
                        || mousekeys[i] == KEYUSEP
                        || mousekeys[i] == KEYCAST) {
                         keystatus[keys[mousekeys[i]]] = 0;
                    }
               }
               else if (((bstatus & mbutbit[i]) != mbutbit[i])   // button released
                        &&((oldbstatus & mbutbit[i]) == mbutbit[i])) {
                    keystatus[keys[mousekeys[i]]] = 0;
                    if (mousekeys[i] == KEYRUN) {
                         keystatus[keys[KEYFWD]] = 0;
                    }
               }
          }
          oldbstatus = bstatus;
     }

//** Les 01/17/96   - START
     if (spaceballinitflag) {
          for (i = 0; i < 6; i++) {
               if (sbpacket.buttonState.current & (1 << i)) {
                    keystatus[keys[spacekeys[i]]] = 1;
               }
               else if (keystatus[keys[spacekeys[i]]]) {
                    keystatus[keys[spacekeys[i]]] = 0;
               }
          }
     }
     if (wingmanflag) {
          for (i = 0; i < 9; i++) {
               if ((wingmaninfo.buttons & (1 << i)) &&
                   !(oldwingmaninfo.buttons & (1 << i))) {
                    switch (wingmankeys[i]) {
                    case KEYCYCSPELLUP:
                         if (currentorb+0x3B+1 <= 0x42) {
                              keystatus[0x3B+currentorb+1]=1;
                         }
                         break;
                    case KEYCYCSPELLDN:
                         if (currentorb+0x3B-1 >= 0x3B) {
                              keystatus[0x3B+currentorb-1]=1;
                         }
                         break;
                    case KEYCYCPOTIONUP:
                         keystatus[0x1a] = 1;
                         break;
                    case KEYCYCPOTIONDN:
                         keystatus[0x1b] = 1;
                         break;
                    default:
                         keystatus[keys[wingmankeys[i]]] = 1;
                         break;
                    }
                    memmove(&oldwingmaninfo,&wingmaninfo,sizeof(oldwingmaninfo));
               }
               else {

                    switch (wingmankeys[i]) {
                    case KEYCYCSPELLUP:
                    case KEYCYCSPELLDN:
                    case KEYCYCPOTIONUP:
                    case KEYCYCPOTIONDN:
                         break;
                    default:
                         if (keystatus[keys[wingmankeys[i]]]) {
                              keystatus[keys[wingmankeys[i]]] = 0;
                         }
                         break;
                    }
               }
          }
     }
//** Les 01/17/96   - END

     i = totalclock - lockclock;
     if (i > 255) {
          i = 255;
     }

     synctics = tics = i;
     lockclock += (long) synctics;


     sprite[plr->spritenum].cstat ^= 1;
     getzrange(plr->x, plr->y, plr->z, plr->sector, &hiz, &hihit, &loz, &lohit, 128L, 0);
     sprite[plr->spritenum].cstat ^= 1;

     if ((lohit & 0xc000) == 49152) {
          if ((sprite[lohit & 4095].z - plr->z) <= (PLAYERHEIGHT << 8)) {
               onsprite = (lohit & 4095);
          }
#if 0
          if (isActor(lohit&4095)) {
               throwx = ((krand() % 512) * (long) tics *
                         (long) sintable[((sprite[onsprite].ang + 512) & 2047)]);
               throwy = ((krand() % 512) * (long) tics *
                         (long) sintable[((sprite[onsprite].ang) & 2047)]);
          }
#endif
     }
     else {
          onsprite = -1;
     }

     feetoffground = (sector[plr->sector].floorz - plr->z);

     if (keystatus[0x3A] != 0) {   // capslock
          runlock^=1;
          keystatus[0x3A]=0;
     }
     if ((keystatus[keys[KEYRUN]] != 0 || keystatus[RSHIFT] != 0)
         || v > 201 || runlock) {
          if (v > 201) {
               v = 201;
          }
          v += v >> 1;
          if (feetoffground > (32 << 8)) {
               tics += tics >> 1;
          }
     }
     if (keystatus[keys[KEYLKDN]] != 0) {
          if (plr->horiz > 100 - (YDIM >> 1)) {
               plr->horiz -= (synctics << 1);
               autohoriz = 0;
          }
     }
     else if (keystatus[keys[KEYLKUP]] != 0) {
          if (plr->horiz < 100 + (YDIM >> 1)) {
               plr->horiz += (synctics << 1);
          }
          autohoriz = 0;
     }
     if (keystatus[0xcf] != 0) {
          plr->orbactive[5] = -1;
     }
     if ((keystatus[keys[KEYFIRE]] != 0 || keystatus[keys[KEYFIRE]+0x80])
         && hasshot == 0) {   // Les 07/27/95
          if (currweaponfired == 0) {
               plrfireweapon(plr);
          }
     }
     if (keystatus[keys[KEYCAST]] > 0 && orbshot == 0 && currweaponflip == 0) { // Les 07/24/95
          if (plr->orb[currentorb] == 1) { // && selectedgun == 0) {
               if (lvlspellcheck(plr)) {
                    orbshot = 1;
                    activatedaorb(plr);
               }
          }
#if 0
          if (selectedgun != 0) {
               keystatus[WEAPONONE] = 1;
          }
#endif
          keystatus[keys[KEYCAST]] = 0; // Les 07/24/95
     }
     if (keystatus[keys[KEYUSEP]] > 0) {     // Les 07/24/95
          keystatus[keys[KEYUSEP]] = 0; // Les 07/24/95
          if (plr->potion[currentpotion] > 0) {
               usapotion(plr);
          }
     }
     if (keystatus[keys[KEYUSE]] != 0) {
          keystatus[keys[KEYUSE]] = 0;
          if (netgame) {
               netdropflag();
          }
          else {
               plruse(plr);
          }
     }
     if (keystatus[keys[KEYSHIELD]] != 0) {
          if (droptheshield == 0) {
               droptheshield = 1;
          }
          else {
               droptheshield = 0;
          }
          keystatus[keys[KEYSHIELD]] = 0;
     }
     if ((sector[plr->sector].floorpicnum != LAVA
          || sector[plr->sector].floorpicnum != SLIME
          || sector[plr->sector].floorpicnum != WATER
          || sector[plr->sector].floorpicnum != HEALTHWATER
          || sector[plr->sector].floorpicnum != ANILAVA
          || sector[plr->sector].floorpicnum != LAVA1
          || sector[plr->sector].floorpicnum != LAVA2)
         && feetoffground <= (32 << 8)) {
          v >>= 1;
     }
     if ((sector[plr->sector].floorpicnum == LAVA
          || sector[plr->sector].floorpicnum == SLIME
          || sector[plr->sector].floorpicnum == WATER
          || sector[plr->sector].floorpicnum == HEALTHWATER
          || sector[plr->sector].floorpicnum == ANILAVA
          || sector[plr->sector].floorpicnum == LAVA1
          || sector[plr->sector].floorpicnum == LAVA2)
         && plr->orbactive[5] < 0  // loz
     && plr->z >= sector[plr->sector].floorz - (plr->height << 8) - (8 << 8)) {
          goalz = loz - (32 << 8);
          switch (sector[plr->sector].floorpicnum) {
          case ANILAVA:
          case LAVA:
          case LAVA1:
          case LAVA2:
               if (plr->treasure[5] == 1) {
                    goalz = loz - (PLAYERHEIGHT << 8);
                    break;
               }
               else {
                    v -= v >> 3;
               }
               if (invincibletime > 0 || manatime > 0) {
                    break;
               }
               else {
                    if (lavasnd == -1) {
                         lavasnd = SND_PlaySound(S_FIRELOOP1, 0, 0, 0, -1);
                    }
                    healthpic(-1);
                    startredflash(10);
               }
               break;
          case WATER:
               if (plr->treasure[4] == 1) {
                    goalz = loz - (PLAYERHEIGHT << 8);
               }
               else {
                    v -= v >> 3;
               }
               break;
          case HEALTHWATER:
               if (plr->health < plr->maxhealth) {
                    healthpic(1);
                    startblueflash(5);
               }
               break;
          }
     }
     else if (plr->orbactive[5] > 0) {
          goalz = plr->z - (plr->height << 8);
          plr->hvel = 0;
     }
     else {
          goalz = loz - (plr->height << 8);
     }
     if (keystatus[keys[KEYJUMP]] != 0) {    // Les 07/24/95
          if (plr->onsomething) {
               plr->hvel -= JUMPVEL;
               plr->onsomething = 0;
          }
          keystatus[keys[KEYJUMP]] = 0;
     }
     if (keystatus[keys[KEYCROUCH]] != 0) {  // Les 07/24/95
          if (goalz < ((sector[plr->sector].floorz) - (plr->height >> 3))) {
               goalz += (48 << 8);
          }
     }
     if (keystatus[keys[KEYMAP]] != 0) {     // Les 07/24/95
          if (plr->dimension == 3) {
               plr->dimension = 2;
               strcpy(displaybuf, "map ");
               itoa(mapon, tempbuf, 10);
               strcat(displaybuf, tempbuf);
               displaytime = 720;
          }
          else {
               plr->dimension = 3;
               followmode = 0;
               followx = 0L;
               followy = 0L;
          }
          keystatus[keys[KEYMAP]] = 0;  // Les 07/24/95
     }
     if (plr->dimension == 2) {
          if (keystatus[keys[KEYZOOMO]] > 0) {    // Les 07/24/95
               if (plr->zoom > 48) {
                    plr->zoom -= (plr->zoom >> 4);
               }
          }
          if (keystatus[keys[KEYZOOMI]] > 0) {    // Les 07/24/95
               if (plr->zoom < 4096) {
                    plr->zoom += (plr->zoom >> 4);
               }
          }
          if (keystatus[0x21] > 0) {
               if (followmode == 0) {
                    strcpy(displaybuf, "Map ");
                    itoa(mapon, tempbuf, 10);
                    strcat(displaybuf, tempbuf);
                    displaytime = 360;
                    followmode = 1;
                    followx = 0L;
                    followy = 0L;
               }
               else {
                    followmode = 0;
                    followx = 0L;
                    followy = 0L;
               }
               keystatus[0x21] = 0;
          }
     }
     else if (plr->dimension == 3 && svga == 0) {
          if (keystatus[keys[KEYZOOMO]] != 0 && plr->screensize > 64) {    // Les 07/24/95
               keystatus[keys[KEYZOOMO]] = 0;
               if (plr->screensize <= 320) {
                    updatepics();
               }
               plr->screensize -= 8;
               dax = (XDIM >> 1) - (plr->screensize >> 1);
               dax2 = dax + plr->screensize - 1;
               day = (STATUSSCREEN >> 1) - (((plr->screensize * STATUSSCREEN) / XDIM) >> 1);
               day2 = day + ((plr->screensize * STATUSSCREEN) / XDIM) - 1;
               setview(dax, day, dax2, day2);
#if 0
               odax = (YDIM >> 1) - ((plr->screensize + 8) >> 1);
               odax2 = dax + (plr->screensize + 8) - 1;
               oday = (STATUSSCREEN >> 1) - ((((plr->screensize + 8) * STATUSSCREEN) / XDIM) >> 1);
               oday2 = day + (((plr->screensize + 8) * STATUSSCREEN) / XDIM) - 1;
               permanentwritesprite(0, 0, BACKGROUND, 0, odax, oday, dax - 1, oday2, 0);
               permanentwritesprite(0, 0, BACKGROUND, 0, dax2 + 1, oday, odax2, oday2, 0);
               permanentwritesprite(0, 0, BACKGROUND, 0, dax, oday, dax2, day - 1, 0);
               permanentwritesprite(0, 0, BACKGROUND, 0, dax, day2 + 1, dax2, oday2, 0);
               if (plr->screensize == XDIM) {
                    permanentwritesprite(0L, 154L, NEWSTATUSBAR, 0, 0L, 0L, XDIM - 1, YDIM - 1, 0);
                    updatepics();
               }
#endif
          }
          if (keystatus[keys[KEYZOOMI]] != 0 && plr->screensize <= XDIM) { // Les 07/24/95
               keystatus[keys[KEYZOOMI]] = 0;
               plr->screensize += 8;
               if (plr->screensize > XDIM) {
                    dax = day = 0;
                    dax2 = XDIM - 1;
                    day2 = YDIM - 1;
               }
               else {
                    dax = (XDIM >> 1) - (plr->screensize >> 1);
                    dax2 = dax + plr->screensize - 1;
                    day = (STATUSSCREEN >> 1) - (((plr->screensize * STATUSSCREEN) / XDIM) >> 1);
                    day2 = day + ((plr->screensize * STATUSSCREEN) / XDIM) - 1;
               }
               setview(dax, day, dax2, day2);
          }
          updatestatusbar();
     }
     if (plr->dimension == 3 && svga == 1) {
          if (keystatus[keys[KEYZOOMO]] != 0) {
               keystatus[keys[KEYZOOMO]] = 0;
               plr->screensize = 320;
               setview(0L, 0L, 639L, 371L);
#if 0
               rotatesprite((xdim - 640) << 15, (ydim - tilesizy[SSTATUSBAR]) << 16, 65536L, 0, SSTATUSBAR, 0, 0, 8 + 16 + 64 + 128, 0L, 0L, xdim - 1L, ydim - 1L);
               updatepics();
#endif
          }
          if (keystatus[keys[KEYZOOMI]] != 0) {
               keystatus[keys[KEYZOOMI]] = 0;
               plr->screensize = 328;
               setview(0L, 0L, 639L, 479L);
          }
          updatestatusbar();
     }

//** Les 01/16/96   - START
     if (spaceballinitflag) {
          if (SPW_IsInputEvent(0, &sbpacket)) {
               a = min(max(a + (sbpacket.sData[5] >> 2), -512), 511);
               a = max(min(a - (sbpacket.sData[4] >> 2), 511), -512);
               s = max(min(s - (sbpacket.sData[0] >> 1), 511), -512);
               v = max(min(v - (sbpacket.sData[2] >> 1), 511), -512);
          }
          if (sbpacket.sData[3] > 400) {     // look up
               if (plr->horiz < 100 + (YDIM >> 1)) {
                    plr->horiz += (synctics << 1);
                    autohoriz = 0;
               }
          }
          else if (sbpacket.sData[3] < -400) {    // look down
               if (plr->horiz > 100 - (YDIM >> 1)) {
                    plr->horiz -= (synctics << 1);
                    autohoriz = 0;
               }
          }
          if (sbpacket.sData[1] > 400) {     // jump
               if (plr->onsomething) {
                    plr->hvel -= JUMPVEL;
                    plr->onsomething = 0;
               }
          }
          else if (sbpacket.sData[1] < -400) {    // crouch
               if (goalz < ((sector[plr->sector].floorz) - (plr->height >> 3))) {
                    goalz += (24 << 8);
               }
          }
     }
//** Les 01/16/96   - END

//** Les 01/17/96   - START
     if (wingmanflag) {
          SWIFT_Get3DStatus(&wingmaninfo);
          a = max(min(a - (wingmaninfo.yaw << wingmanyawsens), 511), -512);
          s = min(max(s + (wingmaninfo.roll >> (8 - wingmanrollsens)), -512), 511);
          v = max(min(v - (wingmaninfo.pitch >> (8 - wingmanpitchsens)), 511), -512);
          if (wingmanzreverse) {
               plr->horiz = (wingmaninfo.z >> (6 - wingmanzsens));
          }
          else {
               plr->horiz = -(wingmaninfo.z >> (6 - wingmanzsens));
          }
     }
//** Les 01/17/96   - END

     onground = plr->onsomething;
     if (keystatus[keys[KEYFLYUP]]) {
          dophysics(plr, goalz, 1, v);
     }
     else if (keystatus[keys[KEYFLYDN]]) {
          dophysics(plr, goalz, -1, v);
     }
     else {
          dophysics(plr, goalz, 0, v);
     }
     if (!onground && plr->onsomething) {
          if (plr->fallz > 32768L) {
               if (rand() % 2) {
                    playsound_loc(S_PLRPAIN1 + (rand() % 2), plr->x, plr->y);
               }
               else {
                    playsound_loc(S_PUSH1 + (rand() % 2), plr->x, plr->y);
               }
               healthpic(-(plr->fallz >> 13));
               plr->fallz = 0L;    // wango
          }
          else if (plr->fallz > 8192L) {
               playsound_loc(S_BREATH1 + (rand() % 2), plr->x, plr->y);
          }
     }
     if (ihaveflag > 0) {
          v -= v >> 2;
     }
     if (v != 0 || s != 0) {
          xvect = yvect = 0;
          if (v != 0) {
               xvect = (v * (long) tics * (long) sintable[((plr->ang + 2560) & 2047)]) >> 3;
               yvect = (v * (long) tics * (long) sintable[((plr->ang + 2048) & 2047)]) >> 3;
          }
          if (s != 0) {
               xvect += ((s * (long) tics * (long) sintable[((plr->ang + 2048) & 2047)]) >> 3);
               yvect += ((s * (long) tics * (long) sintable[((plr->ang + 1536) & 2047)]) >> 3);
          }
#if 0
          xvect += throwx;
          yvect += throwy;
          if (throwx != 0L) {
               throwx >>= 1;
          }
          if (throwy != 0L) {
               throwy >>= 1;
          }
#endif
          oldposx = plr->x;
          oldposy = plr->y;
          oldposz = plr->z;
          clipmove(&plr->x, &plr->y, &plr->z, &plr->sector, xvect, yvect, 128L, 4 << 8, 4 << 8, 0);
          if (plr->sector != tempsectornum) {
               if (lavasnd != -1) {
                    switch (sector[plr->sector].floorpicnum) {
                    case ANILAVA:
                    case LAVA:
                    case LAVA1:
                    case LAVA2:
                         break;
                    default:
                         SND_StopLoop(lavasnd);
                         lavasnd = -1;
                         break;
                    }
                    sectorsounds();
               }
          }
          tempsectornum = plr->sector;

          plr->horiz -= oldhoriz;
          dist = ksqrt((plr->x - oldposx) * (plr->x - oldposx) + (plr->y - oldposy) * (plr->y - oldposy));
          if (keystatus[keys[KEYRUN]] > 0) { // Les 07/24/95
               dist >>= 2;
          }
          if (dist > 0 && feetoffground <= (plr->height << 8) || onsprite != -1) {
               if (svga == 1) {
                    oldhoriz = ((dist * sintable[(totalclock << 5) & 2047]) >> 19) >> 2;
               }
               else {
                    oldhoriz = ((dist * sintable[(totalclock << 5) & 2047]) >> 19) >> 1;
               }
               plr->horiz += oldhoriz;
          }
          else {
               oldhoriz = 0;
          }

          if (plr->horiz > 200) {
               plr->horiz = 200;
          }
          if (plr->horiz < 0) {
               plr->horiz = 0;
          }
          if (onsprite != -1 && dist > 50 && lopoint == 1 && justplayed == 0) {
               switch (sprite[onsprite].picnum) {
               case WALLARROW:
               case OPENCHEST:
               case GIFTBOX:
//                if (walktoggle) {
// BIKINI
//                    playsound_loc(S_WOOD1,(plr->x+3000),plr->y);
//                }
//                else {
// BIKINI
//                    playsound_loc(S_WOOD1,plr->x,(plr->y+3000));
//                }
                    walktoggle ^= 1;
                    justplayed = 1;
                    break;
               case WOODPLANK:     // wood planks
                    if (walktoggle) {
                         playsound_loc(S_SOFTCHAINWALK, (plr->x + 3000), plr->y);
                    }
                    else {
                         playsound_loc(S_SOFTCHAINWALK, plr->x, (plr->y + 3000));
                    }
                    walktoggle ^= 1;
                    justplayed = 1;
                    break;
               case SQUAREGRATE:   // square grating
               case SQUAREGRATE + 1:
                    if (walktoggle) {
                         playsound_loc(S_LOUDCHAINWALK, (plr->x + 3000), plr->y);
                    }
                    else {
                         playsound_loc(S_LOUDCHAINWALK, plr->x, (plr->y + 3000));
                    }
                    walktoggle ^= 1;
                    justplayed = 1;
                    break;
               case SPACEPLANK:    // spaced planks
                    if (walktoggle) {
                         playsound_loc(S_SOFTCREAKWALK, (plr->x + 3000), plr->y);
                    }
                    else {
                         playsound_loc(S_SOFTCREAKWALK, plr->x, (plr->y + 3000));
                    }
                    walktoggle ^= 1;
                    justplayed = 1;
                    break;
               case RAT:
                    playsound_loc(S_RATS1 + (rand() % 2), sprite[i].x, sprite[i].y);
                    justplayed = 1;
                    deletesprite((short) i);
                    break;
               case 1980:
               case 1981:
               case 1984:
               case 1979:
               case 1957:
               case 1955:
               case 1953:
               case 1952:
               case 1941:
               case 1940:
                    playsound_loc(S_DEADSTEP, plr->x, plr->y);
                    justplayed = 1;
                    break;
               default:
                    break;
               }
          }
          if (lopoint == 0 && oldhoriz == -2 && justplayed == 0) {
               lopoint = 1;
          }
          if (lopoint == 1 && oldhoriz != -2 && justplayed == 1) {
               lopoint = 0;
               justplayed = 0;
          }
          if (vel > 199 || vel < -199 && dist > 10) {
               runningtime += synctics;
          }
          else {
               runningtime -= synctics;
          }
          if (runningtime < -360) {
               runningtime = 0;
          }
          if (runningtime > 360) {
               SND_PlaySound(S_PLRPAIN1, 0, 0, 0, 0);
               runningtime = 0;
          }
     }
     if (a != 0) {
          plr->ang += ((a * (long) synctics) >> 4);
          plr->ang = (plr->ang + 2048) & 2047;
     }
     setsprite(plr->spritenum, plr->x, plr->y, plr->z + (plr->height << 8));
     sprite[plr->spritenum].ang = plr->ang;
     if (sector[plr->sector].ceilingz > sector[plr->sector].floorz - (8 << 8)) {
          healthpic(-10);
     }
     if (plr->health <= 0) {
          SND_CheckLoops();
          playerdead(plr);
     }
     if (keystatus[keys[KEYCNTR]] > 0) {     // home key             Les
                                             // 07/24/95
          autohoriz = 1;
          keystatus[keys[KEYCNTR]] = 0;
     }
     if (autohoriz == 1) {
          autothehoriz(plr);
     }
     singleshot(bstatus);
     weaponchange();
}

void
autothehoriz(struct player * plr)
{

     if (plr->horiz < 100) {
          plr->horiz += synctics << 1;
     }
     if (plr->horiz > 100) {
          plr->horiz -= synctics << 1;
     }
     if (plr->horiz >= 90 && plr->horiz <= 110) {
          autohoriz = 0;
     }

}

void
nettypeletter(void)
{
     int  i;

     strcpy(nettemp, "");

     if (typemessageleng <= 40) {
          for (i = 0; i < 128; i++) {
               if (keystatus[i] > 0) {
                    nettemp[typemessageleng] = scantoasc[i];
                    typemessageleng++;
                    nettemp[typemessageleng] = '\0';
                    keystatus[i] = 0;
               }
          }
          printext256(0L, 0L, 31, -1, strupr(nettemp), 1);
     }

}

void
typeletter(void)
{

     int  i,j,exit=0;

     keystatus[0x1c] = keystatus[0x9c] = 0;

     for (i = 0; i < 128; i++) {
          keystatus[i] = 0;
     }

     memset(tempbuf,0,sizeof(tempbuf));

     while (!exit) {
          if (typemessageleng <= 10) {
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
                                   tempbuf[typemessageleng] = fancy[j];
                                   typemessageleng++;
                                   tempbuf[typemessageleng] = '\0';
                                   keystatus[i] = 0;
                              }
                              else {
                                   keystatus[i] = 0;
                              }
                         }
                    }
               }
          }
          if (keystatus[1] > 0) {
               exit = 1;
               keystatus[1] = 0;
          }
          if (keystatus[0x1c] > 0 || keystatus[0x9c] > 0) {
               exit = 2;
               keystatus[0x1c] = keystatus[0x9c] = 0;
          }
          strcpy(displaybuf, tempbuf);
          fancyfontscreen(18, 24, THEFONT, displaybuf);
          nextpage();
     }

     if (exit == 2) {
          keystatus[0x1c] = keystatus[0x9c] = 0;
     }

     checkcheat();
     lockclock = totalclock;
     typemode = 0;
     typemessageleng = 0;

}

extern
int  helmettime,
     shieldtype,
     justteleported;

extern
long scarytime,
     scarysize;

int  mflag,
     nobreakflag,
     show2dobjectsflag,
     show2dmapflag,
     spikeme;

void
checkcheat(void)
{

     int  i,j,y=24;
     struct player *plr;

     plr = &player[pyrn];

     strupr(displaybuf);

     if (strcmp(displaybuf, "WEAPONS") == 0) {
          for (i = 0 ; i < MAXWEAPONS ; i++) {
               plr->weapon[i] = 1;
          }
          plr->ammo[0] = 100;
          plr->ammo[1] = 45;       // DAGGER
          plr->ammo[2] = 55;       // SHORT SWORD
          plr->ammo[3] = 50;       // MACE
          plr->ammo[4] = 80;       // SWORD
          plr->ammo[5] = 100;      // BATTLE AXE
          plr->ammo[6] = 50;       // BOW
          plr->ammo[7] = 40;       // PIKE
          plr->ammo[8] = 250;      // TWO HANDED
          plr->ammo[9] = 50;       // HALBERD
          currweapon = selectedgun = 4;
          updatepics();
     }
     else if (strcmp(displaybuf, "POTIONS") == 0) {
          for (i = 0; i < MAXPOTIONS; i++) {
               plr->potion[i] = 9;
          }
          updatepics();
     }
     else if (strcmp(displaybuf, "SPELLS") == 0) {
          for (i = 0 ; i < MAXNUMORBS ; i++) {
               plr->orb[i] = 1;
               plr->orbammo[i] = 9;
          }
          updatepics();
     }
     else if (strcmp(displaybuf, "MARKETING") == 0) {
          if (godmode == 1) {
               godmode = 0;
          }
          else {
               godmode = 1;
               plr->health = 0;
               healthpic(200);
#if 0
               for (i = 0 ; i < MAXWEAPONS ; i++) {
                    plr->weapon[i] = 3;
               }
#endif
               plr->ammo[0] = 100;
               plr->ammo[1] = 45;       // DAGGER
               plr->ammo[2] = 55;       // SHORT SWORD
               plr->ammo[3] = 50;       // MACE
               plr->ammo[4] = 80;       // SWORD
               plr->ammo[5] = 100;      // BATTLE AXE
               plr->ammo[6] = 50;       // BOW
               plr->ammo[7] = 40;       // PIKE
               plr->ammo[8] = 250;      // TWO HANDED
               plr->ammo[9] = 50;       // HALBERD
               currweapon = selectedgun = 4;
               for (i = 0 ; i < MAXNUMORBS ; i++) {
                    plr->orb[i] = 1;
                    plr->orbammo[i] = 9;
               }
               for (i = 0 ; i < MAXPOTIONS ; i++) {
                    plr->potion[i] = 9;
               }
               plr->armor = 150;
               plr->armortype = 3;
               plr->lvl = 7;
               plr->maxhealth = 200;
               for (i = 0 ; i < MAXTREASURES ; i++) {
                    plr->treasure[i] = 1;
               }
               nobreakflag=1;
               updatepics();
          }
     }
     else if (strcmp(displaybuf, "KILLME") == 0) {
          plr->health = 0;
          plr->potion[0] = 0;
          playerdead(plr);
          updatepics();
     }
     else if (strcmp(displaybuf, "SCARE") == 0) {
          currentorb = 0;
          plr->orbammo[currentorb] += 1;
          orbshot = 1;
          activatedaorb(plr);
          updatepics();
     }
     else if (strcmp(displaybuf, "NIGHTVISION") == 0) {
          currentorb = 1;
          plr->orbammo[currentorb] += 1;
          orbshot = 1;
          activatedaorb(plr);
          updatepics();
     }
     else if (strcmp(displaybuf, "FREEZE") == 0) {
          currentorb = 2;
          plr->orbammo[currentorb] += 1;
          orbshot = 1;
          activatedaorb(plr);
          updatepics();
     }
     else if (strcmp(displaybuf, "MAGICARROW") == 0) {
          currentorb = 3;
          plr->orbammo[currentorb] += 1;
          orbshot = 1;
          activatedaorb(plr);
          updatepics();
     }
     else if (strcmp(displaybuf, "OPENDOOR") == 0) {
          currentorb = 4;
          plr->orbammo[currentorb] += 1;
          orbshot = 1;
          activatedaorb(plr);
          updatepics();
     }
     else if (strcmp(displaybuf, "FLY") == 0) {
          currentorb = 5;
          plr->orbammo[currentorb] += 1;
          orbshot = 1;
          activatedaorb(plr);
          updatepics();
     }
     else if (strcmp(displaybuf, "FIREBALL") == 0) {
          currentorb = 6;
          plr->orbammo[currentorb] += 1;
          orbshot = 1;
          activatedaorb(plr);
          updatepics();
     }
     else if (strcmp(displaybuf, "NUKE") == 0) {
          currentorb = 7;
          plr->orbammo[currentorb] += 1;
          orbshot = 1;
          activatedaorb(plr);
          updatepics();
     }
     else if (strcmp(displaybuf, "HEALTH") == 0) {
          i=currentpotion;
          currentpotion=0;
          plr->potion[currentpotion] += 1;
          usapotion(plr);
          currentpotion=i;
          updatepics();
     }
     else if (strcmp(displaybuf, "STRENGTH") == 0) {
          i=currentpotion;
          currentpotion=1;
          plr->potion[currentpotion] += 1;
          usapotion(plr);
          currentpotion=i;
          updatepics();
     }
     else if (strcmp(displaybuf, "CUREPOISON") == 0) {
          i=currentpotion;
          currentpotion=2;
          plr->potion[currentpotion] += 1;
          usapotion(plr);
          currentpotion=i;
          updatepics();
     }
     else if (strcmp(displaybuf, "RESISTFIRE") == 0) {
          i=currentpotion;
          currentpotion=3;
          plr->potion[currentpotion] += 1;
          usapotion(plr);
          currentpotion=i;
          updatepics();
     }
     else if (strcmp(displaybuf, "INVIS") == 0) {
          i=currentpotion;
          currentpotion=4;
          plr->potion[currentpotion] += 1;
          usapotion(plr);
          currentpotion=i;
          updatepics();
     }
     else if (strcmp(displaybuf, "KEYS") == 0) {
          plr->treasure[14] = 1;
          plr->treasure[15] = 1;
          plr->treasure[16] = 1;
          plr->treasure[17] = 1;
          updatepics();
     }
     else if (strcmp(displaybuf, "PENTAGRAM") == 0) {
          plr->treasure[8] = 1;
     }
     else if (strcmp(displaybuf, "ARMOR") == 0) {
          plr->armortype = 3;
          armorpic(150);
          updatepics();
     }
     else if (strcmp(displaybuf, "HEROTIME") == 0) {
          helmettime = 7200;
     }
     else if (strcmp(displaybuf, "SHIELD") == 0) {
          droptheshield = 0;
          shieldtype = 1;
          shieldpoints = 100;
     }
     else if (strcmp(displaybuf, "SHIELD2") == 0) {
          droptheshield = 0;
          shieldtype = 2;
          shieldpoints = 200;
     }
     else if (strcmp(displaybuf, "NOBREAK") == 0) {
          nobreakflag^=1;
     }
     else if (strcmp(displaybuf, "SHOWOBJECTS") == 0) {
          show2dobjectsflag^=1;
     }
     else if (strcmp(displaybuf, "SHOWMAP") == 0) {
          show2dmapflag^=1;
     }
     else if (strcmp(displaybuf, "ENCHANT") == 0) {
          for (i = 0 ; i < MAXWEAPONS ; i++) {
               plr->weapon[i] = 3;
          }
     }
#if 0
     else if (strcmp(displaybuf, "KILLALL") == 0) {
          for (i = 0 ; i < MAXSPRITES ; i++) {
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
               case KATIE:
               case KATIEAT:
                    if (sprite[i].owner != 4096) {
                         newstatus((short)i, DIE);
                    }
                    break;
               }
          }
     }
#endif
     else if (strcmp(displaybuf, "INTRACORP") == 0) {
          if (svga == 1) {
               keystatus[0x39] = 0;
               keystatus[1] = 0;
               SND_Sound(S_PICKUPFLAG);
               permanentwritesprite(0, 0, STHEORDER, 0, 0, 0, 639, 239, 0);
               permanentwritesprite(0, 240, STHEORDER + 1, 0, 0, 240, 639, 479, 0);
               nextpage();
               i = 0;
               while (!i) {
                    if (keystatus[0x39] > 0 || keystatus[1] > 0)
                         i = 1;
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
               i = 0;
               while (!i) {
                    if (keystatus[0x39] > 0 || keystatus[1] > 0)
                         i = 1;
               }
               keystatus[0x39] = 0;
               keystatus[1] = 0;
          }
          mflag=1;
     }
     else if (strcmp(displaybuf, "SPIKEME") == 0) {
          spikeme=1;
     }
     else if (strcmp(displaybuf, "EXPERIENCE") == 0) {
          score(10000);
     }
     else if (strcmp(displaybuf, "SCAREME") == 0) {
          scarytime = 180;
          scarysize = 30;
          SND_PlaySound(S_SCARYDUDE, 0, 0, 0, 0);
     }
     else if (strcmp(displaybuf, "LEVEL1") == 0) {
          mapon = 1;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL2") == 0) {
          mapon = 2;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL3") == 0) {
          mapon = 3;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL4") == 0) {
          mapon = 4;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL5") == 0) {
          mapon = 5;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL6") == 0) {
          mapon = 6;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL7") == 0) {
          mapon = 7;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL8") == 0) {
          mapon = 8;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL9") == 0) {
          mapon = 9;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL10") == 0) {
          mapon = 10;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL11") == 0) {
          mapon = 11;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL12") == 0) {
          mapon = 12;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL13") == 0) {
          mapon = 13;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL14") == 0) {
          mapon = 14;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LEVEL15") == 0) {
          mapon = 15;
          justteleported = 1;
          vel = 0;
          angvel = 0;
          svel = 0;
          playertorch = 0;
          playsound_loc(S_WARP, plr->x, plr->y);
          loadnewlevel(mapon);
          warpfxsprite(plr->spritenum);
          plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
          plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
          SND_CheckLoops();
     }
     else if (strcmp(displaybuf, "LIST") == 0) {
          for (i=0 ; i < MAXSPRITES ; i++) {
               j=0;
               switch (sprite[i].picnum) {
               case KOBOLD:
               case KOBOLDATTACK:
                    sprintf(displaybuf,"KOBOLD: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case DEVIL:
               case DEVILATTACK:
                    sprintf(displaybuf,"DEVIL: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case IMP:
               case IMPATTACK:
                    sprintf(displaybuf,"IMP: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case MINOTAUR:
               case MINOTAURATTACK:
                    sprintf(displaybuf,"MINOTAUR: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case SKELETON:
               case SKELETONATTACK:
                    sprintf(displaybuf,"SKELETON: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case GRONHAL:
               case GRONHALATTACK:
                    sprintf(displaybuf,"GRONHAL: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case GRONMU:
               case GRONMUATTACK:
                    sprintf(displaybuf,"GRONMU: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case GRONSW:
               case GRONSWATTACK:
                    sprintf(displaybuf,"GRONSW: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case DEMON:
                    sprintf(displaybuf,"DEMON: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case GUARDIAN:
               case GUARDIANATTACK:
                    sprintf(displaybuf,"GUARDIAN: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case WILLOW:
                    sprintf(displaybuf,"WILLOW: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case NEWGUYSTAND:
                    sprintf(displaybuf,"NEWGUYSTAND: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case NEWGUYKNEE:
                    sprintf(displaybuf,"NEWGUYKNEE: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case NEWGUY:
                    sprintf(displaybuf,"NEWGUY: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case NEWGUYCAST:
                    sprintf(displaybuf,"NEWGUYCAST: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case NEWGUYBOW:
                    sprintf(displaybuf,"NEWGUYBOW: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case NEWGUYMACE:
                    sprintf(displaybuf,"NEWGUYMACE: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case NEWGUYPUNCH:
                    sprintf(displaybuf,"NEWGUYPUNCH: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case KURTSTAND:
                    sprintf(displaybuf,"KURTSTAND: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case KURTKNEE:
                    sprintf(displaybuf,"KURTKNEE: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case KURTAT:
                    sprintf(displaybuf,"KURTAT: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case KURTPUNCH:
                    sprintf(displaybuf,"KURTPUNCH: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case KURTREADY:
                    sprintf(displaybuf,"KURTREADY: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case GONZOCSW:
               case GONZOCSWAT:
                    sprintf(displaybuf,"GONZOCSW: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case GONZOCHMAT:
                    sprintf(displaybuf,"GONZOCHMAT: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case GONZOGSW:
               case GONZOGSWAT:
                    sprintf(displaybuf,"GONZOGSW: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case GONZOGHM:
               case GONZOGHMAT:
                    sprintf(displaybuf,"GONZOGHM: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case GONZOGSH:
               case GONZOGSHAT:
                    sprintf(displaybuf,"GONZOGSH: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               case KATIE:
               case KATIEAT:
                    sprintf(displaybuf,"WITCH: SIZE X%d Y%d",
                            sprite[i].xrepeat,sprite[i].yrepeat);
                    j=1;
                    break;
               }
               if (j) {
                    if (cansee(plr->x, plr->y, plr->z, plr->sector,
                          sprite[i].x, sprite[i].y,
                          sprite[i].z - (tilesizy[sprite[i].picnum] << 7),
                          sprite[i].sectnum) == 1) {
                         if (sprite[i].owner != 4096) {
                              fancyfontscreen(0, y, THEFONT, displaybuf);
                              y+=12;
                         }
                    }
               }
          }
          nextpage();
          keystatus[0x01]=0;
          while (keystatus[0x01] == 0);
          keystatus[0x01]=0;
     }
     else if (mflag) {
          for (i=0 ; i < strlen(displaybuf) ; i++) {
               displaybuf[i]+=1;
          }
          if (strcmp(displaybuf, "BNZ") == 0) {
               score(10000);
          }
          else if (strcmp(displaybuf, "SVCFO") == 0) {
               scarytime = 180;
               scarysize = 30;
               SND_PlaySound(S_SCARYDUDE, 0, 0, 0, 0);
          }
          else if (strcmp(displaybuf, "SBGBFM") == 0) {
               for (i=0 ; i < numsectors ; i++) {
                    for (j=sector[i].wallptr ;
                         j < sector[i].wallptr + sector[i].wallnum ; j++) {
                              wall[j].picnum = 316;
                    }
               }
               SND_playSound(S_JUDYDIE);
          }
          else if (strcmp(displaybuf, "NJZPTIJ") == 0) {
               spikeme=1;
          }
          mflag=0;
     }
     strcpy(displaybuf, "");
}


void
typecheat(char ch)
{

     int  i;
     int  j;

     charsperline = 40;

     for (i = 0; i <= typemessageleng; i += charsperline) {
          for (j = 0; j < charsperline; j++)
               tempbuf[j] = typemessage[i + j];
          if (typemessageleng < i + charsperline) {
               tempbuf[(typemessageleng - i)] = '-';
               tempbuf[(typemessageleng - i) + 1] = 0;
          }
          else
               tempbuf[charsperline] = 0;
     }

     strcpy(displaybuf, tempbuf);
     displaytime = 360;


}

void
dosoundthing(void)
{

     musiclevel = (wMIDIVol >> 3);
     digilevel = (wDIGIVol >> 11);

     if (keystatus[keys[KEYFWD]] > 0) {
          soundcontrol++;
     }
     if (keystatus[keys[KEYBACK]] > 0) {
          soundcontrol--;
     }
     if (soundcontrol > 1) {
          soundcontrol = 1;
     }
     if (soundcontrol < 0) {
          soundcontrol = 0;
     }
     switch (soundcontrol) {
     case 0:
          if (keystatus[keys[KEYLEFT]] > 0) {
               musiclevel--;
               if (musiclevel < 0) {
                    musiclevel = 0;
                    strcpy(displaybuf, "Music ");
                    itoa(musiclevel, tempbuf, 10);
                    strcat(displaybuf, tempbuf);
                    displaytime = 10;
               }
               else {
                    strcpy(displaybuf, "Music ");
                    itoa(musiclevel, tempbuf, 10);
                    strcat(displaybuf, tempbuf);
                    displaytime = 10;
                    SND_Mixer(1, musiclevel);
               }
               break;
          }
          else if (keystatus[keys[KEYRIGHT]] > 0) {
               musiclevel++;
               if (musiclevel > 16) {
                    musiclevel = 16;
                    strcpy(displaybuf, "Music ");
                    itoa(musiclevel, tempbuf, 10);
                    strcat(displaybuf, tempbuf);
                    displaytime = 10;
               }
               else {
                    strcpy(displaybuf, "Music ");
                    itoa(musiclevel, tempbuf, 10);
                    strcat(displaybuf, tempbuf);
                    displaytime = 10;
                    SND_Mixer(1, musiclevel);
               }
               break;
          }
          else {
               strcpy(displaybuf, "Music ");
               itoa(musiclevel, tempbuf, 10);
               strcat(displaybuf, tempbuf);
               displaytime = 10;
          }
          break;
     case 1:
          if (keystatus[keys[KEYLEFT]] > 0) {
               digilevel--;
               if (digilevel < 0) {
                    digilevel = 0;
                    strcpy(displaybuf, "Sounds ");
                    itoa(digilevel, tempbuf, 10);
                    strcat(displaybuf, tempbuf);
                    displaytime = 10;
               }
               else {
                    strcpy(displaybuf, "Sounds ");
                    itoa(digilevel, tempbuf, 10);
                    strcat(displaybuf, tempbuf);
                    displaytime = 10;
                    SND_Mixer(0, digilevel);
                    SND_Sound(S_LOUDCHAINWALK);
               }
               break;
          }
          else if (keystatus[keys[KEYRIGHT]] > 0) {
               digilevel++;
               if (digilevel > 16) {
                    digilevel = 16;
                    strcpy(displaybuf, "Sounds ");
                    itoa(digilevel, tempbuf, 10);
                    strcat(displaybuf, tempbuf);
                    displaytime = 10;
               }
               else {
                    strcpy(displaybuf, "Sounds ");
                    itoa(digilevel, tempbuf, 10);
                    strcat(displaybuf, tempbuf);
                    displaytime = 10;
                    SND_Mixer(0, digilevel);
                    SND_Sound(S_LOUDCHAINWALK);
               }
               break;
          }
          else {
               strcpy(displaybuf, "Sounds ");
               itoa(digilevel, tempbuf, 10);
               strcat(displaybuf, tempbuf);
               displaytime = 10;
          }
          break;
     }

}

void
updatestatusbar(void)
{
     struct player *plr;

     plr=&player[pyrn];
     if (svga == 0) {
          if (plr->screensize < 320) {
               permanentwritesprite(0L, 0L, BACKGROUND, 0,
                                    0L, 0L, 319L, 199L, 0);
          }
          if (plr->screensize <= 320) {
               permanentwritesprite(0L, 154L, NEWSTATUSBAR, 0,
                                    0L, 0L, 319L, 199L, 0);
          }
     }
     else {
          if (plr->screensize <= 320) {
               rotatesprite((xdim - 640) << 15,
                            (ydim - tilesizy[SSTATUSBAR]) << 16,
                            65536L, 0, SSTATUSBAR, 0, 0, 8 + 16 + 64 + 128,
                            0L, 0L, xdim - 1L, ydim - 1L);
          }
     }
     updatepics();
}

void
debugchain(void)
{
     short i;
     char buf[8];

     if (!debugkeysflag) {
          return;
     }
     strcpy(tempbuf,"KEYS: ");
     for (i=0 ; i < 256 ; i++) {
          if (keystatus[i]) {
               sprintf(buf,"%X ",i);
               strcat(tempbuf,buf);
          }
     }
     printext256(0L,0L,31,-1,tempbuf,1);
}

