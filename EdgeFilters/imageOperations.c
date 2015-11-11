//
//  imageOperations.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 25/10/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "imageOperations.h"
#include "helperFunctions.h"
#include "wrappers.h"

// Define the Nagao-Matsuyama matrixes
const static int n0[] = {
    0, 0, 0, 0, 0,
    0, 1, 1, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 1, 1, 0,
    0, 0, 0, 0, 0};

const static int n1[] = {
    0, 1, 1, 1, 0,
    0, 1, 1, 1, 0,
    0, 0, 1, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0};

const static int n2[] = {
    0, 0, 0, 1, 1,
    0, 0, 1, 1, 1,
    0, 0, 1, 1, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0};

const static int n3[] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 1, 1,
    0, 0, 1, 1, 1,
    0, 0, 0, 1, 1,
    0, 0, 0, 0, 0};

const static int n4[] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 1, 1, 0,
    0, 0, 1, 1, 1,
    0, 0, 0, 1, 1};

const static int n5[] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 1, 0, 0,
    0, 1, 1, 1, 0,
    0, 1, 1, 1, 0};

const static int n6[] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 1, 1, 0, 0,
    1, 1, 1, 0, 0,
    1, 1, 0, 0, 0};

const static int n7[] = {
    0, 0, 0, 0, 0,
    1, 1, 0, 0, 0,
    1, 1, 1, 0, 0,
    1, 1, 0, 0, 0,
    0, 0, 0, 0, 0};

const static int n8[] = {
    1, 1, 0, 0, 0,
    1, 1, 1, 0, 0,
    0, 1, 1, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0};

const static int* nagao[] = {
    n0, n1, n2, n3, n4, n5, n6, n7, n8
};

const int nagaoSize[9] = {9, 7, 7, 7, 7, 7, 7, 7, 7};

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
//------------------ Add uniform noise -------------------//
//--------------------------------------------------------//
int addUniformNoisePGM(Pgm* pgmIn, Pgm* pgmOut, int range)
{
    int randVal;
    
    if(!pgmIn || !pgmOut)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    // Limit the noise range between 0 and 127
    if (range < 0) {
        range = -range;
    }
    if (range > 127) {
        range = 127;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    // Iterate over all pixels
    for (int i = 0; i < width*height; i++) {
        randVal = random()%(2*range)-range;
        pgmOut->pixels[i] = pgmIn->pixels[i] + randVal;
    }
    
    pgmOut->max_val = pgmIn->max_val+range;
    
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
    
    // Iterate over all pixels
    for (int i = 0; i < width*height; i++) {
        if (drandom()>density) {
            pgmOut->pixels[i] = pgmIn->pixels[i];
        } else {
            // A density percentage of pixels will randomly
            // be transformed to black or white
            if (random()&01) {
                pgmOut->pixels[i] = 0;
            } else {
                pgmOut->pixels[i] = 255;
            }
        }
    }
    
    pgmOut->max_val = 255;
    
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
//------ Check an eight neighbour for contour pixels -----//
//--------------------------------------------------------//
int contour2DKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int borderX, int borderY, int ic)
{
    // This function computes the integral of the absolute values of the neighborhood.
    // If the integral is 0 or max_val*"area of the neighborhood" the central pixel
    // is not on the contour. Otherwise it is a contour pixel.
    int sum = 0;
    int pixel = 0;  // default output value (black, i.e. contour pixel)
    int max_int = pgmIn1->max_val*(2*borderX+1)*(2*borderY+1);
    
    int width = pgmIn1->width;
    
    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++)
            // Compute the integral of the neighborhood
            sum += abs(pgmIn1->pixels[il+l]);

    if ((sum == 0) || (sum == max_int)) {
        // The pixel is not a contour pixel and the output value is white
        pixel = 255;
    }
    
    return pixel;
}

//--------------------------------------------------------//
//--------------- A simple contour check -----------------//
//--------------------------------------------------------//
int contour2DPGM(Pgm* pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 1, 1, contour2DKernel);
}

//--------------------------------------------------------//
//-- Check for internal contour pixels with N8 distance --//
//--------------------------------------------------------//
int contourN8IntKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int borderX, int borderY, int ic)
{
    int pixel = 255;
    int min_dist = borderX+borderY;
    int dist = 0;
    int bck = pgmIn1->max_val;
    
    // skip background pixels
    if  (pgmIn1->pixels[ic] == bck )
        return 255;

    int width = pgmIn1->width;
    
    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++) {
            // Compute the distance with background pixels
            if (pgmIn1->pixels[il+l] == bck) {
                dist = max(abs(k),abs(l));
                if (dist < min_dist) {
                    min_dist = dist;
                }
            }
        }

    // The pixel has unitary distance from the background
    // therefore it belongs to the contour.
    if (min_dist == 1) {
        pixel = 0;
    }
    
    return pixel;
}

