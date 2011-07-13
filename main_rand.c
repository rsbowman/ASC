#include <stdio.h>
#include <stdlib.h>

#include "asc.h"
#include "collapse.h"
#include "input.h"
#include "time.h"
#include "history.h"

int main() {
  int i;
  Complex cx;
  History hist;

  complex_init(&cx);
  history_init(&hist);

  srand(time(0));

  read_asc(&cx, stdin);
  complex_compute_free_faces(&cx);

  printf("checking "); 
  printf("%s", cx.serialized);
  printf("\n");
  complex_print(&cx);
  fflush(stdout);
  
  for (i = 0; i < 100000; ++i) {
    if (!(i % 1000))
      printf("%d\n", i);
    if (complex_collapse_random(&cx, &hist)) {
      printf("collapsed! yay!\n");
      history_print(&hist);
      return 0;
    }
  }
  
  history_destroy(&hist);
  return 0;
}
