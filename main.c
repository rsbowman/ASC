#include <stdio.h>

#include "asc.h"
#include "collapse.h"
#include "input.h"

int main() {
  Complex cx;
  complex_init(&cx);

  read_asc(&cx, stdin);
  complex_compute_free_faces(&cx);

  printf("checking "); 
  printf("%s", cx.serialized);
  //complex_print(&cx);
  printf("\n");

  if (complex_is_collapsible(&cx, True))
    printf("is collapsible!\n");
  else
    printf("is NOT collapsible! uh-oh!\n");
    
  return 0;
}
