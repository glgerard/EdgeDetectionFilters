//
//  imageOperations.c
//  ImageCV
//
//  Created by Gianluca Gerard on 25/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "imageOperations.h"


//--------------------------------------------------------//
//-------------- Absolute of an Image --------------------//
//--------------------------------------------------------//
int absolutePGM(Pgm* pgmIn, Pgm* pgmOut)
{
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    for (int i = 0; i < width*height; i++) {
        pgmOut->pixels[i] = abs(pgmIn->pixels[i]);
    }
    
    return 0;
}

//--------------------------------------------------------//
//-------------- Threshold of an Image -------------------//
//--------------------------------------------------------//
int thresholdPGM(Pgm* pgmIn, Pgm* pgmOut, int threshold)
{
    if(!pgmIn | !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int pixelVal;
    
    if (threshold > 255) {
        threshold = 255;
    }
    
    if (threshold < 0) {
        threshold = 0;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    pgmOut->max_val = 255;
    
    for (int i = 0; i < width*height; i++) {
        pixelVal = pgmIn->pixels[i];
        if (pixelVal>threshold) {
            pixelVal=255;
        }
        if (pixelVal<0) {
            pixelVal=0;
        }
        pgmOut->pixels[i] = pixelVal;
    }
    
    return 0;
}

//--------------------------------------------------------//
//-------------- Linearly Add two images -----------------//
//--------------------------------------------------------//
int linearAddPGM(Pgm* pgmOp1, Pgm* pgmOp2, double w1, double w2, Pgm* pgmOut)
{
    if(!pgmOp1 | !pgmOp2 | !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int pixelVal;
    
    int width = pgmOp1->width;
    int height = pgmOp1->height;
    
    for (int i = 0; i < width*height; i++) {
        pixelVal = (int)(w1*pgmOp1->pixels[i] + w2*pgmOp2->pixels[i]);
        pgmOut->pixels[i] = pixelVal;
    }
    
    return 0;
}

//--------------------------------------------------------//
//---------- Compute the module of two images ------------//
//--------------------------------------------------------//
int modulePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut)
{
    if(!pgmOpX || !pgmOpY | !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int pixelVal;
    
    int width = pgmOpX->width;
    int height = pgmOpX->height;
    
    for (int i = 0; i < width*height; i++) {
        pixelVal = (int)sqrt(pgmOpX->pixels[i]*pgmOpX->pixels[i] +
                             pgmOpY->pixels[i]*pgmOpY->pixels[i]);
        pgmOut->pixels[i] = pixelVal;
    }
    
    return 0;
}

//--------------------------------------------------------//
//---------- Compute the phase of two images ------------//
//--------------------------------------------------------//
int phasePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut)
{
    if(!pgmOpX || !pgmOpY | !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int pixelVal;
    double phi;
    
    int width = pgmOpX->width;
    int height = pgmOpX->height;
    
    for (int i = 0; i < width*height; i++) {
        phi = fabs(atan2(pgmOpY->pixels[i], pgmOpX->pixels[i]))*M_1_PI*255;
        pixelVal = (int)phi;
        pgmOut->pixels[i] = pixelVal;
    }
    
    return 0;
}

//--------------------------------------------------------//
//----------- Scan a PGM and apply a function ------------//
//--------------------------------------------------------//

int scanPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter, int borderX, int borderY,
            int (*func)(Pgm*, double*, int, int, int))
{
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int pixelVal;
    int topVal = 0;
    double *kernel = NULL;
    
    int ic; // the index of the central pixel in the source image
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    if (filter != NULL) {
        borderX = filter->width/2;
        borderY = filter->height/2;
        kernel = filter->kernel;
    }
    
    // Move to the first useful interior pixel
    ic = borderY*width+borderX;
    D(fprintf(stderr,"w=%d,h=%d\n",width,height));
    D(fprintf(stderr,"bw=%d,bh=%d\n",borderX,borderY));
    
    // Loop over all internal source image pixels
    for (int row = borderY; row < height-borderY; row++) {
        D(fprintf(stderr,"start:row=%d,ic=%d\n",row,ic));
        for (int col = borderX; col < width-borderX; col++) {
            D(fprintf(stderr,"(%d,%d),ic=%d\n", row, col, ic));
            
            pixelVal = func(pgmIn, kernel, borderX, borderY, ic);

            pgmOut->pixels[ic++] = pixelVal;
            if (pixelVal > topVal)
                topVal = pixelVal;
        }
        D(fprintf(stderr,"end:row=%d,ic=%d\n",row,ic));
        // move the index of the central pixel to the next row
        ic += borderX*2;
    }
    
    pgmOut->max_val = topVal;
    
    return 0;
}

//--------------------------------------------------------//
//----- Convolve an image submatrix with a filter --------//
//--------------------------------------------------------//
int convolution2DKernel(Pgm* pgmIn, double* kernel, int borderX, int borderY, int ic)
{
    double sum = 0;
    int ix = 0;
    
    int width = pgmIn->width;
    
    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++)
            sum += pgmIn->pixels[il+l]*kernel[ix++];
        // move the index of the neighboring pixel to the next row
    // output the value of the convolution in the destination image
    return (int)floor(sum);
}

//--------------------------------------------------------//
//--------------- Apply a 2D convolution -----------------//
//--------------------------------------------------------//

int convolution2DPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
{
    return scanPGM(pgmIn, pgmOut, filter, 0, 0, convolution2DKernel);
}

//--------------------------------------------------------//
//------ Check an eight neighbour for contour pixels -----//
//--------------------------------------------------------//
int contour2DKernel(Pgm* pgmIn, double* kernel, int borderX, int borderY, int ic)
{
    double sum = 0;
    int pixelVal = 255;
    
    int width = pgmIn->width;
    
    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++)
            sum += pgmIn->pixels[il+l];

    if (sum > 0 && sum < 255*borderX*borderY) {
        pixelVal = 0;
    }
    return pixelVal;
}

