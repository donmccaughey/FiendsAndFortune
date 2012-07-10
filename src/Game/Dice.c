#include "Dice.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "unexpected.h"


static int compareDieRolls(void const *die1, void const *die2);

static uint32_t nextRandomNumber(struct Dice *dice);

static uint32_t nextRandomNumberInRange(struct Dice *dice, 
                                        uint32_t resultInclusiveMin, 
                                        uint32_t resultInclusiveMax); 

static int rollDieRoll(struct Dice *dice, 
                       struct DieRoll *dieRoll, 
                       int diceRolled[]);


static int compareDieRolls(void const *die1, void const *die2)
{
  return *((int *) die1) - *((int *) die2);
}


void initializeDice(struct Dice *dice)
{
  clock_t clockNow = clock();
  time_t timeNow = time(NULL);
  u_int32_t random = arc4random();
  
  dice->state[0] = (unsigned short)clockNow;
  dice->state[1] = (unsigned short)timeNow;
  dice->state[2] = (unsigned short)random;
  dice->nextRandomNumber = nextRandomNumber;
}


int maxDieRoll(char const *dieRollString)
{
  struct DieRoll dieRoll = parseDieRoll(dieRollString);
  return dieRoll.count * dieRoll.sides;
}


int minDieRoll(char const *dieRollString)
{
  struct DieRoll dieRoll = parseDieRoll(dieRollString);
  return dieRoll.count;
}


static uint32_t nextRandomNumber(struct Dice *dice)
{
  return (uint32_t) jrand48(dice->state);
}


static uint32_t nextRandomNumberInRange(struct Dice *dice, 
                                        uint32_t resultInclusiveMin, 
                                        uint32_t resultInclusiveMax)
{
  assert(resultInclusiveMin < resultInclusiveMax);
  
  uint64_t resultRange = (uint64_t) resultInclusiveMax 
                       - (uint64_t) resultInclusiveMin + (uint64_t) 1;
  assert(resultRange > (uint64_t) 1);
  
  uint64_t const sourceInclusiveMax = UINT32_MAX;
  uint64_t const sourceInclusiveMin = 0;
  uint64_t const sourceRange = sourceInclusiveMax 
                             - sourceInclusiveMin + (uint64_t) 1;
  uint64_t sourceRangeExcluded = sourceRange % resultRange;
  uint32_t sourceMaxIncludedValue = (uint32_t) (sourceInclusiveMax 
                                                - sourceRangeExcluded);
  
  /* discard values outside the range of */
  /* [sourceInclusiveMin, sourceMaxIncludedValue] */
  /* to eliminate modulo bias */
  uint32_t sourceValue;
  do {
    sourceValue = dice->nextRandomNumber(dice);
  } while (sourceValue > sourceMaxIncludedValue);
  
  uint64_t resultValue = (uint64_t) sourceValue % resultRange;
  uint64_t resultOffset = (uint64_t) resultInclusiveMin - sourceInclusiveMin;
  return (uint32_t) (resultValue + resultOffset);
}


struct DieRoll parseDieRoll(char const *dieRollString)
{
  struct DieRoll dieRoll = { 
    .count = 1, 
    .sides = 1, 
    .modifier = 0, 
    .multiplier = 1
  };
  int base = 10;
  char *end;
  errno = 0;
  
  long count = strtol(dieRollString, &end, base);
  assert( ! errno);
  assert(count >= 0 && count <= INT_MAX);
  dieRoll.count = (int) count;
  
  if (end[0] == 'D' || end[0] == 'd') {
    long sides = strtol(end + 1, &end, base);
    assert( ! errno);    
    assert(sides > 1 && sides <= INT_MAX);
    dieRoll.sides = (int) sides;
  } else {
    dieRoll.sides = 1;
  }
  
  if (end[0] == '+' || end[0] == '-') {
    long modifier = strtol(end, &end, base);
    assert( ! errno);
    assert(modifier >= INT_MIN && modifier <= INT_MAX);
    dieRoll.modifier = (int) modifier;
  }
  
