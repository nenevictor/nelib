#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <assert.h>

#define NEDA_STATIC
#define NDEBUG

#define PRINT_TESTS 1
#define MEASURE_TIME 1
#define ASSERT_FAILS 0

#define DEFAULT_CHUNK 128U
#define TAB "  " /* two spaces */

#include "../include/neda.h"
NEDA_HEADER(float)
NEDA_BODY_IMPLEMENTATION(float, DEFAULT_CHUNK)

/* For "basic_sort" testing. */
int compare_revers(const float *_a, const float *_b)
{
  return *_b - *_a;
}

int main(void)
{
  /* "register" keyword speeds-up testing a little, suddenly. */
  register int tests_count = 0;
  register int tests_passed_temp = 0;
  register int tests_passed = 0;
  register unsigned int temp_int;
  register unsigned int index;
  register float temp_float;

#if MEASURE_TIME != 0 && defined(__GNUC__)
  struct timespec tests_begin_time;
  struct timespec tests_end_time;
#endif

  struct neda_float *da = 0;
  struct neda_float *da_copy = 0;

  neda_float__init(&da);
  neda_float__set_default_functions();

  printf("neda library testing:\n");

#if MEASURE_TIME != 0 && defined(__GNUC__)
  clock_gettime(CLOCK_MONOTONIC, &tests_begin_time);
#endif

  (void)neda_float__set_move_function;
  (void)neda_float__set_swap_function;
  (void)neda_float__memset;

  /* neda_float__reserve() test: */
  {
    tests_count++;
    tests_passed_temp = 1;
    temp_int = neda_float__capacity(da);
#if PRINT_TESTS != 0
    printf("\nneda_float__reserve() test:\n");
    printf(
        TAB "Initial capacity:                   %.3u; "
            "Expected: %.3u;\n",
        temp_int,
        0U);
#endif

    tests_passed_temp &= temp_int == 0U;

    /* Trying to reserve half of chunk, for example. */
    neda_float__reserve(da, DEFAULT_CHUNK / 2);
    temp_int = neda_float__capacity(da);
#if PRINT_TESTS != 0
    printf(
        TAB "Reserve capacity (less than chunk): %.3u; "
            "Expected: %.3u;\n",
        temp_int,
        DEFAULT_CHUNK);
#endif

    tests_passed_temp &= temp_int == DEFAULT_CHUNK;

    /* Reserving only a little more space. */
    neda_float__reserve(da, DEFAULT_CHUNK + 1);
    temp_int = neda_float__capacity(da);
#if PRINT_TESTS != 0
    printf(
        TAB "Reserve capacity (more than chunk): %.3u; "
            "Expected: %.3u;\n",
        temp_int,
        DEFAULT_CHUNK * 2);
#endif

    tests_passed_temp &= temp_int == DEFAULT_CHUNK * 2;

    if (tests_passed_temp)
    {
      tests_passed++;
    }
#if ASSERT_FAILS != 0
    assert(tests_passed_temp);
#endif
  }

  /* neda_float__push_back(),
   * neda_float__pop_back(),
   * neda_float__push_front(),
   * neda_float__pop_front(),
   * neda_float__push_back_empty(),
   * neda_float__push_front_empty() test:
   */
  {
    tests_count++;
    tests_passed_temp = 1;
#if PRINT_TESTS != 0
    printf("\nneda_float__push_back(),\n"
           "neda_float__pop_back(),\n"
           "neda_float__push_front(),\n"
           "neda_float__push_back_empty(),\n"
           "neda_float__push_front_empty() test:\n");
#endif

    neda_float__push_front(da, 1.1f);
    neda_float__push_back(da, 2.2f);
    neda_float__push_back(da, 3.3f);

    temp_int = neda_float__size(da);
#if PRINT_TESTS != 0
    printf(TAB "(3 values pushed into array)\n");
    printf(TAB "Array size:  %.3u; "
               "Expected: %.3u;\n",
           temp_int,
           3U);
#endif
    tests_passed_temp &= temp_int == 3U;

    temp_float = neda_float__pop_front(da);
#if PRINT_TESTS != 0
    printf(TAB "Pop front:   %.1f; "
               "Expected: %.1f;\n",
           temp_float,
           1.1f);
#endif
    tests_passed_temp &= temp_float == 1.1f;

    temp_float = neda_float__pop_back(da);
#if PRINT_TESTS != 0
    printf(TAB "Pop back:    %.1f; "
               "Expected: %.1f;\n",
           temp_float,
           3.3f);
#endif
    tests_passed_temp &= temp_float == 3.3f;

    temp_int = neda_float__size(da);
#if PRINT_TESTS != 0
    printf(TAB "Array size:  %.3u; "
               "Expected: %.3u;\n",
           temp_int,
           1U);
#endif
    tests_passed_temp &= temp_int == 1U;

#if PRINT_TESTS != 0
    printf(TAB "(array cleared)\n");
    printf(TAB "(2 empty values pushed to back and front)\n");
#endif

    neda_float__clear(da);
    neda_float__push_front_empty(da);
    neda_float__push_back_empty(da);

    temp_float = neda_float__at(da, 0);
#if PRINT_TESTS != 0
    printf(TAB "Front value: %.1f; "
               "Expected: %.1f;\n",
           temp_float,
           0.0f);
#endif
    tests_passed_temp &= temp_float == 0.0f;

    temp_float = neda_float__at(da, 1);
#if PRINT_TESTS != 0
    printf(TAB "Back value:  %.1f; "
               "Expected: %.1f;\n",
           temp_float,
           0.0f);
#endif
    tests_passed_temp &= temp_float == 0.0f;

    neda_float__clear(da);

    if (tests_passed_temp)
    {
      tests_passed++;
    }

#if ASSERT_FAILS != 0
    assert(tests_passed_temp);
#endif
  }

  /* neda_float__at(),
   * neda_float__at_ptr() test:
   */
  {
    tests_count++;
#if PRINT_TESTS != 0
    printf("\nneda_float__at(),\n"
           "neda_float__at_ptr() test: \n");
#endif

    /* Push some values */
    neda_float__push_back(da, 1.1f);
    neda_float__push_back(da, 2.2f);
    neda_float__push_back(da, 3.3f);
    neda_float__push_back(da, 4.4f);
    neda_float__push_back(da, 5.5f);
#if PRINT_TESTS != 0
    printf(TAB "(5 values pushed into array)\n");
#endif

    temp_float = neda_float__at(da, 2);
#if PRINT_TESTS != 0
    printf(TAB "Center value:                   %.1f; "
               "Expected: %.1f;\n",
           temp_float,
           3.3f);
#endif
    tests_passed_temp &= temp_float == 3.3f;

    *neda_float__at_ptr(da, 2) = 9.9f;
#if PRINT_TESTS != 0
    printf(TAB "(changing center value to "
               "%.1f using \"at_ptr()\")\n",
           9.9f);
#endif

    temp_float = neda_float__at(da, 2);
#if PRINT_TESTS != 0
    printf(TAB "Center value (after changing):  %.1f; "
               "Expected: %.1f;\n",
           temp_float,
           9.9f);
#endif
    tests_passed_temp &= temp_float == 9.9f;

    if (tests_passed_temp)
    {
      tests_passed++;
    }

    neda_float__clear(da);

#if ASSERT_FAILS != 0
    assert(tests_passed_temp);
#endif
  }

  /* neda_float__shrink();
   * neda_float__shrink_to_fit() test;
   */
  {
    tests_count++;
    tests_passed_temp = 1;
#if PRINT_TESTS != 0
    printf("\nneda_float__shrink(),\n"
           "neda_float__shrink_to_fit() test:\n");
#endif

    /* To reserve 2 chunks of memory, you should
     * try to reserve a little less.
     * Because if you try to allocate 2 chunks, system
     * automatically allocate third chunk. (Sounds like
     * a marketing trick, maybe latter I'll change that)
     */
    neda_float__reserve(da, DEFAULT_CHUNK * 2 - 1);
#if PRINT_TESTS != 0
    printf(TAB "(reserved 2 chunks of memory)\n");
#endif

    temp_int = neda_float__capacity(da);
#if PRINT_TESTS != 0
    printf(TAB "Capacity (2 chunks):      %.3u; "
               "Expected: %.3u\n",
           temp_int,
           DEFAULT_CHUNK * 2);
#endif
    tests_passed_temp &= temp_int == DEFAULT_CHUNK * 2;

    neda_float__shrink(da);
    temp_int = neda_float__capacity(da);
#if PRINT_TESTS != 0
    printf(TAB "Capacity (shrink):        %.3u; "
               "Expected: %.3u\n",
           temp_int,
           DEFAULT_CHUNK);
#endif
    tests_passed_temp &= temp_int == DEFAULT_CHUNK;

    neda_float__push_back(da, 1.1f);
    neda_float__shrink_to_fit(da);
    temp_int = neda_float__capacity(da);

#if PRINT_TESTS != 0
    printf(TAB "(push one value to array)\n");
    printf(TAB "Capacity (shrink_to_fit): %.3u; "
               "Expected: %.3u\n",
           temp_int,
           1U);
#endif
    tests_passed_temp &= temp_int == 1U;

    if (tests_passed_temp)
    {
      tests_passed++;
    }

    neda_float__clear(da);
    neda_float__reserve(da, DEFAULT_CHUNK);
#if ASSERT_FAILS != 0
    assert(tests_passed_temp);
#endif
  }

  /* neda_float__insert(),
   * neda_float__erase() test:
   */
  {
    tests_count++;
    tests_passed_temp = 1;
#if PRINT_TESTS != 0
    printf("\nneda_float__insert(),\n"
           "neda_float__erase() test:\n");
#endif

    neda_float__push_back(da, 1.1f);
    neda_float__push_back(da, 2.2f);
    neda_float__push_back(da, 3.3f);
    neda_float__push_back(da, 4.4f);
    neda_float__push_back(da, 5.5f);
#if PRINT_TESTS != 0
    printf(TAB "(5 values pushed into array)\n");
#endif

    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }

#if PRINT_TESTS != 0
    printf(TAB "(insert 9.9 before 3.3 (index 2))\n");
#endif
    neda_float__insert(da, 2, 9.9f);

    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }
    tests_passed_temp &= neda_float__at(da, 2) == 9.9f;

