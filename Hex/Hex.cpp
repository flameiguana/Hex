#include "Hex.h"


/*
	A simple tile class that has a method for printing depending on the player number
	value of the tile.
*/
class Hex::Tile{
public:
	TileType type;
	int i, j;

	Tile():type(Empty),i(0),j(0)
	{};

	Tile(int i, int j, int rows):i(i),j(j)
	{
		int columns = rows;
		bool left_right = false;
		bool top_bottom = false;
		type = Empty;

		//TODO: move this logic somewhere else and pass tiletype to cosntructor
		//Assign a tile type based on position
		if(i < (columns + PADDING) && (j == 0 ||j == (rows + PADDING - 1))){
			type = O;
			top_bottom = true;
		}
		if(j < (rows + PADDING) && (i == 0 || i == (columns + PADDING - 1 ))){
			type = X;
			left_right = true;
		}
		//This tyle doesnt have a type
		if(left_right && top_bottom){
			type = Invalid;
		}
	}
	//TODO: move this outside of class
	char getPlayerChar() const{
		switch(type){
			case Empty:
				return ' ';
			case X:
				return 'X';
			case O:
				return 'O';
			case Invalid:
				return '#';
		}
	}
};


/*
* Creates all initial connections in underlying graph representation. The edge types
* are initially Enpty which means there is no special O or X connection.
*
* From left to right, to to bottom, edges are connected like this:
*
*
*                       One (i + 1, j -1)
*                     /\ / 
*                    /  \
*                    |  | -- Two (i + 1, j)
*                    \  /
*                     \/ \
*                        Three (i, j + 1)
*                    
* Where
* i = column
* j = row
* This guarantees that every playable tile has six connections.
* The special edge tiles always coonect to each other.
*/

Hex::Hex(int rows, bool pieRule):rows(rows),columns(rows),turn(0),pieRule(pieRule){
	boardSize = (rows + PADDING) * (columns + PADDING);
	board = new Graph(boardSize);
	boardTiles.resize(boardSize);
	for(int i = 0; i < columns + PADDING; i++){ //Don't try to connect past these nodes
		for(int j = 0; j < rows + PADDING; j++){
			//Connect to tile below this one and to tile to the right.
			int mapping = map(i,j);
			if(j > 0  && i < columns + PADDING - 1)
				board->addEdge(mapping, map(i + 1, j - 1), (Graph::EdgeKey)Empty); //One
			if(i < columns + PADDING - 1)
				board->addEdge(mapping, map(i + 1, j), (Graph::EdgeKey)Empty); //Two
			if(j < rows + PADDING - 1)
				board->addEdge(mapping, map(i, j + 1), (Graph::EdgeKey)Empty); //Three
			//Instead I create an array of tiles.
			boardTiles.insert(boardTiles.begin() + mapping, Tile(i, j, rows));
		}
	}
	//Tiles are now connected, It is time to create special connections.
	//Make top and bottom rows red, left and right rows blue.
	int lastIndex = columns + PADDING - 1;
	for(int x = 1; x < rows + PADDING - 2 ; x++){
			//Top and bottom are O.
			board->updateEdge(map(x, 0), map(x + 1, 0), (Graph::EdgeKey)O);
			board->updateEdge(map(x, lastIndex), map(x + 1, lastIndex), (Graph::EdgeKey)O);
			//Left and right are X.
			board->updateEdge(map(0, x), map(0, x + 1), (Graph::EdgeKey)X);
			board->updateEdge(map(lastIndex, x), map(lastIndex, x + 1),(Graph::EdgeKey)X);
	}
}

//DEEP copy
Hex::Hex(const Hex& other){
	boardSize = other.boardSize;
	board = new Graph(*other.board);
	boardTiles.resize(boardSize);
	//copy other tiles by value
	boardTiles = other.boardTiles;
	pieRule = other.pieRule;
	previousMove = other.previousMove;
	rows = other.rows;
	columns = other.columns; 
	turn = other.turn;
}

/*
Win Conditions:
From any edgeTile, there is a path from this tile to another edgeTile.
There is no path when a tile is of a different color than the player tile (either blank or the other player colors).
*/

bool Hex::checkWin(TileType tileType){
	if(turn < rows) //It is impossible to win at this turn.
		return false;
	std::vector<int> path;

	if(tileType == X){ 
		//Pick the topmost X tile on the left side as the source. Note that any of these nodes would do.
		//Pick the topmost X tile on the right side as the destination.
		board->performBFS(map(0, 1), (Graph::EdgeKey)X); 
		//If there is a path, then blue player (player 1) wins.
		if(board->buildPath(path, map(columns + PADDING - 1, 1))){
			/*For debugging:
			for(int x = 0; x < path.size(); x++){
				std::cout << "( " << unmapi(path.at(x)) << ", " << unmapj(path.at(x)) << " )";
			}*/
			return true;
		}
	}
	else if(tileType == O){
		//Similar to above, but pick two O tiles from both sides of board.
		board->performBFS(map(1, 0), (Graph::EdgeKey)O);
		if(board->buildPath(path, map(1, rows + PADDING - 1))){
			/*
			for(int x = 0; x < path.size(); x++){
				std::cout << "( " << unmapi(path.at(x)) << ", " << unmapj(path.at(x)) << " )";
			}*/
			return true;
		}
	} 
	return false;
}

