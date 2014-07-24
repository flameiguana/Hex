#include <vector>
#include <list>
#include <iostream>
#include <string>
#include "adt/Graph.h"

//Implementation explanations in Hex.cpp.
class Hex{
public:
	//real hex has 13 rows
	explicit Hex(int rows = 11, bool pieRule = true);
	//Since we can't use colors like in real Hex, these are the types of tiles available.
	enum TileType{Empty = 0, X, O, Invalid};
	int getSize() const {return rows;}
	bool markBoard(TileType tileType, int i, int j);
	void computerMove(TileType tileType);
	void printBoard(std::ostream& out) const;
	bool checkWin (TileType tileType); //not const because it modifies underlying graph
	void computerMoveMC(TileType tileType, int simulations);

private:
	//Holds info about a specific tile on board
	struct Tile{
		TileType type;
		int i;
		int j;

		Tile():type(Empty), i(0), j(0)
			{}

		Tile(TileType type, int i, int j):type(type), i(i), j(j)
			{}
	};

	//The internal data structure for the board tiles. If Graph supported templates this probably wouldnt be necessary
	std::vector<Tile> boardTiles;
	static const int PADDING = 2; //Extra rows and columns
	const int rows;
	const int columns; //same as above
	int boardSize;
	//The underlying data structure of our board, which manages connections between tiles and treats tiles as vertices.
	adt::Graph board;

	int turn; //determines whose player's turn it is in order to check legality of move.
	bool pieRule;
	int previousMoveIndex;

	int evaluateTile(int index, TileType tileType) const;
	//This function takes coordinates and maps them onto unique indices
	//of a 1d array (vector)
	int map(int i, int j)const { return i * (rows + PADDING) + j;}
	//Reverse the mapping
	int unmapi(int index) const { return index / (rows + PADDING);}
	int unmapj(int index) const { return index % (rows + PADDING);}
	TileType typeFromCoords(int i, int j) const;
};

std::ostream& operator<<(std::ostream& out, const Hex& board);
std::ostream& operator<<(std::ostream& out, const Hex::TileType& tileType);