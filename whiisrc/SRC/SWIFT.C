//---------------------------------------------------------------------------
// SWIFT.C   - SWIFT services module
//
// Copyright (C) 1993 Logitech, Inc.   All Rights Reserved.
//
// PVCS modification history is at the end of the file.
//--------------------------------------------------------------------------

// This file has been tested in the following development environments:
// 1. Watcom C 9.01, generating 32-bit flat-model code for the Rational
//    DOS/4GW extender.
// 2. Borland C++ 3.10, generating 16-bit small model code for DOS
// 3. Microsoft C 6.00A, generating 16-bit small model code for DOS
// For other environments, you may have to modify this module, or
// the file SWIFTCFG.H - q.v.

//------------------------------------------------------- standard includes -
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

//------------------------------------------------------------ local includes

#include "swiftcfg.h"         // compilation options for this module

#include "swift.h"       // external interface for this module
#include <memcheck.h>

//----------------------------------------------------------------- constants
// #define TRACE


#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

#define DPMI_INT    0x31
#define MOUSE_INT   0x33

#define DOSMEMSIZE  64   // enough for any SWIFT structure

//---------------------------------------------------------- type definitions

//---------------------------------------------------------- global variables

/* Active flag: TRUE after successful init and before termination */
static int fActive;

/* type of SWIFT device */
static int nAttached = SDM_NO_DEVICE;

// raf added static cause it was
// in defined in WHVFX1.C as global variable
//union REGS regs;
//struct SREGS sregs;

static union REGS regs;
static struct SREGS sregs;

#ifdef PROTECTED_MODE
static short selector;               // selector of DOS memory block
static short segment;           // segment of DOS memory block
static void far *pdosmem;       // pointer to DOS memory block

// DPMI real mode interrupt structure
static struct rminfo {
    long di;
    long si;
    long bp;
    long reserved_by_system;
    long bx;
    long dx;
    long cx;
    long ax;
    short flags;
    short es, ds, fs, gs, ip, cs, sp, ss;
} RMI;
#endif // PROTECTED_MODE

static SWIFT_EVENTHANDLER client_handler;

//--------------------------------------------- forward function declarations

#ifdef PROTECTED_MODE
void MouseInt(struct rminfo *prmi);     // Mouse Interrupt
static void far *allocDOS(unsigned nbytes, short *pseg, short *psel);
static void freeDOS(short sel);
#endif

//------------------------------------------------------ function definitions

/* Test for presence of SWIFT extensions and SWIFT device               */
/* Returns 1 (TRUE) if SWIFT features are available, 0 otherwise.       */
/* Remember to call SWIFT_Terminate() if SWIFT_Initialize succeeds!     */

