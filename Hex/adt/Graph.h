#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <list>
#include <string>
#include <algorithm> 
#include <limits>

#include "DisjointSet.h"

namespace adt{
	class Graph
	{
	public:
		typedef int EdgeKey; //similar to assigning a color to an edge
		static const float INFINITE_D; //infinite distance
		static const int NIL = -2;
		//Order equals the total number of vertices.
		explicit Graph(int order); 
		Graph(const Graph& other);
		explicit Graph(const std::string &graphFilename);
		int getOrder() const {return order;}
		int getSource() const {return source;}

		/*Returns the average path length from the source vertex to all other vertices.
		You can specify if you want the key to be taken into account.
		The source vertex is vertex 0 by default.*/
		float averagePathLength(int sourceVertex = 0, EdgeKey keyA = 0, EdgeKey keyB = 0);
		void calculateShortestPaths(int sourceVertex, EdgeKey keyA = 0, EdgeKey keyB = 0);
		//Breadth first search.
		void performBFS(int source, EdgeKey key = 0);
		//Prints out Minimum Spanning Tree of graph, as well as the total weight.
		void MST(EdgeKey a = 0, EdgeKey b = 0);
		void DFS(int node);
		/*Add an edge using integers as indices.*/
		void addEdge(int from, int to, EdgeKey key, float distance = 0.0f);
		void updateEdge(int from, int to, EdgeKey key, float distance = 0.0f);
		void removeEdge(int from, int to);
		std::vector<int> getNeighbors(int from) const; 
		void addArc(int from, int to, float distance, EdgeKey key);
		bool buildPath(std::vector<int> &list, int dest) const;
		void printPath(int destination) const;
		/*Print the graph along with its edge keys.*/
		void print(std::ostream& out) const;
		float getTotalWeight(){return totalWeight;};
		~Graph();

	private:
		class Edge;
		class Vertex; //TODO: allow custom attributes, probably need to use templates.

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
}

/*References
Introduction to Algorithmns
http://en.wikipedia.org/wiki/Kruskal's_algorithma
*/
