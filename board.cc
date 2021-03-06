#include "board.h"
#include "square.h"
#include "game.h"
#include "textdisplay.h"
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

int Board::lockedTiles = 0;

Board::Board(bool graphics): graphics(graphics), td(NULL), theBoard(NULL){}

Board::~Board(){
	delete td;
	delete source;
	delete extraColours;
	
	for(int i = 0; i < 10; i++){
		delete theBoard[i];
	}
	
	delete [] theBoard;
}

//reads in a valid board from a file
void Board::readFromFile(int level){
	/* colours:    Squares:
	 * White: 0    Basic: _
	 * Red: 1      Lateral: h
	 * Green: 2    Upright: v
	 * Blue: 3     Unstable: b
	 *             Psychedelic: p
	 */
	bool locked = false;
	char type;
	char colour;
	char lock;
	for(int i = 0; i < 10; i++){
		
		theBoard[i] = new Square[10];
		
		for(int j = 0; j < 10; j++){
			*source >> lock >> type >> colour;
			
			Square * sq = new Square(i, j, colour, type, locked, td);
			if (i != 0) {
				sq->setAbove(&theBoard[i-1][j]);
			} else {
				sq->setAbove(NULL);
			}
			
			sq->setLevel(level);

			theBoard[i][j] = *sq;
			
			update(i, j, colour, type, locked);
		}
	}
}

//checks if valid cooridinates
bool Board::valid(int x, int y){
	if(x >= 0 && x < 10 && y >= 0 && y < 10){
		return true;
	}
	
	return false;
}

//checks to ensure no matches are made in initialization
bool Board::checkNeighbours(int x, int y, char colour){

	if((!valid(x-1, y) || colour != theBoard[x-1][y].getColour()) && (!valid(x, y-1) || colour != theBoard[x][y-1].getColour())){
		return false;
	}
	return true;

	
}

