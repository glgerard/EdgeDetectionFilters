//
//  imageFilters.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 24/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "imageFilters.h"
#include "imageOperations.h"

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
    
    Filter *gaussExtFilter = gauss2DFilter(sigmaExt, dim);
    Filter *gaussIntFilter = gauss2DFilter(sigmaInt, dim);
    
    Filter *filter = linearAddFilter(gaussExtFilter, gaussIntFilter, 1.0, -1.0);
    
    return filter;
}


//--------------------------------------------------------//
//----- Convolve an image submatrix with a filter --------//
//--------------------------------------------------------//
int convolution2DKernel(Pgm* pgmIn, double* kernel, int borderX, int borderY, int ic)
{
    double sum = 0;
    
    int width = pgmIn->width;
    
    int ix = 0;
    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++)
            sum += pgmIn->pixels[il+l]*kernel[ix++];
    
    return (int)floor(sum);
}

//--------------------------------------------------------//
//--------------- Apply a 2D convolution -----------------//
//--------------------------------------------------------//

int convolution2DPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
{
    return fapplyPGM(pgmIn, pgmOut, filter, 0, 0, convolution2DKernel);
}

//--------------------------------------------------------//
//--------------- Apply a 1D X convolution ---------------//
//--------------------------------------------------------//
int convolution1DXPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
{
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    double sum;
    int pixelVal;
    int topVal = 0;
    
    int ix; // the index in the filter
    int ic; // the index of the central pixel in the source image
    int il; // the index of the pixel used in the integration
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    int filterWidth = filter->width;
    int halfFilterWidth = filterWidth/2;
    
    // Move to the first useful interior pixel
    ic = halfFilterWidth;
    D(fprintf(stderr,"w=%d,h=%d\n",width,height));
    
    // Loop over all internal source image pixels
    for (int row = 0; row < height; row++) {
        D(fprintf(stderr,"start:row=%d,ic=%d\n",row,ic));
        for (int col = halfFilterWidth; col < width-halfFilterWidth; col++) {
            // compute the initial neighoboring pixel index to use in the convolution
            il = ic-halfFilterWidth;
            sum = 0;
            ix = 0;
            
            // Iterate over all filter pixels
            D(fprintf(stderr,"il=%d\n", il));
            for (int l=0; l < filterWidth; l++)
                sum += pgmIn->pixels[il++]*filter->kernel[ix++];
            
            // output the value of the convolution in the destination image
            pixelVal = (int)floor(sum);
            pgmOut->pixels[ic++] = pixelVal;
            if (pixelVal > topVal)
                topVal = pixelVal;
        }
        D(fprintf(stderr,"end:row=%d,ic=%d\n",row,ic));
        // move the index of the central pixel to the next row
        ic += halfFilterWidth*2;
    }
    
    pgmOut->max_val = topVal;
    
    return 0;
}

//--------------------------------------------------------//
//--------------- Apply a 1D Y convolution ---------------//
//--------------------------------------------------------//
int convolution1DYPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
{
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    double sum;
    int pixelVal;
    int topVal = 0;
    
    int ix;  // the index in the filter
    int ic; // the index of the central pixel in the source image
    int il; // the index of the pixel used in the integration
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    int filterHeight = filter->height;
    
    int halfFilterHeight = filterHeight/2;
    int rowShift = halfFilterHeight*width;
    
    // Move to the first useful interior pixel
    ic = rowShift;
    D(fprintf(stderr,"w=%d,h=%d\n",width,height));
    
    // Loop over all internal source image pixels
    for (int row = halfFilterHeight; row < height-halfFilterHeight; row++) {
        D(fprintf(stderr,"start:row=%d,ic=%d\n",row,ic));
        for (int col = 0; col < width; col++) {
            // compute the initial neighoboring pixel index to use in the convolution
            il = ic-rowShift;
            sum = 0;
            ix = 0;
            // Iterate over all filter pixels
            for (int k=0; k < filterHeight; k++) {
                D(fprintf(stderr,"k=%d,il=%d\n", k, il));
                sum += pgmIn->pixels[il]*filter->kernel[ix++];
                // move the index of the neighboring pixel to the next row
                il += width;
            }
            // output the value of the convolution in the destination image
            pixelVal = (int)floor(sum);
            pgmOut->pixels[ic++] = pixelVal;
            if (pixelVal > topVal)
                topVal = pixelVal;
        }
        D(fprintf(stderr,"end:row=%d,ic=%d\n",row,ic));
    }
    
    pgmOut->max_val = topVal;
    return 0;
}