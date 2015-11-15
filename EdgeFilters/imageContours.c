/*! \file imageContours.c
 *  \brief Contour detection functions.
 *  \author Eleonora Maria Aiello
 *  \author Gianluca Gerard
 *  \date 13/11/15
 *  \copyright Apache License Version 2.0, January 2004
 */

#include "imageContours.h"

/*! \fn int contourUniformKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
 * \brief Return the value of the central pixel if it is not at the center of an 8-neighborhood uniform
 *        region. Otherwise returns 0.
 *
 * Computes the sum of the absolute values of the pixels in a 8-neighborhood of the central pixel.
 * If the sum is equal to the area of the neighborhood multipled with the value of the central
 * pixel we are in a uniform region, therefore the pixel cannot be a contour.
 * \param pgmIn1 Pointer to the input PGM image structure.
 * \param pgmIn2 Not used.
 * \param kernel Not used.
 * \param spanX Not used.
 * \param spanY Not used.
 * \param ic Linear index in pgmIn1->pixels of the central pixel.
 * \return The value of the central pixel if it is a contour pixel, otherwise 0.
 */
int contourUniformKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
{
    // This function computes the integral of the absolute values of the neighborhood.
    // If the integral is 9 * value of the central pixel, the central pixel is not
    // on the contour. Otherwise it is a contour pixel.
    
    spanX = 1;
    spanY = 1;
    
    int exp_int = pgmIn1->pixels[ic]*9;
    
    int width = pgmIn1->width;
    
    int sum = 0;

    // Iterate over all filter pixels
    for (int k=-spanY, il = ic-width*spanY; k <= spanY; k++, il += width)
        for (int l=-spanX; l <= spanX; l++)
            // Compute the integral of the neighborhood
            sum += abs(pgmIn1->pixels[il+l]);
    
    if (sum == exp_int)
        // The pixel is not a contour pixel and the output value is 0
        return 0;
    
    return pgmIn1->pixels[ic];
}

/*! \fn int contourUniformPGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Return in \a pgmOut the contours of image pointerd by \a pgmIn by looking for uniform regions.
 *
 * The image in \a pgmIn is filtered such that every pixel at the center of an 8-neighborhood uniform region
 * is set to 0.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int contourUniformPGM(Pgm* pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 0, 0, contourUniformKernel);
}

/*! \fn int contourN8IntKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
 * \brief Check for internal contour pixels with N8 distance. It uses inverted images (background white,
 *        foreground black).
 *
 * Find the subset of the foreground having unitary N8 distance from the background.
 * \param pgmIn1 Pointer to the input PGM image structure.
 * \param pgmIn2 Not used.
 * \param kernel Not used.
 * \param spanX Not used.
 * \param spanY Not used.
 * \param ic Linear index in pgmIn1->pixels of the central pixel.
 * \return The value of the central pixel if it is a contour pixel, otherwise 0.
 */
int contourN8IntKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
{
    spanX = 1;
    spanY = 1;
    
    int bck = pgmIn1->max_val;
    
    // skip background pixels
    if  (pgmIn1->pixels[ic] == bck )
        return 255;
    
    int width = pgmIn1->width;
    
    // Iterate over all filter pixels
    for (int k=-spanY, il = ic-width*spanY; k <= spanY; k++, il += width)
        for (int l=-spanX; l <= spanX; l++)
            if (pgmIn1->pixels[il+l] == bck)
                // There is at least one background pixel
                return 0;
    
    return 255;
}

/*! \fn int contourN8IntPGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Return in \a pgmOut the contours of image pointerd by \a pgmIn by looking for pixels in the
 *        foreground having N8 unitary distance from the background.
 *
 * Find the subset of the foreground of the image \a pgmIn having unitary N8 distance from the background.
 * This subset is stored in \a pgmOut. Background is a value of 0 of the image, every other value is foreground.
 * In the output image the contour is set to 0 and the background to 255, therefore the image is reverted.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int contourN8IntPGM(Pgm* pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 1, 1, contourN8IntKernel);
}

/*! \fn void connectivityKernel(Pgm *pgmNH, Pgm* pgmNL, Pgm* pgmOut, int ic)
 * \brief It returns in \a pgmOut the pixels of \a pgmNL 8-connected to
 *        the central pixel of \a pgmNH.
 *
 * Find and store in \a pgmOut the pixels of \a pgmNL that are 8-connected to a non background pixel in \a pgmNH.
 * It only searches for pixels in an 8-neighboorhood centered on the pixel in \a pgmNH linearly indexed by \a ic.
 * \param pgmNH Pointer to the input Pgm image structure.
 * \param pgmNL Pointer to the input Pgm image structure.
 * \param pgmOut Pointer to the Pgm image structure that stores the result.
 * \param ic Linear index in pgmNH->pixels of the central pixel.
 */
void connectivityKernel(Pgm *pgmNH, Pgm* pgmNL, Pgm* pgmOut, int ic)
{
    if ((pgmOut->pixels[ic] = pgmNH->pixels[ic])==0)
        return;

    int spanX = 1;
    int spanY = 1;
    
    int width = pgmNL->width;
    
    // Iterate over all N8 set in NL pixels
    for (int k=-spanY, il = ic-width*spanY; k <= spanY; k++, il += width)
        for (int l=-spanX; l <= spanX; l++)
            // If the pixel is different from 0 it is connected
            if (pgmNL->pixels[il+l] != 0)
                pgmOut->pixels[il+l] = 255;
}

/*! \fn int connectivityPGM(Pgm *pgmNH, Pgm *pgmNL, Pgm *pgmOut)
 * \brief It returns in \a pgmOut the pixels of \a pgmNL 8-connected to
 *        all the non-background pixels of \a pgmNH.
 *
 * Find and store in \a pgmOut the pixels of \a pgmNL that are 8-connected to all non background pixels in \a pgmNH.
 * It only searches for pixels in an 8-neighboorhood centered on each pixel of \a pgmNH. To identify all 8-connected
 * pixels the search must be repeated until no further pixels are added to the result in \a pgmOut.
 * \param pgmNH Pointer to the input Pgm image structure.
 * \param pgmNL Pointer to the input Pgm image structure.
 * \param pgmOut Pointer to the Pgm image structure that stores the result.
 * \return 0 on success, -1 if either pgmNH, pgmNL or pgmOut are NULL.
 */
int connectivityPGM(Pgm *pgmNH, Pgm *pgmNL, Pgm *pgmOut)
{
    int ic; // the index of the central pixel in the source image
    
    if(!pgmNH || !pgmNL)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    if(!pgmOut)
    {
        fprintf(stderr, "Error! No output data. Please Check.\n");
        return -1;
    }
    
    int width = pgmNH->width;
    int height = pgmNH->height;
    
    int borderX = 1;
    int borderY = 1;
    
    // Timestamp
    struct timeval tvStart;
    gettimeofday(&tvStart, NULL);
    
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
            connectivityKernel(pgmNH, pgmNL, pgmOut, ic);
        }
        D(fprintf(stderr,"end:row=%d,ic=%d\n",row,ic));
        // move the index of the central pixel to the next row
    }
    
    pgmOut->max_val = pgmNH->max_val;
    
    // Timestamp
    struct timeval tvStop;
    gettimeofday(&tvStop, NULL);
    
    fprintf(stderr, "\nElapsed time (msec): %f\n", ((double)tvStop.tv_sec - (double)tvStart.tv_sec) * 1000 +
                                                    ((double)tvStop.tv_usec - (double)tvStart.tv_usec) / 1000);
    
    return 0;
}