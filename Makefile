#############################################################################
# Makefile para construir el ejecutable: prograPong
#############################################################################

CC = gcc
CFLAGS = -g3 -fno-inline -O0 -Wall -Wextra
LDFLAGS = -Wall -Wextra
LOADLIBS = -lncurses
RM = /usr/bin/rm
RMFLAGS = --verbose --force
HALLOFFAME = hallOfFame.bin

####### Files

SOURCES	= $(wildcard *.c)
OBJECTS	= $(patsubst %.c,%.o,$(SOURCES))
TARGET	= prograPong

####### Compile rules

.SUFFIXES: .o .c

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

####### Build

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LOADLIBS) -o $@

clean:
	$(RM) $(RMFLAGS) $(TARGET) $(OBJECTS) $(HALLOFFAME)
