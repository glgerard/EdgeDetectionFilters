//
//  test.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 02/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "test.h"
#include "imageFilters.h"
#include "imageOperations.h"

#define MAXBUF 4096

int testBasicFunctions(Pgm* imgIn)
{
    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    
    // copy the image
    copyPGM(imgIn,imgOut);
    writePGM(imgOut,"copy.pgm");
    // flip the image
    resetPGM(imgOut);
    hflipPGM(imgIn,imgOut);
    writePGM(imgOut,"flip.pgm");
    // invert the image
    resetPGM(imgOut);
    invertPGM(imgIn,imgOut);
    writePGM(imgOut,"invert.pgm");
    
    // normalize the image
    resetPGM(imgOut);
    normalizePGM(imgIn, imgOut);
    writePGM(imgOut,"normalize.pgm");
    
    // normalize the image
    resetPGM(imgOut);
    equalizePGM(imgIn, imgOut);
    writePGM(imgOut,"equalize.pgm");
    
    freePGM(&imgOut);
    
    return 0;
}

int testSharpening(Pgm* imgIn, char* outputFile)
{
    char pname[MAXBUF];
    Filter* filter;
    
    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut2 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    
    // apply a identity filter
    Filter *idFilter = identityFilter(3,3);
    
    /**
    resetPGM(imgOut1);
    convolution2DPGM(imgIn, imgOut1, idFilter);
    resetPGM(imgOut);
    normalizePGM(imgOut1, imgOut);
    sprintf(pname,"%s_copy.pgm", outputFile);
    writePGM(imgOut,pname);
    **/
    
    // apply a box filter
    Filter *bxFilter = boxFilter(3,3);
    
    /**
    resetPGM(imgOut2);
    convolution2DPGM(imgIn, imgOut2, bxFilter);
    resetPGM(imgOut);
    normalizePGM(imgOut2, imgOut);
    sprintf(pname,"%s_box.pgm", outputFile);
    writePGM(imgOut,pname);
    **/
    
    // sharpening
    
    filter = linearAddFilter(idFilter, bxFilter, 2.0, -1.0);
    freeFilter(&idFilter);
    freeFilter(&bxFilter);
    
    resetPGM(imgOut1);
    convolution2DPGM(imgIn, imgOut1, filter);
    freeFilter(&filter);

    resetPGM(imgOut);
    normalizePGM(imgOut1, imgOut);
    
    sprintf(pname,"%s_sharp.pgm", outputFile);
    writePGM(imgOut,pname);
    
    freePGM(&imgOut);
    freePGM(&imgOut1);
    freePGM(&imgOut2);
    
    return 0;
}

int testSobel(Pgm* imgIn, char* outputFile)
{
    char pname[MAXBUF];
    Filter* filter;

    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut2 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut3 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    
    // apply a sobel horizontal filter
    resetPGM(imgOut1);
    filter = sobelXFilter();
    convolution2DPGM(imgIn, imgOut1, filter);
    freeFilter(&filter);

    /**
    resetPGM(imgOut3);
    absolutePGM(imgOut1, imgOut3);
    resetPGM(imgOut);
    thresholdPGM(imgOut3, imgOut);
    resetPGM(imgOut3);
    normalizePGM(imgOut, imgOut3);
    
    sprintf(pname,"%s_sobelH_T.pgm", outputFile);
    writePGM(imgOut3,pname);
    **/
    
    // apply a sobel vertical filter
    resetPGM(imgOut2);
    filter = sobelYFilter();
    convolution2DPGM(imgIn, imgOut2, filter);
    freeFilter(&filter);

    /**
    resetPGM(imgOut3);
    absolutePGM(imgOut2, imgOut3);
    resetPGM(imgOut);
    thresholdPGM(imgOut3, imgOut);
    resetPGM(imgOut3);
    normalizePGM(imgOut, imgOut3);
    
    sprintf(pname,"%s_sobelV_T.pgm", outputFile);
    writePGM(imgOut3,pname);
    **/
    
    // compute the module of the two images with the
    // applied sobel filters
    resetPGM(imgOut);
    modulePGM(imgOut1, imgOut2, imgOut);
    resetPGM(imgOut3);
    thresholdPGM(imgOut, imgOut3);
    resetPGM(imgOut);
    normalizePGM(imgOut3, imgOut);
    
    sprintf(pname,"%s_sobel_mod_T.pgm", outputFile);
    writePGM(imgOut,pname);
    
    // compute the phase of the two images with the
    // applied sobel filters
    resetPGM(imgOut);
    phasePGM(imgOut1, imgOut2, imgOut);
    resetPGM(imgOut3);
    normalizePGM(imgOut, imgOut3);
    
    sprintf(pname,"%s_sobel_phi.pgm", outputFile);
    writePGM(imgOut3,pname);
    
    /**
    // add the two sobel filters
    resetPGM(imgOut);
    resetPGM(imgOut3);
    absolutePGM(imgOut2, imgOut3);
    resetPGM(imgOut2);
    absolutePGM(imgOut1, imgOut2);
    linearAddPGM(imgOut2, imgOut3, 1.0, 1.0, imgOut);
    resetPGM(imgOut3);
    thresholdPGM(imgOut, imgOut3);
    resetPGM(imgOut);
    normalizePGM(imgOut3, imgOut);
    
    sprintf(pname,"%s_sobel_T.pgm", outputFile);
    writePGM(imgOut,pname);
    **/
    
    freePGM(&imgOut);
    freePGM(&imgOut1);
    freePGM(&imgOut2);
    freePGM(&imgOut3);
    
    return 0;
}

int testGauss(Pgm* imgIn, char* outputFile)
{
    char pname[MAXBUF];
    Filter* filter;
    
    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);

    resetPGM(imgOut);
    resetPGM(imgOut1);
    
    // linear X guassian filter
    filter = gauss1DXFilter(1, 0);
    // printFilter(filter);

    convolution1DXPGM(imgIn, imgOut, filter);
    
    // linear Y guassian filter
    filter = gauss1DYFilter(1, 0);
    // printFilter(filter);

    convolution1DYPGM(imgOut, imgOut1, filter);
    
    sprintf(pname,"%s_gauss.pgm", outputFile);
    writePGM(imgOut1, pname);

    freeFilter(&filter);
    freePGM(&imgOut);
    freePGM(&imgOut1);
    
    return 0;
}