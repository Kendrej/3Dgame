#pragma once
#include <vector>

struct Cell
{
	int value = 1;
	bool isBomb = false;
	bool isHidden = true;
	bool isFlagged = false;
};

class Saper
{
public:
	Saper(int size);
	~Saper();
	
	bool isMine(int x, int y, int z);
	void setMine(int x, int y, int z);
	int getCubeSize() { return size; }
	bool isSurface(int x, int y, int z);
	int countBombs(int x, int y, int z);
	void setValue(int x, int y, int z, int value);

	std::vector<std::vector<std::vector<Cell>>> cube;
private:
	int size = 5;
	void createSaper();
};