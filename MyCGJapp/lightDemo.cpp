//
// CGJ: Phong Shading and Text rendered with FreeType library
// The text rendering was based on https://learnopengl.com/In-Practice/Text-Rendering
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
// 
// Author: João Madeiras Pereira
//

#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>


// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <IL/il.h>


// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "geometry.h"
#include "Texture_Loader.h"
#include "AABB.h"

#include "avtFreeType.h"

using namespace std;

#define CAPTION "CGJ Demo: Phong Shading and Text rendered with FreeType"
int WindowHandle = 0;
int WinX = 1024, WinY = 576;

unsigned int FrameCount = 0;

//shaders
VSShaderLib shader;  //geometry
VSShaderLib shaderText;  //render bitmap text

//File with the font
const string font_name = "fonts/arial.ttf";

//Vector with meshes
vector<struct MyMesh> myMeshes;

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint texMode_uniformId;
GLint pvm_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId;
GLint tex_loc, tex_loc1, tex_loc2;

GLuint TextureArray[1];

// Variaveis criadas para o aviao
float rotPlaneH;
float rotPlaneV;
bool isRotatingH;
bool isRotatingV;
bool o, p, q, a;
bool estaAcelerar;
float helice = 0.0f;
float posisaoX = 0.0f;
float posisaoY = 125.0f;
float posisaoZ = 0.0f;
float rotasaoLado = 0.0f;
float rotacaoCima = 0.0f;
float acelerasao = 0.0f;

//variaveis criadas para os misseis
int nMisseisAtivos = 0;
#define N_MISSEIS_MAX 20
float infoMisseis[N_MISSEIS_MAX][5];
int missilIndex;

//variaveis criadas para a camera
float atrasoCamera[20][3];
int posIndex = 0;
int camIndex = 1;
int camera = 3;

// Variaveis criadas para a cidade
#define Q_PREDIOS 21
int alturaPredios[Q_PREDIOS][Q_PREDIOS];
int larguraPredio = 32;
int comprimentoPredio = 32;

//bounding boxes do aviao
AABB aabb;
bool bateu = false;

// Definicoes criadas
#define PI 3.14159265
	
// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 39.0f, beta = 51.0f;
float r = 10.0f;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];
float lightPos[4] = {496.0f, 400.0f, 496.0f, 1.0f};

void refresh(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, refresh, 0);
}

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
    FrameCount = 0;
	glutTimerFunc(1000, timer, 0);
}


// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix
	ratio = (1.0f * w) / h;
	loadIdentity(PROJECTION);
	perspective(53.13f, ratio, 0.1f, 1000.0f);
}

void drawMesh(int objId) {
	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	// Render mesh
	glBindVertexArray(myMeshes[objId].vao);

	if (!shader.isProgramValid()) {
		printf("Program Not Valid!\n");
		exit(1);
	}
	glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void renderTexto() {
	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	glDisable(GL_DEPTH_TEST);
	//the glyph contains background colors and non-transparent for the actual character pixels. So we use the blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	//viewer at origin looking down at  negative z direction
	pushMatrix(MODEL);
	loadIdentity(MODEL);
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);
	pushMatrix(VIEW);
	loadIdentity(VIEW);
	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
	RenderText(shaderText, "This is a sample text", 25.0f, 25.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	RenderText(shaderText, "CGJ Light and Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void applyRotation() {
	if (o) {
		rotasaoLado -= 1.5f;
	}

	if (o && rotPlaneH > -45.0f) {
		rotPlaneH -= 1.5f;
	}
	if (p) {
		rotasaoLado += 1.5f;
	}
	if (p && rotPlaneH < 45.0f) {
		rotPlaneH += 1.5f;
	}
	if (q) {
		rotacaoCima -= 1.5f;
	}
	if (q && rotPlaneV > -15.0f) {
		rotPlaneV -= 1.0f;
	}
	if (a) {
		rotacaoCima += 1.5f;
	}
	if (a && rotPlaneV < 15.0f) {
		rotPlaneV += 1.0f;
	}
	if (rotacaoCima > 360.0f) {
		rotacaoCima = 0.0f;
	}
	if (rotacaoCima < 0) {
		rotacaoCima = 360.0f;
	}
}

void renderAviao() {
	int objId = 0;
	GLint loc;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 4; ++j) {
			int use = 0;
			glUniform1i(texMode_uniformId, 0);

			if (objId > 7)
				continue;

			if (objId == 0) { //parte da frente do aviao
				use = 3;
			}
			else if (objId == 1) { //corpo do aviao
				use = 1;
			}
			else if (objId == 2) { //parte de tras do aviao
				use = 3;
			}
			else if (objId == 3) { //asa do aviao
				use = 3;
			}
			else if (objId == 4 || objId == 7) { //helice do aviao
				glUniform1i(texMode_uniformId, 1);
				use = 5;
			}
			else if (objId == 5) { //Estabilizador vertical do aviao
				use = 3;
			}
			else if (objId == 6) {	//Estabilizador horizontal do aviao
				use = 3;
			}

			// send the material
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, myMeshes[use].mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, myMeshes[use].mat.diffuse);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, myMeshes[use].mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, myMeshes[use].mat.shininess);
			pushMatrix(MODEL);
			if (objId < 8) {
				translate(MODEL, posisaoX, posisaoY, posisaoZ);
				rotate(MODEL, -rotasaoLado, 0, 1, 0);
				rotate(MODEL, rotacaoCima, 0, 0, 1);
			}

			if (objId == 0) { //parte da frente do aviao
				rotate(MODEL, rotPlaneV, 0, 0, 1);
				rotate(MODEL, rotPlaneH, 1, 0, 0);
				translate(MODEL, 1.0f, 0, 0);
				scale(MODEL, 2.0f, 1.0f, 1.0f);
			}
			else if (objId == 1) { //corpo do aviao
				rotate(MODEL, rotPlaneV, 0, 0, 1);
				rotate(MODEL, rotPlaneH, 1, 0, 0);
				translate(MODEL, 0.125f, 0, 0);
				scale(MODEL, 1.5f, 0.5f, 0.5f);
				rotate(MODEL, 90, 0, 0, 1);
			}
			else if (objId == 2) { //parte de tras do aviao
				rotate(MODEL, rotPlaneV, 0, 0, 1);
				rotate(MODEL, rotPlaneH, 1, 0, 0);
				translate(MODEL, -0.2f, 0, 0);
				scale(MODEL, 4.0f, 1.0f, 1.0f);
			}
			else if (objId == 3) { //asa do aviao
				rotate(MODEL, rotPlaneV, 0, 0, 1);
				rotate(MODEL, rotPlaneH, 1, 0, 0);
				scale(MODEL, 1.0f, 0.2f, 5.0f);
			}
			else if (objId == 4) { //helice do aviao
				rotate(MODEL, rotPlaneV, 0, 0, 1);
				rotate(MODEL, rotPlaneH, 1, 0, 0);
				rotate(MODEL, helice, 1, 0, 0);
				translate(MODEL, 2.0f, 0.0f, 0.0f);
				rotate(MODEL, 90, 0, 1, 0);
			}
			else if (objId == 7) { //helice do aviao
				rotate(MODEL, rotPlaneV, 0, 0, 1);
				rotate(MODEL, rotPlaneH, 1, 0, 0);
				rotate(MODEL, helice, 1, 0, 0);
				translate(MODEL, 2.0f, 0.0f, 0.0f);
				rotate(MODEL, -90, 0, 1, 0);
			}
			else if (objId == 5) { //Estabilizador vertical do aviao
				rotate(MODEL, rotPlaneV, 0, 0, 1);
				rotate(MODEL, rotPlaneH, 1, 0, 0);
				translate(MODEL, -1.95f, 0.5f, 0);
				scale(MODEL, 0.5f, 1.2f, 0.2f);
			}
			else if (objId == 6) { //Estabilizador horizontal do aviao
				rotate(MODEL, rotPlaneV, 0, 0, 1);
				rotate(MODEL, rotPlaneH, 1, 0, 0);
				translate(MODEL, -1.95f, 0, 0);
				scale(MODEL, 0.5f, 0.2f, 2.0f);
			}

			drawMesh(use);

			popMatrix(MODEL);
			objId++;
			if (!bateu) {
				if (!o && !p && (rotPlaneH > 0.3f || rotPlaneH < -0.3f)) {
					if (rotPlaneH > 0.0f)
						rotPlaneH -= 0.2f;
					else
						rotPlaneH += 0.2f;
				}
				if (!q && !a && (rotPlaneV > 0.3f || rotPlaneV < -0.3f)) {
					if (rotPlaneV > 0.0f)
						rotPlaneV -= 0.2f;
					else
						rotPlaneV += 0.2f;
				}
				helice += 2.0f;
			}
		}
	}
}

