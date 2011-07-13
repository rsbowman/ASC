#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "asc.h"
#include "ht.h"

void complex_init(Complex *c) {
  simplex_ff_list_init(&c->facets);
  c->free_faces_computed = False;
  c->n_collapses = 0;
}

void complex_copy(Complex *dest, Complex *src) {
  simplex_ff_list_copy(&dest->facets, &src->facets);
  strcpy(dest->serialized, src->serialized);
  dest->free_faces_computed = True;
  dest->n_collapses = src->n_collapses;
}

void complex_add_facet(Complex *c, Simplex *facet) {
  assert(simplex_is_sorted(facet));
  assert(!complex_contains_facet(c, facet) && 
	 "complex already contains this facet");
  simplex_ff_list_insert(&c->facets, facet);
  c->free_faces_computed = False;
}

void complex_destroy(Complex *c) {
  simplex_ff_list_destroy(&c->facets);
}

int complex_size(Complex *c) {
  return simplex_ff_list_size(&c->facets);
}

bool complex_remove_facet(Complex *c, Simplex *facet) {
  SimplexFFNode *current, *prev;
  
  prev = c->facets.head;
  for (current = c->facets.head; current != NULL; current = current->next) {
    if (simplex_is_equal(&current->simplex, facet)) {
      if (current == prev) { // 1st element of list
	c->facets.head = current->next;
      } else { // current != prev so not 1st element
	if (current == c->facets.end) {
	  prev->next = NULL;
	  c->facets.end = prev;
	} else {
	  prev->next = current->next;
	}
      }
      simplex_list_destroy(&current->free_faces);
      free(current);
      c->facets.size--;
      return True;
    }
    prev = current;
  }
  return False;
}

bool complex_contains_facet(Complex *c, Simplex *facet) {
  SimplexFFNode *sn;
  for (sn = c->facets.head; sn != NULL; sn = sn->next) {
    if (simplex_is_equal(&sn->simplex, facet))
      return True;
  }
  return False;
}

bool complex_is_free_face(Complex *c, Simplex *face) {
  SimplexFFNode *current;
  int n = 0,
    fsize = simplex_size(face);

  for (current = c->facets.head; current != NULL; current = current->next) {
    if ((fsize < simplex_size(&current->simplex) - 1) &&
	simplex_is_subface(face, &current->simplex)) {
      return False;
    } else if (simplex_is_facet(face, &current->simplex)) {
      n++;
    }
    if (n > 1) {
      return False;
    }
  }
  return n == 1;
}

bool complex_has_free_faces(Complex *c) {
  SimplexFFNode *facet;

  assert(c->free_faces_computed);

  for (facet = c->facets.head; facet != NULL; facet = facet->next) {
    if (simplex_list_size(&facet->free_faces) > 0)
      return True;
  }

  return False;

  /*SimplexList sxs;
  SimplexFFNode *sn;

  simplex_list_init(&sxs);
  for (sn = c->facets.head; sn != NULL; sn = sn->next) {
    complex_free_faces_for_facet(c, &sn->simplex, &sxs);
    if (simplex_list_size(&sxs) > 0) {
      simplex_list_destroy(&sxs);
      return True;
    } 
  }
  // no need to destroy simplex list b/c no simplices were added
  return False; */
}

void complex_free_faces_for_facet(Complex *c, Simplex *facet,
				  SimplexList *free_faces) {
  SimplexFFNode *cur_facet;
  assert(c->free_faces_computed);

  for (cur_facet = c->facets.head; cur_facet != NULL;
       cur_facet = cur_facet->next) {
    if (simplex_is_equal(&cur_facet->simplex, facet)) {
      simplex_list_copy(free_faces, &cur_facet->free_faces);
      return;
    }
  }

  // shouldn't get here
  assert(False);
}

void complex_compute_free_faces_for(Complex *c, Simplex *facet,
				    SimplexList *free_faces) {
  SimplexList faces;
  SimplexNode *current;

  simplex_list_init(&faces);
  simplex_subfaces(facet, &faces);
  current = faces.head;

  while (current) {
    if (complex_is_free_face(c, &current->simplex))
      simplex_list_append(free_faces, &current->simplex);
    current = current->next;
  }

  simplex_list_destroy(&faces);
}

void complex_compute_free_faces(Complex *c) {
  SimplexFFNode *facet;

  for (facet = c->facets.head; facet != NULL; facet = facet->next) {
    simplex_list_destroy(&facet->free_faces);
    complex_compute_free_faces_for(c, &facet->simplex, &facet->free_faces);
  }

  /* serialize */
  complex_serialize(c, c->serialized, MAX_SERIALIZED_SIZE);

  c->free_faces_computed = True;
}

void complex_recompute_free_faces(Complex *c, Simplex *removed,
				  SimplexList *added) {
  SimplexFFNode *facet;

  for (facet = c->facets.head; facet != NULL; facet = facet->next) {
    if ((simplex_common_subface_size(&facet->simplex, removed) ==
	 removed->size - 1) ||
	simplex_list_contains(added, &facet->simplex)) {
      simplex_list_destroy(&facet->free_faces);
      complex_compute_free_faces_for(c, &facet->simplex, &facet->free_faces);
    }
  }
  complex_serialize(c, c->serialized, MAX_SERIALIZED_SIZE);
  c->free_faces_computed = True;
}

