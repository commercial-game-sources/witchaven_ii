#define  SND_CALLER
#define  GAME
#include <malloc.h>
#include "sos.h"
#include "sosm.h"
#include "icorp.h"
#include <memcheck.h>

int  readfile(int, char *, long);
#pragma aux readfile =\
	"mov  ah,0x3f",\
	"int 0x21",\
	parm [ebx] [edx] [ecx]\
	modify [eax];


int  flag = 0;

extern int displaytime;

//JSA SPOOGE
extern int musiclevel;
extern int digilevel;
extern int loopinstuff;
extern int enchantedsoundhandle;

// local data for hmi INI
static PSTR szHexNumbers = "0123456789ABCDEF";
static WORD wMultiplier[] = {1, 16, 256, 4096, 65536, 1048576, 16777216, 268435456};
// local function prototypes
WORD hmiINIHex2Decimal(PSTR szHexValue);
WORD hmiINIGetHexIndex(BYTE bValue);

//TEMP!!
volatile unsigned joetime;

//krapo

#define  NUMLEVELS      17
#define  SONGSPERLEVEL  4

typedef struct {
     HANDLE hMIDISong;             // if != -1 then the file is playing
     long offset;
     long cache_ptr;
     long cache_length;
     char cache_lock;
}    songbuffertype;
songbuffertype sngbuf[NUMLEVELS][SONGSPERLEVEL];
songbuffertype *sngbufptr[NUMLEVELS][SONGSPERLEVEL];

_SOS_MIDI_INIT_SONG hmiSongStructs[SONGSPERLEVEL];

extern int mapon;

int  oldsong = 0;

//****************
//   CODE STARTS *
//****************


//
//   Temporary method for storing a song in memory.

//mark

BOOL 
loadlevelsongs(int which)
{
     int  i,
          bytesread = 0;

     if (!MusicMode)
          return (0);

     for (i = 0; i < SONGSPERLEVEL; i++) {
          // set lock and try to allocate cache
          sngbufptr[which][i]->cache_lock = 200;
          allocache(&(sngbufptr[which][i]->cache_ptr), sngbufptr[which][i]->cache_length, &(sngbufptr[which][i]->cache_lock));

#if USINGALLOCATEPERMANENTTILE
          sngbufptr[which][i]->cache_ptr = (void *) malloc(sngbufptr[which][i]->cache_length);
          if (sngbufptr[which][i]->cache_ptr == _NULL)
#endif
               if (sngbufptr[which][i]->cache_ptr == 0L) {
                    sngbufptr[which][i]->cache_lock = 0x00;
                    return (0);
               }

          // read data from file
          lseek(hSongFile, sngbufptr[which][i]->offset, SEEK_SET);
          bytesread = read(hSongFile, (void *) sngbufptr[which][i]->cache_ptr, sngbufptr[which][i]->cache_length);
          if (bytesread != sngbufptr[which][i]->cache_length) {
               sngbufptr[which][i]->cache_ptr = 0L;
               sngbufptr[which][i]->cache_lock = 0x00;
               return (0);
          }

          // reset song structure
          // memset( hmiSongStructs[i], 0, sizeof( _SOS_MIDI_INIT_SONG ) );

          // set up song structure
          hmiSongStructs[i].lpSongData = (LPSTR) sngbufptr[which][i]->cache_ptr;
          if (sosMIDIInitSong(&(hmiSongStructs[i]), &sSOSTrackMap[i], &(sngbufptr[which][i]->hMIDISong))) {
               // return error "startsong() : sosMIDIInitSong failed!"
               return (0);
          }
          hmiSongStructs[i].lpSongCallback = sosMIDISongCallback;
     }

     oldsong = which;

     return (1);
}


VOID
_far sosMIDISongCallback(WORD hSong)
{
     int  i;

     if (attacktheme) {
          for (i = 0; i < SONGSPERLEVEL; i++) {
               if (hSong == sngbufptr[oldsong][i]->hMIDISong) {
                    sosMIDIResetSong(sngbufptr[oldsong][i]->hMIDISong, &hmiSongStructs[i]);
               }
          }
          startsong(rand() % 2);
          attacktheme = 0;
     }

}