void renderTorre() {

	GLint loc;
	int objId = 11;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 6; ++j) {
			int use = 0;
			glUniform1i(texMode_uniformId, 0);
			if (objId > 10) {
				objId--;
				continue;
			}

			if (objId == 0 || objId == 7) {
				use = 2;
			}
			else if (objId == 1 || objId == 6) {
				use = 6;
			}
			else if (objId == 2 || objId == 3 || objId == 4 || objId == 5 || objId == 8 || objId == 9 || objId == 10) {
				use = 1;
			}

			// send the material
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, myMeshes[use].mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, myMeshes[use].mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, myMeshes[use].mat.shininess);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			if (objId == 0 || objId == 2) {
				float dif[] = { 0.36f, 0.73f, 0.9f, 0.78f };
				glUniform4fv(loc, 1, dif);
			}
			else if (objId == 1) {
				float dif[] = { 0.36f, 0.73f, 0.89f, 1.0f };
				glUniform4fv(loc, 1, dif);
			}
			else if (objId == 3 || objId == 4 || objId == 5 || objId == 6 || objId == 7 || objId == 8 || objId == 9 || objId == 10) {
				float dif[] = { 0.97f, 0.98f, 0.98f, 1.0f };
				glUniform4fv(loc, 1, dif);
			}
			else {
				glUniform4fv(loc, 1, myMeshes[use].mat.diffuse);
			}
			pushMatrix(MODEL);

			translate(MODEL, 496.0f, 3.0, 496.0f);

			if (objId == 0) { //cone
				glDepthMask(GL_FALSE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				scale(MODEL, 18.0f, 260.0f, 18.0f);
			}
			else if (objId == 1) { //torus de baixo
				translate(MODEL, 0.0f, 162.0f, 0.0f);
				//scale(MODEL, 14.0f, 23.0f, 14.0f);
				scale(MODEL, 9.5f, 4.0f, 9.5f);
				rotate(MODEL, 90, 0, 0, 1);
			}
			else if (objId == 2) { //cilindro do meio transparente
				glDepthMask(GL_FALSE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				translate(MODEL, 0.0f, 183.0f, 0.0f);
				scale(MODEL, 7.5f, 27.0f, 7.5f);
			}
			else if (objId == 3) { // cilindro mais gordo de cima
				translate(MODEL, 0.0f, 202.0f, 0.0f);
				scale(MODEL, 10.0f, 10.0f, 10.0f);
			}
			else if (objId == 4) { //cilindro de cima
				translate(MODEL, 0.0f, 212.0f, 0.0f);
				scale(MODEL, 5.0f, 18.0f, 5.0f);
			}
			else if (objId == 5) { //antena do topo
				translate(MODEL, 0.0f, 230.0f, 0.0f);
				scale(MODEL, 2.0f, 62.0f, 2.0f);
			}
			else if (objId == 6) { //torus de cima
				translate(MODEL, 0.0f, 169.0f, 0.0f);
				//scale(MODEL, 14.0f, 23.0f, 14.0f);
				scale(MODEL, 12.0f, 6.5f, 12.0f);
				rotate(MODEL, 90, 0, 0, 1);
			}
			else if (objId == 7) { //cilindro da base
				//scale(MODEL, 14.0f, 23.0f, 14.0f);
				//translate(MODEL, 0.0f, 81.0f, 0.0f);
				scale(MODEL, 10.0f, 270.0f, 10.0f);
			}
			else if (objId == 8) { //cilindro do meio nao transparente
				translate(MODEL, 0.0f, 187.0f, 0.0f);
				scale(MODEL, 6.0f, 27.0f, 6.0f);
			}
			else if (objId == 9) { //base da base
				translate(MODEL, 0.0f, 1.5f, 0.0f);
				scale(MODEL, 18.0f, 3.0f, 18.0f);
			}
			else if (objId == 10) { //cilindro da base
				//scale(MODEL, 14.0f, 23.0f, 14.0f);
				translate(MODEL, 0.0f, 81.0f, 0.0f);
				scale(MODEL, 6.0f, 162.0f, 6.0f);
			}

			drawMesh(use);
			popMatrix(MODEL);
			objId--;

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
	}
}

