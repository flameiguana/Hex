#include "Graph.h"

class Graph::Edge{
public:
	int source;
	//The vertex index that this edge leads to.
	int destination;
	//This is the distance between two vertices, aka weight.
	float distance;
	EdgeKey key;
	Edge(int source, int destination, float distance, EdgeKey key):source(source),destination(destination),distance(distance),key(key)
	{}
};

class Graph::Vertex{
public:
	//Should probably make this a list, since we are now erasing.
	std::vector<Edge> outEdges;
	bool visited;
	int edgesFromSource;
	int index;
	float distFromSource;
	int parent;
	int totalNeighbors;

	Vertex(int index):visited(false),index(index),distFromSource(INFINITE_D), parent(NIL), totalNeighbors(0){}
	void addEdge(int to, float distance, EdgeKey key){
		Edge temp(index, to, distance, key);
		outEdges.push_back(temp);
		totalNeighbors++;
	}
	std::vector<int> getNeighbors(){
		std::vector<int> neighborIndices;
		for(int i = 0; i < totalNeighbors; i++){
			neighborIndices.push_back(outEdges.at(i).destination);
		}
		return neighborIndices;
	}

	float updateEdge(int to, float distance, EdgeKey key){
		float oldDistance;
		int i = 0;
		while(i < totalNeighbors){
			if(outEdges.at(i).destination == to){
				oldDistance = outEdges.at(i).distance;
				outEdges.at(i).distance = distance;
				outEdges.at(i).key = key;
				break;
			}
			i++;
		}
		if(i > totalNeighbors)
			return 0.0f; //edge not found.
		return oldDistance;
	}

	//Returns weight of removed edge. Undefined behaviour when edge doesnt exist.
	float removeEdge(int to){
		float distance;
		int i = 0;
		while(i < totalNeighbors){
			if(outEdges.at(i).destination == to){
				distance = outEdges.at(i).distance;
				break;
			}
			i++;
		}
		if(i > totalNeighbors)
			return 0.0f;
		outEdges.erase(outEdges.begin() + i);
		totalNeighbors--;
		return distance;
	}

	void print(){
		for(std::size_t i = 0; i < outEdges.size(); i++){
			std::cout << " " << outEdges.at(i).key << "[" << outEdges.at(i).distance << "]" << outEdges.at(i).destination;
			if(i < outEdges.size() -1)
				std::cout << ",";
		}
		std::cout << std::endl;
	}
};

Graph::Graph(int order):order(order),totalWeight(0.0)
{
	if(order < 0){
		std::cerr << "Error: Out of range." << std::endl;
		exit(1);
	}
	//Resize vector to a specific memory size, for efficiency.
	vertexList.reserve(order);
	for(int i = 0; i < order; i++){
		//Vertex temp(i);
		vertexList.push_back(new Vertex(i));
	}
}


//Assumes the format is proper.
Graph::Graph(const std::string &graphFilename):totalWeight(0.0){
	std::ifstream file;
	file.open(graphFilename.c_str());
	if(!file.is_open())
		exit(1);
	file>>order;
	vertexList.reserve(order);
	for(int i = 0; i < order; i++){
		vertexList.push_back(new Vertex(i));
	}

	int u, v, keyInt;
	float cost;
	while(!file.eof()){
		file>>u;
		file>>v;
		file>>cost;
		file>> keyInt;
		addEdge(u, v, (EdgeKey)keyInt, cost);
	}
	file.close();
}

Graph::Graph(const Graph& other){
	order = other.order;
	totalWeight = other.totalWeight;
	source = other.source;
	vertexList.reserve(order);
	for(std::size_t i = 0;  i <  other.vertexList.size(); i++)
		vertexList.push_back(new Vertex(*other.vertexList.at(i)));
}


void Graph::addEdge(int from, int to, EdgeKey key, float distance){
	if(from > order || to > order || from < 0 || to < 0){
		std::cerr << "Error: Out of range." << std::endl;
		exit(1);
	}
	totalWeight += distance;
	//Connect origin to destination.
	(*vertexList.at(from)).addEdge(to, distance, key);

	//Connnect destination to origin.
	(*vertexList.at(to)).addEdge(from, distance, key);
}

void Graph::updateEdge(int from, int to, EdgeKey key, float distance){
	totalWeight += distance;
	totalWeight = totalWeight - vertexList.at(from)->updateEdge(to, distance, key);
	vertexList.at(to)->updateEdge(from, distance, key);
}

