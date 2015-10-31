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
int thresholdPGM(Pgm* pgmIn, Pgm* pgmOut)
{
    if(!pgmIn | !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int pixelVal;
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    for (int i = 0; i < width*height; i++) {
        pixelVal = pgmIn->pixels[i];
        if (pixelVal>255) {
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
//--------------- Apply a 2D convolution -----------------//
//--------------------------------------------------------//
int convolution2DPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
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
    
    int filterWidth = filter->width;
    int filterHeight = filter->height;
    
    int halfFilterHeight = filterHeight/2;
    int rowShift = halfFilterHeight*width;
    int halfFilterWidth = filterWidth/2;
    
    // Move to the first useful interior pixel
    ic = rowShift+halfFilterWidth;
    D(fprintf(stderr,"w=%d,h=%d\n",width,height));
    
    // Loop over all internal source image pixels
    for (int row = halfFilterHeight; row < height-halfFilterHeight; row++) {
        D(fprintf(stderr,"start:row=%d,ic=%d\n",row,ic));
        for (int col = halfFilterWidth; col < width-halfFilterWidth; col++) {
            // compute the initial neighoboring pixel index to use in the convolution
            il = ic-rowShift-halfFilterWidth;
            sum = 0;
            ix = 0;
            // Iterate over all filter pixels
            for (int k=0; k < filterHeight; k++) {
                D(fprintf(stderr,"k=%d,il=%d\n", k, il));
                for (int l=0; l < filterWidth; l++)
                    sum += pgmIn->pixels[il++]*filter->kernel[ix++];
                // move the index of the neighboring pixel to the next row
                il += rowShift-filterWidth;
            }
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
    
    int ix;  // the index in the filter
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
                il += rowShift;
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
        if (drandom()>density) {
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
