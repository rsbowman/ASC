#ifndef COLLAPSE_H
#define COLLAPSE_H

#include "asc.h"
#include "ht.h"
#include "history.h"

void complex_collapse_facet(Complex *c, Simplex *facet, ComplexStack *l);
void complex_collapse_along(Complex *c, Simplex *facet, Simplex *free_face,
			    Complex *new_complex);

void complex_collapse_once(Complex *c, ComplexStack *l);
bool complex_is_collapsible(Complex *c, bool use_ht);
void complex_collapse_simplex(Complex *c, Simplex *s, 
			      ComplexStack *to_return);

void complex_collapse_simplex_rec(Complex *c, Simplex *s, 
				  ComplexStack *to_return);
void complex_collapse_simplex_rec_helper(Complex *c, Simplex *s,
					 SimplexList *already_collapsed,
					 ComplexStack *to_return);

void print_status(long long n, long long leaves,
		  ComplexStack *stack, Hashtable *ht,
		  bool use_ht, int collapses[3]);

void complex_get_random_simplex(Complex *c, Simplex *ret);
bool complex_collapse_random(Complex *cx, History *hist);
void complex_stack_pop_random(ComplexStack *stack, Complex *ret);

#endif
