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

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>


// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <IL/il.h>


// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "geometry.h"

#include "avtFreeType.h"

using namespace std;

#define CAPTION "CGJ Demo: Phong Shading and Text rendered with FreeType"
int WindowHandle = 0;
int WinX = 1024, WinY = 768;
int ambientFlag = 1, diffuseFlag = 1, holoFlag = 1;
#define Q_PREDIOS 20

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

GLint pvm_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId, lPos1_uniformId, lPos2_uniformId, lPos3_uniformId, lPos4_uniformId, lPos5_uniformId, lPos6_uniformId, lPos7_uniformId, lPos8_uniformId;
GLint ambF, difF, holF;
GLint tex_loc, tex_loc1, tex_loc2;


// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 39.0f, beta = 51.0f;
float r = 10.0f;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];


//cenas do jogo em si
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

float atrasoCamera[100][3];
int posIndex = 0;
int camIndex = 0;
int camera = 3;
bool cameraAnda = false;
int alturaPredios[20][20];
int larguraPredio = 32;
int comprimentoPredio = 32;
int renderFlag = 0;

float lightPos[4] = { 496.0f, 400.0f, 496.0f, 1.0f };
float lightPos1[4] = { 2000.0f, 200.0f, 2.0f, 1.0f };
float lightPos2[4] = { 10.0f, 200.0f, 3000.0f, 1.0f };
float lightPos3[4] = { 1200.0f, 200.0f, 1200.0f, 1.0f };
float lightPos4[4] = { 20.0f, 200.0f, 2.0f, 1.0f };
float lightPos5[4] = { 4.0f, 200.0f, 2.0f, 1.0f };
float lightPos6[4] = { 1.0f, 1.0f, 0.0f, 0.0f };


#define PI 3.14159265


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

