#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "collapse.h"
#include "simplex_ff.h"
#include "history.h"

void complex_collapse_along(Complex *c, Simplex *facet, Simplex *free_face,
			    Complex *new_complex) {
  SimplexList faces_to_add;
  SimplexNode *sn;

  /* assert(complex_is_free_face(c, free_face) &&
     simplex_is_facet(free_face, facet)); */

  simplex_list_init(&faces_to_add);

  complex_free_faces_for_facet(c, facet, &faces_to_add);
  simplex_list_remove(&faces_to_add, free_face);

  complex_copy(new_complex, c);
  complex_remove_facet(new_complex, facet);
  for (sn = faces_to_add.head; sn != NULL; sn = sn->next)
      complex_add_facet(new_complex, &sn->simplex);
  
  complex_recompute_free_faces(new_complex, facet, &faces_to_add);
  simplex_list_destroy(&faces_to_add);
}


void complex_collapse_facet(Complex *c, Simplex *facet,
			    ComplexStack *l) {
  SimplexList free_faces;
  SimplexNode *current, *otherface;
  Complex cx_wo_facet, new_complex;
  
  simplex_list_init(&free_faces);
  complex_init(&new_complex); // the complex to add to the list
  complex_init(&cx_wo_facet);
  complex_free_faces_for_facet(c, facet, &free_faces);
  
  complex_copy(&cx_wo_facet, c);
  complex_remove_facet(&cx_wo_facet, facet);

  for (current = free_faces.head; 
       current != NULL; current = current->next) {
    complex_copy(&new_complex, &cx_wo_facet);
    for (otherface = free_faces.head;
	 otherface != NULL; otherface = otherface->next) {
      if (!simplex_is_equal(&current->simplex, &otherface->simplex))
	complex_add_facet(&new_complex, &otherface->simplex);
    }
    /*    complex_recompute_free_faces(&new_complex, facet);*/
    complex_compute_free_faces(&new_complex);
    new_complex.n_collapses++;
    complex_stack_push(l, &new_complex);
    complex_destroy(&new_complex);
  }
  
  complex_destroy(&cx_wo_facet);
  simplex_list_destroy(&free_faces);
}

/* try to collapse a simplex as much as possible */
void complex_collapse_simplex(Complex *c, Simplex *s, 
			      ComplexStack *to_return) {
  Hashtable ht;
  Complex cx;
  ComplexStack stack;
  SimplexList to_collapse;
  SimplexList already_collapsed;
  SimplexNode *sn;

  simplex_list_init(&to_collapse);
  complex_collapsible_facets(c, s, &to_collapse);

  if (simplex_list_size(&to_collapse) == 0) {// no more faces to collapse
    simplex_list_destroy(&to_collapse);
    return;
  }
  simplex_list_destroy(&to_collapse);

  hashtable_init(&ht, HT_SMALL, 100);
  simplex_list_init(&already_collapsed);
  complex_init(&cx);
  complex_stack_init(&stack);

  complex_stack_push(&stack, c);
  while (complex_stack_size(&stack) > 0) {
    complex_stack_pop(&stack, &cx);
    if (hashtable_lookup(&ht, &cx, True)) { // if cx is in the ht, pop another
      complex_destroy(&cx);
      continue;
    }
    complex_collapsible_facets(&cx, s, &to_collapse);
    if (simplex_list_size(&to_collapse) == 0) {
      cx.n_collapses++;
      complex_stack_push(to_return, &cx);
    } else {
      for (sn = to_collapse.head; sn != NULL; sn = sn->next) {
	complex_collapse_facet(&cx, &sn->simplex, &stack);
      }
    }
    simplex_list_destroy(&to_collapse);
    complex_destroy(&cx);
  }

  simplex_list_destroy(&to_collapse);
  simplex_list_destroy(&already_collapsed);
  complex_stack_destroy(&stack);
  hashtable_destroy(&ht);  
}

/* same as above, recursive, and a little faster */
void complex_collapse_simplex_rec(Complex *c, Simplex *s, 
				  ComplexStack *to_return) {
  SimplexList to_collapse;
  SimplexList already_collapsed;

  simplex_list_init(&to_collapse);
  complex_collapsible_facets(c, s, &to_collapse);

  if (simplex_list_size(&to_collapse) == 0) {// no more faces to collapse
    simplex_list_destroy(&to_collapse);
    return;
  }

  simplex_list_init(&already_collapsed);
  complex_collapse_simplex_rec_helper(c, s, &already_collapsed, to_return);

  simplex_list_destroy(&already_collapsed);
  simplex_list_destroy(&to_collapse);  
}

void complex_collapse_simplex_rec_helper(Complex *c, Simplex *s,
					 SimplexList *already_collapsed,
					 ComplexStack *to_return) {
  SimplexList to_collapse, free_faces;
  SimplexNode *facet, *ff;
  Complex new_cx;

  simplex_list_init(&to_collapse);
  simplex_list_init(&free_faces);

  complex_collapsible_facets(c, s, &to_collapse);
  if (simplex_list_size(&to_collapse) > 0) {
    for (facet = to_collapse.head; facet != NULL; facet = facet->next) {
      complex_free_faces_for_facet(c, &facet->simplex, &free_faces);
      for (ff = free_faces.head; ff != NULL; ff = ff->next) {
	if (!simplex_list_contains(already_collapsed, &ff->simplex)) {
	  complex_init(&new_cx);
	  simplex_list_append(already_collapsed, &ff->simplex);
	  complex_collapse_along(c, &facet->simplex, &ff->simplex,
				 &new_cx);
	  complex_collapse_simplex_rec_helper(&new_cx, s, already_collapsed,
					      to_return);
	  complex_destroy(&new_cx);
	}
      }
      simplex_list_destroy(&free_faces);
    }
  } else { // no faces to collapse, yield the current cx
    c->n_collapses++;
    complex_stack_push(to_return, c);
  }

  simplex_list_destroy(&to_collapse);
  simplex_list_destroy(&free_faces);
}

