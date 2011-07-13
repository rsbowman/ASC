#ifndef SIMPLEX_FF_H
#define SIMPLEX_FF_H

#include "simplex.h"

typedef struct SimplexFFNode SimplexFFNode;
typedef struct SimplexFFList SimplexFFList;

struct SimplexFFNode {
  Simplex simplex;
  SimplexList free_faces;
  SimplexFFNode *next;
};

struct SimplexFFList {
  SimplexFFNode *head;
  SimplexFFNode *end;
  int size;
};

SimplexFFNode *simplex_ff_node_create(Simplex *, 
				      SimplexFFNode *);
void simplex_ff_list_init(SimplexFFList *);
void simplex_ff_list_destroy(SimplexFFList *sl);
int simplex_ff_list_size(SimplexFFList *);
SimplexFFNode * simplex_ff_list_append(SimplexFFList *, Simplex *);
SimplexFFNode *simplex_ff_list_insert(SimplexFFList *l, Simplex *s);
void simplex_ff_list_copy(SimplexFFList *dest, 
			  SimplexFFList *src);

void simplex_ff_list_print(SimplexFFList *);
void simplex_ff_list_print_full(SimplexFFList *l);

#endif
