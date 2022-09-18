////////////////////////////////////////////////////////////////////////////
//   rtq.h   Real-Time Queueing System
////////////////////////////////////////////////////////////////////////////
// Copyright Henry Black 1992  All rights reserved
// This listing and the machine-readable source code it represents are
// the exclusive intellectual property of N.R.H.Black Registered Engineer.
// If licensed a condition of such license is that this notice is left
// intact.
////////////////////////////////////////////////////////////////////////////
#ifndef __RTQ_H__
#define __RTQ_H__

#define RTQ_NODE struct rtq_node

RTQ_NODE
   {
      RTQ_NODE *self; // Ring zero address of this node
      RTQ_NODE *left; // Ring zero address of preceding node
      RTQ_NODE *right; // Ring zero address of succeding node
      BYTE *      rtqDatum;  // Ring 3 Datum of Buffer (start of preface)
      BYTE *      rtqInsert; // Ring 3 insertion position

	// evil hack 
#ifndef __WITCHAVEN__
      WORD     rtqLen; // Length of buffer, excluding preface
      WORD     rtqUpCtr;  // Up Counter of bytes used so far
      WORD     rtqQCtr;   // number of nodes attached
      WORD     padding;   // DWORD alignment
#else
      SHORT    rtqLen; // Length of buffer, excluding preface
      SHORT    rtqUpCtr;  // Up Counter of bytes used so far
      SHORT    rtqQCtr;   // number of nodes attached
      SHORT    padding;   // DWORD alignment
#endif

   };

#define RTQ_PARAM_MOVENODE struct rtq_param_movenode
RTQ_PARAM_MOVENODE
   {
      WORD     rtqFromDQ;
      WORD     rtqToDQ;
   };
///////////////////////////////////////////////////////////////////////////
// Prototypes
RTQ_NODE* rtq_fetch(RTQ_NODE*, RTQ_NODE*); // To, From
      
#endif // ifndef __RTQ_H__