//--------------------------------------------------------//
//--------------- Apply a N8 contour check ---------------//
//--------------------------------------------------------//
int contourN8IntPGM(Pgm* pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 1, 1, contourN8IntKernel);
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

//--------------------------------------------------------//
//-------------    Compute the 3/9 operator   ------------//
//--------------------------------------------------------//
int op39Kernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int borderX, int borderY, int ic)
{
    int pixels[9];
    int I[9];
    int sum = 0;

    borderX = 1;
    borderY = 1;
    
    int width = pgmIn1->width;
    
    int total = 0;
    int ix = 0;

    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++) {
            pixels[ix] = pgmIn1->pixels[il+l];
            total += pixels[ix++];
        }
    
    if (total == 0)
        return 0;
    
    // remapping
    
    I[0] = pixels[5];
    I[1] = pixels[2];
    I[2] = pixels[1];
    I[3] = pixels[0];
    I[4] = pixels[3];
    I[5] = pixels[6];
    I[6] = pixels[7];
    I[7] = pixels[8];
    I[8] = pixels[4];
    
    double max = 0;
    for (int j=0; j<9; j++) {
        sum = I[mod((j-1),9)] + I[j] + I[mod((j+1),9)];
        if (sum > max)
            max = sum;
    }
    
    return (int)floor(382.5*(max/total-1.0/3));
}

//--------------------------------------------------------//
//--------------    Apply a 3/9 operator     -------------//
//--------------------------------------------------------//
int op39PGM(Pgm *pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 1, 1, op39Kernel);
}

//--------------------------------------------------------//
//---------- Compute the Nagao-Matsuyama filter ----------//
//--------------------------------------------------------//
int nagaoKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int borderX, int borderY, int ic)
{
    const int *np = NULL;
    int pixelVals[9];
    int ix, in;
    double v;
    double min_var = 585225;
    double sel_mean = 0;
    
    borderX = 2;
    borderY = 2;
    int width = pgmIn1->width;

    for (int n=0; n<9; n++) {
        np = nagao[n];
        ix = 0;
        in = 0;
        // Iterate over all filter pixels
        for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
            for (int l=-borderX; l <= borderX; l++, ix++) {
                if (np[ix] == 1) {
                    pixelVals[in] = pgmIn1->pixels[il+l]*np[ix];
                    in++;
                }
            }
        
        v = var(pixelVals,nagaoSize[n]);
        
        if (v < min_var) {
            min_var = v;
            sel_mean = mean(pixelVals,nagaoSize[n]);
        }
    }

    return (int)floor(sel_mean);
}

//--------------------------------------------------------//
//--------------    Apply a Nagao filter     -------------//
//--------------------------------------------------------//
int nagaoPGM(Pgm *pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 2, 2, nagaoKernel);
}

const int neighbors[] = {
    1, 7,
    2, 6,
    3, 5,
    0, 8
};

//--------------------------------------------------------//
//-------------    A suppression algorithm    ------------//
//--------------------------------------------------------//
int suppressionKernel(Pgm *pgmMod, Pgm* pgmPhi, double* kernel, int borderX, int borderY, int ic)
{
    int pixels[9];
    int ix = 0;
    
    int width = pgmMod->width;

    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++) {
            pixels[ix++] = pgmMod->pixels[il+l];
        }
    
    int q = quadrant((int)(180.0*pgmPhi->pixels[ic]/127));
    
    if((pixels[4] >= pixels[neighbors[2*q]]) && (pixels[4] >= pixels[neighbors[2*q+1]]))
        return pixels[4];
    
    return 0;
}

//--------------------------------------------------------//
//-------------  Apply a suppression kernel   ------------//
//--------------------------------------------------------//
int suppressionPGM(Pgm *pgmMod, Pgm *pgmPhi, Pgm *pgmOut)
{
    return fapplyPGM(pgmMod, pgmPhi, pgmOut, NULL, 1, 1, suppressionKernel);
}

//--------------------------------------------------------//
//-------------    A connectivity algorithm   ------------//
//--------------------------------------------------------//
int connectivityKernel(Pgm *pgmNH, Pgm* pgmNL, double* kernel, int borderX, int borderY, int ic)
{
    if (pgmNL->pixels[ic] == 0)
        return 0;
    
    int width = pgmNL->width;
    
    // Iterate over all N8 set in NH pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++)
            // There is a pixel in pgmNH connected
            if (pgmNH->pixels[il+l] != 0)
                return pgmNL->pixels[ic];
    
    return 0;
}

//--------------------------------------------------------//
//-------------  Apply a connectivity kernel   -----------//
//--------------------------------------------------------//
int connectivityPGM(Pgm *pgmNH, Pgm *pgmNL, Pgm *pgmOut)
{
    return fapplyPGM(pgmNH, pgmNL, pgmOut, NULL, 1, 1, connectivityKernel);
}

