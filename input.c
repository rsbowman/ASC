#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "simplex.h"
#include "asc.h"
#include "input.h"

void read_asc(Complex *cx, FILE *f) {
  Simplex s;
  size_t line_size = 100;
  int n_read, 
    verts[4] = {-1, -1, -1, -1}, 
    n_verts = 0;
  char *buffer;
  
  buffer = (char *)malloc(line_size + 1);
  while ((n_read = getline(&buffer, &line_size, f)) != -1) {
    n_verts = sscanf(buffer, "%d %d %d %d", &verts[0], &verts[1],
		     &verts[2], &verts[3]);
    if (n_verts == 0)
      break;

    assert(0 < n_verts && n_verts <= 4);
    simplex_init(&s, n_verts, verts[0], verts[1], verts[2], verts[3]);
    complex_add_facet(cx, &s);
  }
  if (buffer)
    free(buffer);
}
