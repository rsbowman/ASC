#include <stdio.h>
#include <assert.h>

#include "asc.h"
#include "ht.h"
#include "collapse.h"
#include "common.h"

void test_simplex() {
  Simplex s1, s2, s3, s4, s5, s6, sf1, sf2, sf3;

  simplex_init(&s1, 3, 1, 2, 3, 99);
  simplex_init(&s2, 2, 2, 3, -1, -1);
  simplex_init(&s3, 3, 1, 2, 3, -1);
  simplex_init(&s4, 1, 2, -1, -1, -1);
  simplex_init(&sf1, 2, 1, 2, -1, -1);
  simplex_init(&sf2, 2, 2, 3, -1, -1);
  simplex_init(&sf3, 2, 1, 3, -1, -1);
  simplex_init(&s5, 2, 3, 4, -1, -1);
  simplex_init(&s6, 1, 99, -1, -1, -1);

  SimplexList l;
  simplex_list_init(&l);

  // contains
  assert(simplex_contains_vertex(&s1, 2));
  assert(!simplex_contains_vertex(&s1, 4));

  // is_subface
  assert(simplex_is_subface(&s2, &s1));
  assert(!simplex_is_subface(&s1, &s2));
  assert(simplex_is_facet(&s2, &s1));
  assert(simplex_is_subface(&s4, &s1));
  assert(!simplex_is_facet(&s4, &s1));
  assert(!simplex_is_subface(&s5, &s1));
  assert(!simplex_is_subface(&s6, &s1));

  // is_equal
  assert(simplex_hash(&s1) == simplex_hash(&s3));
  assert(simplex_hash(&s1) != simplex_hash(&s2));

  assert(simplex_is_equal(&s1, &s3));
  assert(!simplex_is_equal(&s1, &s2));

  assert(simplex_compare(&s1, &s3) == 0);
  assert(simplex_compare(&s1, &s2) == 1);
  assert(simplex_compare(&s2, &s1) == -1);

  // subfaces
  simplex_subfaces(&s1, &l);
  assert(simplex_list_size(&l) == 3);
  assert(simplex_list_contains(&l, &sf1));
  assert(simplex_list_contains(&l, &sf2));
  assert(simplex_list_contains(&l, &sf3));
  printf("subfaces of "); simplex_print(&s1); printf(": ");
  simplex_list_print(&l); printf("\n");

  simplex_list_destroy(&l);
}

void test_simplex_list() {
  Simplex s1, s2;
  SimplexList l, l2;
  
  simplex_init(&s1, 3, 1, 2, 3, -1);
  simplex_init(&s2, 3, 2, 3, 4, -1);

  simplex_list_init(&l);
  simplex_list_init(&l2);
  
  simplex_list_append(&l, &s1);
  simplex_list_append(&l, &s2);
  assert(simplex_list_size(&l) == 2);

  simplex_list_copy(&l2, &l);
  assert(simplex_list_size(&l2) == 2);
  assert(simplex_list_contains(&l2, &s1));
  assert(simplex_list_contains(&l2, &s2));
  
  simplex_list_destroy(&l);
  assert(simplex_list_size(&l2) == 2);
  assert(simplex_list_contains(&l2, &s1));
  assert(simplex_list_contains(&l2, &s2));

  simplex_list_destroy(&l2);
}

