//
//  test.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 02/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "test.h"
#include "imageFilters.h"
#include "imageBasicOps.h"
#include "imageFilterOps.h"

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
    
    // apply a box filter
    Filter *bxFilter = boxFilter(3,3);
    
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
    
    // apply a sobel vertical filter
    resetPGM(imgOut2);
    filter = sobelYFilter();
    convolution2DPGM(imgIn, imgOut2, filter);
    freeFilter(&filter);

    // compute the module of the two images with the
    // applied sobel filters
    resetPGM(imgOut);
    modulePGM(imgOut1, imgOut2, imgOut);
    normalizePGM(imgOut, imgOut3);

    sprintf(pname,"%s_sobel_mod.pgm", outputFile);
    writePGM(imgOut3,pname);
    
    resetPGM(imgOut);
    thresholdPGM(imgOut3, imgOut, 255);
    
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

int testDoG(Pgm* imgIn, char* outputFile)
{
    char pname[MAXBUF];
    Filter* filter;

    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);

    fprintf(stderr, "\nDoG filtering (sigma = 6)\n");
    filter = DoGFilter(6, 0);
    // printFilter(filter);
    
    convolution2DPGM(imgIn, imgOut, filter);
    thresholdPGM(imgOut, imgOut1, 0);
    resetPGM(imgOut);
    invertPGM(imgOut1, imgOut);
    sprintf(pname,"%s_dog_6_thr.pgm", outputFile);
    writePGM(imgOut, pname);
    
    resetPGM(imgOut1);
    contourN8IntPGM(imgOut, imgOut1);
    
    sprintf(pname,"%s_dog_6.pgm", outputFile);
    writePGM(imgOut1, pname);
    
    freeFilter(&filter);
    freePGM(&imgOut);
    freePGM(&imgOut1);
    
    return 0;
}

int testNoise(Pgm *imgIn, char* outputFile)
{
    char pname[MAXBUF];

    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);

    // apply Uniform noise
    addUniformNoisePGM(imgIn, imgOut, 32);
    normalizePGM(imgOut, imgOut1);
    sprintf(pname,"%s_uninoise.pgm", outputFile);
    writePGM(imgOut1,pname);
    resetPGM(imgOut);
    resetPGM(imgOut1);
    
    // apply Salt & Pepper noise
    addSaltPepperNoisePGM(imgIn, imgOut, 0.05);
    sprintf(pname,"%s_snp.pgm", outputFile);
    writePGM(imgOut,pname);
    
    freePGM(&imgOut);
    freePGM(&imgOut1);
    
    return 0;
}

int testDenoise(Pgm* imgIn, char* outputFile)
{
    char pname[MAXBUF];
    
    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    
    // apply a box filter to compute the average
    Filter *filter = boxFilter(3,3);
    convolution2DPGM(imgIn, imgOut, filter);
    freeFilter(&filter);

    normalizePGM(imgOut, imgOut1);
    sprintf(pname,"%s_average.pgm", outputFile);
    writePGM(imgOut1,pname);
    
    resetPGM(imgOut);
    resetPGM(imgOut1);
    
    // apply a median filter
    medianPGM(imgIn, imgOut);
    normalizePGM(imgOut, imgOut1);
    sprintf(pname,"%s_median.pgm", outputFile);
    writePGM(imgOut1,pname);
    
    freePGM(&imgOut);
    freePGM(&imgOut1);
    
    return 0;
}

int testOP39(Pgm* imgIn, char* outputFile)
{
    char pname[MAXBUF];
    
    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);

    op39PGM(imgIn, imgOut);
    
    sprintf(pname,"%s_op39.pgm", outputFile);
    writePGM(imgOut,pname);
    
    thresholdPGM(imgOut, imgOut1, 30);
    sprintf(pname,"%s_op39_T.pgm", outputFile);
    writePGM(imgOut1,pname);
    
    freePGM(&imgOut);
    freePGM(&imgOut1);
    
    return 0;
}

int testNagao(Pgm* imgIn, char* outputFile)
{
    char pname[MAXBUF];
    
    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    
    nagaoPGM(imgIn, imgOut);
    
    sprintf(pname,"%s_nagao.pgm", outputFile);
    writePGM(imgOut,pname);
    
    freePGM(&imgOut);
    
    return 0;
}

int testAll(Pgm *imgIn, char* outputFile)
{
    // test basic copy, flip, invert, normalize and equalize
    testBasicFunctions(imgIn);
    
    // test noise
    testNoise(imgIn, outputFile);

    // test sharpening
    testSharpening(imgIn, outputFile);

    // test denoise filters
    testDenoise(imgIn, outputFile);
    
    // test Sobel
    testSobel(imgIn, outputFile);
    
    // test Gauss filter
    testGauss(imgIn, outputFile);
    
    // test DoG filter
    testDoG(imgIn, outputFile);
    
    // test the 3/9 operator
    testOP39(imgIn, outputFile);
    
    // test the 3/9 operator
    testNagao(imgIn, outputFile);
    
    return 0;
}