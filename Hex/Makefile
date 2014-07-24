GPP          = g++ -O3 -Wall -Wextra

EXECBIN      = hex
CPPSOURCE    = main.cpp adt/Graph.cpp adt/DisjointSet.cpp Hex.cpp
CPPHEADER    = Graph.h DisjointSet.h Hex.h
OBJECTS      = ${CPPSOURCE:.cpp=.o}

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GPP} -o $@ ${OBJECTS}

%{OBJECTS} : %.cpp
	${GPP} -c $<

clean :
	- rm *.o
