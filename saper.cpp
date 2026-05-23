#include "saper.h"
#include <random>

Saper::Saper(int size, int bombAmount): size(size), bombs(bombAmount) {
	createSaper();
}

Saper::~Saper() {
}

bool Saper::isBomb(int x, int y, int z) {
	return cube[x][y][z].isBomb;
}

void Saper::setBomb(int x, int y, int z) {
	cube[x][y][z].isBomb = true;
}

bool Saper::isSurface(int x, int y, int z) {
	int maxIdx = size - 1;
	
	return (x == 0 || x == maxIdx ||
			y == 0 || y == maxIdx ||
			z == 0 || z == maxIdx);
}

int Saper::countBombs(int x, int y, int z)
{
	int bombs =0;
	int x_start=x-1,x_stop=x+1,y_start=y-1,y_stop=y+1,z_start=z-1,z_stop=z+1;
	if (x == 0) {
		x_start = 0;
	}
	else if (x == size - 1) {
		x_stop = size-1;
	}
	if (y == 0) {
		y_start = 0;
	}
	else if (y == size - 1) {
		y_stop = size - 1;
	}
	if (z == 0) {
		z_start = 0;
	}
	else if (z == size - 1) {
		z_stop = size - 1;
	}

	for (int i = z_start; i <= z_stop; i++) {
		for (int j = y_start; j <= y_stop; j++) {
			for (int k = x_start; k <= x_stop; k++) {
				if (i == z && j == y && k == x) continue;

				// WARUNEK POWIERZCHNI: Prawda, jeœli s¹siad tworzy obok nas p³aszczyznê 3x3x1 
				// na dowolnej zewnêtrznej œcianie, na której siê obecnie znajdujemy.
				bool sameFace =
					(i == z && (i == 0 || i == size - 1)) ||
					(j == y && (j == 0 || j == size - 1)) ||
					(k == x && (k == 0 || k == size - 1));

				// Jeœli komórki dziel¹ tê sam¹ œcianê szeœcianu - sprawdzamy minê
				if (sameFace) {
					if (isBomb(k, j, i)) {
						bombs++;
					}
				}
			}
		}
	}
	return bombs;
}

void Saper::setValue(int x, int y, int z, int value){
	cube[x][y][z].value = value;
}

void Saper::revealAll()
{
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				if (isFlagged(i, j, k) && !isBomb(i, j, k)) {
					setNotMine(i, j, k);
				}
				setHidden(i, j, k, false);
			}
		}
	}
}

void Saper::defuse() {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				if (isFlagged(i, j, k)) {
					setDefused(i, j, k);
					setFlagged(i, j, k, false);
				}
			}
		}
	}
}


void Saper::createSaper() {
	
	cube.resize(size, std::vector<std::vector<Cell>>(size, std::vector<Cell>(size)));

	int innerSize = size - 2;

	if (innerSize < 0) {
		innerSize =0;
	}
	tilesInTotal = (size * size * size) - (innerSize * innerSize * innerSize);
	if (bombs > tilesInTotal) {
		bombs = tilesInTotal;
	}

	std::random_device rd;  // Inicjalizacja ziarna losowania
	std::mt19937 gen(rd()); // Generator (Mersenne Twister)

	// Dystrybucje losuj¹ce indeksy - dopasowane do wielkoœci tablic/vektorów z góry
	std::uniform_int_distribution<> dis(0, size - 1);

	int bombsPlaced = 0;
	while (bombsPlaced < bombs) {
		// Zlosuj wspó³rzêdne w 3D
		int x = dis(gen); // indek fasady/œciany
		int y = dis(gen);  // wiersz
		int z = dis(gen);  // kolumna

		// SprawdŸ, czy w tym miejscu ju¿ nie ma bomby
		if (!isBomb(x,y,z) && isSurface(x,y,z)) { // <-- Zak³adam zmienn¹ "isMine", zmieñ na w³aœciw¹!
			setBomb(x,y,z); // <-- Postaw bombê
			bombsPlaced++;
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				setValue(i,j,k, countBombs(i,j,k));
			}
		}
	}

}