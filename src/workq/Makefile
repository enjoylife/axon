CFLAGS= -pthread -g -w1 -Wextra$(DEBUGFLAGS)
CC=gcc

SOURCES= workq_main.c
PROGRAMS=$(SOURCES:.c=)
all:	${PROGRAMS}
workq_main: workq.h workq.c workq_main.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ workq_main.c workq.c
clean:
	@rm -rf $(PROGRAMS) *.o
recompile:	clean all
