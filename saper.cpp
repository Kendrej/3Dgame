#include "saper.h"
#include <random>

Saper::Saper(int size): size(size) {
	createSaper();
}

Saper::~Saper() {
}

bool Saper::isMine(int x, int y, int z) {
	return cube[x][y][z].isBomb;
}

void Saper::setMine(int x, int y, int z) {
	cube[x][y][z].isBomb = true;
}

bool Saper::isSurface(int x, int y, int z) {
	int maxIdx = size - 1;
	
	return (x == 0 || x == maxIdx ||
			y == 0 || y == maxIdx ||
			z == 0 || z == maxIdx);
}

void Saper::createSaper() {
	
	cube.resize(size, std::vector<std::vector<Cell>>(size, std::vector<Cell>(size)));

	int bombsToPlace = 10;
	int innerSize = size - 2;

	if (innerSize < 0) {
		innerSize =0;
	}
	int totalCells = (size * size * size) - (innerSize * innerSize * innerSize);
	if (bombsToPlace > totalCells) {
		bombsToPlace = totalCells;
	}

	std::random_device rd;  // Inicjalizacja ziarna losowania
	std::mt19937 gen(rd()); // Generator (Mersenne Twister)

	// Dystrybucje losuj¹ce indeksy - dopasowane do wielkoœci tablic/vektorów z góry
	std::uniform_int_distribution<> dis(0, size - 1);

	int bombsPlaced = 0;
	while (bombsPlaced < bombsToPlace) {
		// Zlosuj wspó³rzêdne w 3D
		int x = dis(gen); // indek fasady/œciany
		int y = dis(gen);  // wiersz
		int z = dis(gen);  // kolumna

		// SprawdŸ, czy w tym miejscu ju¿ nie ma bomby
		if (!isMine(x,y,z) && isSurface(x,y,z)) { // <-- Zak³adam zmienn¹ "isMine", zmieñ na w³aœciw¹!
			setMine(x,y,z); // <-- Postaw bombê
			bombsPlaced++;
		}
	}
}