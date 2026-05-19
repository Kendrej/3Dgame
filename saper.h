#pragma once
#include <vector>

struct Cell
{
	int value = 1;
	bool isBomb = false;
	bool isHidden = true; // Informuje, czy kafelek zostal juz odkryty
};

struct Facade
{
	std::vector<std::vector<Cell>> face;
	int size = 5;
};

struct SaperCube
{
	int size = 6;
	std::vector<Facade> facade;
};

class Saper
{
public:
	Saper();
	~Saper();
	SaperCube saperCube;
private:
	void createSaper();
};