//initialize the current board at a level
void Board::init(int level, int seed, std::ifstream *fin, bool customScript){
	this->level = level;

	string zeroFName = "sequence.txt";
	bool locked = false;
	char type;
	char colour;
	source = fin;
	
	if(theBoard != NULL){
		delete theBoard;
	}
	if(td != NULL){
		delete td;
	}
	if(fin == NULL){
		source = new ifstream(zeroFName.c_str());
	}
	
	theBoard = new Square *[10];
	td = new TextDisplay(graphics);
	
	if(level == 0){
	    
		readFromFile(level);//read board from file
		while(!source->eof()){
			*source >> extra;
		}
		extraColours = new istringstream(extra);
		delete source;
		source = new ifstream(zeroFName.c_str());
	}else if(level == 1){
		
		if(!customScript){
	//		cerr << "random level 1" << endl;
			int specialCount = 1;
			locked = false;
			srand(seed);
			extraColours = NULL;
			for(int i = 0; i < 10; i++){
				theBoard[i] = new Square[10];
				int randColour = 0;
				int randType = 0;
				
				for(int j = 0; j < 10; j++){
					
					//generate colour
					do{
			//			cerr << "loop" << endl;
						randColour = rand()%6;//generate for colour
						if(randColour == 0 || randColour == 1){//1/3 for white
							colour = '0';
						}else if(randColour == 2 || randColour == 3){//1/3 for red
							colour = '1';
						}else if(randColour == 4){//1/6 for green
							colour = '2';
						}else if(randColour == 5){//1/6 for blue
							colour = '3';
						}
					}while(checkNeighbours(i, j, colour));
						
					if(specialCount == 5){//every 5th is special
						randType = rand()%4;
						
						if(randType == 0){//lateral
							type = 'h';
						}else if(randType == 1){//upright
							type = 'v';
						}else if(randType == 2){//unstable
							type = 'b';
						}else if(randType == 3){//psychadelic
							type = 'p';
						}
						
						specialCount = 1;
					}else{
						type = '_';
						specialCount++;
					}
				//	cerr << i << " " << j << " " << type << endl;
					Square * sq = new Square(i, j, colour, type, locked, td);
					if (i != 0) {
						sq->setAbove(&theBoard[i-1][j]);
					} else {
						sq->setAbove(NULL);
					}
					
					sq->setLevel(level);
					
					theBoard[i][j] = *sq;
					
					update(i, j, colour, type, locked);
				}
			}
		}else{
	//		cerr << "scripted level 1" << endl;
			readFromFile(level);
			while(!source->eof()){//load with extra colours
				*source >> extra;
			}
			if(extra != ""){//store extra colours
				extraColours = new istringstream(extra);
			}
			delete source;
			source = new ifstream(zeroFName.c_str());//reset source to default
		}
		
	}else if(level == 2){
		
		if(!customScript){
	//		cerr << "enter random level 2" << endl;
			int totalLocked = 20;//20% locked
			srand(seed);
			type = '_';
	
			for(int i = 0; i < 10; i++){
				theBoard[i] = new Square[10];
				int randColour = 0;
				
				for(int j = 0; j < 10; j++){
					bool lock = false;
					int ranLock = rand()%4;
					
					if(ranLock == 0 && totalLocked > 0){
				//		cerr << "locked:" << i << " " << j << endl;
				//		cerr << totalLocked << endl;

						Board::lockedTiles++;
						totalLocked--;
						lock = true;
					}
					
					//generate colour
					do{
						randColour = rand()%4;//generate for colour
					
						if(randColour == 0){//1/3 for white
							colour = '0';
						}else if(randColour == 1){//1/3 for red
							colour = '1';
						}else if(randColour == 2){//1/6 for green
							colour = '2';
						}else if(randColour == 3){//1/6 for blue
							colour = '3';
						}
					}while(checkNeighbours(i, j, colour));
					Square * sq = new Square(i, j, colour, '_', lock, td);
					if (i != 0) {
						sq->setAbove(&theBoard[i-1][j]);
					} else {
						sq->setAbove(NULL);
					}
					
					sq->setLevel(level);

					theBoard[i][j] = *sq;
					
					update(i, j, colour, type, lock);
				}
			}
		}else{
			cerr << "enter scripted level 2" << endl;
			readFromFile(level);
			while(!source->eof()){//load with extra colours
				*source >> extra;
			}
			if(extra != ""){//colours to be used
				extraColours = new istringstream(extra);
			}
			delete source;
			source = new ifstream(zeroFName.c_str());
		}
		
	}
	//more as levels increase
}

// destroyes the square at x y based on the type of square
// Lateral: h 	
// Upright: v
// Unstable: b
// Psychedelic: p
void Board::explode(int x, int y, char type, int size) {
	// cerr << "In explode, with x: " << x << " and y: " << y << " and type: " << type << endl;
	char oldType = theBoard[x][y].getType();


	this->destroyed++; //increases the number of tiles this match destroyed for scoring purposes

	if (oldType == '_') {
		theBoard[x][y].setType('D');
	} else if (oldType == 'h') {
		theBoard[x][y].setType('D');

		for (int i = 0; i < 10; i++) {
			if (i != x) {
				if (valid(i,y)) {
					explode(i, y, 'D');
				}
			}
		}
	} else if (oldType == 'v') {
		// upright
		theBoard[x][y].setType('D');

		for (int i = 0; i < 10; i++) {
			if (i != y) {
				if (valid(x,i)) {
					explode(x, i, 'D');
				}
			}
		}
	} else if (oldType == 'b') {
		// unstable
		// unstable makes a different sized hole depending on what kind of match it was in
		// Default is 3, but can be passed as a 4th parameter
		theBoard[x][y].setType('D');

		if (size == 3) {
			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					if (i != x and j != y) {
						if (valid(x + i, y + j)) {
							explode(x + i, y + j, 'D');
						}
					}
				}
			}
		} else {
			for (int i = -2; i < 3; i++) {
				for (int j = -2; j < 3; j++) {
					if (i != x and j != y) {
						if (valid(x + i, y + j)) {
							explode(x + i, y + j, 'D');
						}
					}
				}
			}
		}
	} else if (oldType == 'p') {
		// Psychedelic
		int psyCol = theBoard[x][y].getColour();
		theBoard[x][y].setType('D');

		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (x != i and y != j and valid(i,j)) {
					if (theBoard[i][j].getColour() == psyCol) {
						explode(i,j,'D');
					}
				}
			}
		}
	}

	if (type != 'D') {
		theBoard[x][y].setType(type);
		theBoard[x][y].updateTD(x,y,theBoard[x][y].getColour(), type);
	} else {
		theBoard[x][y].setType('D');
	}
}



