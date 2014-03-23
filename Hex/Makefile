#Makefile for Program 5

GPP          = g++ -g -O3

EXECBIN      = program5
CPPSOURCE    = main.cpp Graph.cpp DisjointSet.cpp Hex.cpp
CPPHEADER    = Graph.h DisjointSet.h Hex.h
OBJECTS      = ${CPPSOURCE:.cpp=.o}

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GPP} -o $@ ${OBJECTS}

%.o : %.cpp
	${GPP} -c $<

clean :
	- rm ${OBJECTS}
