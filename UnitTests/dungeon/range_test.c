#include "range.h"

#include <assert.h>
#include <limits.h>


void range_test(void);


static void
range_is_equal_test(void)
{
    assert(range_is_equal(range_make(0, 0), range_make(0, 0)));
    assert(range_is_equal(range_make(42, 42), range_make(0, 0)));
    
    assert(range_is_equal(range_make(-10, 10), range_make(-10, 10)));
    assert(range_is_equal(range_make(-10, -1), range_make(-10, -1)));
    assert(range_is_equal(range_make(1, 10), range_make(1, 10)));
    
    assert(!range_is_equal(range_make(-10, 10), range_make(-10, 11)));
    assert(!range_is_equal(range_make(-10, -1), range_make(-11, -1)));
    assert(!range_is_equal(range_make(1, 10), range_make(1, 11)));
}


static void
range_expand_test(void)
{
    struct range range = range_make(0, 0);
    
    range = range_expand(range, 1);
    
    assert(range_is_equal(range_make(-1, 1), range));
    
    range = range_expand(range, 2);
    
    assert(range_is_equal(range_make(-3, 3), range));
    
    range = range_expand(range, -3);
    
    assert(range_is_empty(range));
}


static void
range_extend_to_include_value_test(void)
{
    struct range range = range_make(0, 0);
    
    range = range_extend_to_include_value(range, 1);
    
    assert(range_is_equal(range_make(1, 2), range));
    
    range = range_extend_to_include_value(range, -1);
    
    assert(range_is_equal(range_make(-1, 2), range));
    
    range = range_extend_to_include_value(range, 42);
    
    assert(range_is_equal(range_make(-1, 43), range));
}


static void
range_length_test(void)
{
    assert(0 == range_length(range_make(0, 0)));
    assert(0 == range_length(range_make(42, 42)));
    
    assert(1 == range_length(range_make(0, 1)));
    assert(1 == range_length(range_make(-2, -1)));
    assert(2 == range_length(range_make(10, 12)));
    assert(2 == range_length(range_make(-1, 1)));
    
    assert(INT_MAX == range_length(range_make(INT_MIN, -1)));
    assert(INT_MAX == range_length(range_make(INT_MIN + 1, 0)));
    assert(INT_MAX == range_length(range_make(INT_MIN + 2, 1)));
    
    assert(INT_MAX == range_length(range_make(0, INT_MAX)));
    assert(INT_MAX == range_length(range_make(-1, INT_MAX - 1)));
}


void range_test(void)
{
    range_is_equal_test();
    range_expand_test();
    range_extend_to_include_value_test();
    range_length_test();
}
