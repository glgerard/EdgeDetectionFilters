/*! \file  imageBasicOps.c
 *  \brief Basic image processing functions (absolute, linear add, threshold and fapply).
 *  \author Eleonora Maria Aiello
 *  \author Gianluca Gerard
 *  \date 25/10/15
 *  \copyright Apache License Version 2.0, January 2004
 */

#include "imageBasicOps.h"

/*! \fn int absolutePGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Convert each pixel of the image \a pgmIn to its absolute value. The final result is stored in \a pgmOut.
 *
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int absolutePGM(Pgm* pgmIn, Pgm* pgmOut)
{
    int i;
    int pixel;
    int max_val = 0;
    
    if(!pgmIn)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    if(!pgmOut)
    {
        fprintf(stderr, "Error! No space to store the result. Please Check.\n");
        return -1;
    }
    
    // Iterate over all pixels
    for (i = 0; i < width*height; i++) {
        pixel = abs(pgmIn->pixels[i]);
        pgmOut->pixels[i] = pixel;
        if ( pixel > max_val)
            max_val = pixel;
    }
    
    pgmOut->max_val = max_val;
    
    return 0;
}

/*! \fn int thresholdPGM(Pgm* pgmIn, Pgm* pgmOut, int threshold)
 * \brief Binarize an image \a pgmIn based on the \a threshold value. The result is stored in \a pgmOut.
 *
 * Each pixel's value greater or equal than the \a threshold is converted to the value 255. All other pixels
 * are set to 0.
 * \param pgmIn Pointer to the input Pgm image structure.
 * \param pgmOut Pointer to the output Pgm image structure.
 * \param threshold The threshold value.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int thresholdPGM(Pgm* pgmIn, Pgm* pgmOut, int threshold)
{
    int i;

    if(!pgmIn)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    if(!pgmOut)
    {
        fprintf(stderr, "Error! No space to store the result. Please Check.\n");
        return -1;
    }
    
    // Limit the threshold value between 0 and 255
    if (threshold > 255) {
        threshold = 255;
    }
    
    if (threshold < 0) {
        threshold = 0;
    }

    // Iterate over all pixels
    for (i = 0; i < width*height; i++)
        // Set the output value to black or white if it is
        // below or above the threshold
        if (pgmIn->pixels[i]>=threshold)
            pgmOut->pixels[i] = 255;
        else
            pgmOut->pixels[i] =0;
    
    pgmOut->max_val = 255;

    return 0;
}

/*! \fn int linearAddPGM(Pgm* pgmOp1, Pgm* pgmOp2, double w1, double w2, Pgm* pgmOut)
 * \brief Linear weighted sum of two images.
 *
 * It adds linearly the two images arrays. Each array is first multiplied by a weight. The two images
 * must have the same dimensions.
 * \param pgmOp1 The pointer to the first image.
 * \param pgmOp2 The pointer to the second image.
 * \param w1 Weigth for the first image.
 * \param w2 Weight for the second image.
 * \param pgmOut The pointer to the image that stores the result.
 * \return 0 on success, -1 if either pgmOp1, pgmOp2 or pgmOut are NULL.
 */
int linearAddPGM(Pgm* pgmOp1, Pgm* pgmOp2, double w1, double w2, Pgm* pgmOut)
{
    int i;
    int pixel;
    int max_val = 0;

    if(!pgmOp1 | !pgmOp2)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmOp1->width;
    int height = pgmOp1->height;

    if(!pgmOut)
    {
        fprintf(stderr, "Error! No space to store the result. Please Check.\n");
        return -1;
    }
    
    // Iterate over all pixels
    for (i = 0; i < width*height; i++) {
        pixel = (int)(w1*pgmOp1->pixels[i] + w2*pgmOp2->pixels[i]);
        pgmOut->pixels[i] = pixel;
        if ( pixel > max_val)
            max_val = pixel;
    }
    
    pgmOut->max_val = max_val;
    
    return 0;
}

/*! \fn int comparePGM(Pgm* pgmOp1, Pgm* pgmOp2)
 * \brief Compare two images \a pgmOp1 and \a pgmOp2.
 * \param pgmOp1 Pointer to the first Pgm image structure.
 * \param pgmOp2 Pointer to the second Pgm image structure.
 * \return 0 if the images are equal, 1 otherwise.
 */
