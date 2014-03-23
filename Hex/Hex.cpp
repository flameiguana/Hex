#include "Hex.h"


/*
	A simple tile class that has a method for printing depending on the player number
	value of the tile.
*/
class Hex::Tile{
public:
	//Default playerNumnber in tile is 0 (blank tile).
	int playerNumber;
	int i, j;
	//bool playable; //is this tile playable?

	Tile(int i, int j, int rows):i(i),j(j)
	{
		int columns = rows;
		bool BLUE = false;
		bool RED = false;
		bool playable = true;
		playerNumber = 0;
		//Makes these tiles red or blue by default
		if(i < (columns + PADDING) && (j == 0 ||j == (rows + PADDING - 1))){
			playerNumber = 2;
			playable = false;
			RED = true;
		}
		if(j < (rows + PADDING) && (i == 0 || i == (columns + PADDING - 1 ))){
			playerNumber = 1;
			playable = false;
			BLUE = true;
		}
		//Color for this tile doesn't matter.
		if(BLUE && RED){
			playerNumber = 3;
			playable = false;
		}
	}
	void printTile(){
		char tileChar = 0;
		switch(playerNumber){
			case 0:
				tileChar = ' ';
				break;
			case 1:
				tileChar = 'X';
				break;
			case 2:
				tileChar = 'O';
				break;
			case 3:
				tileChar = '#';
				break;
		}
		std::cout << tileChar;
	}
};


/*
* Creates all initial connections in underlying graph representation. The edge colors
* are initially GREEN, which means there is no special BLUE or RED connection.
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
				board->addEdge(mapping, map(i + 1, j - 1), 0.0f, Graph::GREEN); //One
			if(i < columns + PADDING - 1)
				board->addEdge(mapping, map(i + 1, j),0.0f,  Graph::GREEN); //Two
			if(j < rows + PADDING - 1)
				board->addEdge(mapping, map(i, j + 1), 0.0f, Graph::GREEN); //Three
			//Instead I create an array of tiles.
			boardTiles[mapping] = new Tile(i, j, rows);
		}
	}
	//Tiles are now connected, It is time to create special connections.
	//Make top and bottom rows red, left and right rows blue.
	int lastIndex = columns + PADDING - 1;
	for(int x = 1; x < rows + PADDING - 2 ; x++){
			//Top and bottom are red.
			board->updateEdge(map(x, 0), map(x + 1, 0), 0.0f, Graph::RED);
			board->updateEdge(map(x, lastIndex), map(x + 1, lastIndex), 0.0f, Graph::RED);
			//Left and right are blue.
			board->updateEdge(map(0, x), map(0, x + 1), 0.0f, Graph::BLUE);
			board->updateEdge(map(lastIndex, x), map(lastIndex, x + 1), 0.0f, Graph::BLUE);
	}
	
}

//DEEP copy
Hex::Hex(const Hex& other){
	boardSize = other.boardSize;
	board = new Graph(*other.board);
	boardTiles.resize(boardSize);
	for(int i = 0;  i <  other.boardTiles.size(); i++)
		boardTiles.at(i) = new Tile(*other.boardTiles.at(i));
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

bool Hex::checkWin(int playerNumber, bool printWinner){
	if(turn < rows) //It is impossible to win at this turn.
		return false;
	std::vector<int> path;
	if(playerNumber == 1){ 
		//Pick the topmost blue tile on the left side as the source. Note that any of these nodes would do.
		board->performBFS(map(0, 1), Graph::BLUE); 
		//Pick the topmost red tile on the right side as the destination.
		//If there is a path, then blue player (player 1) wins.
		if(board->buildPath(path, map(columns + PADDING - 1, 1))){
			/*For debugging:
			for(int x = 0; x < path.size(); x++){
				std::cout << "( " << unmapi(path.at(x)) << ", " << unmapj(path.at(x)) << " )";
			}*/
			if(printWinner){
				std::cout << std::endl;
				std::cout << "Player 1 Wins" << std::endl;
			}
			return true;
		}
		else path.clear();
	}

	else if(playerNumber == 2){
		//Similar to above, but pick two red tiles from both sides of board.
		board->performBFS(map(1, 0), Graph::RED);
		if(board->buildPath(path, map(1, rows + PADDING - 1))){
			/*
			for(int x = 0; x < path.size(); x++){
				std::cout << "( " << unmapi(path.at(x)) << ", " << unmapj(path.at(x)) << " )";
			}*/
			if(printWinner){
				std::cout << std::endl;
				std::cout << "Player 2 Wins" << std::endl;
			}
			return true;
			}
		else path.clear();
	} 
	return false;
}

