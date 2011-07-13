#ifndef COMMON_H
#define COMMON_H

typedef enum bool {
  False = 0,
  True = 1
} bool;

typedef unsigned int hashtype;

#define INT_TO_LETTER(x) (((char)(x)+'a'))

int random_int(int a, int b);

#endif
