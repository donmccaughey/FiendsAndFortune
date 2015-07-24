#include <stdio.h>
#include <stdlib.h>

#include "alloc_or_die.h"

void
dice_test(void);

void
dungeonTest(void);

void
pointTest(void);

void
pointsTest(void);

void
rangeTest(void);

void
tilesTest(void);


int main (int argc, const char *argv[])
{
    FILE *out = stdout;
    fprintf(out, "Fiends and Fortune unit tests\n");
    
    dice_test();
    dungeonTest();
    pointTest();
    pointsTest();
    rangeTest();
    tilesTest();
    
    fprintf(out, "Okay\n");
    alloc_count_is_zero_or_die();
    return EXIT_SUCCESS;
}