BOOL 
startsong(WORD which)
{
     int  i;

     if (!MusicMode)
          return (0);

     // check to see if songs are playing and stop/fade if necessary
     for (i = 0; i < SONGSPERLEVEL; i++) {
          if (!sosMIDISongDone(sngbufptr[oldsong][i]->hMIDISong)) {
               sosMIDIStopSong(sngbufptr[oldsong][i]->hMIDISong);
               // sosMIDIUnInitSong(sngbufptr[oldsong][i]->hMIDISong);
               // sngbufptr[oldsong][i]->hMIDISong=-1;
          }
     }


     if ((wError = sosMIDIStartSong(sngbufptr[oldsong][which]->hMIDISong))) {
          // return error "startsong() : sosMIDIStartSong failed!"
          return (0);
     }
     // sprintf(displaybuf,"playing song %d", which);
     // displaytime=250;

     if (which < 2)
          attacktheme = 0;

     return (1);
}
//krapo

void 
dolevelmusic(int which)
{
     int  i,
          temp;

     if (!MusicMode)
          return;

     if (which < 0 || which > NUMLEVELS-1) {
          which=0;
     }

     // check to see if songs are playing and stop/fade if necessary
     for (i = 0; i < SONGSPERLEVEL; i++) {
          if (!sosMIDISongDone(sngbufptr[oldsong][i]->hMIDISong)) {
               sosMIDIStopSong(sngbufptr[oldsong][i]->hMIDISong);
               // sosMIDIUnInitSong(sngbufptr[oldsong][i]->hMIDISong);
               // sngbufptr[oldsong][i]->hMIDISong=-1;
          }
          if (which != 16)
               sosMIDIUnInitSong(sngbufptr[oldsong][i]->hMIDISong);
          sngbufptr[oldsong][i]->hMIDISong = -1;
          sngbufptr[oldsong][i]->cache_lock = 0;
     }

     loadlevelsongs(which);
     startsong(0);
}


VOID
SND_MenuMusic(void)
{
     // 15 levels the sixteenth set is the menu song!
     dolevelmusic(NUMLEVELS - 1);
}


