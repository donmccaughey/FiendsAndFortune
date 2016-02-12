#include "digger.h"

#include <assert.h>

#include "common/alloc_or_die.h"
#include "common/dice.h"
#include "common/fail.h"
#include "common/rnd.h"

#include "area.h"
#include "generator.h"
#include "tile.h"


static void
location_of_door(struct rnd *rnd, bool *left, bool *right, bool *ahead);

static bool
space_beyond_door(struct digger *digger, bool is_straight_ahead);


static struct box
box_for_area(struct digger *digger,
             int length,
             int width,
             int left_offset,
             int padding)
{
    assert(left_offset < width);
    struct point origin;
    struct size size;
    switch (digger->direction) {
        case direction_north:
            origin = point_make(digger->point.x - left_offset - padding,
                                digger->point.y,
                                digger->point.z);
            size = size_make(width + (2 * padding), length + padding, 1);
            break;
        case direction_south:
            origin = point_make(digger->point.x + left_offset + padding,
                                digger->point.y,
                                digger->point.z);
            size = size_make(-width - (2 * padding), -length - padding, 1);
            break;
        case direction_east:
            origin = point_make(digger->point.x,
                                digger->point.y + left_offset + padding,
                                digger->point.z);
            size = size_make(length + padding, -width - (2 * padding), 1);
            break;
        case direction_west:
            origin = point_make(digger->point.x,
                                digger->point.y - left_offset - padding,
                                digger->point.z);
            size = size_make(-length - padding, +width + (2 * padding), 1);
            break;
        default:
            fail("Unrecognized direction %i", digger->direction);
            origin = point_make(0, 0, 0);
            size = size_make(0, 0, 0);
            break;
    }
    return box_normalize(box_make(origin, size));
}


struct area *
digger_dig_area(struct digger *digger,
                int length,
                int width,
                int left_offset,
                enum wall_type entrance_type,
                enum area_type area_type)
{
    int const padding = 0;
    struct box box_to_dig = box_for_area(digger,
                                         length,
                                         width,
                                         left_offset,
                                         padding);
    if (generator_is_box_excavated(digger->generator, box_to_dig)) {
        return NULL;
    }
    
    struct box box_for_level = generator_box_for_level(digger->generator,
                                                       digger->point.z);
    struct box new_box_for_level = box_for_level = box_make_from_boxes(box_for_level,
                                                                       box_to_dig);
    if (   new_box_for_level.size.width > digger->generator->max_size.width
        || new_box_for_level.size.length > digger->generator->max_size.length)
    {
        return NULL;
    }
    
    struct area *area = generator_add_area(digger->generator,
                                           area_type,
                                           orientation_from_direction(digger->direction),
                                           box_to_dig,
                                           tile_type_empty);
    
    struct tile *entrance_tile;
    switch (digger->direction) {
        case direction_north:
            entrance_tile = generator_tile_at(digger->generator, digger->point);
            entrance_tile->walls.south = entrance_type;
            break;
        case direction_south:
            entrance_tile = generator_tile_at(digger->generator,
                                              point_north(digger->point));
            entrance_tile->walls.south = entrance_type;
            break;
        case direction_east:
            entrance_tile = generator_tile_at(digger->generator, digger->point);
            entrance_tile->walls.west = entrance_type;
            break;
        case direction_west:
            entrance_tile = generator_tile_at(digger->generator,
                                              point_east(digger->point));
            entrance_tile->walls.west = entrance_type;
            break;
        default:
            fail("Unrecognized direction %i", digger->direction);
            break;
    }
    
    digger->point = point_move(digger->point, length, digger->direction);
    return area;
}


struct area *
digger_dig_chamber(struct digger *digger,
                   int length,
                   int width,
                   int left_offset,
                   enum wall_type entrance_type)
{
    struct box padded_box = box_for_area(digger,
                                         length,
                                         width,
                                         left_offset,
                                         digger->generator->padding);
    if (generator_is_box_excavated(digger->generator, padded_box)) {
        return NULL;
    }
    return digger_dig_area(digger,
                           length,
                           width,
                           left_offset,
                           entrance_type,
                           area_type_chamber);
}


struct area *
digger_dig_intersection(struct digger *digger)
{
    int const length = 1;
    int const width = 1;
    int const left_offset = 0;
    struct box padded_box = box_for_area(digger,
                                         length,
                                         width,
                                         left_offset,
                                         digger->generator->padding);
    if (generator_is_box_excavated(digger->generator, padded_box)) {
        return NULL;
    }
    return digger_dig_area(digger,
                           length,
                           width,
                           left_offset,
                           wall_type_none,
                           area_type_intersection);
}


