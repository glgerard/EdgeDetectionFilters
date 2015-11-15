/*! \file imageFilterOps.h
 *  \brief Interfaces to the filter operations (blurs, noises, sharpening, edge detection).
 *  \author Eleonora Maria Aiello
 *  \author Gianluca Gerard
 *  \date 07/11/15
 *  \copyright Apache License Version 2.0, January 2004
 */

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

int sobelPGM(Pgm* imgIn, Pgm* imgOut, uint8_t phase);
int prewittPGM(Pgm* pgmIn, Pgm* pgmOut, uint8_t phase);
int dogPGM(Pgm* imgIn, Pgm* imgOut, double sigma, int dim);
int cedPGM(Pgm* imgIn, Pgm* imgOut, double sigma, int dim, int threshold, int thresholdRatio);

void execImageOps(Pgm *pgmIn, Pgm* pgmOut, FILE *fp);

#endif /* imageFilterOps_H */
