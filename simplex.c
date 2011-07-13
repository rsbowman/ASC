#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "simplex.h"

void simplex_init(Simplex *s, int size, int v1, int v2, int v3, int v4) {
  int i;
  s->size = size;
  s->vertices[0] = v1;
  s->vertices[1] = v2;
  s->vertices[2] = v3;
  s->vertices[3] = v4;
  for (i = size; i < NVERTICES; ++i) 
    s->vertices[i] = -1;
  assert(simplex_is_sorted(s));
}

int simplex_size(Simplex *s) {
  return s->size;
}

void simplex_copy(Simplex *dest, Simplex *src) {
  memcpy(dest, src, sizeof(Simplex));
}

/* assume s->vertices are sorted */
bool simplex_contains_vertex(Simplex *s, int vertex) {
  int i;
  for (i = 0; i < s->size; ++i) {
    if (s->vertices[i] == vertex)
      return True;
    if (s->vertices[i] > vertex)
      return False;
  }
  return False;
}

int simplex_common_subface_size(Simplex *s1, Simplex *s2) {
  int s = 0, i;

  for (i = 0; i < s1->size; ++i) {
    if (simplex_contains_vertex(s2, s1->vertices[i]))
      s++;
  }
  return s;
}

bool simplex_is_sorted(Simplex *s) {
  int i;
  for (i = 0; i < s->size - 1; ++i) {
    if (s->vertices[i] >= s->vertices[i+1])
      return False;
  }
  return True;
}

/* assume the simlices verts are sorted */
bool simplex_is_subface(Simplex *sf, Simplex *f) {
  int s1 = sf->size, s2 = f->size;
  int *v1 = sf->vertices, *v2 = f->vertices;
  int i = 0, j = 0;

  while (i < s1 && j < s2) {
    while ((v1[i] > v2[j]) && (j < s2))
      j++;
    if ((v1[i] != v2[j]) || (j >= s2))
      return False;
    i++; j++;
  }
  return i == s1;
}

/* return 1 if s1->vertices is lexicographically smaller than s2->vertices,
 * 0 if equal, &c.
 * assume unused bits of ->vertices are all set to the same value
 */
int simplex_compare(Simplex *s1, Simplex *s2) {
  int i;
  for (i = 0; i < NVERTICES; ++i) {
    if (s1->vertices[i] < s2->vertices[i])
      return 1;
    else if (s1->vertices[i] > s2->vertices[i])
      return -1;
  }
  return 0;
}

bool simplex_is_equal(Simplex *s1, Simplex *s2) {
  int i;
  if (s1->size != s2->size)
    return False;
  for (i = 0; i < s1->size; ++i)
    if (s1->vertices[i] != s2->vertices[i])
      return False;
  return True;
}

bool simplex_is_facet(Simplex *facet, Simplex *s) {
  int v1, v2, v3;
  if (simplex_size(facet) != simplex_size(s) - 1)
    return False;
  switch (simplex_size(facet)) {
  case 1: 
    v1 = facet->vertices[0];
    return (v1 == s->vertices[0] || v1 == s->vertices[1]);
  case 2:
    v1 = facet->vertices[0];
    v2 = facet->vertices[1];
    return ((v1 == s->vertices[0] && (v2 == s->vertices[1] ||
				      v2 == s->vertices[2])) ||
	    (v1 == s->vertices[1] && v2 == s->vertices[2]));
  case 3:
    v1 = facet->vertices[0];
    v2 = facet->vertices[1];
    v3 = facet->vertices[2];
    return ((v1 == s->vertices[0] && v2 == s->vertices[1] &&
	     (v3 == s->vertices[2] || v3 == s->vertices[3])) ||
	    (v1 == s->vertices[0] && v2 == s->vertices[2] && 
	     v3 == s->vertices[3]) ||
	    (v1 == s->vertices[1] && v2 == s->vertices[2] &&
	     v3 == s->vertices[3]));
  }
  assert(False);
}

/*bool simplex_is_facet(Simplex *facet, Simplex *s) {
  return ((simplex_size(facet) == simplex_size(s) - 1) &&
	  simplex_is_subface(facet, s));
	  }*/