/* 
* This method checks for the validity of a move, and then modifies the board.
* Here is how the edges of the graph are updated:
* 1. Return list of neighbors (as a vector of indices into Graph's vertexlist) of that tile.
* 2. Visit each neighbor tile. If they are of the same color, update edge color (from source to dest and dest to source).
*/
bool Hex::markBoard(int playerNumber, int i, int j){
	//check for valid index
	if(i > columns - 1 || j < 0 ||  j > rows - 1 || j < 0){
		std::cout << "Invalid Move: Out of Range " << i  << ", " << j << std::endl;
		return false;
	}
	//convert to internal representation (skip top and left sets of nodes)
	i++;
	j++;

	int thisTile = map(i,j);
	Tile* tile = boardTiles.at(thisTile);
	//Check if a player is already there. Take into account pie rule.
	if(tile->playerNumber != 0 && !(pieRule && turn == 1)){
		std::cout << "Invalid Move: Position Already Taken " << i -1 << ", " << j -1<< std::endl;
		return false;
	}
	turn++;
	tile->playerNumber = playerNumber;
	std::vector<int> sorroundingTiles = board->getNeighbors(thisTile);
	for(int i = 0; i < sorroundingTiles.size(); i++){
		int thatTile = sorroundingTiles.at(i);
		if(playerNumber == boardTiles.at(thatTile)->playerNumber){
			Graph::EdgeColor color;
			if(playerNumber == 1)
				color = Graph::BLUE;
			else color =  Graph::RED;
			board->updateEdge(thisTile, thatTile, 0.0f, color);
		}
	}
	previousMove = thisTile;
	return true;
}


//One point if this tile is playable (not occupied)
//Two points if this tile connects to one of my own tiles
//One point if it connects to the other player's tile (potential block)
int Hex::evaluateTile(int index, int playerNumber){
	int value = -1;

	Tile tile = *boardTiles.at(index);
	if(tile.playerNumber == 0){
		value++;
		std::vector<int> neighbors = board->getNeighbors(index);
		for(int i = 0; i < neighbors.size(); i++){
			Tile* neighborTile = boardTiles.at(neighbors.at(i));
			if(neighborTile->playerNumber == playerNumber)
				value += 3;
			else if(neighborTile->playerNumber != 3 && neighborTile->playerNumber != 0)
				value++; //there will always be one
		}
	}
	return value;
}

//The AI makes a move. For now just be a douche and try to block opponent' move.
//Strategy:
//Look at other players previous move and its sorrounding tiles.
//Value sorrounding tiles by:
//Whether they will connect my own tiles.
//Whether they block the other player's bridges.
//have an array of playable tile indices
//have an array of value for each index
void Hex::computerMove(int playerNumber){
	//AI goes first.
	if(turn == 0){
		//put somewhere random
		if(pieRule)
			markBoard(playerNumber, rand()%(rows), rand()%(rows));
		else
			markBoard(playerNumber, rows/2, columns/2); //Middle is the best
		return;
	}
	if(turn == 1 && pieRule){	
		markBoard(playerNumber, rows/2, columns/2); //Middle is the best
		return;
	}

	std::vector<int> sorroundingTiles = board->getNeighbors(previousMove);
	//originally tiles are worth nothing.
	std::vector<int> tileValues = std::vector<int>(6, -1);
	for(int i = 0; i < 6; i++)
	{
		tileValues.at(i) = evaluateTile(sorroundingTiles.at(i), playerNumber);
	}
	//get index of max elment.
	std::vector<int>::iterator it = std::max_element(tileValues.begin(), tileValues.end());
	int vectorIndex = std::distance(tileValues.begin(), it);
	int maxValue = *it;
	//No playable tile was found. Find first empty tile. Ideally would want next best tile.
	if (maxValue == -1){
		for(int i = 0; i < boardTiles.size(); i ++){
			if(boardTiles.at(i)->playerNumber == 0){
				markBoard(playerNumber, boardTiles.at(i)->i - 1, boardTiles.at(i)->j - 1);
				return;
			}
		}
	}

	else {
		int bestTileIndex = sorroundingTiles.at(vectorIndex);
		markBoard(playerNumber, unmapi(bestTileIndex) - 1, unmapj(bestTileIndex) - 1);
	}
}


