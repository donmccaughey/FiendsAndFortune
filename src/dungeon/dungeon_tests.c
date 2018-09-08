#include <stdlib.h>
#include <base/base.h>


void
box_test(void);

void
direction_test(void);

void
dungeon_test(void);

void
generator_test(void);

void
point_test(void);


int
main(int argc, char *argv[])
{
    box_test();
    direction_test();
    dungeon_test();
    generator_test();
    point_test();
    alloc_count_is_zero_or_die();
    return EXIT_SUCCESS;
}