int SWIFT_Initialize(void)
{
   SWIFT_StaticData sdBuf;
   int fSwift = FALSE;

   if (fActive) {
	 /* SWIFT extensions already active, must have returned TRUE! */
	 #ifdef TRACE
	 printf("SWIFT_Initialize: Redundant, already active.\n");
	 printf("SWIFT_Initialize: returns TRUE\n");
	 #endif
	 return TRUE;
   }

	nAttached = SDM_NO_DEVICE;

   if (_dos_getvect(0x33) == NULL) {
	 /* No mouse driver loaded */
	 #ifdef TRACE
	 printf("SWIFT_Initialize: No mouse driver loaded.\n");
	 printf("SWIFT_Initialize: returns FALSE\n");
	 #endif
		return FALSE;
	}

	// Reset the mouse and driver
	AX(regs) = 0;
#ifdef __386__
	int386( 0x33, &regs, &regs);
#else
	int86( 0x33, &regs, &regs);
#endif
	if (AX(regs) == 0) {
		// no mouse
		#ifdef TRACE
		printf("SWIFT_Initialize: No pointing device attached.\n");
		printf("SWIFT_Initialize: returns FALSE\n");
		#endif
		return FALSE;
	}

	#ifdef TRACE
	AX(regs) = 36; // Get Mouse Information
	BX(regs) = 0xffff;
	CX(regs) = 0xffff;
	DX(regs) = 0xffff;
	#ifdef __386__
		int386( 0x33, &regs, &regs);
	#else
		int86( 0x33, &regs, &regs);
	#endif
	printf("SWIFT_Initialize: driver version %d.%02d\n", regs.h.bh, regs.h.bl);
	printf("SWIFT_Initialize: %s mouse using IRQ %d\n",
			 (regs.h.ch==1) ? "bus" :
			 (regs.h.ch==2) ? "serial" :
			 (regs.h.ch==3) ? "inport" :
			 (regs.h.ch==4) ? "PS/2" :
									"unknown", regs.h.cl);
	#endif

#ifdef PROTECTED_MODE

	// allocate a DOS real-mode buffer
	pdosmem = allocDOS(DOSMEMSIZE, &segment, &selector);
	if (!pdosmem) {
		#ifdef TRACE
		printf("SWIFT_Initialize: DOS Alloc failed!\n");
		printf("SWIFT_Initialize: returns FALSE\n");
		#endif
		return FALSE;
	}
#endif

	if (SWIFT_GetStaticDeviceInfo(&sdBuf)) {
		// SWIFT device supported and attached
		fSwift = TRUE;
	}

	if (!fSwift) {
		// SWIFT functions not present
		#ifdef TRACE
		printf("SWIFT_Initialize: no SWIFT support in mouse driver.\n");
		#endif
	} else if (sdBuf.deviceType == SWIFT_DEV_NONE) {
		#ifdef TRACE
		printf("SWIFT_Initialize: no SWIFT device connected.\n");
		#endif
	} else {
		nAttached = SWIFT_GetAttachedDevice ();
		#ifdef TRACE
		printf("SWIFT_Initialize: ");
		switch (nAttached) {
			case SDM_CYBERMAN:
				printf("CyberMan %d.%02d connected.\n",
						 sdBuf.majorVersion, sdBuf.minorVersion);
				break;
			case SDM_WINGMAN_WARRIOR:
				printf("WingMan Warrior %d.%02d connected.\n",
						 sdBuf.majorVersion, sdBuf.minorVersion);
				break;
			default:
				printf("Unknown SWIFT device (type %d) connected.\n",
						 nAttached);
				break;
		} /* switch */
		#endif
		fActive = TRUE;
	}

	if (!fActive) {
		// activation of SWIFT module failed for some reason
#ifdef PROTECTED_MODE
		if (pdosmem) {
			// if DOS buffer was allocated, free it
			freeDOS(selector);
			pdosmem = 0;
		}
#endif
	}

	#ifdef TRACE
	printf("SWIFT_Initialize: returns %s.\n", (fActive ? "TRUE" : "FALSE"));
	#endif
	return fActive;
} /* end SWIFT_Initialize */


void SWIFT_Terminate(void)
/* Free resources required for SWIFT support.  If SWIFT_Initialize has  */
/* not been called, or returned FALSE, this function does nothing.      */
/* SWIFT_Terminate should always be called at some time after a call to */
/* SWIFT_Initialize has returned TRUE.                                  */
{
	#ifdef TRACE
	printf("SWIFT_Terminate called.\n");
	#endif

	if (fActive) {
		// disable event handler
		SWIFT_SetMouseEventHandler((SWIFT_EVENTHANDLER)NULL, 0);

		#ifdef PROTECTED_MODE
		/* free DOS buffer */
		if (pdosmem) {
			freeDOS(selector);
			pdosmem = 0;
		}
		#endif // PROTECTED_MODE
		fActive = FALSE;
	}
} /* end SWIFT_Terminate */


