#include <assert.h>

#include "dice.h"
#include "rnd.h"


void dice_test(void);
static void dice_parse_test(void);
static void rollTest(void);
static void rollDiceAndAdjustTowardsAverageTest(void);
static void rollDiceAndDropLowestTest(void);
static void rollDicePlusTest(void);


void dice_test(void)
{
    dice_parse_test();
    rollTest();
    rollDiceAndAdjustTowardsAverageTest();
    rollDiceAndDropLowestTest();
    rollDicePlusTest();
}


static void dice_parse_test(void)
{
    struct dice dice;
    
    dice = dice_parse("0");
    assert(0 == dice.count);
    assert(1 == dice.sides);
    assert(0 == dice.modifier);
    assert(1 == dice.multiplier);
    
    dice = dice_parse("1");
    assert(1 == dice.count);
    assert(1 == dice.sides);
    assert(0 == dice.modifier);
    assert(1 == dice.multiplier);
    
    dice = dice_parse("5");
    assert(5 == dice.count);
    assert(1 == dice.sides);
    assert(0 == dice.modifier);
    assert(1 == dice.multiplier);
    
    dice = dice_parse("1d6");
    assert(1 == dice.count);
    assert(6 == dice.sides);
    assert(0 == dice.modifier);
    assert(1 == dice.multiplier);
    
    dice = dice_parse("2D6");
    assert(2 == dice.count);
    assert(6 == dice.sides);
    assert(0 == dice.modifier);
    assert(1 == dice.multiplier);
    
    dice = dice_parse("10+1");
    assert(10 == dice.count);
    assert(1 == dice.sides);
    assert(1 == dice.modifier);
    assert(1 == dice.multiplier);
    
    dice = dice_parse("1d6+1");
    assert(1 == dice.count);
    assert(6 == dice.sides);
    assert(1 == dice.modifier);
    assert(1 == dice.multiplier);
    
    dice = dice_parse("2d4-1");
    assert(2 == dice.count);
    assert(4 == dice.sides);
    assert(-1 == dice.modifier);
    assert(1 == dice.multiplier);
    
    dice = dice_parse("5*10");
    assert(5 == dice.count);
    assert(1 == dice.sides);
    assert(0 == dice.modifier);
    assert(10 == dice.multiplier);
    
    dice = dice_parse("1d6*10");
    assert(1 == dice.count);
    assert(6 == dice.sides);
    assert(0 == dice.modifier);
    assert(10 == dice.multiplier);
    
    dice = dice_parse("5d4*-5");
    assert(5 == dice.count);
    assert(4 == dice.sides);
    assert(0 == dice.modifier);
    assert(-5 == dice.multiplier);
    
    dice = dice_parse("2d10+4*100");
    assert(2 == dice.count);
    assert(10 == dice.sides);
    assert(4 == dice.modifier);
    assert(100 == dice.multiplier);
}


static void rollTest(void)
{
    struct rnd *alwaysRollOnes = rnd_alloc_fake_fixed(0);
    struct rnd *alwaysRollTwos = rnd_alloc_fake_fixed(1);
    int dieRoll;
    
    /* "roll" a constant number */
    dieRoll = roll(alwaysRollOnes, "0");
    assert(0 == dieRoll);
    
    dieRoll = roll(alwaysRollOnes, "3");
    assert(3 == dieRoll);
    
    dieRoll = roll(alwaysRollTwos, "1+1");
    assert(2 == dieRoll);
    
    dieRoll = roll(alwaysRollTwos, "5-1");
    assert(4 == dieRoll);
    
    dieRoll = roll(alwaysRollTwos, "5*10");
    assert(50 == dieRoll);
    
    /* roll dice that always roll 1's */
    dieRoll = roll(alwaysRollOnes, "0d4+1");
    assert(0 == dieRoll);
    
    dieRoll = roll(alwaysRollOnes, "1D6+2");
    assert(3 == dieRoll);
    
    dieRoll = roll(alwaysRollOnes, "2d10+1");
    assert(3 == dieRoll);
    
    dieRoll = roll(alwaysRollOnes, "3d4+5");
    assert(8 == dieRoll);
    
    dieRoll = roll(alwaysRollOnes, "1d12*5");
    assert(5 == dieRoll);
    
    dieRoll = roll(alwaysRollOnes, "2d20+1*10");
    assert(30 == dieRoll);
    
    /* roll dice that always roll 2's */
    dieRoll = roll(alwaysRollTwos, "1D6-2");
    assert(0 == dieRoll);
    
    dieRoll = roll(alwaysRollTwos, "2d10-2");
    assert(2 == dieRoll);
    
    dieRoll = roll(alwaysRollTwos, "3d4-3");
    assert(3 == dieRoll);
    
    dieRoll = roll(alwaysRollTwos, "2d8*5");
    assert(20 == dieRoll);
    
    dieRoll = roll(alwaysRollTwos, "2d20+1*10");
    assert(50 == dieRoll);
    
    rnd_free(alwaysRollOnes);
    rnd_free(alwaysRollTwos);
}


