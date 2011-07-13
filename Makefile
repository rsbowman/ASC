CC = gcc
CFLAGS = -Wall -Werror -march=core2 -g -D_GNU_SOURCE -O3
SOURCE = common.c asc.c simplex.c simplex_ff.c collapse.c ht.c input.c history.c

% : %.c 
	${CC} ${CFLAGS} $^ -o $@

all : asc_test main main_rand

clean:
	rm -f main test *.log *.o *~ nohup.out 

test: asc_test
	./asc_test


main	: ${SOURCE}
asc_test: ${SOURCE}
main_rand: ${SOURCE}
