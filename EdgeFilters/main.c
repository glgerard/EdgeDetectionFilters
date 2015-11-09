//
//  main.c
//  EdgeDetection
//
//  Created by Gianluca Gerard on 11/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include "imageUtilities.h"
#include "imageFilters.h"
#include "imageOperations.h"
#include "test.h"

#define MAXBUF 4096
#define TRUE 1
#define FALSE 0

int removeExt(char* src)
{
    for (int i = (int)strlen(src); i != 0; i--) {
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
    char pname[MAXBUF];

    int c;
    int oflag = FALSE;
    FILE *fp = NULL;
    char *filename;
    
    char inputFile[MAXBUF];
    char outputFile[MAXBUF];
    char command[MAXBUF];

    while ( (c = getopt(argc, argv, "f:o:")) != -1) {
        switch (c) {
            case 'f':
                filename = basename(optarg);
                strlcpy(command, filename, sizeof(command));
                removeExt(command);
                fp = fopen(optarg, "rb");
                if (fp == NULL) {
                    printf("Command file \"%s\" not found.\n", optarg);
                    exit(1);
                }
                break;
            case 'o':
                oflag = TRUE;
                strlcpy(outputFile, optarg, sizeof(outputFile));
                break;
            default:
                break;
        }
    }

    if (fp == NULL) {
        printf("Error! No command file name\n");
        exit(1);
    }

    argc -= optind;
    argv += optind;

    if(argc == 0)
    {
        printf("Error! No image file name\n");
        exit(1);
    }
    
    strlcpy(inputFile, argv[0], sizeof(inputFile));
    Pgm *imgIn = readPGM(inputFile);
    
    if(imgIn == NULL)
        exit(2);
    
    srandom(357);
    
    if (oflag == FALSE) {
        strlcpy(outputFile, argv[0], sizeof(outputFile));
        removeExt(outputFile);
    }
    
    Pgm* imgOut = applyFilters(imgIn, fp);
    
    sprintf(pname,"%s_%s.pgm", outputFile, command);
    writePGM(imgOut,pname);
    
    // calculate histogram and write it in a file
    // calcHist(imgOut);
    
    freePGM(&imgIn);
    freePGM(&imgOut);
    
    return 0;
}
