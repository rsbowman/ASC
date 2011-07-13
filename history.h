#ifndef HISTORY_H
#define HISTORY_H

#include "asc.h"
#include "simplex.h"

/*
 * support keeping a history of which simplices have been collapsed
 * and what cxs result from the collapsing
 */

typedef struct HistoryNode {
  Complex cx;
  Simplex sx;
  struct HistoryNode *next;
} HistoryNode;

typedef struct History {
  HistoryNode *head;
  HistoryNode *end;
  int size;
} History;

HistoryNode *history_node_create(Complex *c, Simplex *s, HistoryNode *next);
void history_init(History *h);
void history_destroy(History *);
//void simplex_list_copy(History *dest, History *src);
HistoryNode *history_append(History *l, Complex *c, Simplex *s);
//bool simplex_list_remove(History *l, Simplex *facet);
void history_print(History *l);
//bool simplex_list_contains(History *l, Simplex *s);
int history_size(History *l);

#endif
