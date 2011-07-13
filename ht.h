#ifndef HT_H
#define HT_H

#include "asc.h"
#include "common.h"

// a big hashtable:
#define HT_BIG 500009
#define HT_SMALL 37

#define MAX_HT_DEPTH 1000

typedef struct SerializedComplexNode SerializedComplexNode;
struct SerializedComplexNode {
  char serialized[MAX_SERIALIZED_SIZE];
  SerializedComplexNode *next;
};

/* actually a LRU Cache...
 */
typedef struct Hashtable {
  int n_buckets;
  int size;
  int max_size;
  SerializedComplexNode **bucket;
  long long hits;
  long long inserts;
  long long deletes;
  long long sizes[MAX_HT_DEPTH];

  hashtype *lru_data;
  int lru_index;
} Hashtable;

void hashtable_init(Hashtable *ht, int n_buckets, int max_size);
void hashtable_destroy(Hashtable *ht);
bool hashtable_lookup(Hashtable *ht, Complex *c, bool insert);
void hashtable_insert(Hashtable *ht, Complex *c, hashtype hash);
void hashtable_print_stats(Hashtable *ht);
int hashtable_bucket_size(Hashtable *ht, hashtype hash);

SerializedComplexNode *
serialized_complex_node_create(Complex *c, SerializedComplexNode *next);

void hashtable_record_use(Hashtable *ht, hashtype hash);
void hashtable_remove_lru(Hashtable *ht);
ComplexNode *get_lru_node(Hashtable *ht);

#endif
