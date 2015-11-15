/*! \file imageContours.h
 *  \brief Interfaces to the contour detection functions.
 *  \author Eleonora Maria Aiello
 *  \author Gianluca Gerard
 *  \date 13/11/15
 *  \copyright Apache License Version 2.0, January 2004
 */

#ifndef imageContours_h
#define imageContours_h

#include <stdio.h>
#include <sys/time.h>
#include "helperFunctions.h"
#include "imageUtilities.h"
#include "imageFilters.h"
#include "imageBasicOps.h"

int contourUniformPGM(Pgm* pgmIn, Pgm* pgmOut);
int contourN8IntPGM(Pgm* pgmIn, Pgm* pgmOut);
int connectivityPGM(Pgm *pgmNH, Pgm *pgmNL, Pgm *pgmOut);

#endif /* imageContours_h */
