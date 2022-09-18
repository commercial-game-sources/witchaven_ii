#ifndef DOSEXTH
#define DOSEXTH

RCSTART

#ifdef __RAD32__

#ifdef __WATCOMC__
#pragma pack(1)
#endif

void* RADLINK AllocDOS(u16 para);

void RADLINK FreeDOS(void* p);

s32 RADLINK mem386();    

  void* RealToPtr(u32 seg, u32 ofs);
  #pragma aux RealToPtr = "shl eax,4" "add eax,ebx" parm [EAX] [EBX];

  #define SegToPtr(seg) ((void*)((u32)seg<<4))

  void* RealPtrToPtr(void* RealPtr);
  #pragma aux RealPtrToPtr = "shld ebx,eax,20" "and eax,0xffff" "and ebx,0xffff0" "add eax,ebx" parm [EAX] modify [EBX];

  void* PtrToRealPtr(void* Ptr);
  #pragma aux PtrToRealPtr = "shrd eax,ebx,20" "and ebx,0xf" "mov ax,bx" parm [EBX] modify [EBX EAX];

  #define PtrToRealSeg(ptr) ((u16)((u32)ptr>>4))

  typedef struct SimStructtag {
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 reserved;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u16 flags;
    u16 es,ds,fs,gs,ip,cs,sp,ss;
  } SimStruct;

#ifdef __RADWINEXT__

u32 WinGet16(void*p);
#pragma aux WinGet16 = \
  "mov ax,0"    \
  "mov cx,1"    \
  "int 031h"    \
  "mov di,ax"   \
  "mov ax,6"    \
  "mov bx,ds"   \
  "int 31h"     \
  "shl ecx,16"  \
  "mov cx,dx"   \
  "add ecx,esi" \
  "mov dx,cx"   \
  "shr ecx,16"  \
  "mov ax,7"    \
  "mov bx,di"   \
  "int 31h"     \
  "mov ax,8"    \
  "mov cx,0"    \
  "mov dx,64000" \
  "int 31h"     \
  "shl ebx,16"  \
  parm [ESI]    \
  modify [EAX EBX ECX EDX EDI ESI] \
  value [ebx];

void WinFree16(void*p);
#pragma aux WinFree16 = \
  "mov ax,1"         \
  "shr ebx,16"       \
  "int 31h"          \
  parm [ebx]         \
  modify [ebx eax];

#else

#define WinGet16
#define WinFree16(p)

#endif
 
void RADLINK SimInt(u16 Int,SimStruct far*ss);

#ifdef __WATCOMC__
#pragma pack()
#endif

#else

#ifdef __WATCOMC__

u32 WinGet16(u32,u16 ds);
#pragma aux WinGet16 = \
  "mov ax,6"    \
  "int 31h"     \
  "add dx,si"   \
  "adc di,cx"   \
  "mov ax,0"    \
  "mov cx,1"    \
  "int 031h"    \
  "mov bx,ax"   \
  "mov ax,7"    \
  "mov cx,di"   \
  "int 031h"    \
  "mov ax,8"    \
  "mov cx,0"    \
  "mov dx,64000" \
  "int 31h"     \
  "mov dx,bx"   \
  "xor ax,ax"   \
  parm [DI SI] [BX]   \
  modify [AX BX CX DX DI SI] \
  value [dx ax];

void WinFree16(void PTR4*p);
#pragma aux WinFree16 = \
  "mov ax,1"         \
  "int 31h"          \
  parm [bx ax]       \
  modify [bx ax];

#endif

#endif

RCEND

#endif
