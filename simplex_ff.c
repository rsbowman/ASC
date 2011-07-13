#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "simplex_ff.h"

void simplex_ff_list_init(SimplexFFList *sl) {
  sl->head = sl->end = NULL;
  sl->size = 0;
}

void simplex_ff_list_destroy(SimplexFFList *sl) {
  SimplexFFNode *current, *tofree;

  if (sl == NULL)
    return;

  current = sl->head;
  while (current != NULL) {
    tofree = current;
    current = current->next;
    simplex_list_destroy(&tofree->free_faces);
    free(tofree);
  }

  sl->head = sl->end = NULL;
  sl->size = 0;
}

int simplex_ff_list_size(SimplexFFList *s) {
  return s->size;
}

SimplexFFNode *simplex_ff_node_create(Simplex *s, SimplexFFNode *next) {
  SimplexFFNode *n = (SimplexFFNode *)malloc(sizeof(SimplexFFNode));
  memcpy(&n->simplex, s, sizeof(Simplex));
  simplex_list_init(&n->free_faces);
  n->next = next;
  return n;
}

SimplexFFNode *simplex_ff_list_append(SimplexFFList *l, Simplex *s) {
  SimplexFFNode *new_node = simplex_ff_node_create(s, NULL);

  if (!l->head) {
    l->head = l->end = new_node;
  } else {
    l->end->next = new_node;
    l->end = new_node;
  }
  l->size++;
  return l->end;
}

SimplexFFNode *simplex_ff_list_insert(SimplexFFList *l, Simplex *s) {
  SimplexFFNode *new_node = simplex_ff_node_create(s, NULL); 
  SimplexFFNode *cur = l->head, *prev = l->head;

  if (cur == NULL) {
    l->head = l->end = new_node;
  } else {
    while (cur->next != NULL && simplex_compare(s, &cur->simplex) == -1) {
      prev = cur;
      cur = cur->next;
    }
    if (cur->next == NULL) {
	cur->next = new_node;
	l->end = new_node;
    } else if (cur == l->head) {
      new_node->next = l->head;
      l->head = new_node;
    } else {
      prev->next = new_node;
      new_node->next = cur;
    }
  }

  l->size++;
  return cur;
}

void simplex_ff_list_copy(SimplexFFList *dest, SimplexFFList *src) {
  SimplexFFNode *current, *end = NULL;

  assert(dest->head == NULL && dest->size == 0);

  for (current = src->head; current != NULL; current = current->next) {
    end = simplex_ff_list_append(dest, &current->simplex);
    simplex_list_copy(&end->free_faces, &current->free_faces);
  }
  dest->end = end;
  dest->size = src->size;
}

void simplex_ff_list_print(SimplexFFList *l) {
  SimplexFFNode *cur;

  printf("[");
  for (cur = l->head; cur != NULL; cur = cur->next) {
    simplex_print(&cur->simplex);
    if (cur != l->end)
      printf(", ");
  }
  printf("]");
}

void simplex_ff_list_print_full(SimplexFFList *l) {
  SimplexFFNode *cur;

  printf("[");
  for (cur = l->head; cur != NULL; cur = cur->next) {
    simplex_print(&cur->simplex);
    printf("(ff: ");
    simplex_list_print(&cur->free_faces);
    printf(")");
    if (cur != l->end)
      printf(", ");
  }
  printf("]");
}
