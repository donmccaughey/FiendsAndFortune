#include "Tiles.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "Tile.h"
#include "TileStatistics.h"


static void appendTileToTiles(struct Tiles *tiles, struct Tile const *tile);
static int compareTilesByCoordinate(void const *item1, void const *item2);
static void gatherStatistics(struct Tile const *tile, struct TileStatistics *statistics);


void addTileToTiles(struct Tiles *tiles, struct Tile const *tile)
{
  appendTileToTiles(tiles, tile);
  qsort(tiles->tiles, tiles->count, sizeof(struct Tile), compareTilesByCoordinate);
}


static void appendTileToTiles(struct Tiles *tiles, struct Tile const *tile)
{
  if (tiles->capacity == tiles->count) {
    if (tiles->capacity) {
      tiles->capacity *= 2;
    } else {
      tiles->capacity = 256;
    }
    tiles->tiles = REALLOC_OR_DIE(tiles->tiles, tiles->capacity * sizeof(struct Tile));
  }
  tiles->tiles[tiles->count] = *tile;
  ++tiles->count;
}


static int compareTilesByCoordinate(void const *item1, void const *item2)
{
  struct Tile const *tile1 = item1;
  struct Tile const *tile2 = item2;

  return comparePoints(&tile1->point, &tile2->point);
}


void finalizeTiles(struct Tiles *tiles)
{
  free(tiles->tiles);
}


struct Tile *findTileInTilesAt(struct Tiles const *tiles, int32_t x, int32_t y, int32_t z)
{
  struct Tile tile = { .point = { x, y, z} };

  struct Tile *tileInTiles = bsearch(&tile, tiles->tiles, tiles->count, sizeof(struct Tile), compareTilesByCoordinate);
  return tileInTiles ? tileInTiles : NULL;
}


static void gatherStatistics(struct Tile const *tile, struct TileStatistics *statistics)
{
  if (tile->point.x < statistics->minX) {
    statistics->minX = tile->point.x;
  }
  if (tile->point.x > statistics->maxX) {
    statistics->maxX = tile->point.x;
  }
  if (tile->point.y < statistics->minY) {
    statistics->minY = tile->point.y;
  }
  if (tile->point.y > statistics->maxY) {
    statistics->maxY = tile->point.y;
  }

  ++statistics->count;
}


void gatherTileStatistics(struct Tiles const *tiles, struct TileStatistics *statistics)
{
  statistics->count = 0;
  statistics->maxX = INT32_MIN;
  statistics->maxY = INT32_MIN;
  statistics->minX = INT32_MAX;
  statistics->minY = INT32_MAX;
  for (size_t i = 0; i < tiles->count; ++i) {
    gatherStatistics(&tiles->tiles[i], statistics);
  }
}


void initializeTiles(struct Tiles *tiles)
{
  memset(tiles, 0, sizeof(struct Tiles));
  tiles->tiles = CALLOC_OR_DIE(0, sizeof(struct Tile));
}


Boolean removeTileFromTiles(struct Tiles *tiles, struct Tile const *tile)
{
  struct Tile *found = bsearch(tile, tiles->tiles, tiles->count, sizeof(struct Tile), compareTilesByCoordinate);
  if ( ! found) {
    return FALSE;
  }
  struct Tile *tail = found + 1;
  struct Tile *end = tiles->tiles + tiles->count;
  memmove(found, tail, (end - tail) * sizeof(struct Tile));
  --tiles->count;
  return TRUE;
}


struct Tiles const tilesOnLevel(struct Tiles const *tiles, int32_t z)
{
  struct Tiles tilesOnLevel = { NULL, 0, 0 };

  // TODO: replace linear search with binary lower bound/upper bound search
  for (size_t i = 0; i < tiles->count; ++i) {
    struct Tile *tile = &tiles->tiles[i];
    if (tile->point.z == z) {
      if ( ! tilesOnLevel.tiles) {
        tilesOnLevel.tiles = tile;
      }
      ++tilesOnLevel.count;
    } else if (tilesOnLevel.tiles) {
      return tilesOnLevel;
    }
  }

  return tilesOnLevel;
}
