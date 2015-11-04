//
//  main.c
//  EdgeDetection
//
//  Created by Gianluca Gerard on 11/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "imageUtilities.h"
#include "imageFilters.h"
#include "imageOperations.h"
#include "test.h"

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

int calcHist(Pgm* imgIn)
{
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
    
    return 0;
}

int main(int argc, char** argv)
{
    char inputFile[MAXBUF];
    char outputFile[MAXBUF];
    char pname[MAXBUF];
    
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
    
    // test basic copy, flip, invert, normalize and equalize
    // testBasicFunctions(imgIn);
    
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

    // testSharpening
    // testSharpening(imgIn, outputFile);
    
    // test Sobel
    // testSobel(imgIn, outputFile);

    // test Gauss filter
    // testGauss(imgIn, outputFile);
    
    // test DoG filter
    testDoG(imgIn, outputFile);
    
    // calculate histogram and write it in a file
    // calcHist(imgIn);
    
    freePGM(&imgIn);
    freePGM(&imgOut);
    
    return 0;
}