int SWIFT_GetAttachedDevice(void) {
/* Returns the device-type code for the attached SWIFT device, if any.  */

	int i;


#ifdef PROTECTED_MODE
	for (i=1; i<=2; i++) {
		// get SWIFT device model number
		memset(&RMI, 0, sizeof(RMI));
		RMI.ax = SWIFT_GET_DEVICE_MODEL;
		MouseInt(&RMI);

		if ((RMI.ax == 0) && (RMI.bx == SDM_CYBERMAN))
			// CyberMan found
		  return ((unsigned)RMI.bx);

		if ((RMI.ax == 0) && (RMI.bx == SDM_WINGMAN_WARRIOR))
			// WingMan Warrior found
		  return ((unsigned)RMI.bx);
	}

	// unknown SWIFT device
	return (0);
#else // REAL_MODE
	for (i=1; i<=2; i++) {
		// get SWIFT device model number
		AX(regs) = SWIFT_GET_DEVICE_MODEL;
	 BX(regs) = i;
		int86( 0x33, &regs, &regs);

		if ((AX(regs) == 0) && (BX(regs) == SDM_CYBERMAN))
			// CyberMan found
			return (unsigned)BX(regs);

		if ((AX(regs) == 0) && (BX(regs) == SDM_WINGMAN_WARRIOR))
			// WingMan Warrior found
			return (unsigned)BX(regs);
	}

	// unknown SWIFT device
	return (0);
#endif
} /* end SWIFT_GetAttachedDevice */


int SWIFT_GetStaticDeviceInfo(SWIFT_StaticData far *psd)
/* Reads static device data.                           */
{
#ifdef PROTECTED_MODE
	memset(&RMI, 0, sizeof(RMI));
	RMI.ax = SWIFT_GET_STATIC_STATUS;  // SWIFT: Get Static Device Data
	RMI.es = segment;             // DOS buffer real-mode segment
	RMI.dx = 0;                   //  "    "      "   "   offset
	MouseInt(&RMI);               // get data into DOS buffer

	*psd = *(SWIFT_StaticData *)pdosmem;    // then copy into caller's buffer
	return (RMI.ax == 1);         // return success

#else // assume 16-bit '86

	AX(regs) = SWIFT_GET_STATIC_STATUS;
   DX(regs) = FP_OFF(psd);
   sregs.es = FP_SEG(psd);
   int86x(0x33, &regs, &regs, &sregs);
   return AX(regs) == 1;

#endif
} /* end SWIFT_GetStaticDeviceInfo */


//-------------------------------------------------------------------------
// Internal Mouse event handler: SWIFT_internal_handler
//
// This routine is called directly by the mouse driver.  It's job
// is to access the parameters passed to it by the driver in registers,
// and pass them on to the client event handler using standard
// parameter-passing conventions.  For SWIFT, it also takes care of
// the translation of the extended-info-pointer in SI.

#ifdef __WATCOMC__
// version for WATCOM
void _loadds far SWIFT_internal_handler (int mAX, int mBX, int mCX, int mDX, int mSI)
{
#pragma aux SWIFT_internal_handler parm [eax] [ebx] [ecx] [edx] [esi]

   pSWIFT_EventData pinfo;

   if (client_handler) {

	 if (mAX & 0xff80) {
	    // SWIFT event, get info from extended info block
	    pinfo = (pSWIFT_EventData) ((mSI & 0xFFFF) << 4);
	 } else {
	    // ordinary mouse event
	    // pass event mask, button status, no extended info
	    pinfo = (pSWIFT_EventData)NULL;
	 }
	 (*client_handler)(mAX, mBX, mCX, mDX, pinfo); 
   }
} // SWIFT_internal_handler
#endif // __WATCOMC__

#ifdef __BORLANDC__
// version for Borland C++ 3.10
void far SWIFT_internal_handler (void)
{
   pSWIFT_EventData pinfo;
   int mAX, mBX, mCX, mDX, mSI;
   asm {
	mov  mAX, ax
	mov  mBX, bx
	mov  mCX, cx
	mov  mDX, dx
	mov  mSI, si
	mov  ax, DGROUP
	mov  ds, ax
   }

   if (client_handler) {

	 if (mAX & 0xff80) {
	    // SWIFT event, get info from extended info block
	    pinfo = (pSWIFT_EventData) MK_FP(mSI, 0);
	 } else {
	    // ordinary mouse event
	    // pass event mask, button status, no extended info
	    pinfo = (pSWIFT_EventData)NULL;
	 }
	 (*client_handler)(mAX, mBX, mCX, mDX, pinfo); 
   }
} // SWIFT_internal_handler
#endif // __BORLANDC__


