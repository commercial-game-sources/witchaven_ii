#define NUMOPTIONS 12
#define XDIM    320
#define YDIM    200
#define MAXMIRRORS 64

#define KEYFIFOSIZ 64
extern void (__interrupt __far *oldkeyhandler)();
extern void __interrupt __far keyhandler(void);

#ifdef WHAVEN
long sparksx;
long sparksy;
long sparksz;
int attacktheme=0;
int batsnd=-1;
int cartsnd=-1;
int lavasnd=-1;

volatile char keystatus[256], keyfifo[KEYFIFOSIZ], keyfifoplc, keyfifoend;
volatile char readch, oldreadch, extended, keytemp;
#else
extern long sparksx;
extern long sparksy;
extern long sparksz;

extern int attacktheme;
extern int batsnd;
extern int cartsnd;
extern int lavasnd;

extern volatile char keystatus[], keyfifo[], keyfifoplc, keyfifoend;
extern volatile char readch, oldreadch, extended, keytemp;
#endif

#ifdef WHAVEN
short mirrorwall[MAXMIRRORS], mirrorsector[MAXMIRRORS], mirrorcnt;
short floormirrorsector[64], floormirrorcnt;
int godmode=0;
signed char ceilingshadearray[MAXSECTORS];
signed char floorshadearray[MAXSECTORS];
signed char wallshadearray[MAXWALLS];
int  playertorch=0;
int  thefirstcone;
int dogchewtime;
point3d osprite[MAXSPRITESONSCREEN];
long   smoothratio;
volatile char keystatus[256];
int shieldpoints=0;
int poisoned=0;
int poisontime=0;
int  shockme=-1;
int  cyberenabled=0;
int  svgahealth=0;
int  victor=0;
int  autohoriz=0;
int  svga=0;
char boardname[80];
char tempboard[80];
char loadgamename[80];
int   delaycnt,
      engineinitflag,
      timerinitflag,
      videoinitflag;
#else
extern short mirrorwall[], mirrorsector[], mirrorcnt;
extern short floormirrorsector[], floormirrorcnt;
extern int godmode;
extern signed char ceilingshadearray[];
extern signed char floorshadearray[];
extern signed char wallshadearray[];
extern int playertorch;
extern int thefirstcone;
extern int dogchewtime;
extern point3d osprite[];
extern long   smoothratio;
extern volatile char keystatus[];
extern int shieldpoints;
extern int poisoned;
extern int poisontime;
extern int  shockme;
extern int  cyberenabled;
extern int  svgahealth;
extern int  victor;
extern int  autohoriz;
extern int  svga;
extern char boardname[];
extern char tempboard[];
extern char loadgamename[];
extern int  delaycnt,
            engineinitflag,
            timerinitflag,
            videoinitflag;
#endif

#ifdef SVGA
long synctics;
long globhiz, globloz, globhihit, globlohit;
long chainxres[4] = {256,320,360,400};
long chainyres[11] = {200,240,256,270,300,350,360,400,480,512,540};
long vesares[7][2] = {320,200,640,400,640,480,800,600,1024,768,
                                      1280,1024,1600,1200};
char option[NUMOPTIONS] = {0,0,0,0,0,0,1,0};

#else
extern long synctics;
extern long globhiz, globloz, globhihit, globlohit;
extern long chainxres[];
extern long chainyres[];
extern long vesares[7][2];
extern char option[];
extern char keys[]; 
#endif