#if PRINT_TESTS != 0
    printf(TAB "(erase 9.9 (index 2))\n");
#endif
    neda_float__erase(da, 2);
    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }

    if (tests_passed_temp)
    {
      tests_passed++;
    }

    neda_float__clear(da);

#if ASSERT_FAILS != 0
    assert(tests_passed_temp);
#endif
  }

  /* neda_float__data(),
   * neda_float__size(),
   * neda_float__capacity() test:
   */
  {
    tests_count++;
    tests_passed_temp = 1;
#if PRINT_TESTS != 0
    printf("\nneda_float__data(),\n"
           "neda_float__size(),\n"
           "neda_float__capacity() test\n");
#endif

    neda_float__push_back(da, 9.9f);
    neda_float__push_back(da, 8.8f);
    neda_float__push_back(da, 7.7f);
    neda_float__shrink(da);
#if PRINT_TESTS != 0
    printf(TAB "(3 values pushed into array)\n");
#endif

    temp_float = neda_float__data(da)[0];
#if PRINT_TESTS != 0
    printf(
        TAB "Data (check first element): %.1f; "
            "Expected: %.1f;\n",
        temp_float,
        9.9f);
#endif
    tests_passed_temp &= temp_float == 9.9f;

    temp_int = neda_float__size(da);
#if PRINT_TESTS != 0
    printf(
        TAB "Size:                       %.3u; "
            "Expected: %.3u;\n",
        temp_int,
        3U);
#endif
    tests_passed_temp &= temp_int == 3U;

    temp_int = neda_float__capacity(da);
#if PRINT_TESTS != 0
    printf(
        TAB "Capacity:                   %.3u; "
            "Expected: %.3u;\n",
        temp_int,
        DEFAULT_CHUNK);
#endif
    tests_passed_temp &= temp_int == DEFAULT_CHUNK;

    if (tests_passed_temp)
    {
      tests_passed++;
    }

    neda_float__clear(da);
#if ASSERT_FAILS != 0
    assert(tests_passed_temp);
#endif
  }

  /* neda_float__fill_size(),
   * neda_float__fill() test:
   */
  {
    tests_count++;
    tests_passed_temp = 1;
#if PRINT_TESTS != 0
    printf("\nneda_float__fill_size(),\n"
           "neda_float__fill() test:\n");
    printf(TAB "(fill array with 10 values)\n");
#endif

    neda_float__fill_size(da, 10, 3.3f);
    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }
    tests_passed_temp &= neda_float__at(da, 0) == 3.3f && neda_float__size(da) == 10;