static void rollDiceAndAdjustTowardsAverageTest(void)
{
    struct rnd *ascendingRolls = rnd_alloc_fake_ascending(0);
    struct rnd *alwaysRollOnes = rnd_alloc_fake_fixed(0);
    struct rnd *alwaysRollTwos = rnd_alloc_fake_fixed(1);
    int dieRoll;
    
    dieRoll = rollDiceAndAdjustTowardsAverage(ascendingRolls, 3, 6);
    assert(8 == dieRoll);
    
    dieRoll = rollDiceAndAdjustTowardsAverage(alwaysRollOnes, 3, 6);
    assert(9 == dieRoll);
    
    dieRoll = rollDiceAndAdjustTowardsAverage(alwaysRollTwos, 3, 6);
    assert(6 == dieRoll);
    
    rnd_free(ascendingRolls);
    rnd_free(alwaysRollOnes);
    rnd_free(alwaysRollTwos);
}


static void rollDiceAndDropLowestTest(void)
{
    struct rnd *ascendingRolls = rnd_alloc_fake_ascending(0);
    struct rnd *alwaysRollOnes = rnd_alloc_fake_fixed(0);
    struct rnd *alwaysRollTwos = rnd_alloc_fake_fixed(1);
    int dieRoll;
    
    dieRoll = rollDiceAndDropLowest(ascendingRolls, 4, 6);
    assert(9 == dieRoll);
    
    dieRoll = rollDiceAndDropLowest(alwaysRollOnes, 3, 6);
    assert(2 == dieRoll);
    
    dieRoll = rollDiceAndDropLowest(alwaysRollTwos, 5, 4);
    assert(8 == dieRoll);
    
    rnd_free(ascendingRolls);
    rnd_free(alwaysRollOnes);
    rnd_free(alwaysRollTwos);
}


static void rollDicePlusTest(void)
{
    struct rnd *alwaysRollOnes = rnd_alloc_fake_fixed(0);
    struct rnd *alwaysRollTwos = rnd_alloc_fake_fixed(1);
    int dieRoll;
    
    /* "roll" a constant number */
    dieRoll = rollDicePlus(alwaysRollTwos, 1, 1, 1);
    assert(2 == dieRoll);
    
    dieRoll = rollDicePlus(alwaysRollTwos, 5, 1, -1);
    assert(4 == dieRoll);
    
    /* roll dice that always roll 1's */
    dieRoll = rollDicePlus(alwaysRollOnes, 1, 6, 2);
    assert(3 == dieRoll);
    
    dieRoll = rollDicePlus(alwaysRollOnes, 2, 10, 1);
    assert(3 == dieRoll);
    
    dieRoll = rollDicePlus(alwaysRollOnes, 3, 4, 5);
    assert(8 == dieRoll);
    
    /* roll dice that always roll 2's */
    dieRoll = rollDicePlus(alwaysRollTwos, 1, 6, -2);
    assert(0 == dieRoll);
    
    dieRoll = rollDicePlus(alwaysRollTwos, 2, 10, -2);
    assert(2 == dieRoll);
    
    dieRoll = rollDicePlus(alwaysRollTwos, 3, 4, -3);
    assert(3 == dieRoll);
    
    rnd_free(alwaysRollOnes);
    rnd_free(alwaysRollTwos);
}