/* 
* This method checks for the validity of a move, and then modifies the board.
* Here is how the edges of the graph are updated:
* 1. Return list of neighbors (as a vector of indices into Graph's vertexlist) of that tile.
* 2. Visit each neighbor tile. If they are of the same color, update edge color (from source to dest and dest to source).
*/
bool Hex::markBoard(TileType tileType, int i, int j){
	//check for valid index
	if(i > columns - 1 || j < 0 ||  j > rows - 1 || j < 0){
		std::cerr << "Invalid Move: Out of Range " << i  << ", " << j << std::endl;
		return false;
	}
	//convert to internal representation (skip top and left sets of nodes)
	i++;
	j++;

	int thisTile = map(i,j);
	Tile& tile = boardTiles.at(thisTile);
	//Check if a player is already there. Take into account pie rule.
	if(tile.type != Empty && !(pieRule && turn == 1)){
		std::cerr << "Invalid Move: Position Already Taken " << i -1 << ", " << j -1<< std::endl;
		return false;
	}
	turn++;
	tile.type = tileType;
	std::vector<int> sorroundingTiles = board->getNeighbors(thisTile);
	std::vector<int>::iterator it;
	for(it = sorroundingTiles.begin(); it != sorroundingTiles.end(); ++it){
		int thatTile = *it;
		if(tileType == boardTiles.at(thatTile).type){
			board->updateEdge(thisTile, thatTile, (Graph::EdgeKey)tileType, 0.0f);
		}
	}
	previousMove = thisTile;
	return true;
}

/* 
Used by my custom AI function (not Monte Carlo)
One point if this tile is playable (not occupied)
Two points if this tile connects to one of my own tiles
One point if it connects to the other player's tile (potential block)
*/

int Hex::evaluateTile(int index, TileType tileType){
	int value = -1;

	Tile& tile = boardTiles.at(index);
	if(tile.type == Empty){
		value++;
		std::vector<int> neighbors = board->getNeighbors(index);
		std::vector<int>::iterator it;
		for(it = neighbors.begin(); it != neighbors.end(); ++it){
			Tile& neighborTile = boardTiles.at(*it);
			if(neighborTile.type == tileType)
				value += 3;
			else if(neighborTile.type != Invalid && neighborTile.type != Empty)
				value++; //there will always be one
		}
	}
	return value;
}
/*My own stab at making an AI that doesn't use brute force
The AI makes a move. For now just be a douche and try to block opponent' move.
Strategy:
	Look at other players previous move and its sorrounding tiles.
	Value sorrounding tiles by:
		Whether they will connect my own tiles.
		Whether they block the other player's bridges.
		have an array of playable tile indices
		have an array of value for each index */
void Hex::computerMove(TileType tileType){
	//if AI goes first.
	if(turn == 0){
		//put somewhere random
		if(pieRule)
			markBoard(tileType, rand()%(rows), rand()%(rows));
		else
			markBoard(tileType, rows/2, columns/2); //Middle is the best
		return;
	}
	if(turn == 1 && pieRule){	
		markBoard(tileType, rows/2, columns/2); //Middle is the best
		return;
	}

	std::vector<int> sorroundingTiles = board->getNeighbors(previousMove);
	//originally tiles are worth nothing.
	const int MAX_TILES = 6;
	std::vector<int> tileValues = std::vector<int>(MAX_TILES, -1);
	for(int i = 0; i < MAX_TILES; i++)
	{
		tileValues.at(i) = evaluateTile(sorroundingTiles.at(i), tileType);
	}
	//get index of max elment.
	std::vector<int>::iterator it = std::max_element(tileValues.begin(), tileValues.end());
	int vectorIndex = std::distance(tileValues.begin(), it);
	int maxValue = *it;
	//No playable tile was found. Find first empty tile. Ideally would want next best tile.
	if (maxValue == -1){
		std::vector<Tile>::iterator it;
		for(it = boardTiles.begin(); it != boardTiles.end(); ++it){
			if((*it).type == Empty){
				markBoard(tileType, (*it).i - 1, (*it).j - 1);
				return;
			}
		}
	}

	else {
		int bestTileIndex = sorroundingTiles.at(vectorIndex);
		markBoard(tileType, unmapi(bestTileIndex) - 1, unmapj(bestTileIndex) - 1);
	}
}


