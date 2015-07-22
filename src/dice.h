#ifndef DICE_H_INCLUDED
#define DICE_H_INCLUDED


#include <stddef.h>


struct rnd;


struct dice {
    int count;
    int sides;
    int modifier;
    int multiplier;
};


inline struct dice
dice_make(int count, int sides)
{
    return (struct dice){
        .count=count,
        .sides=sides,
        .modifier=0,
        .multiplier=1,
    };
}

int
dice_max_score(char const *dice_string);

int
dice_min_score(char const *dice_string);

struct dice
dice_parse(char const *dice_string);

int
dice_roll(struct dice dice, struct rnd *rnd, int die_scores[]);

int
dice_roll_with_average_scoring(struct dice dice, struct rnd *rnd);

int
dice_roll_and_adjust_upwards(struct dice dice, struct rnd *rnd);

int
dice_roll_and_drop_lowest(struct dice dice, struct rnd *rnd);

inline int
roll(char const *dice_string, struct rnd *rnd)
{
    return dice_roll(dice_parse(dice_string), rnd, NULL);
}


#endif /* #ifndef DICE_H_INCLUDED */