//get square at x, y
Square *Board::getSquare(int x, int y){
	return &theBoard[x][y];
}

//update the board
void Board::update(int x, int y, int colour, char ch, bool lock){
	
	td->update(x, y, colour, ch, lock);//TextDisplay update
	
	
}

//swaps the colour and type of s1 and s2, need to check for match and account for moves
void Board::swap(Square * s1, Square * s2){
	
	int tmpColour = s1->getColour();
	int tmpType = s1->getType();
	
	/*cout << tmpColour <<endl;
	cout << tmpType << endl;*/
	
	s1->setColour(s2->getColour());
	s1->setType(s2->getType());
	
	s2->setColour(tmpColour);
	s2->setType(tmpType);
}


//check for L
int Board::checkL(int x, int y, int matchingColour){
	
	if(valid(x+1, y) && valid(x+2, y) && valid(x, y+1) && valid(x, y+2) &&
 	   theBoard[x+1][y].getColour() == matchingColour &&
	   theBoard[x+2][y].getColour() == matchingColour &&
	   theBoard[x][y+1].getColour() == matchingColour &&
	   theBoard[x][y+2].getColour() == matchingColour){//right and down
	//	cerr << x << " " << y << " right and down" << endl;
		return 0;
	}else if(valid(x+1, y) && valid(x+2, y) && valid(x+2, y+1) && valid(x+2, y+2)
			&&
			 theBoard[x+1][y].getColour() == matchingColour &&
			 theBoard[x+2][y].getColour() == matchingColour &&
			 theBoard[x+2][y+1].getColour() == matchingColour &&
			 theBoard[x+2][y+2].getColour() == matchingColour){//right and up
		return 1;
	}else if(valid(x+1, y+2) && valid(x+2, y+2) && valid(x, y+1) && valid(x, y+2) &&
			 theBoard[x+1][y+2].getColour() == matchingColour &&
			 theBoard[x+2][y+2].getColour() == matchingColour &&
			 theBoard[x][y+1].getColour() == matchingColour &&
			 theBoard[x][y+2].getColour() == matchingColour){//down and left
		return 2;
	}else if(valid(x+1, y) && valid(x+2, y) && valid(x+2, y-1) && valid(x+2, y-2) &&
			 theBoard[x+1][y].getColour() == matchingColour &&
			 theBoard[x+2][y].getColour() == matchingColour &&
			 theBoard[x+2][y-1].getColour() == matchingColour &&
			 theBoard[x+2][y-2].getColour() == matchingColour){//up and left
		return 3;
	}
	
	return -1;//no match
	
}

//Lateral Square
bool Board::checkH(int x, int y, int matchingColour){
	if(valid(x, y+1) && valid(x, y+2) && valid(x, y+3) &&
	   theBoard[x][y+1].getColour() == matchingColour &&
	   theBoard[x][y+2].getColour() == matchingColour &&
	   theBoard[x][y+3].getColour() == matchingColour){
		return true;
	}
	return false;
	
}

