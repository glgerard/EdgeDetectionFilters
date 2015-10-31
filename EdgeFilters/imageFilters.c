//
//  imageFilters.c
//  ImageCV
//
//  Created by Gianluca Gerard on 24/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "imageFilters.h"


//---------------------------------------------------------//
//------------- Create a new empty filter -----------------//
//---------------------------------------------------------//
Filter* newFilter(int width, int height)
{
    Filter* newFilter = (Filter*)malloc(1*sizeof(Filter));
    newFilter->width = width;
    newFilter->height = height;
    newFilter->kernel = (double*) calloc(width*height, sizeof(double));
    
    return newFilter;
}

//---------------------------------------------------------//
//----------------- Free Filter structure -----------------//
//---------------------------------------------------------//
void freeFilter(Filter** filter)
{
    free((*filter)->kernel);
    (*filter)->kernel = NULL;
    free(*filter);
    *filter = NULL;
}
//--------------------------------------------------------//
//--------------- Linear Sum of Filters ------------------//
//--------------------------------------------------------//
Filter *linearAddFilter(Filter* filterOp1, Filter* filterOp2, double w1, double w2)
{
    if(!filterOp1 || !filterOp2 )
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return NULL;
    }
    
    int width = filterOp1->width;
    int height = filterOp1->height;
    
    Filter* filter = newFilter(width,height);

    for (int i = 0; i < width*height; i++) {
        filter->kernel[i] = w1*filterOp1->kernel[i] + w2*filterOp2->kernel[i];
    }
    
    return filter;
}

//--------------------------------------------------------//
//-------------------- Identity Filter -------------------//
//--------------------------------------------------------//

Filter* identityFilter(int width, int height)
{
    Filter* filter = newFilter(width,height);
    for (int i=0; i<height*width; i++)
        filter->kernel[i] = 0.0;
    filter->kernel[width*height/2] = 1.0;
    return filter;
}

//--------------------------------------------------------//
//-------------------- Box  Filter -----------------------//
//--------------------------------------------------------//
Filter* boxFilter(int width, int height)
{
    Filter* filter = newFilter(width,height);
    double weigth = 1.0/(width*height);
    for (int i=0; i<height*width; i++)
        filter->kernel[i] = weigth;
    return filter;
}

//--------------------------------------------------------//
//-------------------- Generic Filter --------------------//
//--------------------------------------------------------//
Filter* genericFilter(const int* matrix, int width, int height) {
    Filter* filter = newFilter(width, height);
    for (int i=0; i<height*width; i++)
        filter->kernel[i] = matrix[i];
    return filter;
}

//--------------------------------------------------------//
//------------------- Prewitt Filter Gx ------------------//
//--------------------------------------------------------//
Filter* prewittXFilter()
{
    const int sobel[] = {1,1,1,0,0,0,-1,-1,-1};
    return genericFilter(sobel,3,3);
}

//--------------------------------------------------------//
//------------------- Prewitt Filter Gy ------------------//
//--------------------------------------------------------//
Filter* prewittYFilter()
{
    const int sobel[] = {1,0,-1,1,0,-1,1,0,-1};
    return genericFilter(sobel,3,3);
}

//--------------------------------------------------------//
//-------------------   Sobel Filter Gx    ---------------//
//--------------------------------------------------------//
Filter* sobelXFilter()
{
    const int sobel[] = {1,2,1,0,0,0,-1,-2,-1};
    return genericFilter(sobel,3,3);
}

//--------------------------------------------------------//
//-------------------   Sobel Filter Gy    ---------------//
//--------------------------------------------------------//
Filter* sobelYFilter()
{
    const int sobel[] = {1,0,-1,2,0,-2,1,0,-1};
    return genericFilter(sobel,3,3);
}