void complex_collapsible_facets(Complex *c, Simplex *s,
				SimplexList *subfaces) {
  SimplexFFNode *sn;
  SimplexList ff;

  simplex_list_init(&ff);
  for (sn = c->facets.head; sn != NULL; sn = sn->next) {
    if (simplex_is_subface(&sn->simplex, s)) {
      complex_free_faces_for_facet(c, &sn->simplex, &ff);
      if (simplex_list_size(&ff) > 0)
	simplex_list_append(subfaces, &sn->simplex);
      simplex_list_destroy(&ff);
    }
  }
}

void complex_serialize(Complex *c, char *buf, int max_size) {
  int i, j=0;
  SimplexFFNode *sn;
  Simplex *sx;

  for (sn = c->facets.head; sn != NULL; sn = sn->next) {
    sx = &sn->simplex;
    for (i = 0; i < sx->size; ++i) {
      buf[j++] = INT_TO_LETTER(sx->vertices[i]);
      assert(j < max_size);
    }
    if (sn->next != NULL) {
      buf[j++] = ',';
      assert(j  < max_size);
    }
  }
  while (j < max_size)
    buf[j++] = 0;

  /* there'd better be at least one null char to avoid
     bad stuff when we strcpy */
  assert(buf[MAX_SERIALIZED_SIZE - 1] == 0);
}

void complex_print(Complex *c) {
  printf("ASC ");
  simplex_ff_list_print(&c->facets);
}

hashtype complex_hash(Complex *c) {
  hashtype h = 5831;
  int i;
  char *buf = c->serialized;

  assert(c->free_faces_computed);

  for (i = 0; i < MAX_SERIALIZED_SIZE && buf[i] != 0; ++i)
    h = ((h << 5) + h) + buf[i];

  return h;
  /*
  SimplexFFNode *sn;

  for (sn = c->facets.head; sn != NULL; sn = sn->next)
    h = ((h << 5) + h) + simplex_hash(&sn->simplex);

    return h;*/
}

/*
 * c2 contains a subcomplex EXACTLY THE SAME (same vertex names, etc) as c1,
 * no homeomorphism or anything (see below)
 */
bool complex_is_subcomplex(Complex *c1, Complex *c2) {
  SimplexFFNode *sn;
  for (sn = c1->facets.head; sn != NULL; sn = sn->next) {
    if (!complex_contains_facet(c2, &sn->simplex))
      return False;
  }
  return True;
}

/*
 * return true if two complexes are EXACTLY EQUAL
 *   (NO homeomorphism crap or anything like that)
 */
bool complex_equals(Complex *c1, Complex *c2) {  
  assert(c1->free_faces_computed && c2->free_faces_computed);
  return strcmp(c1->serialized, c2->serialized) == 0;
}

//////////////////////////////////////////////////////////
// ComplexList

void complex_stack_init(ComplexStack *l) {
  assert(l);
  l->head = NULL;
  l->size = 0;
}

/*
 * create a ComplexNode and COPY all info so that c can be
 * destroyed after the call (no sharing, etc. etc.)
 */
ComplexNode *complex_node_create(Complex *c, ComplexNode *next) {
  ComplexNode *new_node = (ComplexNode *)malloc(sizeof(ComplexNode));
  complex_init(&new_node->complex);
  complex_copy(&new_node->complex, c);
  new_node->next = next;
  return new_node;
}

void complex_node_destroy(ComplexNode *cn) {
  complex_destroy(&cn->complex);
  free(cn);
}

void complex_stack_push(ComplexStack *l, Complex *c) {
  ComplexNode *new_node;

  new_node = complex_node_create(c, 0);
  new_node->next = l->head;
  l->head = new_node;
  l->size++;
}

bool complex_stack_pop(ComplexStack *l, Complex *c) {
  ComplexNode *cn;

  if (l->size == 0)
    return False;

  complex_copy(c, &l->head->complex);
  cn = l->head->next;
  complex_node_destroy(l->head);
  l->head = cn;
  l->size--;
  return True;
}

void complex_stack_destroy(ComplexStack *l) {
  ComplexNode *current, *tofree;
  current = l->head;

  while (current) {
    tofree = current;
    current = current->next;
    //printf("destroying.. "); complex_print(&tofree->complex);fflush(stdout);
    complex_destroy(&tofree->complex);
    //printf("freeing.. ");  fflush(stdout);
    free(tofree);
    //printf("ok\n"); fflush(stdout);
  }

  l->head = NULL;
  l->size = 0;
}

int complex_stack_size(ComplexStack *l) {
  return l->size;
}

void complex_stack_print(ComplexStack *l) {
  ComplexNode *cn;
  int i = 0;

  printf("[");
  for (cn = l->head; cn != NULL; cn = cn->next) {
    complex_print(&cn->complex);
    if (i++ != l->size - 1)
      printf(", ");
  }
  printf("]");
}