void renderCity() {

	GLint loc;

	glUniform1i(texMode_uniformId, 0);

	//Do city
	for (int i = 0; i < Q_PREDIOS; i++) {
		for (int j = 0; j < Q_PREDIOS; j++) {

			if ((i == 10 && j == 10) || (i == 2 && j == 12) || (i == 2 && j == 13) || (i == 3 && j == 12) || (i == 3 && j == 13) || (i == 4 && j == 20)
				|| (i == 5 && j == 19) || (i == 6 && j == 18) || (i == 7 && j == 17) || (i == 8 && j == 16) || (i == 9 && j == 7)
				|| (i == 10 && j == 6) || (i == 11 && j == 5) || (i == 17 && j == 16) || (i == 8 && j == 0) || (i == 9 && j == 1) || (i == 10 && j == 2)
				|| (i == 11 && j == 3) || (i == 12 && j == 4) || (i == 13 && j == 5) || (i == 14 && j == 6) || (i == 15 && j == 7) || (i == 16 && j == 8)
				|| (i == 17 && j == 9) || (i == 18 && j == 10) || (i == 19 && j == 11) || (i == 20 && j == 12) || (i == 9 && j == 0) || (i == 10 && j == 1)
				|| (i == 11 && j == 2) || (i == 12 && j == 3) || (i == 13 && j == 4) || (i == 14 && j == 5) || (i == 15 && j == 6) || (i == 16 && j == 7)
				|| (i == 17 && j == 8) || (i == 18 && j == 9) || (i == 19 && j == 10) || (i == 20 && j == 11)) {
				alturaPredios[i][j] = 0;
				continue;
			}
			else if (i > 8 && i < 12 && j > 8 && j < 12) {
				continue;
			}
			else {
				// send the material
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
				glUniform4fv(loc, 1, myMeshes[4].mat.ambient);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
				glUniform4fv(loc, 1, myMeshes[4].mat.diffuse);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
				glUniform4fv(loc, 1, myMeshes[4].mat.specular);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
				glUniform1f(loc, myMeshes[4].mat.shininess);
				pushMatrix(MODEL);

				translate(MODEL, i * 48, 0, j * 48);
				scale(MODEL, comprimentoPredio, alturaPredios[i][j], larguraPredio);

				drawMesh(4);

				popMatrix(MODEL);

				//passeio (wip)

				// send the material
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
				glUniform4fv(loc, 1, myMeshes[4].mat.ambient);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
				glUniform4fv(loc, 1, myMeshes[4].mat.diffuse);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
				glUniform4fv(loc, 1, myMeshes[4].mat.specular);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
				glUniform1f(loc, myMeshes[4].mat.shininess);
				pushMatrix(MODEL);

				translate(MODEL, i * 48-2, 0, j * 48-2);
				scale(MODEL, comprimentoPredio+4, 0.95f, larguraPredio+4);

				drawMesh(4);

				popMatrix(MODEL);
			}
		}
	}
}

