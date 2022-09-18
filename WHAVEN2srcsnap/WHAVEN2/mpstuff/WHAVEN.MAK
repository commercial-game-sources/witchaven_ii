#
#  Makefile for Witchaven
#

.ERASE

PROCESSOR = 386 # for the 32-bit compilers

# name the compiler
CC = wcc$(PROCESSOR)
ASM = wasm
LINK = wlink

MODEL = f   # flat

#OBJP = d:\wh\src\obj\
#RUNP = d:\wh\run\

OBJWC = ..\..\vxd\watcom\
OBJP = 
RUNP = ..\run\

# options to use
AFLAGS = /c /Cp /Zm /Zd /Fo$(OBJP)$^&.obj /nologo
#LNKOPT = option quiet

#CFLAGS = -s -4r -d2 -or
#CFLAGS = -s -4r -or -i..\..\vxd\include -ic:\mpsdk\include
CFLAGS = -s -4r -or -ic:\mpsdk\include
#AFLAGS = -4p -mf -zq
LNKOPT = option quiet


.EXTENSIONS:
.EXTENSIONS: .exe .lib .obj .asm .c

.BEFORE
        @set INCLUDE=.;$(%watcom)\h
        @set DOS4G=QUIET

# implicit rules

.c.obj :
                  $(CC) $^& $(CFLAGS) >>errors

.asm.obj :
                  $(ASM) $(AFLAGS) $^&.asm >>errors

# explicit rules

$(RUNP)wh2.exe : whaven.lnk &
                  $(OBJP)whfx.obj &
                  $(OBJP)whaven.obj &
                  $(OBJP)whplr.obj &
                  $(OBJP)whobj.obj &
                  $(OBJP)whtag.obj &
                  $(OBJP)whinp.obj &
                  $(OBJP)engine.obj &
                  $(OBJP)a.obj &
                  $(OBJP)cache1d.obj &
                  $(OBJP)whsndmod.obj &
                  $(OBJP)whmenu.obj &
                  $(OBJP)whani.obj &
                  $(OBJP)whasm.obj &
                  $(OBJP)jstick.obj &
                  $(OBJP)whnet.obj &
                  $(OBJP)cdrom.obj &
                  $(OBJP)whctm.obj &
                  $(OBJP)mplib.obj &
                  $(OBJP)mplpc.obj
                  $(LINK) $(LNKOPT) @whaven.lnk >>errors

#                  copy $(RUNP)wh2.exe $(RUNP)wh2.bak
$(OBJP)whaven.obj :     whaven.c        build.h         names.h         &
                                                                whaven.mak      icorp.h

$(OBJP)whplr.obj :      whplr.c         build.h         names.h         &
                                                                whaven.mak      icorp.h

$(OBJP)whobj.obj :      whobj.c         build.h         names.h         &
                                                                whaven.mak      icorp.h

$(OBJP)whtag.obj :      whtag.c         build.h         names.h         &
                                                                whaven.mak      icorp.h

$(OBJP)whinp.obj :      whinp.c         build.h         names.h         &
                                                                whaven.mak      icorp.h

$(OBJP)whfx.obj :       whfx.c          build.h         names.h         &
                                                                whaven.mak      icorp.h

$(OBJP)whmenu.obj :     whmenu.c        build.h         names.h          &
                                                                whaven.mak      icorp.h
                                                                                                        
$(OBJP)whsndmod.obj :   whsndmod.c      sndmod.h   &
                                                                whaven.mak      icorp.h

$(OBJP)whasm.obj :      whasm.asm       build.h         names.h        &
                                                                whaven.mak      icorp.h

$(OBJP)whani.obj :      whani.c         build.h         names.h          &
                                                                whaven.mak      icorp.h

$(OBJP)jstick.obj :     jstick.asm      build.h         names.h        &
                                                                whaven.mak      icorp.h

$(OBJP)whnet.obj :      whnet.c         build.h         names.h          &
                                                                whaven.mak      icorp.h

$(OBJP)cdrom.obj :      cdrom.c         build.h         names.h          &
                                                                whaven.mak      icorp.h


$(OBJP)whctm.obj :      whctm.c         build.h         names.h          &
                                                                whaven.mak      icorp.h

$(OBJP)mplib.obj:       mplib.c ..\..\vxd\include\rtq.h whaven.mak ..\..\vxd\include\mplib.h
$(OBJP)mplpc.obj:       mplpc.c ..\..\vxd\include\rtq.h whaven.mak ..\..\vxd\include\lpc.h ..\..\vxd\include\mplib.h ..\..\vxd\include\services.h

