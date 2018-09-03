#include "characteristics_sets.h"

#include <stdlib.h>
#include <base/base.h>
#include <mechanics/mechanics.h>


typedef int
(compare_fn)(void const *, void const *);


struct characteristics_sets *
characteristics_sets_alloc_method_4(struct rnd *rnd)
{
    struct characteristics_sets *sets = calloc_or_die(1, sizeof(struct characteristics_sets));
    size_t count = sizeof sets->values / sizeof sets->values[0];
    for (size_t i = 0; i < count; ++i) {
        sets->values[i].strength = roll("3d6", rnd);
        sets->values[i].intelligence = roll("3d6", rnd);
        sets->values[i].wisdom = roll("3d6", rnd);
        sets->values[i].dexterity = roll("3d6", rnd);
        sets->values[i].constitution = roll("3d6", rnd);
        sets->values[i].charisma = roll("3d6", rnd);
    }
    characteristics_sets_sort(sets);
    return sets;
}


void
characteristics_sets_free(struct characteristics_sets *sets)
{
    free_or_die(sets);
}


void
characteristics_sets_sort(struct characteristics_sets *sets)
{
    size_t size = sizeof sets->values[0];
    size_t count = sizeof sets->values / size;
    compare_fn *compare = (compare_fn *)abilities_compare;
    qsort(&sets->values, count, size, compare);
}
