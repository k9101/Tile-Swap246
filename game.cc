#include "game.h"
#include "board.h"
#include <iomanip>

using namespace std;

int Game::score = 0;
const int MAXLEVELS = 2;

//dtor
Game::~Game(){
	delete theBoard;
}

//hint helper
bool Game::checkAll(int x, int y, int matchingColour){//checks for a match
	if (theBoard->checkL(x, y, matchingColour) != -1 ||
		theBoard->checkH(x, y, matchingColour) ||
		theBoard->checkU(x, y, matchingColour) ||
		theBoard->checkPsy(x, y, matchingColour) != -1||
		theBoard->checkBasic(x, y, matchingColour) != -1){
		return true;
	}
	return false;
}

//displays the first match
bool Game::hint(){
	
	int x = 0;
	int y = 0;
	int dir = 0;
	bool match = false;
	
	theBoard->setTDGraphics(false);
	
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++){
			
			if(theBoard->valid(i, j-1)){//west
				swap(i, j, i, j-1);
				if (checkAll(i, j, theBoard->getSquare(i, j)->getColour())){//check for match
					//cerr << "west" << endl;
					x = i;
					y = j;
					dir = 2;
					match = true;
				}
				swap(i, j, i, j-1);//swap back
				
				if(match){
					break;
				}
			}else if(theBoard->valid(i-1, j)){//north
				swap(i, j, i-1, j);
				if (checkAll(i, j, theBoard->getSquare(i, j)->getColour())){//check for match
				//	cerr << "north" << endl;
					x = i;
					y = j;
					dir = 0;
					match = true;
					
				}
				swap(i, j, i-1, j);//swap back
				
				if(match){
					break;
				}
			}else if(theBoard->valid(i, j+1)){//east
				swap(i, j, i, j+1);
				if (checkAll(i, j, theBoard->getSquare(i, j)->getColour())){//check for match
				//	cerr << "east" << endl;
					x = i;
					y = j;
					dir = 3;
					match = true;
				}
				
				swap(i, j, i, j+1);//swap back
				
				if(match){
					break;
				}
			}else if(theBoard->valid(i+1, j+1)){//south
				swap(i, j, i+1, j+1);
				if (checkAll(i, j, theBoard->getSquare(i, j)->getColour())){//check for match
		//			cerr << "south" << endl;
					x = i;
					y = j;
					dir = 1;
					match = true;
				}
				swap(i, j, i+1, j+1);//swap back
				
				if(match){
					break;
				}
			}
			
		}
		
		if(match){
			
			cout << x << " " << y << " " << dir << endl;  //prints if match found and print set
			theBoard->setTDGraphics(graphics);
			return true;
		}
	}
	theBoard->setTDGraphics(graphics);
	return false;//will return false if no hint
}

//will rely on hint
void Game::scramble(){
	
	//check if no moves left 
	
	srand(time(NULL));
	
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++){
			int randX = rand()%10;
			int randY = rand()%10;
			while(randX != i && randY != j){//ensure don't select the same
				randX = rand()%10;
				randY = rand()%10;
			}
			swap(i, j, randX, randY);
			
		}
	}
	
}

//clear board and create new at the same level
void Game::reset(){
	delete theBoard;
	
	theBoard = new Board(graphics);
	theBoard->init(level, seed, NULL);
}

//combine level up and down
void Game::changeLevel(bool up){
	
	if(up && level == MAXLEVELS){//don't change
		return;
	}else if(!up && level == 0){ //Changed this to down and level = 0
		return;
	}
	
	prevScore = score;

	if(up){//level up
		level++;
	}else{//down
		level--;
	}
	
	delete theBoard;
	theBoard = new Board(graphics);
	theBoard->init(level, seed, NULL);
}

//swap squares, update board
void Game::swap(int x1, int y1, int x2, int y2){
//	cerr << "Entering Swap" << endl;
	theBoard->swap(theBoard->getSquare(x1, y1), theBoard->getSquare(x2, y2));
	
	int colour1 = theBoard->getSquare(x1, y1)->getColour();
	int type1 = theBoard->getSquare(x1, y1)->getType();
	int colour2 = theBoard->getSquare(x2, y2)->getColour();
	int type2 = theBoard->getSquare(x2, y2)->getType();
	
	theBoard->update(x1, y1, colour1, type1, theBoard->getSquare(x1, y1)->getLocked());//doesn't affect locked state
	theBoard->update(x2, y2, colour2, type2, theBoard->getSquare(x2, y2)->getLocked());
	
	
}

//cleans up swap
bool Game::checkSwap(int x, int y, int dir){
	if(dir == 0){//north
		if(x-1 < 0){
			cout << "Invalid switch!" << endl;
		}else{
			swap(x, y, x-1, y);
			return true;
		}
	}else if(dir == 1){//south
		if(x+1 > 9){
			cout << "Invalid switch!" << endl;
		}else{
			swap(x, y, x+1, y);
			return true;
		}
	}else if(dir == 2){//west
		if(y-1 < 0){
			cout << "Invalid switch!" << endl;
		}else{
			swap(x, y, x, y-1);
			return true;
		}
		
	}else if(dir == 3){//east
		if(y+1 > 9){
			cout << "Invalid switch!" << endl;
		}else{
			swap(x, y, x, y+1);
			return true;
		}
	}
	
	return false;
}

//returns current level
int Game::getLevel(){
	return level;
}

bool Game::levelWon(){
	bool won = false;

	if (level == 0) {
		if (score >= prevScore + 200) {
			won = true;
		}
	} else if (level == 1) {
		if (score >= prevScore + 300) {
			won = true;
		}
	} else if (level == 2) {
		if (score >= prevScore + 500 && Board::lockedTiles <= 0) {
			won = true;
		}
	}
	
	return won;
}

bool Game::checkMatch(int chain) {
	return theBoard->checkMatch(chain);
}

//decrement moves
void Game::decMoves(){
	moves--;
}


void Game::increaseScore(int x) {
	score += x;
}

//overload operator<< called to board
ostream &operator<<(ostream &out, const Game &g){
	int high_score = 0;
	//write header output
	out << setw(18) << left << "Level:" << right << g.level << endl;
	out << setw(18) << left << "Score:" << right << g.score << endl;
	out << setw(18) << left << "Moves Remaining:" << right << g.moves << endl;
	
	ifstream getScore("high_score.txt");
	
	getScore >> high_score;//get old high score
	
	if (high_score < g.score){//update to file
		ofstream hScore("high_score.txt");
		high_score = g.score;
		hScore << g.score;
	}
	
	
	out << setw(18) << left << "High Score:" << right << high_score << endl;
	out << "------------------" << endl << endl;

	out << *g.theBoard;
	return out;
}