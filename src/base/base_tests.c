#include <stdlib.h>
#include <base/base.h>


void
alloc_or_die_test(void);

void
int_test(void);

void
ptr_array_test(void);

void
result_test(void);

void
rnd_test(void);

void
sort_test(void);

void
str_test(void);


int
main(int argc, char *argv[])
{
    alloc_or_die_test();
    int_test();
    ptr_array_test();
    result_test();
    rnd_test();
    sort_test();
    str_test();
    alloc_count_is_zero_or_die();
    return EXIT_SUCCESS;
}
