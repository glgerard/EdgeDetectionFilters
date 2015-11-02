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

#endif /* test_h */
