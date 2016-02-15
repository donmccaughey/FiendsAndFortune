#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/alloc_or_die.h"
#include "common/dice.h"
#include "common/rnd.h"

#include "character/character.h"

#include "dungeon/area.h"
#include "dungeon/dungeon.h"

#include "treasure/coins.h"
#include "treasure/gem.h"
#include "treasure/jewelry.h"
#include "treasure/magic_item.h"
#include "treasure/treasure.h"
#include "treasure/treasure_map.h"
#include "treasure/treasure_type.h"


static struct option long_options[] = {
    {
        .name="debug",
        .has_arg=no_argument,
        .flag=NULL,
        .val='d'
    },
    {
        .name="help",
        .has_arg=no_argument,
        .flag=NULL,
        .val='h'
    },
    {
        .name="verbose",
        .has_arg=no_argument,
        .flag=NULL,
        .val='v'
    },
    {
        .name=NULL,
        .has_arg=no_argument,
        .flag=NULL,
        .val=0
    }
};

static char const short_options[] = "dhv";


static void
check(FILE *out, char const *constant_number);

static void
enumerate_treasure_items(struct treasure *treasure, FILE *out);

static void
generate_character(struct rnd *rnd, FILE *out, char const *method_name);

static void
generate_each_treasure(struct rnd *rnd, FILE *out);

static void
generate_map(struct rnd *rnd, FILE *out);

static void
generate_magic_items(struct rnd *rnd, FILE *out, int count);

static void
generate_random_dungeon(struct rnd *rnd, FILE *out);

static void
generate_sample_dungeon(struct rnd *rnd, FILE *out);

static void
generate_treasure_type(struct rnd *rnd, FILE *out, char letter);

static void
generate_treasure_type_table(FILE *out);

static void
print_dungeon(struct dungeon *dungeon, FILE *out);

static void
usage(int argc, char *argv[]);


static void
check(FILE *out, char const *constant_number)
{
    uint32_t fixed_value = (uint32_t)strtoul(constant_number, NULL, 10);
    struct rnd *fake_rnd = rnd_alloc_fake_fixed(fixed_value);
    
    generate_treasure_type_table(out);
    generate_map(fake_rnd, out);
    generate_each_treasure(fake_rnd, out);
    generate_sample_dungeon(fake_rnd, out);
    generate_random_dungeon(fake_rnd, out);
    generate_character(fake_rnd, out, "simple");
    generate_character(fake_rnd, out, "method1");
    generate_character(fake_rnd, out, "method2");
    generate_character(fake_rnd, out, "method3");
    generate_character(fake_rnd, out, "method4");
    generate_character(fake_rnd, out, "generalnpc");
    generate_character(fake_rnd, out, "specialnpc");
    
    rnd_free(fake_rnd);
}


static void
enumerate_treasure_items(struct treasure *treasure, FILE *out)
{
    if (treasure->gems_count) {
        fprintf(out, "  Gems: --------------------------------\n");
        for (int i = 0; i < treasure->gems_count; ++i) {
            fprintf(out, "    %2i  %s\n", i + 1, treasure->gems[i].true_description);
        }
    }
    
    if (treasure->jewelry_count) {
        fprintf(out, "  Jewelry: -----------------------------\n");
        for (int i = 0; i < treasure->jewelry_count; ++i) {
            fprintf(out, "    %2i  %s\n",
                    i + 1, treasure->jewelry[i].true_description);
        }
    }
    
    if (treasure->maps_count) {
        fprintf(out, "  Maps: --------------------------------\n");
        for (int i = 0; i < treasure->maps_count; ++i) {
            fprintf(out, "    %2i  %s\n", i + 1, treasure->maps[i].true_description);
        }
    }
    
    if (treasure->magic_items_count) {
        fprintf(out, "  Magic Items: -------------------------\n");
        for (int i = 0; i < treasure->magic_items_count; ++i) {
            fprintf(out, "    %2i  %s\n",
                    i + 1, treasure->magic_items[i].true_description);
            if (treasure->magic_items[i].true_details) {
                int j = 0;
                while (treasure->magic_items[i].true_details[j]) {
                    fprintf(out, "            %s\n",
                            treasure->magic_items[i].true_details[j]);
                    ++j;
                }
            }
        }
    }
}


