/*! \file  helperFunctions.h
 *  \brief Interfaces to basic math, sort and line processing functions.
 *  \author Eleonora Maria Aiello
 *  \author Gianluca Gerard
 *  \date 07/11/15
 *  \copyright Apache License Version 2.0, January 2004
 */

#ifndef helperFunctions_h
#define helperFunctions_h

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

int mod (int a, int b);
int max (int a, int b);
double mean(int *array, int len);
double var(int* array, int len);
int* sort(int* array, int len);
char *trimwhitespace(char *str);
int quadrant(int a);

#endif /* helperFunctions_h */
