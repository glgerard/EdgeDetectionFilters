//
//  imageFilters.h
//  ImageCV
//
//  Created by Gianluca Gerard on 24/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#ifndef imageFilters_h
#define imageFilters_h

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "imageUtilities.h"

typedef struct
{
    int width;
    int height;
    double* kernel;
} Filter;

//---------------------------------------------------------//
//---------------- Basic Functions for Filter -------------//
//---------------------------------------------------------//

Filter* newFilter(int width, int height);
void freeFilter(Filter** filter);
void printFilter(Filter* filter);

Filter *linearAddFilter(Filter* filterOp1, Filter* filterOp2, double w1, double w2);

//---------------------------------------------------------//
//---------------------- Basic Filters --------------------//
//---------------------------------------------------------//
Filter* identityFilter(int width, int height);
Filter* boxFilter(int width, int height);
Filter* prewittXFilter();
Filter* prewittYFilter();
Filter* sobelXFilter();
Filter* sobelYFilter();

Filter* gauss1DXFilter(double sigma, int width);
Filter* gauss1DYFilter(double sigma, int height);

#endif /* imageFilters_h */