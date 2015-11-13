//
//  wrappers.h
//  EdgeFilters
//
//  Created by Gianluca Gerard on 07/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#ifndef imageFilterOps_H
#define imageFilterOps_H

#include <stdio.h>
#include "helperFunctions.h"
#include "imageUtilities.h"
#include "imageFilters.h"
#include "imageBasicOps.h"
#include "imageContours.h"

#define TRUE 1
#define FALSE 0

//---------------------------------------------------------//
//--------------- Noise  operations for PGM ---------------//
//---------------------------------------------------------//
int addUniformNoisePGM(Pgm* pgmIn, Pgm* pgmOut, int range);
int addSaltPepperNoisePGM(Pgm* pgmIn, Pgm* pgmOut, double prob);

int medianPGM(Pgm *pgmIn, Pgm* pgmOut);
int averagePGM(Pgm *pgmIn, Pgm* pgmOut);

int sharpeningPGM(Pgm* imgIn, Pgm* imgOut);
int gaussPGM(Pgm* imgIn, Pgm* imgOut, double sigma, int dim);

//---------------------------------------------------------//
//------- Contour/Edge Detection operations for PGM -------//
//---------------------------------------------------------//

int op39PGM(Pgm *pgmIn, Pgm* pgmOut);
int nagaoPGM(Pgm *pgmIn, Pgm* pgmOut);
int suppressionPGM(Pgm *pgmMod, Pgm *pgmPhi, Pgm *pgmOut);

int applySobel(Pgm* pgmIn, Pgm* pgmOut, int iarg);
int applyPrewitt(Pgm* imgIn, Pgm* imgOut, int eval);
int applyDoG(Pgm* imgIn, Pgm* imgOut, double sigma, int dim);
int applyCED(Pgm* imgIn, Pgm* imgOut, double sigma, int dim, int threshold, int thresholdRatio);

int execImageOps(Pgm *pgmIn, Pgm* pgmOut, FILE *fp);

#endif /* imageFilterOps_H */
