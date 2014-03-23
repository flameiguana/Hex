#include "DisjointSet.h"

/* Very simple node class that is only here to hold indices into the vector of nodes..*/
class DisjointSet::SimpleNode{
public:
	int id;
	int headID; //lets us quickly access the head node.
	SimpleNode(int id, int headID):id(id),headID(headID)
		{}
};

DisjointSet::DisjointSet(int size)
{
	sets.resize(size);
}

//First create a node with the given ID
//Then create a new list with the given node as the head /"representative".
void DisjointSet::makeSet(int nodeID)
{
	SimpleNode* node = new SimpleNode(nodeID, nodeID);
	elementIndex = nodeIDs.begin();
	elementIndex += nodeID;
	nodeIDs.insert(elementIndex, node);

	std::list<SimpleNode*>* newSet = new std::list<SimpleNode*>();
	newSet->push_back(node);
	setIndex = sets.begin();
	setIndex += nodeID;
	sets.insert(setIndex, newSet);
}


//returns setid
int DisjointSet::findSet(int nodeID){
	//return set id (which is the same as the head id) of given node
	if(nodeIDs.at(nodeID) == NULL)
		return -1;
	else return (nodeIDs.at(nodeID))->headID;
}

//Join two lists. The shorter one is joined to the larger one.
void DisjointSet::unite(int nodeAID, int nodeBID){

	int headOfA = findSet(nodeAID);
	int headOfB = findSet(nodeBID);

	std::list<SimpleNode*>* setOfA = sets.at(headOfA);
	std::list<SimpleNode*>* setOfB = sets.at(headOfB);

	std::list<SimpleNode*>::const_iterator iterator;
	if(setOfA->size() < setOfB->size()){ //update set a
		for (iterator = setOfA->begin(); iterator != setOfA->end(); ++iterator) {
			(*iterator)->headID = headOfB;
		}
		//merge list a to list b. setOfA is now empty
		setOfB->merge(*setOfA);
	}
	else{
	 //update set b
		for (iterator = setOfB->begin(); iterator != setOfB->end(); ++iterator) {
			(*iterator)->headID = headOfA;
		}
		//merge list b to list a. setOfB is now empty
		setOfA->merge(*setOfB);
	}
}

//Didn't have time to implement.
DisjointSet::~DisjointSet(){}
