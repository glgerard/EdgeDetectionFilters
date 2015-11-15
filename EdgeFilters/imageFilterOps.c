/*! \file imageFilterOps.c
 *  \brief All the filter operations (blurs, noises, sharpening, edge detection) are implemented in this file.
 *  \author Eleonora Maria Aiello
 *  \author Gianluca Gerard
 *  \date 07/11/15
 *  \copyright Apache License Version 2.0, January 2004
 */

#include "imageFilterOps.h"

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

// Number of 1's in each Nagao matrix
const static int nagaoSize[9] = {9, 7, 7, 7, 7, 7, 7, 7, 7};

/*! \fn int addUniformNoisePGM(Pgm* pgmIn, Pgm* pgmOut, int range)
 * \brief Add uniform noise to the image \a pgmIn. The final result is stored in \a pgmOut.
 *
 * Add to each pixel of the image a value in the interval [-range, +range] with uniform density.
 * \param imgIn Pointer to the input PGM image structure.
 * \param imgOut Pointer to the output PGM image structure.
 * \param range The absolute max value of the range.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int addUniformNoisePGM(Pgm* pgmIn, Pgm* pgmOut, int range)
{
    int randVal;
    
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

/*! \fn int addSaltPepperNoisePGM(Pgm* pgmIn, Pgm* pgmOut, double density)
 * \brief Add Salt & Pepper noise to the image \a pgmIn. The final result is stored in \a pgmOut.
 *
 * Superimpose a percentage 'density' of white or black pixels onto the image.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \param density The percentage of pixels that will become black or white with equal probability.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int addSaltPepperNoisePGM(Pgm* pgmIn, Pgm* pgmOut, double density)
{
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

/*! \fn medianKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
 * \brief Return the median pixel of an image subarray of dimensions (dimX, dimY).
 *
 * \param pgmIn1 Pointer to the Pgm structure of the input image.
 * \param pgmIn2 Not used.
 * \param kernel Not used.
 * \param dimX The number of columns to the left and right of the central column of the image subarray.
 * \param dimY The number of rows to the top and to the bottom of the central row of the image subarray.
 * \param ic The linear index of the central subarray pixel in \a pgmIn1.
 * \return The value of the median pixel.
 */
int medianKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
{
    int* pixels;
    int pixel;
    int ix = 0;
    int width = pgmIn1->width;
    
    pixels = calloc((2*spanX+1)*(2*spanY+1), sizeof(int));
    
    // Iterate over all filter pixels
    for (int k=-spanY, il = ic-width*spanY; k <= spanY; k++, il += width)
        for (int l=-spanX; l <= spanX; l++)
            pixels[ix++] = pgmIn1->pixels[il+l];
    
    int nPixels = ix;
    
    // Sort the pixels
    pixels = sort(pixels, nPixels);
    
    // Store the pixel in the middle of the sorted list
    pixel = pixels[nPixels/2];
    
    free(pixels);
    
    return pixel;
}

