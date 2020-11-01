#include "options.h"

#include <ctype.h>
#include <getopt.h>
#include <base/base.h>
#include <dungeon/dungeon.h>


static struct option long_options[] = {
    {
        .name="debug",
        .has_arg=no_argument,
        .flag=NULL,
        .val=option_value_debug
    },
    {
        .name="format",
        .has_arg=required_argument,
        .flag=NULL,
        .val=option_value_format
    },
    {
        .name="help",
        .has_arg=no_argument,
        .flag=NULL,
        .val=option_value_help
    },
    {
        .name="jrand48",
        .has_arg=required_argument,
        .flag=NULL,
        .val=option_value_jrand48
    },
    {
        .name="verbose",
        .has_arg=no_argument,
        .flag=NULL,
        .val=option_value_verbose
    },
    {
        .name=NULL,
        .has_arg=no_argument,
        .flag=NULL,
        .val=option_value_none
    }
};

static char const short_options[] = "dhj:v";

static char const *output_formats[] = {
        "text",
        "json",
};
static size_t const output_formats_count = ARRAY_COUNT(output_formats);


static void
get_action_modifiers(struct options *options,
                     int argc,
                     char *argv[],
                     int start_index);

static void
set_action_modifier_defaults(struct options *options, char const *action_string);


static void
get_action(struct options *options, int argc, char *argv[], int start_index)
{
    int i = start_index;
    int remaining_arg_count = argc - i;
    if (!remaining_arg_count) {
        fprintf(stderr, "%s: no action given\n", options->command_name);
        options->error = true;
        return;
    }
    
    char const *action_string = argv[i];
    ++i;
    --remaining_arg_count;
    options->action = action_from_string(action_string);
    if (action_invalid == options->action) {
        fprintf(stderr, "%s: invalid action - %s\n",
                options->command_name, action_string);
        options->error = true;
        return;
    }
    
    set_action_modifier_defaults(options, action_string);
    if (remaining_arg_count) get_action_modifiers(options, argc, argv, i);
}


static void
get_action_modifiers(struct options *options,
                     int argc,
                     char *argv[],
                     int start_index)
{
    int i = start_index;
    int remaining_arg_count = argc - i;
    
    char const *modifier_string = argv[i];
    ++i;
    --remaining_arg_count;
    
    switch (options->action) {
        case action_character:
            options->character_method = ability_score_generation_method_from_string(
                    modifier_string);
            if (ability_score_generation_method_invalid == options->character_method) {
                options->error = true;
                fprintf(stderr, "%s: invalid character method - %s\n",
                        options->command_name, modifier_string);
            }
            break;
        case action_check:
            errno = 0;
            options->check_constant = (uint32_t)strtoul(modifier_string, NULL, 10);
            if (errno) {
                options->error = true;
                fprintf(stderr, "%s: invalid check constant - %s\n",
                        options->command_name, modifier_string);
            }
            break;
        case action_dungeon:
            if (0 == strcasecmp("small", modifier_string)) {
                options->dungeon_type_small = true;
            } else if (0 == strcasecmp("random", modifier_string)) {
                options->dungeon_type_small = false;
            } else {
                options->error = true;
                fprintf(stderr, "%s: invalid dungeon type - %s\n",
                        options->command_name, modifier_string);
            }
            break;
        case action_magic:
            errno = 0;
            options->magic_count = (int)strtol(modifier_string, NULL, 10);
            if (errno) {
                options->error = true;
                fprintf(stderr, "%s: invalid magic count - %s\n",
                        options->command_name, modifier_string);
            }
            break;
        default:
            break;
    }
    
    while (remaining_arg_count > 0) {
        fprintf(stderr, "%s: invalid option (ignored) - %s\n",
                options->command_name, argv[i]);
        ++i;
        --remaining_arg_count;
    }
}


static void
get_format(struct options *options, char const *arg)
{
    for (size_t i = 0; i < output_formats_count; ++i) {
        if (0 == strcasecmp(arg, output_formats[i])) {
            options->output_format = i;
            return;
        }
    }
    options->error = true;
    fprintf(stderr, "%s: invalid format - %s\n", options->command_name, optarg);
}


