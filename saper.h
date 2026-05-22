#pragma once
#include <vector>

struct Cell
{
	int value = 1;
	bool isBomb = false;
	bool isHidden = true; // Informuje, czy kafelek zostal juz odkryty
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

	std::vector<std::vector<std::vector<Cell>>> cube;
private:
	int size = 5;
	void createSaper();
};