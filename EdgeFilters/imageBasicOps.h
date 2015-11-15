//
//  imageBasicOps.h
//  EdgeFilters
//
//  Created by Gianluca Gerard on 25/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#ifndef imageBasicOps_h
#define imageBasicOps_h

//---------------------------------------------------------//
//------------------ Standard includes --------------------//
//---------------------------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "helperFunctions.h"
#include "imageUtilities.h"
#include "imageFilters.h"
#include "imageFilterOps.h"

#define drandom() (((double)(random()&0xffffff))/0xffffff)

//---------------------------------------------------------//
//------------- Basic image operations for PGM ------------//
//---------------------------------------------------------//
int absolutePGM(Pgm* pgmIn, Pgm* pgmOut);
int thresholdPGM(Pgm* pgmIn, Pgm* pgmOut, int threshold);
int linearAddPGM(Pgm* pgmOp1, Pgm* pgmOp2, double w1, double w2, Pgm* pgmOut);
int comparePGM(Pgm* pgmOp1, Pgm* pgmOp2);

//---------------------------------------------------------//
//----------- Advanced image operations for PGM -----------//
//---------------------------------------------------------//
int modulePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut);
int phasePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut);

int fapplyPGM(Pgm* pgmIn1, Pgm* pgmIn2, Pgm* pgmOut, Filter* filter, int dimX, int dimY,
            int (*func)(Pgm*, Pgm*, double*, int, int, int));

//---------------------------------------------------------//
//---------------------   Convolutions  -------------------//
//---------------------------------------------------------//
int convolution2DPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter);
int convolution1DXPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter);
int convolution1DYPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter);

#endif /* imageBasicOps_h */
