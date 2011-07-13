#ifndef ASC_H
#define ASC_H

#include "common.h"
#include "simplex.h"
#include "simplex_ff.h"

typedef struct Complex Complex;
typedef struct ComplexNode ComplexNode;
typedef struct ComplexStack ComplexStack;

#define MAX_SERIALIZED_SIZE 300

struct Complex {
  SimplexFFList facets;
  bool free_faces_computed;
  int n_collapses; /* number of collapses to arrive at this complex*/
  char serialized[MAX_SERIALIZED_SIZE];
};

struct ComplexNode {
  Complex complex;
  ComplexNode *next;
};

struct ComplexStack {
  ComplexNode *head;
  int size;
};

void complex_init(Complex *);
void complex_add_facet(Complex *c, Simplex *facet);
bool complex_remove_facet(Complex *c, Simplex *facet);
void complex_destroy(Complex *c);
void complex_copy(Complex *dest, Complex *src);

int complex_size(Complex *c);
bool complex_contains_facet(Complex *c, Simplex *);
bool complex_is_free_face(Complex *c, Simplex *face);
bool complex_has_free_faces(Complex *c);
void complex_free_faces_for_facet(Complex *c, Simplex *facet,
					  SimplexList *free_faces);
void complex_compute_free_faces_for(Complex *c, Simplex *facet,
				    SimplexList *free_faces);

void complex_collapsible_facets(Complex *c, Simplex *s, 
				SimplexList *subfaces);


void complex_compute_free_faces(Complex *c);
void complex_recompute_free_faces(Complex *c, Simplex *, SimplexList *);

hashtype complex_hash(Complex *c);
bool complex_equals(Complex *c1, Complex *c2);
bool complex_is_subcomplex(Complex *c1, Complex *c2);

void complex_serialize(Complex *c, char *buf, int max_size);
void complex_print(Complex *c);

void complex_stack_init(ComplexStack *l);
ComplexNode *complex_node_create(Complex *c, ComplexNode *next);
void complex_stack_push(ComplexStack *l, Complex *c);
void complex_stack_destroy(ComplexStack *l);
bool complex_stack_pop(ComplexStack *l, Complex *c);
int complex_stack_size(ComplexStack *l);
void complex_stack_print(ComplexStack *l);

#endif
