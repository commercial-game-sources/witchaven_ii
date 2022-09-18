/*************************************************************************/
/* SWIFT.H - include file for SWIFT services module                      */
/*                                                                       */
/* Copyright (C) 1993-1995 Logitech, Inc.   All Rights Reserved.         */
/* 11/11/93  Spike  1.0  Created.                                             */
/* 09/26/95  JCB    2.0  Upgraded to support WingMan Warrior.                 */
/*                                                                       */
/*************************************************************************/

#ifndef __SWIFT_H__
#define __SWIFT_H__

/*----------------------------------------------------------- constants */
/* event mask bits, for use with SWIFT_SetMouseEventHandler only!       */
#define MOUSE_CURSOR_MOTION             0x0001
#define MOUSE_LEFT_BUTTON_PRESSED       0x0002
#define MOUSE_LEFT_BUTTON_RELEASED      0x0004
#define MOUSE_RIGHT_BUTTON_PRESSED      0x0008
#define MOUSE_RIGHT_BUTTON_RELEASED     0x0010
#define MOUSE_MIDDLE_BUTTON_PRESSED     0x0020
#define MOUSE_MIDDLE_BUTTON_RELEASED    0x0040
#define MOUSE_OTHER_BUTTON_PRESSED      0x0080
#define MOUSE_OTHER_BUTTON_RELEASED     0x0100
#define MOUSE_X_CHANGED                 0x0200
#define MOUSE_Y_CHANGED                 0x0400
#define MOUSE_Z_CHANGED                 0x0800
#define MOUSE_PITCH_CHANGED             0x1000
#define MOUSE_ROLL_CHANGED              0x2000
#define MOUSE_YAW_CHANGED               0x4000
#define MOUSE_OTHER_CHANGE              0x8000

#define MOUSE_ANY_BUTTON_CHANGED        0x01FE
#define MOUSE_DYNAMIC_DATA              MOUSE_OTHER_CHANGE
#define MOUSE_3D_MOTION                 0x7E00

/* SWIFT command defines */
#define SWIFT_GET_STATUS                0x5301
#define SWIFT_6D_DATA_PASSTHRU          0x5302
#define SWIFT_SET_TACTILE               0x5330
#define SWIFT_INSTALL_EVENT_HANDLER     0x53C0
#define SWIFT_GET_STATIC_STATUS         0x53C1
#define SWIFT_GET_DYNAMIC_STATUS        0x53C2
#define SWIFT_GET_DEVICE_MODEL          0x53C3

/* Dynamic device data */
#define SDD_EXTERNAL_POWER_CONNECTED    1
#define SDD_EXTERNAL_POWER_TOO_HIGH     2

/* device type codes, returned in deviceType field (below)              */
#define SWIFT_DEV_NONE                  0
#define SWIFT_DEV_CONNECTED             1

/* Device models - refer to SWIFT Supplement for full listing           */
#define SDM_NO_DEVICE                   0
#define SDM_CYBERMAN                    32
#define SDM_WINGMAN_WARRIOR             76


/*---------------------------------------------------- type definitions */
/* Static data about device, returned by SWIFT_GetStaticDeviceInfo      */
typedef struct {
	unsigned char        deviceType;
	unsigned char        majorVersion;
	unsigned char        minorVersion;
	unsigned char        coordDescriptor[6];
	unsigned char        numButtons;
} SWIFT_StaticData;

/* Interactive input status of device, returned by SWIFT_Get3DStatus    */
typedef struct {
   short        x;
   short        y;
   short        z;
   short        pitch; // aka - xr
   short        roll;  // aka - yr
   short        yaw;   // aka - zr
   short        buttons;
} SWIFT_3DStatus;

// CyberMan status button definitions (above)
#define CSB_RIGHT                       0x001
#define CSB_MIDDLE                      0x002
#define CSB_LEFT                        0x004

// WingMan Warrior status button definitions (above)
#define WSB_TOP                         0x001
#define WSB_THUMB                       0x002
#define WSB_TRIGGER                     0x004
#define WSB_RESERVED                    0x008
#define WSB_MIDDLE                      0x010
#define WSB_HATWEST                     0x020
#define WSB_HATEAST                     0x040
#define WSB_HATNORTH                    0x080
#define WSB_HATSOUTH                    0x100
#define WSB_HATALL                      0x1E0


/* Extended event data passed to event handler                          */
typedef struct {
   short        x, y, z;
   short        pitch, roll, yaw;  // aka - xr, yr, zr
   short        buttons;
   short        dynamic_data;
} SWIFT_EventData;

// CyberMan event button definitions (above)
#define CEB_LEFT                        0x001
#define CEB_RIGHT                       0x002
#define CEB_MIDDLE                      0x004

// WingMan Warrior event button definitions (above)
#define WEB_TRIGGER                     0x001
#define WEB_TOP                         0x002
#define WEB_THUMB                       0x004
#define WEB_RESERVED                    0x008
#define WEB_MIDDLE                      0x010
#define WEB_HATWEST                     0x020
#define WEB_HATEAST                     0x040
#define WEB_HATNORTH                    0x080
#define WEB_HATSOUTH                    0x100
#define WEB_HATALL                      0x1E0

typedef SWIFT_EventData far *pSWIFT_EventData;

/* Extended event handler prototype                                     */
typedef void (*SWIFT_EVENTHANDLER)(int, int, int, int, pSWIFT_EventData);
// 1st parameter = the set of events that triggered the call
// 2nd parameter = button status at the time of the call
// 3rd parameter = horizontal cursor position
// 4th parameter = vertical cursor position
// 5th parameter points to extended event data if available, or NULL

/*------------------------------------------------- function prototypes */

int SWIFT_Initialize(void);
/* Test for presence of SWIFT extensions and SWIFT device               */
/* Returns 1 (TRUE) if SWIFT features are available, 0 otherwise.       */
/* Remember to call SWIFT_Terminate() if SWIFT_Initialize succeeds!     */

void SWIFT_Terminate(void);
/* Free resources required for SWIFT support.  If SWIFT_Initialize has  */
/* not been called, or returned FALSE, this function does nothing.      */
/* SWIFT_Terminate should always be called at some time after a call to */
/* SWIFT_Initialize has returned TRUE.                                  */

int SWIFT_GetAttachedDevice(void);
/* Returns the device-type code for the attached SWIFT device, if any.  */

int SWIFT_GetStaticDeviceInfo(SWIFT_StaticData far *psd);
/* Reads static device data.                                            */

void SWIFT_SetMouseEventHandler(SWIFT_EVENTHANDLER f, unsigned m);
/* Set the current mouse event handler to function f, event mask m.     */

void SWIFT_Get3DStatus(SWIFT_3DStatus far *);
/* Read the current input state of the device.                          */

void SWIFT_TactileFeedback(int d, int on, int off);
/* Generates tactile feedback to user.                                  */
/* d   = duration of tactile burst, in milliseconds.                    */
/* on  = motor on-time per cycle, in milliseconds.                      */
/* off = motor off-time per cycle, in milliseconds.                     */

unsigned SWIFT_GetDynamicDeviceData(void);
/* Returns Dynamic Device Data word - see SDD_* above                   */

#endif
