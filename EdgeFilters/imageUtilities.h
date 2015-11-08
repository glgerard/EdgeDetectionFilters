#ifndef _IMAGEUTILITY
#define _IMAGEUTILITY
//---------------------------------------------------------//
//------------------ Standard includes --------------------//
//---------------------------------------------------------// 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define __DEBUG__

#ifdef __DEBUG__
#define D(a) a
#else
#define D(a)
#endif

typedef struct 
{
  int width;
  int height;
  int max_val;
  int* pixels;
} Pgm;

//---------------------------------------------------------//
//------------ Input/Output Functions for PGM -------------//
//---------------------------------------------------------// 
Pgm* newPGM(int width, int height, int max_val);
Pgm* readPGM(char* filename);
int writePGM(Pgm* pgm, char* filename);
void resetPGM(Pgm* pgm);
void freePGM(Pgm** pgm);

//---------------------------------------------------------//
//----------------- Basic Functions for PGM ---------------//
//---------------------------------------------------------// 
int invertPGM(Pgm* pgmIn, Pgm* pgmOut);
int hflipPGM(Pgm* pgmImg, Pgm* pgmOut);
int copyPGM(Pgm* pgmImg, Pgm* pgmOut);
int* histogramPGM(Pgm* pgm); 

//---------------------------------------------------------//
//----------------- Other Functions for PGM ---------------//
//---------------------------------------------------------//
int normalizePGM(Pgm* pgmIn, Pgm* pgmOut);
int equalizePGM(Pgm* pgmIn, Pgm* pgmOut);

#endif