int comparePGM(Pgm* pgmOp1, Pgm* pgmOp2)
{
    int i;
    
    if(!pgmOp1 | !pgmOp2)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }

    int width = pgmOp1->width;
    int height = pgmOp1->height;
    
    // Iterate over all pixels
    for (i = 0; i < width*height; i++)
        if (pgmOp1->pixels[i] != pgmOp2->pixels[i])
            return 1;
    
    return 0;
}

/*! \fn int modulePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut)
 * \brief Store in \a pgmOut the "module" of the two images in \a pgmOpX and \a pgmOpY as if each image's pixel
 *  would be the x and y coordinate of a point in the cartesian plane.
 *
 * \param pgmOpX Pointer to the first Pgm image structure.
 * \param pgmOpY Pointer to the second Pgm image structure.
 * \param pgmOut Pointer to the output Pgm image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int modulePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut)
{
    int i;
    int pixel;
    int max_val = 0;

    if(!pgmOpX | !pgmOpY)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    int width = pgmOpX->width;
    int height = pgmOpX->height;
    
    if(!pgmOut)
    {
        fprintf(stderr, "Error! No space to store the result. Please Check.\n");
        return -1;
    }
    
    // Iterate over all pixels
    for (i = 0; i < width*height; i++) {
        pixel = (int)sqrt(pgmOpX->pixels[i]*pgmOpX->pixels[i] +
                             pgmOpY->pixels[i]*pgmOpY->pixels[i]);
        pgmOut->pixels[i] = pixel;
        if (pixel > max_val)
            max_val = pixel;
    }
    
    pgmOut->max_val = max_val;
    
    return 0;
}

/*! \fn int phasePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut)
 * \brief Store in \a pgmOut the "phase" of the two images in \a pgmOpX and \a pgmOpY as if each image's pixel
 *  would be the x and y coordinate of a point in polar coordinates.
 *
 * \param pgmOpX Pointer to the first Pgm image structure.
 * \param pgmOpY Pointer to the second Pgm image structure.
 * \param pgmOut Pointer to the output Pgm image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int phasePGM(Pgm* pgmOpX, Pgm* pgmOpY, Pgm* pgmOut)
{
    int i;
    int pixel;
    int max_val = 0;
    double phi;
    
    if(!pgmOpX | !pgmOpY)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }

    int width = pgmOpX->width;
    int height = pgmOpX->height;
    
    if(!pgmOut)
    {
        fprintf(stderr, "Error! No space to store the result. Please Check.\n");
        return -1;
    }

    // Iterate over all pixels
    for (i = 0; i < width*height; i++) {
        phi = atan2(pgmOpY->pixels[i], pgmOpX->pixels[i])*M_1_PI*127;
        pixel = (int)phi;
        pgmOut->pixels[i] = pixel;
        if (pixel > max_val)
            max_val = pixel;
    }
    
    pgmOut->max_val = max_val;

    return 0;
}

/*! \fn int int fapplyPGM(Pgm* pgmIn1, Pgm* pgmIn2, Pgm* pgmOut, Filter* filter, int dimX, int dimY,
 int (*func)(Pgm*, Pgm*, double*, int, int, int))
 * \brief SScan an image and apply a function \a func to each pixel.
 *
 * It scans the image \a pgmIn1 and to each pixel in the image applies the function \a func.
 * The function \a func receives 6 parameters:
 *  - \a pgmIn1
 *  - \a pgmIn2
 *  - \a filter->kernel (if != NULL)
 *  - dimX/2
 *  - dimY/2
 *  - a linear index in the \a pgmIn1->pixels array
 * Each pixel in \a pgmOut is replaced with the return value of \a func for the corresponding pixel in \a pgmIn1.
 * \param pgmIn1 Pointer to the first Pgm image structure.
 * \param pgmIn2 Pointer to a second Pgm image structure that can optionally be accessed by \a func.
 * \param pgmOut Pointer to the output Pgm image structure.
 * \param filter Pointer to an optional Filter structure.
 * \param dimX The dimension along X of the subarray of \a pgmIn1 checked by \a func.
 * \param dimY The dimension along Y of the subarray of \a pgmIn1 checked by \a func.
 * \param func The function used to compute the value of the output pixel.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int fapplyPGM(Pgm* pgmIn1, Pgm* pgmIn2, Pgm* pgmOut, Filter* filter, int dimX, int dimY,
              int (*func)(Pgm*, Pgm*, double*, int, int, int))
{
    int row, col;
    int pixel;
    int max_val = 0;
    double *kernel = NULL; // a local pointer to the filter matrix if defined
    int ic; // the index of the central pixel in the source image

    
    if(!pgmIn1)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    if(!pgmOut)
    {
        fprintf(stderr, "Error! No space to store the result. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn1->width;
    int height = pgmIn1->height;
    
    if (filter != NULL) {
        dimX = filter->width;
        dimY = filter->height;
        kernel = filter->kernel;
    }
    
    int spanX = dimX/2;
    int spanY = dimY/2;
    
    // Start Timestamp
    struct timeval tvStart;
    gettimeofday(&tvStart, NULL);

    // Move to the first useful interior pixel
    ic = spanY*width+spanX;
    D(fprintf(stderr,"w=%d,h=%d\n",width,height));
    D(fprintf(stderr,"bw=%d,bh=%d\n",spanX,spanY));
    
    // Loop over all internal source image pixels
    for (row = spanY; row < height-spanY; row++, ic += spanX*2) {
        D(fprintf(stderr,"start:row=%d,ic=%d\n",row,ic));
        for (col = spanX; col < width-spanX; col++, ic++) {
            D(fprintf(stderr,"(%d,%d),ic=%d\n", row, col, ic));
            
            // Apply the function to each pixel neighborhood
            pixel = func(pgmIn1, pgmIn2, kernel, spanX, spanY, ic);

            pgmOut->pixels[ic] = pixel;
            if (pixel > max_val)
                max_val = pixel;
        }
        D(fprintf(stderr,"end:row=%d,ic=%d\n",row,ic));
        // move the index of the central pixel to the next row
    }
    
    pgmOut->max_val = max_val;
    
    // Stop Timestamp
    struct timeval tvStop;
    gettimeofday(&tvStop, NULL);
    
    fprintf(stderr, "\nElapsed time (msec): %f\n", ((double)tvStop.tv_sec - (double)tvStart.tv_sec) * 1000 +
            ((double)tvStop.tv_usec - (double)tvStart.tv_usec) / 1000);
    
    return 0;
}

/*! \fn int convolution2DKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
 * \brief Convolve an image \a pgmIn with a matrix stored in \a kernel. The result is stored in \a pgmOut.
 *
 * It returns the value of the convolution of the matrix in \a kernel with the equally sized subregion of
 * the image \a pgmIn1 centered at the linarly indexed pixel \a ic.
 * \param pgmIn1 Pointer to the input Pgm image structure.
 * \param pgmIn2 Not used.
 * \param kernel Pointer to convolution matrix.
 * \param spanX The number of columns to the left and right of the central column of the image subarray.
 * \param spanY The number of rows to the top and to the bottom of the central row of the image subarray.
 * \param ic The linear index of the central subarray pixel in \a pgmIn1.
 * \return The result of the convolution.
 */
