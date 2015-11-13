//
//  wrappers.h
//  EdgeFilters
//
//  Created by Gianluca Gerard on 07/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#ifndef wrappers_h
#define wrappers_h

#include <stdio.h>
#include "imageUtilities.h"

#define TRUE 1
#define FALSE 0


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
int suppressionPGM(Pgm *pgmMod, Pgm *pgmPhi, Pgm *pgmOut);
int connectivityPGM(Pgm *pgmNH, Pgm *pgmNL, Pgm *pgmOut);

int applySharpening(Pgm* imgIn, Pgm* imgOut);
int applyGauss(Pgm* imgIn, Pgm* imgOut, double sigma, int dim);
int applySobel(Pgm* pgmIn, Pgm* pgmOut, int iarg);
int applyPrewitt(Pgm* imgIn, Pgm* imgOut, int eval);
int applyDoG(Pgm* imgIn, Pgm* imgOut, double sigma, int dim);
int applyCED(Pgm* imgIn, Pgm* imgOut, double sigma, int dim, int threshold, int thresholdRatio);

int applyFilters(Pgm *pgmIn, Pgm* pgmOut, FILE *fp);

#endif /* wrappers_h */