struct area *
digger_dig_passage(struct digger *digger,
                   int distance,
                   enum wall_type entrance_type)
{
    int const length = distance;
    int const width = 1;
    int const left_offset = 0;
    struct box padded_box = box_for_area(digger,
                                         length,
                                         width,
                                         left_offset,
                                         digger->generator->padding);
    if (generator_is_box_excavated(digger->generator, padded_box)) {
        return NULL;
    }
    return digger_dig_area(digger,
                           length,
                           width,
                           left_offset,
                           entrance_type,
                           area_type_passage);
}


struct area *
digger_dig_room(struct digger *digger,
                int length,
                int width,
                int left_offset,
                enum wall_type entrance_type)
{
    struct box padded_box = box_for_area(digger,
                                         length,
                                         width,
                                         left_offset,
                                         digger->generator->padding);
    if (generator_is_box_excavated(digger->generator, padded_box)) {
        return NULL;
    }
    return digger_dig_area(digger,
                           length,
                           width,
                           left_offset,
                           entrance_type,
                           area_type_room);
}


bool
digger_chambers(struct digger *digger, enum wall_type entrance_type)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 2) {
        // square 20x20
        if (!digger_dig_chamber(digger, 2, 2, 0, entrance_type)) return false;
    } else if (score <= 4) {
        // square 20x20
        if (!digger_dig_chamber(digger, 2, 2, 1, entrance_type)) return false;
    } else if (score <= 6) {
        // square 30x30
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 3);
        if (!digger_dig_chamber(digger, 3, 3, left_offset, entrance_type)) {
            return false;
        }
    } else if (score <= 8) {
        // square 40x40
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 4);
        if (!digger_dig_chamber(digger, 4, 4, left_offset, entrance_type)) {
            return false;
        }
    } else if (score <= 10) {
        // rectangular 20x30
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 3);
        if (!digger_dig_chamber(digger, 2, 3, left_offset, entrance_type)) {
            return false;
        }
    } else if (score <= 13) {
        // rectangular 20x30
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (!digger_dig_chamber(digger, 3, 2, left_offset, entrance_type)) {
            return false;
        }
    } else if (score <= 15) {
        // rectangular 30x50
        int orientation = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (orientation) {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 3);
            if (!digger_dig_chamber(digger, 5, 3, left_offset, entrance_type)) {
                return false;
            }
        } else {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 5);
            if (!digger_dig_chamber(digger, 3, 5, left_offset, entrance_type)) {
                return false;
            }
        }
    } else if (score <= 17) {
        // rectangular 40x60
        int orientation = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (orientation) {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 4);
            if (!digger_dig_chamber(digger, 6, 4, left_offset, entrance_type)) {
                return false;
            }
        } else {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 6);
            if (!digger_dig_chamber(digger, 4, 6, left_offset, entrance_type)) {
                return false;
            }
        }
    } else {
        // unusual shape and size
    }
    // TODO: exits
    generator_delete_digger(digger->generator, digger);
    return true;
}


bool
digger_doors(struct digger *digger)
{
    struct digger *door_digger = generator_copy_digger(digger->generator,
                                                       digger);
    bool door_left = false;
    bool door_right = false;
    bool door_ahead = false;
    
    location_of_door(digger->generator->rnd,
                     &door_left, &door_right, &door_ahead);
    if (!door_ahead) {
        // repeat periodic check
        int score = roll("1d20", digger->generator->rnd);
        if (score >= 3 && score <= 5) {
            location_of_door(digger->generator->rnd,
                             &door_left, &door_right, &door_ahead);
        } else {
            if (!digger_dig_passage(digger, 3, wall_type_none)) return false;
        }
    }
    
    if (door_left) {
        struct digger *left_digger = generator_copy_digger(digger->generator,
                                                           door_digger);
        digger_turn_90_degrees_left(left_digger);
        if (!space_beyond_door(left_digger, false)) return false;
    }
    
    if (door_right) {
        struct digger *right_digger = generator_copy_digger(digger->generator,
                                                            door_digger);
        digger_turn_90_degrees_right(right_digger);
        if (!space_beyond_door(right_digger, false)) return false;
    }
    
    if (door_ahead) {
        struct digger *ahead_digger = generator_copy_digger(digger->generator,
                                                            door_digger);
        if (!space_beyond_door(ahead_digger, true)) return true;
    }
    
    generator_delete_digger(digger->generator, door_digger);
    return true;
}


