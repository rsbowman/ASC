#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>

#include "ht.h"

void hashtable_init(Hashtable *ht, int n_buckets, int max_size) {
  int i;
  ht->hits = 0;
  ht->inserts = 0;
  ht->deletes = 0;
  ht->n_buckets = n_buckets;
  ht->size = 0;
  ht->max_size = max_size;
  ht->bucket = (SerializedComplexNode **)
    calloc(n_buckets, sizeof(SerializedComplexNode *));
  for (i = 0; i < n_buckets; ++i)
    ht->bucket[i] = NULL;
  ht->lru_data = (hashtype *)calloc(max_size, sizeof(hashtype));
  for (i = 0; i < max_size; ++i)
    ht->lru_data[i] = -1;
  ht->lru_index = 0;
  for (i = 0; i < MAX_HT_DEPTH; ++i)
    ht->sizes[i] = 0;
}

void hashtable_destroy(Hashtable *ht) {
  int i;
  SerializedComplexNode *cn, *tofree;

  for (i = 0; i < ht->n_buckets; ++i) {
    if (ht->bucket[i]) {
      cn = ht->bucket[i];
      while (cn != NULL) {
	tofree = cn;
	cn = cn->next;
	free(tofree);
      }
    }
  }
  free(ht->bucket);
}

/* 
 * look up a complex in the hashtable.  if it's there, return true.
 * if not, add it and return false.
 */
bool hashtable_lookup(Hashtable *ht, Complex *c, bool insert) {
  hashtype hash = complex_hash(c) % ht->n_buckets;
  SerializedComplexNode *cn = ht->bucket[hash];

  if (cn != NULL) {
    for ( ; cn != NULL; cn = cn->next) {
      if (strcmp(c->serialized, cn->serialized) == 0) {
	ht->hits++;
	return True;
      }
    }
  }

  if (insert) { // not in the list, add it
    hashtable_insert(ht, c, hash);
  }

  return False;
}

int hashtable_bucket_size(Hashtable *ht, hashtype hash) {
  SerializedComplexNode *cn = ht->bucket[hash];
  int size = 0;

  while (cn != NULL) {
    cn = cn->next;
    size++;
  }

  return size;
}

SerializedComplexNode *
serialized_complex_node_create(Complex *c, SerializedComplexNode *next) {
  SerializedComplexNode *new_node = (SerializedComplexNode *)
    malloc(sizeof(SerializedComplexNode));
  strcpy(new_node->serialized, c->serialized);
  new_node->next = next;
  return new_node;							      
}

void hashtable_insert(Hashtable *ht, Complex *c, hashtype hash) {
  SerializedComplexNode *cn, *new_node;
  int depth = 1;

  if (ht->size >= ht->max_size) {
    hashtable_remove_lru(ht);
  }

  cn = ht->bucket[hash];
  new_node = serialized_complex_node_create(c, NULL);
  if (cn == NULL) {
    ht->bucket[hash] = new_node;
    ht->sizes[0]++;
  } else {
    while (cn->next != NULL) {
      cn = cn->next;
      depth++;
    }
    assert(cn->next == NULL);
    cn->next = new_node;
    if (depth < MAX_HT_DEPTH)
      ht->sizes[depth]++;
  }

  ht->size++;
  ht->inserts++;
  hashtable_record_use(ht, hash);
}

void hashtable_record_use(Hashtable *ht, hashtype hash) {
  ht->lru_data[ht->lru_index++] = hash;
  if (ht->lru_index >= ht->max_size)
    ht->lru_index = 0;
}

hashtype hashtable_get_lru_node(Hashtable *ht) {
  int index = ht->lru_index;

  assert(index >= 0 && index < ht->max_size);
  return ht->lru_data[index];
}

void hashtable_remove_lru(Hashtable *ht) {
  hashtype hash_to_remove;
  SerializedComplexNode *cur, *prev;
  int depth = 0;

  hash_to_remove = hashtable_get_lru_node(ht);
  assert(hash_to_remove != -1);
  cur = prev = ht->bucket[hash_to_remove];

  assert(cur != NULL);
  if (cur->next == NULL) {
    free(cur);
    ht->bucket[hash_to_remove] = NULL;
    ht->size--;
    ht->deletes++;
    ht->sizes[0]--;
  } else {
    while (cur->next != NULL) {
      prev = cur;
      cur = cur->next;
      depth++;
    }
    
    prev->next = NULL;
    free(cur);
    ht->size--;
    ht->deletes++;
    if (depth < MAX_HT_DEPTH)
      ht->sizes[depth]--;
  }
}

void hashtable_print_stats(Hashtable *ht) {
  int i;
  printf("%lld inserts, %lld deletes, %lld hits\n", 
	 ht->inserts, ht->deletes, ht->hits);
  printf("hash bucket sizes: ");
  for (i = 0; i < MAX_HT_DEPTH; ++i) {
    if (ht->sizes[i])
      printf("%lld ", ht->sizes[i]);
  }
  printf("\n");
}