void test_create_complex() {
  Complex c, c2;
  SimplexList free_faces;
  Simplex s1, s2, ff, ff1, ff2, nf, nf2, c2s1, c2s2, c2ff, c2nf;

  simplex_init(&s1, 3, 1, 2, 3, -1);
  simplex_init(&s2, 3, 2, 3, 4, -1);
  simplex_init(&ff, 2, 3, 4, -1, -1);
  simplex_init(&ff1, 2, 1, 2, -1, -1);
  simplex_init(&ff2, 2, 1, 3, -1, -1);
  simplex_init(&nf, 2, 2, 3, -1, -1);
  simplex_init(&nf2, 1, 1, -1, -1, -1);
  simplex_init(&c2s1, 4, 1, 2, 3, 4);
  simplex_init(&c2s2, 2, 4, 5, -1, -1);
  simplex_init(&c2ff, 1, 5, -1, -1, -1);
  simplex_init(&c2nf, 1, 4, -1, -1, -1);

  complex_init(&c);
  complex_init(&c2);

  simplex_list_init(&free_faces);

  complex_add_facet(&c, &s1);
  complex_add_facet(&c, &s2);
  complex_add_facet(&c2, &c2s1);
  complex_add_facet(&c2, &c2s2);

  complex_compute_free_faces(&c);
  complex_compute_free_faces(&c2);

  complex_print(&c); printf("\n");
  
  // complex_size
  assert(complex_size(&c) == 2);
  assert(complex_size(&c2) == 2);

  // is_free_face
  assert(complex_is_free_face(&c, &ff));
  assert(!complex_is_free_face(&c, &nf));
  assert(!complex_is_free_face(&c, &nf2));

  assert(complex_is_free_face(&c2, &c2ff));
  assert(!complex_is_free_face(&c2, &c2nf));

  // free_face_for_facet
  complex_free_faces_for_facet(&c, &s1, &free_faces);
  assert(simplex_list_size(&free_faces) == 2);
  assert(simplex_list_contains(&free_faces, &ff1));
  assert(simplex_list_contains(&free_faces, &ff2));

  printf("free faces of "); simplex_print(&s1); printf(": "); 
  simplex_list_print(&free_faces);
  printf("\n");

  simplex_list_destroy(&free_faces); 
  complex_destroy(&c);
  complex_destroy(&c2);
}

void test_free_faces() {
  Simplex s1, s2, s3;
  Simplex ff1, ff2, ff3;
  Complex cx;
  SimplexList list;

  simplex_list_init(&list);

  simplex_init(&s1, 4, 0, 1, 2, 3);
  simplex_init(&s2, 4, 0, 1, 2, 4);
  simplex_init(&s3, 4, 0, 1, 2, 5);

  simplex_init(&ff1, 3, 0, 1, 3, -1);
  simplex_init(&ff2, 3, 0, 2, 3, -1);
  simplex_init(&ff3, 3, 1, 2, 3, -1);

  complex_init(&cx);
  complex_add_facet(&cx, &s1);
  complex_add_facet(&cx, &s2);
  complex_add_facet(&cx, &s3);

  complex_compute_free_faces(&cx);

  complex_free_faces_for_facet(&cx, &s1, &list);
  assert(simplex_is_facet(&ff3, &s1));
  assert(complex_is_free_face(&cx, &ff1));
  assert(complex_is_free_face(&cx, &ff2));
  assert(complex_is_free_face(&cx, &ff3));
  assert(simplex_list_size(&list) == 3);
}

void test_complex_hash() {
  Simplex s1, s2, s3;
  Complex c1, c2, c3;

  simplex_init(&s1, 3, 1, 2, 3, -1);
  simplex_init(&s2, 3, 2, 3, 4, -1);
  simplex_init(&s3, 3, 3, 4, 5, -1);

  complex_init(&c1);
  complex_init(&c2);
  complex_init(&c3);

  complex_add_facet(&c1, &s1); 
  complex_add_facet(&c1, &s2);
  complex_add_facet(&c1, &s3);

  complex_add_facet(&c2, &s2);
  complex_add_facet(&c2, &s3);
  complex_add_facet(&c2, &s1);

  complex_add_facet(&c3, &s1);
  complex_add_facet(&c3, &s2);

  complex_compute_free_faces(&c1);
  complex_compute_free_faces(&c2);
  complex_compute_free_faces(&c3);

  assert(complex_hash(&c1) == complex_hash(&c2));
  assert(complex_hash(&c1) != complex_hash(&c3));

  assert(complex_is_subcomplex(&c1, &c2));
  assert(complex_is_subcomplex(&c2, &c1));
  assert(complex_is_subcomplex(&c3, &c1));
  assert(!complex_is_subcomplex(&c1, &c3));

  printf("xxx "); complex_print(&c1);
  complex_print(&c2);
  assert(complex_equals(&c1, &c2));
  assert(!complex_equals(&c1, &c3));

  complex_destroy(&c1);
  complex_destroy(&c2);
  complex_destroy(&c3);
}

