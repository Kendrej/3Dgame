#pragma once
#include <vector>

struct Cell
{
	int value = 0;
	bool isBomb = false;
	bool isHidden = true;
	bool isFlagged = false;
	bool exploded = false;
	bool isDefused = false;
	bool isNotMine = false;
};

class Saper
{
public:
	Saper(int size, int bombAmount);
	~Saper();
	
	bool isBomb(int x, int y, int z);
	void setBomb(int x, int y, int z);
	int getBombs() { return bombs; }

	void setDefused(int x, int y, int z) { cube[x][y][z].isDefused = true; }

	void setNotMine(int x, int y, int z) { cube[x][y][z].isNotMine = true; }

	bool isHidden(int x, int y, int z) { return cube[x][y][z].isHidden; }
	void setHidden(int x, int y, int z, bool hidden) { cube[x][y][z].isHidden = hidden; }

	bool isFlagged(int x, int y, int z) { return cube[x][y][z].isFlagged; }
	void setFlagged(int x, int y, int z, bool flagged) { cube[x][y][z].isFlagged = flagged; }

	bool exploded(int x, int y, int z) { return cube[x][y][z].exploded; }
	void setExploded(int x, int y, int z, bool exploded) { cube[x][y][z].exploded = exploded; }

	int getTilesInTotal() { return tilesInTotal; }
	void setTilesInTotal(int total) { tilesInTotal = total; }

	int getFlagsInTotal() { return flagsInTotal; }
	void setFlagsInTotal(int total) { flagsInTotal = total; }

	int getTilesRevealed() { return tilesRevealed; }
	void setTilesRevealed(int revealed) { tilesRevealed = revealed; }

	int getCubeSize() { return size; }
	bool isSurface(int x, int y, int z);
	int countBombs(int x, int y, int z);
	int getValue(int x, int y, int z) { return cube[x][y][z].value; }
	void setValue(int x, int y, int z, int value);
	Cell getCell(int x, int y, int z) { return cube[x][y][z]; }
	void revealAll();
	void defuse();
	void revealEmpty(int x, int y, int z, int &revealed);
private:
	int size = 5;
	int bombs = 10;
	int tilesInTotal;
	int tilesRevealed = 0;
	int flagsInTotal;
	std::vector<std::vector<std::vector<Cell>>> cube;
	void createSaper();
};