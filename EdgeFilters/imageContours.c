//
//  imageContours.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 13/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "imageContours.h"

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