/*! \file  imageFilters.h
 *  \brief Interfaces for functions to create and free filters.
 *  \author Eleonora Maria Aiello
 *  \author Gianluca Gerard
 *  \date 24/10/15
 *  \copyright Apache License Version 2.0, January 2004
 */

#ifndef imageFilters_h
#define imageFilters_h

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "imageUtilities.h"

/*! \def smallestOdd(x)
 *    \brief Find the smallestOdd next to \a x .
 *
 *    It returns \a x if it is odd, \a x - 1 if it is even.
 */
#define smallestOdd(x) x%2 == 1? x: x-1

/*! \struct Filter
 *    \brief The structure used for image filters.
 */
typedef struct
{
    int width;      /*!< Filter's width */
    int height;     /*!< Filter's height */
    double* kernel; /*!< Pointer to array with the filter's values */
} Filter;

//---------------------------------------------------------//
//--------------- Basic Functions for Filters -------------//
//---------------------------------------------------------//

Filter* newFilter(int width, int height);
void freeFilter(Filter** filter);
void printFilter(Filter* filter);

Filter *linearAddFilter(Filter* filterOp1, Filter* filterOp2, double w1, double w2);

//---------------------------------------------------------//
//---------------------- Basic Filters --------------------//
//---------------------------------------------------------//
Filter* identityFilter(int width, int height);
Filter* boxFilter(int width, int height);
Filter* prewittXFilter();
Filter* prewittYFilter();
Filter* sobelXFilter();
Filter* sobelYFilter();

Filter* gauss1DXFilter(double sigma, int width);
Filter* gauss1DYFilter(double sigma, int height);
Filter* gauss2DFilter(double sigma, int dim);
Filter* DoGFilter(double sigma, int dim);

#endif /* imageFilters_h */