//
//                         INTERNAL ROUTINES
//

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_InitSOSTimer() : Registers the timerevent() function to be        //
//          called by SOS.                                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_InitSOSTimer(VOID)
{
     if ((wError = sosTIMERRegisterEvent(120, (VOID(far *)) timerevent, &hTimerT_ClockHandle))) {
          sosTIMERUnInitSystem(0);
          sosDIGIUnInitSystem();
          crash("TIMER FAILURE!");
     }
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    timerevent() : Handles timer functions originally in game.c           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
_far _loadds timerevent(VOID)
{
     joetime++;
     totalclock++;
     keytimerstuff();


}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    sosDIGISampleCallback(WORD,WORD,WORD) : Call back routine from SOS    //
//          Digi functions. wCallSource indicates which process is complete //
//          hSample is the particular sample handle.                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
_far cdecl sosDIGISampleCallback(WORD wDriverHandle, WORD wCallSource, WORD hSample)
{
//
// recording driver will also use this callback for future!
// will have to switch wDriverHandle as well...
//

     switch (wCallSource) {
     case _SAMPLE_DONE:
          for (wIndex = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++)
               if (hSample == SampleRay[wIndex].SOSHandle)
                    break;
          sSOSSampleData[wIndex].wLoopCount = 0;
          sSOSSampleData[wIndex].dwSampleSize = 0;
          SampleRay[wIndex].SOSHandle = -1;
          SampleRay[wIndex].playing = 0;
          SampleRay[wIndex].priority = 0;
          SampleRay[wIndex].number = -1;
          break;

     }

}




VOID
SND_SetupTables(VOID)
{
     int  i,
          j,
          listindex = 0;


     if (SoundMode) {
          hSoundFile = open("JOESND", O_RDONLY | O_BINARY);
          if (hSoundFile == -1) {
               crash("COULDN'T OPEN JOESND!");
          }
          DigiList = malloc(0x1000);
          if (DigiList == NULL) {
               crash("malloc failed for DigiList");
          }
          lseek(hSoundFile, -4096L, SEEK_END);
          readfile(hSoundFile, (void *) FP_OFF(DigiList), 4096);
     }

     if (MusicMode) {
          if (sMIDIHardware.wPort == 0x388) {
               hSongFile = open("F_SONGS", O_RDONLY | O_BINARY);
          }
          else {
               hSongFile = open("W_SONGS", O_RDONLY | O_BINARY);
          }

          if (hSongFile == -1) {
               crash("COULDN'T OPEN SONGS!");
          }
          SongList = malloc(0x1000);
          if (SongList == NULL) {
               crash("malloc failed for SongList");
          }
          lseek(hSongFile, -4096L, SEEK_END);
          readfile(hSongFile, (void *) FP_OFF(SongList), 4096);
     }

     // setup offset and length of all songs
     for (i = 0; i < NUMLEVELS; i++) {
          for (j = 0; j < SONGSPERLEVEL; j++) {
               sngbufptr[i][j] = &sngbuf[i][j];
               sngbufptr[i][j]->hMIDISong = -1;
               sngbufptr[i][j]->offset = (SongList[listindex * 3] * 4096L);
               sngbufptr[i][j]->cache_ptr = 0L;
               sngbufptr[i][j]->cache_length = (WORD) (SongList[listindex * 3 + 1]);
               sngbufptr[i][j]->cache_lock = 0x00;
               listindex++;
          }
     }


     return;
}



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//    SND_LoadMidiIns(void) : Read melodic and percussive banks into mem.   //
//          Conditional : GENERAL MIDI and WAVE synths don't need patches   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
void
SND_LoadMidiIns(void)
{
     static WORD wLength;

//JSA_DEMO check port address to verify FM device

     if ((MusicMode == _STANDARD_MUSIC || MusicMode == _DIG_MIDI_MUSIC) &&
         sMIDIHardware.wPort == 0x388) {
//              (wMIDIDeviceID != _MIDI_MPU_401 && wMIDIDeviceID != _MIDI_MT_32
//                  && wMIDIDeviceID!=_MIDI_AWE32) ) {
          hMiscHandle = open("melodic.bnk", O_RDONLY);
          if (hMiscHandle == -1)
               crash("MELODIC BANK FILE FAILED!");
          m_bnkptr = malloc(0x152c);
          if (m_bnkptr == NULL) {
               crash("malloc failed for m_bnkptr");
          }
          read(hMiscHandle, (void *) FP_OFF(m_bnkptr), 0x152c);
          close(hMiscHandle);
          if ((wError = sosMIDISetInsData(hSOSDriverHandles[MIDI], m_bnkptr, 0x00)))
               crash("BAD SetInsData MEL!");

          hMiscHandle = open("drum.bnk", O_RDONLY);
          if (hMiscHandle == -1)
               crash("PERCUSSIVE BANK FILE FAILED!");
          d_bnkptr = malloc(0x152c);
          if (d_bnkptr == NULL) {
               crash("malloc failed for d_bnkptr");
          }
          read(hMiscHandle, (void *) FP_OFF(d_bnkptr), 0x152c);
          close(hMiscHandle);
          if ((wError = sosMIDISetInsData(hSOSDriverHandles[MIDI], d_bnkptr, 0x00)))
               printf("BAD SetInsData DRUM!");
     }

     if (MusicMode == _DIG_MIDI_MUSIC) {
          hMiscHandle = open("test.dig", O_RDONLY);
          if (hMiscHandle == -1)
               crash("DIGI_MIDI FILE FAILED!");
          wLength = lseek(hMiscHandle, 0L, SEEK_END);
          lseek(hMiscHandle, 0L, SEEK_SET);
          digi_bnkptr = malloc(wLength);
          if (digi_bnkptr == NULL) {
               crash("malloc failed for digi_bnkptr");
          }
          read(hMiscHandle, (void *) FP_OFF(digi_bnkptr), wLength);
          close(hMiscHandle);
          if ((wError = sosMIDISetInsData(hSOSDriverHandles[DIG_MIDI], digi_bnkptr, 0x00)))
               crash("BAD SetInsData digmidi!");
     }
}

//
//                         PUBLIC ROUTINES
//



VOID
SND_DoBuffers(void)
{


     for (wIndex = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++) {
          sSOSSampleData[wIndex].lpSamplePtr = (LPSTR) malloc((long) 55000);
          if (sSOSSampleData[wIndex].lpSamplePtr == _NULL)
               crash("Could Not get a Sound Buffer!!");
     }

}

VOID
SND_UnDoBuffers(void)
{

     for (wIndex = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++) {
          if (sSOSSampleData[wIndex].lpSamplePtr != NULL)
               free((void *) FP_OFF(sSOSSampleData[wIndex].lpSamplePtr));
     }

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_Startup() : Initialize all SOS Drivers and start timer         //
//             service.                                                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

extern
_SOS_HARDWARE sDIGISettings;
extern
_SOS_MIDI_HARDWARE sMIDISettings;
extern
WORD wDIGIDevice,
     wMIDIDevice;

VOID
SND_Startup(VOID)
{

     if (SD_Started)
          return;

     wDIGIDeviceID = wDIGIDevice;
     wMIDIDeviceID = wMIDIDevice;
     memmove(&sDIGIHardware, &sDIGISettings, sizeof(sDIGIHardware));
     memmove(&sMIDIHardware, &sMIDISettings, sizeof(sMIDIHardware));

//GET Volume values
     wMIDIVol = (musiclevel << 3);
     wDIGIVol = (digilevel << 11);


//TIMERSYSTEM

     wError = sosTIMERInitSystem(_TIMER_DOS_RATE, _SOS_DEBUG_NORMAL);
     if (wError != _ERR_NO_ERROR)
          crash("Problem with Timer SysInit!");

     SND_InitSOSTimer();

     hSOSDriverHandles[DIGI] = -1;
     hSOSDriverHandles[MIDI] = -1;

     if (wDIGIDeviceID == 0xffffffff)
          SoundMode = _OFF;
     if (wMIDIDeviceID == 0xffffffff)
          MusicMode = _OFF;

//DIGISYSTEM

     if (SoundMode == _ON) {       // && wDIGIDeviceID!=0xffffffff) {
          wError = sosDIGIInitSystem(_NULL, _SOS_DEBUG_NORMAL);
          if (wError != _ERR_NO_ERROR)
               crash("Error on Digi SysInit!");
     }

//MIDISYSTEM
     if (MusicMode == _STANDARD_MUSIC) {
          if (wError = sosMIDIInitSystem(_NULL, _SOS_DEBUG_NORMAL)) {
               sosTIMERUnInitSystem(0);
               sosDIGIUnInitSystem();
               crash("Could not Init Midi System!");
          }
     }

//MIDIDRIVER
     if (MusicMode == _STANDARD_MUSIC) {
          sSOSMIDIInitDriver.lpDriverMemory = _NULL;

          if ((wError = sosMIDIInitDriver(wMIDIDeviceID, &sMIDIHardware,
                            &sSOSMIDIInitDriver, &hSOSDriverHandles[MIDI]))) {
               sosMIDIUnInitSystem();
               sosDIGIUnInitSystem();
               sosTIMERUnInitSystem(0);
               crash("Could not Init Midi Driver!\n");
          }

          SND_LoadMidiIns();
          sosMIDIEnableChannelStealing(_FALSE);
          for (wIndex = 0; wIndex < MAX_ACTIVE_SONGS; wIndex++)
               hSOSSongHandles[wIndex] = 0x7fff;

          Midi_Loaded = _TRUE;
          sosMIDISetMasterVolume(wMIDIVol);
     }


//DIGIDRIVER
     if (SoundMode == _ON) {       // && wDIGIDeviceID!=0xffffffff) {

          if ((wError = sosDIGIInitDriver(wDIGIDeviceID, &sDIGIHardware,
                                &sSOSInitDriver, &hSOSDriverHandles[DIGI]))) {
               sosTIMERUnInitSystem(0);
               sosDIGIUnInitSystem();
               crash("Could not initialize Digi Driver!");
          }
          if ((wError = sosTIMERRegisterEvent(_SOS_FILL_TIMER_RATE, sSOSInitDriver.lpFillHandler,
                                              &hTimerDig_FillHandle))) {
               sosDIGIUnInitDriver(hSOSDriverHandles[DIGI], _TRUE, _TRUE);
               sosTIMERUnInitSystem(0);
               sosDIGIUnInitSystem();
               crash("Could not register lpFillHandler!");
          }
          Digi_Loaded = _TRUE;

          sosDIGISetMasterVolume(hSOSDriverHandles[DIGI], wDIGIVol);

          for (wIndex = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++)
               SampleRay[wIndex].number = -1;

          // RAF NOV 17
          // ADDED SMK STUFF
          // smkinit(hSOSDriverHandles[DIGI]);
     }

     // read in offset page list's
     SND_SetupTables();
     SD_Started = _TRUE;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_Shutdown() : Un-Initialize all SOS Drivers and releases        //
//             timer service(s).                                            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_Shutdown(VOID)
{
     int  i;

     if (!SD_Started)
          return;

     if (SoundMode && Digi_Loaded) {
          SND_DIGIFlush();

          sosTIMERRemoveEvent(hTimerDig_FillHandle);
          flag = sosDIGIUnInitDriver(hSOSDriverHandles[DIGI], _TRUE, _TRUE);
          if (hSoundFile != -1)
               close(hSoundFile);
          if (hLoopFile != -1)
               close(hLoopFile);
          if (hSongFile != -1)
               close(hSongFile);
          if (DigiList != NULL)
               free(DigiList);
          if (LoopList != NULL)
               free(LoopList);
          Digi_Loaded = _FALSE;
     }

     if (MusicMode && Midi_Loaded) {
          for (i = 0; i < SONGSPERLEVEL; i++) {
               if (!sosMIDISongDone(sngbufptr[oldsong][i]->hMIDISong)) {
                    sosMIDIStopSong(sngbufptr[oldsong][i]->hMIDISong);
                    // sosMIDIUnInitSong(sngbufptr[oldsong][i]->hMIDISong);
                    // sngbufptr[oldsong][i]->hMIDISong=-1;
               }
               sngbufptr[oldsong][i]->cache_lock = 0;
               sosMIDIUnInitSong(sngbufptr[oldsong][i]->hMIDISong);
               sngbufptr[oldsong][i]->hMIDISong = -1;
          }


          sosMIDIUnInitDriver(hSOSDriverHandles[MIDI], _TRUE);
          if (m_bnkptr != NULL)
               free(m_bnkptr);
          if (d_bnkptr != NULL)
               free(d_bnkptr);
          if (digi_bnkptr != NULL)
               free(digi_bnkptr);
          if (lpMIDISong != NULL)
               free((PSTR) FP_OFF(lpMIDISong));
          Midi_Loaded = _FALSE;
     }

     sosMIDIUnInitSystem();

     sosDIGIUnInitSystem();

     sosTIMERRemoveEvent(hTimerT_ClockHandle);

     sosTIMERUnInitSystem(0);

     SD_Started = _FALSE;

}

void
SND_StopMusic(void)
{
     int  i;

     if (MusicMode && Midi_Loaded) {
          for (i = 0; i < SONGSPERLEVEL; i++) {
               if (!sosMIDISongDone(sngbufptr[oldsong][i]->hMIDISong)) {
                    sosMIDIStopSong(sngbufptr[oldsong][i]->hMIDISong);
                    // sosMIDIUnInitSong(sngbufptr[oldsong][i]->hMIDISong);
                    // sngbufptr[oldsong][i]->hMIDISong=-1;
               }
               sngbufptr[oldsong][i]->cache_lock = 0;
               sosMIDIUnInitSong(sngbufptr[oldsong][i]->hMIDISong);
               sngbufptr[oldsong][i]->hMIDISong = -1;
          }
     }
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//       SND_Mixer(WORD wSource,WORD wVolume) : Change Music or SFX Volume  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
VOID
SND_Mixer(WORD wSource, WORD wVolume)
{
     if (wSource == MIDI) {
          if (MusicMode) {
               wMIDIVol = (wVolume << 3);
               sosMIDISetMasterVolume((BYTE) wMIDIVol);
          }
     }
     else {
          if (SoundMode) {
               wDIGIVol = (wVolume << 11);
               sosDIGISetMasterVolume(hSOSDriverHandles[DIGI], wDIGIVol);
          }
     }
}


//WORD
int
SND_PlaySound(WORD sound, long x, long y, WORD Pan, WORD loopcount)
{
     WORD wVol,
          flag = 0;
     long sqrdist;
     long prioritize;

     if (!SoundMode)
          return (0);
     // return((WORD)0);


     prioritize = DigiList[(sound * 3) + 2];

     if (((x == 0) && (y == 0)) || ((player[pyrn].x == x) && (player[pyrn].y == y))) {
          wVol = 0x7fff;
          Pan = 0;
     }
     else {
          sqrdist = labs(player[pyrn].x - x) + labs(player[pyrn].y - y);
          if (sqrdist < 1500)
               wVol = 0x7fff;
          else if (sqrdist > 8500)
               wVol = 0x1f00;
          else
               wVol = 39000 - (sqrdist << 2);
     }


     if (sound == S_STONELOOP1) {
          for (wIndex = 0, flag = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++)
               if (sound == SampleRay[wIndex].number)
                    return (0);
     }

     for (wIndex = 0, flag = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++)
          if (!SampleRay[wIndex].playing) {
               flag = 1;
               break;
          }

     // if(!flag && prioritize<9)               //none available low prio
     if (!flag)
          return (0);

     else if (!flag) {             // none avail but high prio
          for (wIndex = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++) {
               if (SampleRay[wIndex].priority < 9 && sSOSSampleData[wIndex].wLoopCount != -1) {
                    if (!sosDIGISampleDone(hSOSDriverHandles[DIGI], SampleRay[wIndex].SOSHandle) &&
                        (sSOSSampleData[wIndex].dwSampleSize != 0)) {
                         sosDIGIStopSample(hSOSDriverHandles[DIGI], SampleRay[wIndex].SOSHandle);
                         sSOSSampleData[wIndex].wLoopCount = 0;
                         sSOSSampleData[wIndex].dwSampleSize = 0;
                         SampleRay[wIndex].SOSHandle = -1;
                         SampleRay[wIndex].playing = 0;
                         SampleRay[wIndex].priority = 0;
                         SampleRay[wIndex].number = -1;
                         break;
                    }
               }
          }
     }


     sSOSSampleData[wIndex].dwSampleSize = (WORD) DigiList[(sound * 3) + 1];

     SeekIndex = (DigiList[(sound * 3) + 0] * 4096);

     lseek(hSoundFile, SeekIndex, 0x00);
     memset((void *) FP_OFF(sSOSSampleData[wIndex].lpSamplePtr), '0', 55000);
     read(hSoundFile, (void *) FP_OFF(sSOSSampleData[wIndex].lpSamplePtr), sSOSSampleData[wIndex].dwSampleSize);

     if (loopcount)
          sSOSSampleData[wIndex].wLoopCount = loopcount;

     if (Pan)
          Pan = ((getangle(player[pyrn].x - x, player[pyrn].y - y) + (2047 - player[pyrn].ang)) % 2047) >> 6;

//HILES this will put the enchanted weapons inthe right speaker
//     if( sound == S_ENCHANTLOOP1 || sound == S_ENCHANTLOOP2 )
//          sSOSSampleData[wIndex].wSamplePanLocation = 0xe000;
//     else
          sSOSSampleData[wIndex].wSamplePanLocation = PanArray[Pan];
     sSOSSampleData[wIndex].wVolume = wVol;
     SampleRay[wIndex].SOSHandle = sosDIGIStartSample(hSOSDriverHandles[DIGI], &sSOSSampleData[wIndex]);
     SampleRay[wIndex].x = x;
     SampleRay[wIndex].y = y;
     SampleRay[wIndex].playing = 1;
     SampleRay[wIndex].number = sound;
     SampleRay[wIndex].priority = prioritize;
     ActiveSampleBits |= (0x01 << wIndex);

     return (SampleRay[wIndex].SOSHandle);

}


WORD
SND_Sound(WORD sound)
{
     static WORD handle;

     if (!SoundMode)
          return (-1);
     return (SND_PlaySound(sound, 0, 0, 0, 0));

}

VOID
SND_CheckLoops(void)
{

     // special case loops
     if (cartsnd != -1) {
          SND_StopLoop(cartsnd);
          cartsnd = -1;
     }
     if (lavasnd != -1) {
          SND_StopLoop(lavasnd);
          lavasnd = -1;
     }
     if (batsnd != -1) {
          SND_StopLoop(batsnd);
          batsnd = -1;
     }

     if (enchantedsoundhandle != -1) {
          SND_StopLoop(enchantedsoundhandle);
     }
     


     // ambient sound array
     for (wIndex = 0; wIndex < MAX_AMB_SOUNDS; wIndex++) {
          if (ambsoundarray[wIndex].hsound != -1) {
               SND_StopLoop(ambsoundarray[wIndex].hsound);
               ambsoundarray[wIndex].hsound = -1;
          }
     }

     SND_DIGIFlush();

}

VOID
SND_StopLoop(WORD which)
{

     if (!SoundMode)
          return;

     for (wIndex = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++)
          if (which == SampleRay[wIndex].SOSHandle)
               break;

     sosDIGIStopSample(hSOSDriverHandles[DIGI], SampleRay[wIndex].SOSHandle);
     sSOSSampleData[wIndex].wLoopCount = 0;
     SampleRay[wIndex].SOSHandle = -1;
     SampleRay[wIndex].playing = 0;
     SampleRay[wIndex].number = -1;

}


VOID
SND_DIGIFLush(void)
{
     if (!SoundMode)
          return;

     for (wIndex = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++) {
          if (SampleRay[wIndex].playing)
               sosDIGIStopSample(hSOSDriverHandles[DIGI], SampleRay[wIndex].SOSHandle);
          // if( sSOSSampleData[wIndex].lpSamplePtr != NULL )
          // free((void *)FP_OFF(sSOSSampleData[wIndex].lpSamplePtr));
          SampleRay[wIndex].SOSHandle = -1;
          SampleRay[wIndex].playing = 0;
          SampleRay[wIndex].number = -1;
          ActiveSampleBits |= (0x01 << wIndex);
     }
}



//
//    Sound Location Stuff
//


VOID
SND_UpdateSoundLoc(WORD which, WORD Volume, WORD Pan)
{

     gVol = Volume;
     gPan = sosDIGISetPanLocation(hSOSDriverHandles[DIGI], SampleRay[which].SOSHandle, PanArray[Pan]);
     sosDIGISetSampleVolume(hSOSDriverHandles[DIGI], SampleRay[which].SOSHandle, Volume);

}



// JOE START functions called often from external modules

//JSA 3/20/95
//
//    note: soundnum=sound to play; xplc is x location of source; yplc is y loc
//

void
playsound_loc(int soundnum, long xplc, long yplc)
{
     long sqrdist;
     unsigned wVol,
          wPan;

     SND_PlaySound(soundnum, xplc, yplc, 1, 0);
}

void
updatesound_loc(void)
{
     int  wIndex;
     unsigned wVol,
          wPan;
     long sqrdist;

     if (!SoundMode)
          return;

     for (wIndex = 0; wIndex < MAX_ACTIVE_SAMPLES; wIndex++)
          if (SampleRay[wIndex].playing && SampleRay[wIndex].x && SampleRay[wIndex].y) {
               if (sSOSSampleData[wIndex].dwSampleSize != 0) {
                    sqrdist = labs(player[pyrn].x - SampleRay[wIndex].x) +
                         labs(player[pyrn].y - SampleRay[wIndex].y);

                    if (sqrdist < 1500)
                         wVol = 0x7fff;
                    else if (sqrdist > 8500)
                         wVol = 0x1f00;
                    else
                         wVol = 39000 - (sqrdist << 2);

                    wPan = ((getangle(player[pyrn].x - SampleRay[wIndex].x, player[pyrn].y - SampleRay[wIndex].y) + (2047 - player[pyrn].ang)) % 2047) >> 6;
                    SND_UpdateSoundLoc(wIndex, wVol, wPan);
                    // sprintf(displaybuf,"%dVol %x Pan %x Dist
                    // %ld",SampleRay[wIndex].number,wVol,wPan,sqrdist);
                    // displaytime=100;
               }

          }

}

// Location Stuff End
