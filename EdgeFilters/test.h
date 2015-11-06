//
//  test.h
//  EdgeFilters
//
//  Created by Gianluca Gerard on 02/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#ifndef test_h
#define test_h

#include <stdio.h>
#include "imageUtilities.h"

int testBasicFunctions(Pgm* imgIn);
int testSharpening(Pgm* imgIn, char* outputFile);
int testSobel(Pgm* imgIn, char* outputFile);
int testGauss(Pgm* imgIn, char* outputFile);
int testDoG(Pgm* imgIn, char* outputFile);
int testNoise(Pgm *imgIn, char* outputFile);
int testDenoise(Pgm* imgIn, char* outputFile);
int testOP39(Pgm* imgIn, char* outputFile);
int testNagao(Pgm* imgIn, char* outputFile);

#endif /* test_h */
