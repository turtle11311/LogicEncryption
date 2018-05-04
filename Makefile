TARGET = le
CC = g++
ABC	= ./alanmi-abc-906cecc894b2

SRCS = main.cpp ReadBench.cpp Simulate.cpp CorruptionCompute.cpp
OBJS = ${SRCS:.c=.o}
LIB = -lm -ldl -rdynamic -lreadline -pthread -std=c++11 -ltermcap ${ABC}/libabc.a
INCLUDE = -I. -I${ABC}/src
CFLAGS  = -O3 -Wall
${TARGET}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${LIB} ${LIBS} ${INCLUDE} -o ${TARGET}

.SUFFIXES: .c .o
.SUFFIXES: .cpp .o

.c.o:
	${CC} ${CFLAGS} ${INCLUDE} -o $@ -c $<
	
.cpp.o:
	${CC} ${CFLAGS} ${INCLUDE} -o $@ -c $<

clean:
	rm -f *~ $(TARGET);