/*! \fn int medianPGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Apply a median filter to the image \a pgmIn. The final result is stored in \a pgmOut.
 *
 * For each pixel of \a pgmIn compute the median of a 3x3 subarray centered at the pixel.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int medianPGM(Pgm *pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 3, 3, medianKernel);
}


/*! \fn int averagePGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Apply a box filter to the image \a pgmIn. The final result is stored in \a pgmOut.
 *
 * For each pixel of \a pgmIn compute the average of a 3x3 subarray centered at the pixel.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int averagePGM(Pgm *pgmIn, Pgm* pgmOut)
{
    // apply a box filter
    Filter *bxFilter = boxFilter(3,3);
    
    int ret = convolution2DPGM(pgmIn, pgmOut, bxFilter);
    
    freeFilter(&bxFilter);
    
    return ret;
}

/*! \fn int sharpeningPGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Sharpen an image \a pgmIn. The final result is stored in \a pgmOut.
 *
 * Apply a filter (the difference between an identity and a box filter) to the image stored in \a pgmIn.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int sharpeningPGM(Pgm* pgmIn, Pgm* pgmOut)
{
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
    
    // create a identity filter
    Filter *idFilter = identityFilter(3,3);
    
    // create a box filter
    Filter *bxFilter = boxFilter(3,3);
    
    // sharpening
    Filter* filter = linearAddFilter(idFilter, bxFilter, 2.0, -1.0);
    freeFilter(&idFilter);
    freeFilter(&bxFilter);
    
    resetPGM(pgmOut);
    convolution2DPGM(pgmIn, pgmOut, filter);
    
    freeFilter(&filter);
    
    return 0;
}

/*! \fn int gaussPGM(Pgm* pgmIn, Pgm* pgmOut, double sigma, int dim)
 * \brief Gaussian blur of an image \a pgmIn.
 *
 * Apply as a filter two 1D Gaussian filters with the same \a sigma and matrix linear dimension \a dim.
 * \param pgmIn Pointer to the input Pgm image structure.
 * \param pgmOut Pointer to the output Pgm image structure.
 * \param sigma The sigma of the gaussians.
 * \param dim The dimension of the gaussian filter. If 0 it will default to the nearest odd value close to 6 sigma.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int gaussPGM(Pgm* pgmIn, Pgm* pgmOut, double sigma, int dim)
{
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
    
    Filter* filter;
    Pgm* imgOut1 = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    
    // linear X guassian filter
    filter = gauss1DXFilter(sigma, dim);
    
    convolution1DXPGM(pgmIn, imgOut1, filter);
    
    freeFilter(&filter);
    
    // linear Y guassian filter
    filter = gauss1DYFilter(sigma, dim);
    
    convolution1DYPGM(imgOut1, pgmOut, filter);
    
    freeFilter(&filter);
    freePGM(&imgOut1);
    
    return 0;
}

/*! \fn op39Kernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
 * \brief Return the result of applying the 3/9 operator to a pixel of \a pgmIn1.
 *
 * \param pgmIn1 Pointer to the Pgm structure of the input image.
 * \param pgmIn2 Not used.
 * \param kernel Not used.
 * \param spanX Not used.
 * \param spanY Not used.
 * \param ic The linear index of the central subarray pixel in \a pgmIn1.
 * \return The result of the 3/9 operator or 0 if the integral of the subarray is 0.
 */
int op39Kernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int dimX, int dimY, int ic)
{
    int pixels[9];
    int I[9];
    const double K = 255*1.5;
    const double P0 = 1.0/3;
    
    // Define how far to move to the left, right, top and bottom
    // in to match the 3x3 matrixes of the 3/9 operator
    int spanX = 1;
    int spanY = 1;
    
    int width = pgmIn1->width;
    
    int Pi = 0;  // The integral of the image subarray
    int ix = 0;
    
    // Iterate over all filter pixels
    for (int k=-spanY, il = ic-width*spanY; k <= spanY; k++, il += width)
        for (int l=-spanX; l <= spanX; l++) {
            pixels[ix] = pgmIn1->pixels[il+l];
            Pi += pixels[ix++];
        }
    
    if (Pi == 0)
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
    
    int Pik = 0;
    int Pij = 0;
    for (int j=0; j<9; j++) {
        Pij = I[mod((j-1),9)] + I[j] + I[mod((j+1),9)];
        if (Pij > Pik)
            Pik = Pij;
    }
    
    // Return the normalized maxima among the 8 parameters Pjj
    return (int)floor(K*((double)Pik/Pi-P0));
}

/*! \fn int op39PGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Filter an image \a pgmIn with the 3/9 operator. The final result is stored in \a pgmOut.
 *
 * Apply a 3/9 operator filter to the image stored in \a pgmIn.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int op39PGM(Pgm *pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 0, 0, op39Kernel);
}

/*! \fn nagaoKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
 * \brief Return the result of applying the Nagao-Matsuyama filter to a pixel of \a pgmIn1.
 *
 * \param pgmIn1 Pointer to the Pgm structure of the input image.
 * \param pgmIn2 Not used.
 * \param kernel Not used.
 * \param spanX Not used.
 * \param spanY Not used.
 * \param ic The linear index of the central subarray pixel in \a pgmIn1.
 * \return The result of the Nagao-Matsuyama filter.
 */
