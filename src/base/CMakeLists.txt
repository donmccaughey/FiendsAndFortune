add_library(base STATIC
        alloc_or_die.c
        fail.c
        int.c
        ptr_array.c
        result.c
        rnd.c
        sort.c
        str.c
        )
target_include_directories(base
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.."
        )

add_executable(base_tests
        alloc_or_die_test.c
        base_tests.c
        int_test.c
        ptr_array_test.c
        sort_test.c
        str_test.c
        result_test.c
        rnd_test.c
        )
target_link_libraries(base_tests base)
add_test(base_tests base_tests)
