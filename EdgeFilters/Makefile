CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.c imageFilters.c imageBasicOps.c imageUtilities.c helperFunctions.c imageFilterOps.c imageContours.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=EdgeFilters

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
