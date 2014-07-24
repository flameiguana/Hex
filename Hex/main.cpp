#include "Hex.h"
#include <iostream>
#include <ctime>

void printInstructions(){
	std::cout << "How to play:\n";
	std::cout << "Specify i j to place your piece. i are columns, j rows. Don't use commas.\n";
	std::cout << "Pie rule is in place, meaning whoever goes first can get their fist\n";
	std::cout << "move stolen by the other player.\n";
	std::cout << std::endl;
}

void readInput(int &x){
	while(true){
		if(std::cin >> x)
			break;
		else {
			std::cout << "Not a Number. Try again." << std::endl;
			std::cin.clear();
			std::cin.ignore();			
		}
	}
}

//Game loop. The hex class itself does not dictate how it should be used, so the client could have AI vs
//AI or player vs player games if they really wanted to. I could even see the game being networked.
void playGame(int rows, int simulations, bool humanFirst){
	Hex game(rows);
	std::cout << game << std::endl;
	int i,j;
	bool myTurn = humanFirst;
	//First player is always X
	Hex::TileType playerMarker = Hex::X;
	while(true){
		if(myTurn)
		{
			std::cout << "Enter Your Move: ";
			readInput(i);
			readInput(j);
			//This will keep getting input until the move is valid.
			while(!game.markBoard(playerMarker, i, j)){
				readInput(i);
				readInput(j);
			}
		}
		else
		{
			std::cout << "Waiting for AI..." << std::endl;
			game.computerMoveMC(playerMarker, simulations); //is guaranteed to be valid
		}
		std::cout << game << std::endl;
		if(game.checkWin(playerMarker))
		{
			std::cout << playerMarker << " Wins!" << std::endl;
			break;
		}
		myTurn = !myTurn;
		//flip between X and O
		playerMarker = playerMarker == Hex::X ? Hex::O : Hex::X;
	}
}



int main(int argc, const char* arg[]){
	//
	const int simulations = 25000;
	//needed by hex
	srand ( unsigned ( std::time(0) ) );
	printInstructions();

	int rows;
	std::cout << "What board size do you want?" << std::endl;
	readInput(rows);

	char first;
	std::cout << "Would you like to go first (y/n)?. First player is X, second is O" << std::endl;
	std::cin >> first;

	bool humanFirst = (first == 'y');
		
	playGame(rows, simulations, humanFirst);

	std::cin.get();
	std::cout << "Press 'Enter' to exit the game." << std::endl;
	std::cin.get();
}