void test_remove_face() {
  Complex c1, c2, c3;
  Simplex c3s1 = {3, {1, 2, 3, -1}},
    c3s2 = {3, {2, 3, 4, -1}},
    c3s3 = {3, {3, 4, 5, -1}},
    c3s4 = {3, {4, 5, 6, -1}},
    sx = {3, {7, 8, 9, -1}};
   
  simplex_init(&c3s1, 3, 1, 2, 3, -1);
  simplex_init(&c3s2, 3, 2, 3, 4, -1);
  simplex_init(&c3s3, 3, 3, 4, 5, -1);
  simplex_init(&c3s4, 3, 4, 5, 6, -1);
  simplex_init(&sx, 3, 7, 8, 9, -1);

  complex_init(&c1);
  complex_init(&c2);
  complex_init(&c3);

  complex_add_facet(&c1, &c3s1);  
  complex_add_facet(&c1, &c3s2);  
  complex_add_facet(&c1, &c3s3);  
  complex_add_facet(&c1, &c3s4);

  assert(complex_size(&c1) == 4);
  complex_copy(&c2, &c1);
  complex_copy(&c3, &c1);

  assert(complex_remove_facet(&c1, &c3s1));
  assert(complex_size(&c1) == 3);
  assert(complex_contains_facet(&c1, &c3s2));
  assert(complex_contains_facet(&c1, &c3s3));
  assert(complex_contains_facet(&c1, &c3s4));

  assert(complex_remove_facet(&c2, &c3s2));
  assert(complex_size(&c2) == 3);
  assert(complex_contains_facet(&c2, &c3s1));
  assert(complex_contains_facet(&c2, &c3s3));
  assert(complex_contains_facet(&c2, &c3s4));
 
  assert(complex_remove_facet(&c3, &c3s4));
  assert(complex_size(&c2) == 3);
  assert(complex_contains_facet(&c3, &c3s1));  
  assert(complex_contains_facet(&c3, &c3s2));  
  assert(complex_contains_facet(&c3, &c3s3));

  assert(!complex_remove_facet(&c1, &sx));

  complex_destroy(&c1);
  complex_destroy(&c2);
  complex_destroy(&c3);
}

void test_complex_stack() {
  ComplexStack stack;

  Complex c1, c2, c3, popped;
  Simplex s1, s2;

  simplex_init(&s1, 3, 1, 2, 3, -1);
  simplex_init(&s2, 3, 1, 3, 4, -1);

  complex_init(&c1);
  complex_init(&c2);
  complex_init(&c3);
  complex_init(&popped);
  complex_stack_init(&stack);

  complex_add_facet(&c1, &s1);
  complex_add_facet(&c1, &s2);
  complex_copy(&c2, &c1);
  complex_copy(&c3, &c1);

  complex_compute_free_faces(&c1);

  assert(complex_stack_size(&stack) == 0);
  assert(!complex_stack_pop(&stack, &popped));
  complex_stack_push(&stack, &c1);

  assert(complex_stack_size(&stack) == 1);
  assert(complex_stack_pop(&stack, &popped));
  assert(complex_stack_size(&stack) == 0);
  assert(complex_size(&popped) == 2);
  assert(complex_contains_facet(&popped, &s1));
  complex_destroy(&popped);
  complex_stack_push(&stack, &c1);
  complex_stack_push(&stack, &c2);
  complex_stack_push(&stack, &c3);
  assert(complex_stack_size(&stack) == 3);
  assert(complex_stack_pop(&stack, &popped));
  assert(complex_stack_size(&stack) == 2);
  assert(complex_size(&popped) == 2);
  assert(complex_contains_facet(&popped, &s1));

  complex_destroy(&popped);
  complex_destroy(&c1);
  complex_destroy(&c2);
  complex_destroy(&c3);
}