//--------------------------------------------------------//
//--------------- A simple contour check -----------------//
//--------------------------------------------------------//
int contour2DPGM(Pgm* pgmIn, Pgm* pgmOut)
{
    return scanPGM(pgmIn, pgmOut, NULL, 1, 1, contour2DKernel);
}

//--------------------------------------------------------//
//----- Check for contour pixels with N8 distance --------//
//--------------------------------------------------------//
int contourN8IntKernel(Pgm* pgmIn, double* kernel, int borderX, int borderY, int ic)
{
    // skip background pixels
    if  (pgmIn->pixels[ic] == 0 )
        return 255;
    
    int pixelVal = 255;
    int max_dist = 0;
    
    int width = pgmIn->width;
    
    // Iterate over all filter pixels
    int dist = 0;
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++) {
            if (pgmIn->pixels[il+l] == 0) {
                dist = abs(k)+abs(l);
            }
            if (dist > max_dist) {
                max_dist = dist;
            }
        }

    if (max_dist >= 1) {
        pixelVal = 0;
    }
    
    return pixelVal;
}

//--------------------------------------------------------//
//--------------- Apply a N8 contour check ---------------//
//--------------------------------------------------------//
int contourN8IntPGM(Pgm* pgmIn, Pgm* pgmOut)
{
    return scanPGM(pgmIn, pgmOut, NULL, 1, 1, contourN8IntKernel);
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
//------------------ Add uniform noise -------------------//
//--------------------------------------------------------//
int addUniformNoisePGM(Pgm* pgmIn, Pgm* pgmOut, int range)
{
    int pixelVal;
    
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    for (int i = 0; i < width*height; i++) {
        pixelVal = (random()&(2*range)-range)+pgmIn->pixels[i];
        if (pixelVal > 255) {
            pixelVal = 255;
        }
        if (pixelVal < 0) {
            pixelVal = 0;
        }
        pgmOut->pixels[i] = pixelVal;
    }
    
    return 0;
}

//--------------------------------------------------------//
//--------------- Add salt & pepper noise ----------------//
//--------------------------------------------------------//
int addSaltPepperNoisePGM(Pgm* pgmIn, Pgm* pgmOut, double density)
{
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
        
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    for (int i = 0; i < width*height; i++) {
        if (drandom()<density) {
            pgmOut->pixels[i] = pgmIn->pixels[i];
        } else {
            if (random()&01) {
                pgmOut->pixels[i] = 0;
            } else {
                pgmOut->pixels[i] = 255;
            }
        }
    }
    
    return 0;
}
