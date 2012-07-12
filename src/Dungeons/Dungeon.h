#ifndef DUNGEONS_DUNGEON_H_INCLUDED
#define DUNGEONS_DUNGEON_H_INCLUDED


#include <stddef.h>
#include <stdio.h>
#include "Tiles.h"


struct Dice;
struct DungeonStatistics;
struct Tile;


struct Dungeon {
  struct Tiles tiles;
};


void finalizeDungeon(struct Dungeon *dungeon);

void gatherDungeonStatistics(struct Dungeon *dungeon, struct DungeonStatistics *statistics);

void generateDungeon(struct Dungeon *dungeon, struct Dice *dice);

void graphDungeonUsingText(struct Dungeon *dungeon, FILE *out);

void initializeDungeon(struct Dungeon *dungeon);


#endif /* #ifndef DUNGEONS_DUNGEON_H_INCLUDED */
