ASC, Software For Collapsing Abstract Simplicial Complexes
==========================================================

ASC was written by Sean Bowman in 2006.  It contains code for storing
and working with [abstract simplicial
complexes](http://en.wikipedia.org/wiki/Abstract_simplicial_complex),
especially implementing Whitehead's [simplicial
collapsing](http://en.wikipedia.org/wiki/Collapse_%28topology%29).  ASC was written to investigate the Zeeman conjecture.  Sadly, all I really found out is that collapsing simplicial complexes takes a lot of time.

There are some tests in asc_test.c, and the main program reads a simplicial complex and tries to collapse it to something small.  The input format is a text file containing a list of simplicies, one on each line.  Vertices are represented by integers, and the current implementation only allows simplices of dimension three and below.  For example, you can specify a triangle with

    0 1
    1 2
    0 2

The main program uses depth first search to try to collapse the given complex to a point.  The program main_rand collapses simplices with free faces randomly.

This program is licensed under the GPL.
