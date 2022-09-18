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

OBJP =
RUNP = ..\run\

# options to use
#AFLAGS = /c /Cp /Zm /Zd /Fo$(OBJP)$^&.obj /nologo
#LNKOPT = option quiet

CFLAGS = -s -4r -d2 -or -Igcl520h\
AFLAGS = -4p -mf -zq
LNKOPT = option quiet


.EXTENSIONS:
.EXTENSIONS: .exe .lib .obj .asm .c

.BEFORE
        @set INCLUDE=.;$(%watcom)\h;$(%watcom)\h\nt
	@set DOS4G=QUIET

# implicit rules

.c.obj :
		  $(CC) $^& $(CFLAGS) >>errors

.asm.obj :
		  $(ASM) $(AFLAGS) $^&.asm >>errors

# explicit rules

$(RUNP)wh2.exe : whaven.lnk &
                  $(OBJP)whaven.obj &
		  $(OBJP)whplr.obj &
		  $(OBJP)whobj.obj &
		  $(OBJP)whtag.obj &
		  $(OBJP)whinp.obj &
		  $(OBJP)engine.obj &
		  $(OBJP)a.obj &
		  $(OBJP)cache1d.obj &
                  $(OBJP)whfx.obj &
		  $(OBJP)whsndmod.obj &
		  $(OBJP)whmenu.obj &
		  $(OBJP)whasm.obj &
                  $(OBJP)whani.obj &
		  $(OBJP)jstick.obj &
		  $(OBJP)whnet.obj &
		  $(OBJP)cdrom.obj &
                  $(OBJP)whctm.obj &
                  $(OBJP)whvfx1.obj &
                  $(OBJP)av_wc10r.obj &
                  $(OBJP)swift.obj &
                  $(OBJP)whsmk.obj &
                  $(OBJP)readcfg.obj &
                  $(OBJP)profile.obj &
                  $(OBJP)mplib.obj &
                  $(OBJP)mplpc.obj
		  $(LINK) $(LNKOPT) @whaven.lnk >>errors

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

$(OBJP)whvfx1.obj :     whvfx1.c        build.h         names.h          &
								whaven.mak      icorp.h

$(OBJP)swift.obj :      swift.c         build.h         names.h          &
								whaven.mak      icorp.h

$(OBJP)whsndmod.obj :   whsndmod.c         build.h         names.h       &
								whaven.mak      icorp.h

$(OBJP)whsmk.obj :      whsmk.c         build.h        names.h          &
                                                                whaven.mak

$(OBJP)readcfg.obj :    readcfg.c       build.h        names.h          &
                                                                whaven.mak

$(OBJP)profile.obj :    profile.c       build.h        names.h          &
                                                                whaven.mak