static void
generate_character(struct rnd *rnd, FILE *out, char const *method_name)
{
    enum characteristic_generation_method method = characteristic_generation_method_simple;
    uint32_t special_characteristics = STRENGTH;
    if (0 == strcasecmp("method1", method_name)) {
        method = characteristic_generation_method_1;
    } else if (0 == strcasecmp("method2", method_name)) {
        method = characteristic_generation_method_2;
    } else if (0 == strcasecmp("method3", method_name)) {
        method = characteristic_generation_method_3;
    } else if (0 == strcasecmp("method4", method_name)) {
        method = characteristic_generation_method_4;
    } else if (0 == strcasecmp("generalnpc", method_name)) {
        method = characteristic_generation_method_general_NPC;
    } else if (0 == strcasecmp("specialnpc", method_name)) {
        method = characteristic_generation_method_special_NPC;
    } else {
        method_name = "simple";
    }
    
    int *characteristics = alloc_characteristics(rnd, method, special_characteristics);
    if (   method == characteristic_generation_method_1
        || method == characteristic_generation_method_2)
    {
        fprintf(out, "Character (%s): -------------------------\n", method_name);
        fprintf(out, "(%s)\n", characteristic_generation_method_description(method));
        for (int i = 0; i < 6; ++i) {
            fprintf(out, "  %2i) %i\n", i + 1, characteristics[i]);
        }
    } else if (method == characteristic_generation_method_4) {
        fprintf(out, "Possible Characters (%s): -------------------------\n", method_name);
        fprintf(out, "(%s)\n", characteristic_generation_method_description(method));
        fprintf(out, "                 %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i\n",
                1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        fprintf(out, "                 %2s   %2s   %2s   %2s   %2s   %2s   %2s   %2s   %2s   %2s   %2s   %2s\n",
                "--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "--", "--");
        fprintf(out, "  Strength:      %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i\n",
                characteristics[0], characteristics[0 + 6], characteristics[0 + 12],
                characteristics[0 + 18], characteristics[0 + 24], characteristics[0 + 30],
                characteristics[0 + 36], characteristics[0 + 42], characteristics[0 + 48],
                characteristics[0 + 54], characteristics[0 + 60], characteristics[0 + 66]);
        fprintf(out, "  Intelligence:  %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i\n",
                characteristics[1], characteristics[1 + 6], characteristics[1 + 12],
                characteristics[1 + 18], characteristics[1 + 24], characteristics[1 + 30],
                characteristics[1 + 36], characteristics[1 + 42], characteristics[1 + 48],
                characteristics[1 + 54], characteristics[1 + 60], characteristics[1 + 66]);
        fprintf(out, "  Wisdom:        %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i\n",
                characteristics[2], characteristics[2 + 6], characteristics[2 + 12],
                characteristics[2 + 18], characteristics[2 + 24], characteristics[2 + 30],
                characteristics[2 + 36], characteristics[2 + 42], characteristics[2 + 48],
                characteristics[2 + 54], characteristics[2 + 60], characteristics[2 + 66]);
        fprintf(out, "  Dexterity:     %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i\n",
                characteristics[3], characteristics[3 + 6], characteristics[3 + 12],
                characteristics[3 + 18], characteristics[3 + 24], characteristics[3 + 30],
                characteristics[3 + 36], characteristics[3 + 42], characteristics[3 + 48],
                characteristics[3 + 54], characteristics[3 + 60], characteristics[3 + 66]);
        fprintf(out, "  Constitution:  %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i\n",
                characteristics[4], characteristics[4 + 6], characteristics[4 + 12],
                characteristics[4 + 18], characteristics[4 + 24], characteristics[4 + 30],
                characteristics[4 + 36], characteristics[4 + 42], characteristics[4 + 48],
                characteristics[4 + 54], characteristics[4 + 60], characteristics[4 + 66]);
        fprintf(out, "  Charisma:      %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i   %2i\n",
                characteristics[5], characteristics[5 + 6], characteristics[5 + 12],
                characteristics[5 + 18], characteristics[5 + 24], characteristics[5 + 30],
                characteristics[5 + 36], characteristics[5 + 42], characteristics[5 + 48],
                characteristics[5 + 54], characteristics[5 + 60], characteristics[5 + 66]);
    } else {
        fprintf(out, "Character (%s): -------------------------\n", method_name);
        fprintf(out, "(%s)\n", characteristic_generation_method_description(method));
        fprintf(out, "  Strength:     %2i\n", characteristics[0]);
        fprintf(out, "  Intelligence: %2i\n", characteristics[1]);
        fprintf(out, "  Wisdom:       %2i\n", characteristics[2]);
        fprintf(out, "  Dexterity:    %2i\n", characteristics[3]);
        fprintf(out, "  Constitution: %2i\n", characteristics[4]);
        fprintf(out, "  Charisma:     %2i\n", characteristics[5]);
    }
    free_or_die(characteristics);
}


static void
generate_each_treasure(struct rnd *rnd, FILE *out)
{
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        generate_treasure_type(rnd, out, letter);
    }
}


static void
generate_map(struct rnd *rnd, FILE *out)
{
    struct treasure_map treasure_map;
    treasure_map_initialize(&treasure_map);
    treasure_map_generate(&treasure_map, rnd);
    fprintf(out, "%s\n", treasure_map.true_description);
    enumerate_treasure_items(&treasure_map.treasure, out);
    treasure_map_finalize(&treasure_map);
}


static void
generate_magic_items(struct rnd *rnd, FILE *out, int count)
{
    struct magic_item magic_item;
    
    fprintf(out, "Magic Items:\n");
    for (int i = 0; i < count; ++i) {
        magic_item_initialize(&magic_item);
        magic_item_generate(&magic_item, rnd, ANY_MAGIC_ITEM);
        fprintf(out, "  %4i %s\n", (i + 1), magic_item.true_description);
        if (magic_item.true_details) {
            int j = 0;
            while (magic_item.true_details[j]) {
                fprintf(out, "           %s\n", magic_item.true_details[j]);
                ++j;
            }
        }
        magic_item_finalize(&magic_item);
    }
    fprintf(out, "\n");
}


static void
generate_random_dungeon(struct rnd *rnd, FILE *out)
{
    struct dungeon *dungeon = dungeon_alloc();
    dungeon_generate(dungeon, rnd);
    print_dungeon(dungeon, out);
    dungeon_free(dungeon);
}


static void
generate_sample_dungeon(struct rnd *rnd, FILE *out)
{
    struct dungeon *dungeon = dungeon_alloc();
    dungeon_generate_small(dungeon);
    print_dungeon(dungeon, out);
    dungeon_free(dungeon);
}


static void
generate_treasure_type(struct rnd *rnd, FILE *out, char letter)
{
    struct treasure treasure;
    int individual_count;
    
    fprintf(out, "Treasure type %c: ", letter);
    
    if (letter >= 'J' && letter <= 'N') {
        individual_count = roll("1d10", rnd);
    } else {
        individual_count = 0;
    }
    
    treasure_initialize(&treasure);
    treasure_type_generate(treasure_type_by_letter(letter), rnd, individual_count, &treasure);
    
    char *description = treasure_alloc_description(&treasure);
    int value_cp = treasure_value_in_cp(&treasure);
    char *value_gp = coins_alloc_gp_cp_description(value_cp);
    fprintf(out, "%s (total %s)\n", description, value_gp);
    free_or_die(value_gp);
    free_or_die(description);
    enumerate_treasure_items(&treasure, out);
    
    treasure_finalize(&treasure);
}


static void
generate_treasure_type_table(FILE *out)
{
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        struct treasure_type *treasureType = treasure_type_by_letter(letter);
        char *description = treasure_type_alloc_description(treasureType, letter == 'A');
        fprintf(out, "%s", description);
        free_or_die(description);
    }
}


