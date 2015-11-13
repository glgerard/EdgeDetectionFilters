//
//  wrappers.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 07/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "imageFilterOps.h"
#include "imageFilters.h"
#include "imageBasicOps.h"
#include "helperFunctions.h"


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
//--------------      Apply a 3/9 filter     -------------//
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
    0, 8,
    3, 5,
    2, 6
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

/** \fn int sharpeningPGM(Pgm* imgIn, Pgm* imgOut)
        \brief Sharpen an image.
               Apply as a filter the difference between an identity and a box filter.
        \param imgIn Pointer to the input PGM image structure.
        \param imgOut Pointer to the output PGM image structure.
 */
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

int applyCED(Pgm* imgIn, Pgm* imgOut, double sigma, int dim, int threshold_low, int threshold_high)
{    
    Pgm* imgOutX = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOutY = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOutMod = newPGM(imgIn->width, imgIn->height, imgIn->max_val);
    Pgm* imgOutPhi = newPGM(imgIn->width, imgIn->height, imgIn->max_val);

    applyGauss(imgIn, imgOut, sigma, dim);
    
    Filter* gx = sobelXFilter();
    Filter* gy = sobelYFilter();
    
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
                farg = sqrt(2.0);
                iarg = 0;
            } else
                sscanf(ch, "%f", &farg);
            ch = strtok(NULL, " ");
            if ( ch == NULL) {
                iarg = 25;
            }
            else
                iarg = atoi(ch);
            applyCED(pgmTmp, pgmOut, farg, 0, iarg, iarg*3);
        }
        
    }
    
    freePGM(&pgmTmp);
    return 0;
}