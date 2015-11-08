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

int applySharpening(Pgm* imgIn, Pgm* imgOut);
int applyGauss(Pgm* imgIn, Pgm* imgOut, double sigma, int dim);
int applySobel(Pgm* pgmIn, Pgm* pgmOut, int iarg);
int applyDoG(Pgm* imgIn, Pgm* imgOut, double sigma, int dim);

#endif /* wrappers_h */