void renderChao() {

	GLint loc;
	int objId = 0;
	int use = 4;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 10; j++) {
			glUniform1i(texMode_uniformId, 0);
			if (objId > 18) {
				objId++;
				continue;
			}

			// send the material
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, myMeshes[use].mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, myMeshes[use].mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, myMeshes[use].mat.shininess);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");

			if (objId == 0) {
				float dif[] = { 0.36f, 0.73f, 0.89f, 1.0f };
				glUniform4fv(loc, 1, dif);

			}
			else if (objId == 1) {
				float dif[] = { 0.2f, 0.2f, 0.2f, 1.0f };
				glUniform4fv(loc, 1, dif);

			}
			else if (objId == 2 || objId == 3) {
				float dif[] = { 0.2f, 0.66f, 0.32f, 1.0f };
				glUniform4fv(loc, 1, dif);

			}
			else if (objId > 3) {
				float dif[] = { 0.6f, 0.6f, 0.6f, 1.0f };
				glUniform4fv(loc, 1, dif);
			}
			else {
				glUniform4fv(loc, 1, myMeshes[use].mat.diffuse);
			}

			pushMatrix(MODEL);

			if (objId == 0) { //agua
				translate(MODEL, -16.0f, -5.0f, -16.0f);
				scale(MODEL, 1024.0f, 5.0f, 1024.0f);
			}
			else if (objId == 1) { //linha combojo
				translate(MODEL, 81.0f, 0.0f, -16.0f);
				scale(MODEL, 14.0f, 1.1f, 1074.0f);
			}
			else if (objId == 2) { //parque
				translate(MODEL, 96.0f, 0.0f, 576.0f);
				scale(MODEL, 80.0f, 1.1f, 80.0f);
			}
			else if (objId == 3) { //parque cima
				translate(MODEL, 816.0f, 0.0f, 768.0f);
				scale(MODEL, 32.0f, 1.1f, 32.0f);
			}
			else if (objId == 4) {
				translate(MODEL, -2, 0, -2);
				scale(MODEL, 372.0f, 1.0f, 336.0f);
			}
			else if (objId == 5) {
				translate(MODEL, -2.0f, 0.0f, 334.0f);
				scale(MODEL, 420.0f, 1.0f, 324.0f);
			}
			else if (objId == 6) {
				translate(MODEL, -2, 0.0, 670);
				scale(MODEL, 180.0f, 1.0f, 324.0f);
			}
			else if (objId == 7) {
				translate(MODEL, 430, 0.0, 382);
				scale(MODEL, 324.0f, 1.0f, 288.0f);
			}
			else if (objId == 8) {
				translate(MODEL, 430, 0.0, 622);
				scale(MODEL, 564.0f, 1.0f, 372.0f);
			}
			else if (objId == 9) {
				translate(MODEL, 430.0f, 0.0f, 382.0f);
				rotate(MODEL, 45, 0, 1, 0);
				scale(MODEL, 229.1f, 1.0f, 568.5f);
			}
			else if (objId == 10) {
				translate(MODEL, 442.0f, 0.0f, -2.0f);
				rotate(MODEL, 45, 0, 1, 0);
				scale(MODEL, 390.25f, 1.0f, 780.5f);
			}
			else if (objId == 11) {
				translate(MODEL, 370.0f, 0.0f, -2.0f);
				rotate(MODEL, -45, 0, 1, 0);
				scale(MODEL, 297.0f, 1.0f, 297.0f);
			}
			else if (objId == 12) {
				translate(MODEL, 190.0f, 0.0f, 670.0f);
				scale(MODEL, 228.0f, 1.0f, 84.0f);
			}
			else if (objId == 13) {
				translate(MODEL, 418.0, 0.0f, 754.0f);
				rotate(MODEL, -135, 0, 1, 0);
				scale(MODEL, 339.41f, 1.0f, 120.0f);
			}
			else if (objId == 14) {
				translate(MODEL, 178.0f, 0.0f, 670.0f);
				scale(MODEL, 150.0f, 1.0f, 154.0f);
			}
			else if (objId == 15) {
				translate(MODEL, 441.0f , 0.0f, 776.0f);
				rotate(MODEL, -135, 0, 1, 0);
				scale(MODEL, 325.0f, 1.0f, 15.0f);
			}
			else if (objId == 16) {
				translate(MODEL, -2.0f, 0.0f, 994.0f);
				scale(MODEL, 180.0f, 1.0f, 14.21f);
			}
			else if (objId == 17) {
				translate(MODEL, 200.58f, 0.0f, 995.21f);
				scale(MODEL, 793.42f, 1.0f, 13.0f);
			}
			else if (objId == 18) {
				translate(MODEL, 205.58f, 0.0f, 990.21f);
				scale(MODEL, 788.42f, 1.0f, 13.0f);
			}

			drawMesh(use);
			popMatrix(MODEL);
			objId++;

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
	}
	for (int k = 0; k < 50; k++) {
		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, myMeshes[use].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshes[use].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, myMeshes[use].mat.shininess);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		float dif[] = { 0.6f, 0.6f, 0.6f, 1.0f };
		glUniform4fv(loc, 1, dif);

		pushMatrix(MODEL);

		translate(MODEL, 430.0f, 0.0f, 974.0f - k * 4.0f);
		rotate(MODEL, -90, 0, 1, 0);
		scale(MODEL, 20.0f, 1.0f, 208.0f - k * 4.0f);

		drawMesh(use);
		popMatrix(MODEL);

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
}

void updateMisseis() {
	if (nMisseisAtivos > 0) {
		GLint loc;
		for (int i = 0; i < N_MISSEIS_MAX; i++) {
			if (infoMisseis[i][0] != 0.0f) {
				// send the material
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
				glUniform4fv(loc, 1, myMeshes[3].mat.ambient);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
				glUniform4fv(loc, 1, myMeshes[3].mat.diffuse);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
				glUniform4fv(loc, 1, myMeshes[3].mat.specular);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
				glUniform1f(loc, myMeshes[3].mat.shininess);
				pushMatrix(MODEL);

				translate(MODEL, infoMisseis[i][2], infoMisseis[i][3], infoMisseis[i][4]);
				rotate(MODEL, -infoMisseis[i][0], 0, 1, 0);
				rotate(MODEL, infoMisseis[i][1], 0, 0, 1);
				translate(MODEL, 0.0f, -0.75f, 0.0f);
				scale(MODEL, 2.0f, 0.5f, 0.5f);
				rotate(MODEL, 90, 0, 0, 1);

				drawMesh(3);

				popMatrix(MODEL);

				//printf("rotL: %f  rotC: %f", infoMisseis[0][0], infoMisseis[0][1]);

				infoMisseis[i][2] += 1.0f * cos(infoMisseis[i][0] * PI / 180) * cos(infoMisseis[i][1] * PI / 180);
				infoMisseis[i][3] += 1.0f * sin(infoMisseis[i][1] * PI / 180);
				infoMisseis[i][4] += 1.0f * sin(infoMisseis[i][0] * PI / 180) * cos(infoMisseis[i][1] * PI / 180);
			}
		}
	}
}

