#pragma once
#include <vector>
#include <list>
#include <cstddef>

/*
Uses lists as its internal representation.
*/
namespace adt{
	class DisjointSet
	{
	public:

		DisjointSet(int size);

		void makeSet(int nodeID);//O(1)

		int findSet(int nodeID); //O(1)
		void unite(int repA, int repB);

		~DisjointSet();

	private:
		class SimpleNode;
		std::vector<std::list<SimpleNode*>*> sets;
		std::vector<SimpleNode*>::iterator elementIndex;
		std::vector<std::list<SimpleNode*>*>::iterator setIndex;
		//This is used so that we don't have to pass back a node to the client. They only need the node ID.
		std::vector<SimpleNode*> nodeIDs;
	};
}
