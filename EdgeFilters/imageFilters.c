//
//  imageFilters.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 24/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "imageFilters.h"
#include "imageBasicOps.h"

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

//---------------------------------------------------------//
//----------------- Print Filter structure ----------------//
//---------------------------------------------------------//
void printFilter(Filter* filter)
{
    int width = filter->width;
    int height = filter->height;
    
    int il=0;
    for (int i=0; i<height; i++) {
        printf("[");
        for (int j=0; j<width; j++) {
            printf("%f ",filter->kernel[il++]);
        }
        printf("]\n");
    }
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
    const int prewitt[] = {1,1,1,0,0,0,-1,-1,-1};
    return genericFilter(prewitt,3,3);
}

//--------------------------------------------------------//
//------------------- Prewitt Filter Gy ------------------//
//--------------------------------------------------------//
Filter* prewittYFilter()
{
    const int prewitt[] = {1,0,-1,1,0,-1,1,0,-1};
    return genericFilter(prewitt,3,3);
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

//--------------------------------------------------------//
//------------------   Gauss 1D Filter   -----------------//
//--------------------------------------------------------//
Filter* gauss1DFilter(double sigma, int dim, Filter *filter)
{
    double gc = M_2_SQRTPI*M_SQRT1_2/(2*sigma);
    double sigma2 = sigma*sigma;
    
    int halfDim = dim/2;
    
    int x=-halfDim;
    for (int i=0; i<dim; i++) {
        filter->kernel[i] = gc*exp(-x*x/sigma2);
        x++;
    }
    
    return filter;
}

//--------------------------------------------------------//
//------------------   Gauss 1D X Filter   ---------------//
//--------------------------------------------------------//
Filter* gauss1DXFilter(double sigma, int width)
{
    if (width == 0)
        width = (int)(2*sigma)*2+1;
    
    Filter* filter = newFilter(width,1);
    
    return gauss1DFilter(sigma, width, filter);
}

//--------------------------------------------------------//
//------------------   Gauss 1D Y Filter   ---------------//
//--------------------------------------------------------//
Filter* gauss1DYFilter(double sigma, int height)
{
    if (height == 0)
        height = (int)(2*sigma)*2+1;

    Filter* filter = newFilter(1,height);
    
    return gauss1DFilter(sigma, height, filter);
}


//--------------------------------------------------------//
//------------------   Gauss 2D Filter   -----------------//
//--------------------------------------------------------//
Filter* gauss2DFilter(double sigma, int dim)
{
    double gc;
    double sigma2 = sigma*sigma;
    double gc0 = M_1_PI/(2*sigma2);

    if (dim == 0)
        dim = (int)(2*sigma)*2+1;
    
    Filter* filter = newFilter(dim,dim);

    int halfDim = dim/2;
    
    int il = 0;
    for (int x=-halfDim; x<=halfDim; x++) {
        gc = gc0*exp(-x*x/sigma2);
        for (int y=-halfDim; y<=halfDim; y++)
            filter->kernel[il++] = gc*exp(-y*y/sigma2);
    }
    
    return filter;
}

//--------------------------------------------------------//
//-------------------     DoG Filter   -------------------//
//--------------------------------------------------------//
Filter* DoGFilter(double sigma, int dim)
{
    double sigmaExt = sigma;
    double sigmaInt = sigma/1.66;
    
    if (dim == 0)
        dim = (int)(2*sigma)*2+1;
    
    Filter *gaussExtFilter = gauss2DFilter(sigmaExt, dim);
    Filter *gaussIntFilter = gauss2DFilter(sigmaInt, dim);
    
    Filter *filter = linearAddFilter(gaussExtFilter, gaussIntFilter, 1.0, -1.0);
    
    return filter;
}