#ifdef _MSC_VER
// version for Microsoft C
void far SWIFT_internal_handler (void)
{
   pSWIFT_EventData pinfo;
   int mAX, mBX, mCX, mDX, mSI;
   _asm {
	mov  mAX, ax
	mov  mBX, bx
	mov  mCX, cx
	mov  mDX, dx
	mov  mSI, si
	mov  ax, SEG client_handler
	mov  ds, ax
   }

   if (client_handler) {

	 if (mAX & 0xff80) {
	    // SWIFT event, get info from extended info block
	    pinfo = (pSWIFT_EventData)((long)mSI << 16);
	 } else {
	    // ordinary mouse event
	    // pass event mask, button status, no extended info
	    pinfo = (pSWIFT_EventData)NULL;
	 }
	 (*client_handler)(mAX, mBX, mCX, mDX, pinfo); 
   }
} // SWIFT_internal_handler
#endif // _MSC_VER

//-------------------------------------------------------------------------
// SWIFT_SetMouseEventHandler
//
// This routine installs the caller's SWIFT event handling function
// using the standard mouse function 0Ch (Function 12).
// This handler can set SWIFT events in the mask as well as standard
// mouse events - when SWIFT events occur the handler is called back
// with additional SWIFT information, passed through a pointer.
// See SWIFT.H: SWIFT_EVENTHANDLER.

void SWIFT_SetMouseEventHandler(SWIFT_EVENTHANDLER hndlr, unsigned mask)
// Set the current event handler to hndlr, with the specified mask.
{
   void far *mHandler = (void far *)SWIFT_internal_handler;

   client_handler = hndlr;

   segread(&sregs);
   AX(regs) = 0x0C; /* Function 12 - Install Event Handler  */
   CX(regs) = mask;
   DX(regs) = FP_OFF(mHandler);
   sregs.es = FP_SEG(mHandler);
   #ifdef __386__
   int386x(0x33, &regs, &regs, &sregs);
   #else
   int86x(0x33, &regs, &regs, &sregs);
   #endif
} /* end SWIFT_SetMouseEventHandler */


//-------------------------------------------------------------------------
// SWIFT_Get3DStatus

void SWIFT_Get3DStatus(SWIFT_3DStatus far *pstat)
// Read the current input state of the device.
{
   #ifdef TRACE
   if (!fActive) {
	 printf("SWIFT_Get3DStatus: SWIFT module not active!\n");
   }
   #endif

#ifdef PROTECTED_MODE

   memset(&RMI, 0, sizeof(RMI));
   RMI.ax = 0x5301;
   RMI.es = segment;
   RMI.dx = 0;
   MouseInt(&RMI);
   *pstat = *(SWIFT_3DStatus *)pdosmem;

#else

   AX(regs) = 0x5301;
   regs.x.dx = FP_OFF(pstat);
   sregs.es = FP_SEG(pstat);
   int86x( 0x33, &regs, &regs, &sregs);

#endif
} /* end SWIFT_Get3DStatus */


//-------------------------------------------------------------------------
//

void SWIFT_TactileFeedback(int d, int on, int off)
/* Generates tactile feedback to user.                      */
/* d   = duration of tactile burst, in milliseconds.             */
/* on  = motor on-time per cycle, in milliseconds.               */
/* off = motor off-time per cycle, in milliseconds.              */
{

#ifdef PROTECTED_MODE
   // Use DPMI call 300h to issue mouse interrupt
   memset(&RMI, 0, sizeof(RMI));
   RMI.ax = 0x5330;      // SWIFT: Get Position & Buttons
   RMI.bx = ((on / 5) << 8) + (off / 5);
	RMI.cx = d / 40;
	MouseInt(&RMI);
#else // REAL MODE
	AX(regs) = 0x5330;
	BX(regs) = ((on / 5) << 8) + (off / 5);
   CX(regs) = d / 40;
   int86( 0x33, &regs, &regs);
#endif

   #ifdef TRACE
   printf("SWIFT_TactileFeedback(dur=%d ms, on=%d ms, off=%d ms)\n",
		d / 40 * 40, on/5*5, off/5*5);
   #endif
} /* end SWIFT_TactileFeedback */



