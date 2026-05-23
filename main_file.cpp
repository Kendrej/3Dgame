#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include "myCube.h"
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "saper.h"

int cubeSize = 5;
int bomsAmount = 10;
std::unique_ptr<Saper> saperGame = std::make_unique<Saper>(cubeSize, bomsAmount);

float speed_x = 0;
float speed_y = 0;

float current_angle_x = 0.0f; 
float current_angle_y = 0.0f;

GLuint tex;
GLuint unmasked;
GLuint one;
GLuint two;
GLuint three;
GLuint four;
GLuint five;
GLuint six;
GLuint seven;
GLuint eight;
GLuint bomb;
GLuint flag;
GLuint explosion;
GLuint defused;
GLuint notMine;

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
			speed_y = PI;
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_y = -PI;
		}
		if (key == GLFW_KEY_UP) {
			speed_x = PI;  // Zmienione z -PI na PI (lecimy kamerą do góry)
		}
		if (key == GLFW_KEY_DOWN) {
			speed_x = -PI; // Zmienione z PI na -PI (lecimy kamerą w dół)
		}
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
			saperGame = std::make_unique<Saper>(cubeSize, bomsAmount);
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
	if (ImGui::GetIO().WantCaptureMouse) {
		return;
	}
	
	if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		// 1. ZWYKŁY Y, powrót do standardu myszki OpenGL
		float x_ndc = (2.0f * xpos) / width - 1.0f;
		float y_ndc = 1.0f - (2.0f * ypos) / height;

		// 2. Naprawa odbitej osi: bazowe Z jest dodatnie w ostatniej linijce (12 zamiast -12)
		glm::vec3 cam_pos = glm::vec3(
			12.0f * cos(current_angle_x) * sin(current_angle_y),
			12.0f * sin(current_angle_x),
			12.0f * cos(current_angle_x) * cos(current_angle_y)
		);

		// 3. Wstrzyknięcie prawdziwych proporcji ekranu (width / height) zamiast 1.0f!!!
		float ratio = (float)width / (float)height;
		glm::mat4 P = glm::perspective(glm::radians(50.0f), ratio, 1.0f, 50.0f);
		glm::mat4 V = glm::lookAt(cam_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec4 ray_clip = glm::vec4(x_ndc, y_ndc, -1.0, 1.0);
		glm::vec4 ray_eye = glm::inverse(P) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0); // Wektor kierunkowy

		glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(V) * ray_eye));

		// Macierz obrotu calej kostki
		glm::mat4 M = glm::mat4(1.0f);
		

		int gridSize = saperGame->getCubeSize();
		float spacing = 1.01f;
		float offset = (gridSize - 1) * spacing / 2.0f;
		float cubeRadius = offset;

		float min_distance = 100000.0f;
		int hit_z = -1, hit_y = -1, hit_x = -1;

		// Sprawdzanie kolizji dla kazdego z 150 klocków
		for (int z = 0; z < gridSize; z++) {
			for (int y = 0; y < gridSize; y++) {
				for (int x = 0; x < gridSize; x++) {

					glm::mat4 Model_local = glm::mat4(1.0f);

					
					float pos_x = (x * spacing) - offset;
					float pos_y = (y * spacing) - offset;
					float pos_z = (z * spacing) - offset;

					Model_local = glm::translate(Model_local, glm::vec3(pos_x, pos_y, pos_z));
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
						hit_z = z; hit_y = y; hit_x = x;
					}
				}
			}
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (hit_z != -1) {
				if (saperGame->isBomb(hit_x, hit_y, hit_z) && saperGame->isHidden(hit_x, hit_y, hit_z)) {
					printf("BOOM! Game Over!\n");
					saperGame->setExploded(hit_x, hit_y, hit_z, true);
					saperGame->revealAll();
				}
				else if(saperGame->isHidden(hit_x, hit_y, hit_z)){
					saperGame->setHidden(hit_x, hit_y, hit_z, false);
					saperGame->setTilesRevealed(saperGame->getTilesRevealed() + 1);
				}
				
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (hit_z != -1 && saperGame->isHidden(hit_x,hit_y,hit_z)) {
				if (saperGame->isFlagged(hit_x, hit_y, hit_z)) {
					saperGame->setFlagged(hit_x, hit_y, hit_z, false);
					saperGame->setFlagsInTotal(saperGame->getFlagsInTotal() - 1);
				}
				else {
					saperGame->setFlagged(hit_x, hit_y, hit_z, true);
					saperGame->setFlagsInTotal(saperGame->getFlagsInTotal() + 1);
				}
			}
		}

		if (saperGame->getFlagsInTotal() == saperGame->getBombs() && saperGame->getTilesRevealed() + saperGame->getFlagsInTotal() == saperGame->getTilesInTotal()) {
			printf("Congratulations! You won!\n");
			saperGame->defuse();
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

	if (error) {
		fprintf(stderr, "BLAD LodePNG: Nie udalo sie wczytac tekstury %s!\nLodePNG error: %s\n", filename, lodepng_error_text(error));
		// Tworzymy małą, czerwoną teksturę ostrzegawczą w locie, żeby program się nie wysypał
		unsigned char errorPixel[] = { 255, 0, 0, 255 };
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, errorPixel);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		return tex;
	}

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
	one = readTexture("assets/revealed_tile_1.png");
	two = readTexture("assets/revealed_tile_2.png");
	three = readTexture("assets/revealed_tile_3.png");
	four = readTexture("assets/revealed_tile_4.png");
	five = readTexture("assets/revealed_tile_5.png");
	six = readTexture("assets/revealed_tile_6.png");
	seven = readTexture("assets/revealed_tile_7.png");
	eight = readTexture("assets/revealed_tile_8.png");
	bomb = readTexture("assets/revealed_tile_bomb.png");
	flag = readTexture("assets/masked_tile_flag.png");
	explosion = readTexture("assets/tile_exploded.png");
	defused = readTexture("assets/tile_defused.png");
	notMine = readTexture("assets/tile_not_mine.png");

	// --- INICJALIZACJA IMGUI ---
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Ciemny, nowoczesny motyw wizualny
	ImGui::StyleColorsDark();

	// Powiązanie z Twoim oknem GLFW i wersją OpenGL
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	freeShaders();
	glDeleteTextures(1, &tex);

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

