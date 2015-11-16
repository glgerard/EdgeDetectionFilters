SHELL=/bin/sh

#CC=gcc
CC=/opt/local/bin/x86_64-apple-darwin15-gcc-4.9.3
CFLAGS=-c -Wall
LDFLAGS=-lm
SOURCES=main.c imageFilters.c imageBasicOps.c imageUtilities.c helperFunctions.c imageFilterOps.c imageContours.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=filterPGM

.SUFFIXES:
.SUFFIXES: .c .o

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

install: $(EXECUTABLE)
	[ -d $(HOME)/bin ] || mkdir $(HOME)/bin
	[ -d $(HOME)/bin/flts ] || mkdir $(HOME)/bin/flts
	cp $(EXECUTABLE) $(HOME)/bin
	cp run.sh $(HOME)/bin/runFilters.sh
	cp filters/*.flt $(HOME)/bin/flts
clean:
	rm $(OBJECTS) $(EXECUTABLE)

.c.o:
	$(CC) $(CFLAGS) $< -o $@
