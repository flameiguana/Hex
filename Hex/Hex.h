#include <vector>
#include <list>
#include <iostream>
#include <string>
#include "Graph.h"

//Implementation explanations in Hex.cpp.
class Hex{
public:
	Hex(int rows = 11, bool pieRule = true); explicit
	Hex(const Hex& other);
	enum TileType{Empty = 0, X, O, Invalid};
	int getSize() const {return rows;}
	bool markBoard(TileType tileType, int i, int j);
	void computerMove(TileType tileType);
	void printBoard(std::ostream& out) const;
	bool checkWin (TileType tileType);
	void computerMoveMC(TileType tileType, int simulations);

	~Hex();
private:
	class Tile;
	bool pieRule;
	//The internal data structure for the board tiles.
	std::vector<Tile> boardTiles;

	Graph* board;
	int boardSize;
	int previousMove;
	int rows;
	int columns; //same as above
	int turn; //determines whose player's turn it is in order to check legality of move.
	static const int PADDING = 2; //Extra rows and columns

	int evaluateTile(int index, TileType tileType);
	//This function takes coordinates and maps them onto unique indices
	//of a 1d array (vector)
	int map(int i, int j)const { return i * (rows + PADDING) + j;}
	//Reverse the mapping
	int unmapi(int index) const { return index / (rows + PADDING);}
	int unmapj(int index) const { return index % (rows + PADDING);}
};

std::ostream& operator<<(std::ostream& out, const Hex& board);