// ------------------------------------------------------------
//
// Collisions
//
float max(float* values, int numVals) {
	float maxVal = std::numeric_limits<float>::lowest();
	for (int i = 0; i < numVals; i++) {
		if (values[i] > maxVal) {
			maxVal = values[i];
		}
	}
	return maxVal;
}

float min(float* values, int numVals) {
	float minVal = std::numeric_limits<float>::max();
	for (int i = 0; i < numVals; i++) {
		if (values[i] < minVal) {
			minVal = values[i];
		}
	}
	return minVal;
}

float clamp(float value, float min, float max) {
	return std::max(min, std::min(max, value));
}

void updateAABB(AABB* aabb, float rotasaoLado, float rotasaoCima, float posX, float posY, float posZ) {
	glm::vec4 newvert1 = aabb->vert1;
	glm::vec4 newvert2 = aabb->vert2;
	glm::vec4 newvert3 = aabb->vert3;
	glm::vec4 newvert4 = aabb->vert4;
	glm::mat4 trans;
	
	trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, rotasaoCima, glm::vec3(rotasaoLado/360.0, 0.0, (360.0-rotasaoLado)/360.0));
	newvert1 = trans * newvert1;
	newvert2 = trans * newvert2;
	newvert3 = trans * newvert3;
	newvert4 = trans * newvert4;

	trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, -rotasaoLado, glm::vec3(0.0, 1.0, 0.0));
	newvert1 = trans * newvert1;
	newvert2 = trans * newvert2;
	newvert3 = trans * newvert3;
	newvert4 = trans * newvert4;

	trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(posX, posY, posZ));
	newvert1 = trans * newvert1;
	newvert2 = trans * newvert2;
	newvert3 = trans * newvert3;
	newvert4 = trans * newvert4;

	aabb->v1 = newvert1;
	aabb->v2 = newvert2;
	aabb->v3 = newvert3;
	aabb->v4 = newvert4;

	float vertsX[4] = { newvert1[0], newvert2[0], newvert3[0], newvert4[0] };
	float vertsZ[4] = { newvert1[2], newvert2[2], newvert3[2], newvert4[2] };

	float minX = min(vertsX, 4);
	float maxX = max(vertsX, 4);
	float minZ = min(vertsZ, 4);
	float maxZ = max(vertsZ, 4);

	// find tl and br

	aabb->tl[0] = minX;
	aabb->tl[2] = minZ;
	aabb->br[0] = maxX;
	aabb->br[2] = maxZ;

	//printf("tlX:%f tlY:%f brX:%f brY:%f\n", aabb->tl[0], aabb->tl[2], aabb->br[0], aabb->br[2]);
	
	glm::vec4 newvert1A = aabb->vert1A;
	glm::vec4 newvert2A = aabb->vert2A;
	glm::vec4 newvert3A = aabb->vert3A;
	glm::vec4 newvert4A = aabb->vert4A;
	glm::mat4 transA;

	transA = glm::mat4(1.0f);
	transA = glm::rotate(transA, rotasaoCima, glm::vec3(rotasaoLado / 360.0, 0.0, (360.0 - rotasaoLado) / 360.0));
	newvert1A = transA * newvert1A;
	newvert2A = transA * newvert2A;
	newvert3A = transA * newvert3A;
	newvert4A = transA * newvert4A;

	transA = glm::mat4(1.0f);
	transA = glm::rotate(transA, -rotasaoLado, glm::vec3(0.0, 1.0, 0.0));
	newvert1A = transA * newvert1A;
	newvert2A = transA * newvert2A;
	newvert3A = transA * newvert3A;
	newvert4A = transA * newvert4A;

	transA = glm::mat4(1.0f);
	transA = glm::translate(transA, glm::vec3(posX, posY, posZ));
	newvert1A = transA * newvert1A;
	newvert2A = transA * newvert2A;
	newvert3A = transA * newvert3A;
	newvert4A = transA * newvert4A;

	aabb->v1A = newvert1A;
	aabb->v2A = newvert2A;
	aabb->v3A = newvert3A;
	aabb->v4A = newvert4A;

	float vertsXA[4] = { newvert1A[0], newvert2A[0], newvert3A[0], newvert4A[0] };
	float vertsZA[4] = { newvert1A[2], newvert2A[2], newvert3A[2], newvert4A[2] };

	float minXA = min(vertsXA, 4);
	float maxXA = max(vertsXA, 4);
	float minZA = min(vertsZA, 4);
	float maxZA = max(vertsZA, 4);

	// find tl and br

	aabb->tlA[0] = minXA;
	aabb->tlA[2] = minZA;
	aabb->brA[0] = maxXA;
	aabb->brA[2] = maxZA;
}

// Car-butter/cheerio collision
bool checkCollisionBox(float a_xmin, float a_xmax, float a_zmin, float a_zmax, float b_xmin, float b_xmax, float b_zmin, float b_zmax) {
	return (a_xmin <= b_xmax && a_xmax >= b_xmin) &&
		(a_zmin <= b_zmax && a_zmax >= b_zmin);
}