int nagaoKernel(Pgm* pgmIn1, Pgm* pgmIn2, double* kernel, int spanX, int spanY, int ic)
{
    const int *np = NULL;
    int pixelVals[9];
    int ix, in;
    double v;
    double min_var = 585225;
    double sel_mean = 0;
    
    // Define how far to move to the left, right, top and bottom
    // in to match the 5x5 Nagao matrixes
    spanX = 2;
    spanY = 2;
    int width = pgmIn1->width;
    
    // Iterates over all Nagao matrixes
    for (int n=0; n<9; n++) {
        np = nagao[n];
        ix = 0;
        in = 0;
        // Iterate over all filter pixels
        for (int k=-spanY, il = ic-width*spanY; k <= spanY; k++, il += width)
            for (int l=-spanX; l <= spanX; l++, ix++) {
                if (np[ix] == 1) {
                    pixelVals[in] = pgmIn1->pixels[il+l]*np[ix];
                    in++;
                }
            }
        
        // Compute the variance
        v = var(pixelVals,nagaoSize[n]);
        
        // Select the mean of the array with the minimum variance
        if (v < min_var) {
            min_var = v;
            sel_mean = mean(pixelVals,nagaoSize[n]);
        }
    }
    
    return (int)floor(sel_mean);
}

/*! \fn int nagaoPGM(Pgm* pgmIn, Pgm* pgmOut)
 * \brief Filter an image \a pgmIn with the Nagao-Matsuyama filter. The final result is stored in \a pgmOut.
 *
 * Apply a Nagao-Matsuyama filter to the image stored in \a pgmIn.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int nagaoPGM(Pgm *pgmIn, Pgm* pgmOut)
{
    return fapplyPGM(pgmIn, NULL, pgmOut, NULL, 0, 0, nagaoKernel);
}

// These are the pairs of neighbors in a 3x3 array to confront with
// the central pixel to find the maximum in each quadrant.
const static int neighbors[] = {
    1, 7,
    0, 8,
    3, 5,
    2, 6
};

/*! \fn suppressionKernel(Pgm *pgmMod, Pgm* pgmPhi, double* kernel, int spanX, int spanY, int ic)
 * \brief Return the result of applying non-maximum suppression to a pixel of \a pgmIn1.
 *
 * \param pgmMod Pointer to the Pgm structure containing the modulus component.
 * \param pgmPhi Pointer to the Pgm structure containing the phase component.
 * \param kernel Not used.
 * \param spanX Not used.
 * \param spanY Not used.
 * \param ic The linear index of the central subarray pixel in \a pgmIn1.
 * \return The result the result of non-maximum suppression.
 */
int suppressionKernel(Pgm *pgmMod, Pgm* pgmPhi, double* kernel, int spanX, int spanY, int ic)
{
    int pixels[9];
    int ix = 0;
    
    int width = pgmMod->width;
    
    // Define the left, right, top and bottom span from the
    // center to match the 3x3 pixels matrix
    spanX = 1;
    spanY = 1;
    
    // Copy the image pixels in a local pixels array
    for (int k=-spanY, il = ic-width*spanY; k <= spanY; k++, il += width)
        for (int l=-spanX; l <= spanX; l++) {
            pixels[ix++] = pgmMod->pixels[il+l];
        }
    
    // Compute the quadrant of the phase of the central image pixel
    int q = quadrant((int)(180.0*pgmPhi->pixels[ic]/127));
    
    // Check if the central pixel is the maximum along the direction
    // perpendicular to the orientation of the gradient
    if((pixels[4] >= pixels[neighbors[2*q]]) && (pixels[4] >= pixels[neighbors[2*q+1]]))
        return pixels[4];
    
    return 0;
}