//-------------------------------------------------------------------------
//

unsigned SWIFT_GetDynamicDeviceData(void)
/* Returns Dynamic Device Data word - see SDD_* above            */
{
#ifdef PROTECTED_MODE
	memset(&RMI, 0, sizeof(RMI));
	RMI.ax = 0x53C2;         // SWIFT: Get Dynamic Device Data
	MouseInt(&RMI);
	return (unsigned)RMI.ax;
#else // REAL_MODE
	AX(regs) = 0x53C2;
	int86( 0x33, &regs, &regs);
	return (unsigned)AX(regs);
#endif
} /* end SWIFT_GetDynamicDeviceData */


//---------------------------------------------------- Auxiliary Functions -

#ifdef PROTECTED_MODE

//-------------------------------------------------------------------------
// MouseInt
//
// Generate a call to the mouse driver (interrupt 33h) in real mode,
// using the DPMI function 'Simulate Real-Mode Interrupt'.

void MouseInt(struct rminfo *prmi)
// generate a mouse interrupt (33h)
{
   memset(&sregs, 0, sizeof (sregs));
   AX(regs) = 0x0300;         // DPMI: simulate interrupt
   BX(regs) = MOUSE_INT;
   CX(regs) = 0;
   DI(regs) = FP_OFF(prmi);
   sregs.es = FP_SEG(prmi);
   #ifdef TARGET_32
   int386x( DPMI_INT, &regs, &regs, &sregs );
   #else
   int86x( DPMI_INT, &regs, &regs, &sregs );
   #endif
} // end MouseInt()


//-------------------------------------------------------------------------
// freeDOS
//
// Release real-mode DOS memory block via DPMI

void freeDOS(short sel)
{
   AX(regs) = 0x0101;         // DPMI free DOS memory
   DX(regs) = sel;

   #ifdef TARGET_32
   int386( DPMI_INT, &regs, &regs);
   #else
   int86( DPMI_INT, &regs, &regs);
   #endif
} // end freeDOS()


//-------------------------------------------------------------------------
// allocDOS
//
// Allocate a real-mode DOS memory block via DPMI

void far *allocDOS(unsigned nbytes, short *pseg, short *psel)
{
   unsigned npara = (nbytes + 15) / 16;
   void far *pprot;
   pprot = NULL;
   *pseg = 0;            // assume will fail
   *psel = 0;

   // DPMI call 100h allocates DOS memory
   segread(&sregs);
   AX(regs) = 0x0100;              // DPMI: Allocate DOS Memory
   BX(regs) = npara;               // number of paragraphs to alloc
   #ifdef TARGET_32
   int386( DPMI_INT, &regs, &regs);
   #else
   int86( DPMI_INT, &regs, &regs);
   #endif
   if (regs.w.cflag == 0) {
	 *pseg = AX(regs);  // the real-mode segment
	 *psel = DX(regs);  // equivalent protected-mode selector
	 // pprot is the protected mode address of the same allocated block.
	 // The Rational extender maps the 1 MB physical DOS memory into
	 // the bottom of our virtual address space.
	 pprot = (void far *) ((unsigned)*pseg << 4);
   }
   return pprot;
}
#endif // PROTECTED_MODE

/****************************************************************************
*  PVCS history
*
*  $Revision:   1.0  $
*  $Author:   SpikeM  $
*  $Date:   15 Feb 1993 12:56:18  $
*  $Log:   P:/IMAGE/SCANMAN/SMDS2_2/SCANMAN/VCS/ABOUT.C_V  $
****************************************************************************/
