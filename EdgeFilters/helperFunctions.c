/*! \file  helperFunctions.c
 *  \brief Basic math, sort and line processing functions.
 *  \author Eleonora Maria Aiello
 *  \author Gianluca Gerard
 *  \date 07/11/15
 *  \copyright Apache License Version 2.0, January 2004
 */

#include "helperFunctions.h"

/*! \fn int mod (int a, int b)
 *   \brief Return the modulus of \a a given \a b.
 */
int mod (int a, int b)
{
    int ret = a % b;
    if(ret < 0)
        ret+=b;
    return ret;
}

/*! \fn int max (int a, int b)
 *   \brief Return the maximum between \a a and \a b.
 */
int max (int a, int b)
{
    return a >= b ? a : b;
}

/*! \fn double mean(int *array, int len)
 *   \brief Return the average value of an array \a array of length \a len.
 */
double mean(int *array, int len)
{
    int sum = 0;
    
    for (int i=0; i<len; i++)
        sum += array[i];
    
    return (double)sum/len;
}

/*! \fn double var(int* array, int len)
 *   \brief Return the variance of an array \a array of length \a len.
 */
double var(int* array, int len)
{
    double sum = 0;
    double m = mean(array, len);
    
    for (int i=0; i<len; i++)
        sum += (array[i]-m)*(array[i]-m);
    
    return sum/len;
}

/*! \fn int* sort(int* array, int len)
 *   \brief Return the array \a array of length \a len sorted in increasing order.
 */
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

/*! \fn char *trimwhitespace(char *str)
 *   \brief Remove leading and trailing white spaces from the null terminated string \a str.
 */
char *trimwhitespace(char *str)
{
    char *end;
    
    // Trim leading space
    while(isspace(*str)) str++;
    
    if(*str == 0)  // All spaces?
        return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;
    
    // Write new null terminator
    *(end+1) = 0;
    
    return str;
}

/*! \fn int quadrant(int a)
 *   \brief Return one of four quadrants where the angle \a a falls. The angle is in degree.
 *
 *   The angle \a a is categorized in one of the following 4 quadrants:
 *      - 0: [ -22.5,  +22.5] or [+157.5, +180.0] or [-157.5, -180.0]
 *      - 1: [ +22.5,  +67.5] or [-157.5, -112.5]
 *      - 2: [ +67.5, +112.5] or [-112.5,  -67.5]
 *      - 3: [+112.5, +157.5] or [ -67.5,  -22.5]
 */
int quadrant(int a)
{
    int angle = (int)(a/22.5);
    
    if ((-6 == angle) || (angle == -5))
        return 1;
    else if ((-4 == angle) || (angle == -3))
        return 2;
    else if ((-2 == angle) || (angle == -1))
        return 3;
    else if ((1 == angle) || (angle == 2))
        return 1;
    else if ((3 == angle) || (angle == 4))
        return 2;
    else if ((5 == angle) || (angle == 6))
        return 3;
    
    return 0;
}