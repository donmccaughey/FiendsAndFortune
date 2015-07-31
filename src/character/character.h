#ifndef FNF_CHARACTER_CHARACTER_H_INCLUDED
#define FNF_CHARACTER_CHARACTER_H_INCLUDED


#include <stddef.h>
#include <stdint.h>


#define STRENGTH      ((uint32_t) 0x0001u)
#define INTELLIGENCE  ((uint32_t) 0x0002u)
#define WISDOM        ((uint32_t) 0x0004u)
#define DEXTERITY     ((uint32_t) 0x0008u)
#define CONSTITUTION  ((uint32_t) 0x0010u)
#define CHARISMA      ((uint32_t) 0x0020u)
#define NO_SPECIAL_CHARACTERISTICS 0


struct rnd;


enum characteristic_generation_method {
    characteristic_generation_method_simple,
    characteristic_generation_method_1,
    characteristic_generation_method_2,
    characteristic_generation_method_3,
    characteristic_generation_method_4,
    characteristic_generation_method_general_NPC,
    characteristic_generation_method_special_NPC
};


struct character {
    int strength;
    int intelligence;
    int wisdom;
    int dexterity;
    int constitution;
    int charisma;
};


char const *
characteristic_generation_method_description(enum characteristic_generation_method method);

void
character_finalize(struct character *character);

void
character_initialize(struct character *character);

int *
alloc_characteristics(struct rnd *rnd,
                      enum characteristic_generation_method method,
                      uint32_t characteristic_flags);


#endif