void test_hashtable() {
  Simplex s1, s2, s3, s4;
  Complex c1, c2, c3, c4;
  Hashtable ht;

  simplex_init(&s1, 3, 1, 2, 3, -1);
  simplex_init(&s2, 3, 2, 3, 4, -1);
  simplex_init(&s3, 3, 3, 4, 5, -1);
  simplex_init(&s4, 3, 4, 65, 74, -1); // hash collision w/ c1 below

  complex_init(&c1);
  complex_init(&c2);
  complex_init(&c3);
  complex_init(&c4);

  hashtable_init(&ht, 10, 20);

  complex_add_facet(&c1, &s1); 
  complex_add_facet(&c1, &s2);
  complex_add_facet(&c1, &s3);

  complex_add_facet(&c2, &s2);
  complex_add_facet(&c2, &s3);
  complex_add_facet(&c2, &s1);

  complex_add_facet(&c3, &s1);
  complex_add_facet(&c3, &s2);

  complex_add_facet(&c4, &s4);

  complex_compute_free_faces(&c1);
  complex_compute_free_faces(&c2);
  complex_compute_free_faces(&c3);
  complex_compute_free_faces(&c4);

  // this should cause collision:
  /*  assert(complex_hash(&c1) == complex_hash(&c4));*/

  assert(!hashtable_lookup(&ht, &c1, True));
  assert(hashtable_lookup(&ht, &c1, True));
  assert(hashtable_lookup(&ht, &c2, True));
  assert(!hashtable_lookup(&ht, &c3, True));
  assert(!hashtable_lookup(&ht, &c4, True));
  assert(hashtable_lookup(&ht, &c1, True));
  assert(hashtable_lookup(&ht, &c4, True));
  assert(ht.inserts == 3);
  /*  assert(ht.sizes[0] == 2);
  assert(ht.sizes[1] == 1);
  assert(ht.sizes[2] == 0);*/

  hashtable_destroy(&ht);
}

void test_collapse_along() {
  Complex c, new;
  Simplex s1, s2, ff, newfacet;

  simplex_init(&s1, 3, 1, 2, 3, -1);
  simplex_init(&s2, 3, 2, 3, 4, -1);
  simplex_init(&ff, 2, 3, 4, -1, -1);
  simplex_init(&newfacet, 2, 2, 4, -1, -1);

  complex_init(&c);
  complex_init(&new);
  complex_add_facet(&c, &s1);
  complex_add_facet(&c, &s2);

  complex_compute_free_faces(&c);

  complex_collapse_along(&c, &s2, &ff, &new);
  assert(complex_size(&new) == 2);
  assert(complex_contains_facet(&new, &newfacet));

  complex_destroy(&new);
  complex_destroy(&c);
}

void test_collapse() {
  Complex c1, c2;
  Simplex s1;
  ComplexStack cxs;

  simplex_init(&s1, 3, 1, 2, 3, -1);

  complex_init(&c1);
  complex_init(&c2);
  complex_stack_init(&cxs);
  complex_add_facet(&c1, &s1);

  complex_compute_free_faces(&c1);

  complex_collapse_facet(&c1, &s1, &cxs);
  assert(complex_stack_size(&cxs) == 3);
  printf("collapsing "); complex_print(&c1); printf(" gives\n  ");
  complex_stack_print(&cxs); printf("\n");

  complex_stack_pop(&cxs, &c2);
  assert(complex_stack_size(&cxs) == 2);
  assert(complex_size(&c2) == 2);

  printf("hey! "); complex_print(&c1); printf("\n"); fflush(stdout);
  assert(complex_is_collapsible(&c1, False));

  complex_destroy(&c1);
  complex_destroy(&c2);
  complex_stack_destroy(&cxs);
}