void complex_collapse_once(Complex *c, ComplexStack *l) {
  SimplexFFNode *sn;

  for (sn = c->facets.head; sn != NULL; sn = sn->next) {
    //complex_collapse_facet(c, &sn->simplex, l);
    complex_collapse_simplex_rec(c, &sn->simplex, l);
  }
}

time_t t_start, t_end;

bool complex_is_collapsible(Complex *c, bool use_ht) {
  ComplexStack stack;
  Hashtable ht;
  Complex cx;
  long long n = 0, leaves = 0;
  int n_collapses_seen[3] = {0, 0, 0};

  assert(c->free_faces_computed);

  complex_init(&cx);
  complex_stack_init(&stack);
  if (use_ht)
    hashtable_init(&ht, HT_BIG, HT_BIG*0.80);

  complex_stack_push(&stack, c);

  t_start = time(0);

  while (complex_stack_size(&stack) > 0) {
    /*    for profiling, etc.:   
    if (n > 100000) {
      return False; 
      } */

    complex_stack_pop(&stack, &cx);
    if (!(n++ % 500000))
      print_status(n, leaves, &stack, &ht, use_ht, n_collapses_seen);

    switch (cx.n_collapses) {
    case 1: n_collapses_seen[0]++; break;
    case 2: n_collapses_seen[1]++; break;
    case 3: n_collapses_seen[2]++; break;
    default: break;
    }

    if (complex_has_free_faces(&cx)) {
      /* if there are faces to collapse, see if we've already seen 
	 this complex.  if not, collapse. */
      if (!use_ht || !hashtable_lookup(&ht, &cx, True))
	complex_collapse_once(&cx, &stack);
    } else { // no free faces, see if it's small
      leaves++;
      if (complex_size(&cx) < 3) {
	print_status(n, leaves, &stack, &ht, use_ht, n_collapses_seen);
	printf("collapsed cx: %s\n", cx.serialized);
	complex_destroy(&cx);
	complex_stack_destroy(&stack);
	return True;
      }
    }
    complex_destroy(&cx);
  }
  complex_stack_destroy(&stack);
  if (use_ht)
    hashtable_destroy(&ht);
  return False;
}

void print_status(long long n, long long leaves,
		  ComplexStack *stack, Hashtable *ht,
		  bool use_ht, int collapses[3]) {
  int size = complex_stack_size(stack);
  float secs, popped_per_s, n_million = n/1000000.0;

  t_end = time(0);
  
  secs = (float)t_end - (float)t_start;
  if (secs == 0)
    popped_per_s = 0.0;
  else
    popped_per_s = (float)n/secs;

  printf("time %d, ", (int)secs);
  if (n < 1000000) 
    printf("popped %lld from stack, ", n);
  else
    printf("%.01fM from stack, ", n_million);
  
  printf("%lld leaves, %.0f/s\n", leaves, popped_per_s);

  printf("stack has %d elements, collapsed (%d, %d, %d)\n", 
	 size, collapses[0], collapses[1], collapses[2]);
  if (use_ht)
    hashtable_print_stats(ht);
  fflush(stdout);
}

/*
 * randomly collapse simplices until we can't any more.  return
 * true if we manage to collapse the cx.
 */
bool complex_collapse_random(Complex *cx, History *hist) {
  Complex cx_popped;
  ComplexStack stack;
  Simplex sx;
  int size = complex_size(cx);
  bool result;

  if (!complex_has_free_faces(cx)) {
    //printf("no free faces, got %s\n", cx->serialized);
    if (size < 3)
      return True;
    else
      return False;
  }
  
  complex_init(&cx_popped);
  complex_stack_init(&stack);
  complex_get_random_simplex(cx, &sx);
  history_append(hist, cx, &sx);
  complex_collapse_facet(cx, &sx, &stack);

  complex_stack_pop_random(&stack, &cx_popped);
  complex_stack_destroy(&stack);

  result = complex_collapse_random(&cx_popped, hist);
  complex_destroy(&cx_popped);

  return result;
}

/*
 * return a random simplex (one THAT HAS FREE FACES) from 
 * a complex
 */
void complex_get_random_simplex(Complex *c, Simplex *ret) {
  int size = complex_size(c);
  int which;
  bool has_free_faces = False;
  SimplexFFNode *n;

  assert(complex_has_free_faces(c)); // otherwise we're in bad shape

  while (!has_free_faces) {
    n = c->facets.head;
    which = random_int(0, size);
    while (which--) {
      n = n->next;
    }
    has_free_faces = simplex_list_size(&n->free_faces) > 0;
  }
  simplex_copy(ret, &n->simplex);
}

void complex_stack_pop_random(ComplexStack *stack, Complex *ret) {
  int size = complex_stack_size(stack);
  int which = random_int(0, size);
  Complex cx;

  complex_init(&cx);
  do {
    complex_destroy(&cx);
    complex_stack_pop(stack, &cx);
  } while (which--);
  
  complex_copy(ret, &cx);
  complex_destroy(&cx);
}