#if PRINT_TESTS != 0

    printf(TAB "(replace those 10 values)\n");
#endif
    neda_float__fill(da, 9.9f);
    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }
    tests_passed_temp &= neda_float__at(da, 0) == 9.9f;

    if (tests_passed_temp)
    {
      tests_passed++;
    }

    neda_float__clear(da);

#if ASSERT_FAILS != 0
    assert(tests_passed_temp);
#endif
  }

  /* neda_float__copy() test: */
  {
    tests_count++;
    tests_passed_temp = 1;
#if PRINT_TESTS != 0
    printf("\nneda_float__copy() test:\n");
#endif

    neda_float__push_back(da, 1.1f);
    neda_float__push_back(da, 2.2f);
    neda_float__push_back(da, 3.3f);
    neda_float__push_back(da, 4.4f);
    neda_float__push_back(da, 5.5f);
#if PRINT_TESTS != 0
    printf(TAB "(5 values pushed into array)\n");
#endif
    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }

    neda_float__init(&da_copy);
    neda_float__copy(da, da_copy);
#if PRINT_TESTS != 0
    printf(TAB "(copy array)\n");
#endif
    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }
    tests_passed_temp &= neda_float__at(da, 0) == neda_float__at(da_copy, 0) &&
                         neda_float__at(da, 1) == neda_float__at(da_copy, 1) &&
                         neda_float__at(da, 2) == neda_float__at(da_copy, 2) &&
                         neda_float__at(da, 3) == neda_float__at(da_copy, 3) &&
                         neda_float__at(da, 4) == neda_float__at(da_copy, 4);
    neda_float__free(&da_copy);

    if (tests_passed_temp)
    {
      tests_passed++;
    }

    neda_float__clear(da);
