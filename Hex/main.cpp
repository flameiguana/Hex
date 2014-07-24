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

//Game loop.
void goFirst(int rows, int simulations){
	Hex game(rows);
	std::cout << game << std::endl;
	int i,j;
	while(true){
		readInput(i);
		readInput(j);
		//This will keep getting input until the move is valid.
		while(!game.markBoard(1, i, j)){
			readInput(i);
			readInput(j);
		}
		std::cout << game << std::endl;
		if(game.checkWin(1))
		{
			std::cout << "Player 1 Wins" << std::endl;
			break;
		}
			
		std::cout << "Waiting for AI..." << std::endl;
		game.computerMoveMC(2, simulations); //is guaranteed to be valid
		std::cout << game << std::endl;
		if(game.checkWin(2))
		{
			std::cout << "Player 2 Wins" << std::endl;
			break;
		}
			
	}
}


void goSecond(int rows, int simulations){
	Hex game(rows);
	std::cout << game << std::endl;
	int i,j;
	while(true){
		std::cout << "Waiting for AI..." << std::endl;
		game.computerMoveMC(2, simulations);
 		std::cout << game << std::endl;
		if(game.checkWin(2))
		{
			std::cout << "Player 2 Wins" << std::endl;
			break;
		}
		readInput(i);
		readInput(j);
		while(!game.markBoard(1, i, j)){
			readInput(i);
			readInput(j);
		}
		std::cout << game << std::endl;
		if(game.checkWin(1))
		{
			std::cout << "Player 1 Wins" << std::endl;
			break;
		}
	}
}

int main(int argc, const char* arg[]){
	int simulations = 25000;
	srand ( unsigned ( std::time(0) ) );
	printInstructions();

	int rows;
	std::cout << "What board size do you want?" << std::endl;
	readInput(rows);

	char first;
	std::cout << "Would you like to go first (y/n)?. First player is X, second is O" << std::endl;
	std::cin >> first;

	if(first == 'y')
		goFirst(rows, simulations);
	else
		goSecond(rows, simulations);

	std::cin.get();
	std::cout << "Press Enter to exit" << std::endl;
	std::cin.get();
}
