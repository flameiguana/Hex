GPP          = g++ -g -O3

EXECBIN      = hex
CPPSOURCE    = main.cpp Graph.cpp DisjointSet.cpp Hex.cpp
CPPHEADER    = Graph.h DisjointSet.h Hex.h
OBJECTS      = ${CPPSOURCE:.cpp=.o}

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GPP} -o $@ ${OBJECTS}

%{OBJECTS} : %.cpp
	${GPP} -c $<

clean :
	- rm ${OBJECTS}