/*! \fn int suppressionPGM(Pgm *pgmMod, Pgm *pgmPhi, Pgm *pgmOut)
 * \brief Filter the image \a pgmMod with non-maximum suppression. The final result is stored in \a pgmOut.
 *
 * Apply a non-maximum suppression filter to the image stored in \a pgmMod. This array typically contains the magnitude
 * of the gradient of an image. The orientation of the gradient is stored instead in \a pgmPhi.
 * \param pgmMod Pointer to the PGM image structure with the magnitude component.
 * \param pgmPhi Pointer to the PGM image structure with the phase component.
 * \param pgmOut Pointer to the output PGM image structure.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int suppressionPGM(Pgm *pgmMod, Pgm *pgmPhi, Pgm *pgmOut)
{
    return fapplyPGM(pgmMod, pgmPhi, pgmOut, NULL, 0, 0, suppressionKernel);
}

/*! \fn int sobelPGM(Pgm* pgmIn, Pgm* pgmOut, uint8_t phase)
 * \brief Filter the image \a pgmIn with two Sobel filters alogn the vertical and horizontal direction.
 *        Returns either the magnitute or the phase based on \a phase.
 *
 * Apply the vertical and horizontal Sobel filters and returns the magnitute if \a phase is 0
 * or the phase if \a phase is 1.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \param phase Returns the phase if set to 1. Otherwise it returns the magnitude.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int sobelPGM(Pgm* pgmIn, Pgm* pgmOut, uint8_t phase)
{
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
    
    Filter* filter;
    
    Pgm* gx = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    // apply a sobel horizontal filter
    filter = sobelXFilter();
    convolution2DPGM(pgmIn, gx, filter);
    freeFilter(&filter);
    
    Pgm* gy = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    // apply a sobel vertical filter
    filter = sobelYFilter();
    convolution2DPGM(pgmIn, gy, filter);
    freeFilter(&filter);
    
    // compute the module of the two images with the
    // applied sobel filters
    resetPGM(pgmOut);
    
    if (phase == 1) {
        phasePGM(gx, gy, pgmOut);
    } else
        modulePGM(gx, gy, pgmOut);
    
    freePGM(&gx);
    freePGM(&gy);
    
    return 0;
}

/*! \fn int prewittPGM(Pgm* pgmIn, Pgm* pgmOut, uint8_t phase)
 * \brief Filter the image \a pgmIn with two Prewitt filters alogn the vertical and horizontal direction.
 *        Returns either the magnitute or the phase based on \a phase.
 *
 * Apply the vertical and horizontal Prewitt filters and returns the magnitute if \a phase is 0
 * or the phase if \a phase is 1.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \param phase Returns the phase if set to 1. Otherwise it returns the magnitude.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int prewittPGM(Pgm* pgmIn, Pgm* pgmOut, uint8_t phase)
{
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
    
    Filter* filter;
    
    Pgm* gx = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    // apply a sobel horizontal filter
    filter = prewittXFilter();
    convolution2DPGM(pgmIn, gx, filter);
    freeFilter(&filter);
    
    Pgm* gy = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    // apply a sobel vertical filter
    filter = prewittYFilter();
    convolution2DPGM(pgmIn, gy, filter);
    freeFilter(&filter);
    
    // compute the module of the two images with the
    // applied sobel filters
    resetPGM(pgmOut);
    
    if (phase == 0) {
        modulePGM(gx, gy, pgmOut);
    } else
        phasePGM(gx, gy, pgmOut);
    
    freePGM(&gx);
    freePGM(&gy);
    
    return 0;
}

/*! \fn dogPGM(Pgm* pgmIn, Pgm* pgmOut, double sigma, int dim)
 * \brief Filter the image \a pgmIn with two Prewitt filters alogn the vertical and horizontal direction.
 *        Returns either the magnitute or the phase based on \a phase.
 *
 * Apply the vertical and horizontal Prewitt filters and returns the magnitute if \a phase is 0
 * or the phase if \a phase is 1.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \param sigma The external sigma of the DoG filter. The internal sigma is set to \a sigma / 1.66 .
 * \param dim The rows and columns of the DoG filter. If set to 0 then it will be the smallest odd next to 6 \a sigma.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int dogPGM(Pgm* pgmIn, Pgm* pgmOut, double sigma, int dim)
{
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
    
    Filter* filter;
    
    fprintf(stderr, "\nDoG filtering (sigma = %f)\n",sigma);
    filter = DoGFilter(sigma, dim);
    
    resetPGM(pgmOut);
    convolution2DPGM(pgmIn, pgmOut, filter);
    
    freeFilter(&filter);
    
    return 0;
}

/*! \fn cedPGM(Pgm* imgIn, Pgm* imgOut, double sigma, int dim, int threshold_low, int threshold_high)
 * \brief Filter the image \a pgmIn with two Prewitt filters alogn the vertical and horizontal direction.
 *        Returns either the magnitute or the phase based on \a phase.
 *
 * Apply the vertical and horizontal Prewitt filters and returns the magnitute if \a phase is 0
 * or the phase if \a phase is 1.
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \param sigma The external sigma of the DoG filter. The internal sigma is set to \a sigma / 1.66 .
 * \param dim The rows and columns of the DoG filter. If set to 0 then it will be the smallest odd next to 6 \a sigma.
 * \return 0 on success, -1 if either pgmIn or pgmOut are NULL.
 */
