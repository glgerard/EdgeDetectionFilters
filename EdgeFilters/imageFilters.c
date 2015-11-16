/*! \file  imageFilters.c
 *  \brief Functions to create and free filters.
 *  \author Eleonora Maria Aiello
 *  \author Gianluca Gerard
 *  \date 24/10/15
 *  \copyright Apache License Version 2.0, January 2004
 */

#include "imageFilters.h"
#include "imageFilterOps.h"

/*! \fn Filter* newFilter(int width, int height)
 * \brief Create a new image filter of size \a width x \a height.
 *
 * \param width Filter's width.
 * \param height Filter's height.
 * \return Pointer to the newly created Filter structure or NULL if either one of the two dimensions is less than 1.
 */
Filter* newFilter(int width, int height)
{
    if ((width < 1) || (height<1)) {
        fprintf(stderr, "Error! Dimensions must both be greater than 0. Please Check.\n");
        return NULL;
    }
    Filter* newFilter = (Filter*)malloc(1*sizeof(Filter));
    newFilter->width = width;
    newFilter->height = height;
    newFilter->kernel = (double*) calloc(width*height, sizeof(double));
    
    return newFilter;
}

/*! \fn void freeFilter(Filter** filter)
 * \brief Free a Filter structure whose pointer is stored in \a filter.
 *
 * \param filter The pointer to a filter's pointer.
 */
void freeFilter(Filter** filter)
{
    if (*filter == NULL) {
        fprintf(stderr, "Error! Filter was already free. Please Check.\n");
        return;
    }
    
    free((*filter)->kernel);
    (*filter)->kernel = NULL;
    free(*filter);
    *filter = NULL;
}

/*! \fn void printFilter(Filter* filter)
 * \brief Print the values of the Filter pointed by \a filter.
 *
 * \param filter The pointer to the Filter.
 */
void printFilter(Filter* filter)
{
    int i;
    int width = filter->width;
    int height = filter->height;
    int il=0;
    
    if (!filter) {
        fprintf(stderr, "Error! No input data. Please Check.\n");
        return;
    }
    
    for (i=0; i<height; i++) {
        printf("[");
        for (int j=0; j<width; j++) {
            printf("%f ",filter->kernel[il++]);
        }
        printf("]\n");
    }
}

/*! \fn Filter* identityFilter(int width, int height)
 * \brief It creates an identity Filter.
 *
 * The identify filter has all values at 0 except for the one at the center
 * of the Filter's array which is set to 1.
 * \param width Filter's width.
 * \param height Filter's height.
 * \return Pointer to the newly created Filter structure or NULL if the Filter could not be created.
 * \warning \a width and \a height must both be odd values.
 */
Filter* identityFilter(int width, int height)
{
    int i;
    
    Filter* filter = newFilter(width,height);
    if (!filter) {
        return NULL;
    }
    for (i=0; i<height*width; i++)
        filter->kernel[i] = 0.0;
    filter->kernel[width*height/2] = 1.0;
    return filter;
}

/*! \fn Filter* boxFilter(int width, int height)
 * \brief It creates a box Filter.
 *
 * The box filter has all values set to the inverse of (\a width * \a height).
 *
 * \param width Filter's width.
 * \param height Filter's height.
 * \return Pointer to the newly created Filter structure or NULL if the Filter could not be created.
 */
Filter* boxFilter(int width, int height)
{
    int i;
    double weigth = 1.0/(width*height);

    Filter* filter = newFilter(width,height);
    if (!filter) {
        return NULL;
    }
    for (i=0; i<height*width; i++)
        filter->kernel[i] = weigth;
    return filter;
}

/*! \fn Filter* genericFilter(const double* matrix, int width, int height)
 * \brief Creates a generic filter copying the values in matrix.
 *
 * The box filter has all values set to the inverse of (\a width * \a height).
 *
 * \param matrix Pointer to the array of values that will be copied in the filter's matrix.
 * \param width Filter's width.
 * \param height Filter's height.
 * \return Pointer to the newly created Filter structure or NULL if the Filter could not be created.
 */
Filter* genericFilter(const double* matrix, int width, int height)
{
    int i;
    Filter* filter = newFilter(width, height);
    if (!filter) {
        return NULL;
    }
    for (i=0; i<height*width; i++)
        filter->kernel[i] = matrix[i];
    return filter;
}

const static double prewittX[] = {
     1.0, 1.0, 1.0,
     0.0, 0.0, 0.0,
    -1.0,-1.0,-1.0};

/*! \fn Filter* prewittXFilter()
 * \brief Creates the vertical Prewitt filter.
 *
 * \return Pointer to the vertical Prewitt filter.
 */
Filter* prewittXFilter()
{
    return genericFilter(prewittX,3,3);
}

const static double prewittY[] = {
     1.0, 0.0,-1.0,
     1.0, 0.0,-1.0,
     1.0, 0.0,-1.0};

/*! \fn Filter* prewittYFilter()
 * \brief Creates the horizontal Prewitt filter.
 *
 * \return Pointer to the horizontal Prewitt filter.
 */
