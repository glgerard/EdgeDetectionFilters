/*! \file imageUtilities.h
 *  \brief Interfaces to functions for reading and writing in PGM P2 - P5 format.
 *  \author Piercarlo Dondi
 *  \author Alessandro Gaggia
 *  \author Gianluca Gerard
 */

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

/*! \struct Pgm
 * \brief Structure to store PGM format images.
 */
typedef struct 
{
  int width;   /*!< Image's width */
  int height;  /*!< Image's height */
  int max_val; /*!< Image's maximum pixel values */
  int* pixels; /*!< Pointer to the array of pixels values */
} Pgm;

/*! \struct Histogram
 * \brief Structure to store images Histograms.
 */
typedef struct
{
    int min_val;     /*!< Histogram's minimum channel */
    int max_val;     /*!< Histogram's maximum channel */
    int size;        /*!< Histogram's number of channels */
    int* channels;   /*!< Pointer to the array containing the values for each channel */
} Histogram;

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
Histogram* histogramPGM(Pgm* pgm);
void freeHistogram(Histogram** h);

//---------------------------------------------------------//
//----------------- Other Functions for PGM ---------------//
//---------------------------------------------------------//
int normalizePGM(Pgm* pgmIn, Pgm* pgmOut);
int equalizePGM(Pgm* pgmIn, Pgm* pgmOut);

#endif