GLuint texToDraw(Cell cell) {
	if (cell.isDefused) return defused;
	else if (cell.isNotMine) return notMine;
	else if (cell.isFlagged) return flag;
	else if (cell.isHidden) return tex;
	else if (cell.exploded) return explosion;
	else if (cell.isBomb) return bomb;
	else {
		switch (cell.value) {
		case 0: return unmasked;
		case 1: return one;
		case 2: return two;
		case 3: return three;
		case 4: return four;
		case 5: return five;
		case 6: return six;
		case 7: return seven;
		case 8: return eight;
		default: return unmasked;
		}
	}
}


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości

	glm::mat4 M = glm::mat4(1.0f); //Zainicjuj macierz modelu macierzą jednostkową

	// Dodatnie Z w logice kamery, ujednolicone z nową myszką!
	glm::vec3 cam_pos = glm::vec3(
		12.0f * cos(angle_x) * sin(angle_y),
		12.0f * sin(angle_x),
		12.0f * cos(angle_x) * cos(angle_y)
	);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float ratio = (float)width / (float)height; // Proporcje ekranu

	glm::mat4 V = glm::lookAt(cam_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 P = glm::perspective(glm::radians(50.0f), ratio, 1.0f, 50.0f); // Rzutowanie z Ratio

	// Wymiary z naszego sapera (5x5, 6 scian)
	int gridSize = saperGame->getCubeSize(); // np. 5
	float spacing = 1.01f; // Minimalny odstęp, by uniknąć wylatywania krawędzi poza model
	float offset = (gridSize - 1) * spacing / 2.0f; // żeby wyśrodkować względem zera
	float cubeRadius = offset; // <-- NAPRAWIONE! Odległość ściany od środka równa perfekcyjnie wpasowanej siatce na rogach

	// Renderowanie scian kostki
	for (int z = 0; z < gridSize; z++) {
		for (int y = 0; y < gridSize; y++) {
			for (int x = 0; x < gridSize; x++) {

				glm::mat4 Model_local = glm::mat4(1.0f);

				// Przesuniecie wzgledem srodka sciany, aby wygenerowac siatke (x, y)
				float pos_x = (x * spacing) - offset;
				float pos_y = (y * spacing) - offset;
				float pos_z = (z * spacing) - offset;

				Model_local = glm::translate(Model_local, glm::vec3(pos_x, pos_y, pos_z));

				// Skalowanie malej komorki
				Model_local = glm::scale(Model_local, glm::vec3(0.5f)); // Troche pomniejszamy bazowy model myCube

				// Rysowanie zaleznie od odkrycia komorki
				Cell currentCell = saperGame->getCell(x, y, z);
				GLuint textureToDraw = texToDraw(currentCell);
				

				texKostka(P, V, M * Model_local, textureToDraw);
			}
		}
	}
	

	//glfwSwapBuffers(window); //Skopiuj bufor tylny do bufora przedniego
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

		// Blokuj wychylenie powyżej/poniżej 89 stopni (~1.55 radiana), 
		// żeby kamera nie przeleciała wertykalnie na drugą stronę osi!
		if (angle_x > 1.5f) angle_x = 1.5f;
		if (angle_x < -1.5f) angle_x = -1.5f;

		current_angle_x = angle_x; // Aktualizuj dla raycastu!
		current_angle_y = angle_y; // Aktualizuj dla raycastu!

		glfwSetTime(0); //Wyzeruj licznik czasu

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		ImGui::Begin("Moje Pierwsze Okno");

		ImGui::Text("Siemanko! To jest czyste okienko ImGui.");
		ImGui::Text("Twoje zmienne do zabawy: cubeSize (%d), bomsAmount (%d)", cubeSize, bomsAmount);

		ImGui::End();

		drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
