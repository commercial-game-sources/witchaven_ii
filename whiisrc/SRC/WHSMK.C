/***************************************************************************
 *    WHSMK.C  - Smacker code for Witchaven II
 *
 *                                                     03/01/96 Les Bird
 ***************************************************************************/

#include "smack.h"
#include "svga.h"
#include "icorp.h"
#include <memcheck.h>

#define   MAXRADBUFFS         64

#pragma pack(1);

char *levelname[16] = {
     "antechamber of asmodeus",
     "halls of ragnoth",
     "lokis tomb",
     "forsaken realm",
     "eye of midian",
     "dungeon of disembowlment",
     "stronghold of chaos",
     "jaws of venom",
     "descent into doom",
     "hack n sniff",
     "straits of perdition",
     "plateau of insanity",
     "crypt of decay",
     "mausoleum of madness",
     "gateway into oblivion",
     "lungs of hell"
};

Smack *smk;

struct radbuftype {
     long cache_ptr;
     long cache_length;
     char cache_lock;
};

struct radbuftype radbuf[MAXRADBUFFS];

extern
unsigned vixen;

extern
long frameplace;

extern
char palshifted;

extern
int  treasurescnt,
     treasuresfound,
     killcnt,
     kills,
     expgained,
     bonus;

extern
int  mapon;

extern
long qsetmode;

RCFUNC
void PTR4 *RADLINK
radmalloc(u32 numbytes)
{
     int  i;

     for (i = 0; i < MAXRADBUFFS; i++) {
          if (radbuf[i].cache_ptr == 0L) {
               break;
          }
     }
     if (i == MAXRADBUFFS) {
          crash("no more radbuff pointers");
     }
     radbuf[i].cache_lock = 200;
     radbuf[i].cache_length = numbytes;
     allocache(&(radbuf[i].cache_ptr), radbuf[i].cache_length,
               &(radbuf[i].cache_lock));
     if (radbuf[i].cache_ptr == 0L) {
          crash("radmalloc failed on");
     }
     return((void PTR4 *) radbuf[i].cache_ptr);
}

RCFUNC
void RADLINK
radfree(void PTR4 * ptr)
{
     int  i;

     for (i = 0; i < MAXRADBUFFS; i++) {
          if (radbuf[i].cache_ptr == (long) ptr) {
               radbuf[i].cache_lock = 1;
               break;
          }
     }
}

void
smkinit(unsigned int digifh)
{
     if (digifh != -1) {
          SmackSoundUseSOS3(digifh, 120);
     }
     else {
          SmackSoundUseSOS3(0, 120);
     }
     SmackTimerSetup();
}

#include  <ctype.h>
#pragma   off  (argsused)
void
texttobuf(char *buffer,short x,short y,short fontstart,char *text,short pal)
{
     short          letter,i,offset,sl;
     char           ltr,space;
     short          xl,yl,termxl,termyl;
     char           *buf=NULL;
     char           *pic=NULL;

     /* ASSUMES BUF of 320 x 200 DIMENSION */

     offset=0;
     sl=strlen(text);
     for( i=0; i<sl; i++ ) {
          space=0;
          ltr=toupper(text[i]);
          if( (x+offset) > 320 ) {
               break;
          }
          if( ltr == ' ' ) {
               offset+=6;
               continue;
          }
          if( (ltr >= 'A') && (ltr <= 'Z') ) {
               letter=ltr-'A';
          }
          else {
               if( (ltr >= '0') && (ltr <= '9') ) {
                    letter=ltr-'0'+26;                    
               }
               else {
                    letter=0;
               }
          }
          letter+=fontstart;
          if( waloff[letter] == 0 ) loadtile(letter);
          termxl=tilesizx[letter];     
          termyl=tilesizy[letter];
          yl=0;
          yloop:
               xl=0;
               buf=( char *)(buffer+(yl*320)+(y*320));
               pic=( char *)(waloff[letter]);
               xloop:          
                    if( *(pic+(xl*termyl)+yl) != 255 ) {
                         *(buf+x+offset+xl)=*(pic+(xl*termyl)+yl);
                    }
                    xl++;
                    if( xl < termxl ) goto xloop;
               yl++;
               if( yl < termyl ) goto yloop;
          offset+=termxl;
     }
}
#pragma on (argsused)

