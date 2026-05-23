#pragma once
#include <vector>

struct Cell
{
	int value = 1;
	bool isBomb = false;
	bool isHidden = true;
	bool isFlagged = false;
	bool exploded = false;
};

class Saper
{
public:
	Saper(int size, int bombAmount);
	~Saper();
	
	bool isBomb(int x, int y, int z);
	void setBomb(int x, int y, int z);

	bool isHidden(int x, int y, int z) { return cube[x][y][z].isHidden; }
	void setHidden(int x, int y, int z, bool hidden) { cube[x][y][z].isHidden = hidden; }

	bool isFlagged(int x, int y, int z) { return cube[x][y][z].isFlagged; }
	void setFlagged(int x, int y, int z, bool flagged) { cube[x][y][z].isFlagged = flagged; }

	bool exploded(int x, int y, int z) { return cube[x][y][z].exploded; }
	void setExploded(int x, int y, int z, bool exploded) { cube[x][y][z].exploded = exploded; }

	int getCubeSize() { return size; }
	bool isSurface(int x, int y, int z);
	int countBombs(int x, int y, int z);
	void setValue(int x, int y, int z, int value);
	Cell getCell(int x, int y, int z) { return cube[x][y][z]; }
	void revealAll();

private:
	int size = 5;
	int bombs = 10;
	std::vector<std::vector<std::vector<Cell>>> cube;
	void createSaper();
};