//
//  helperFunctions.h
//  EdgeFilters
//
//  Created by Gianluca Gerard on 07/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

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
