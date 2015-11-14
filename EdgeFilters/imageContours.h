//
//  imageContours.h
//  EdgeFilters
//
//  Created by Gianluca Gerard on 13/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#ifndef imageContours_h
#define imageContours_h

#include <stdio.h>
#include <sys/time.h>
#include "helperFunctions.h"
#include "imageUtilities.h"
#include "imageFilters.h"
#include "imageBasicOps.h"

int contour2DPGM(Pgm* pgmIn, Pgm* pgmOut);
int contourN8IntPGM(Pgm* pgmIn, Pgm* pgmOut);
int connectivityPGM(Pgm *pgmNH, Pgm *pgmNL, Pgm *pgmOut);

#endif /* imageContours_h */