//upright square
bool Board::checkU(int x, int y, int matchingColour){
	if(valid(x+1, y) && valid(x+2, y) && valid(x+3, y) &&
	   theBoard[x+1][y].getColour() == matchingColour &&
	   theBoard[x+2][y].getColour() == matchingColour &&
	   theBoard[x+3][y].getColour() == matchingColour){
		return true;
	}
	
	return false;
}

//psychadelic
int Board::checkPsy(int x, int y, int matchingColour){
	if(valid(x+1, y) && valid(x+2, y) && valid(x+3, y) && valid(x+4, y) &&
	   theBoard[x+1][y].getColour() == matchingColour &&
	   theBoard[x+2][y].getColour() == matchingColour &&
	   theBoard[x+3][y].getColour() == matchingColour &&
	   theBoard[x+4][y].getColour() == matchingColour){//vertical
		return 1;
	}else if(valid(x, y+1) && valid(x, y+2) && valid(x, y+3) && valid(x, y+4) &&
			 theBoard[x][y+1].getColour() == matchingColour &&
			 theBoard[x][y+2].getColour() == matchingColour &&
			 theBoard[x][y+3].getColour() == matchingColour &&
			 theBoard[x][y+4].getColour() == matchingColour){//horizontal
		return 2;
	}
	return -1;
}

//basic
int Board::checkBasic(int x, int y, int matchingColour){
	if(valid(x+1, y) && valid(x+2, y) &&
	   theBoard[x+1][y].getColour() == matchingColour &&
	   theBoard[x+2][y].getColour() == matchingColour){//vertical
		return 0;
	}else if(valid(x, y+1) && valid(x, y+2) &&
			 theBoard[x][y+1].getColour() == matchingColour &&
			 theBoard[x][y+2].getColour() == matchingColour){//horizontal
		return 1;
	}
	
	return -1;
}

