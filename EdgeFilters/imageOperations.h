//
//  imageOperations.h
//  EdgeFilters
//
//  Created by Gianluca Gerard on 25/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#ifndef imageOperations_h
#define imageOperations_h

//---------------------------------------------------------//
//------------------ Standard includes --------------------//
//---------------------------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "imageUtilities.h"
#include "imageFilters.h"

#define drandom() (((double)(random()&0xffffff))/0xffffff)

//---------------------------------------------------------//
//------------- Basic image operations for PGM ------------//
//---------------------------------------------------------//
int absolutePGM(Pgm* pgmIn, Pgm* pgmOut);
int thresholdPGM(Pgm* pgmIn, Pgm* pgmOut, int threshold);
int linearAddPGM(Pgm* pgmOp1, Pgm* pgmOp2, double w1, double w2, Pgm* pgmOut);

//---------------------------------------------------------//
//----------- Advanced image operations for PGM -----------//
//---------------------------------------------------------//
int medianPGM(Pgm *pgmIn, Pgm* pgmOut);
int modulePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut);
int phasePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut);

int fapplyPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter, int borderX, int borderY,
            int (*func)(Pgm*, double*, int, int, int));

//---------------------------------------------------------//
//--------------- Noise  operations for PGM ---------------//
//---------------------------------------------------------//
int addUniformNoisePGM(Pgm* pgmIn, Pgm* pgmOut, int range);
int addSaltPepperNoisePGM(Pgm* pgmIn, Pgm* pgmOut, double prob);

//---------------------------------------------------------//
//------- Contour/Edge Detection operations for PGM -------//
//---------------------------------------------------------//
int contour2DPGM(Pgm* pgmIn, Pgm* pgmOut);
int contourN8IntPGM(Pgm* pgmIn, Pgm* pgmOut);
int op39PGM(Pgm *pgmIn, Pgm* pgmOut);
int nagaoPGM(Pgm *pgmIn, Pgm* pgmOut);

Pgm* applyFilters(Pgm *pgmOrig, FILE *fp);


#endif /* imageOperations_h */
