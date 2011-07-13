#ifndef SIMPLEX_H
#define SIMPLEX_H

#include "common.h"

#define NVERTICES 4

typedef struct Simplex {
  int size;
  int vertices[NVERTICES];
} Simplex;

typedef struct SimplexNode {
  Simplex simplex;
  struct SimplexNode *next;
} SimplexNode;

typedef struct SimplexList {
  SimplexNode *head;
  SimplexNode *end;
  int size;
} SimplexList;

void simplex_init(Simplex *, int size, int, int, int, int);
int simplex_size(Simplex *);
void simplex_copy(Simplex *dest, Simplex *src);
bool simplex_is_sorted(Simplex *s);
bool simplex_is_equal(Simplex *, Simplex *);
int simplex_compare(Simplex *, Simplex *);

bool simplex_is_subface(Simplex *sf, Simplex *f);
bool simplex_is_facet(Simplex *facet, Simplex *s);
bool simplex_is_facet(Simplex *sf, Simplex *f);
void simplex_subfaces(Simplex *s, SimplexList *);
bool simplex_contains_vertex(Simplex *s, int vertex);
int simplex_common_subface_size(Simplex *s1, Simplex *s2);
void simplex_print(Simplex *s);
void simplex_print_letters(Simplex *s);

SimplexNode *simplex_node_create(Simplex *s, SimplexNode *next);
void simplex_list_init(SimplexList *l);
void simplex_list_destroy(SimplexList *);
void simplex_list_copy(SimplexList *dest, SimplexList *src);
SimplexNode *simplex_list_append(SimplexList *l, Simplex *s);
bool simplex_list_remove(SimplexList *l, Simplex *facet);
void simplex_list_print(SimplexList *l);
bool simplex_list_contains(SimplexList *l, Simplex *s);
int simplex_list_size(SimplexList *l);

hashtype simplex_hash(Simplex *s);
#endif
