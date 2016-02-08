#include "digger.h"

#include <assert.h>

#include "common/alloc_or_die.h"
#include "common/dice.h"
#include "common/fail.h"
#include "common/rnd.h"

#include "dungeon.h"
#include "dungeon_generator.h"


static struct box
box_for_area(struct digger *digger,
             int length,
             int width,
             int left_offset,
             int buffer);


struct area *
digger_dig_area(struct digger *digger,
                int length,
                int width,
                int left_offset,
                enum area_type area_type)
{
    struct box box_to_dig = box_for_area(digger, length, width, left_offset, 0);
    if (dungeon_is_box_excavated(digger->generator->dungeon, box_to_dig)) {
        return NULL;
    }
    
    struct box box_for_level = dungeon_box_for_level(digger->generator->dungeon,
                                                     digger->point.z);
    struct box new_box_for_level = box_for_level = box_make_from_boxes(box_for_level,
                                                                       box_to_dig);
    if (   new_box_for_level.size.width > digger->generator->max_size.width
        || new_box_for_level.size.length > digger->generator->max_size.length)
    {
        return NULL;
    }
    
    struct area *area = dungeon_add_area(digger->generator->dungeon,
                                         area_type,
                                         orientation_from_direction(digger->direction),
                                         box_to_dig,
                                         tile_type_empty);
    digger->point = point_move(digger->point, length, digger->direction);
    return area;
}


struct area *
digger_dig_chamber(struct digger *digger,
                   int length,
                   int width,
                   int left_offset)
{
    int const buffer = 1;
    struct box box_to_dig = box_for_area(digger, length, width, left_offset, buffer);
    if (dungeon_is_box_excavated(digger->generator->dungeon, box_to_dig)) {
        return NULL;
    }
    return digger_dig_area(digger, length, width, left_offset, area_type_chamber);
}


struct area *
digger_dig_intersection(struct digger *digger)
{
    int const length = 1;
    int const width = 1;
    int const left_offset = 0;
    int const buffer = 1;
    struct box box_to_dig = box_for_area(digger, length, width, left_offset, buffer);
    if (dungeon_is_box_excavated(digger->generator->dungeon, box_to_dig)) {
        return NULL;
    }
    return digger_dig_area(digger, length, width, left_offset, area_type_intersection);
}


struct area *
digger_dig_passage(struct digger *digger, int distance)
{
    int const length = distance;
    int const width = 1;
    int const left_offset = 0;
    int const buffer = 1;
    struct box box_to_dig = box_for_area(digger, length, width, left_offset, buffer);
    if (dungeon_is_box_excavated(digger->generator->dungeon, box_to_dig)) {
        return NULL;
    }
    return digger_dig_area(digger, length, width, left_offset, area_type_passage);
}


void
digger_generate_chamber(struct digger *digger)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 2) {
        // square 20x20
        digger_dig_chamber(digger, 2, 2, 0);
    } else if (score <= 4) {
        // square 20x20
        digger_dig_chamber(digger, 2, 2, 1);
    } else if (score <= 6) {
        // square 30x30
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 3);
        digger_dig_chamber(digger, 3, 3, left_offset);
    } else if (score <= 8) {
        // square 40x40
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 4);
        digger_dig_chamber(digger, 4, 4, left_offset);
    } else if (score <= 10) {
        // rectangular 20x30
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 3);
        digger_dig_chamber(digger, 2, 3, left_offset);
    } else if (score <= 13) {
        // rectangular 20x30
        int left_offset = rnd_next_uniform_value(digger->generator->rnd, 2);
        digger_dig_chamber(digger, 3, 2, left_offset);
    } else if (score <= 15) {
        // rectangular 30x50
        int orientation = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (orientation) {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 3);
            digger_dig_chamber(digger, 5, 3, left_offset);
        } else {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 5);
            digger_dig_chamber(digger, 3, 5, left_offset);
        }
    } else if (score <= 17) {
        // rectangular 40x60
        int orientation = rnd_next_uniform_value(digger->generator->rnd, 2);
        if (orientation) {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 4);
            digger_dig_chamber(digger, 6, 4, left_offset);
        } else {
            int left_offset = rnd_next_uniform_value(digger->generator->rnd, 6);
            digger_dig_chamber(digger, 4, 6, left_offset);
        }
    } else {
        // unusual shape and size
    }
    // TODO: exits
    dungeon_generator_delete_digger(digger->generator, digger);
}