bool
digger_rooms(struct digger *digger, enum wall_type entrance_type)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 2) {
        // square 10x10
        if (!digger_dig_room(digger, 1, 1, 0, entrance_type)) return false;
    } else if (score <= 4) {
        // square 20x20
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (!digger_dig_room(digger, 2, 2, left_offset, entrance_type)) {
            return false;
        }
    } else if (score <= 6) {
        // square 30x30
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 3);
        if (!digger_dig_room(digger, 3, 3, left_offset, entrance_type)) {
            return false;
        }
    } else if (score <= 8) {
        // square 40x40
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 4);
        if (!digger_dig_room(digger, 4, 4, left_offset, entrance_type)) {
            return false;
        }
    } else if (score <= 10) {
        // rectangular 10x20
        int orientation = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (orientation) {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 2);
            if (!digger_dig_room(digger, 1, 2, left_offset, entrance_type)) {
                return false;
            }
        } else {
            if (!digger_dig_room(digger, 2, 1, 0, entrance_type)) return false;
        }
    } else if (score <= 13) {
        // rectangular 20x30
        int orientation = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (orientation) {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 2);
            if (!digger_dig_room(digger, 3, 2, left_offset, entrance_type)) {
                return false;
            }
        } else {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 3);
            if (!digger_dig_room(digger, 2, 3, left_offset, entrance_type)) {
                return false;
            }
        }
    } else if (score <= 15) {
        // rectangular 20x40
        int orientation = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (orientation) {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 2);
            if (!digger_dig_room(digger, 4, 2, left_offset, entrance_type)) {
                return false;
            }
        } else {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 4);
            if (!digger_dig_room(digger, 2, 4, left_offset, entrance_type)) {
                return false;
            }
        }
    } else if (score <= 17) {
        // rectangular 30x40
        int orientation = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (orientation) {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 3);
            if (!digger_dig_room(digger, 4, 3, left_offset, entrance_type)) {
                return false;
            }
        } else {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 4);
            if (!digger_dig_room(digger, 3, 4, left_offset, entrance_type)) {
                return false;
            }
        }
    } else {
        // unusual shape and size
    }
    // TODO: exits
    generator_delete_digger(digger->generator, digger);
    return true;
}


bool
digger_side_passages(struct digger *digger)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 2) {
        // left 90 degrees
        if (!digger_dig_intersection(digger)) return false;
        
        struct digger *side_digger = generator_copy_digger(digger->generator,
                                                           digger);
        digger_turn_90_degrees_left(side_digger);
        if (!digger_dig_passage(side_digger, 3, wall_type_none)) return false;
        
        if (!digger_dig_passage(digger, 3, wall_type_none)) return false;
        return true;
    } else if (score <= 4) {
        // right 90 degrees
        if (!digger_dig_intersection(digger)) return false;
        
        struct digger *side_digger = generator_copy_digger(digger->generator,
                                                           digger);
        digger_turn_90_degrees_right(side_digger);
        if (!digger_dig_passage(side_digger, 3, wall_type_none)) return false;
        
        if (!digger_dig_passage(digger, 3, wall_type_none)) return false;
        return true;
    } else if (score == 5) {
        // left 45 degrees ahead
        return true;
    } else if (score == 6) {
        // right 45 degrees ahead
        return true;
    } else if (score == 7) {
        // left 45 degrees behind
        return true;
    } else if (score == 8) {
        // right 45 degrees behind
        return true;
    } else if (score == 9) {
        // left curve 45 degrees ahead
        return true;
    } else if (score == 10) {
        // right curve 45 degrees ahead
        return true;
    } else if (score <= 13) {
        // passage T's
        struct area *intersection = digger_dig_intersection(digger);
        if (!intersection) return false;
        
        struct digger *left_digger = generator_copy_digger(digger->generator,
                                                           digger);
        digger_turn_90_degrees_left(left_digger);
        if (!digger_dig_passage(left_digger, 3, wall_type_none)) return false;
        
        struct digger *right_digger = generator_copy_digger(digger->generator,
                                                            digger);
        digger_turn_90_degrees_right(right_digger);
        if (!digger_dig_passage(right_digger, 3, wall_type_none)) return false;
        
        generator_delete_digger(digger->generator, digger);
        return true;
    } else if (score <= 15) {
        // passage Y's
        return true;
    } else if (score < 19) {
        // four way intersection
        struct area *intersection = digger_dig_intersection(digger);
        if (!intersection) return false;
        
        struct digger *left_digger = generator_copy_digger(digger->generator,
                                                           digger);
        digger_turn_90_degrees_left(left_digger);
        if (!digger_dig_passage(left_digger, 3, wall_type_none)) return false;
        
        struct digger *right_digger = generator_copy_digger(digger->generator,
                                                            digger);
        digger_turn_90_degrees_right(right_digger);
        if (!digger_dig_passage(right_digger, 3, wall_type_none)) return false;
        
        if (!digger_dig_passage(digger, 3, wall_type_none)) return false;
        return true;
    } else {
        // passage X's
        return true;
    }
}


