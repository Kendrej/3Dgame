#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "myCube.h"
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

#include "saper.h"

Saper saperGame;

float speed_x = 0;//[radiany/s]
float speed_y = 0;//[radiany/s]

float current_angle_x = 0.0f; // Globalne katy do wykorzystania w testowaniu myszy
float current_angle_y = 0.0f;

GLuint tex;
GLuint unmasked;


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			speed_y = -PI;
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_y = PI;
		}
		if (key == GLFW_KEY_UP) {
			speed_x = -PI;
		}
		if (key == GLFW_KEY_DOWN) {
			speed_x = PI;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
			speed_y = 0;
		}
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
			speed_x = 0;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		// Obliczanie wektora raycasting z myszki
		float x_ndc = (2.0f * xpos) / width - 1.0f;
		float y_ndc = 1.0f - (2.0f * ypos) / height;

		glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
		glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, -12.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec4 ray_clip = glm::vec4(x_ndc, y_ndc, -1.0, 1.0);
		glm::vec4 ray_eye = glm::inverse(P) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0); // Wektor kierunkowy

		glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(V) * ray_eye));
		glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, -12.0f); // Pozycja kamery w world space

		// Macierz obrotu calej kostki
		glm::mat4 M = glm::mat4(1.0f);
		M = glm::rotate(M, current_angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
		M = glm::rotate(M, current_angle_x, glm::vec3(1.0f, 0.0f, 0.0f));

		int gridSize = saperGame.saperCube.facade[0].size;
		float spacing = 1.03f;
		float offset = (gridSize - 1) * spacing / 2.0f;
		float cubeRadius = offset;

		float min_distance = 100000.0f;
		int hit_f = -1, hit_y = -1, hit_x = -1;

		// Sprawdzanie kolizji dla kazdego z 150 klocków
		for (int f = 0; f < 6; f++) {
			for (int y = 0; y < gridSize; y++) {
				for (int x = 0; x < gridSize; x++) {

					glm::mat4 Model_local = glm::mat4(1.0f);

					// Ta sama sciezka macierzy co w drawScene
					if (f == 0) Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius));
					if (f == 1) { Model_local = glm::rotate(Model_local, glm::radians(180.0f), glm::vec3(0, 1, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); }
					if (f == 2) { Model_local = glm::rotate(Model_local, glm::radians(90.0f), glm::vec3(0, 1, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); }
					if (f == 3) { Model_local = glm::rotate(Model_local, glm::radians(-90.0f), glm::vec3(0, 1, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); }
					if (f == 4) { Model_local = glm::rotate(Model_local, glm::radians(90.0f), glm::vec3(1, 0, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); }
					if (f == 5) { Model_local = glm::rotate(Model_local, glm::radians(-90.0f), glm::vec3(1, 0, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); }

					float pos_x = (x * spacing) - offset;
					float pos_y = (y * spacing) - offset;
					Model_local = glm::translate(Model_local, glm::vec3(pos_x, pos_y, 0.0f));
					Model_local = glm::scale(Model_local, glm::vec3(0.5f));

					glm::mat4 finalMatrix = M * Model_local;

					// Przeniesienie promienia do lokalnej przestrzeni klocka
					glm::vec3 ray_origin_local = glm::vec3(glm::inverse(finalMatrix) * glm::vec4(cam_pos, 1.0f));
					glm::vec3 ray_dir_local = glm::normalize(glm::vec3(glm::inverse(finalMatrix) * glm::vec4(ray_wor, 0.0f)));

					// Prosty Ray-AABB intersection dla kostki obrobionej lokalnie od -1 do 1
					float tMin = 0.0f;
					float tMax = 100000.0f;
					glm::vec3 obbMin = glm::vec3(-1.0f);
					glm::vec3 obbMax = glm::vec3(1.0f);

					bool intersect = true;
					for (int i = 0; i < 3; i++) {
						if (abs(ray_dir_local[i]) < 0.001f) {
							if (ray_origin_local[i] < obbMin[i] || ray_origin_local[i] > obbMax[i]) { intersect = false; break; }
						} else {
							float t1 = (obbMin[i] - ray_origin_local[i]) / ray_dir_local[i];
							float t2 = (obbMax[i] - ray_origin_local[i]) / ray_dir_local[i];
							if (t1 > t2) std::swap(t1, t2);
							if (t1 > tMin) tMin = t1;
							if (t2 < tMax) tMax = t2;
							if (tMin > tMax) { intersect = false; break; }
						}
					}

					// Jesli uderzylo i stalo sie to blizej uzytkownika niz poprzednie klocki
					if (intersect && tMin < min_distance) {
						min_distance = tMin;
						hit_f = f; hit_y = y; hit_x = x;
					}
				}
			}
		}

		if (hit_f != -1) {
			saperGame.saperCube.facade[hit_f].face[hit_y][hit_x].isHidden = false;
		}
	}
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0, 0, 0, 1); //Ustaw kolor czyszczenia bufora kolorów
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości na pikselach
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback); // Zarejestrowanie obslugi myszki!
	tex = readTexture("assets/masked_tile.png");
	unmasked = readTexture("assets/revealed_tile.png");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	glDeleteTextures(1, &tex);
}