int
main(int argc, char *argv[])
{
    FILE *out = stdout;
    struct rnd *rnd = global_rnd;
    
    fprintf(out, "Fiends and Fortune\n");
    
    bool debug = false;
    bool error = false;
    bool help = false;
    bool verbose = false;
    int ch;
    int long_option_index;
    while (-1 != (ch = getopt_long(argc, argv, short_options, long_options, &long_option_index))) {
        switch (ch) {
            case 'd':
                debug = true;
                break;
            case 'h':
                help = true;
                break;
            case 'v':
                verbose = true;
                break;
            default:
                error = true;
                break;
        }
    }
    
    int remaining_arg_count = argc - optind;
    char const *command = NULL;
    if (remaining_arg_count) {
        command = argv[optind];
        ++optind;
        --remaining_arg_count;
    }
    char const *subcommand = NULL;
    if (remaining_arg_count) {
        subcommand = argv[optind];
        ++optind;
        --remaining_arg_count;
    }
    
    if (!command) {
        usage(argc, argv);
    } else if (help) {
        usage(argc, argv);
    } else if (error) {
        usage(argc, argv);
    } else if (0 == strcasecmp(command, "character")) {
        generate_character(rnd, out, subcommand ? subcommand : "simple");
    } else if (0 == strcasecmp(command, "check")) {
        check(out, subcommand ? subcommand : "0");
    } else if (0 == strcasecmp(command, "dungeon")) {
        if (subcommand && 0 == strcasecmp(subcommand, "small")) {
            generate_sample_dungeon(rnd, out);
        } else {
            generate_random_dungeon(rnd, out);
        }
    } else if (0 == strcasecmp(command, "each")) {
        generate_each_treasure(rnd, out);
    } else if (0 == strcasecmp(command, "magic")) {
        int count = subcommand ? (int)strtol(subcommand, NULL, 10) : 10;
        generate_magic_items(rnd, out, count);
    } else if (0 == strcasecmp(command, "map")) {
        generate_map(rnd, out);
    } else if (0 == strcasecmp(command, "table")) {
        generate_treasure_type_table(out);
    } else if (command[0] >= 'A' && command[0] <= 'Z' && command[1] == '\0') {
        generate_treasure_type(rnd, out, argv[1][0]);
    } else if (command[0] >= 'a' && command[0] <= 'z' && command[1] == '\0') {
        char letter = command[0] - 'a' + 'A';
        generate_treasure_type(rnd, out, letter);
    } else {
        usage(argc, argv);
    }
    
    fprintf(out, "\n");
    alloc_count_is_zero_or_die();
    return EXIT_SUCCESS;
}