bool checkaColisaoPredios(AABB* aabb) {
	glm::vec4 v1 = aabb->v1;
	glm::vec4 v2 = aabb->v2;
	glm::vec4 v4 = aabb->v4;
	glm::vec4 v1A = aabb->v1A;
	glm::vec4 v2A = aabb->v2A;
	float CorpoY = posisaoY;

	int predioX = 0;
	int predioZ = 0;

	predioX = (int) floor(v1[0] / 48.0f);
	predioZ = (int) floor(v1[2] / 48.0f);

	//printf("v1: %f, %f, %f  v4: %f, %f, %f", v1[0], v1[1], v1[2], v4[0], v4[1], v4[2]);
	//printf("Altura do aviao: %f Altura do predio: %d\n", CorpoZ, alturaPredios[predioX][predioZ]);
	//printf("%d\n", bateu);
	if (((int)v1[0]) % 48 < 32 && ((int)v1[2]) % 48 < 32 && predioX < Q_PREDIOS && predioZ < Q_PREDIOS && predioX >= 0 && predioZ >= 0) {
		if (v1[1] < alturaPredios[predioX][predioZ]) {
			return true;
		}
	}
	else {
		if (CorpoY < 0.0f) {
			return true;
		}
	}

	predioX = (int)floor(v2[0] / 48.0f);
	predioZ = (int)floor(v2[2] / 48.0f);

	// cabeso do aviao
	if (((int)v2[0]) % 48 < 32 && ((int)v2[2]) % 48 < 32 && predioX < Q_PREDIOS && predioZ < Q_PREDIOS && predioX >= 0 && predioZ >= 0) {
		if (v2[1] < alturaPredios[predioX][predioZ]) {
			return true;
		}
	}
	else {
		if (CorpoY < 0.0f) {
			return true;
		}
	}

	predioX = (int)floor(v1A[0] / 48.0f);
	predioZ = (int)floor(v1A[2] / 48.0f);

	//printf("Asa1: %f, %f  Asa2: %f, %f", v1A[0], v1A[2], v2A[0], v2A[2]);
	//printf("%f\n", v1A[2]);
	//asa do aviao
	if (((int)v1A[0]) % 48 < 32 && ((int)v1A[2]) % 48 < 32 && predioX < Q_PREDIOS && predioZ < Q_PREDIOS && predioX >= 0 && predioZ >= 0) {
		if (v1A[1] < alturaPredios[predioX][predioZ]) {
			return true;
		}
	}
	else {
		if (CorpoY < 0.0f) {
			return true;
		}
	}

	predioX = (int)floor(v2A[0] / 48.0f);
	predioZ = (int)floor(v2A[2] / 48.0f);

	//printf("%f\n", alturaPredios[predioX][predioZ]);
	//asa do aviao
	if (((int)v2A[0]) % 48 < 32 && ((int)v2A[2]) % 48 < 32 && predioX < Q_PREDIOS && predioZ < Q_PREDIOS && predioX >= 0 && predioZ >= 0) {
		if (v2A[1] < alturaPredios[predioX][predioZ]) {
			return true;
		}
	}
	else {
		if (CorpoY < 0.0f) {
			return true;
		}
	}
	return false;
}

bool checkaColisao(float ballX, float ballZ, float ballRadius, glm::vec4 v1, glm::vec4 v2, glm::vec4 v3, glm::vec4 v4) {
	float alturaCheerio = 0.75;
	float minBallX = ballX - ballRadius;
	float maxBallX = ballX + ballRadius;
	float minBallZ = ballZ - ballRadius;
	float maxBallZ = ballZ + ballRadius;
	return ((v1[0] >= minBallX && v1[0] <= maxBallX && v1[2] >= minBallZ && v1[2] <= maxBallZ) ||
		(v2[0] >= minBallX && v2[0] <= maxBallX && v2[2] >= minBallZ && v2[2] <= maxBallZ) ||
		(v3[0] >= minBallX && v3[0] <= maxBallX && v3[2] >= minBallZ && v3[2] <= maxBallZ) ||
		(v4[0] >= minBallX && v4[0] <= maxBallX && v4[2] >= minBallZ && v4[2] <= maxBallZ)) && v1[1] <= alturaCheerio; //adicionei esta condicao para ele poder saltar
}

// plane-balls collision
bool checkCollisionBall(float ballX, float ballZ, float ballRadius, glm::vec4 topLeft, glm::vec4 bottomRight) {
	// get center point circle first
	float boxX = topLeft[0], boxZ = topLeft[2];
	float boxSizeX = std::max(bottomRight[0], topLeft[0]) - std::min(bottomRight[0], topLeft[0]);
	float boxSizeZ = std::max(bottomRight[2], topLeft[2]) - std::min(bottomRight[2], topLeft[2]);

	float aabb_half_extent_x = boxSizeX / 2.0f;
	float aabb_half_extent_z = boxSizeZ / 2.0f;
	float aabb_center_x = boxX + aabb_half_extent_x;
	float aabb_center_z = boxZ + aabb_half_extent_z;

	float differenceX = ballX - aabb_center_x;
	float differenceZ = ballZ - aabb_center_z;
	float clampedX = clamp(differenceX, -aabb_half_extent_x, aabb_half_extent_x);
	float clampedZ = clamp(differenceZ, -aabb_half_extent_z, aabb_half_extent_z);

	float closestX = aabb_center_x + clampedX;
	float closestZ = aabb_center_z + clampedZ;
	float distance[3] = { closestX - ballX, closestZ - ballZ, 0.0f };	// third coord for length func

	return length(distance) < ballRadius;
}

void handleCollisions() {
	updateAABB(&aabb, (rotasaoLado * PI / 180), (rotacaoCima * PI / 180), posisaoX, posisaoY, posisaoZ);
	bateu = checkaColisaoPredios(&aabb);
}
// ------------------------------------------------------------
//
// Render stufff
//

