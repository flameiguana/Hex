#include "Hex.h"
#include <iostream>
#include <ctime>

using namespace std;
void randomizeBoard(Hex &board){ }

void printInstructions(){
	cout << "How to play:\n";
	cout << "Specify i j to place your piece. i goes horizontal, j vertical. Don't use commas.\n";
	cout << "Pie rule is in place, meaning whoever goes first can get their fist move stolen by\n";
	cout <<	"the other player.\n";
	cout << endl;
}

void readInput(int &x){
	while(true){
		if(cin >> x)
			break;
		else {
			cout << "Not a Number. Try again." << endl;
			cin.clear();
			cin.ignore();			
		}
	}
}


void goFirst(int rows, int simulations){
	Hex game(rows);
	game.printBoard();
	int i,j;
	while(true){
	readInput(i);
	readInput(j);
	while(!game.markBoard(1, i, j)){
		//This will keep getting input until the move is valid.
		readInput(i);
		readInput(j);
	}
	game.printBoard();
	if(game.checkWin(1))
		break;
	game.computerMoveMC(2, simulations); //is guaranteed to be valid
	game.printBoard();
	if(game.checkWin(2))
		break;
	}
}

void goSecond(int rows, int simulations){
	Hex game(rows);
	string input;
	game.printBoard();
	int i,j;
	while(true){
	game.computerMoveMC(2, simulations);
 	game.printBoard();
 	if(game.checkWin(2))
		break;
	readInput(i);
	readInput(j);
	while(!game.markBoard(1, i, j)){
		readInput(i);
		readInput(j);
	}
	game.printBoard();
	if(game.checkWin(1))
		break;
	}
}

int main(int argc, const char* arg[]){
	int simulations = 25000;
	srand ( unsigned ( std::time(0) ) );
	printInstructions();
	int rows;
	cout << "What board size do you want?" << endl;
	readInput(rows);

	char first;
	cout << "Would you like to go first (y/n)?. First player is X, second is O" << endl;
	cin >> first;

	if(first == 'y')
		goFirst(rows, simulations);
	else
		goSecond(rows, simulations);
	cin.get();
	cin.get();
}
