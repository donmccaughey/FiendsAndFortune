#ifndef DUNGEONS_TILES_H_INCLUDED
#define DUNGEONS_TILES_H_INCLUDED


#include <stddef.h>
#include <stdint.h>
#include "Boolean.h"


struct Tile;
struct TileStatistics;


struct Tiles {
  size_t capacity;
  size_t count;
  struct Tile *tiles;
};


void addTileToTiles(struct Tiles *tiles, struct Tile const *tile);

void finalizeTiles(struct Tiles *tiles);

struct Tile *findTileInTilesAt(struct Tiles const *tiles, int32_t x, int32_t y, int32_t z);

void gatherTileStatistics(struct Tiles const *tiles, struct TileStatistics *statistics);

void initializeTiles(struct Tiles *tiles);

Boolean removeTileFromTiles(struct Tiles *tiles, struct Tile const *tile);

struct Tiles const tilesOnLevel(struct Tiles const *tiles, int32_t z);


#endif /* #ifndef DUNGEONS_TILES_H_INCLUDED */
