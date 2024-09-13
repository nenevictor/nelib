#include <stdio.h>
#include <search.h>
#include "../include/neda.h"

NEDA_HEADER(int)
NEDA_BODY_IMPLEMENTATION(int, 128)

int main(void)
{
  struct neda_int *da;
  int i = 0;

  neda_int__init(&da);
  neda_int__set_default_functions();
  neda_int__reserve(da, 1 << 24);

  while (i < (1 << 24))
  {
    neda_int__push_back(da, rand());
    i++;
  }

  printf(
      "Elements count: %i\n",
      neda_int__size(da));

  neda_int__quick_sort(da);

  i = 0;
  while (i < 256)
  {
    printf(
        "%i: %i\n",
        i,
        neda_int__at(da, i));

    i++;
  }

  neda_int__free(&da);
  return 0;
}