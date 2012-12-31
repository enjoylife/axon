#CFLAGS= -lpthread -g  -W 64 $(DEBUGFLAGS)
CFLAGS= -pthread -g  -std=c99 -pedantic  $(DEBUGFLAGS)
CC=gcc
#CC=c99

SOURCES= hazard_test.c JouleQueue_test.c
PROGRAMS=$(SOURCES:.c=)
all:	${PROGRAMS}

hazard_test: hazard.h hazard.c hazard_test.c common.h
	${CC} ${CFLAGS} ${LDFLAGS} -o $@  $^

JouleQueue_test: JouleQueue.h JouleQueue.c JouleQueue_test.c common.h
	${CC} ${CFLAGS} ${LDFLAGS} -o $@  $^

clean:
	@rm -rf $(PROGRAMS) *.o
	@rm -rf  *.dSYM
recompile:	clean all
