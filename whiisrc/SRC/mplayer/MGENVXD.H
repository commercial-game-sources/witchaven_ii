//
//   File:         Mgenvxd.H
//        
#ifndef   mgenvxd_h
#define   mgenvxd_h

#include "rtq.h"

#define DQ_NUM_DQS 64

//      typedefs
typedef struct tagDWORDPAIR
{
   DWORD     NumOfBuffers;
   DWORD     SizeOfEachBuffer;
} DWORDPAIR;

typedef struct tagDQSIZES
{
   DWORDPAIR NumAndSize[DQ_NUM_DQS];
} DQSIZES;

typedef struct tagMGenVxDInfo
{
   HWND  hWnd;       // Handle of Window to receive messages
   DWORD uMsg;    // Default type of message to be posted back
   DWORD dwSizeWanted; // Size of memory requested (in bytes)
   DWORD hVXD;       // Handle of the VXD
   DWORD pDatum;     // Base address of shared memory allocation
   DWORD dwSlot;     // Serial or Slot number of this VxD user (from zero)
   DWORD dwSizeGiven;  // Size of memory allocation given (in bytes)
   WORD  wSelect32;    // Ring 3 32bit selector that points to datum
   WORD  wSelect16;    // Ring 3 16bit selector that points to datum
   DQSIZES *pdqsizes;  // 16 pairs of number and size for dequeues
} MGenVxDInfo;
// 
//                              symbolic constants
#define BUFSIZE 80
//
//                               Globals
// Globals in a DLL are not very friendly, we don't have any 
// if we can avoid them.
//
//          prototypes

// these are the names of the various functions available
// don't link to them implicitly if you wish to dynamically load
// mgenvxd.dll
#define MGENINITVXDFUNC      "MGenInitVxD"
#define MGENUNINITVXDFUNC    "MGenUninitVxD"
#define MGENWAKEUPFUNC       "MGenWakeUp"
#define MGENGETQCTRFUNC      "MGenGetQueueCtr"
#define MGENMOVENODEFUNC     "MGenMoveTo"
#define MGENGETNODEFUNC      "MGenGetNode"
#define MGENGETMASTERNODEFUNC     "MGenGetMasterNode"
#define MGENFLUSHNODEFUNC    "MGenFlushNodes"
#define MGENMCOUNTFUNC       "MGenMCount"
#define MGENSANITYCHECKFUNC  "MGenSanityCheck"


// initialize the vxd according to the vxdinfo struct depicted above
INT MGENINITVXD (MGenVxDInfo*);

// unload the vxd
INT MGENUNINITVXD (HANDLE, HWND);

// wake up dos program
INT MGENWAKEUP (HANDLE);

// count the number of items in queue
INT MGENGETQCTR(HANDLE, DWORD);

// move node from q1 to q2 (q1 is the first argument)
RTQ_NODE *MGENMOVENODE(HANDLE, DWORD, DWORD);

// get the first node from q1
RTQ_NODE *MGENGETNODE(HANDLE, DWORD);

// move all the nodes from q1 to q2.  does nothing if q1 = q2
INT MGENFLUSHNODES(HANDLE, DWORD, DWORD);

// get prtq_Master[0], also size of RTQ_NODE is returned
RTQ_NODE *MGENGETMASTERNODE(HANDLE, DWORD *);

// get a count of all the nodes in a given set of queues
// determined by the LARGE_INTEGER passed.  low order bits
// are 0-31, high order bits are 32-63
INT MGENMCOUNT(HANDLE, LARGE_INTEGER);

// performs consistency check on the chunnel address space
INT MGENSANITYCHECK(HANDLE);

#endif // mgenvxd_h

