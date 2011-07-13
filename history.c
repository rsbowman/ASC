#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "history.h"


HistoryNode *history_node_create(Complex *c, Simplex *s, 
				 HistoryNode *next) {
  HistoryNode *new_node = (HistoryNode *)malloc(sizeof(HistoryNode));
  assert(new_node != NULL);
  complex_init(&new_node->cx);
  complex_copy(&new_node->cx, c);
  simplex_copy(&new_node->sx, s);
  new_node->next = next;
  return new_node;
}

void history_init(History *h) {
  h->head = h->end = NULL;
  h->size = 0;
}

void history_destroy(History *l) {
  HistoryNode *current, *tofree;

  if (l == NULL)
    return;

  current = l->head;

  while (current) {
    tofree = current;
    current = current->next;
    complex_destroy(&tofree->cx);
    free(tofree);
  }

  l->head = l->end = NULL;
  l->size = 0;
}

HistoryNode *history_append(History *l, Complex *c, Simplex *s) {
  HistoryNode *new_node = history_node_create(c, s, 0);

  if (!l->head) {
    l->head = l->end = new_node;
  } else {
    l->end->next = new_node;
    l->end = new_node;
  }
  l->size++;

  return l->end;
}

void history_print(History *l) {
  HistoryNode *current = l->head;
  printf("starting with ");
  
  while (current) {
    complex_print(&current->cx);
    printf("\ncollapse ");
    simplex_print(&current->sx);
    if (current != l->end)
      printf(" to get\n");
    current = current->next;
  }
  printf(" and turn yourself around, that's what it's all about\n");
}

int history_size(History *l) {
  return l->size;
}
