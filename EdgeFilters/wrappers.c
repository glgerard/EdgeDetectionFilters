//
//  wrappers.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 07/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "wrappers.h"
#include "imageFilters.h"
#include "imageOperations.h"

int applySharpening(Pgm* imgIn, Pgm* imgOut)
{
    Filter* filter;
    
    // create a identity filter
    Filter *idFilter = identityFilter(3,3);
    
    // create a box filter
    Filter *bxFilter = boxFilter(3,3);
    
    // sharpening
    
    filter = linearAddFilter(idFilter, bxFilter, 2.0, -1.0);
    freeFilter(&idFilter);
    freeFilter(&bxFilter);
    
    resetPGM(imgOut);

    convolution2DPGM(imgIn, imgOut, filter);
    freeFilter(&filter);
    
    return 0;
}

int applyGauss(Pgm* imgIn, Pgm* imgOut, double sigma, int dim)
{
    Filter* filter;
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    
    // linear X guassian filter
    filter = gauss1DXFilter(sigma, dim);
    
    convolution1DXPGM(imgIn, imgOut1, filter);
    
    freeFilter(&filter);

    // linear Y guassian filter
    filter = gauss1DYFilter(sigma, dim);
    
    convolution1DYPGM(imgOut1, imgOut, filter);

    freeFilter(&filter);
    freePGM(&imgOut1);
    
    return 0;
}

int applySobel(Pgm* imgIn, Pgm* imgOut, int eval)
{
    Filter* filter;
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut2 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    
    // apply a sobel horizontal filter
    filter = sobelXFilter();
    convolution2DPGM(imgIn, imgOut1, filter);
    freeFilter(&filter);
    
    // apply a sobel vertical filter
    filter = sobelYFilter();
    convolution2DPGM(imgIn, imgOut2, filter);
    freeFilter(&filter);
    
    // compute the module of the two images with the
    // applied sobel filters
    resetPGM(imgOut);
    
    if (eval == 0) {
        modulePGM(imgOut1, imgOut2, imgOut);
    } else
        phasePGM(imgOut1, imgOut2, imgOut);
    
    freePGM(&imgOut1);
    freePGM(&imgOut2);
    
    return 0;
}

int applyPrewitt(Pgm* imgIn, Pgm* imgOut, int eval)
{
    Filter* filter;
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut2 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    
    // apply a sobel horizontal filter
    filter = prewittXFilter();
    convolution2DPGM(imgIn, imgOut1, filter);
    freeFilter(&filter);
    
    // apply a sobel vertical filter
    filter = prewittYFilter();
    convolution2DPGM(imgIn, imgOut2, filter);
    freeFilter(&filter);
    
    // compute the module of the two images with the
    // applied sobel filters
    resetPGM(imgOut);
    
    if (eval == 0) {
        modulePGM(imgOut1, imgOut2, imgOut);
    } else
        phasePGM(imgOut1, imgOut2, imgOut);
    
    freePGM(&imgOut1);
    freePGM(&imgOut2);
    
    return 0;
}

int applyDoG(Pgm* imgIn, Pgm* imgOut, double sigma, int dim)
{
    Filter* filter;
    
    fprintf(stderr, "\nDoG filtering (sigma = %f)\n",sigma);
    filter = DoGFilter(sigma, dim);
    
    resetPGM(imgOut);
    convolution2DPGM(imgIn, imgOut, filter);
    
    freeFilter(&filter);
    
    return 0;
}

int applyCED(Pgm* imgIn, Pgm* imgOut, double sigma, int dim, int threshold, int thresholdRatio)
{
    int threshold_high = threshold;
    int threshold_low = 0;
    
    Pgm* imgOutX = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOutY = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOutMod = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOutPhi = newPGM(imgIn->width, imgIn->height, imgIn->max_val);

    applyGauss(imgIn, imgOut, sigma, dim);
    
    Filter* gx = prewittXFilter();
    Filter* gy = prewittYFilter();
    
    convolution2DPGM(imgOut, imgOutX, gx);
    convolution2DPGM(imgOut, imgOutY, gy);
    
    modulePGM(imgOutX, imgOutY, imgOutMod);
    phasePGM(imgOutX, imgOutY, imgOutPhi);
    
    resetPGM(imgOut);
    
    suppressionPGM(imgOutMod, imgOutPhi, imgOut);

    freePGM(&imgOutX);
    freePGM(&imgOutY);
    freePGM(&imgOutMod);
    freePGM(&imgOutPhi);
    
    if (thresholdRatio == 0) {
        threshold_low = threshold/2;
    } else
        threshold_low = threshold/3;
    
    Pgm *imgNH = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm *imgNLshadow = newPGM(imgIn->width, imgIn->height, imgIn->max_val);

    thresholdPGM(imgOut, imgNH, threshold_high);
    thresholdPGM(imgOut, imgNLshadow, threshold_low);
    
    Pgm *imgNL = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    linearAddPGM(imgNLshadow, imgNH, 1.0, -1.0, imgNL);
    
    Histogram *hist;
    int change = 0;
    int size = imgNH->width*imgNH->height;
    
    while (!change) {
        resetPGM(imgNLshadow);
        connectivityPGM(imgNH, imgNL, imgNLshadow);
        
        resetPGM(imgOut);
        linearAddPGM(imgNH, imgNLshadow, 1.0, 1.0, imgOut);
        
        copyPGM(imgNH, imgNLshadow);
        
        thresholdPGM(imgOut, imgNH, threshold_high);
        
        linearAddPGM(imgNH, imgNLshadow, 1.0, -1.0, imgOut);
        
        hist = histogramPGM(imgOut);
        
        change = hist->channels[0] == size;
        
        printf("%d\n", hist->channels[0]);
        
    }
    
    copyPGM(imgNH, imgOut);

    freePGM(&imgNLshadow);
    freePGM(&imgNH);
    freePGM(&imgNL);
    
    return 0;
}