void Graph::removeEdge(int from, int to){
	//Update total weight and remove edge.
	totalWeight = totalWeight - vertexList.at(from)->removeEdge(to);
	vertexList.at(to)->removeEdge(from);
}

void Graph::addArc(int from, int to, float distance, EdgeKey key){
	if(from > order || to > order || from < 0 || to < 0){
		std::cerr << "Error: Out of range." << std::endl;
		exit(1);
	}
	totalWeight += distance; //not so sure about this.
	(*vertexList.at(from)).addEdge(to, distance, key);
}

std::vector<int> Graph::getNeighbors(int from){
	return vertexList.at(from)->getNeighbors();
}
/*The element with highest distance is considered to be a lesser value, so that decreasing
order search actually puts them in increasing order*/
bool Graph::compareVertices(const Vertex *a, const Vertex *b){
	float distA = (*a).distFromSource;
	float distB = (*b).distFromSource;
	//Must be strict inequality. Passsing this tells us that at most one of them is INFINITE_D.
	if(distA == distB)
		return false; 
	else if(distA == INFINITE_D)
		return true;
	else if(distB = INFINITE_D)
		return false;
	else return distA > distB;
}

bool Graph::compareEdges(const Edge *a, const Edge *b){
	float distA = (*a).distance;
	float distB = (*b).distance;
	//Must be strict inequality. Passsing this tells us that at most one of them is INFINITE_D.
	return distA < distB;
}

//Resets vertices to default state (NIL parent and infinite distance from source).
void Graph::resetVertices(){
	for(int i = 0; i < order; i++){
		(*vertexList.at(i)).distFromSource = INFINITE_D;
		(*vertexList.at(i)).parent = NIL;
		vertexList.at(i)->visited = false;
	}
}

//Updates estimated distanceFromSource if the new total distance connecting u and v is shorter.
void Graph::relaxVertex(Vertex* u, Vertex* v, float distance){
	float newDistance = (*u).distFromSource + distance;
	//Always update when v is INFINITE_D. We don't do a direct comparison because -1 messes things up.
	if((*v).distFromSource == INFINITE_D)  {
		(*v).distFromSource = newDistance;
		(*v).parent = (*u).index;
	} 
	//Now make sure U does not have distance of INFINITE_D before comparing.
	else if((*u).distFromSource != INFINITE_D && newDistance < (*v).distFromSource){
		(*v).distFromSource = newDistance;
		(*v).parent = (*u).index;
	}
}


/*Runs Dijkstra's Algorithmn. All it does is modify properties of all vertices, giving them a parent,
and setting distance from source so that the shortest possible paths are found. */

//For hex game, need to use edge keys as neighbors, so that they can be specified in Dijkstra's
//TODO: Incorrect inplementation. It isn't used in this project so fixing is not urgent.
void Graph::calculateShortestPaths(int sourceVertex, EdgeKey keyA, EdgeKey keyB){
	resetVertices();  
	bool ignorekeys = false;
	if(keyA == keyB){
		ignorekeys = true;
	}
	//A queue of vertex pointers. Decided not to use priority_queue because it is unclear about when it uses comparator.
	std::vector<Vertex*> Q;
	source = sourceVertex;
	//Set distance of source vertex to zero.
	(*vertexList.at(source)).distFromSource = 0;
	//TODO: This is wrong. Should only push back the source node, and find every node from, there. This way isolated nodes
	//wont be included. 
	for(int i = 0; i < order; i++){
		Q.push_back(vertexList.at(i)); //Push on back of vector for efficiency. 
	}
	//Sort using custom function. Pointers to vertices with shorter distance from source will end up at the back of vector (aka decreasing weight).
	std::sort(Q.begin(), Q.end(), Graph::compareVertices);
	while(!Q.empty()){
		//Pop last element.
		Vertex u = *vertexList.at((*Q.back()).index);
		Q.pop_back();
		for(int i = 0; i < u.totalNeighbors; i++){
			Edge v = (u).outEdges.at(i);
			//Skip vertices of different key if a key was specified.
			if(ignorekeys|| v.key == keyA || v.key == keyB){
				//std::cout << v.key << " ";
				//The Vertex pointers in both vectors (Graph::vertexList and Q) have same value.
				relaxVertex(vertexList.at(u.index), vertexList.at(v.destination), v.distance);
			}
		}
		std::sort(Q.begin(), Q.end(), Graph::compareVertices);
	}
}