void renderScene(void) {

	GLint loc;

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);

	if (estaAcelerar && acelerasao < 2.0f) {
		acelerasao += 0.05f;
	}
	else {
		if (acelerasao > 0.0f) {
			acelerasao -= 0.02f;
		}
	}
	
	//atualizar o buffer circular (atraso da camera)
	camIndex = (camIndex + 1) % 20;

	posIndex = (posIndex + 1) % 20;
	atrasoCamera[posIndex][0] = posisaoX;
	atrasoCamera[posIndex][1] = posisaoY;
	atrasoCamera[posIndex][2] = posisaoZ;

	
	// use our shader
	glUseProgram(shader.getProgramIndex());

	//Associar os Texture Units aos Objects Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);
	//Indicar aos samplers do GLSL quais os Texture Units a serem usados
	glUniform1i(tex_loc, 0);


	float ratio = (float)WinX / WinY;
	int dirCamera = 1;
	// set the camera using a function similar to gluLookAt
	switch (camera) {
	case 0:
		if (rotacaoCima <= 295.5f && rotacaoCima > 115.5f) {
			dirCamera = -1;
		}
		// camera movel com motion sickness
		lookAt(atrasoCamera[camIndex][0] - (10 * cos(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180)) + 5 * sin(-rotacaoCima * PI / 180) * cos(rotasaoLado * PI / 180), atrasoCamera[camIndex][1] - 10 * sin((rotacaoCima)*PI / 180) + 5 * cos(-rotacaoCima * PI / 180), atrasoCamera[camIndex][2] - (10 * sin(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180)) + 5 * sin(-rotacaoCima * PI / 180) * sin(rotasaoLado * PI / 180), posisaoX, posisaoY, posisaoZ, 0, 1 * dirCamera, 0);
		//posisaoCamera[0] = (posisaoX - 3 * cos(-rotasaoLado * PI / 180)) + camX;
		//posisaoCamera[1] = (posisaoZ - 3 * sin(-rotasaoLado * PI / 180)) + camZ;
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		break;
	case 1:
		//camera de cima orto
		lookAt(496.0, 1, 496.0, 496.0, 0.0, 496.0, 1, 0, 0);
		loadIdentity(PROJECTION);
		ortho(-504 * ratio, 504 * ratio, -504, 504, -1000, 1000);
		break;
	case 2:
		//camera de cima perspetiva
		lookAt(496.0, 400.0, 496.0, 496.0, 0.0, 496.0, 1, 0, 0);
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		break;
	case 3:
		if (rotacaoCima <= 295.5f && rotacaoCima > 115.5f){
			dirCamera = -1;
		}
		// camera movel
		lookAt(posisaoX - (10 * cos(rotasaoLado * PI / 180)*cos(rotacaoCima* PI /180)) + 5*sin(-rotacaoCima * PI/180) * cos(rotasaoLado * PI / 180), posisaoY - 10 * sin((rotacaoCima) * PI / 180) + 5 * cos(-rotacaoCima * PI /180), posisaoZ - (10 * sin(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180)) + 5 * sin(-rotacaoCima * PI / 180) * sin(rotasaoLado * PI / 180), posisaoX, posisaoY, posisaoZ, 0, 1*dirCamera, 0);
		//posisaoCamera[0] = (posisaoX - 3 * cos(-rotasaoLado * PI / 180)) + camX;
		//posisaoCamera[1] = (posisaoZ - 3 * sin(-rotasaoLado * PI / 180)) + camZ;
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		break;
	case 4:
		/*
		//camera de cima tudo
		lookAt(TAM_TABULEIRO / 2, TAM_TABULEIRO, TAM_TABULEIRO / 2, TAM_TABULEIRO / 2, 0.0, TAM_TABULEIRO / 2, 1, 0, 0);
		loadIdentity(PROJECTION);
		ortho(-50 * ratio, 50 * ratio, -50, 50, -300, 300);*/
		break;
	case 5:
		/*
		// camera movel sem rato
		lookAt(posisaoAtualX - 3 * cos(-rotasao * PI / 180), 2.0, posisaoAtualY - 3 * sin(-rotasao * PI / 180), posisaoAtualX, 1.0, posisaoAtualY, 0, 1, 0);
		posisaoCamera[0] = posisaoAtualX - 3 * cos(-rotasao * PI / 180);
		posisaoCamera[1] = posisaoAtualY - 3 * sin(-rotasao * PI / 180);
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);*/
		break;
	case 6:
		/*
		// camera movel sem rato dentro do camião
		lookAt((posisaoAtualX + 3.1 * cos((-rotasao) * PI / 180)) + 0.4 * sin(rotasao * PI / 180), 0.6 + jump, (posisaoAtualY + 3.1 * sin((-rotasao) * PI / 180)) + 0.4 * cos(rotasao * PI / 180), (posisaoAtualX + 5.0 * cos((-rotasao) * PI / 180)) + 0.4 * sin(rotasao * PI / 180), 0.5 + jump, (posisaoAtualY + 5.0 * sin((-rotasao) * PI / 180)) + 0.4 * cos(rotasao * PI / 180), 0, 1, 0);
		posisaoCamera[0] = (posisaoAtualX + 3.1 * cos((-rotasao) * PI / 180)) + 0.4 * sin(rotasao * PI / 180);
		posisaoCamera[1] = (posisaoAtualY + 3.1 * sin((-rotasao) * PI / 180)) + 0.4 * cos(rotasao * PI / 180);

		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);

		//mete primeiro o espelho no stencil a 2
		glEnable(GL_STENCIL_TEST);
		renderEspelho();
		glDisable(GL_STENCIL_TEST);*/
		break;
	}
	// use our shader
	glUseProgram(shader.getProgramIndex());

		//send the light position in eye coordinates
		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

		float res[4];
		multMatrixPoint(VIEW, lightPos,res);   //lightPos definido em World Coord so is converted to eye space
		glUniform4fv(lPos_uniformId, 1, res);

	int objId=0; //id of the object mesh - to be used as index of mesh: Mymeshes[objID] means the current mesh

	applyRotation();

	renderChao();

	renderAviao();

	renderCity();

	renderTorre();

	updateMisseis();

	handleCollisions();
	
	if (!bateu) {
		posisaoX += (0.3f + acelerasao) * cos(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180);
		posisaoY += (0.3f + acelerasao) * sin(rotacaoCima * PI / 180);
		posisaoZ += (0.3f + acelerasao) * sin(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180);
	}

	//renderTexto();

	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{

	//Shift acelera o bicho
	int shift = glutGetModifiers();

	switch (key) {

	case 27:
		glutLeaveMainLoop();
		break;

	case 'c':
		printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
		break;
	case 'm': glEnable(GL_MULTISAMPLE); break;
	case 'n': glDisable(GL_MULTISAMPLE); break;
	case 'a':
		if (rotPlaneH > -45.0f) {
			rotPlaneH -= 1.0f;
			isRotatingH = true;
		}
		o = true;
		break;
	case 'd':
		if (rotPlaneH < 45.0f) {
			rotPlaneH += 1.0f;
			isRotatingH = true;
		}
		p = true;
		break;
	case 'w':
		if (rotPlaneV > -45.0f) {
			rotPlaneV -= 1.0f;
			isRotatingV = true;
		}
		q = true;
		break;
	case 's':
		if (rotPlaneV < 45.0f) {
			rotPlaneV += 1.0f;
			isRotatingV = true;
		}
		a = true;
		break;
	case 'h':
		estaAcelerar = true;
		break;
	case '0':
		camera = 0;
		break;
	case '1':
		camera = 1;
		break;
	case '2':
		camera = 2;
		break;
	case '3':
		camera = 3;
		break;
	case 32: //espaso
		infoMisseis[missilIndex][0] = rotasaoLado + 0.0f;
		infoMisseis[missilIndex][1] = rotacaoCima + 0.0f;
		infoMisseis[missilIndex][2] = posisaoX + 0.0f;
		infoMisseis[missilIndex][3] = posisaoY + 0.0f;
		infoMisseis[missilIndex][4] = posisaoZ + 0.0f;
		missilIndex = (missilIndex + 1) % N_MISSEIS_MAX;
		nMisseisAtivos += 1;
	}
}

