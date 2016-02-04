#include "orientation.h"


enum orientation
orientation_from_direction(enum direction direction)
{
    switch (direction) {
        case direction_north: return orientation_north_south;
        case direction_south: return orientation_north_south;
        case direction_east: return orientation_east_west;
        case direction_west: return orientation_east_west;
        default: return orientation_unknown;
    }
}


char const *
orientation_name(enum orientation orientation)
{
    switch (orientation) {
        case orientation_north_south: return "North to South";
        case orientation_east_west: return "East to West";
        default: return "unknown";
    }
}
