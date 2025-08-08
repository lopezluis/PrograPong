#############################################################################
# Makefile para construir el ejecutable: pongPAyED
#############################################################################

CC = C:\PROGRA~1\CodeBlocks\MinGW\bin\gcc.exe
OUTDIR = obj\Debug
EXEDIR = bin\Debug
CFLAGS = -fno-inline -O0 -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8 -g3
INCUDEDIR = -IC:\PROGRA~1\CodeBlocks\MinGW\PDCurses
LDFLAGS = -Wall -Wextra
LOADLIBS = C:\PROGRA~1\CodeBlocks\MinGW\PDCurses\wincon\pdcurses.a
RM = del
RMFLAGS = /F

####### Files

SOURCES	= $(wildcard *.c)
OBJECTS	= $(patsubst %.c,%.o,$(SOURCES))
OBJSRM = $(patsubst %.c,$(OUTDIR)\\%.o,$(SOURCES))
TARGET	= PrograPong.exe

####### Compile rules

.SUFFIXES: .o .c

.c.o:
	$(CC) $(CFLAGS) $(INCUDEDIR) -c $< -o $(OUTDIR)\$@

####### Build

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJSRM) $(LOADLIBS) -o $(EXEDIR)\$@

clean:
	$(RM) $(RMFLAGS) $(EXEDIR)\$(TARGET) $(OBJSRM)