void texKostka(glm::mat4 P, glm::mat4 V, glm::mat4 M, GLuint currentTex) {

	spTextured->use(); //Aktywuj program cieniujący

	glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(M)); //Załaduj do programu cieniującego macierz modelu


	glEnableVertexAttribArray(spTextured->a("vertex"));
	glVertexAttribPointer(spTextured->a("vertex"), 4, GL_FLOAT, false, 0, myCubeVertices); //Współrzędne wierzchołków bierz z tablicy myCubeVertices

	glEnableVertexAttribArray(spTextured->a("texCoord"));
	glVertexAttribPointer(spTextured->a("texCoord"), 2, GL_FLOAT, false, 0, myCubeTexCoords); //Współrzędne teksturowania bierz z tablicy myCubeTexCoords

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, currentTex);
	glUniform1i(spTextured->u("tex"), 0);

	glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount);

	glDisableVertexAttribArray(spTextured->a("vertex"));
	glDisableVertexAttribArray(spTextured->a("color"));
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości

	glm::mat4 M = glm::mat4(1.0f); //Zainicjuj macierz modelu macierzą jednostkową
	M = glm::rotate(M, angle_y, glm::vec3(0.0f, 1.0f, 0.0f)); //Pomnóż macierz modelu razy macierz obrotu o kąt angle wokół osi Y
	M = glm::rotate(M, angle_x, glm::vec3(1.0f, 0.0f, 0.0f)); //Pomnóż macierz modelu razy macierz obrotu o kąt angle wokół osi X
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, -12.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku (oddalona kamera)
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

	// Wymiary z naszego sapera (5x5, 6 scian)
	int gridSize = saperGame.saperCube.facade[0].size; // np. 5
	float spacing = 1.03f; // Minimalny odstęp, by uniknąć wylatywania krawędzi poza model
	float offset = (gridSize - 1) * spacing / 2.0f; // żeby wyśrodkować względem zera
	float cubeRadius = offset; // <-- NAPRAWIONE! Odległość ściany od środka równa perfekcyjnie wpasowanej siatce na rogach

	// Renderowanie scian kostki
	for (int f = 0; f < 6; f++) {
		for (int y = 0; y < gridSize; y++) {
			for (int x = 0; x < gridSize; x++) {

				glm::mat4 Model_local = glm::mat4(1.0f);

				// Ustawienie scian (obracamy w zaleznosci od numeru 'f')
				if (f == 0) Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); // Przod
				if (f == 1) { Model_local = glm::rotate(Model_local, glm::radians(180.0f), glm::vec3(0, 1, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); } // Tyl
				if (f == 2) { Model_local = glm::rotate(Model_local, glm::radians(90.0f), glm::vec3(0, 1, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); } // Prawa
				if (f == 3) { Model_local = glm::rotate(Model_local, glm::radians(-90.0f), glm::vec3(0, 1, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); } // Lewa
				if (f == 4) { Model_local = glm::rotate(Model_local, glm::radians(90.0f), glm::vec3(1, 0, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); } // Gora
				if (f == 5) { Model_local = glm::rotate(Model_local, glm::radians(-90.0f), glm::vec3(1, 0, 0)); Model_local = glm::translate(Model_local, glm::vec3(0.0f, 0.0f, cubeRadius)); } // Dol

				// Przesuniecie wzgledem srodka sciany, aby wygenerowac siatke (x, y)
				float pos_x = (x * spacing) - offset;
				float pos_y = (y * spacing) - offset;

				Model_local = glm::translate(Model_local, glm::vec3(pos_x, pos_y, 0.0f));

				// Skalowanie malej komorki
				Model_local = glm::scale(Model_local, glm::vec3(0.5f)); // Troche pomniejszamy bazowy model myCube

				// Rysowanie zaleznie od odkrycia komorki
				Cell currentCell = saperGame.saperCube.facade[f].face[y][x];
				GLuint textureToDraw = currentCell.isHidden ? tex : unmasked;

				texKostka(P, V, M * Model_local, textureToDraw);
			}
		}
	}
	

	glfwSwapBuffers(window); //Skopiuj bufor tylny do bufora przedniego
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x = 0; //zadeklaruj zmienną przechowującą aktualny kąt obrotu
	float angle_y = 0; //zadeklaruj zmienną przechowującą aktualny kąt obrotu
	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle_x += speed_x * glfwGetTime(); //Oblicz kąt o jaki obiekt obrócił się podczas poprzedniej klatki
		angle_y += speed_y * glfwGetTime(); //Oblicz kąt o jaki obiekt obrócił się podczas poprzedniej klatki

		current_angle_x = angle_x; // Aktualizuj dla raycastu!
		current_angle_y = angle_y; // Aktualizuj dla raycastu!

		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
