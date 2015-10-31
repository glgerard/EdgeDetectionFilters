//
//  main.c
//  ImageCV
//
//  Created by Gianluca Gerard on 11/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "imageUtilities.h"
#include "imageFilters.h"
#include "imageOperations.h"

#define MAXBUF 4096

int removeExt(char* src)
{
    for (int i = sizeof(src); i != 0; i--) {
        if (src[i] == '.') {
            src[i] = '\0';
            break;
        }
    }
    return 0;
}

int main(int argc, char** argv)
{
    char inputFile[MAXBUF];
    char outputFile[MAXBUF];
    char pname[MAXBUF];
    
    Filter* filter;
    
    if(argc == 1)
    {
        printf("Error! No file name\n");
        exit(1);
    }
    
    strlcpy(inputFile, argv[1], sizeof(inputFile));
    Pgm *imgIn = readPGM(inputFile);
    
    if(imgIn == NULL)
        exit(2);
    
    if(argc==2) {
        strlcpy(outputFile, argv[1], sizeof(outputFile));
        removeExt(outputFile);
    }
    else
        strlcpy(outputFile, argv[2], sizeof(outputFile));
    
    srandom(357);
    
    Pgm* imgOut = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut1 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut2 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOut3 = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
/***
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
***/
    
    // apply Uniform noise
    resetPGM(imgOut);
    addUniformNoisePGM(imgIn, imgOut, 32);
    sprintf(pname,"%s_uninoise.pgm", outputFile);
    writePGM(imgOut,pname); 

    // apply Salt & Pepper noise
    resetPGM(imgOut);
    addSaltPepperNoisePGM(imgIn, imgOut, 0.05);
    sprintf(pname,"%s_snp.pgm", outputFile);
    writePGM(imgOut,pname);

    // apply a identity flter
    resetPGM(imgOut1);
    Filter *idFilter = identityFilter(3,3);
    convolutionPGM(imgIn, imgOut1, idFilter);
    sprintf(pname,"%s_copy.pgm", outputFile);
    resetPGM(imgOut);
    normalizePGM(imgOut1, imgOut);
    writePGM(imgOut,pname);
    
    // apply a box filter
    resetPGM(imgOut2);
    Filter *bxFilter = boxFilter(3,3);
    convolutionPGM(imgIn, imgOut2, bxFilter);
    sprintf(pname,"%s_box.pgm", outputFile);
    resetPGM(imgOut);
    normalizePGM(imgOut2, imgOut);
    writePGM(imgOut,pname);

    // sharpening

    filter = linearAddFilter(idFilter, bxFilter, 2.0, -1.0);
    freeFilter(&idFilter);
    freeFilter(&bxFilter);
    resetPGM(imgOut1);
    convolutionPGM(imgIn, imgOut1, filter);
    sprintf(pname,"%s_sharp.pgm", outputFile);
    resetPGM(imgOut);
    normalizePGM(imgOut1, imgOut);
    writePGM(imgOut,pname);
    freeFilter(&filter);
    
    // apply a sobel horizontal filter
    resetPGM(imgOut1);
    filter = sobelXFilter();
    convolutionPGM(imgIn, imgOut1, filter);
    sprintf(pname,"%s_sobelH_T.pgm", outputFile);
    resetPGM(imgOut3);
    absolutePGM(imgOut1, imgOut3);
    resetPGM(imgOut);
    thresholdPGM(imgOut3, imgOut);
    resetPGM(imgOut3);
    normalizePGM(imgOut, imgOut3);
    writePGM(imgOut3,pname);
    freeFilter(&filter);
    
    // apply a sobel vertical filter
    resetPGM(imgOut2);
    filter = sobelYFilter();
    convolutionPGM(imgIn, imgOut2, filter);
    sprintf(pname,"%s_sobelV_T.pgm", outputFile);
    resetPGM(imgOut3);
    absolutePGM(imgOut2, imgOut3);
    resetPGM(imgOut);
    thresholdPGM(imgOut3, imgOut);
    resetPGM(imgOut3);
    normalizePGM(imgOut, imgOut3);
    writePGM(imgOut3,pname);
    freeFilter(&filter);

    // compute the module of the two images with the
    // applied sobel filters
    resetPGM(imgOut);
    modulePGM(imgOut1, imgOut2, imgOut);
    sprintf(pname,"%s_sobel_mod_T.pgm", outputFile);
    resetPGM(imgOut3);
    thresholdPGM(imgOut, imgOut3);
    resetPGM(imgOut);
    normalizePGM(imgOut3, imgOut);
    writePGM(imgOut,pname);

    // compute the phase of the two images with the
    // applied sobel filters
    resetPGM(imgOut);
    phasePGM(imgOut1, imgOut2, imgOut);
    sprintf(pname,"%s_sobel_phi.pgm", outputFile);
    resetPGM(imgOut3);
    normalizePGM(imgOut, imgOut3);
    writePGM(imgOut3,pname);

    // add the two sobel filters
    resetPGM(imgOut);
    resetPGM(imgOut3);
    absolutePGM(imgOut2, imgOut3);
    resetPGM(imgOut2);
    absolutePGM(imgOut1, imgOut2);
    linearAddPGM(imgOut2, imgOut3, 1.0, 1.0, imgOut);
    sprintf(pname,"%s_sobel_T.pgm", outputFile);
    resetPGM(imgOut3);
    thresholdPGM(imgOut, imgOut3);
    resetPGM(imgOut);
    normalizePGM(imgOut3, imgOut);
    writePGM(imgOut,pname);
    
    freePGM(&imgOut1);
    freePGM(&imgOut2);
    freePGM(&imgOut3);

    // calculate histogram and write it in a file
    int i;
    int* histogram = histogramPGM(imgIn);
    FILE* hist = fopen("histogram.dat", "w");
    
    for(i=0; i<imgIn->max_val+1; i++)
    {
        printf("HISTOGRAM[%d] = %d\n", i, histogram[i]);
        fprintf(hist, "%d %d\n", i, histogram[i]);
    }
    fclose(hist);
    free(histogram);
 
    freePGM(&imgIn);
    freePGM(&imgOut);
    
    return 0;
    
}