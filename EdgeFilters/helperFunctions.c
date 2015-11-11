//
//  helperFunctions.c
//  EdgeFilters
//
//  Created by Gianluca Gerard on 07/11/15.
//  Copyright © 2015 Gianluca Gerard. All rights reserved.
//

#include "helperFunctions.h"


//--------------------------------------------------------//
//-------------      Compute a modulo b       ------------//
//--------------------------------------------------------//
int mod (int a, int b)
{
    int ret = a % b;
    if(ret < 0)
        ret+=b;
    return ret;
}

//--------------------------------------------------------//
//----------- Compute the max between a and b ------------//
//--------------------------------------------------------//
int max (int a, int b)
{
    return a >= b ? a : b;
}

//--------------------------------------------------------//
//--------- Compute the mean values of an array ----------//
//--------------------------------------------------------//
double mean(int *array, int len)
{
    int sum = 0;
    
    for (int i=0; i<len; i++)
        sum += array[i];
    
    return (double)sum/len;
}

//--------------------------------------------------------//
//----------- Compute the variance of an array -----------//
//--------------------------------------------------------//
double var(int* array, int len)
{
    double sum = 0;
    double m = mean(array, len);
    
    for (int i=0; i<len; i++)
        sum += (array[i]-m)*(array[i]-m);
    
    return sum/len;
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
//------ Remove leading and trailing white spaces --------//
//--------------------------------------------------------//
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

int quadrant(int a)
{
    int angle = (int)(a/22.5);

    /**
    if ((-8 <= angle) && (angle < -7))
        return 0;
    else if ((-7 <= angle) && (angle < -5))
        return 1;
    else if ((-5 <= angle) && (angle < -3))
        return 2;
    else if ((-3 <= angle) && (angle < -1))
        return 3;
    else if ((-1 <= angle) && (angle < 1) )
        return 0;
    else if ((1 <= angle) && (angle < 3))
        return 1;
    else if ((3 <= angle) && (angle < 5))
        return 2;
    else if ((5 <= angle) && (angle < 7))
        return 3;
    else if ((angle <= 8))
        return 0;
**/
    
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