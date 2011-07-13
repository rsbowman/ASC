#include <stdlib.h>

#include "common.h"

int random_int(int a, int b) {
  return (rand() % b) + a;
}