int convolution2DKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
{
    int k,l, il;
    double sum = 0;
    
    int width = pgmIn1->width;
    
    int ix = 0;
    // Iterate over all filter pixels
    for (k=-spanY, il = ic-width*spanY; k <= spanY; k++, il += width)
        for (l=-spanX; l <= spanX; l++)
            sum += pgmIn1->pixels[il+l]*kernel[ix++];
    
    return (int)floor(sum);
}

/*! \fn int convolution2DPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
 * \brief Apply to the image \a pgmIn a 2D convolution with the Filter \a filter.
 * Store in \a pgmOut the result
 * \param pgmIn Pointer to the input Pgm image structure.
 * \param pgmOut Pointer to the output Pgm image structure.
 * \param filter Pointer to the Filter structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int convolution2DPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, filter, 0, 0, convolution2DKernel);
}

/*! \fn int convolution1DXPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
 * \brief Apply to the image \a pgmIn  a one-dimensional convolution
 * with the Filter \a filter along the image columns.
 * \param pgmIn Pointer to the input Pgm image structure.
 * \param pgmOut Pointer to the output Pgm image structure.
 * \param filter Pointer to the Filter structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int convolution1DXPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
{
    int row, col, l;
    double sum;
    int pixelVal;
    int topVal = 0;
    
    int ix; // the index in the filter
    int ic; // the index of the central pixel in the source image
    int il; // the index of the pixel used in the integration
    
    if(!pgmIn)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    if(!pgmOut)
    {
        fprintf(stderr, "Error! No space to store the result. Please Check.\n");
        return -1;
    }
    
    if(!filter) {
        fprintf(stderr, "Error! No filter defined. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    int filterWidth = filter->width;
    int halfFilterWidth = filterWidth/2;
    
    // Move to the first useful interior pixel
    ic = halfFilterWidth;
    D(fprintf(stderr,"w=%d,h=%d\n",width,height));
    
    // Loop over all internal source image pixels
    for (row = 0; row < height; row++) {
        D(fprintf(stderr,"start:row=%d,ic=%d\n",row,ic));
        for (col = halfFilterWidth; col < width-halfFilterWidth; col++) {
            // compute the initial neighoboring pixel index to use in the convolution
            il = ic-halfFilterWidth;
            sum = 0;
            ix = 0;
            
            // Iterate over all filter pixels
            D(fprintf(stderr,"il=%d\n", il));
            for (l=0; l < filterWidth; l++)
                sum += pgmIn->pixels[il++]*filter->kernel[ix++];
            
            // output the value of the convolution in the destination image
            pixelVal = (int)floor(sum);
            pgmOut->pixels[ic++] = pixelVal;
            if (pixelVal > topVal)
                topVal = pixelVal;
        }
        D(fprintf(stderr,"end:row=%d,ic=%d\n",row,ic));
        // move the index of the central pixel to the next row
        ic += halfFilterWidth*2;
    }
    
    pgmOut->max_val = topVal;
    
    return 0;
}

/*! \fn int convolution1DYPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
 * \brief Apply to the image \a pgmIn  a one-dimensional convolution
 * with the Filter \a filter along the image rows.
 * \param pgmIn Pointer to the input Pgm image structure.
 * \param pgmOut Pointer to the output Pgm image structure.
 * \param filter Pointer to the Filter structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int convolution1DYPGM(Pgm* pgmIn, Pgm* pgmOut, Filter* filter)
{
    int row, col, k;
    double sum;
    int pixelVal;
    int topVal = 0;
    
    int ix;  // the index in the filter
    int ic; // the index of the central pixel in the source image
    int il; // the index of the pixel used in the integration
    
    if(!pgmIn)
    {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return -1;
    }
    
    if(!pgmOut)
    {
        fprintf(stderr, "Error! No space to store the result. Please Check.\n");
        return -1;
    }
    
    if(!filter) {
        fprintf(stderr, "Error! No filter defined. Please Check.\n");
        return -1;
    }
    
    int width = pgmIn->width;
    int height = pgmIn->height;
    
    int filterHeight = filter->height;
    
    int halfFilterHeight = filterHeight/2;
    int rowShift = halfFilterHeight*width;
    
    // Move to the first useful interior pixel
    ic = rowShift;
    D(fprintf(stderr,"w=%d,h=%d\n",width,height));
    
    // Loop over all internal source image pixels
    for (row = halfFilterHeight; row < height-halfFilterHeight; row++) {
        D(fprintf(stderr,"start:row=%d,ic=%d\n",row,ic));
        for (col = 0; col < width; col++) {
            // compute the initial neighoboring pixel index to use in the convolution
            il = ic-rowShift;
            sum = 0;
            ix = 0;
            // Iterate over all filter pixels
            for (k=0; k < filterHeight; k++) {
                D(fprintf(stderr,"k=%d,il=%d\n", k, il));
                sum += pgmIn->pixels[il]*filter->kernel[ix++];
                // move the index of the neighboring pixel to the next row
                il += width;
            }
            // output the value of the convolution in the destination image
            pixelVal = (int)floor(sum);
            pgmOut->pixels[ic++] = pixelVal;
            if (pixelVal > topVal)
                topVal = pixelVal;
        }
        D(fprintf(stderr,"end:row=%d,ic=%d\n",row,ic));
    }
    
    pgmOut->max_val = topVal;
    return 0;
}
