#ifndef DUNGEONS_RANGE_H_INCLUDED
#define DUNGEONS_RANGE_H_INCLUDED


#include <stdbool.h>
#include <stdint.h>


struct Range {
  int32_t begin;
  int32_t end;
};

struct ReverseRange {
  int32_t top;
  int32_t bottom;
};


bool equalRanges(struct Range range1, struct Range range2);

bool equalReverseRanges(struct ReverseRange reverseRange1, struct ReverseRange reverseRange2);

struct Range expandRange(struct Range range, int32_t amount);

struct ReverseRange expandReverseRange(struct ReverseRange reverseRange, int32_t amount);

struct Range extendRangeToIncludeValue(struct Range range, int32_t value);

struct ReverseRange extendReverseRangeToIncludeValue(struct ReverseRange reverseRange, int32_t value);

bool isEmptyRange(struct Range range);

bool isEmptyReverseRange(struct ReverseRange reverseRange);

uint32_t lengthOfRange(struct Range range);

uint32_t lengthOfReverseRange(struct ReverseRange reverseRange);

struct Range makeRange(int32_t begin, int32_t end);

struct Range makeRangeFromReverseRange(struct ReverseRange reverseRange);

struct ReverseRange makeReverseRange(int32_t top, int32_t bottom);

struct ReverseRange makeReverseRangeFromRange(struct Range range);


#endif /* #ifndef DUNGEONS_RANGE_H_INCLUDED */
