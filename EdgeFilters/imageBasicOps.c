//
//  imageOperations.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 25/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "imageBasicOps.h"
#include "helperFunctions.h"
#include "imageFilterOps.h"

//--------------------------------------------------------//
//-------------- Absolute of an Image --------------------//
//--------------------------------------------------------//
int absolutePGM(Pgm* pgmIn, Pgm* pgmOut)
{
    int pixel;
    int max_val = 0;
    
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    // Iterate over all pixels
    for (int i = 0; i < width*height; i++) {
        pixel = abs(pgmIn->pixels[i]);
        pgmOut->pixels[i] = pixel;
        if ( pixel > max_val)
            max_val = pixel;
    }
    
    pgmOut->max_val = max_val;
    
    return 0;
}

//--------------------------------------------------------//
//-------------- Threshold of an Image -------------------//
//--------------------------------------------------------//
int thresholdPGM(Pgm* pgmIn, Pgm* pgmOut, int threshold)
{
    int pixel;

    if(!pgmIn | !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    // Limit the threshold value between 0 and 255
    if (threshold > 255) {
        threshold = 255;
    }
    
    if (threshold < 0) {
        threshold = 0;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    // Iterate over all pixels
    for (int i = 0; i < width*height; i++) {
        pixel = pgmIn->pixels[i];
        // Set the output value to black or white if it is
        // below or above the threshold
        if (pixel>threshold) {
            pixel=255;
        }
        if (pixel<threshold) {
            pixel=0;
        }
        pgmOut->pixels[i] = pixel;
    }
    
    pgmOut->max_val = 255;

    return 0;
}

//--------------------------------------------------------//
//-------------- Linearly Add two images -----------------//
//--------------------------------------------------------//
int linearAddPGM(Pgm* pgmOp1, Pgm* pgmOp2, double w1, double w2, Pgm* pgmOut)
{
    int pixel;
    int max_val = 0;

    if(!pgmOp1 | !pgmOp2 | !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmOp1->width;
    int height = pgmOp1->height;
    
    // Iterate over all pixels
    for (int i = 0; i < width*height; i++) {
        pixel = (int)(w1*pgmOp1->pixels[i] + w2*pgmOp2->pixels[i]);
        pgmOut->pixels[i] = pixel;
        if ( pixel > max_val)
            max_val = pixel;
    }
    
    pgmOut->max_val = max_val;
    
    return 0;
}

//--------------------------------------------------------//
//---------- Compute the module of two images ------------//
//--------------------------------------------------------//
int modulePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut)
{
    int pixel;
    int max_val = 0;
    
    if(!pgmOpX || !pgmOpY | !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmOpX->width;
    int height = pgmOpX->height;
    
    // Iterate over all pixels
    for (int i = 0; i < width*height; i++) {
        pixel = (int)sqrt(pgmOpX->pixels[i]*pgmOpX->pixels[i] +
                             pgmOpY->pixels[i]*pgmOpY->pixels[i]);
        pgmOut->pixels[i] = pixel;
        if (pixel > max_val)
            max_val = pixel;
    }
    
    pgmOut->max_val = max_val;
    
    return 0;
}

//--------------------------------------------------------//
//---------- Compute the phase of two images ------------//
//--------------------------------------------------------//
int phasePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut)
{
    int pixel;
    int max_val = 0;
    double phi;
    
    if(!pgmOpX || !pgmOpY | !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmOpX->width;
    int height = pgmOpX->height;
    
    // Iterate over all pixels
    for (int i = 0; i < width*height; i++) {
        phi = atan2(pgmOpY->pixels[i], pgmOpX->pixels[i])*M_1_PI*127;
        pixel = (int)phi;
        pgmOut->pixels[i] = pixel;
        if (pixel > max_val)
            max_val = pixel;
    }
    
    pgmOut->max_val = max_val;

    return 0;
}

//--------------------------------------------------------//
//--- Scan an image and apply a function to each pixel ---//
//--------------------------------------------------------//
int fapplyPGM(Pgm* pgmIn1, Pgm* pgmIn2, Pgm* pgmOut, Filter* filter, int borderX, int borderY,
              int (*func)(Pgm*, Pgm*, double*, int, int, int))
{
    int pixel;
    int max_val = 0;
    double *kernel = NULL; // a local pointer to the filter matrix if defined
    int ic; // the index of the central pixel in the source image
    
    if(!pgmIn1 || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn1->width;
    int height = pgmIn1->height;
    
    if (filter != NULL) {
        borderX = filter->width/2;
        borderY = filter->height/2;
        kernel = filter->kernel;
    }
    
    // Timestamp
    time_t t0 = time(0);

    // Move to the first useful interior pixel
    ic = borderY*width+borderX;
    D(fprintf(stderr,"w=%d,h=%d\n",width,height));
    D(fprintf(stderr,"bw=%d,bh=%d\n",borderX,borderY));
    
    // Loop over all internal source image pixels
    for (int row = borderY; row < height-borderY; row++, ic += borderX*2) {
        D(fprintf(stderr,"start:row=%d,ic=%d\n",row,ic));
        for (int col = borderX; col < width-borderX; col++, ic++) {
            D(fprintf(stderr,"(%d,%d),ic=%d\n", row, col, ic));
            
            // Apply the function to each pixel neighborhood
            pixel = func(pgmIn1, pgmIn2, kernel, borderX, borderY, ic);

            pgmOut->pixels[ic] = pixel;
            if (pixel > max_val)
                max_val = pixel;
        }
        D(fprintf(stderr,"end:row=%d,ic=%d\n",row,ic));
        // move the index of the central pixel to the next row
    }
    
    pgmOut->max_val = max_val;
    
    // Timestamp
    time_t t1 = time(0);
    double datetime_diff_ms = difftime(t1, t0) * 1000.;
    fprintf(stderr, "\nElapsed time (msec): %f\n", datetime_diff_ms);
    
    return 0;
}

//--------------------------------------------------------//
//----- Convolve an image submatrix with a filter --------//
//--------------------------------------------------------//
int convolution2DKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int borderX, int borderY, int ic)
{
    double sum = 0;
    
    int width = pgmIn1->width;
    
    int ix = 0;
    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++)
            sum += pgmIn1->pixels[il+l]*kernel[ix++];
    
    return (int)floor(sum);
}

//--------------------------------------------------------//
//--------------- Apply a 2D convolution -----------------//
//--------------------------------------------------------//

int convolution2DPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, filter, 0, 0, convolution2DKernel);
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

//--------------------------------------------------------//
//---------- Compute an image submatrix median -----------//
//--------------------------------------------------------//
int medianKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int borderX, int borderY, int ic)
{
    int* pixels;
    int pixel;
    int ix = 0;
    int width = pgmIn1->width;

    pixels = calloc((2*borderX+1)*(2*borderY+1), sizeof(int));
    
    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++)
            pixels[ix++] = pgmIn1->pixels[il+l];
    
    int nPixels = ix;
    
    // Sort the pixels
    pixels = sort(pixels, nPixels);
    
    // Store the pixel in the middle of the sorted list
    pixel = pixels[nPixels/2];
    
    free(pixels);
    
    return pixel;
}

//--------------------------------------------------------//
//--------------    Apply a median filter    -------------//
//--------------------------------------------------------//
int medianPGM(Pgm *pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 1, 1, medianKernel);
}

//--------------------------------------------------------//
//-------------    Apply an average filter   -------------//
//--------------------------------------------------------//
int averagePGM(Pgm *pgmIn, Pgm* pgmOut)
{
    // apply a box filter
    Filter *bxFilter = boxFilter(3,3);
    printFilter(bxFilter);
    
    int ret = convolution2DPGM(pgmIn, pgmOut, bxFilter);
    
    freeFilter(&bxFilter);
    
    return ret;
}