#if ASSERT_FAILS != 0
    assert(tests_passed_temp);
#endif
  }

  /*
   * neda_float__sort()
   * neda_float__quick_sort() test:
   */
  {
    tests_count++;
    tests_passed_temp = 1;
#if PRINT_TESTS != 0
    printf("neda_float__sort(),\n"
           "neda_float__quick_sort() test:\n");
#endif

    neda_float__push_back(da, 1.1f);
    neda_float__push_back(da, 5.5f);
    neda_float__push_back(da, 3.3f);
    neda_float__push_back(da, 4.4f);
    neda_float__push_back(da, 2.2f);
#if PRINT_TESTS != 0
    printf(TAB "(5 values pushed into array)\n");
#endif
    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }
    neda_float__sort(da);
#if PRINT_TESTS != 0
    printf(TAB "(sort)\n");
#endif
    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }
    tests_passed_temp &= neda_float__at(da, 0) == 1.1f &&
                         neda_float__at(da, 1) == 2.2f &&
                         neda_float__at(da, 2) == 3.3f &&
                         neda_float__at(da, 3) == 4.4f &&
                         neda_float__at(da, 4) == 5.5f;

    neda_float__set_compare_function(compare_revers);
    neda_float__quick_sort(da);

#if PRINT_TESTS != 0
    printf(TAB "(quick sort with reversed compare function)\n");
#endif
    temp_int = neda_float__size(da);
    index = 0;
    while (index < temp_int)
    {
      temp_float = neda_float__at(da, index);
      (void)(temp_float);
#if PRINT_TESTS != 0
      printf(
          TAB TAB "Index: %u; Value: %.1f\n",
          index,
          temp_float);
#endif
      index++;
    }
    tests_passed_temp &= neda_float__at(da, 0) == 5.5f &&
                         neda_float__at(da, 1) == 4.4f &&
                         neda_float__at(da, 2) == 3.3f &&
                         neda_float__at(da, 3) == 2.2f &&
                         neda_float__at(da, 4) == 1.1f;

    if (tests_passed_temp)
    {
      tests_passed++;
    }

    neda_float__clear(da);
#if ASSERT_FAILS != 0
    assert(tests_passed_temp);
#endif
  }

  neda_float__free(&da);

  printf(
      "\n\nTests count: %u; Tests passed: %u;",
      tests_count,
      tests_passed);
#if MEASURE_TIME != 0 && defined(__GNUC__)
  clock_gettime(CLOCK_MONOTONIC, &tests_end_time);
  printf(" Time: %lus and %.10luns;",
         (unsigned long)(tests_end_time.tv_sec - tests_begin_time.tv_sec),
         (unsigned long)(tests_end_time.tv_nsec - tests_begin_time.tv_nsec));

#endif
  printf("\n");

  return 0;
}