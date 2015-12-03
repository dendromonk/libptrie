CC = cc
AR = ar
CFLAGS = -Os -Wall
ARFLAGS = rcsv

OBJS = ptrie.o
TARGET = libptrie.a

${TARGET}: ${OBJS}
	${AR} ${ARFLAGS} $@ ${OBJS}

clean:
	rm -f ${OBJS} ${TARGET}