void test_collapse_simplex() {
  Complex c, c2;
  ComplexStack stack;

  Simplex s1, s2, s3, s4;

  simplex_init(&s1, 2, 0, 1, -1, -1);
  simplex_init(&s2, 2, 2, 4, -1, -1);
  simplex_init(&s3, 3, 1, 2, 3, -1);
  simplex_init(&s4, 4, 2, 3, 4, 5);

  complex_init(&c);
  complex_init(&c2);

  complex_add_facet(&c, &s1);
  complex_add_facet(&c, &s2);
  complex_add_facet(&c, &s3);

  complex_add_facet(&c2, &s3);
  complex_add_facet(&c2, &s4);

  complex_compute_free_faces(&c);
  complex_compute_free_faces(&c2);

  complex_stack_init(&stack);
  complex_collapse_simplex_rec(&c, &s3, &stack);

  assert(complex_stack_size(&stack) == 2);
  //printf("collapse_simplex: "); complex_stack_print(&stack); printf("\n");

  complex_stack_destroy(&stack);

  complex_collapse_simplex_rec(&c2, &s4, &stack);
  printf("collapse_simplex: "); complex_stack_print(&stack); printf("\n");
  assert(complex_stack_size(&stack) == 1);

  complex_stack_destroy(&stack);
  complex_destroy(&c);
}

void test_non_mfld() {
  Complex cx, cx2, cx3;
  ComplexStack stack;
  Simplex s1, s2, s3, edge;
  
  complex_init(&cx); // 3 2-simplices attached along an edge
  complex_init(&cx2);
  complex_init(&cx3);
  complex_stack_init(&stack);
  simplex_init(&s1, 3, 1, 2, 3, -1);
  simplex_init(&s2, 3, 2, 3, 4, -1);
  simplex_init(&s3, 3, 2, 3, 5, -1);
  simplex_init(&edge, 2, 2, 3, -1, -1);
  complex_add_facet(&cx, &s1);
  complex_add_facet(&cx, &s2);
  complex_add_facet(&cx, &s3);
  complex_compute_free_faces(&cx);
  
  assert(!complex_is_free_face(&cx, &edge));
  complex_collapse_simplex_rec(&cx, &s3, &stack);
  assert(complex_stack_size(&stack) == 1);
  complex_stack_pop(&stack, &cx2);
  assert(!complex_is_free_face(&cx2, &edge));
  complex_collapse_simplex_rec(&cx2, &s2, &stack);
  assert(complex_stack_size(&stack) == 1);
  complex_stack_pop(&stack, &cx3);
  complex_print(&cx3); fflush(stdout);
  assert(complex_is_free_face(&cx3, &edge));

  complex_destroy(&cx);
  complex_destroy(&cx2);
  complex_destroy(&cx3);
  complex_stack_destroy(&stack);
}

void test_rand() {
  int nums[6] = {0, 0, 0, 0, 0, 0};
  int i;

  for (i = 0; i < 1000; ++i)
    nums[random_int(1, 6)]++;

  for (i = 1; i < 6; ++i) {
    assert(nums[i] > 0);
  }
}

int main() {
  printf("test simplex:\n");
  test_simplex();
  printf("test simplex list:\n");
  test_simplex_list();
  printf("test create complex:\n");
  test_create_complex();
  printf("test complex hash:\n");
  test_complex_hash();
  printf("test remove face:\n");
  test_remove_face();
  printf("test complex stack:\n");
  test_complex_stack();
  printf("test hashtable:\n");
  test_hashtable();
  printf("test collapse along\n");
  test_collapse_along();
  printf("test collapse:\n");
  test_collapse();
  printf("test collapse_simplex\n");
  test_collapse_simplex();
  printf("test non mfld\n");
  test_non_mfld();
  printf("test free faces\n");
  test_free_faces();

  printf("test rand\n");
  test_rand();

  printf("tests passed! yay!\n");
  return 0;
}
