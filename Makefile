CC = cc
AR = ar
CFLAGS = -O0 -g -ggdb
ARFLAGS = rcsv

OBJS = ptrie.o
TARGET = libptrie.a

${TARGET}: ${OBJS}
	${AR} ${ARFLAGS} $@ ${OBJS}

clean:
	rm -f ${OBJS} ${TARGET}
