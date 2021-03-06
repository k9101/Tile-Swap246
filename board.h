#ifndef __BOARD_H__
#define __BOARD_H__

#include "square.h"
#include "textdisplay.h"
#include <cstdlib>
#include <sstream>
#include <fstream>

class Board { 
	int destroyed;
	int level;
	bool graphics;
	TextDisplay * td;
	Square **theBoard;
	
	std::ifstream *source;
	std::string extra;
	std::istringstream *extraColours;
	
	void readFromFile(int level);
	bool checkNeighbours(int x, int y, char colour);
	void setLevel(int level);
public:

	static int lockedTiles;

	Board(bool graphics);
	void init(int level, int seed, std::ifstream *fin, bool customScript = false);
	
	Square *getSquare(int x, int y);
	

	void explode(int x, int y, char type, int size = 3);

	
	void update(int x, int y, int colour, char ch, bool locked);
	void swap(Square * s1, Square * s2);
	bool valid(int x, int y);
	
	int  checkL(int x, int y, int matchingColour);
	bool checkH(int x, int y, int matchingColour);
	bool checkU(int x, int y, int matchingColour);
	int  checkPsy(int x, int y, int matchingColour);
	int  checkBasic(int x, int y, int matchingColour);
	
	bool checkMatch(int chain);
	
	void setTDGraphics(bool graphics);
	~Board();

	friend std::ostream &operator<<(std::ostream &out, const Board &b);
	
};

#endif