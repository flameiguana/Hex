#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <list>
#include <string>
#include <algorithm> 

#include "DisjointSet.h"

#define INFINITE_D 100000 //TODO change to the max value of float.
#define NIL -2


class Graph
{
public:
	typedef enum {RED = 0, GREEN, BLUE, ALL} EdgeColor;
	static std::string edgeColorToString(EdgeColor color);
	//Order equals the total number of vertices.
	Graph(int order);
	Graph(const Graph& other);
	Graph(const std::string &graphFilename);
	int getOrder(){return order;}
	int getSource(){return source;}

	/*Returns the average path length from the source vertex to all other vertices.
	You can specify if you want the color to be taken into account.
	The source vertex is vertex 0 by default.*/
	float averagePathLength(int _source = 0, EdgeColor colorA = ALL, EdgeColor colorB = ALL);
	void calculateShortestPaths(int _source, EdgeColor colorA=ALL, EdgeColor colorB=ALL);
	//Breadth first search.
	void performBFS(int source, EdgeColor color = ALL);
	//Prints out Minimum Spanning Tree of graph, as well as the total weight.
	void MST(EdgeColor a = ALL, EdgeColor b = ALL);
	void DFS(int node);
	/*Add an edge using integers as indices.*/
	void addEdge(int from, int to, float distance, EdgeColor color);
	void updateEdge(int from, int to, float distance, EdgeColor color);
	void removeEdge(int from, int to);
	std::vector<int> getNeighbors(int from);
	void addArc(int from, int to, float distance, EdgeColor color);
	bool buildPath(std::vector<int> &list, int dest); //Returns false
	void printPath(int destination);
	/*Print the graph along with its edge colors.*/
	void print();
	float getTotalWeight(){return totalWeight;};
	~Graph();

private:
	class Edge;
	class Vertex; //allow custom attributes, probably need to use templates.

	int order;
	float totalWeight;
	int source;
	//Switched to pointer because gcc 4.4 doesnt support forward declaring class and using it directly.
	std::vector<Vertex*> vertexList;
	void resetVertices();
	static bool compareVertices(const Vertex *a, const  Vertex *b);
	static bool compareEdges(const Edge *a, const Edge *b);
	void relaxVertex(Vertex* u, Vertex* v, float distance);
};


/*References
Introduction to Algorithmns
http://en.wikipedia.org/wiki/Kruskal's_algorithma
*/