static void
print_dungeon(struct dungeon *dungeon, FILE *out)
{
    int starting_level = dungeon_starting_level(dungeon);
    int level_count = dungeon_level_count(dungeon);
    for (int i = 0; i < level_count; ++i) {
        if (i > 0) fprintf(out, "\n");
        int level = starting_level + i;
        fprintf(out, "Level %i\n", level);
        fprintf(out, "\n");
        dungeon_print_map_for_level(dungeon, level, out);
        fprintf(out, "\n");
        fprintf(out, "Level %i Areas of Interest:\n", level);
        dungeon_print_areas_for_level(dungeon, level, out);
    }
}


static void
usage(int argc, char *argv[])
{
    fprintf(stderr, "Usage: %s ACTION\n", basename(argv[0]));
    fprintf(stderr, "\n");
    fprintf(stderr, "Available actions:\n");
    fprintf(stderr, "   character [METHOD]  Generate a character where METHOD is\n");
    fprintf(stderr, "                         `method1', `method2', `method3', `method4',\n");
    fprintf(stderr, "                         `generalnpc', `specialnpc' or `simple'\n");
    fprintf(stderr, "                         (default `simple')\n");
    fprintf(stderr, "   check [N]           Run tests where N is the \"constant\"\n");
    fprintf(stderr, "                         random number (default 0)\n");
    fprintf(stderr, "   dungeon [TYPE]      Generate a dungeon where TYPE is\n");
    fprintf(stderr, "                         `random' or `small' (default `random')\n");
    fprintf(stderr, "   each                Generate one of each treasure\n");
    fprintf(stderr, "   magic [COUNT]       Generate COUNT magic items (default 10)\n");
    fprintf(stderr, "   map                 Generate one treasure map\n");
    fprintf(stderr, "   table               Generate the treasure type table\n");
    fprintf(stderr, "   LETTER              Generate the treasure type for LETTER (A-Z)\n");
}