void
digger_generate_side_passage(struct digger *digger)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 2) {
        // left 90 degrees
        if (!digger_dig_intersection(digger)) return;
        
        struct digger *side_digger = dungeon_generator_copy_digger(digger->generator,
                                                                   digger);
        digger_turn_90_degrees_left(side_digger);
        digger_dig_passage(side_digger, 3);
        
        digger_dig_passage(digger, 3);
    } else if (score <= 4) {
        // right 90 degrees
        if (!digger_dig_intersection(digger)) return;
        
        struct digger *side_digger = dungeon_generator_copy_digger(digger->generator,
                                                                   digger);
        digger_turn_90_degrees_right(side_digger);
        digger_dig_passage(side_digger, 3);
        
        digger_dig_passage(digger, 3);
    } else if (score == 5) {
        // left 45 degrees ahead
    } else if (score == 6) {
        // right 45 degrees ahead
    } else if (score == 7) {
        // left 45 degrees behind
    } else if (score == 8) {
        // right 45 degrees behind
    } else if (score == 9) {
        // left curve 45 degrees ahead
    } else if (score == 10) {
        // right curve 45 degrees ahead
    } else if (score <= 13) {
        // passage T's
        if (!digger_dig_intersection(digger)) return;
        
        struct digger *left_digger = dungeon_generator_copy_digger(digger->generator,
                                                                   digger);
        digger_turn_90_degrees_left(left_digger);
        digger_dig_passage(left_digger, 3);
        
        struct digger *right_digger = dungeon_generator_copy_digger(digger->generator,
                                                                    digger);
        digger_turn_90_degrees_right(right_digger);
        digger_dig_passage(right_digger, 3);
        
        dungeon_generator_delete_digger(digger->generator, digger);
    } else if (score <= 15) {
        // passage Y's
    } else if (score < 19) {
        // four way intersection
        if (!digger_dig_intersection(digger)) return;
        
        struct digger *left_digger = dungeon_generator_copy_digger(digger->generator,
                                                                   digger);
        digger_turn_90_degrees_left(left_digger);
        digger_dig_passage(left_digger, 3);
        
        struct digger *right_digger = dungeon_generator_copy_digger(digger->generator,
                                                                    digger);
        digger_turn_90_degrees_right(right_digger);
        digger_dig_passage(right_digger, 3);
        
        digger_dig_passage(digger, 3);
    } else {
        // passage X's
    }
}


void
digger_generate_turn(struct digger *digger)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 8) {
        // left 90 degrees
        digger_turn_90_degrees_left(digger);
        digger_dig_passage(digger, 3);
    } else if (score == 9) {
        // left 45 degrees ahead
    } else if (score == 10) {
        // left 45 degrees behind
    } else if (score <= 18) {
        // right 90 degrees
        digger_turn_90_degrees_right(digger);
        digger_dig_passage(digger, 3);
    } else if (score == 19) {
        // right 45 degrees ahead
    } else {
        // right 45 degrees behind
    }
}


void
digger_move(struct digger *digger, int steps, enum direction direction)
{
    digger->point = point_move(digger->point, steps, direction);
}


void
digger_periodic_check(struct digger *digger)
{
    int score = roll("1d20", digger->generator->rnd);
    if (score <= 2) {
        digger_dig_passage(digger, 6);
    } else if (score <= 5) {
        // door
    } else if (score <= 10) {
        // side passage
        digger_generate_side_passage(digger);
    } else if (score <= 13) {
        // passage turns
        digger_generate_turn(digger);
    } else if (score <= 16) {
        // chamber
        digger_generate_chamber(digger);
    } else if (score == 17) {
        // stairs
    } else if (score == 18) {
        // dead end
        dungeon_generator_delete_digger(digger->generator, digger);
    } else if (score == 19) {
        // trick/trap
    } else {
        // wandering monster
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


static struct box
box_for_area(struct digger *digger,
             int length,
             int width,
             int left_offset,
             int buffer)
{
    assert(left_offset < width);
    struct point origin;
    struct size size;
    switch (digger->direction) {
        case direction_north:
            origin = point_make(digger->point.x - left_offset - buffer,
                                digger->point.y,
                                digger->point.z);
            size = size_make(width + (2 * buffer), length + buffer, 1);
            break;
        case direction_south:
            origin = point_make(digger->point.x + left_offset + buffer,
                                digger->point.y,
                                digger->point.z);
            size = size_make(-width - (2 * buffer), -length - buffer, 1);
            break;
        case direction_east:
            origin = point_make(digger->point.x,
                                digger->point.y + left_offset + buffer,
                                digger->point.z);
            size = size_make(length + buffer, -width - (2 * buffer), 1);
            break;
        case direction_west:
            origin = point_make(digger->point.x,
                                digger->point.y - left_offset - buffer,
                                digger->point.z);
            size = size_make(-length - buffer, +width + (2 * buffer), 1);
            break;
        default:
            fail("Unrecognized direction %i", digger->direction);
            origin = point_make(0, 0, 0);
            size = size_make(0, 0, 0);
            break;
    }
    return box_normalize(box_make(origin, size));
}
