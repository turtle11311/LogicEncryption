TARGET = le
CC = g++
ABC	= ./alanmi-abc-906cecc894b2

SRCS = main.cpp ReadBench.cpp Simulate.cpp CorruptionCompute.cpp mpfr_mul_d.cpp gmpfrxx.cpp
OBJS = ${SRCS:.cpp=.o}
LIB = -lm -ldl -rdynamic -lreadline -pthread -ltermcap -lgmp -lmpfr ${ABC}/libabc.a
INCLUDE = -I. -I${ABC}/src
CXXFLAGS  = -O3 -Wall -std=c++11
${TARGET}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${LIB} ${LIBS} ${INCLUDE} -o ${TARGET}

.SUFFIXES: .c .o
.SUFFIXES: .cpp .o

.c.o:
	${CC} ${CFLAGS} ${INCLUDE} -o $@ -c $<
	
.cpp.o:
	${CC} ${CXXFLAGS} ${INCLUDE} -o $@ -c $<

clean:
	rm -f *~ $(TARGET);
	rm -rf *.o