Filter* prewittYFilter()
{
    return genericFilter(prewittY,3,3);
}

const static double sobelX[] = {
     1.0, 2.0, 1.0,
     0.0, 0.0, 0.0,
    -1.0,-2.0,-1.0};

/*! \fn Filter* sobelXFilter()
 * \brief Creates the vertical Sobel filter.
 *
 * \return Pointer to the vertical Sobel filter.
 */
Filter* sobelXFilter()
{
    return genericFilter(sobelX,3,3);
}

const static double sobelY[] = {
     1.0, 0.0,-1.0,
     2.0, 0.0,-2.0,
     1.0, 0.0,-1.0};

/*! \fn Filter* sobelYFilter()
 * \brief Creates the horizontal Sobel filter.
 *
 * \return Pointer to the horizontal Sobel filter.
 */
Filter* sobelYFilter()
{
    return genericFilter(sobelY,3,3);
}

/*! \fn Filter* gauss1DFilter(double sigma, int dim, Filter *filter)
 * \brief Creates a 1 dimensional Gauss filter with sigma \a sigma and linear dimension \a dim.
 *
 * \param sigma The sigma of the Gaussian.
 * \param dim The linear dimension of the filter.
 * \param filter Pointer to the Filter structure where the filters values are stored.
 * \return Pointer to the Gauss filter.
 */
Filter* gauss1DFilter(double sigma, int dim, Filter *filter)
{
    int i;
    double gc = M_2_SQRTPI*M_SQRT1_2/(2*sigma);
    double sigma2 = sigma*sigma;
    
    int halfDim = dim/2;
    
    int x=-halfDim;
    for (i=0; i<dim; i++) {
        filter->kernel[i] = gc*exp(-x*x/sigma2);
        x++;
    }
    
    return filter;
}

/*! \fn Filter* gauss1DXFilter(double sigma, int width)
 * \brief Creates a 1 dimensional Gauss filter with 1 row and \a width columns.
 *
 * \param sigma The sigma of the Gaussian.
 * \param width The columns of the filter.
 * \return Pointer to the Gauss filter.
 */
Filter* gauss1DXFilter(double sigma, int width)
{
    if (width == 0)
        width = smallestOdd((int)(6*sigma));
    
    Filter* filter = newFilter(width,1);
    
    return gauss1DFilter(sigma, width, filter);
}

/*! \fn Filter* gauss1DYFilter(double sigma, int height)
 * \brief Creates a 1 dimensional Gauss filter with 1 column and \a height rows.
 *
 * \param sigma The sigma of the Gaussian.
 * \param height The rows of the filter.
 * \return Pointer to the Gauss filter.
 */
Filter* gauss1DYFilter(double sigma, int height)
{
    if (height == 0)
        height = smallestOdd((int)(6*sigma));

    Filter* filter = newFilter(1,height);
    
    return gauss1DFilter(sigma, height, filter);
}

/*! \fn Filter* gauss2DFilter(double sigma, int dim)
 * \brief Creates a 2-dimensional Gauss filter with \a dim columns and \a dim rows.
 *
 * \param sigma The sigma of the Gaussian.
 * \param dim The number of rows and columns of the filter. If it is set to 0 then the dimension
 *        is the smallest odd number next to 6 \a sigma.
 * \return Pointer to the Gauss filter.
 */
Filter* gauss2DFilter(double sigma, int dim)
{
    int x, y;
    double gc;
    double sigma2 = sigma*sigma;
    double gc0 = M_1_PI/(2*sigma2);

    if (dim == 0)
        dim = smallestOdd((int)(6*sigma));
    
    Filter* filter = newFilter(dim,dim);

    int halfDim = dim/2;
    
    int il = 0;
    for (x=-halfDim; x<=halfDim; x++) {
        gc = gc0*exp(-x*x/sigma2);
        for (y=-halfDim; y<=halfDim; y++)
            filter->kernel[il++] = gc*exp(-y*y/sigma2);
    }
    
    return filter;
}

/*! \fn Filter* DoGFilter(double sigma, int dim)
 * \brief Creates a DoG filter with \a dim columns and \a dim rows.
 *
 * This filter is the result of the difference of two equal Gaussian filters with equal size and different
 * sigmas. The external gaussian has sigma set by \a sigma. The internal gaussian has a smaller sigma equal to
 * \a sigma / 1.66 .
 *
 * \param sigma The sigma of the external Gaussian.
 * \param dim The number of rows and columns of the filter. If it is set to 0 then the dimension
 *        is the smallest odd number next to 6 \a sigma.
 * \return Pointer to the DoG filter.
 */
Filter* DoGFilter(double sigma, int dim)
{
    double sigmaExt = sigma;
    double sigmaInt = sigma/1.66;
    
    if (dim == 0)
        dim = (int)(4*sigma)+1;
    
    Filter *gaussExtFilter = gauss2DFilter(sigmaExt, dim);
    Filter *gaussIntFilter = gauss2DFilter(sigmaInt, dim);
    
    Filter *filter = linearAddFilter(gaussExtFilter, gaussIntFilter, 1.0, -1.0);
    
    return filter;
}