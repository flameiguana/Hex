#include <vector>
#include <list>
#include <iostream>
#include <string>
#include "Graph.h"

//Implementation explanations in Hex.cpp.
class Hex{
public:
	Hex(int rows = 11, bool pieRule = true);
	Hex(const Hex& other);
	int getSize(){return rows;}
	bool markBoard(int playerNumber, int i, int j);
	void computerMove(int playerNumber);
	void printBoard();
	bool checkWin(int playerNumber, bool printWinner = true);
	void computerMoveMC(int playerNumber, int simulations);
	~Hex();
private:
	class Tile;
	bool pieRule;
	//The internal data structure for the board tiles, since my 
	//graph class doesn't allow for template variables.
	std::vector<Tile*> boardTiles;

	Graph* board;
	int boardSize;
	int previousMove;
	int rows;
	int columns; //same as above
	int turn; //determines whose player's turn it is in order to check legality of move.
	static const int PADDING = 2; //Extra rows and columns
	//This function takes coordinates and maps them onto unique indices
	//of a 1d array (vector)
	int evaluateTile(int index, int playerNumber);
	int map(int i, int j){ return i * (rows + PADDING) + j;}
	//Reverse the mapping
	int unmapi(int index){ return index / (rows + PADDING);}
	int unmapj(int index){ return index % (rows + PADDING);}
};
