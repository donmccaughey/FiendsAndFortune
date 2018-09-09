#include <assert.h>
#include <background/background.h>
#include <base/base.h>
#include <dungeon/dungeon.h>


void
orientation_test(void);


static void
orientation_from_direction_test(void)
{
    assert(orientation_north_to_south == orientation_from_direction(direction_north));
    assert(orientation_north_to_south == orientation_from_direction(direction_south));

    assert(orientation_east_to_west == orientation_from_direction(direction_east));
    assert(orientation_east_to_west == orientation_from_direction(direction_west));

    assert(orientation_northeast_to_southwest == orientation_from_direction(direction_northeast));
    assert(orientation_northeast_to_southwest == orientation_from_direction(direction_southwest));

    assert(orientation_southeast_to_northwest == orientation_from_direction(direction_northwest));
    assert(orientation_southeast_to_northwest == orientation_from_direction(direction_southeast));
}


static void
orientation_name_test(void)
{
    assert(str_eq("north to south", orientation_name(orientation_north_to_south)));
    assert(str_eq("east to west", orientation_name(orientation_east_to_west)));
    assert(str_eq("northeast to southwest", orientation_name(orientation_northeast_to_southwest)));
    assert(str_eq("southeast to northwest", orientation_name(orientation_southeast_to_northwest)));
}


void
orientation_test(void)
{
    orientation_from_direction_test();
    orientation_name_test();
}
