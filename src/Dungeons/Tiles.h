#ifndef DUNGEONS_TILES_H_INCLUDED
#define DUNGEONS_TILES_H_INCLUDED


#include <stddef.h>
#include <stdint.h>
#include "Boolean.h"


struct Tile;
struct Tiles;
struct TileStatistics;


/// The Tiles struct takes ownership of the given tile.
void addTileToTiles(struct Tiles *tiles, struct Tile *tile);

struct Tiles *createTiles(void);

struct Tiles *createTilesOnLevel(struct Tiles *tiles, int32_t z);

void destroyTiles(struct Tiles *tiles);

struct Tile *findTileInTilesAt(struct Tiles const *tiles, int32_t x, int32_t y, int32_t z);

void gatherTileStatistics(struct Tiles const *tiles, struct TileStatistics *statistics);

/// The caller is responsible for destroying the tile.
Boolean removeTileFromTiles(struct Tiles *tiles, struct Tile const *tile);

struct Tile *tileInTilesAtIndex(struct Tiles const *tiles, size_t index);

size_t tilesCount(struct Tiles const *tiles);


#endif /* #ifndef DUNGEONS_TILES_H_INCLUDED */