void processKeysUp(unsigned char key, int xx, int yy) {
	switch (key) {
	case 'a':
		o = false;
		isRotatingH = false;
		break;
	case 'd':
		p = false;
		isRotatingH = false;
		break;
	case 'w':
		q = false;
		isRotatingV = false;
		break;
	case 's':
		a = false;
		isRotatingV = false;
		break;
	case 'h':
		estaAcelerar = false;
	}
}

// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
			if (r < 0.1f)
				r = 0.1f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;
		rAux = r;
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);
		if (rAux < 0.1f)
			rAux = 0.1f;
	}

	camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
	camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

//  uncomment this if not using an idle or refresh func
//	glutPostRedisplay();
}

// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/pointlight.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/pointlight.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	//glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());

	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode");
	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	
	printf("InfoLog for Per Fragment Phong Lightning Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	// Shader for bitmap Text
	shaderText.init();
	shaderText.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/text.vert");
	shaderText.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/text.frag");

	glLinkProgram(shaderText.getProgramIndex());
	printf("InfoLog for Text Rendering Shader\n%s\n\n", shaderText.getAllInfoLogs().c_str());
	
	return(shader.isProgramLinked() && shaderText.isProgramLinked());
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void init()
{
	MyMesh amesh;

	/* Initialization of DevIL */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		printf("wrong DevIL version \n");
		exit(0);
	}
	ilInit();

	/// Initialization of freetype library with font_name file
	freeType_init(font_name);

	// set the camera position based on its spherical coordinates
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

	//texturas
	glGenTextures(1, TextureArray);
	Texture2D_Loader(TextureArray, "textures/Flor.png", 0);
	
	float amb[] = { 0.2f, 0.0f, 0.2f, 1.0f };
	float diff[] = { 1.0f, 0.86, 0.97f, 1.0f };
	float spec[] = {0.8f, 0.8f, 0.8f, 1.0f};
	float emissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float shininess= 100.0f;
	int texcount = 0;

	// create geometry and VAO of the torus  0
	amesh = createTorus(2.0f, 18.0f, 6, 8);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	float amb1[]= {0.3f, 0.0f, 0.0f, 1.0f};
	float diff1[] = { 0.8f, 0.0f, 0.6f, 1.0f };
	float spec1[] = {0.9f, 0.9f, 0.9f, 1.0f};
	shininess=500.0;

	// create geometry and VAO of the cylinder    1
	amesh = createCylinder(1.0f, 1.0f, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec1, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	// create geometry and VAO of the Cone     2
	amesh = createCone(1.0f, 1.0f, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	// create geometry and VAO of the Sphere     3
	amesh = createSphere(0.5f, 10);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	float diff2[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	// create geometry and VAO of the Cube    4
	amesh = createCube();
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff2, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	// create geometry and VAO of the Quad    5
	amesh = createQuad(1.0f, 1.0f);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	// create geometry and VAO of the sphere for skytree tower   6
	amesh = createSphere(1.0f, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.81f, 0.91f, 0.98f, 1.0f);

	srand(time(NULL));

	for (int i = 0; i < Q_PREDIOS; i++) {
		for (int j = 0; j < Q_PREDIOS; j++) {
			if (i > 8 && i < 12 && j > 8 && j < 12) {
				alturaPredios[i][j] = 0;
			}
			else {
				alturaPredios[i][j] = (rand() % 75) + 25;
			}
		}
	}
	//ola
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 3; j++) {
			if (j == 0)
				atrasoCamera[i][j] = posisaoX;
			else if (j == 1)
				atrasoCamera[i][j] = posisaoY;
			else if (j == 2)
				atrasoCamera[i][j] = posisaoZ;
		}
	}

}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (4, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);


//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutTimerFunc(0, timer, 0);
	//glutIdleFunc(renderScene);  // Use it for maximum performance
	glutTimerFunc(0, refresh, 0);    //use it to to get 60 FPS whatever

//	Mouse and Keyboard Callbacks
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processKeys);
	glutKeyboardUpFunc(processKeysUp);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc ( mouseWheel ) ;
	

//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);
}