void simplex_subfaces(Simplex *s, SimplexList *subfaces) {
  int v1, v2, v3, v4;
  Simplex s1, s2, s3, s4;

  v1 = s->vertices[0]; v2 = s->vertices[1]; 
  v3 = s->vertices[2]; v4 = s->vertices[3];

  assert(s->size > 0 && s->size < 5);
  // if s->size == 1, we return an empty list below
  if (s->size == 2) {
    s1.size = 1; s2.size = 1;
    s1.vertices[0] = v1; s2.vertices[0] = v2;
    simplex_list_append(subfaces, &s1);
    simplex_list_append(subfaces, &s2);
  } else if (s->size == 3) {
    s1.size = 2; s2.size = 2; s3.size = 2;
    s1.vertices[0] = v1; s1.vertices[1] = v2;
    s2.vertices[0] = v2; s2.vertices[1] = v3;
    s3.vertices[0] = v1; s3.vertices[1] = v3;
    simplex_list_append(subfaces, &s1);
    simplex_list_append(subfaces, &s2);
    simplex_list_append(subfaces, &s3);
  } else if (s->size == 4) {
    s1.size = 3; s2.size = 3; s3.size = 3; s4.size = 3;
    s1.vertices[0] = v1; s1.vertices[1] = v2; s1.vertices[2] = v3;
    s2.vertices[0] = v1; s2.vertices[1] = v2; s2.vertices[2] = v4;
    s3.vertices[0] = v1; s3.vertices[1] = v3; s3.vertices[2] = v4;
    s4.vertices[0] = v2; s4.vertices[1] = v3; s4.vertices[2] = v4;
    simplex_list_append(subfaces, &s1);
    simplex_list_append(subfaces, &s2);
    simplex_list_append(subfaces, &s3);
    simplex_list_append(subfaces, &s4);
  }
}

/* found at http://www.cs.yorku.ca/~oz/hash.html */
hashtype simplex_hash(Simplex *s) { 
  hashtype h = 5381;
  int i;
  for (i = 0; i < s->size; ++i)
    h = ((h << 5) + h) + s->vertices[i];
  return h;
}

void simplex_print(Simplex *s) {
  int i;
  printf("{");
  for (i = 0; i < s->size; ++i) {
    printf("%d", s->vertices[i]);
    if (i != s->size - 1)
      printf(", ");
  }
  printf("}");
}

void simplex_print_letters(Simplex *s) {
  int i;
  for (i = 0; i < s->size; ++i) {
    printf("%c", INT_TO_LETTER(s->vertices[i]));
  }
}

//////////////////////////////////////////////////////////
// SimplexList 

void simplex_list_init(SimplexList *l) {
  l->head = l->end = NULL;
  l->size = 0;
}

void simplex_list_destroy(SimplexList *l) {
  SimplexNode *current, *tofree;

  if (l == NULL)
    return;

  current = l->head;

  while (current) {
    tofree = current;
    current = current->next;
    free(tofree);
  }

  l->head = l->end = NULL;
  l->size = 0;
}

void simplex_list_copy(SimplexList *dest, SimplexList *src) {
  SimplexNode *current, *end = NULL;
  // dest should be empty:
  assert(dest->head == 0 && dest->size == 0);

  for (current = src->head; current != NULL; current = current->next) {
    end = simplex_list_append(dest, &current->simplex);
  }
  dest->end = end;
  dest->size = src->size;
}

SimplexNode *simplex_node_create(Simplex *s, SimplexNode *next) {
  SimplexNode *new_node = (SimplexNode *)malloc(sizeof(SimplexNode));
  assert(new_node != NULL);
  memcpy(&new_node->simplex, s, sizeof(Simplex));
  new_node->next = next;
  return new_node;
}

/*
 * append node COPYING the data from s
 * return a pointer to the added node
 */
SimplexNode *simplex_list_append(SimplexList *l, Simplex *s) {
  SimplexNode *new_facet_node = simplex_node_create(s, 0);

  if (!l->head) {
    l->head = l->end = new_facet_node;
  } else {
    l->end->next = new_facet_node;
    l->end = new_facet_node;
  }
  l->size++;

  return l->end;
}

bool simplex_list_remove(SimplexList *l, Simplex *facet) {
  SimplexNode *current, *prev;
  
  prev = l->head;
  for (current = l->head; current != NULL; current = current->next) {
    if (simplex_is_equal(&current->simplex, facet)) {
      if (current == prev) { // 1st element of list
	l->head = current->next;
      } else { // current != prev so not 1st element
	if (current == l->end) {
	  prev->next = NULL;
	  l->end = prev;
	} else {
	  prev->next = current->next;
	}
      }
      free(current);
      l->size--;
      return True;
    }
    prev = current;
  }
  return False;
}

int simplex_list_size(SimplexList *l) {
  return l->size;
}

bool simplex_list_contains(SimplexList *l, Simplex *s) {
  SimplexNode *current;
  for (current = l->head; current != NULL; current = current->next) {
    if (simplex_is_equal(&current->simplex, s))
      return True;
  }
  return False;
}

void simplex_list_print(SimplexList *l) {
  SimplexNode *current = l->head;
  printf("[");
  while (current) {
    simplex_print(&current->simplex);
    if (current != l->end)
      printf(", ");
    current = current->next;
  }
  printf("]");
}