//Does not take into consideration edgeWeights.
//Shortest path is based on number of edges from source.
void Graph::performBFS(int sourceID, EdgeKey key){
	resetVertices();
	source = sourceID;
	std::queue<Vertex*> Q;
	Vertex* sourceVertex = vertexList.at(source);
	sourceVertex->visited = true;
	sourceVertex->edgesFromSource = 0;
	Q.push(sourceVertex);
	while(!Q.empty()){
		Vertex* u = Q.front();
		Q.pop();
		for(int i = 0; i < u->totalNeighbors; i++){
			Edge uv = u->outEdges.at(i);
			if(uv.key != key)
				continue;
			Vertex* v = vertexList.at(uv.destination);
			if(!v->visited){
				v->edgesFromSource = u->edgesFromSource + 1;
				v->parent = u->index;
				v->visited = true;
				Q.push(v);
			}
		}
	}
}

//Recursive helper function for print path
bool Graph::buildPath(std::vector<int> &list, int dest)
{
	//nieed to check for valid input
	//int i = 0;
	Vertex* v;
	while(dest != NIL){
		v = vertexList.at(dest);
		list.push_back(dest);
		if(dest == source){
			std::reverse(list.begin(), list.end());
			return true;
		}
		dest = v->parent;
		//std::cout << ++i << std::endl; //Was beig used for loop detection.
	}
	return false;
}

void Graph::printPath(int destination){
	if(source == NIL){
		std::cout << "Error: Source node not specified. Run Dijkstra's to set source node." << std::endl;
		exit(1);
	}
	std::vector<int> list;
	if(buildPath(list, destination) == false)
		std::cout << "Path not found." << std::endl;
	else{
		for(std::size_t i = 0; i < list.size(); i++){
			std::cout << list.at(i) << " ";
		}
		std::cout << std::endl;
	}
}

float Graph::averagePathLength(int sourceVertex, EdgeKey keyA, EdgeKey keyB){
	if(sourceVertex > order || sourceVertex < 0){
		std::cout << "Error: Out of range." << std::endl;
		exit(1);
	}
	calculateShortestPaths(sourceVertex, keyA, keyB);
	float totalLength = 0.0;
	float n = (float)order;
	for(int i = 1; i < order; i++){
		float distFromSource = (*vertexList.at(i)).distFromSource;
		if(distFromSource != INFINITE_D)
			totalLength += (float)distFromSource;
		else
			n--; //discard vertices that don't connect to source.
	}
	return totalLength/(n - 1.0f);
}

//Kruskal's algorithmn. The assignment called for printng stuff, so it is void for now
void Graph::MST(EdgeKey a, EdgeKey b){
	std::vector<Edge*> edgeList; //The index will be from, it will hold an int called to.
	bool ignorekeys = false;
	if(a == b){
		ignorekeys = true;
	}
	Graph minimum(order);
	DisjointSet set(order);
	for(std::size_t u = 0; u < vertexList.size(); u++){
		set.makeSet(u);
		//create a single vector of all edges.
		for(std::size_t j = 0; j < vertexList.at(u)->outEdges.size(); j++){
			edgeList.push_back(&vertexList.at(u)->outEdges.at(j));
		}
	}

	//sort every edge
	std::sort(edgeList.begin(), edgeList.end(), Graph::compareEdges);

	for(std::size_t i = 0; i < edgeList.size(); i++){
		int u = (*edgeList.at(i)).source;
		int v = (*edgeList.at(i)).destination;
		EdgeKey key = (*edgeList.at(i)).key;
		float distance = (*edgeList.at(i)).distance; //a.k.a edge weight
		//Only add nodes with the same key edges, and those that are in different sets.
		if((ignorekeys || key == a || key == b) && set.findSet(u) != set.findSet(v)){ 
			minimum.addArc(u, v, distance, key);
			set.unite(u, v);
		}
	}

	if(minimum.totalWeight == 0.0){ //print negative one
		std::cout << -1 << std::endl;
	}
	else{
		minimum.print();
		std::cout << "Total Cost of MST: " << minimum.totalWeight << std::endl;
	}
}

void Graph::print(){
	for (std::size_t i = 0; i < vertexList.size(); i++) {
		std::cout << i << ": ";
		(*vertexList.at(i)).print();
	}
}

//Explicitly delete vertices.
Graph::~Graph()
{
	for(int i = 0; i < order; i++)
		delete vertexList.at(i);
}