int cedPGM(Pgm* pgmIn, Pgm* pgmOut, double sigma, int dim, int threshold_low, int threshold_high)
{    
    Pgm* imgOutX = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    Pgm* imgOutY = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    Pgm* imgOutMod = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    Pgm* imgOutPhi = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);

    gaussPGM(pgmIn, pgmOut, sigma, dim);
    
    Filter* gx = sobelXFilter();
    Filter* gy = sobelYFilter();
    
    convolution2DPGM(pgmOut, imgOutX, gx);
    convolution2DPGM(pgmOut, imgOutY, gy);
    
    modulePGM(imgOutX, imgOutY, imgOutMod);
    phasePGM(imgOutX, imgOutY, imgOutPhi);
    
    resetPGM(pgmOut);
   
    suppressionPGM(imgOutMod, imgOutPhi, pgmOut);

    freePGM(&imgOutX);
    freePGM(&imgOutY);
    freePGM(&imgOutMod);
    freePGM(&imgOutPhi);

    // Find strong and weak edges with thresholding
    Pgm *imgNH = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    thresholdPGM(pgmOut, imgNH, threshold_high);
    
    Pgm *imgNLshadow = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    thresholdPGM(pgmOut, imgNLshadow, threshold_low);

    Pgm *imgNL = newPGM(pgmIn->width, pgmIn->height, pgmIn->max_val);
    linearAddPGM(imgNLshadow, imgNH, 1.0, -1.0, imgNL);
    
    int change = 1;
    
    // Find 8-connected pixels in imgNL by a repeated search

    for (int i=0; (i<50) && change; i++) {
        connectivityPGM(imgNH, imgNL, pgmOut);
        
        change = comparePGM(imgNH, pgmOut);
        
        copyPGM(pgmOut, imgNH);
    }
    
    copyPGM(imgNH, pgmOut);
    
    freePGM(&imgNLshadow);
    freePGM(&imgNH);
    freePGM(&imgNL);
    
    return 0;
}

/*! \fn execImageOps(Pgm *pgmIn, Pgm* pgmOut, FILE *fp)
 * \brief Filter the image \a pgmIn with the filters listed in file \a fp.
 *
 * The file \a fp contains a list of filters, one per line, that will be applied in sequence to
 * the image pointed by \a pgmIn. The output will be stored in the Pgm structure pointed by
 * \a pgmOut. Certain filters have optional parameters.
 *
 * \param pgmIn Pointer to the input PGM image structure.
 * \param pgmOut Pointer to the output PGM image structure.
 * \param fp Pointer to a file with the list of filters.
 *
 * \par List of implemented filters
 *
 *   - threshold [threshold_value (default 0)]
 *   - uniform_noise [range_value (default 32)]
 *   - salt_n_pepper [density (default 0.05)]
 *   - normalize
 *   - equalize
 *   - median
 *   - average
 *   - internal_contour
 *   - operator_39
 *   - nagao
 *   - sharpening
 *   - prewitt [mod|phase (default mod)]
 *   - sobel [mod|phase (default mod)]
 *   - gauss [sigma (default 1)] [dim (default 0)]
 *   - dog [sigma (default 1)] [dim (default 0)]
 *   - ced [sigma (default sqrt(2))] [threshold (default 25)]
 */
void execImageOps(Pgm *pgmIn, Pgm* pgmOut, FILE *fp)
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
            sharpeningPGM(pgmTmp, pgmOut);
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
            prewittPGM(pgmTmp, pgmOut, iarg);
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
            sobelPGM(pgmTmp, pgmOut, iarg);
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
            gaussPGM(pgmTmp, pgmOut, farg, iarg);
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
            dogPGM(pgmTmp, pgmOut, farg, iarg);
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
            cedPGM(pgmTmp, pgmOut, farg, 0, iarg, iarg*3);
        }
        
    }
    
    freePGM(&pgmTmp);
    return;
}