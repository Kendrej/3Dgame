#include "saper.h"

Saper::Saper() {
	createSaper();
}

Saper::~Saper() {
}

void Saper::createSaper() {
	
	for (int i = 0; i < saperCube.size; i++) {
		Facade facade;
		for (int j = 0; j < facade.size; j++) {
			std::vector<Cell> row;
			for (int k = 0; k < facade.size; k++) {
				Cell cell;
				row.push_back(cell);
			}
			facade.face.push_back(row);
		}
		saperCube.facade.push_back(facade);
	}
}