  if (end[0] == '*' || end[0] == 'X' || end[0] == 'x') {
    long multiplier = strtol(end + 1, &end, base);
    assert( ! errno);
    assert(multiplier >= INT_MIN && multiplier <= INT_MAX);
    assert(multiplier != 0 && multiplier != 1);
    dieRoll.multiplier = (int) multiplier;
  }
  return dieRoll;
}


int roll(struct Dice *dice, char const *dieRollString)
{
  struct DieRoll dieRoll = parseDieRoll(dieRollString);
  
  return rollDieRoll(dice, &dieRoll, NULL);
}


int rollDice(struct Dice *dice, int count, int sides)
{
  struct DieRoll dieRoll = { count, sides, .modifier = 0, .multiplier = 1 };
  
  return rollDieRoll(dice, &dieRoll, NULL);
}


int rollDiceAndAdjustTowardsAverage(struct Dice *dice, int count, int sides)
{
  struct DieRoll dieRoll = { count, sides, .modifier = 0, .multiplier = 1 };
  int diceRolled[count];
  
  rollDieRoll(dice, &dieRoll, diceRolled);
  
  int total = 0;
  int const highRoll = sides;
  int const lowRoll = 1;
  int const lowPlusHigh = highRoll + lowRoll;
  int const lowAverage = lowPlusHigh / 2;
  int const highAverage = lowAverage + (lowPlusHigh % 2);
  for (int i = 0; i < count; ++i) {
    if (diceRolled[i] == 1) {
      total += lowAverage;
    } else if (diceRolled[i] == sides) {
      total += highAverage;
    } else {
      total += diceRolled[i];
    }
  }
  return total;
}


int rollDiceAndAdjustUpwards(struct Dice *dice, int count, int sides)
{
  struct DieRoll dieRoll = { count, sides, .modifier = 0, .multiplier = 1 };
  int diceRolled[count];
  
  rollDieRoll(dice, &dieRoll, diceRolled);
  
  int total = 0;
  for (int i = 0; i < count; ++i) {
    if (diceRolled[i] < 6) {
      total += diceRolled[i] + 1;
    } else {
      total += diceRolled[i];
    }
  }
  return total;
}


int rollDiceAndDropLowest(struct Dice *dice, int count, int sides)
{
  struct DieRoll dieRoll = { count, sides, .modifier = 0, .multiplier = 1 };
  int diceRolled[count];
  
  rollDieRoll(dice, &dieRoll, diceRolled);
  qsort(diceRolled, (size_t) count, sizeof diceRolled[0], compareDieRolls);
  
  int total = 0;
  for (int i = 1; i < count; ++i) {
    total += diceRolled[i];
  }
  return total;
}


int rollDicePlus(struct Dice *dice, int count, int sides, int modifier)
{
  struct DieRoll dieRoll = { count, sides, modifier, .multiplier = 1 };
  
  return rollDieRoll(dice, &dieRoll, NULL);
}


static int rollDieRoll(struct Dice *dice, 
                       struct DieRoll *dieRoll, 
                       int diceRolled[])
{
  assert(dieRoll->count >= 0);
  assert(dieRoll->sides > 0);
# ifndef NDEBUG
  double maxPossibleRoll = (double) dieRoll->count * (double) dieRoll->sides;
  double maxPossibleTotal = (maxPossibleRoll + (double) dieRoll->modifier) 
                          * (double) dieRoll->multiplier;
  assert(maxPossibleTotal <= (double) INT_MAX);
# endif /* #ifndef NDEBUG */
  
  if (dieRoll->count == 0) {
    return 0;
  } else if (dieRoll->sides == 1) {
    return (dieRoll->count + dieRoll->modifier) * dieRoll->multiplier;
  } else {
    int total = dieRoll->modifier;
    for (int i = 0; i < dieRoll->count; ++i) {
      int result = (int) nextRandomNumberInRange(dice, 1, dieRoll->sides);
      if (diceRolled) {
        diceRolled[i] = result;
      }
      total += result;
    }
    return total * dieRoll->multiplier;
  }
}