void
smkplayseq(int s)
{
     int  frames=0;
     long dax,
          dax2,
          day,
          day2;
     long bonus,i,killp,rating,treap;
     char *ratings[]={
          "poor",
          "average",
          "good",
          "perfect"
     },*smklist[]={
          "intro.smk",
          "ending1.smk",
          "stairs.smk",
          "ending2.smk",
          "ending3.smk"
     };
     struct radbuftype   smkbuf;
     struct player *plr;

     sprintf(tempbuf,"%c:\\smk\\%s",vixen,smklist[s]);   
     if (access(tempbuf,F_OK) != 0) {
          strcpy(tempbuf,smklist[s]);
     }
     if ((s == 0 || s == 1 || s == 3 || s == 4) && SoundMode) {
          smk=SmackOpen(tempbuf,SMACKTRACK1,SMACKAUTOEXTRA);
     }
     else {
          smk=SmackOpen(tempbuf,0,SMACKAUTOEXTRA);
     }
     if (smk) {
          smkbuf.cache_ptr=0L;
          smkbuf.cache_length=(320*200);
          smkbuf.cache_lock=200;
          allocache(&(smkbuf.cache_ptr),smkbuf.cache_length,
                    &(smkbuf.cache_lock));
          if( smkbuf.cache_ptr == 0L ) return;
          plr=&player[0];
          switch (s) {
          case 0:
          case 1:
          case 3:
          case 4:
               SND_StopMusic();
               break;
          case 2:
               if (kills > killcnt) {
                    kills = killcnt;
               }
               killp = (kills * 100) / (killcnt+1);
               if (treasuresfound > treasurescnt) {
                    treasuresfound = treasurescnt;
               }
               treap = (treasuresfound * 100) / (treasurescnt+1); 
               rating = (killp + treap) / 2;
               if (rating >= 95) {
                    rating = 3;
               }
               else if (rating >= 70) {
                    rating = 2;
               }
               else if (rating >= 40) {
                    rating = 1;
               }
               else {
                    rating = 0;
               }
               bonus = rating * 500;
               plr->score += bonus;
               break;
          }
          keystatus[1] = 0;
          keystatus[28] = 0;
          keystatus[57] = 0;
          SVGADetect(0);
          if( !SVGASetup(320,200) ) {
               goto done;
          }
          SmackToBuffer(smk,0L,0L,320L,200L,(void *)smkbuf.cache_ptr,0); 
          finishpaletteshifts();
          SVGASetGraph();
          while (1) {
               if (smk->NewPalette) {
                    SVGASetPalette(smk->Palette);
               }
               SmackDoFrame(smk);
               SmackNextFrame(smk);
               if (s == 2) {
                    texttobuf(( void *)smkbuf.cache_ptr,10,13,THEFONT,levelname[mapon],0);
                    texttobuf(( void *)smkbuf.cache_ptr,10, 31, THEFONT, "level conquered", 0);
                    texttobuf(( void *)smkbuf.cache_ptr,10, 64, THEFONT, "enemies killed", 0);
                    sprintf(tempbuf, "%d of %d", kills, killcnt);
                    texttobuf(( void *)smkbuf.cache_ptr,160 + 48 + 14, 64, THEFONT, tempbuf, 0);

                    texttobuf(( void *)smkbuf.cache_ptr,10, 64 + 18, THEFONT, "treasures found", 0);
                    sprintf(tempbuf, "%d of %d",treasuresfound, treasurescnt);
                    texttobuf(( void *)smkbuf.cache_ptr,160 + 48 + 14, 64 + 18, THEFONT, tempbuf, 0);

                    texttobuf(( void *)smkbuf.cache_ptr,10, 64 + (18 * 2), THEFONT,
                              "experience gained", 0);
                    sprintf(tempbuf, "%d", expgained + bonus);
                    texttobuf(( void *)smkbuf.cache_ptr,160 + 48 + 14, 64 + (18 * 2), THEFONT,
                              tempbuf, 0);

                    texttobuf(( void *)smkbuf.cache_ptr,10, 64 + (18 * 3), THEFONT,
                              "rating", 0);
                    texttobuf(( void *)smkbuf.cache_ptr,160 + 48 + 14, 64 + (18 * 3), THEFONT,
                              ratings[rating], 0);

                    texttobuf(( void *)smkbuf.cache_ptr,10, 64 + (18 * 4), THEFONT,
                              "bonus", 0);
                    sprintf(tempbuf, "%d", bonus);
                    texttobuf(( void *)smkbuf.cache_ptr,160 + 48 + 14, 64 + (18 * 4), THEFONT,
                              tempbuf, 0);
               }
               SVGABlit(( void *)smkbuf.cache_ptr,0,0,320,200,320,0,0);
               while (SmackWait(smk)) {
                   #ifdef ALLOWSCREENCAPTURE 
                    if( (svga == 0) && (keystatus[0x58] != 0) ) { 
                         keystatus[0x58] = 0;
                         screencapture("captxxxx.pcx", keystatus[0x2A]|keystatus[0x36]);
                    }
                   #endif
                    if (keystatus[1] || keystatus[57] || keystatus[28]) {
                         goto done;
                    }
               }
               if (keystatus[1] || keystatus[57] || keystatus[28]) {
                    goto done;
               }
               if (s == 1 || s == 3 || s == 4) {
                    frames++;
                    if (frames == smk->Frames) {
                         goto done;
                    }
               }
          }
done:
          smkbuf.cache_lock=1;
          SmackClose(smk);
          for (i = 0 ; i < MAXRADBUFFS ; i++) {
               radbuf[i].cache_lock = 1;
          }
          qsetmode=201;
          setgamemode();
          if( plr->dimension == 3 && svga == 0 ) {
               dax = (XDIM >> 1) - (plr->screensize >> 1);
               dax2 = dax + plr->screensize - 1;
               day = (STATUSSCREEN >> 1) - (((plr->screensize * STATUSSCREEN) / XDIM) >> 1);
               day2 = day + ((plr->screensize * STATUSSCREEN) / XDIM) - 1;
               setview(dax, day, dax2, day2);
               updatestatusbar();
          }
          if( plr->dimension == 3 && svga == 1 ) {
               if( plr->screensize == 320 ) {
                    setview(0L, 0L, 639L, 371L);
               }
               else {
                    setview(0L, 0L, 639L, 479L);
               }
               updatestatusbar();
          }
     }
}

