SHELL=/bin/sh

CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.c imageFilters.c imageBasicOps.c imageUtilities.c helperFunctions.c imageFilterOps.c imageContours.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=filterPGM

.SUFFIXES:
.SUFFIXES: .c .o

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

install: $(EXECUTABLE)
	cp $(EXECUTABLE) $(HOME)/bin
	cp run.sh $(HOME)/bin/runFilters.sh

clean:
	rm $(OBJECTS) $(EXECUTABLE)

.c.o:
	$(CC) $(CFLAGS) $< -o $@
