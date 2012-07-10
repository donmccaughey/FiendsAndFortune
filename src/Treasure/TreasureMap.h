#ifndef TREASURE_MAP_H_INCLUDED
#define TREASURE_MAP_H_INCLUDED


#include "Boolean.h"
#include "Treasure.h"


struct Dice;


struct TreasureMap {
  Boolean isFalse;
  struct Treasure treasure;
  char *trueDescription;
};


void finalizeTreasureMap(struct TreasureMap *treasureMap);

void generateTreasureMap(struct TreasureMap *treasureMap, struct Dice *dice);

void initializeTreasureMap(struct TreasureMap *treasureMap);


#endif /* #ifndef TREASURE_MAP_H_INCLUDED */