//changed so that match is found on edge closest to origin
//everything seems to be working now
bool Board::checkMatch(int chain) { 
	bool match = false;

	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			this->destroyed = 0;
			//	cerr << "Check Match" << endl;
			//	cerr << "X: " << x << " Y: " << y << endl;
			int matchingColour = theBoard[x][y].getColour();
			int matchVal = checkPsy(x, y, matchingColour);
			
			if(matchVal != -1){//psychadelic
				// cerr << "psychadelic square" << endl;
				
				if(matchVal == 1){//vertical
					// cerr << "vertical" << endl;
					if (theBoard[x][y].getType() != 'D') explode(x, y, 'D');
					if (theBoard[x + 1][y].getType() != 'D') explode(x+1, y, 'D');
					if (theBoard[x + 2][y].getType() != 'D') explode(x+2, y, 'p');
					if (theBoard[x + 3][y].getType() != 'D') explode(x+3, y, 'D');
					if (theBoard[x + 4][y].getType() != 'D') explode(x+4, y, 'D');

					if(level == 2){
						for(int i = x; i < x+5; i++){
							if(theBoard[i][y].getLocked()){
								theBoard[i][y].unlock();
							}
						}
					}
				}else if(matchVal == 2){//horizontal
					// cerr << "horizontal" << endl;
					if (theBoard[x][y].getType() != 'D') explode(x, y, 'D');
					if (theBoard[x][y+1].getType() != 'D') explode(x, y+1, 'D');
					if (theBoard[x][y+2].getType() != 'D') explode(x, y+2, 'p');
					if (theBoard[x][y+3].getType() != 'D')explode(x, y+3, 'D');
					if (theBoard[x][y+4].getType() != 'D')explode(x, y+4, 'D');

					if(level == 2){
						for(int i = y; i < y+5; i++){
							if(theBoard[x][i].getLocked()){
								theBoard[x][i].unlock();
							}
						}
					}
				}
				
				match = true;
				continue;
			}else if(checkH(x, y, matchingColour)){//lateral
				// cerr << "lateral square" << endl;
				
				if (theBoard[x][y].getType() != 'D') explode(x, y, 'D');
				if (theBoard[x][y+1].getType() != 'D') explode(x, y+1, 'h');//could add random to select x+1 or x+2
				if (theBoard[x][y+3].getType() != 'D') explode(x, y+2, 'D');
				if (theBoard[x][y+4].getType() != 'D') explode(x, y+3, 'D');

				if(level == 2){
					for(int i = y; i < y+4; i++){
						if(theBoard[x][i].getLocked()){
							theBoard[x][i].unlock();
						}
					}
				}
				
				match = true;
				continue;
				
			}else if(checkU(x, y, matchingColour)){//Upright
				// cerr << "upright square" << endl;
				
				if (theBoard[x][y].getType() != 'D') explode(x, y, 'D');
				if (theBoard[x+1][y].getType() != 'D') explode(x+1, y, 'v');//could add random to select x+1 or x+2
				if (theBoard[x+2][y].getType() != 'D') explode(x+2, y, 'D');
				if (theBoard[x+3][y].getType() != 'D') explode(x+3, y, 'D');

				if(level == 2){
					for(int i = x; i < x+4; i++){
						if(theBoard[i][y].getLocked()){
							theBoard[i][y].unlock();
						}
					}
				}
				
				match = true;
				continue;
			}
			
			
			matchVal = checkL(x, y, matchingColour);//L
			
			if(matchVal != -1){
				// cerr << "Unstable square" << endl;
				
				if(level == 2){
					if(theBoard[x][y].getLocked()){
						theBoard[x][y].unlock();
					}
				}

				if(matchVal == 0){//down and right
					// cerr << "Down and right" << endl;
					if (theBoard[x][y].getType() != 'D') explode(x, y, 'b');
					if (theBoard[x+1][y].getType() != 'D') explode(x+1, y, 'D');
					if (theBoard[x+2][y].getType() != 'D') explode(x+2, y, 'D');
					if (theBoard[x][y+1].getType() != 'D') explode(x, y+1, 'D');
					if (theBoard[x][y+2].getType() != 'D') explode(x, y+2, 'D');

					if(level == 2){
						if(theBoard[x+1][y].getLocked()){
							theBoard[x+1][y].unlock();
						}
						if(theBoard[x+2][y].getLocked()){
							theBoard[x+2][y].unlock();
						}
						if(theBoard[x][y+1].getLocked()){
							theBoard[x][y+1].unlock();
						}
						if(theBoard[x][y+2].getLocked()){
							theBoard[x][y+2].unlock();
						}
					}
				}else if(matchVal == 1){//up and right
					// cerr << "up and right" << endl;
					if (theBoard[x][y].getType() != 'D') explode(x, y, 'D');
					if (theBoard[x+1][y].getType() != 'D') explode(x+1, y, 'D');
					if (theBoard[x+2][y].getType() != 'D') explode(x+2, y, 'b');
					if (theBoard[x+2][y+1].getType() != 'D') explode(x+2, y+1, 'D');
					if (theBoard[x+1][y+2].getType() != 'D') explode(x+1, y+2, 'D');

					if(level == 2){
						if(theBoard[x+1][y].getLocked()){
							theBoard[x+1][y].unlock();
						}
						if(theBoard[x+2][y].getLocked()){
							theBoard[x+2][y].unlock();
						}
						if(theBoard[x+2][y+1].getLocked()){
							theBoard[x+2][y+1].unlock();
						}
						if(theBoard[x+1][y+2].getLocked()){
							theBoard[x+1][y+2].unlock();
						}
					}
					
				}else if(matchVal == 2){//down and left
					// cerr << "down and left" << endl;
					if (theBoard[x][y].getType() != 'D') explode(x, y, 'D');
					if (theBoard[x][y+1].getType() != 'D') explode(x, y+1, 'D');
					if (theBoard[x][y+2].getType() != 'D') explode(x, y+2, 'b');
					if (theBoard[x+1][y+2].getType() != 'D') explode(x+1, y+2, 'D');
					if (theBoard[x+2][y+2].getType() != 'D') explode(x+2, y+2, 'D');

					if(level == 2){
						if(theBoard[x+1][y+2].getLocked()){
							theBoard[x+1][y+2].unlock();
						}
						if(theBoard[x+2][y+2].getLocked()){
							theBoard[x+2][y+2].unlock();
						}
						if(theBoard[x][y+1].getLocked()){
							theBoard[x][y+1].unlock();
						}
						if(theBoard[x][y+2].getLocked()){
							theBoard[x][y+2].unlock();
						}
					}
					
				}else if(matchVal == 3){//up and left
					// cerr << "up and left" << endl;

					if (theBoard[x][y].getType() != 'D') explode(x, y, 'D');
					if (theBoard[x+1][y].getType() != 'D') explode(x+1, y, 'D');
					if (theBoard[x+2][y].getType() != 'D') explode(x+2, y, 'b');
					if (theBoard[x+2][y-1].getType() != 'D') explode(x+2, y-1, 'D');
					if (theBoard[x+2][y-2].getType() != 'D') explode(x+2, y-2, 'D');

					if(level == 2){
						if(theBoard[x+1][y].getLocked()){
							theBoard[x+1][y].unlock();
						}
						if(theBoard[x+2][y].getLocked()){
							theBoard[x+2][y].unlock();
						}
						if(theBoard[x+2][y-1].getLocked()){
							theBoard[x+2][y-1].unlock();
						}
						if(theBoard[x+2][y-2].getLocked()){
							theBoard[x+2][y-2].unlock();
						}
					}
					
				}
				
				match = true;
				continue;
			}
			
			
			matchVal = checkBasic(x, y, matchingColour);//basic
			
			if(/*theBoard[x][y].getType() == '_' && */matchVal != -1){
				// cerr << "Basic match" << endl;
				
				if(matchVal == 0){
					// cerr << "vertical" << endl;
					if (theBoard[x][y].getType() != 'D') explode(x,y,'D');
					if (theBoard[x+1][y].getType() != 'D') explode(x+1, y,'D');
					if (theBoard[x+2][y].getType() != 'D') explode(x+2,y,'D');
					
					if(level == 2){
						for(int i = x; i < x+3; i++){
							if(theBoard[i][y].getLocked()){
								theBoard[i][y].unlock();
							}
						}
					}
				}else if(matchVal == 1){
					// cerr << "horizontal" << endl;
					if (theBoard[x][y].getType() != 'D') explode(x,y,'D');
					if (theBoard[x][y+1].getType() != 'D') explode(x,y+1,'D');
					if (theBoard[x][y+2].getType() != 'D') explode(x,y+2,'D');
					
					if(level == 2){
						for(int i = y; i < y+3; i++){
							if(theBoard[x][i].getLocked()){
								theBoard[x][i].unlock();
							}
						}
					}
				}
				match = true;
			}

			if (this->destroyed == 3) {
				Game::increaseScore(3 * pow(2,chain));
				// cerr << pow(2,chain) << endl;
			} else if (this->destroyed == 4) {
				Game::increaseScore(2 * 4 * pow(2,chain));
			} else if (this->destroyed == 5) {
				Game::increaseScore(3 * 5 * pow(2,chain));
			} else {
				Game::increaseScore(4 * this->destroyed * pow(2,chain));
			}

		}
	}

	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			if (theBoard[x][y].getType() == 'D') {
				char c = '\0';
				if(extra != ""){
					if(extraColours->eof()){//reload colours
						delete extraColours;
						extraColours = new istringstream(extra);
					}
					*extraColours >> c;
				}
				theBoard[x][y].moveDown(c);
			}
		}
	}

	return match;
}

void Board::setTDGraphics(bool graphics){
	td->setGraphics(graphics);
}

ostream &operator<<(ostream &out, const Board &b){
	// std::cerr << "here in board.cc" << std::endl;
	out << *b.td;
	return out;
}