void refresh(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, refresh, 0);
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	if (h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix
	ratio = (1.0f * w) / h;
	loadIdentity(PROJECTION);
	perspective(53.13f, ratio, 0.1f, 1000.0f);
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

void renderAviao() {
	int objId = 0;
	GLint loc;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 4; ++j) {
			int use = 0;

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


// ------------------------------------------------------------
//
// Render stufff
//
void renderTorre() {

	GLint loc;
	int objId = 11;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 6; ++j) {
			int use = 0;
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


	//Do city
	for (int i = 0; i < Q_PREDIOS; i++) {
		for (int j = 0; j < Q_PREDIOS; j++) {

			if (i == 10 && j == 10) {
				renderTorre();
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
			}
		}
	}
}


void renderScene(void) {

	GLint loc;

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	// set the camera using a function similar to gluLookAt
	lookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);
	// use our shader
	glUseProgram(shader.getProgramIndex());

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
	float ratio = (float)WinX / WinY;
	int dirCamera = 1;
	switch (camera) {
	case 0:
		if (rotacaoCima <= 295.5f && rotacaoCima > 115.5f) {
			dirCamera = -1;
		}
		// camera movel com motion sickness
		lookAt(atrasoCamera[camIndex][0] - (10 * cos(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180)) + 5 * sin(-rotacaoCima * PI / 180) * cos(rotasaoLado * PI / 180), atrasoCamera[camIndex][1] - 10 * sin((rotacaoCima)*PI / 180) + 5 * cos(-rotacaoCima * PI / 180), atrasoCamera[camIndex][2] - (10 * sin(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180)) + 5 * sin(-rotacaoCima * PI / 180) * sin(rotasaoLado * PI / 180), atrasoCamera[camIndex][0], atrasoCamera[camIndex][1], atrasoCamera[camIndex][2], 0, 1 * dirCamera, 0);
		//posisaoCamera[0] = (posisaoX - 3 * cos(-rotasaoLado * PI / 180)) + camX;
		//posisaoCamera[1] = (posisaoZ - 3 * sin(-rotasaoLado * PI / 180)) + camZ;
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		break;
	case 1:
		/*
		//camera de cima orto
		lookAt(38.0, 30.0, 68.0, 38.0, 0.0, 68.0, 1, 0, 0);
		loadIdentity(PROJECTION);
		ortho(-640 * ratio / 40, 640 * ratio / 40, -640 / 40, 640 / 40, -100, 100);*/
		break;
	case 2:
		//camera de cima perspetiva
		lookAt(-3.0, 125.0f, 0.0f, 38.0, 0.0, 68.0, 1, 0, 0);
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		break;
	case 3:
		if (rotacaoCima <= 295.5f && rotacaoCima > 115.5f) {
			dirCamera = -1;
		}
		// camera movel
		lookAt(posisaoX - (10 * cos(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180)) + 5 * sin(-rotacaoCima * PI / 180) * cos(rotasaoLado * PI / 180), posisaoY - 10 * sin((rotacaoCima)*PI / 180) + 5 * cos(-rotacaoCima * PI / 180), posisaoZ - (10 * sin(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180)) + 5 * sin(-rotacaoCima * PI / 180) * sin(rotasaoLado * PI / 180), posisaoX, posisaoY, posisaoZ, 0, 1 * dirCamera, 0);
		//posisaoCamera[0] = (posisaoX - 3 * cos(-rotasaoLado * PI / 180)) + camX;
		//posisaoCamera[1] = (posisaoZ - 3 * sin(-rotasaoLado * PI / 180)) + camZ;
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		break;
	}

	//send the light position in eye coordinates
	//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

	glUniform1i(ambF, ambientFlag);
	glUniform1i(difF, diffuseFlag);
	glUniform1i(holF, holoFlag);
	float res[4];
	multMatrixPoint(VIEW, lightPos, res);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos_uniformId, 1, res);

	float res2[4];
	multMatrixPoint(VIEW, lightPos1, res2);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos1_uniformId, 1, res2);

	float res3[4];
	multMatrixPoint(VIEW, lightPos2, res3);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos2_uniformId, 1, res3);

	float res4[4];
	multMatrixPoint(VIEW, lightPos3, res4);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos3_uniformId, 1, res4);

	float res5[4];
	multMatrixPoint(VIEW, lightPos4, res5);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos4_uniformId, 1, res5);

	float res6[4];
	multMatrixPoint(VIEW, lightPos5, res6);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos5_uniformId, 1, res6);

	float res7[4];
	multMatrixPoint(VIEW, lightPos6, res7);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos6_uniformId, 1, res7);

	float lightPos7[4] = { posisaoX, posisaoY, posisaoZ, 1.0f }; //tem que ser posto aqui porque sao valores dinamicos
	float lightPos8[4] = { posisaoX, posisaoY, posisaoZ, 1.0f };

	float res8[4];
	multMatrixPoint(VIEW, lightPos7, res8);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos7_uniformId, 1, res8);

	float res9[4];
	multMatrixPoint(VIEW, lightPos8, res9);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos8_uniformId, 1, res9);

	int objId = 0; //id of the object mesh - to be used as index of mesh: Mymeshes[objID] means the current mesh

	renderAviao();
	renderCity();
	applyRotation();

	posisaoX += (0.3f + acelerasao) * cos(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180);
	posisaoY += (0.3f + acelerasao) * sin(rotacaoCima * PI / 180);
	posisaoZ += (0.3f + acelerasao) * sin(rotasaoLado * PI / 180) * cos(rotacaoCima * PI / 180);

	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	switch (key) {

	case 27:
		glutLeaveMainLoop();
		break;

	case '9':
		printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
		break;
	case 'm': glEnable(GL_MULTISAMPLE); break;
	case 'k': glDisable(GL_MULTISAMPLE); break;
	case 'n': if (ambientFlag == 1) { ambientFlag = 0; }
			else ambientFlag = 1; break;
	case 'c': if (diffuseFlag == 1) { diffuseFlag = 0; }
			else diffuseFlag = 1; break;
	case 'h': if (holoFlag == 1) { holoFlag = 0; }
			else holoFlag = 1; break;
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
	case 'f':
		estaAcelerar = true;
		break;
	case '3':
		camera = 3;
		break;
	case '0':
		camera = 0;
		break;
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
	case 'f':
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
	if (state == GLUT_DOWN) {
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

	deltaX = -xx + startX;
	deltaY = yy - startY;

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
	camY = rAux * sin(betaAux * 3.14f / 180.0f);

	//  uncomment this if not using an idle or refresh func
	//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);

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
	glBindFragDataLocation(shader.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	//glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());


	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos[0]");
	lPos1_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos[1]");
	lPos2_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos[2]");
	lPos3_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos[3]");
	lPos4_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos[4]");
	lPos5_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos[5]");
	lPos6_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos[6]");
	lPos7_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos[7]");
	lPos8_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos[8]");
	ambF = glGetUniformLocation(shader.getProgramIndex(), "luzAmbiente");
	difF = glGetUniformLocation(shader.getProgramIndex(), "luzDifusa");
	holF = glGetUniformLocation(shader.getProgramIndex(), "luzHolofote");
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
	camY = r * sin(beta * 3.14f / 180.0f);


	float amb[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float diff[] = { 1.0f, 0.86, 0.97f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
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

	float amb1[] = { 0.3f, 0.0f, 0.0f, 1.0f };
	float diff1[] = { 0.8f, 0.0f, 0.6f, 1.0f };
	float spec1[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	shininess = 500.0;

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

	for (int i = 0; i < Q_PREDIOS; i++) {
		for (int j = 0; j < Q_PREDIOS; j++) {
			alturaPredios[i][j] = rand() % 75 + 25;
		}
	}

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


int main(int argc, char** argv) {

	//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100, 100);
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
	glutMouseWheelFunc(mouseWheel);


	//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	if (!setupShaders())
		return(1);

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);
}



