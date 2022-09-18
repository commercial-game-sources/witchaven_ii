// File MGENORD.H
//
#ifndef mgenord_h
#define mgenord_h

#define MGENVXD_REGISTER_ORD       1
#define MGENVXD_GETMEM_ORD         2
#define MGENVXD_DEREGISTER_ORD     3
#define MGENVXD_WAKEUP_ORD         4
#define MGENVXD_MAKEDQS_ORD        5


// Virtual 8086 API Ordinals
#define V86API_GETSELECTOR16_ORD (1)
#define V86API_GETSELECTOR32_ORD (2)
#define V86API_GETFLAT32_ORD (3)
#define V86API_MOVERP_ORD (6)
#define V86API_MOVEPR_ORD (7)
#define V86API_POST_ORD   (8)
#define V86API_INIT_ORD    (9)
#define V86API_UNINIT_ORD  (10)
#define V86API_INSERTKEY_ORD (11)
#define V86API_REMOVEHOTKEY_ORD  (12)
#define V86API_INSTALLHOTKEY_ORD (13)
#define V86API_HOOKINT48_ORD  (14)
#define V86API_WAKEUPDLL_ORD (15)

#define DPMIAPI_GETFLAT32_ORD (1)
#define DPMIAPI_POST_WINDOWS_ORD (2)
// these are DPMI functions.  Make sure they don't clash with the
// other MGENVXD_XXXX functions above, or the DPMI functions!
#define MGENVXD_GETQUEUECTR_ORD    6
#define MGENVXD_MOVENODE_ORD       7
#define MGENVXD_GETNODE_ORD        8
#define MGENVXD_FLUSHNODE_ORD      9
#define MGENVXD_MCOUNT_ORD         10
#define MGENVXD_MASTERNODE_ORD     11
#define MGENVXD_SANITYCHECK_ORD    12
#define MGENVXD_WAKEUPDLL_ORD			13

//
#define HWND_OFFSET (0)
#define UMSG_OFFSET (1)
#define SIZEREQUEST_OFFSET (2)
#define HVXD_OFFSET (3)
#define DATUM_OFFSET (4)
#define SLOT_OFFSET (5)
#define SIZEGIVEN_OFFSET (6)
#define SELECTOR32_OFFSET (7)
#define SELECTOR16_OFFSET (8)

#endif // mgenord_h