/*
	Monte Carlo Steps:
	Copy current board.
	For desired iterations, shuffle empty tiles and place tiles in alternating order.
	Check who won, give a point to first tile if it wins.
	Find the tile with the highest win percentage and use that as the next real move.
*/

void Hex::computerMoveMC(int playerNumber, int simulations){
	std::vector<float> timesWon(boardSize, 0.0f);
	std::vector<float> timesPlayed(boardSize, 0.0f);

	//Find which tiles are empty. I tried keeping a list and deleting the tile when a piece
	// was placed there but iterators get invalidated and shit.
	std::vector<int> emptyTiles;
	for(int i = 0; i < boardSize; i++){
		if(boardTiles.at(i)->playerNumber == 0)
			emptyTiles.push_back(i);
	}

	//run fake games for the desired number of simulations.
	for(int simID = 0; simID < simulations; simID++){
		Hex boardCopy = Hex(*this);
		std::random_shuffle(emptyTiles.begin(), emptyTiles.end());
		int firstMove = emptyTiles.front();
		timesPlayed.at(firstMove) = timesPlayed.at(firstMove) + 1;

		//simulates switching players
		int playerMove = playerNumber;
		int otherPlayer = playerMove == 1 ? 2 : 1;

		std::vector<int>::iterator i;
		for(i = emptyTiles.begin(); i != emptyTiles.end(); i++){
			boardCopy.markBoard(playerMove, unmapi(*i) - 1, unmapj(*i) - 1);
			//swap player colors
			playerMove = playerMove == 1 ? 2 : 1;
		}
		if(boardCopy.checkWin(playerNumber, false))
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
		computerMove(playerNumber); // Use the other AI
	}
	else
		markBoard(playerNumber, unmapi(bestTileIndex) - 1, unmapj(bestTileIndex) - 1);
}

//The top part of a row of tiles, along with indices.
void printTop(int count, int spacing){
	//Print some spaces
	std::cout << std::string(spacing, ' ');
	//Don't want to index the first or last columns.
	std::cout << std::string(4, ' ');
	for(int i = 0; i < count - 2; i++){
		std::cout << "  " << i <<" ";
	}
	std::cout << std::endl;
	//Print the triangle shape.
	std::cout << std::string(spacing, ' ');
	for(int i = 0; i < count; i++){
		std::cout << " / \\";
	}
	std::cout << std::endl;
}

//The bottom part of a row of tiles.
void printBottom(int count, bool extra = true){
	for(int i = 0; i < count; i++)
		std::cout << " \\ /";
	if(extra)
		std::cout << " \\";
	std::cout << std::endl;
}

//Prints a fancy ascii board.
void Hex::printBoard(){
	std::cout << std::endl;
	int spacing = 1; //used for indenting the board

	printTop(rows + PADDING, spacing);

	for(int j = 0; j < (columns + PADDING); j++){
		std::cout << std::string(spacing - 1, ' '); //Substitute space with row index.
		if(j > 0 && j < rows + PADDING - 1) //Ignore first and last row.
			std::cout << j - 1 << "| ";
		else 
			std::cout << " | ";

		//Print the actual values in the tiles.
		for(int i = 0; i < (rows + PADDING); i++){
			boardTiles.at(map(i,j))->printTile();
			std::cout << " | ";
		}
		std::cout << std::endl;
		//The bottom part serves as the top of the next row.
		std::cout << std::string(spacing, ' ');
		if(j < columns + PADDING - 1)
			printBottom(rows + PADDING);
		else
			printBottom(rows + PADDING, false); //dont print extra dash
		spacing += 2;
	}
	//board->print(); prints the graph representation
}

//Apply delete on all pointers.
Hex::~Hex(){
	for(int i = 0; i < boardSize; i++){
		delete boardTiles.at(i);
	}
	delete board;
}