//--------------------------------------------------------//
//-------------  Apply a sequence of filters  ------------//
//--------------------------------------------------------//
int applyFilters(Pgm *pgmIn, Pgm* pgmOut, FILE *fp)
{
    char buffer[64];
    char *ch, *cmdline;
    int iarg;
    float farg;
    
    Pgm* pgmTmp = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);

    copyPGM(pgmIn, pgmOut);
    
    while (fgets(buffer,sizeof(buffer),fp)!=NULL) {
        copyPGM(pgmOut, pgmTmp);
        // Read a line till \n or 64 char
        cmdline = trimwhitespace(buffer);
        ch = strtok(cmdline, " ");
        if (strcmp(ch,"threshold")==0) {
            ch = strtok(NULL, " ");
            if (ch == NULL) {
                iarg = 0;
            } else
                iarg = atoi(ch);
            thresholdPGM(pgmTmp, pgmOut, iarg);
        } else if (strcmp(ch,"uniform_noise")==0) {
            ch = strtok(NULL, " ");
            if (ch == NULL) {
                iarg = 32;
            } else
                iarg = atoi(ch);
            addUniformNoisePGM(pgmTmp, pgmOut, iarg);
        } else if (strcmp(ch,"salt_n_pepper")==0) {
            ch = strtok(NULL, " ");
            if (ch == NULL) {
                farg = 0.05;
            } else
                sscanf(ch,"%f",&farg);
            addSaltPepperNoisePGM(pgmTmp, pgmOut, farg);
        } else if (strcmp(ch,"normalize")==0) {
            normalizePGM(pgmTmp, pgmOut);
        } else if (strcmp(ch,"equalize")==0) {
            equalizePGM(pgmTmp, pgmOut);
        } else if (strcmp(ch,"median")==0) {
            fprintf(stderr,"Start median\n");
            medianPGM(pgmTmp, pgmOut);
            fprintf(stderr,"Median completed\n");
        } else if (strcmp(ch,"average")==0) {
            averagePGM(pgmTmp, pgmOut);
        } else if (strcmp(ch,"internal_contour")==0) {
            contourN8IntPGM(pgmTmp, pgmOut);
        } else if (strcmp(ch,"operator_39")==0) {
            op39PGM(pgmTmp, pgmOut);
        } else if (strcmp(ch,"nagao")==0) {
            nagaoPGM(pgmTmp, pgmOut);
        } else if (strcmp(ch,"sharpening")==0) {
            applySharpening(pgmTmp, pgmOut);
        } else if (strcmp(ch, "prewitt")==0) {
            ch = strtok(NULL, " ");
            if (ch==NULL) {
                iarg = 0;
            } else
                if (strcmp(ch, "phase") == 0) {
                    iarg = 1;
                }
                else
                    iarg = 0;
            applyPrewitt(pgmTmp, pgmOut, iarg);
        } else if (strcmp(ch, "sobel")==0) {
            ch = strtok(NULL, " ");
            if (ch==NULL) {
                iarg = 0;
            } else
                if (strcmp(ch, "phase") == 0) {
                    iarg = 1;
                }
                else
                    iarg = 0;
            applySobel(pgmTmp, pgmOut, iarg);
        } else if (strcmp(ch, "gauss")==0) {
            ch = strtok(NULL, " ");
            if (ch==NULL) {
                farg = 1.0;
                iarg = 0;
            } else
                sscanf(ch, "%f", &farg);
                ch = strtok(NULL, " ");
                if ( ch == NULL) {
                    iarg = 0;
                }
                else
                    iarg = atoi(ch);
            applyGauss(pgmTmp, pgmOut, farg, iarg);
        } else if (strcmp(ch, "dog")==0) {
            ch = strtok(NULL, " ");
            if (ch==NULL) {
                farg = 1.0;
                iarg = 0;
            } else
                sscanf(ch, "%f", &farg);
            ch = strtok(NULL, " ");
            if ( ch == NULL) {
                iarg = 0;
            }
            else
                iarg = atoi(ch);
            applyDoG(pgmTmp, pgmOut, farg, iarg);
        } else if (strcmp(ch, "ced")==0) {
            ch = strtok(NULL, " ");
            if (ch==NULL) {
                farg = 1.0;
                iarg = 0;
            } else
                sscanf(ch, "%f", &farg);
            ch = strtok(NULL, " ");
            if ( ch == NULL) {
                iarg = 64;
            }
            else
                iarg = atoi(ch);
            applyCED(pgmTmp, pgmOut, farg, 0, iarg, 1);
        }
        
    }

    freePGM(&pgmTmp);
    return 0;
}