static void
get_jrand48(struct options *options, char const *arg)
{
    unsigned long long const max_seed = 0x0000ffffffffffff;
    errno = 0;
    unsigned long long long_seed = strtoull(arg, NULL, 0);
    if (errno || long_seed > max_seed) {
        options->error = true;
        fprintf(stderr, "%s: invalid jrand48 seed - %s\n",
                options->command_name, optarg);
        return;
    }
    unsigned short seed[3];
    seed[0] = long_seed & 0x000000000000ffff;
    seed[1] = (long_seed & 0x00000000ffff0000) >> 16;
    seed[2] = (long_seed & 0x0000ffff00000000) >> 32;
    rnd_free(options->rnd);
    options->rnd = rnd_alloc_jrand48(seed);
}


static void
getopt_reset(void)
{
#ifdef HAS_OPTRESET
    optreset = 1;
#endif
    optind = 1;
}


static int
get_options(struct options *options, int argc, char *argv[])
{
    getopt_reset();
    int ch;
    int long_option_index;
    while (-1 != (ch = getopt_long(argc, argv, short_options, long_options, &long_option_index))) {
        switch (ch) {
            case option_value_debug:
                options->debug = true;
                break;
            case option_value_format:
                get_format(options, optarg);
                break;
            case option_value_help:
                options->help = true;
                return optind;
            case option_value_jrand48:
                get_jrand48(options, optarg);
                break;
            case option_value_verbose:
                options->verbose = true;
                break;
            default:
                options->error = true;
                break;
        }
    }
    return optind;
}


struct options *
options_alloc(int argc, char *argv[])
{
    struct options *options = calloc_or_die(1, sizeof(struct options));
    options->command_name = basename_or_die(argv[0]);
    options->rnd = rnd_alloc();
    
    int action_index = get_options(options, argc, argv);
    if (options->error) return options;
    
    get_action(options, argc, argv, action_index);
    return options;
}


void
options_free(struct options *options)
{
    if (options) {
        rnd_free(options->rnd);
        free_or_die(options->command_name);
        dungeon_options_free(options->dungeon_options);
        free_or_die(options);
    }
}


void
options_print_usage(struct options const *options)
{
    FILE *out = stdout;
    if (options->error) out = stderr;
    
    fprintf(out, "Usage: %s [OPTIONS] ACTION\n", options->command_name);
    fprintf(out, "\n");
    fprintf(out, "  -d, --debug         print debugging information\n");
    fprintf(out, "  -h, --help          display this help message and exit\n");
    fprintf(out, "  -j, --jrand48=SEED  use the jrand48 random number generator\n");
    fprintf(out, "                        with the given 48-bit SEED\n");
    fprintf(out, "  ---format=FORMAT    output format where FORMAT is\n");
    fprintf(out, "                        `text' or `json' (default `text'\n");
    fprintf(out, "  -v, --verbose       print more details\n");
    fprintf(out, "\n");
    fprintf(out, "Available actions:\n");
    fprintf(out, "  character [METHOD]  generate a character where METHOD is\n");
    fprintf(out, "                        `method1', `method2', `method3', `method4',\n");
    fprintf(out, "                        `generalnpc', `specialnpc' or `simple'\n");
    fprintf(out, "                        (default `simple')\n");
    fprintf(out, "  check [N]           run tests where N is the \"constant\"\n");
    fprintf(out, "                        random number (default 0)\n");
    fprintf(out, "  dungeon [TYPE]      generate a dungeon where TYPE is\n");
    fprintf(out, "                        `random' or `small' (default `random')\n");
    fprintf(out, "  each                generate one of each treasure\n");
    fprintf(out, "  magic [COUNT]       generate COUNT magic items (default 10)\n");
    fprintf(out, "  map                 generate one treasure map\n");
    fprintf(out, "  table               generate the treasure type table\n");
    fprintf(out, "  LETTER              generate the treasure type for LETTER (A-Z)\n");
}


static void
set_action_modifier_defaults(struct options *options, char const *action_string)
{
    switch (options->action) {
        case action_character:
            options->character_method = ability_score_generation_method_simple;
            break;
        case action_check:
            options->check_constant = 0;
            break;
        case action_dungeon:
            options->dungeon_type_small = false;
            options->dungeon_options = dungeon_options_alloc_default();
            options->dungeon_options->padding = rnd_next_uniform_value(options->rnd, 2);
            break;
        case action_magic:
            options->magic_count = 10;
            break;
        case action_treasure:
            options->treasure_type = toupper(action_string[0]);
            break;
        default:
            break;
    }
}
