# Makefile for zlib (MSVC + NMAKE; others?)
#
# GRR 1998.10.21:  This makefile now builds both shared (DLL) and static
#   libraries.  Objects ending in ".obj" are the static ones; those ending
#   in ".objdll" are "position-independent" DLL objects.  Since cl.exe
#   doesn't seem to honor the -out: option, the .objdll flavor are renamed
#   using what is probably an NMAKE-specific feature:  %|xF, where "x" is
#   some combo of d (drive), p (path, including drive), f (file basename)
#   and e (extension).  The useful output is zlibstat.lib (the static version),
#   zlibdll.lib (the DLL import or stub library with which one links), and
#   zlib.dll (the DLL itself, required at runtime).  To build only one or the
#   other, remove either zlib.dll or zlibstat.lib from the "all:" target.
#
#   SUMMARY:  Link programs with either zlibstat.lib or zlibdll.lib.  If the
#             latter, be sure to distribute zlib.dll with the executable(s).

!include <ntwin32.mak>

CC=cl
LD=link
CFLAGS=-nologo -O
LDFLAGS=
O=.obj
OD=.objdll

# variables
OBJ1 = adler32$(O) compress$(O) crc32$(O) gzio$(O) uncompr$(O)
OBJ2 = deflate$(O) trees$(O) zutil$(O) inflate$(O) infblock$(O)
OBJ3 = inftrees$(O) infcodes$(O) infutil$(O) inffast$(O)
OBJS = $(OBJ1) $(OBJ2) $(OBJ3)

OBJD1 = adler32$(OD) compress$(OD) crc32$(OD) gzio$(OD) uncompr$(OD)
OBJD2 = deflate$(OD) trees$(OD) zutil$(OD) inflate$(OD) infblock$(OD)
OBJD3 = inftrees$(OD) infcodes$(OD) infutil$(OD) inffast$(OD)
OBJSDLL = $(OBJD1) $(OBJD2) $(OBJD3)

.SUFFIXES: .objdll

# [see %devstudio%\vc\include\win32.mak for macro definitions]
.c.obj:
	$(CC) -c $(CFLAGS) $(cvars) $<

# -out: doesn't work with cl.exe (some other option? RTFM, I suppose...)
.c.objdll:
#	$(CC) -c $(CFLAGS) $(cvarsdll) -out:$@ $<
	$(CC) -c $(CFLAGS) $(cvarsdll) $<
	ren %|fF.obj $@

all:  zlib.dll zlibstat.lib example.exe minigzip.exe

adler32$(O) adler32$(OD):	adler32.c zutil.h zlib.h zconf.h
compress$(O) compress$(OD):	compress.c zlib.h zconf.h
crc32$(O) crc32$(OD):		crc32.c zutil.h zlib.h zconf.h
deflate$(O) deflate$(OD):	deflate.c deflate.h zutil.h zlib.h zconf.h
gzio$(O) gzio$(OD):		gzio.c zutil.h zlib.h zconf.h
infblock$(O) infblock$(OD):	infblock.c zutil.h zlib.h zconf.h inftrees.h \
  infutil.h infcodes.h infblock.h
infcodes$(O) infcodes$(OD):	infcodes.c zutil.h zlib.h zconf.h inftrees.h \
  infutil.h infcodes.h inffast.h
inflate$(O) inflate$(OD):	inflate.c zutil.h zlib.h zconf.h infblock.h
inftrees$(O) inftrees$(OD):	inftrees.c zutil.h zlib.h zconf.h inftrees.h
infutil$(O) infutil$(OD):	infutil.c zutil.h zlib.h zconf.h inftrees.h \
  infutil.h
inffast$(O) inffast$(OD):	inffast.c zutil.h zlib.h zconf.h inftrees.h \
  infutil.h inffast.h
trees$(O) trees$(OD):		trees.c deflate.h zutil.h zlib.h zconf.h
uncompr$(O) uncompr$(OD):	uncompr.c zlib.h zconf.h
zutil$(O) zutil$(OD):		zutil.c zutil.h zlib.h zconf.h

example$(O): example.c zlib.h zconf.h
minigzip$(O): minigzip.c zlib.h zconf.h

zlib.dll: $(OBJSDLL) nt/zlib.dnt
	link $(dlllflags) -out:$@ -def:nt/zlib.dnt $(OBJSDLL) $(guilibsdll)
	-ren zlib.lib zlibdll.lib

zlibstat.lib: $(OBJS)
	lib -nologo -out:$@ $(OBJS)

zlibdll.lib: zlib.dll

example.exe: example.obj zlibdll.lib
	$(LD) $(LDFLAGS) example.obj zlibdll.lib

minigzip.exe: minigzip.obj zlibdll.lib
	$(LD) $(LDFLAGS) minigzip.obj zlibdll.lib 

test: example.exe minigzip.exe
	example
	echo hello world | minigzip | minigzip -d 

clean:
	del *.objdll
	del *.obj
	del *.exe
	del *.dll
	del *.lib
	del zlib.exp
	del foo.gz