/*
	Make as a move based on MonteCarlo simulations. The more simulations the better the sample size.
	Steps:
	Copy current board.
	For desired iterations, shuffle empty tiles and place tiles in alternating order. This
	behaves as if the opponent is making random moves.
	Check who won, give a point to first tile if it wins.
	Find the tile with the highest win percentage and use that as the next real move.
*/

void Hex::computerMoveMC(TileType tileType, int simulations){
	std::vector<float> timesWon(boardSize, 0.0f);
	std::vector<float> timesPlayed(boardSize, 0.0f);

	//Find which tiles are empty.
	std::vector<int> emptyTiles;
	for(int i = 0; i < boardSize; i++){
		if(boardTiles.at(i).type == Empty)
			emptyTiles.push_back(i);
	}

	//run fake games for the desired number of simulations.
	for(int simID = 0; simID < simulations; simID++){
		Hex boardCopy(*this);
		std::random_shuffle(emptyTiles.begin(), emptyTiles.end());
		int firstMove = emptyTiles.front();
		timesPlayed.at(firstMove) = timesPlayed.at(firstMove) + 1;

		//simulates switching players
		int playerMove = (int)tileType;
		int otherPlayer = playerMove == 1 ? 2 : 1;

		std::vector<int>::iterator i;
		for(i = emptyTiles.begin(); i != emptyTiles.end(); i++){
			boardCopy.markBoard((TileType)playerMove, unmapi(*i) - 1, unmapj(*i) - 1);
			//swap player colors
			playerMove = playerMove == 1 ? 2 : 1;
		}
		if(boardCopy.checkWin(tileType))
			timesWon.at(firstMove) = timesWon.at(firstMove) + 1.0f;
	}

	int bestTileIndex;
	float bestWinPercentage = 0.0f;
	//winPercentage = times tile won / times tile was Played
	for(int i = 0; i < boardSize; i++){
		if(timesPlayed.at(i) != 0.0f){
			float winPercentage = timesWon.at(i) / timesPlayed.at(i);
			if(winPercentage > bestWinPercentage){
				bestTileIndex = i;
				bestWinPercentage = winPercentage;
			}
		}
	}

	if(bestWinPercentage == 0.0f){
		computerMove(tileType); // Use the other AI
	}
	else
		markBoard(tileType, unmapi(bestTileIndex) - 1, unmapj(bestTileIndex) - 1);
}

//Anonymous namespace to hide implementation details (as opposed to using private static methods)
namespace {
	//The top part of a row of tiles, along with indices.
	void printTop(std::ostream& out, int count, int spacing){
		//Print some spaces
		out << std::string(spacing, ' ');
		//Don't want to index the first or last columns.
		out << std::string(4, ' ');
		for(int i = 0; i < count - 2; i++){
			out << "  " << i <<" ";
		}
		out << std::endl;
		//Print the triangle shape.
		out << std::string(spacing, ' ');
		for(int i = 0; i < count; i++){
			out << " / \\";
		}
		out << std::endl;
	}

	//The bottom part of a row of tiles.
	void printBottom(std::ostream& out, int count, bool extra = true){
		for(int i = 0; i < count; i++)
			out << " \\ /";
		if(extra)
			out << " \\";
		out << std::endl;
	}
}


//Prints a fancy ascii board.
void Hex::printBoard(std::ostream& out) const{
	out << std::endl;
	int spacing = 1; //used for indenting the board

	printTop(out, rows + PADDING, spacing);

	for(int j = 0; j < (columns + PADDING); j++){
		int removeSpacing = 1;
		if(j > 9)
			removeSpacing = 2;

		out << std::string(spacing - removeSpacing, ' '); //Substitute a space with row index.
		if(j > 0 && j < rows + PADDING - 1) //Ignore first and last row.
			out << j - 1 << "| ";
		else 
			out << " | ";

		//Print the actual values in the tiles.
		for(int i = 0; i < (rows + PADDING); i++){
			const Tile& tile = boardTiles.at(map(i,j));
			out << tile.getPlayerChar();
			out << " | ";
		}
		out << std::endl;
		//The bottom part serves as the top of the next row.
		out << std::string(spacing, ' ');
		if(j < columns + PADDING - 1)
			printBottom(out, rows + PADDING);
		else
			printBottom(out, rows + PADDING, false); //dont print extra dash

		spacing += 2;
	}
	//board->print(); prints the graph representation
}

std::ostream& operator<<(std::ostream& out, const Hex& board)
{
	board.printBoard(out);
	return out;
}

//Apply delete on all pointers.
Hex::~Hex(){
	delete board;
}