bool
digger_turns(struct digger *digger)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 8) {
        digger_turn_90_degrees_left(digger);
        return digger_dig_passage(digger, 3, wall_type_none);
    } else if (score == 9) {
        // left 45 degrees ahead
        return true;
    } else if (score == 10) {
        // left 45 degrees behind
        return true;
    } else if (score <= 18) {
        digger_turn_90_degrees_right(digger);
        return digger_dig_passage(digger, 3, wall_type_none);
    } else if (score == 19) {
        // right 45 degrees ahead
        return true;
    } else {
        // right 45 degrees behind
        return true;
    }
}


void
digger_move(struct digger *digger, int steps, enum direction direction)
{
    digger->point = point_move(digger->point, steps, direction);
}


bool
digger_periodic_check(struct digger *digger)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 2) {
        return digger_dig_passage(digger, 6, wall_type_none);
    } else if (score <= 5) {
        return digger_doors(digger);
    } else if (score <= 10) {
        return digger_side_passages(digger);
    } else if (score <= 13) {
        return digger_turns(digger);
    } else if (score <= 16) {
        return digger_chambers(digger, wall_type_none);
    } else if (score == 17) {
        // stairs
        return true;
    } else if (score == 18) {
        // dead end
        generator_delete_digger(digger->generator, digger);
        return true;
    } else if (score == 19) {
        // trick/trap
        return true;
    } else {
        // wandering monster
        return true;
    }
}


void
digger_turn_90_degrees_left(struct digger *digger)
{
    digger->point = point_rotate_90_degrees_left(digger->point, digger->direction);
    digger->direction = direction_90_degrees_left(digger->direction);
}


void
digger_turn_90_degrees_right(struct digger *digger)
{
    digger->point = point_rotate_90_degrees_right(digger->point, digger->direction);
    digger->direction = direction_90_degrees_right(digger->direction);
}


static void
location_of_door(struct rnd *rnd, bool *left, bool *right, bool *ahead)
{
    int score = roll("1d20", rnd);
    if (score <= 6) {
        *left = true;
    } else if (score <= 12) {
        *right = true;
    } else {
        *ahead = true;
    }
}


static bool
space_beyond_door(struct digger *digger, bool is_straight_ahead)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 4) {
        // parallel passage or 10x10 room
        if (is_straight_ahead) {
            if (!digger_dig_room(digger, 1, 1, 0, wall_type_door)) return true;
        } else {
            if (digger->generator->padding) {
                int const distance = digger->generator->padding;
                if (!digger_dig_passage(digger, distance, wall_type_door)) {
                    return false;
                }
                if (!digger_dig_intersection(digger)) return false;
            } else {
                if (!digger_dig_passage(digger, 1, wall_type_door)) {
                    return false;
                }
            }
            
            struct digger *left_digger = generator_copy_digger(digger->generator,
                                                               digger);
            digger_turn_90_degrees_left(left_digger);
            if (!digger_dig_passage(left_digger, 3, wall_type_none)) {
                return false;
            }
            
            digger_turn_90_degrees_right(digger);
            if (!digger_dig_passage(digger, 3, wall_type_none)) return false;
        }
        return true;
    } else if (score <= 8) {
        // passage straight ahead
        if (!digger_dig_passage(digger, 6, wall_type_door)) return false;
        return true;
    } else if (score == 9) {
        // passage 45 degrees ahead/behind
        return true;
    } else if (score == 10) {
        // passage 45 degrees behind/ahead
        return true;
    } else if (score <= 18) {
        if (is_straight_ahead || !digger->generator->padding) {
            if (!digger_rooms(digger, wall_type_door)) return false;
        } else if (digger->generator->padding) {
            int const distance = digger->generator->padding;
            if (!digger_dig_passage(digger, distance, wall_type_door)) {
                return true;
            }
            if (!digger_rooms(digger, wall_type_none)) return false;
        }
        return true;
    } else {
        if (is_straight_ahead || !digger->generator->padding) {
            if (!digger_chambers(digger, wall_type_door)) return false;
        } else {
            int const distance = digger->generator->padding;
            if (!digger_dig_passage(digger, distance, wall_type_door)) {
                return false;
            }
            if (!digger_chambers(digger, wall_type_none)) return false;
        }
        return true;
    }
}
