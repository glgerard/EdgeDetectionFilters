//
//  imageOperations.c
//  EdgeFilters
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
        if (pixelVal<threshold) {
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
    int topVal = 0;
    
    int width = pgmOpX->width;
    int height = pgmOpX->height;
    
    for (int i = 0; i < width*height; i++) {
        pixelVal = (int)sqrt(pgmOpX->pixels[i]*pgmOpX->pixels[i] +
                             pgmOpY->pixels[i]*pgmOpY->pixels[i]);
        pgmOut->pixels[i] = pixelVal;
        
        if (pixelVal > topVal)
            topVal = pixelVal;
    }
    
    pgmOut->max_val = topVal;
    
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
    int topVal = 0;
    
    int width = pgmOpX->width;
    int height = pgmOpX->height;
    
    for (int i = 0; i < width*height; i++) {
        phi = fabs(atan2(pgmOpY->pixels[i], pgmOpX->pixels[i]))*M_1_PI*255;
        pixelVal = (int)phi;
        pgmOut->pixels[i] = pixelVal;
        
        if (pixelVal > topVal)
            topVal = pixelVal;
    }
    
    pgmOut->max_val = topVal;

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
    
    if (range < 0) {
        range = -range;
    }
    
    if (range > 127) {
        range = 127;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
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
    
    pgmOut->max_val = 255;
    
    return 0;
}

//--------------------------------------------------------//
//--- Scan an image and apply a function to each pixel ---//
//--------------------------------------------------------//
int fapplyPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter, int borderX, int borderY,
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
    for (int row = borderY; row < height-borderY; row++, ic += borderX*2) {
        D(fprintf(stderr,"start:row=%d,ic=%d\n",row,ic));
        for (int col = borderX; col < width-borderX; col++, ic++) {
            D(fprintf(stderr,"(%d,%d),ic=%d\n", row, col, ic));
            
            pixelVal = func(pgmIn, kernel, borderX, borderY, ic);

            pgmOut->pixels[ic] = pixelVal;
            if (pixelVal > topVal)
                topVal = pixelVal;
        }
        D(fprintf(stderr,"end:row=%d,ic=%d\n",row,ic));
        // move the index of the central pixel to the next row
    }
    
    pgmOut->max_val = topVal;
    
    return 0;
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

    if (sum > 0 && sum < 255*(2*borderX+1)*(2*borderY+1)) {
        pixelVal = 0;
    }
    return pixelVal;
}

//--------------------------------------------------------//
//--------------- A simple contour check -----------------//
//--------------------------------------------------------//
int contour2DPGM(Pgm* pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, pgmOut, NULL, 1, 1, contour2DKernel);
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
    return fapplyPGM(pgmIn, pgmOut, NULL, 1, 1, contourN8IntKernel);
}

//--------------------------------------------------------//
//---------------------- Bubble Sort ---------------------//
//--------------------------------------------------------//
int* sort(int* array, int len)
{
    int temp;
    int swap = TRUE;
    
    while (swap) {
        swap = FALSE;
        for (int i=0; i<len; i++) {
            if (array[i] > array[i+1]) {
                temp = array[i];
                array[i] = array[i+1];
                array[i+1] = temp;
                swap = TRUE;
            }
        }
    }
    return array;
}

//--------------------------------------------------------//
//---------- Compute an image submatrix median -----------//
//--------------------------------------------------------//
int medianKernel(Pgm* pgmIn, double* kernel, int borderX, int borderY, int ic)
{
    int* pixelVals;
    int ix = 0;
    
    pixelVals = calloc((2*borderX+1)*(2*borderY+1), sizeof(int));
    
    int width = pgmIn->width;
    
    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++)
            pixelVals[ix++] = pgmIn->pixels[il+l];
    
    int nPixels = ix;
    
    pixelVals = sort(pixelVals, nPixels);
    
    return pixelVals[nPixels/2];
}

//--------------------------------------------------------//
//--------------    Apply a median filter    -------------//
//--------------------------------------------------------//
int medianPGM(Pgm *pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, pgmOut, NULL, 1, 1, medianKernel);
}

int mod (int a, int b)
{
    int ret = a % b;
    if(ret < 0)
        ret+=b;
    return ret;
}

//--------------------------------------------------------//
//-------------    Compute the 3/9 operator   ------------//
//--------------------------------------------------------//
int op39Kernel(Pgm* pgmIn, double* kernel, int borderX, int borderY, int ic)
{
    int pixelVals[9];
    int I[9];
    int sum = 0;

    borderX = 1;
    borderY = 1;
    
    int width = pgmIn->width;
    
    int total = 0;
    int ix = 0;

    // Iterate over all filter pixels
    for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
        for (int l=-borderX; l <= borderX; l++) {
            pixelVals[ix] = pgmIn->pixels[il+l];
            total += pixelVals[ix++];
        }
    
    if (total == 0)
        return 0;
    
    // remapping
    
    I[0] = pixelVals[5];
    I[1] = pixelVals[2];
    I[2] = pixelVals[1];
    I[3] = pixelVals[0];
    I[4] = pixelVals[3];
    I[5] = pixelVals[6];
    I[6] = pixelVals[7];
    I[7] = pixelVals[8];
    I[8] = pixelVals[4];
    
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
    return fapplyPGM(pgmIn, pgmOut, NULL, 1, 1, op39Kernel);
}

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

double mean(int *array, int len)
{
    int sum = 0;
    
    for (int i=0; i<len; i++)
        sum += array[i];

    return (double)sum/len;
}

double var(double m, int* array, int len)
{
    double sum = 0;
    
    for (int i=0; i<len; i++)
        sum += (array[i]-m)*(array[i]-m);
    
    return sum/len;
}

//--------------------------------------------------------//
//---------- Compute the Nagao-Matsuyama filter ----------//
//--------------------------------------------------------//
int nagaoKernel(Pgm* pgmIn, double* kernel, int borderX, int borderY, int ic)
{
    const int *np = NULL;
    int pixelVals[9];
    int ix, in;
    double m, v;
    double min_var = 585225;
    double sel_mean = 0;
    
    borderX = 2;
    borderY = 2;
    int width = pgmIn->width;

    for (int n=0; n<9; n++) {
        np = nagao[n];
        ix = 0;
        in = 0;
        // Iterate over all filter pixels
        for (int k=-borderY, il = ic-width*borderY; k <= borderY; k++, il += width)
            for (int l=-borderX; l <= borderX; l++, ix++) {
                if (np[ix] == 1) {
                    pixelVals[in] = pgmIn->pixels[il+l]*np[ix];
                    in++;
                }
            }
        
        m = mean(pixelVals,nagaoSize[n]);
        v = var(m,pixelVals,nagaoSize[n]);
        
        if (v < min_var) {
            min_var = v;
            sel_mean = m;
        }
    }

    return (int)floor(sel_mean);
}

//--------------------------------------------------------//
//------------- Apply a Nagao-Matsuyama fiter ------------//
//--------------------------------------------------------//
int nagaoPGM(Pgm *pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, pgmOut, NULL, 2, 2, nagaoKernel);
}
