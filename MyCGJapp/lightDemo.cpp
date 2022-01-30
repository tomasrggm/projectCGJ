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
#include <fstream>
#include <time.h>

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/scene.h"

using namespace std;

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
#include "meshFromAssimp.h"
#include "AABB.h"

#include "avtFreeType.h"
#include "l3dBillboard.h"
#include <flare.h>

using namespace std;

/* IMPORTANT: Use the next data to make this Assimp demo to work*/

// Created an instance of the Importer class in the meshFromAssimp.cpp file
extern Assimp::Importer importer;
// the global Assimp scene object
extern const aiScene* scene;
//char model_dir[50];  //initialized by the user input at the console
char model_dir[] = { 'T', 'r', 'a', 'i', 'n', 's' };
// scale factor for the Assimp model to fit in the window
extern float scaleFactor;

#define frand()			((float)rand()/RAND_MAX)
#define MAX_PARTICULAS  225
#define CAPTION "Tokyo drift"
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
vector<struct MyMesh> myMeshes2; //para o assimp

//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint texMode_uniformId;
GLint pvm_uniformId;
GLint vm_uniformId;
GLint m_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId;
GLint fogF;
GLint tex_loc, tex_loc1, tex_loc2, tex_loc3, tex_loc4, tex_loc5, tex_loc6, tex_loc7, tex_loc8, tex_loc9, tex_loc10, tex_loc11, tex_loc12, tex_loc13, tex_loc14, tex_normalMap_loc, tex_loc16;
GLint luzLocal1_loc, luzLocal2_loc, luzLocal3_loc, luzLocal4_loc, luzLocal5_loc, luzLocal6_loc, luzLocal7_loc, luzLocal8_loc, luzLocal9_loc, luzLocal10_loc, luzLocal11_loc, luzLocal12_loc, luzLocal13_loc, luzLocal14_loc, luzLocal15_loc, luzLocal16_loc;
GLint tex_cube_loc;
GLint view_uniformId;
GLint dia;
GLint pointLights;
GLint corVariavel_loc;
GLint luzBofia_loc;
GLint spotLightDir_loc;
GLint mapa;
GLint assimp;

GLuint TextureArray[17];

//Particles
int fireworks = 1;
typedef struct {
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // color
	GLfloat x, y, z;    // posi‹o
	GLfloat vx, vy, vz; // velocidade 
	GLfloat ax, ay, az; // acelera‹o
} Particle;

Particle particula[8][MAX_PARTICULAS];
int dead_num_particles = 0;
int fireworkCounter = 0;
int aviaoExplodido[3];
bool isExplosion;
float cores[8][3] = { {1,0.32,0.76},
					{1,0.32,0.32},
					{0.71,0.32,1},
					{0.32,0.8,1},
					{0.32,1,0.84},
					{0.32,1,0.35},
					{1,0.93,0.32},
					{1,0.66,0.22} };

//Variaveis fun mode
bool minigame = false;

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
float rotasaoCima = 0.0f;
float acelerasao = 0.0f;
bool pause = false;
bool nossoAviao = false;
int vidas = 3;
int pontos = 0;
int imunidade;
int pontosCounter;
bool verAviao;

//variaveis para os avioes inimigos
float posisaoXUm = posisaoX;
float posisaoYUm = posisaoY;
float posisaoZUm = posisaoZ;
float rotasaoLadoUm = 0.0f;
float rotasaoCimaUm = 0.0f;

float posisaoXDois = posisaoX;
float posisaoYDois = posisaoY;
float posisaoZDois = posisaoZ;
float rotasaoLadoDois = 0.0f;
float rotasaoCimaDois = 0.0f;

float posisaoXTres = posisaoX;
float posisaoYTres = posisaoY;
float posisaoZTres = posisaoZ;
float rotasaoLadoTres = 0.0f;
float rotasaoCimaTres = 0.0f;

//spide do comboio
float posicaoComboio = 0.0f;

//variaveis criadas para a spotLight
float spotLights[4] = { 0.0f,0.0f,0.0f, 1.0f };
float spotLightDirection[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLint spotLight1;
GLint spotDirection;
GLint spotlightOn;

//variaveis criadas para os misseis
int nMisseisAtivos = 0;
#define N_MISSEIS_MAX 20
float infoMisseis[N_MISSEIS_MAX][5];
float infoMisseisUm[N_MISSEIS_MAX][5];
float infoMisseisDois[N_MISSEIS_MAX][5];
float infoMisseisTres[N_MISSEIS_MAX][5];
int missilIndex;
int missilIndexUm;
int missilIndexDois;
int missilIndexTres;
int missilTimer;

//variaveis criadas para a camera
float atrasoCamera[20][3];
int posIndex = 0;
int camIndex = 1;
int camera = 3;
float rotLCam = 0.0f;
float rotCCam = 0.0f;

// Variaveis criadas para a cidade
#define Q_PREDIOS 21
int alturaPredios[Q_PREDIOS][Q_PREDIOS];
int larguraPredio = 32;
int comprimentoPredio = 32;
int fogFlag = 0;

//Variaveis criadas para o mini mapa
float posisaoXMapa = posisaoX;
float posisaoYMapa = posisaoY;
float posisaoZMapa = posisaoZ;

//bounding boxes do aviao
AABB aabb;
bool bateu = false;
AABB aabbUm;
AABB aabbDois;
AABB aabbTres;

int desenhaMapa = 0;
int diaLigado = 1;
int pointLightsLigadas = 0;
int rotasaoLampada = 0;

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
long myTime, timebase = 0, frame = 0;
char s[32];
float lightPos[4] = {496.0f, 400.0f, 496.0f, 0.0f};
float luzesLocais[14][4] = { {504.66f, 205.0f, 501.00f, 1.0},
							{504.66f, 205.0f, 491.00f, 1.0},
							{496.00f, 205.0f, 485.50f, 1.0},
							{487.00f, 205.0f, 490.50f, 1.0},
							{487.00f, 205.0f, 501.50f, 1.0},
							{496.00f, 205.0f, 506.50f, 1.0},
							{493.2f, 271.0f, 496.00f, 1.0},
							{497.2f, 271.0f, 496.0f, 1.0},
							{496.0f, 130.0f, 490.50f, 1.0},
							{496.0f, 130.0f, 505.0f, 1.0},
							{496.0f, 130.0f, 485.0f, 1.0},
							{496.0f, 130.0f, 496.0f, 1.0},
							//{120.0f, 3.0f, 120.0f, 1.0} 
							{88.0f, 3.0f, 88.0f, 1.0},
							{497.2f, 271.0f, 496.0f, 1.0},
							};
int contadorBofia = 0;
int luzBofia = 0;
float corLuz[4] = { 1.0f, 0.0f, 1.0f , 1.0 };
int luzIndice = 0;
int mudaLuz = -1;
int luzSpot = 0;

//delta de tempo para nao prender velocidade a framerate
float oldTimeSinceStart = 0;
float timeSinceStart = 0;
float deltaTime = 0;


//signs
#define N_SIGNS 50
int signsX[N_SIGNS];
int signsZ[N_SIGNS];
int signTypes[N_SIGNS];
int temSign[Q_PREDIOS][Q_PREDIOS];
int orientasao[N_SIGNS];

//aros de velocidade
#define N_AROS 50
int arosX[N_AROS];
int arosZ[N_AROS];
int aroTypes[N_AROS];
int alturaAro[N_AROS];
int arosInfo[37][3];
//lens flare
inline double clamp(const double x, const double min, const double max) {
	return (x < min ? min : (x > max ? max : x));
}

inline int clampi(const int x, const int min, const int max) {
	return (x < min ? min : (x > max ? max : x));
}

GLuint FlareTextureArray[5];

//Flare effect
FLARE_DEF AVTflare;
float lightScreenPos[3];  //Position of the light in Window Coordinates
bool spotlight_mode = false;
bool flareEffect = false;
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


void render_flare(FLARE_DEF* flare, int lx, int ly, int* m_viewport) {  //lx, ly represent the projected position of light on viewport

	int     dx, dy;          // Screen coordinates of "destination"
	int     px, py;          // Screen coordinates of flare element
	int		cx, cy;
	float    maxflaredist, flaredist, flaremaxsize, flarescale, scaleDistance;
	int     width, height, alpha;    // Piece parameters;
	int     i;
	float	diffuse[4];

	GLint loc;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int screenMaxCoordX = m_viewport[0] + m_viewport[2] - 1;
	int screenMaxCoordY = m_viewport[1] + m_viewport[3] - 1;

	//viewport center
	cx = m_viewport[0] + (int)(0.5f * (float)m_viewport[2]) - 1;
	cy = m_viewport[1] + (int)(0.5f * (float)m_viewport[3]) - 1;

	// Compute how far off-center the flare source is.
	maxflaredist = sqrt(cx * cx + cy * cy);
	flaredist = sqrt((lx - cx) * (lx - cx) + (ly - cy) * (ly - cy));
	scaleDistance = (maxflaredist - flaredist) / maxflaredist;
	flaremaxsize = (int)(m_viewport[2] * flare->fMaxSize);
	flarescale = (int)(m_viewport[2] * flare->fScale);

	// Destination is opposite side of centre from source
	dx = clampi(cx + (cx - lx), m_viewport[0], screenMaxCoordX);
	dy = clampi(cy + (cy - ly), m_viewport[1], screenMaxCoordY);

	// Render each element. To be used Texture Unit 0

	glUniform1i(tex_loc13, 0);  //use TU 0

	for (i = 0; i < flare->nPieces; ++i)
	{
		// Position is interpolated along line between start and destination.
		px = (int)((1.0f - flare->element[i].fDistance) * lx + flare->element[i].fDistance * dx);
		py = (int)((1.0f - flare->element[i].fDistance) * ly + flare->element[i].fDistance * dy);
		px = clampi(px, m_viewport[0], screenMaxCoordX);
		py = clampi(py, m_viewport[1], screenMaxCoordY);

		// Piece size are 0 to 1; flare size is proportion of screen width; scale by flaredist/maxflaredist.
		width = (int)(scaleDistance * flarescale * flare->element[i].fSize);

		// Width gets clamped, to allows the off-axis flaresto keep a good size without letting the elements get big when centered.
		if (width > flaremaxsize)  width = flaremaxsize;

		height = (int)((float)m_viewport[3] / (float)m_viewport[2] * (float)width);
		memcpy(diffuse, flare->element[i].matDiffuse, 4 * sizeof(float));
		diffuse[3] *= scaleDistance;   //scale the alpha channel

		if (width > 1)
		{
			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, diffuse);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FlareTextureArray[flare->element[i].textureId]);
			pushMatrix(MODEL);
			translate(MODEL, (float)(px - width * 0.0f), (float)(py - height * 0.0f), 0.0f);
			scale(MODEL, (float)width, (float)height, 1);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(myMeshes[8].vao);
			glDrawElements(myMeshes[8].type, myMeshes[8].numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			popMatrix(MODEL);
		}
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {
	WinX = w;
	WinY = h;
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

void drawMesh(int objId) {
	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(m_uniformId, 1, GL_FALSE, mMatrix[MODEL]);
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

void desenhaVidas() {
	GLint loc;
	int use = 5;
	float ratio = (float)WinX / WinY;
	glDisable(GL_DEPTH_TEST);
	int dir = 1;

	if (rotasaoCima > 90.0 && rotasaoCima < 270) {
		dir = -1;
	}

	glViewport(-WinX / 2.2, WinY / 2.2, WinX, WinY);
	loadIdentity(VIEW);
	lookAt(posisaoX - (10 * cos(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)), posisaoY - 10 * sin((rotasaoCima)*PI / 180), posisaoZ - (10 * sin(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)), posisaoX, posisaoY, posisaoZ, 0, dir, 0);
	loadIdentity(PROJECTION);
	perspective(53.13f, ratio, 0.1f, 1000.0f);
	glEnable(GL_BLEND);
	glUniform1i(texMode_uniformId, 13);

	// send the material
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, myMeshes[use].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, myMeshes[use].mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, myMeshes[use].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, myMeshes[use].mat.shininess);
	for (int i = 0; i < vidas; i++) {
		pushMatrix(MODEL);

		translate(MODEL, posisaoX, posisaoY, posisaoZ);
		rotate(MODEL, -rotasaoLado, 0, 1, 0);
		rotate(MODEL, rotasaoCima, 0, 0, 1);
		rotate(MODEL, -90, 0, 1, 0);
		translate(MODEL, i * 1.2, 0, 0);
		scale(MODEL, 1.2, 1, 1);

		drawMesh(use);
		popMatrix(MODEL);
	}
	glViewport(0, 0, WinX, WinY);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void renderTexto() {
	glUseProgram(shaderText.getProgramIndex());
	//Render text (bitmap fonts) in screen coordinates. So use ortoghonal projection with viewport coordinates.
	glDisable(GL_DEPTH_TEST);
	//the glyph contains background colors and non-transparent for the actual character pixels. So we use the blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	//printf("%d, %d, %d, %d\n", m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
	//viewer at origin looking down at  negative z direction
	pushMatrix(MODEL);
	loadIdentity(MODEL);
	pushMatrix(PROJECTION);
	loadIdentity(PROJECTION);
	pushMatrix(VIEW);
	loadIdentity(VIEW);
	ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
	//RenderText(shaderText, "This is a sample text", 25.0f, 25.0f, 1.0f, 0.5f, 0.8f, 0.2f);
	//RenderText(shaderText, "CGJ Light and Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
	string msg = "Lives: " + std::to_string(vidas) + "  Points : " + std::to_string(pontos);
	RenderText(shaderText, msg, 25.0, 25.0, 1.0f, 0.35f, 0.85f, 1.0f);
	if (vidas == 0) {
		RenderText(shaderText, "You died", WinX / 2.85, WinY / 2, 2.5f, 0.3, 0.7f, 0.9f);
		RenderText(shaderText, "Press [r] to resume", WinX / 2.4, WinY / 2.65, 1.0f, 0.3, 0.7f, 0.9f);
		pause = true;
	}
	else if (pause) {
		RenderText(shaderText, "Game paused", WinX / 2.85, WinY / 2, 2.5f, 0.3, 0.7f, 0.9f);
		RenderText(shaderText, "Press [p] to resume", WinX / 2.4, WinY / 2.65, 1.0f, 0.3, 0.7f, 0.9f);
	}

	glUseProgram(shader.getProgramIndex());

	popMatrix(PROJECTION);
	popMatrix(VIEW);
	popMatrix(MODEL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void carroDaBofia() {
	if (luzesLocais[12][0] < 904.0f && luzesLocais[12][2] <= 88.0f) {
		luzesLocais[12][0] += 2.0f;
	}
	else if (luzesLocais[12][0] >= 904.0f && luzesLocais[12][2] < 904.0f) {
		luzesLocais[12][2] += 2.0f;
	}
	else if (luzesLocais[12][2] >= 904.0f && luzesLocais[12][0] >= 88.0f) {
		luzesLocais[12][0] -= 2.0f;
	}
	else {
		luzesLocais[12][2] -= 2.0f;
	}
}

//Verifica se o avião está dentro das boundaries da cidade

void respawnaAviao() {
	if (posisaoX < 0.0f) {
		posisaoX = 1008.0f;
	}
	else if (posisaoX > 1008.0f) {
		posisaoX = 0.0f;
	}
	if (posisaoZ < 0.0f) {
		posisaoZ = 1008.0f;
	}
	else if (posisaoZ > 1008.0f) {
		posisaoZ = 0.0f;
	}

}

void applyRotation() {
	if (o) {
		/*if (!bateu) {*/
		rotasaoLado -= 1.5f;
		//}
		//else {
		//	rotLCam -= 1.5f;
		//}
	}

	if (o && rotPlaneH > -45.0f && !bateu) {
		rotPlaneH -= 1.5f;
	}
	if (p) {
		/*if (!bateu) {*/
		rotasaoLado += 1.5f;
		/*	}
			else {
				rotLCam += 1.5f;
			}*/
	}
	if (p && rotPlaneH < 45.0f && !bateu) {
		rotPlaneH += 1.5f;
	}
	if (q) {
		/*	if (!bateu) {*/
		rotasaoCima -= 1.5f;
		/*	}
			else {
				rotCCam -= 1.5f;
			}*/
	}
	if (q && rotPlaneV > -15.0f && !bateu) {
		rotPlaneV -= 1.0f;
	}
	if (a) {
		/*	if (!bateu) {*/
		rotasaoCima += 1.5f;
		/*	}
			else {
				rotCCam += 1.5f;
			}*/
	}
	if (a && rotPlaneV < 15.0f && !bateu) {
		rotPlaneV += 1.0f;
	}
	if (rotasaoCima > 360.0f) {
		rotasaoCima = 0.0f;
	}
	if (rotasaoCima < 0) {
		rotasaoCima = 360.0f;
	}
	if (rotasaoLado > 360.0f) {
		rotasaoLado = 0.0f;
	}
	if (rotasaoLado < 0) {
		rotasaoLado = 360.0f;
	}
}

void renderMiniMapa() {
	GLint loc;
	int use = 5;

	glUniform1i(texMode_uniformId, 11);

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

	translate(MODEL, posisaoX, posisaoY, posisaoZ);
	rotate(MODEL, -rotasaoLado, 0, 1, 0);
	rotate(MODEL, rotasaoCima, 0, 0, 1);
	rotate(MODEL, -90, 0, 1, 0);
	scale(MODEL, 10, 10, 1);

	drawMesh(use);
	glDisable(GL_STENCIL_TEST);
	popMatrix(MODEL);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NEVER, 0x1, 0x1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

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

	translate(MODEL, posisaoX, posisaoY, posisaoZ);
	rotate(MODEL, -rotasaoLado, 0, 1, 0);
	rotate(MODEL, rotasaoCima, 0, 0, 1);
	rotate(MODEL, -90, 0, 1, 0);
	scale(MODEL, 9.5, 9.5, 0);

	drawMesh(use);
	glDisable(GL_STENCIL_TEST);
	popMatrix(MODEL);
}

void renderWorld(){
	// Render Sky Box
	int objId = 4;
	glUniform1i(texMode_uniformId, 16);

	//it won't write anything to the zbuffer; all subsequently drawn scenery to be in front of the sky box. 
	glDepthMask(GL_FALSE);
	glFrontFace(GL_CW); // set clockwise vertex order to mean the front

	pushMatrix(MODEL);
	pushMatrix(VIEW);  //se quiser anular a translação

	//  Fica mais realista se não anular a translação da câmara 
	// Cancel the translation movement of the camera - de acordo com o tutorial do Antons
	mMatrix[VIEW][12] = 0.0f;
	mMatrix[VIEW][13] = 0.0f;
	mMatrix[VIEW][14] = 0.0f;

	scale(MODEL, 1000.0f, 1000.0f, 1000.0f);
	translate(MODEL, -0.5f, -0.5f, -0.5f);

	// send matrices to OGL
	glUniformMatrix4fv(m_uniformId, 1, GL_FALSE, mMatrix[MODEL]); //Transformação de modelação do cubo unitário para o "Big Cube"
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);

	glBindVertexArray(myMeshes[objId].vao);
	glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	popMatrix(MODEL);
	popMatrix(VIEW);

	glFrontFace(GL_CCW); // restore counter clockwise vertex order to mean the front
	glDepthMask(GL_TRUE);
	//end skybox
}

void renderCubeEnvironment() {
	//ENVIRONMENT START
	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform1i(texMode_uniformId, 18);

	int objId = 4;
	GLint loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, myMeshes[objId].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, myMeshes[objId].mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, myMeshes[objId].mat.shininess);
	pushMatrix(MODEL);

	translate(MODEL, 76.0f, 127.0f, 76.0f);

	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
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

	popMatrix(MODEL);
	glDisable(GL_BLEND);
	glUniform1i(texMode_uniformId, 0);

}

void renderBumpMapping() {
	//BUMP MAP CUBE
	GLint loc;
	int objId = 4;
	glUniform1i(texMode_uniformId, 17);

	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, myMeshes[objId].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, myMeshes[objId].mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, myMeshes[objId].mat.shininess);
	pushMatrix(MODEL);

	translate(MODEL, 76.0f, 107.0f, 76.0f);

	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(view_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
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

	popMatrix(MODEL);

	glUniform1i(texMode_uniformId, 0);
	// BUMP END

}

void renderArvores(float x, float y, float z, float sx, float sy, float sz, int texture) {
	//glDepthMask(GL_FALSE);
	//glEnable(GL_BLEND);
	//glDisable(GL_CULL_FACE);
	// send the material
	GLint loc;
	int objId = 5;
	glUniform1i(texMode_uniformId, 1);

	// send the material
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, myMeshes[objId].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, myMeshes[objId].mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, myMeshes[objId].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, myMeshes[objId].mat.shininess);

	float cameraPos[3] = { posisaoX, 280,  posisaoZ };

	pushMatrix(MODEL);
	translate(MODEL, x, y, z);
	scale(MODEL, sx, sy, sz);
	//float pos[3] = { 40.5, 2.0, 65.0 };
	float pos[3] = { x, y, z };
	l3dBillboardCylindricalBegin(cameraPos, pos);

	drawMesh(objId);

	popMatrix(MODEL);
	//glDisable(GL_BLEND);
	//glDepthMask(GL_TRUE);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
}

//funcoes dos fireworks
void iniParticles(int firework)
{
	GLfloat v, theta, phi;
	int i;
	int xis, ipselon, zeh;

	if (isExplosion) {
		xis = aviaoExplodido[0];
		ipselon = aviaoExplodido[1];
		zeh = aviaoExplodido[2];
		isExplosion = false;
	}
	else {
		xis = rand() % 1000;
		ipselon = rand() % 150 + 75;
		zeh = rand() % 1000;
	}

	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	for (i = 0; i < MAX_PARTICULAS; i++)
	{
		v = 10 * frand() + 0.2;
		phi = frand() * PI;
		theta = 2.0 * frand() * PI;

		particula[firework][i].x = xis;
		particula[firework][i].y = ipselon;
		particula[firework][i].z = zeh;
		particula[firework][i].vx = v * cos(theta) * sin(phi);
		particula[firework][i].vy = v * cos(phi);
		particula[firework][i].vz = v * sin(theta) * sin(phi);
		particula[firework][i].ax = 0.0f; /* simular um pouco de vento */
		particula[firework][i].ay = -0.15f; /* simular a aceleração da gravidade */
		particula[firework][i].az = 0.0f;

		/* tom amarelado que vai ser multiplicado pela textura que varia entre branco e preto */
		/*particula[i].r = 0.882f;
		particula[i].g = 0.552f;
		particula[i].b = 0.211f;*/
		//rosa
		//particula[firework][i].r = rand() % 100 /101;
		//particula[firework][i].g = rand() % 100 / 101;
		//particula[firework][i].b = rand() % 100 / 101;
		// 
		particula[firework][i].r = cores[fireworkCounter][0];
		particula[firework][i].g = cores[fireworkCounter][1];
		particula[firework][i].b = cores[fireworkCounter][2];
		// 
		//azul(alterar o mandaEfeitos para meter o G a rand e tirar o rand do B
		/*
		particula[i].r = 0.3f;
		particula[i].g = 0.3f;
		particula[i].b = 1.0f;*/

		particula[firework][i].life = 1.0f;		/* vida inicial */
		particula[firework][i].fade = 0.0050f;	    /* step de decréscimo da vida para cada iteração */
	}
}

void updateParticles(int i)
{
	int j;
	float h;

	/* Método de Euler de integração de eq. diferenciais ordinárias
	h representa o step de tempo; dv/dt = a; dx/dt = v; e conhecem-se os valores iniciais de x e v */

	//h = 0.125f;
	h = 0.033;
	if (fireworks) {

		for (j = 0; j < MAX_PARTICULAS; j++)
		{
			particula[i][j].x += (h * particula[i][j].vx);
			particula[i][j].y += (h * particula[i][j].vy);
			particula[i][j].z += (h * particula[i][j].vz);
			particula[i][j].vx += (h * particula[i][j].ax);
			particula[i][j].vy += (h * particula[i][j].ay);
			particula[i][j].vz += (h * particula[i][j].az);
			particula[i][j].life -= particula[i][j].fade;
		}
	}
}

void mandaEfeitos() {
	float particle_color[4];
	GLint loc;

	for (int j = 0; j < 8; j++) {
		if (particula[j][0].life > 0.0001) {
			printf("Entrou\n");
			updateParticles(j);

			// draw fireworks particles
			int objId = 5;  //quad for particle

			//glBindTexture(GL_TEXTURE_2D, TextureArray[11]); //particle.tga associated to TU0 

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDepthMask(GL_FALSE);  //Depth Buffer Read Only

			glUniform1i(texMode_uniformId, 14); // draw modulated textured particles 
			//glUniform1i(tex_loc, 0);

			for (int i = 0; i < MAX_PARTICULAS; i++)
			{
				if (particula[j][i].life > 0.0f) /* só desenha as que ainda estão vivas */
				{

					/* A vida da partícula representa o canal alpha da cor. Como o blend está activo a cor final é a soma da cor rgb do fragmento multiplicada pelo
					alpha com a cor do pixel destino */

					particle_color[0] = particula[j][i].r;
					particle_color[1] = particula[j][i].g;
					particle_color[2] = particula[j][i].b;
					particle_color[3] = particula[j][i].life;

					//para a cor mudar ao longo do tempo
					/*particula[i].r -= 0.001;
					particula[i].b += 0.002;*/

					//particula[j][i].r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					//particula[j][i].g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					//particula[j][i].b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

					// send the material - diffuse color modulated with texture
					loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
					glUniform4fv(loc, 1, particle_color);

					pushMatrix(MODEL);
					translate(MODEL, particula[j][i].x, particula[j][i].y, particula[j][i].z);
					if (camera == 2) {
						rotate(MODEL, -90, 1, 0, 0);
					}
					else {
						rotate(MODEL, -90 - rotasaoLado, 0, 1, 0);
					}

					// send matrices to OGL
					computeDerivedMatrix(PROJ_VIEW_MODEL);
					glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
					glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
					computeNormalMatrix3x3();
					glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

					glBindVertexArray(myMeshes[objId].vao);
					glDrawElements(myMeshes[objId].type, myMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
					popMatrix(MODEL);
				}
				else dead_num_particles++;
			}
		}

		glDepthMask(GL_TRUE); //make depth buffer again writeable
	}
	/*
		if (dead_num_particles == MAX_PARTICULAS) {
			fireworks = 0;
			dead_num_particles = 0;
			printf("All particles dead\n");
		}
		*/
		//glDisable(GL_BLEND);
}

void renderAviao(float posisaoX, float posisaoY, float posisaoZ, float rotasaoCima, float rotasaoLado, float rotPlaneV, float rotPlaneH) {
	int objId = 0;
	GLint loc;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 4; ++j) {
			int use = 0;
			if (nossoAviao) {
				glUniform1i(texMode_uniformId, 11);
			}
			else {
				glUniform1i(texMode_uniformId, 12);
			}


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
			if (!nossoAviao) {
				if (objId == 1) {
					float dif[4] = { 0.8f, 0.0f, 0.6, 1.0 };
					glUniform4fv(loc, 1, dif);
				}
				else {
					float dif[4] = { 0.78f, 0.98f, 1.0, 1.0 };
					glUniform4fv(loc, 1, dif);
				}
			}
			else {
				glUniform4fv(loc, 1, myMeshes[use].mat.diffuse);
			}

			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, myMeshes[use].mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, myMeshes[use].mat.shininess);
			pushMatrix(MODEL);
			if (objId < 8) {
				translate(MODEL, posisaoX, posisaoY, posisaoZ);
				rotate(MODEL, -rotasaoLado, 0, 1, 0);
				rotate(MODEL, rotasaoCima, 0, 0, 1);
				rotate(MODEL, rotPlaneV, 0, 0, 1);
				rotate(MODEL, rotPlaneH, 1, 0, 0);
			}

			if (objId == 0) { //parte da frente do aviao
				translate(MODEL, 1.0f, 0, 0);
				scale(MODEL, 2.0f, 1.0f, 1.0f);
			}
			else if (objId == 1) { //corpo do aviao
				glUniform1i(texMode_uniformId, 10);
				translate(MODEL, 0.125f, 0, 0);
				scale(MODEL, 1.5f, 0.5f, 0.5f);
				rotate(MODEL, 90, 0, 0, 1);
			}
			else if (objId == 2) { //parte de tras do aviao
				translate(MODEL, -0.2f, 0, 0);
				scale(MODEL, 4.0f, 1.0f, 1.0f);
			}
			else if (objId == 3) { //asa do aviao
				scale(MODEL, 1.0f, 0.2f, 5.0f);
			}
			else if (objId == 4) { //helice do aviao
				rotate(MODEL, helice, 1, 0, 0);
				translate(MODEL, 2.0f, 0.0f, 0.0f);
				rotate(MODEL, 90, 0, 1, 0);
			}
			else if (objId == 7) { //helice do aviao
				rotate(MODEL, helice, 1, 0, 0);
				translate(MODEL, 2.0f, 0.0f, 0.0f);
				rotate(MODEL, -90, 0, 1, 0);
			}
			else if (objId == 5) { //Estabilizador vertical do aviao
				translate(MODEL, -1.95f, 0.5f, 0);
				scale(MODEL, 0.5f, 1.2f, 0.2f);
			}
			else if (objId == 6) { //Estabilizador horizontal do aviao
				translate(MODEL, -1.95f, 0, 0);
				scale(MODEL, 0.5f, 0.2f, 2.0f);
			}

			drawMesh(use);

			popMatrix(MODEL);
			objId++;
		}
	}
}

void renderAviaoCone(float posisaoX, float posisaoY, float posisaoZ, float rotasaoLado) {
	GLint loc;
	int use = 2;
	glUniform1i(texMode_uniformId, 11);

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
	translate(MODEL, posisaoX, posisaoY, posisaoZ);
	rotate(MODEL, -rotasaoLado, 0, 1, 0);
	rotate(MODEL, -90, 0, 0, 1);
	translate(MODEL, 0, -4, 0);
	scale(MODEL, 1, 8, 4);

	drawMesh(use);

	popMatrix(MODEL);
}

void atualizaInimigo(int inimigo) {
	switch (inimigo) {
	case 1:
		rotasaoLadoUm = (float)(rand() % 360);
		rotasaoCimaUm = (float)(rand() % 360);
		posisaoXUm = (float)(rand() % 1000);
		if (minigame) {
			posisaoYUm = 200.0f;
			rotasaoCimaUm = 0.0f;
		}
		else {
			posisaoYUm = (float)(rand() % 150 + 150);
			rotasaoCimaUm = (float)(rand() % 360);
		}
		posisaoZUm = (float)(rand() % 1000);
		break;
	case 2:
		rotasaoLadoDois = (float)(rand() % 360);
		rotasaoCimaDois = (float)(rand() % 360);
		posisaoXDois = (float)(rand() % 1000);
		if (minigame) {
			posisaoYDois = 200.0f;
			rotasaoCimaDois = 0.0f;
		}
		else {
			posisaoYDois = (float)(rand() % 150 + 150);
			rotasaoCimaDois = (float)(rand() % 360);
		}
		posisaoZDois = (float)(rand() % 1000);
		break;
	case 3:
		rotasaoLadoTres = (float)(rand() % 360);
		posisaoXTres = (float)(rand() % 1000);
		if (minigame) {
			posisaoYTres = 200.0f;
			rotasaoCimaTres = 0.0f;
		}
		else {
			rotasaoCimaTres = (float)(rand() % 360);
			posisaoYTres = (float)(rand() % 150 + 150);
		}
		posisaoZTres = (float)(rand() % 1000);
		break;
	}
}

void atualizaAviaoInfo() {
	if (posisaoXUm < 0.0f || posisaoXUm > 1008.0f || posisaoYUm < 100.0f || posisaoYUm > 300.0f || posisaoZUm < 0.0f || posisaoZUm > 1008.0f) {
		atualizaInimigo(1);
	}
	if (posisaoXDois < 0.0f || posisaoXDois > 1008.0f || posisaoYDois < 100.0f || posisaoYDois > 300.0f || posisaoZDois < 0.0f || posisaoZDois > 1008.0f) {
		atualizaInimigo(2);
	}
	if (posisaoXTres < 0.0f || posisaoXTres > 1008.0f || posisaoYTres < 100.0f || posisaoYTres > 300.0f || posisaoZTres < 0.0f || posisaoZTres > 1008.0f) {
		atualizaInimigo(3);
	}
}

void renderTorre() {

	GLint loc;
	int objId = 11;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 6; ++j) {
			int use = 0;
			glUniform1i(texMode_uniformId, 0);
			if (objId > 11) {
				objId--;
				continue;
			}

			if (objId == 0 || objId == 7) {
				use = 2;
			}
			else if (objId == 1 || objId == 6) {
				use = 6;
			}
			else if (objId == 2 || objId == 3 || objId == 4 || objId == 5 || objId == 8 || objId == 9 || objId == 10 || objId == 11) {
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
			else if (objId == 3 || objId == 4 || objId == 5 || objId == 6 || objId == 7 || objId == 8 || objId == 9 || objId == 10 || objId == 11) {
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
			else if (objId == 11) { //bolinha
				translate(MODEL, 0.0f, 268.0f, 0.0f);
				scale(MODEL, 2.0f, 4.0f, 2.0f);
			}

			drawMesh(use);
			popMatrix(MODEL);
			objId--;

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
	}
}

void renderSign(int x, int z, int type, int ori) {
	GLint loc;

	int use = 4;

	// send the material
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, myMeshes[use].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, myMeshes[use].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	float dif[] = { 0.7f, 0.73f, 0.89f, 1.0f };
	glUniform4fv(loc, 1, dif);

	//int orientX = rand() % 2;
	pushMatrix(MODEL);
	translate(MODEL, x * 48 + 4, alturaPredios[x][z], z * 48 + 29);
	if (ori == 0) {
		rotate(MODEL, 90, 0, 1, 0);
	}
	switch (type) {
	case 1:
		//cola
		glUniform1i(texMode_uniformId, 3);
		//translate(MODEL, 98.0f, alturaPredios[2][0], 29.0f);
		scale(MODEL, 20.0f, 10.0f, 1.5f);
		break;
	case 2:
		//genshin
		glUniform1i(texMode_uniformId, 5);
		scale(MODEL, 25.0f, 7.0f, 1.5f);
		break;
	case 3:
		//banana
		glUniform1i(texMode_uniformId, 4);
		scale(MODEL, 17.0f, 10.0f, 1.0f);
		break;
	case 4:
		//lidl
		glUniform1i(texMode_uniformId, 7);
		scale(MODEL, 23.0f, 8.0f, 1.5f);
		break;
	case 5:
		//jumbo
		glUniform1i(texMode_uniformId, 6);
		scale(MODEL, 16.0f, 10.0f, 1.0f);
		break;
	case 6:
		//beer
		glUniform1i(texMode_uniformId, 8);
		scale(MODEL, 20.0f, 14.0f, 1.0f);
		break;
	case 7:
		//tea
		glUniform1i(texMode_uniformId, 9);
		scale(MODEL, 20.0f, 10.0f, 1.0f);
		break;
	}
	/*if(orientX == 0){
		rotate(MODEL, 90, 0, 1, 0);
	}*/
	drawMesh(use);
	popMatrix(MODEL);


}

void renderSigns() {
	for (int i = 0; i < N_SIGNS; i++) {
		//renderSign(rand() % Q_PREDIOS, rand() % Q_PREDIOS, rand() % 5 + 1);
		renderSign(signsX[i], signsZ[i], signTypes[i], orientasao[i]);
		//printf("%d\n", i);
	}
	/*if (i == 2 && j == 0) {
		renderSign(i, j, 1);
	}
	else if (i == 4 && j == 1) {
		renderSign(i, j, 2);
	}
	else if (i == 1 && j == 1) {
		renderSign(i, j, 3);
	}
	else if (i == 2 && j == 2) {
		renderSign(i, j, 4);
	}
	else if (i == 3 && j == 1) {
		renderSign(i, j, 5);
	}*/
}

void renderAro(int x, int z, int type, int altura) {
	GLint loc;

	int use = 7;

	// send the material
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, myMeshes[use].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, myMeshes[use].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	float dif[] = { 0.8f, 0.f, 0.75f, 1.0f };
	glUniform4fv(loc, 1, dif);

	glUniform1i(texMode_uniformId, 10);
	//int orientX = rand() % 2;
	pushMatrix(MODEL);
	translate(MODEL, x, altura, z);
	//switch (type) {
	//case 1:
	//	//horizontal
	//	break;
	//case 2:
	//	//vertical
	//	rotate(MODEL, 90, 0, 1, 0);
	//	break;
	//case 3:
	//	//cima
	//	rotate(MODEL, 90, 1, 0, 0);
	//	break;
	//}
	if (type == 2) {
		rotate(MODEL, 90, 0, 1, 0);
	}
	else if (type == 3) {
		rotate(MODEL, 90, 1, 0, 0);
	}
	else if (type == 4) {
		rotate(MODEL, 45, 0, 1, 0);
		rotate(MODEL, 90, 1, 0, 0);
	}
	else if (type == 5) {
		rotate(MODEL, 135, 0, 1, 0);
		rotate(MODEL, 90, 1, 0, 0);
	}
	else if (type == 1) {
		rotate(MODEL, 90, 1, 0, 0);
	}
	else {
		rotate(MODEL, 90, 0, 1, 0);
		rotate(MODEL, 90, 1, 0, 0);
	}
	//scale(MODEL, 2.5f, 2.5f, 2.5f);
	/*if(orientX == 0){
		rotate(MODEL, 90, 0, 1, 0);
	}*/
	drawMesh(use);
	popMatrix(MODEL);


}
void renderAros() {
	//for (int i = 0; i < N_AROS; i++) {
	//	//renderSign(rand() % Q_PREDIOS, rand() % Q_PREDIOS, rand() % 5 + 1);
	//	renderAro(arosX[i], arosZ[i], aroTypes[i], alturaAro[i]);
	//	//printf("%d\n", i);
	//}
	renderAro(0, 0, 1, 2);
	for (int i = 0; i < 12; i++) {
		renderAro(448 + i * 48, 40 + i * 48, 4, 50);
		arosInfo[0 + i][0] = 448 + i * 48;
		arosInfo[0 + i][1] = 40 + i * 48;
		arosInfo[0 + i][2] = 4;
	}

	for (int i = 0; i < 3; i++) {
		renderAro(540 - i * 48, 256 + i * 48, 5, 50);
		arosInfo[12 + i][0] = 540 - i * 48;
		arosInfo[12 + i][1] = 256 + i * 48;
		arosInfo[12 + i][2] = 5;
	}

	for (int i = 0; i < 8; i++) {
		renderAro(424, 400 + i * 48, 1, 50);
		arosInfo[15 + i][0] = 424;
		arosInfo[15 + i][1] = 400 + i * 48;
		arosInfo[15 + i][2] = 1;
	}

	for (int i = 0; i < 5; i++) {
		renderAro(400 - i * 48, 784 + i * 48, 5, 50);
		arosInfo[23 + i][0] = 400 - i * 48;
		arosInfo[23 + i][1] = 784 + i * 48;
		arosInfo[23 + i][2] = 5;
	}

	for (int i = 0; i < 9; i++) {
		renderAro(400 - i * 48, 664, 6, 50);
		arosInfo[28 + i][0] = 400 - i * 48;
		arosInfo[28 + i][1] = 664;
		arosInfo[28 + i][2] = 6;
	}
	/*if (i == 2 && j == 0) {
		renderSign(i, j, 1);
	}
	else if (i == 4 && j == 1) {
		renderSign(i, j, 2);
	}
	else if (i == 1 && j == 1) {
		renderSign(i, j, 3);
	}
	else if (i == 2 && j == 2) {
		renderSign(i, j, 4);
	}
	else if (i == 3 && j == 1) {
		renderSign(i, j, 5);
	}*/
}

void renderCity() {

	GLint loc;

	glUniform1i(texMode_uniformId, 0);

	//Do city
	for (int i = 0; i < Q_PREDIOS; i++) {
		for (int j = 0; j < Q_PREDIOS; j++) {

			if (alturaPredios[i][j] != 0) {
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

				//POSSIVEL TETO DO PREDIO (IMPORTANTE)
				// send the material
				/*
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
				glUniform4fv(loc, 1, myMeshes[5].mat.ambient);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
				glUniform4fv(loc, 1, myMeshes[5].mat.diffuse);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
				glUniform4fv(loc, 1, myMeshes[5].mat.specular);
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
				glUniform1f(loc, myMeshes[5].mat.shininess);
				pushMatrix(MODEL);

				translate(MODEL, i * 48+16, alturaPredios[i][j] + 0.4, j * 48+16);
				rotate(MODEL, -90, 1, 0, 0);
				scale(MODEL, 32.0, 32.0f, 1.0);

				drawMesh(5);

				popMatrix(MODEL);*/
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
			if (objId == 2 || objId == 3) {
				float shiny = 20.0f;
				glUniform1f(loc, shiny);
			}
			else {
				glUniform1f(loc, myMeshes[use].mat.shininess);
			}
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
				glUniform1i(texMode_uniformId, 2);
				translate(MODEL, 96.0f, 0.0f, 576.0f);
				scale(MODEL, 80.0f, 1.1f, 80.0f);
			}
			else if (objId == 3) { //parque cima
				glUniform1i(texMode_uniformId, 2);
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
				translate(MODEL, 441.0f, 0.0f, 776.0f);
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
	}
}

void updateMisseis() {
	GLint loc;
	for (int i = 0; i < N_MISSEIS_MAX; i++) {
		if (infoMisseis[i][3] != 0.0f) {
			// send the material
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, myMeshes[3].mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			float dif[4] = { 0.9f, 0.0f, 0.7f, 1.0f };
			glUniform4fv(loc, 1, dif);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, myMeshes[3].mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, myMeshes[3].mat.shininess);
			pushMatrix(MODEL);

			glUniform1i(texMode_uniformId, 10);
			translate(MODEL, infoMisseis[i][2], infoMisseis[i][3], infoMisseis[i][4]);
			rotate(MODEL, -infoMisseis[i][0], 0, 1, 0);
			rotate(MODEL, infoMisseis[i][1], 0, 0, 1);
			translate(MODEL, 0.0f, -0.75f, 0.0f);
			scale(MODEL, 2.0f, 0.5f, 0.5f);
			rotate(MODEL, 90, 0, 0, 1);

			drawMesh(3);

			popMatrix(MODEL);

			//printf("rotL: %f  rotC: %f", infoMisseis[0][0], infoMisseis[0][1]);
			if (!pause) {
				infoMisseis[i][2] += 1.0f * cos(infoMisseis[i][0] * PI / 180) * cos(infoMisseis[i][1] * PI / 180) * deltaTime * 100;
				infoMisseis[i][3] += 1.0f * sin(infoMisseis[i][1] * PI / 180) * deltaTime * 100;
				infoMisseis[i][4] += 1.0f * sin(infoMisseis[i][0] * PI / 180) * cos(infoMisseis[i][1] * PI / 180) * deltaTime * 100;
			}
		}
	}
}

void updateMisseisInimigos() {
	GLint loc;
	glUniform1i(texMode_uniformId, 12);
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < N_MISSEIS_MAX; i++) {
			//printf("Missil %d: %f %f %f %f %f\n", i, infoMisseisUm[i][0], infoMisseisUm[i][1], infoMisseisUm[i][2], infoMisseisUm[i][3], infoMisseisUm[i][4]);
			// send the material
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, myMeshes[3].mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			float dif[4] = { 0.0f, 0.81f, 0.9f, 1.0f };
			glUniform4fv(loc, 1, dif);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, myMeshes[3].mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, myMeshes[3].mat.shininess);
			pushMatrix(MODEL);
			if (j == 0) {
				if (infoMisseisUm[i][3] != 0.0f) {

					translate(MODEL, infoMisseisUm[i][2], infoMisseisUm[i][3], infoMisseisUm[i][4]);
					rotate(MODEL, -infoMisseisUm[i][0], 0, 1, 0);
					rotate(MODEL, infoMisseisUm[i][1], 0, 0, 1);
					translate(MODEL, 0.0f, -0.75f, 0.0f);
					scale(MODEL, 2.0f, 0.5f, 0.5f);
					rotate(MODEL, 90, 0, 0, 1);

					drawMesh(3);

					popMatrix(MODEL);

					//printf("rotL: %f  rotC: %f", infoMisseisUm[0][0], infoMisseisUm[0][1]);

					if (!pause) {
						infoMisseisUm[i][2] += 1.0f * cos(infoMisseisUm[i][0] * PI / 180) * cos(infoMisseisUm[i][1] * PI / 180);
						infoMisseisUm[i][3] += 1.0f * sin(infoMisseisUm[i][1] * PI / 180);
						infoMisseisUm[i][4] += 1.0f * sin(infoMisseisUm[i][0] * PI / 180) * cos(infoMisseisUm[i][1] * PI / 180);
					}
				}
				if (infoMisseisUm[i][3] < 0.0f) {
					infoMisseisUm[i][0] = 0.0f;
					infoMisseisUm[i][1] = 0.0f;
					infoMisseisUm[i][2] = 0.0f;
					infoMisseisUm[i][3] = 0.0f;
					infoMisseisUm[i][4] = 0.0f;
				}
			}
			if (j == 1) {
				if (infoMisseisDois[i][3] != 0.0f) {

					translate(MODEL, infoMisseisDois[i][2], infoMisseisDois[i][3], infoMisseisDois[i][4]);
					rotate(MODEL, -infoMisseisDois[i][0], 0, 1, 0);
					rotate(MODEL, infoMisseisDois[i][1], 0, 0, 1);
					scale(MODEL, 2.0f, 0.5f, 0.5f);
					rotate(MODEL, 90, 0, 0, 1);

					drawMesh(3);

					popMatrix(MODEL);

					//printf("rotL: %f  rotC: %f", infoMisseisDois[0][0], infoMisseisDois[0][1]);
					if (!pause) {
						infoMisseisDois[i][2] += 1.0f * cos(infoMisseisDois[i][0] * PI / 180) * cos(infoMisseisDois[i][1] * PI / 180);
						infoMisseisDois[i][3] += 1.0f * sin(infoMisseisDois[i][1] * PI / 180);
						infoMisseisDois[i][4] += 1.0f * sin(infoMisseisDois[i][0] * PI / 180) * cos(infoMisseisDois[i][1] * PI / 180);
					}
				}
				if (infoMisseisDois[i][3] < 0.0f) {
					infoMisseisDois[i][0] = 0.0f;
					infoMisseisDois[i][1] = 0.0f;
					infoMisseisDois[i][2] = 0.0f;
					infoMisseisDois[i][3] = 0.0f;
					infoMisseisDois[i][4] = 0.0f;
				}
			}
			if (j == 2) {
				if (infoMisseisTres[i][3] != 0.0f) {

					translate(MODEL, infoMisseisTres[i][2], infoMisseisTres[i][3], infoMisseisTres[i][4]);
					rotate(MODEL, -infoMisseisTres[i][0], 0, 1, 0);
					rotate(MODEL, infoMisseisTres[i][1], 0, 0, 1);
					scale(MODEL, 2.0f, 0.5f, 0.5f);
					rotate(MODEL, 90, 0, 0, 1);

					drawMesh(3);

					popMatrix(MODEL);

					//printf("rotL: %f  rotC: %f", infoMisseisTres[0][0], infoMisseisTres[0][1]);
					if (!pause) {
						infoMisseisTres[i][2] += 1.0f * cos(infoMisseisTres[i][0] * PI / 180) * cos(infoMisseisTres[i][1] * PI / 180);
						infoMisseisTres[i][3] += 1.0f * sin(infoMisseisTres[i][1] * PI / 180);
						infoMisseisTres[i][4] += 1.0f * sin(infoMisseisTres[i][0] * PI / 180) * cos(infoMisseisTres[i][1] * PI / 180);
					}
				}
				if (infoMisseisTres[i][3] < 0.0f) {
					infoMisseisTres[i][0] = 0.0f;
					infoMisseisTres[i][1] = 0.0f;
					infoMisseisTres[i][2] = 0.0f;
					infoMisseisTres[i][3] = 0.0f;
					infoMisseisTres[i][4] = 0.0f;
				}
			}
		}
	}
}

// ------------------------------------------------------------
//
// Collisions
//

float max(float* values, int numVals) {
	float maxVal = -10000.0f;
	for (int i = 0; i < numVals; i++) {
		if (values[i] > maxVal) {
			maxVal = values[i];
		}
	}
	return maxVal;
}

float min(float* values, int numVals) {
	float minVal = 10000.0f;
	for (int i = 0; i < numVals; i++) {
		if (values[i] < minVal) {
			minVal = values[i];
		}
	}
	return minVal;
}

float maximo(float um, float dois) {
	if (um > dois)
		return um;
	else return dois;
}

float minimo(float um, float dois) {
	if (um < dois)
		return um;
	else return dois;
}


float clamp(float value, float min, float max) {
	return maximo(min, minimo(max, value));
}

void updateAABB(AABB* aabb, float rotasaoLado, float rotasaoCima, float posX, float posY, float posZ) {

	//atualiza o quad das asas do aviao
	glm::vec4 newvertEsq = aabb->luzEsq;
	glm::vec4 newvertDir = aabb->luzDir;
	glm::mat4 transLuz;

	//printf("%f\n", rotasaoLado*180/PI);

	/*
	transLuz = glm::mat4(1.0f);
	transLuz = glm::rotate(transLuz, rotPlaneH, glm::vec3(1.0,0.0,0.0));
	newvertEsq = transLuz * newvertEsq;
	newvertDir = transLuz * newvertDir;*/

	transLuz = glm::mat4(1.0f);
	transLuz = glm::rotate(transLuz, rotasaoCima, glm::vec3(rotasaoLado / 360.0, 0.0, (360.0 - rotasaoLado) / 360.0));
	newvertEsq = transLuz * newvertEsq;
	newvertDir = transLuz * newvertDir;

	transLuz = glm::mat4(1.0f);
	transLuz = glm::rotate(transLuz, -rotasaoLado, glm::vec3(0.0, 1.0, 0.0));
	newvertEsq = transLuz * newvertEsq;
	newvertDir = transLuz * newvertDir;

	transLuz = glm::mat4(1.0f);
	transLuz = glm::translate(transLuz, glm::vec3(posX, posY, posZ));
	newvertEsq = transLuz * newvertEsq;
	newvertDir = transLuz * newvertDir;

	aabb->luzEsquerda = newvertEsq;
	aabb->luzDireita = newvertDir;

	//atualiza o quad do corpo do aviao
	glm::vec4 newvert1 = aabb->vert1;
	glm::vec4 newvert2 = aabb->vert2;
	glm::vec4 newvert3 = aabb->vert3;
	glm::vec4 newvert4 = aabb->vert4;
	glm::mat4 trans;

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

	//printf("tlX:%f tlY:%f brX:%f brY:%f\n", aabb->tl[0], aabb->tl[2], aabb->br[0], aabb->br[2]);

	//atualiza o quad das asas do aviao

	glm::vec4 newvert1A = aabb->vert1A;
	glm::vec4 newvert2A = aabb->vert2A;
	glm::vec4 newvert3A = aabb->vert3A;
	glm::vec4 newvert4A = aabb->vert4A;
	glm::mat4 transA;

	transA = glm::mat4(1.0f);
	transA = glm::rotate(transA, rotPlaneH, glm::vec3(1.0f, 0.0f, 0.0f));
	newvert1A = transA * newvert1A;
	newvert2A = transA * newvert2A;
	newvert3A = transA * newvert3A;
	newvert4A = transA * newvert4A;

	transA = glm::mat4(1.0f);
	transA = glm::rotate(transA, rotasaoCima, glm::vec3(rotasaoLado / 360.0, 0.0, (360.0 - rotasaoLado) / 360.0));
	newvert1A = transA * newvert1A;
	newvert2A = transA * newvert2A;
	newvert3A = transA * newvert3A;
	newvert4A = transA * newvert4A;

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

	//Atualiza a colliding box do corpo do avião

	glm::vec4 newvert1C = aabb->vert1C;
	glm::vec4 newvert2C = aabb->vert2C;
	glm::vec4 newvert3C = aabb->vert3C;
	glm::vec4 newvert4C = aabb->vert4C;
	glm::vec4 newvert5C = aabb->vert5C;
	glm::vec4 newvert6C = aabb->vert6C;
	glm::vec4 newvert7C = aabb->vert7C;
	glm::vec4 newvert8C = aabb->vert8C;
	glm::mat4 transC;

	transC = glm::mat4(1.0f);
	transC = glm::rotate(transC, rotasaoCima, glm::vec3(rotasaoLado / 360.0, 0.0, (360.0 - rotasaoLado) / 360.0));
	newvert1C = transC * newvert1C;
	newvert2C = transC * newvert2C;
	newvert3C = transC * newvert3C;
	newvert4C = transC * newvert4C;
	newvert5C = transC * newvert5C;
	newvert6C = transC * newvert6C;
	newvert7C = transC * newvert7C;
	newvert8C = transC * newvert8C;

	transC = glm::mat4(1.0f);
	transC = glm::rotate(transC, -rotasaoLado, glm::vec3(0.0, 1.0, 0.0));
	newvert1C = transC * newvert1C;
	newvert2C = transC * newvert2C;
	newvert3C = transC * newvert3C;
	newvert4C = transC * newvert4C;
	newvert5C = transC * newvert5C;
	newvert6C = transC * newvert6C;
	newvert7C = transC * newvert7C;
	newvert8C = transC * newvert8C;

	transC = glm::mat4(1.0f);
	transC = glm::translate(transC, glm::vec3(posX, posY, posZ));
	newvert1C = transC * newvert1C;
	newvert2C = transC * newvert2C;
	newvert3C = transC * newvert3C;
	newvert4C = transC * newvert4C;
	newvert5C = transC * newvert5C;
	newvert6C = transC * newvert6C;
	newvert7C = transC * newvert7C;
	newvert8C = transC * newvert8C;

	aabb->v1C = newvert1C;
	aabb->v2C = newvert2C;
	aabb->v3C = newvert3C;
	aabb->v4C = newvert4C;
	aabb->v5C = newvert5C;
	aabb->v6C = newvert6C;
	aabb->v7C = newvert7C;
	aabb->v8C = newvert8C;

	float vertsX[8] = { newvert1C[0], newvert2C[0], newvert3C[0], newvert4C[0], newvert5C[0], newvert6C[0], newvert7C[0], newvert8C[0] };
	float vertsY[8] = { newvert1C[1], newvert2C[1], newvert3C[1], newvert4C[1], newvert5C[1], newvert6C[1], newvert7C[1], newvert8C[1] };
	float vertsZ[8] = { newvert1C[2], newvert2C[2], newvert3C[2], newvert4C[2], newvert5C[2], newvert6C[2], newvert7C[2], newvert8C[2] };

	float maxX = max(vertsX, 8);
	float minX = min(vertsX, 8);
	float maxY = max(vertsY, 8);
	float minY = min(vertsY, 8);
	float maxZ = max(vertsZ, 8);
	float minZ = min(vertsZ, 8);

	aabb->tlC = glm::vec4(maxX, maxY, maxZ, 1.0f);
	aabb->brC = glm::vec4(minX, minY, minZ, 1.0f);

	//atualiza a colliding box das asas do aviao

	glm::vec4 newvert1CA = aabb->vert1CA;
	glm::vec4 newvert2CA = aabb->vert2CA;
	glm::vec4 newvert3CA = aabb->vert3CA;
	glm::vec4 newvert4CA = aabb->vert4CA;
	glm::vec4 newvert5CA = aabb->vert5CA;
	glm::vec4 newvert6CA = aabb->vert6CA;
	glm::vec4 newvert7CA = aabb->vert7CA;
	glm::vec4 newvert8CA = aabb->vert8CA;
	glm::mat4 transCA;

	transCA = glm::mat4(1.0f);
	transCA = glm::rotate(transCA, rotasaoCima, glm::vec3(rotasaoLado / 360.0, 0.0, (360.0 - rotasaoLado) / 360.0));
	newvert1CA = transCA * newvert1CA;
	newvert2CA = transCA * newvert2CA;
	newvert3CA = transCA * newvert3CA;
	newvert4CA = transCA * newvert4CA;
	newvert5CA = transCA * newvert5CA;
	newvert6CA = transCA * newvert6CA;
	newvert7CA = transCA * newvert7CA;
	newvert8CA = transCA * newvert8CA;

	transCA = glm::mat4(1.0f);
	transCA = glm::rotate(transCA, -rotasaoLado, glm::vec3(0.0, 1.0, 0.0));
	newvert1CA = transCA * newvert1CA;
	newvert2CA = transCA * newvert2CA;
	newvert3CA = transCA * newvert3CA;
	newvert4CA = transCA * newvert4CA;
	newvert5CA = transCA * newvert5CA;
	newvert6CA = transCA * newvert6CA;
	newvert7CA = transCA * newvert7CA;
	newvert8CA = transCA * newvert8CA;

	transCA = glm::mat4(1.0f);
	transCA = glm::translate(transCA, glm::vec3(posX, posY, posZ));
	newvert1CA = transCA * newvert1CA;
	newvert2CA = transCA * newvert2CA;
	newvert3CA = transCA * newvert3CA;
	newvert4CA = transCA * newvert4CA;
	newvert5CA = transCA * newvert5CA;
	newvert6CA = transCA * newvert6CA;
	newvert7CA = transCA * newvert7CA;
	newvert8CA = transCA * newvert8CA;

	aabb->v1CA = newvert1CA;
	aabb->v2CA = newvert2CA;
	aabb->v3CA = newvert3CA;
	aabb->v4CA = newvert4CA;
	aabb->v5CA = newvert5CA;
	aabb->v6CA = newvert6CA;
	aabb->v7CA = newvert7CA;
	aabb->v8CA = newvert8CA;

	float vertsAX[8] = { newvert1CA[0], newvert2CA[0], newvert3CA[0], newvert4CA[0], newvert5CA[0], newvert6CA[0], newvert7CA[0], newvert8CA[0] };
	float vertsAY[8] = { newvert1CA[1], newvert2CA[1], newvert3CA[1], newvert4CA[1], newvert5CA[1], newvert6CA[1], newvert7CA[1], newvert8CA[1] };
	float vertsAZ[8] = { newvert1CA[2], newvert2CA[2], newvert3CA[2], newvert4CA[2], newvert5CA[2], newvert6CA[2], newvert7CA[2], newvert8CA[2] };

	float maxAX = max(vertsAX, 8);
	float minAX = min(vertsAX, 8);
	float maxAY = max(vertsAY, 8);
	float minAY = min(vertsAY, 8);
	float maxAZ = max(vertsAZ, 8);
	float minAZ = min(vertsAZ, 8);

	aabb->tlCA = glm::vec4(maxAX, maxAY, maxAZ, 1.0f);
	aabb->brCA = glm::vec4(minAX, minAY, minAZ, 1.0f);
}

bool checkaColisaoComMisseisInimigos(AABB* aabb) {
	/*
	printf("MaxX: %f\n", aabb->tlC[0]);
	printf("MinX: %f\n", aabb->brC[0]);
	printf("MaxY: %f\n", aabb->tlC[1]);
	printf("MaxY: %f\n", aabb->brC[1]);
	printf("MaxZ: %f\n", aabb->tlC[2]);
	printf("MaxZ: %f\n", aabb->brC[2]);*/
	bool colideComMissil = false;
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < N_MISSEIS_MAX; i++) {
			if (j == 0) {
				if (infoMisseisUm[i][3] != 0.0f) {
					colideComMissil = colideComMissil || (infoMisseisUm[i][2] >= aabb->brC[0] && infoMisseisUm[i][2] <= aabb->tlC[0] &&
						infoMisseisUm[i][3] >= aabb->brC[1] && infoMisseisUm[i][3] <= aabb->tlC[1] &&
						infoMisseisUm[i][4] >= aabb->brC[2] && infoMisseisUm[i][4] <= aabb->tlC[2]) ||
						(infoMisseisUm[i][2] >= aabb->brCA[0] && infoMisseisUm[i][2] <= aabb->tlCA[0] &&
							infoMisseisUm[i][3] >= aabb->brCA[1] && infoMisseisUm[i][3] <= aabb->tlCA[1] &&
							infoMisseisUm[i][4] >= aabb->brCA[2] && infoMisseisUm[i][4] <= aabb->tlCA[2]);
				}
			}
			else if (j == 1) {
				if (infoMisseisDois[i][3] != 0.0f) {
					colideComMissil = colideComMissil || (infoMisseisDois[i][2] >= aabb->brC[0] && infoMisseisDois[i][2] <= aabb->tlC[0] &&
						infoMisseisDois[i][3] >= aabb->brC[1] && infoMisseisDois[i][3] <= aabb->tlC[1] &&
						infoMisseisDois[i][4] >= aabb->brC[2] && infoMisseisDois[i][4] <= aabb->tlC[2]) ||
						(infoMisseisDois[i][2] >= aabb->brCA[0] && infoMisseisDois[i][2] <= aabb->tlCA[0] &&
							infoMisseisDois[i][3] >= aabb->brCA[1] && infoMisseisDois[i][3] <= aabb->tlCA[1] &&
							infoMisseisDois[i][4] >= aabb->brCA[2] && infoMisseisDois[i][4] <= aabb->tlCA[2]);
				}
			}
			else if (j == 2) {
				if (infoMisseisTres[i][3] != 0.0f) {
					colideComMissil = colideComMissil || (infoMisseisTres[i][2] >= aabb->brC[0] && infoMisseisTres[i][2] <= aabb->tlC[0] &&
						infoMisseisTres[i][3] >= aabb->brC[1] && infoMisseisTres[i][3] <= aabb->tlC[1] &&
						infoMisseisTres[i][4] >= aabb->brC[2] && infoMisseisTres[i][4] <= aabb->tlC[2]) ||
						(infoMisseisTres[i][2] >= aabb->brCA[0] && infoMisseisTres[i][2] <= aabb->tlCA[0] &&
							infoMisseisTres[i][3] >= aabb->brCA[1] && infoMisseisTres[i][3] <= aabb->tlCA[1] &&
							infoMisseisTres[i][4] >= aabb->brCA[2] && infoMisseisTres[i][4] <= aabb->tlCA[2]);
				}
			}
		}
		if (colideComMissil) {
			return colideComMissil;
		}
	}
	return colideComMissil;
}

bool checkaColisaoComMisseis(AABB* aabb) {
	bool colideComMissil = false;
	for (int i = 0; i < N_MISSEIS_MAX; i++) {
		if (infoMisseis[i][3] != 0.0f) {
			colideComMissil = colideComMissil || (infoMisseis[i][2] >= aabb->brC[0] && infoMisseis[i][2] <= aabb->tlC[0] &&
				infoMisseis[i][3] >= aabb->brC[1] && infoMisseis[i][3] <= aabb->tlC[1] &&
				infoMisseis[i][4] >= aabb->brC[2] && infoMisseis[i][4] <= aabb->tlC[2]) ||
				(infoMisseis[i][2] >= aabb->brCA[0] && infoMisseis[i][2] <= aabb->tlCA[0] &&
					infoMisseis[i][3] >= aabb->brCA[1] && infoMisseis[i][3] <= aabb->tlCA[1] &&
					infoMisseis[i][4] >= aabb->brCA[2] && infoMisseis[i][4] <= aabb->tlCA[2]);
		}
	}
	if (colideComMissil) {
		return colideComMissil;
	}
	return colideComMissil;
}

// Car-butter/cheerio collision
bool checkCollisionBox(float a_xmin, float a_xmax, float a_zmin, float a_zmax, float b_xmin, float b_xmax, float b_zmin, float b_zmax) {
	return (a_xmin <= b_xmax && a_xmax >= b_xmin) &&
		(a_zmin <= b_zmax && a_zmax >= b_zmin);
}

bool checkaColisaoPredios(AABB* aabb) {
	glm::vec4 v1 = aabb->v3;
	glm::vec4 v2 = aabb->v4;
	glm::vec4 v1A = aabb->v1A;
	glm::vec4 v2A = aabb->v2A;
	float CorpoY = posisaoY;

	int predioX = 0;
	int predioZ = 0;

	predioX = (int)floor(v1[0] / 48.0f);
	predioZ = (int)floor(v1[2] / 48.0f);

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

bool checkaColisaoComAros() {
	for (int i = 0; i < 37; i++) {
		int tipoAro = arosInfo[i][2];
		switch (tipoAro) {
		case 1:
			if (posisaoX < arosInfo[i][0] + 8 && posisaoX > arosInfo[i][0] - 8 && posisaoY < 58 && posisaoY > 42) {
				return true;
			}
			break;
		case 4:
			if (posisaoZ < arosInfo[i][1] + 8 && posisaoZ > arosInfo[i][1] - 8 && posisaoY < 58 && posisaoY > 42) {
				return true;
			}
			break;
		case 5:
			if (posisaoZ < arosInfo[i][1] + 8 && posisaoZ > arosInfo[i][1] - 8 && posisaoY < 58 && posisaoY > 42) {
				return true;
			}
			break;
		case 6:
			if (posisaoZ < arosInfo[i][1] + 8 && posisaoZ > arosInfo[i][1] - 8 && posisaoY < 58 && posisaoY > 42) {
				return true;
			}
			break;
		}
	}
	return false;
}

void handleCollisions() {
	updateAABB(&aabb, (rotasaoLado * PI / 180), (rotasaoCima * PI / 180), posisaoX, posisaoY, posisaoZ);
	updateAABB(&aabbUm, (rotasaoLadoUm * PI / 180), (rotasaoCimaUm * PI / 180), posisaoXUm, posisaoYUm, posisaoZUm);
	updateAABB(&aabbDois, (rotasaoLadoDois * PI / 180), (rotasaoCimaDois * PI / 180), posisaoXDois, posisaoYDois, posisaoZDois);
	updateAABB(&aabbTres, (rotasaoLadoTres * PI / 180), (rotasaoCimaTres * PI / 180), posisaoXTres, posisaoYTres, posisaoZTres);
	bateu = checkaColisaoPredios(&aabb);

	if (checkaColisaoComMisseisInimigos(&aabb)) {
		printf("morreu\n");
		if (imunidade == 0 && vidas > 0) {
			vidas -= 1;
			if (vidas > 0)
				imunidade = 120;
		}
	}
	/* CODIGO PARA SE QUISERMOS QUE O AVIAO PERCA VIDAS AO COLIDIR COM PREDIOS
	else if (bateu && vidas > 0) {
		if (imunidade == 0) {
			vidas -= 1;
			if (vidas > 0) {
				imunidade = 120;
				posisaoX = 0.0f;
				posisaoY = 125.0f;
				posisaoZ = 0.0f;
				rotasaoCima = 0.0f;
				rotasaoLado = 0.0f;
			}
		}
		else {
			posisaoX = 0.0f;
			posisaoY = 125.0f;
			posisaoZ = 0.0f;
			rotasaoCima = 0.0f;
			rotasaoLado = 0.0f;
		}
	}*/

	if (checkaColisaoComMisseis(&aabbUm)) {
		printf("Aviao 1 morto\n");
		pontos += 44;
		fireworkCounter = (fireworkCounter + 1) % 8;
		aviaoExplodido[0] = posisaoXUm;
		aviaoExplodido[1] = posisaoYUm;
		aviaoExplodido[2] = posisaoZUm;
		isExplosion = true;
		iniParticles(fireworkCounter);
		atualizaInimigo(1);
	}
	if (checkaColisaoComMisseis(&aabbDois)) {
		printf("Aviao 2 morto\n");
		pontos += 44;
		fireworkCounter = (fireworkCounter + 1) % 8;
		aviaoExplodido[0] = posisaoXDois;
		aviaoExplodido[1] = posisaoYDois;
		aviaoExplodido[2] = posisaoZDois;
		isExplosion = true;
		iniParticles(fireworkCounter);
		atualizaInimigo(2);
	}
	if (checkaColisaoComMisseis(&aabbTres)) {
		printf("Aviao 3 morto\n");
		pontos += 44;
		fireworkCounter = (fireworkCounter + 1) % 8;
		aviaoExplodido[0] = posisaoXTres;
		aviaoExplodido[1] = posisaoYTres;
		aviaoExplodido[2] = posisaoZTres;
		isExplosion = true;
		iniParticles(fireworkCounter);
		atualizaInimigo(3);
	}

	if (checkaColisaoComAros()) {
		acelerasao += 0.02f * deltaTime * 100;
	}

}

void rodaLuzDeCima() {
	luzesLocais[6][0] = 496 + 2.8 * cos(rotasaoLampada * PI / 180);
	luzesLocais[6][2] = 496 + 2.8 * sin(rotasaoLampada * PI / 180);
	luzesLocais[7][0] = 496 + 2.8 * cos((rotasaoLampada + 180) * PI / 180);
	luzesLocais[7][2] = 496 + 2.8 * sin((rotasaoLampada + 180) * PI / 180);
	rotasaoLampada = (rotasaoLampada + 15) % 360;
}

void fazGradienteLuz() {
	if (mudaLuz == -1) {
		if (corLuz[luzIndice] > 0.0) {
			corLuz[luzIndice] -= 0.05;
		}
		else {
			mudaLuz = -mudaLuz;
			luzIndice = (luzIndice + 1) % 3;
		}
	}
	else {
		if (corLuz[luzIndice] < 1.0) {
			corLuz[luzIndice] += 0.05;
		}
		else {
			mudaLuz = -mudaLuz;
			luzIndice = (luzIndice + 1) % 3;
		}
	}
	//printf("rgb: %f %f %f \n", corLuz[0], corLuz[1], corLuz[2]);
	//printf("luz indice: %d\n", luzIndice);
}

void heatSeek()
{
	//HEAT SEEK ZONA EXPERIMENTAL
	float novaRLUm = atan((posisaoZ - posisaoZUm) / (posisaoX - posisaoXUm)) * 180.0f / PI;
	if (posisaoX < posisaoXUm && posisaoZ < posisaoZUm) {
		novaRLUm = -90.0f - (90.0f - novaRLUm);
	}
	else if (posisaoX < posisaoXUm && posisaoZ > posisaoZUm) {
		novaRLUm = -180.0f + novaRLUm;
	}

	if (abs(rotasaoLadoUm - novaRLUm) > 1.0f) {
		if (rotasaoLadoUm < novaRLUm) {
			rotasaoLadoUm += 1.0f;
		}
		else {
			rotasaoLadoUm -= 1.0f;
		}
	}

	float novaRCUm = atan(sqrt(pow(posisaoX - posisaoXUm, 2.0) + pow(posisaoZ - posisaoZUm, 2.0)) / (posisaoY - posisaoYUm)) * 180.0f / PI;

	if (posisaoY > posisaoYUm) {
		novaRCUm = 90.0f - novaRCUm;
	}
	else {
		novaRCUm = -(90.0f + novaRCUm);
	}

	if (abs(rotasaoCimaUm - novaRCUm) > 1.5f) {
		if (rotasaoCimaUm < novaRCUm) {
			rotasaoCimaUm += 1.5f;
		}
		else {
			rotasaoCimaUm -= 1.5f;
		}
	}
	//=============================
	//HEAT SEEK ZONA EXPERIMENTAL
	float novaRLDois = atan((posisaoZ - posisaoZDois) / (posisaoX - posisaoXDois)) * 180.0f / PI;
	if (posisaoX < posisaoXDois && posisaoZ < posisaoZDois) {
		novaRLDois = -90.0f - (90.0f - novaRLDois);
	}
	else if (posisaoX < posisaoXDois && posisaoZ > posisaoZDois) {
		novaRLDois = -180.0f + novaRLDois;
	}

	if (abs(rotasaoLadoDois - novaRLDois) > 1.0f) {
		if (rotasaoLadoDois < novaRLDois) {
			rotasaoLadoDois += 1.0f;
		}
		else {
			rotasaoLadoDois -= 1.0f;
		}
	}

	float novaRCDois = atan(sqrt(pow(posisaoX - posisaoXDois, 2.0) + pow(posisaoZ - posisaoZDois, 2.0)) / (posisaoY - posisaoYDois)) * 180.0f / PI;

	if (posisaoY > posisaoYDois) {
		novaRCDois = 90.0f - novaRCDois;
	}
	else {
		novaRCDois = -(90.0f + novaRCDois);
	}

	if (abs(rotasaoCimaDois - novaRCDois) > 1.5f) {
		if (rotasaoCimaDois < novaRCDois) {
			rotasaoCimaDois += 1.5f;
		}
		else {
			rotasaoCimaDois -= 1.5f;
		}
	}
	//=============================
}

void aiRecursive_render(const aiScene* sc, const aiNode* nd)
{
	GLint loc;

	// Get node transformation matrix
	aiMatrix4x4 m = nd->mTransformation;
	// OpenGL matrices are column major
	m.Transpose();

	// save model matrix and apply node transformation
	pushMatrix(MODEL);

	float aux[16];
	memcpy(aux, &m, sizeof(float) * 16);
	multMatrix(MODEL, aux);


	// draw all meshes assigned to this node
	for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {

		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, myMeshes2[nd->mMeshes[n]].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, myMeshes2[nd->mMeshes[n]].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, myMeshes2[nd->mMeshes[n]].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.emissive");
		glUniform4fv(loc, 1, myMeshes2[nd->mMeshes[n]].mat.emissive);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, myMeshes2[nd->mMeshes[n]].mat.shininess);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
		glUniform1i(loc, myMeshes2[nd->mMeshes[n]].mat.texCount);

		unsigned int  diffMapCount = 0;  //read 2 diffuse textures

		//devido ao fragment shader suporta 2 texturas difusas simultaneas, 1 especular e 1 normal map

		//glUniform1i(normalMap_loc, false);   //GLSL normalMap variable initialized to 0
		//glUniform1i(specularMap_loc, false);
		//glUniform1ui(diffMapCount_loc, 0);

		if (myMeshes2[nd->mMeshes[n]].mat.texCount != 0)
			for (unsigned int i = 0; i < myMeshes2[nd->mMeshes[n]].mat.texCount; ++i) {
				if (myMeshes2[nd->mMeshes[n]].texTypes[i] == DIFFUSE) {
					if (diffMapCount == 0) {
						diffMapCount++;
						loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitDiff");
						glUniform1i(loc, myMeshes2[nd->mMeshes[n]].texUnits[i]);
						//glUniform1ui(diffMapCount_loc, diffMapCount);
					}
					else if (diffMapCount == 1) {
						diffMapCount++;
						loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitDiff1");
						glUniform1i(loc, myMeshes2[nd->mMeshes[n]].texUnits[i]);
						//glUniform1ui(diffMapCount_loc, diffMapCount);
					}
					else printf("Only supports a Material with a maximum of 2 diffuse textures\n");
				}
				else if (myMeshes2[nd->mMeshes[n]].texTypes[i] == SPECULAR) {
					loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitSpec");
					glUniform1i(loc, myMeshes2[nd->mMeshes[n]].texUnits[i]);
					//glUniform1i(specularMap_loc, true);
				}
				else if (myMeshes2[nd->mMeshes[n]].texTypes[i] == NORMALS) { //Normal map
					loc = glGetUniformLocation(shader.getProgramIndex(), "texUnitNormalMap");
					//if (normalMapKey)
					//	glUniform1i(normalMap_loc, normalMapKey);
					glUniform1i(loc, myMeshes2[nd->mMeshes[n]].texUnits[i]);

				}
				else printf("Texture Map not supported\n");
			}

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// bind VAO
		glBindVertexArray(myMeshes2[nd->mMeshes[n]].vao);

		if (!shader.isProgramValid()) {
			printf("Program Not Valid!\n");
			exit(1);
		}
		// draw
		glDrawElements(myMeshes2[nd->mMeshes[n]].type, myMeshes2[nd->mMeshes[n]].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	// draw all children
	for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
		aiRecursive_render(sc, nd->mChildren[n]);
	}
	popMatrix(MODEL);
}

void renderObjetoAssimp() {
	glUniform1i(assimp, 1);
	// sets the model matrix to a scale matrix so that the model fits in the window
	pushMatrix(MODEL);
	translate(MODEL, 86, 5, posicaoComboio);
	scale(MODEL, scaleFactor * 10, scaleFactor * 10, scaleFactor * 10);
	rotate(MODEL, 90, 0, 1, 0);

	aiRecursive_render(scene, scene->mRootNode);
	glUniform1i(assimp, 0);
	posicaoComboio -= 0.5f;
	if (posicaoComboio < 0.0f) {
		posicaoComboio = 1050.0f;
	}
	popMatrix(MODEL);
}

// ------------------------------------------------------------
//
// Render stufff
//


void renderScene(void) {

	//printf("LuzEsq: %f, %f, %f\n", aabb.luzEsquerda[0], aabb.luzEsquerda[1], aabb.luzEsquerda[2]);
	//printf("LuzDir: %f, %f, %f\n", aabb.luzDireita[0], aabb.luzDireita[1], aabb.luzDireita[2]);

	GLint loc;
	float pos[3], right[3], up[3];
	//printf("%f\n", rotasaoLado);

	timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = timeSinceStart - oldTimeSinceStart;
	deltaTime = deltaTime / 1000;
	oldTimeSinceStart = timeSinceStart;

	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);

	if (!pause) {

		pontosCounter++;
		if (pontosCounter == 27) {
			pontos += 1;
			pontosCounter = 0;
		}

		missilTimer++;
		if (missilTimer == 20) {
			//printf("ola1 %f\n", posisaoYUm);
			infoMisseisUm[missilIndexUm][0] = rotasaoLadoUm;
			infoMisseisUm[missilIndexUm][1] = rotasaoCimaUm;
			infoMisseisUm[missilIndexUm][2] = posisaoXUm;
			infoMisseisUm[missilIndexUm][3] = posisaoYUm;
			infoMisseisUm[missilIndexUm][4] = posisaoZUm;
			//printf("rL:%f rC:%f pX:%f pY:%f pZ:%f", infoMisseis[missilIndexUm][0], infoMisseis[missilIndexUm][1], infoMisseis[missilIndexUm][2], infoMisseis[missilIndexUm][3], infoMisseis[missilIndexUm][4]);
			missilIndexUm = (missilIndexUm + 1) % N_MISSEIS_MAX;
		}
		else if (missilTimer == 40) {
			//printf("ola2 %f\n", posisaoYDois);
			infoMisseisDois[missilIndexDois][0] = rotasaoLadoDois;
			infoMisseisDois[missilIndexDois][1] = rotasaoCimaDois;
			infoMisseisDois[missilIndexDois][2] = posisaoXDois;
			infoMisseisDois[missilIndexDois][3] = posisaoYDois;
			infoMisseisDois[missilIndexDois][4] = posisaoZDois;
			missilIndexDois = (missilIndexDois + 1) % N_MISSEIS_MAX;
		}
		else if (missilTimer == 60) {
			//printf("ola3 %f\n", posisaoYTres);
			infoMisseisTres[missilIndexTres][0] = rotasaoLadoTres + 0.0f;
			infoMisseisTres[missilIndexTres][1] = rotasaoCimaTres + 0.0f;
			infoMisseisTres[missilIndexTres][2] = posisaoXTres + 0.0f;
			infoMisseisTres[missilIndexTres][3] = posisaoYTres + 0.0f;
			infoMisseisTres[missilIndexTres][4] = posisaoZTres + 0.0f;
			missilIndexTres = (missilIndexTres + 1) % N_MISSEIS_MAX;
		}
		if (missilTimer == 60) {
			missilTimer = 0;
		}

		if (estaAcelerar && acelerasao < 2.0f) {
			acelerasao += 0.05f * deltaTime * 100;
		}
		else {
			if (acelerasao > 0.0f) {
				acelerasao -= 0.005f * deltaTime * 100;
			}
		}

		if (imunidade > 0) {
			imunidade -= 1;
		}
	}

	if (diaLigado == 0) {
		glUniform1i(dia, 0);
	}
	else {
		glUniform1i(dia, 1);
	}
	if (pointLightsLigadas == 0) {
		glUniform1i(pointLights, 0);
	}
	else {
		glUniform1i(pointLights, 1);
	}

	glUniform4fv(corVariavel_loc, 1, corLuz);

	glUniform1i(fogF, fogFlag);

	//atualizar o buffer circular (atraso da camera)
	camIndex = (camIndex + 1) % 20;

	posIndex = (posIndex + 1) % 20;
	atrasoCamera[posIndex][0] = posisaoX;
	atrasoCamera[posIndex][1] = posisaoY;
	atrasoCamera[posIndex][2] = posisaoZ;

	// use our shader
	glUseProgram(shader.getProgramIndex());

	float ratio = (float)WinX / WinY;

	int dirCamera = 1;
	// set the camera using a function similar to gluLookAt
	switch (camera) {
	case 0:
		minigame = false;
		if (rotasaoCima <= 295.5f && rotasaoCima > 115.5f) {
			dirCamera = -1;
		}
		// camera movel com motion sickness
		lookAt(atrasoCamera[camIndex][0] - (10 * cos(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)) + 5 * sin(-rotasaoCima * PI / 180) * cos(rotasaoLado * PI / 180), atrasoCamera[camIndex][1] - 10 * sin((rotasaoCima)*PI / 180) + 5 * cos(-rotasaoCima * PI / 180), atrasoCamera[camIndex][2] - (10 * sin(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)) + 5 * sin(-rotasaoCima * PI / 180) * sin(rotasaoLado * PI / 180), posisaoX, posisaoY, posisaoZ, 0, 1 * dirCamera, 0);
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
		luzSpot = 1;
		//camera de cima perspetiva
		lookAt(496.0, 400.0, 496.0, 496.0, 0.0, 496.0, 1, 0, 0);
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		if (!minigame) {
			rotasaoCima = 0.0f;
			rotasaoCimaUm = 0.0f;
			rotasaoCimaDois = 0.0f;
			rotasaoCimaTres = 0.0f;
			posisaoY = 200.0f;
			posisaoYUm = 200.0f;
			posisaoYDois = 200.0f;
			posisaoYTres = 200.0f;
			minigame = true;
		}
		break;
	case 3:
		minigame = false;
		if (rotasaoCima <= 295.5f && rotasaoCima > 115.5f) {
			dirCamera = -1;
		}
		// camera movel
		lookAt(posisaoX - (10 * cos(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)) + 5 * sin(-rotasaoCima * PI / 180) * cos(rotasaoLado * PI / 180), posisaoY - 10 * sin((rotasaoCima)*PI / 180) + 5 * cos(-rotasaoCima * PI / 180), posisaoZ - (10 * sin(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)) + 5 * sin(-rotasaoCima * PI / 180) * sin(rotasaoLado * PI / 180), posisaoX, posisaoY, posisaoZ, 0, 1 * dirCamera, 0);
		//posisaoCamera[0] = (posisaoX - 3 * cos(-rotasaoLado * PI / 180)) + camX;
		//posisaoCamera[1] = (posisaoZ - 3 * sin(-rotasaoLado * PI / 180)) + camZ;
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		break;
	case 4:
		minigame = false;
		//TA MAL, FAZER CONTAS
		//lookAt(posisaoX /* + 5 * sin(-rotasaoCima * PI / 180) * cos(rotasaoLado * PI / 180) */ , posisaoY /* - 5 * sin((rotasaoCima)*PI / 180) + 5 * cos(-rotasaoCima * PI / 180)*/, posisaoZ/* + 5 * sin(-rotasaoCima * PI / 180) * sin(rotasaoLado * PI / 180)*/, posisaoX + (10 * cos(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)) - 5 * sin(-rotasaoCima * PI / 180) * cos(rotasaoLado * PI / 180), posisaoY + 10 * sin((rotasaoCima)*PI / 180) - 5 * cos(-rotasaoCima * PI / 180), posisaoZ + (10 * sin(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)) - 5 * sin(-rotasaoCima * PI / 180) * sin(rotasaoLado * PI / 180), 0, 1, 0);
		lookAt(posisaoX + 1 * sin(-rotasaoCima * PI / 180) * cos(rotasaoLado * PI / 180), posisaoY + 1 * cos(-rotasaoCima * PI / 180), posisaoZ + 1 * sin(-rotasaoCima * PI / 180) * sin(rotasaoLado * PI / 180), posisaoX + (10 * cos(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180))/* - 1 * sin(-rotasaoCima * PI / 180) * cos(rotasaoLado * PI / 180)*/, posisaoY + 10 * sin((rotasaoCima)*PI / 180)/* - 1 * cos(-rotasaoCima * PI / 180)*/, posisaoZ + (10 * sin(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180))/* - 1 * sin(-rotasaoCima * PI / 180) * sin(rotasaoLado * PI / 180)*/, 0, 1 * dirCamera, 0);

		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		break;
	case 9:
		minigame = false;
		// camera movel
		lookAt(luzesLocais[12][0], 3, luzesLocais[12][2], 496.0f, 150.0f, 496.0f, 0, 1, 0);
		//posisaoCamera[0] = (posisaoX - 3 * cos(-rotasaoLado * PI / 180)) + camX;
		//posisaoCamera[1] = (posisaoZ - 3 * sin(-rotasaoLado * PI / 180)) + camZ;
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);
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

	//Indicar aos samplers do GLSL quais os Texture Units a serem usados

	glUniform1i(tex_loc, 0);
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
	glUniform1i(tex_loc3, 3);
	glUniform1i(tex_loc4, 4);
	glUniform1i(tex_loc5, 5);
	glUniform1i(tex_loc6, 6);
	glUniform1i(tex_loc7, 7);
	glUniform1i(tex_loc8, 8);
	glUniform1i(tex_loc9, 9);
	glUniform1i(tex_loc10, 10);
	glUniform1i(tex_loc11, 11);
	glUniform1i(tex_loc12, 12);
	glUniform1i(tex_loc13, 0);
	glUniform1i(tex_cube_loc, 13);
	glUniform1i(tex_loc14, 14);
	glUniform1i(tex_normalMap_loc, 15);
	glUniform1i(tex_loc16, 16);

	//send the light position in eye coordinates
	//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

	glUniform1i(spotlightOn, luzSpot);
	float res[4];
	multMatrixPoint(VIEW, lightPos, res);   //lightPos definido em World Coord so is converted to eye space
	glUniform4fv(lPos_uniformId, 1, res);

		multMatrixPoint(VIEW, luzesLocais[0], res);
		glUniform4fv(luzLocal1_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[1], res);
		glUniform4fv(luzLocal2_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[2], res);
		glUniform4fv(luzLocal3_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[3], res);
		glUniform4fv(luzLocal4_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[4], res);
		glUniform4fv(luzLocal5_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[5], res);
		glUniform4fv(luzLocal6_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[6], res);
		glUniform4fv(luzLocal7_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[7], res);
		glUniform4fv(luzLocal8_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[8], res);
		glUniform4fv(luzLocal9_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[9], res);
		glUniform4fv(luzLocal10_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[10], res);
		glUniform4fv(luzLocal11_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[11], res);
		glUniform4fv(luzLocal12_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[12], res);
		glUniform4fv(luzLocal13_loc, 1, res);
		multMatrixPoint(VIEW, luzesLocais[13], res);
		glUniform4fv(luzLocal16_loc, 1, res);

	float loc14[4] = { aabb.luzEsquerda[0],aabb.luzEsquerda[1],aabb.luzEsquerda[2],1.0f };
	multMatrixPoint(VIEW, loc14, res);
	glUniform4fv(luzLocal14_loc, 1, res);

	float loc15[4] = { aabb.luzDireita[0],aabb.luzDireita[1],aabb.luzDireita[2],1.0f };
	multMatrixPoint(VIEW, loc15, res);
	glUniform4fv(luzLocal15_loc, 1, res);
	//printf("%f\n", rotasaoLado);
	//float spotDir[4] = { 1.0*cos(rotasaoLado*PI/180),1.0*sin(rotasaoCima*PI/180),1.0*sin(rotasaoLado*PI/180),1.0f };
	float spotDir[4] = { 0.0,0.0,-1.0,1.0 };
	glUniform4fv(spotLightDir_loc, 1, spotDir);

	int objId = 0; //id of the object mesh - to be used as index of mesh: Mymeshes[objID] means the current mesh


	renderWorld();



	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 0x0, 0x0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	if (!pause) {
		respawnaAviao();
		applyRotation();
	}

	//renderAssimp
	renderObjetoAssimp();

	//nosso
	nossoAviao = true;
	if (imunidade > 0 && imunidade % 12 == 0) {
		verAviao = !verAviao;
	}
	if (imunidade == 0) {
		verAviao = true;
	}
	//if(verAviao)
	renderAviao(posisaoX, posisaoY, posisaoZ, rotasaoCima, rotasaoLado, rotPlaneV, rotPlaneH);

	nossoAviao = false;
	renderAviao(posisaoXUm, posisaoYUm, posisaoZUm, rotasaoCimaUm, rotasaoLadoUm, 0.0f, 0.0f);

	renderAviao(posisaoXDois, posisaoYDois, posisaoZDois, rotasaoCimaDois, rotasaoLadoDois, 0.0f, 0.0f);

	renderAviao(posisaoXTres, posisaoYTres, posisaoZTres, rotasaoCimaTres, rotasaoLadoTres, 0.0f, 0.0f);

	renderCity();

	renderChao();

	renderSigns();

	renderArvores(496, 280, 496, 10, 10, 10, 0);

	renderAros();

	renderTorre();

	updateMisseis();

	updateMisseisInimigos();

	renderBumpMapping();

	renderCubeEnvironment();





	/*if (!bateu) {*/
	handleCollisions();
	/*	rotLCam = rotasaoLado;
		rotCCam = rotasaoCima;
	}*/

	if (fireworks) {
		mandaEfeitos();
	}

	//Minimapa
	if (camera == 3 || camera == 4) {
		glUniform1i(mapa, 1);
		//glDisable(GL_DEPTH_TEST);
		glViewport(WinX - WinX / 6, WinY / 36, WinX / 5, WinY / 5);
		loadIdentity(VIEW);
		lookAt(posisaoX - (10 * cos(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)), posisaoY - 10 * sin((rotasaoCima)*PI / 180), posisaoZ - (10 * sin(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180)), posisaoX, posisaoY, posisaoZ, 0, 1, 0);
		loadIdentity(PROJECTION);
		perspective(53.13f, ratio, 0.1f, 1000.0f);

		renderMiniMapa();
		loadIdentity(VIEW);
		lookAt(posisaoX, 200, posisaoZ, posisaoX, 0.0f, posisaoZ, 1, 0, 0);
		loadIdentity(PROJECTION);
		ortho(-40 * ratio, 40 * ratio, -40, 40, -500, 500);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 0x1, 0x1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glDisable(GL_DEPTH_TEST);

		renderChao();

		//renderAssimp
		renderObjetoAssimp();

		renderCity();

		renderSigns();

		renderAros();

		renderTorre();

		//renderAviao(posisaoX, posisaoY, posisaoZ, rotasaoCima, rotasaoLado, rotPlaneV, rotPlaneH);
		renderAviaoCone(posisaoX, posisaoY, posisaoZ, rotasaoLado);

		renderAviao(posisaoXUm, posisaoYUm, posisaoZUm, rotasaoCimaUm, rotasaoLadoUm, 0.0f, 0.0f);

		renderAviao(posisaoXDois, posisaoYDois, posisaoZDois, rotasaoCimaDois, rotasaoLadoDois, 0.0f, 0.0f);

		renderAviao(posisaoXTres, posisaoYTres, posisaoZTres, rotasaoCimaTres, rotasaoLadoTres, 0.0f, 0.0f);

		updateMisseis();

		updateMisseisInimigos();

		glDisable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, WinX, WinY);
		glUniform1i(mapa, 0);
		//ACABOU MINIMAPA
	}


	multMatrixPoint(VIEW, spotLights, res);
	glUniform4fv(spotLight1, 1, res);
	multMatrixPoint(VIEW, spotLightDirection, res);
	glUniform4fv(spotDirection, 1, res);

	if (!bateu && !pause) {
		posisaoX += (0.3f + acelerasao) * cos(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180) * deltaTime * 100;
		posisaoY += (0.3f + acelerasao) * sin(rotasaoCima * PI / 180) * deltaTime * 100;
		posisaoZ += (0.3f + acelerasao) * sin(rotasaoLado * PI / 180) * cos(rotasaoCima * PI / 180) * deltaTime * 100;

		posisaoXUm += (0.3f) * cos(rotasaoLadoUm * PI / 180) * cos(rotasaoCimaUm * PI / 180) * deltaTime * 100;
		posisaoYUm += (0.3f) * sin(rotasaoCimaUm * PI / 180) * deltaTime * 100;
		posisaoZUm += (0.3f) * sin(rotasaoLadoUm * PI / 180) * cos(rotasaoCimaUm * PI / 180) * deltaTime * 100;

		posisaoXDois += (0.3f) * cos(rotasaoLadoDois * PI / 180) * cos(rotasaoCimaDois * PI / 180) * deltaTime * 100;
		posisaoYDois += (0.3f) * sin(rotasaoCimaDois * PI / 180) * deltaTime * 100;
		posisaoZDois += (0.3f) * sin(rotasaoLadoDois * PI / 180) * cos(rotasaoCimaDois * PI / 180) * deltaTime * 100;

		posisaoXTres += (0.3f) * cos(rotasaoLadoTres * PI / 180) * cos(rotasaoCimaTres * PI / 180) * deltaTime * 100;
		posisaoYTres += (0.3f) * sin(rotasaoCimaTres * PI / 180) * deltaTime * 100;
		posisaoZTres += (0.3f) * sin(rotasaoLadoTres * PI / 180) * cos(rotasaoCimaTres * PI / 180) * deltaTime * 100;

		heatSeek();
	}

	atualizaAviaoInfo();

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (!bateu && !pause) {
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
	contadorBofia = (contadorBofia + 1) % 20;
	if (contadorBofia == 0) {
		luzBofia = (luzBofia == 0) ? 1 : 0;
		glUniform1i(luzBofia_loc, luzBofia);
	}
	if (!pause) {
		carroDaBofia();
		rodaLuzDeCima();
	}
	fazGradienteLuz();
	


	renderTexto();
	desenhaVidas();

	if (flareEffect && !spotlight_mode) {

		int flarePos[2];
		int m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		pushMatrix(MODEL);
		loadIdentity(MODEL);
		computeDerivedMatrix(PROJ_VIEW_MODEL);  //pvm to be applied to lightPost. pvm is used in project function
		glUniform1i(texMode_uniformId, 15);

		if (!project(luzesLocais[13], lightScreenPos, m_viewport))
			printf("Error in getting projected light in screen\n");  //Calculate the window Coordinates of the light position: the projected position of light on viewport
		flarePos[0] = clampi((int)lightScreenPos[0], m_viewport[0], m_viewport[0] + m_viewport[2] - 1);
		flarePos[1] = clampi((int)lightScreenPos[1], m_viewport[1], m_viewport[1] + m_viewport[3] - 1);
		popMatrix(MODEL);

		//viewer looking down at  negative z direction
		pushMatrix(PROJECTION);
		loadIdentity(PROJECTION);
		pushMatrix(VIEW);
		loadIdentity(VIEW);
		ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
		render_flare(&AVTflare, flarePos[0], flarePos[1], m_viewport);
		popMatrix(PROJECTION);
		popMatrix(VIEW);
	}



	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	int shift = glutGetModifiers();

	switch (key) {

	case 27:
		glutLeaveMainLoop();
		break;

	case 'c':
		//printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
		printf("Pressed c \n");
		pointLightsLigadas = (pointLightsLigadas == 0 ? 1 : 0);
		break;
	case 'm': glEnable(GL_MULTISAMPLE); break;
	case 'l': printf("X:%f Z:%f  Y:%f\n", posisaoX, posisaoZ, posisaoY); break;
	case 'k':
		if (spotlight_mode) flareEffect = false;
		else
			if (flareEffect) flareEffect = false;
			else flareEffect = true;
		break;
	case 'n':
		printf("Pressed n \n");
		//glDisable(GL_MULTISAMPLE);
		diaLigado = (diaLigado == 0 ? 1 : 0);
		if (!diaLigado) {
			glClearColor(0.15f, 0.05f, 0.3f, 1.0f);
			pointLightsLigadas = 1;
		}
		else {
			glClearColor(0.81f, 0.91f, 0.98f, 1.0f);
			pointLightsLigadas = 0;
		}
		break;
	case 'p':
		if (vidas > 0) {
			pause = !pause;
		}
		break;
	case 'r':
		pause = false;
		vidas = 3;
		pontos = 0;
		posisaoX = 0.0f;
		posisaoY = 125.0f;
		posisaoZ = 0.0f;
		rotasaoCima = 0.0f;
		rotasaoLado = 0.0f;
		break;
	case 'a':
		if (rotPlaneH > -45.0f) {
			rotPlaneH -= 1.0f;
			isRotatingH = true;
		}
		o = true;
		break;
	case 'e':
		printf("Pressed e\n");
		iniParticles(fireworkCounter);
		fireworkCounter = (fireworkCounter + 1) % 8;
		break;
	case 'd':
		if (rotPlaneH < 45.0f) {
			rotPlaneH += 1.0f;
			isRotatingH = true;
		}
		p = true;
		break;
	case 'w':
		if (!minigame) {
			if (rotPlaneV > -45.0f) {
				rotPlaneV -= 1.0f;
				isRotatingV = true;
			}
			q = true;
		}
		break;
	case 's':
		if (!minigame) {
			if (rotPlaneV < 45.0f) {
				rotPlaneV += 1.0f;
				isRotatingV = true;
			}
			a = true;
		}
		break;
	case 'h':
		estaAcelerar = true;
		break;
	case 'g': if (luzSpot == 0) luzSpot = 1; else luzSpot = 0; break;
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
	case '4':
		camera = 4;
		break;
	case '9':
		camera = 9;
		break;
	case 'f':
		if (fogFlag == 0) fogFlag = 1; else fogFlag = 0;
		break;
	case 32: //espaso
		infoMisseis[missilIndex][0] = rotasaoLado + 0.0f;
		infoMisseis[missilIndex][1] = rotasaoCima + 0.0f;
		infoMisseis[missilIndex][2] = posisaoX + 0.0f;
		infoMisseis[missilIndex][3] = posisaoY + 0.0f;
		infoMisseis[missilIndex][4] = posisaoZ + 0.0f;
		missilIndex = (missilIndex + 1) % N_MISSEIS_MAX;
		if (nMisseisAtivos < 20) {
			nMisseisAtivos += 1;
		}
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
	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());

	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode");
	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	m_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_model");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	view_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_View");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	tex_loc3 = glGetUniformLocation(shader.getProgramIndex(), "texmap3");
	tex_loc4 = glGetUniformLocation(shader.getProgramIndex(), "texmap4");
	tex_loc5 = glGetUniformLocation(shader.getProgramIndex(), "texmap5");
	tex_loc6 = glGetUniformLocation(shader.getProgramIndex(), "texmap6");
	tex_loc7 = glGetUniformLocation(shader.getProgramIndex(), "texmap7");
	tex_loc8 = glGetUniformLocation(shader.getProgramIndex(), "texmap8");
	tex_loc9 = glGetUniformLocation(shader.getProgramIndex(), "texmap9");
	tex_loc10 = glGetUniformLocation(shader.getProgramIndex(), "texmap10");
	tex_loc11 = glGetUniformLocation(shader.getProgramIndex(), "texmap11");
	tex_loc12 = glGetUniformLocation(shader.getProgramIndex(), "texmap12");
	tex_loc13 = glGetUniformLocation(shader.getProgramIndex(), "texmap13");
	tex_loc14 = glGetUniformLocation(shader.getProgramIndex(), "texmap14");
	tex_normalMap_loc = glGetUniformLocation(shader.getProgramIndex(), "normalMap");
	tex_loc16 = glGetUniformLocation(shader.getProgramIndex(), "texmap16");
	luzLocal1_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal1");
	luzLocal2_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal2");
	luzLocal3_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal3");
	luzLocal4_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal4");
	luzLocal5_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal5");
	luzLocal6_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal6");
	luzLocal7_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal7");
	luzLocal8_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal8");
	luzLocal9_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal9");
	luzLocal10_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal10");
	luzLocal11_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal11");
	luzLocal12_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal12");
	luzLocal13_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal13");
	luzLocal14_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal14");
	luzLocal15_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal15");
	luzLocal16_loc = glGetUniformLocation(shader.getProgramIndex(), "luzLocal16");
	tex_cube_loc = glGetUniformLocation(shader.getProgramIndex(), "cubeMap");
	
	dia = glGetUniformLocation(shader.getProgramIndex(), "dia");
	pointLights = glGetUniformLocation(shader.getProgramIndex(), "pointLights");
	corVariavel_loc = glGetUniformLocation(shader.getProgramIndex(), "corVariavel");
	fogF = glGetUniformLocation(shader.getProgramIndex(), "fogFlag");
	spotLight1 = glGetUniformLocation(shader.getProgramIndex(), "spotLight1");
	spotDirection = glGetUniformLocation(shader.getProgramIndex(), "spotDirection");
	luzBofia_loc = glGetUniformLocation(shader.getProgramIndex(), "luzBofia");
	spotlightOn = glGetUniformLocation(shader.getProgramIndex(), "spotlightOn");
	spotLightDir_loc = glGetUniformLocation(shader.getProgramIndex(), "spotLightDirection");
	mapa = glGetUniformLocation(shader.getProgramIndex(), "mapa");
	assimp = glGetUniformLocation(shader.getProgramIndex(), "assimp");

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

	//texturas
	glGenTextures(17, TextureArray);
	Texture2D_Loader(TextureArray, "textures/Flor.png", 0);
	Texture2D_Loader(TextureArray, "textures/grass.jpg", 1);
	Texture2D_Loader(TextureArray, "textures/lines.jpg", 2);
	Texture2D_Loader(TextureArray, "textures/cola.png", 3);
	Texture2D_Loader(TextureArray, "textures/banana.png", 4);
	Texture2D_Loader(TextureArray, "textures/genshin.png", 5);
	Texture2D_Loader(TextureArray, "textures/jumbo.png", 6);
	Texture2D_Loader(TextureArray, "textures/lidl.jpg", 7);
	Texture2D_Loader(TextureArray, "textures/beer.jpg", 8);
	Texture2D_Loader(TextureArray, "textures/tea.jpg", 9);
	Texture2D_Loader(TextureArray, "textures/Plane.png", 10);
	Texture2D_Loader(TextureArray, "textures/Plane-chan.png", 11);
	Texture2D_Loader(TextureArray, "textures/Particla.tga", 12);

	const char* filenames[] = { "textures/posx.jpg", "textures/negx.jpg", "textures/posy.jpg", "textures/negy.jpg", "textures/posz.jpg", "textures/negz.jpg" };
	TextureCubeMap_Loader(TextureArray, filenames, 13);
	Texture2D_Loader(TextureArray, "textures/stone.tga", 14);
	Texture2D_Loader(TextureArray, "textures/normal.tga", 15);
	Texture2D_Loader(TextureArray, "textures/lightwood.tga", 16);
	

	


	//Flare elements textures
	glGenTextures(5, FlareTextureArray);
	Texture2D_Loader(FlareTextureArray, "textures/crcl.tga", 0);
	Texture2D_Loader(FlareTextureArray, "textures/flar.tga", 1);
	Texture2D_Loader(FlareTextureArray, "textures/hxgn.tga", 2);
	Texture2D_Loader(FlareTextureArray, "textures/ring.tga", 3);
	Texture2D_Loader(FlareTextureArray, "textures/sun.tga", 4);

	//---- cenas para assimp ------------------------------------------------
	std::string filepath = "Trains/Trains.fbx";
	strcat(model_dir, "/");  //directory path in the VS project
	//check if file exists
	ifstream fin(filepath.c_str());
	if (!fin.fail()) {
		fin.close();
	}
	else {
		printf("Couldn't open file: %s\n", filepath.c_str());
	}

	//import 3D file into Assimp scene graph
	if (!Import3DFromFile(filepath)) {
		return;
	}

	//creation of Mymesh array with VAO Geometry and Material
	myMeshes2 = createMeshFromAssimp(scene);
	//Acabou-se o assimp

	//Associar os Texture Units aos Objects Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureArray[2]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureArray[3]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TextureArray[4]);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, TextureArray[5]);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, TextureArray[6]);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, TextureArray[7]);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, TextureArray[8]);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, TextureArray[9]);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, TextureArray[10]);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, TextureArray[11]);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, TextureArray[12]);
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[13]);
	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, TextureArray[14]);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[15]);
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureArray[16]);
	
	float amb[] = { 0.15f, 0.10f, 0.2f, 1.0f };
	//float amb[] = { 0.15f, 0.05f, 0.3f, 1.0f};
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

	// create geometry and VAO of the torus  7
	amesh = createTorus(6.5f, 8.0f, 6, 8);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	myMeshes.push_back(amesh);

	// create geometry and VAO of the quad for flare elements 8
	amesh = createQuad(1, 1);
	myMeshes.push_back(amesh);

	//Load flare from file
	loadFlareFile(&AVTflare, "textures/flare.txt");

	srand(time(NULL));

	for (int i = 0; i < Q_PREDIOS; i++) {
		for (int j = 0; j < Q_PREDIOS; j++) {
			if ((i == 10 && j == 10) || (i == 2 && j == 12) || (i == 2 && j == 13) || (i == 3 && j == 12) || (i == 3 && j == 13) || (i == 4 && j == 20)
				|| (i == 5 && j == 19) || (i == 6 && j == 18) || (i == 7 && j == 17) || (i == 8 && j == 16) || (i == 9 && j == 7)
				|| (i == 10 && j == 6) || (i == 11 && j == 5) || (i == 17 && j == 16) || (i == 8 && j == 0) || (i == 9 && j == 1) || (i == 10 && j == 2)
				|| (i == 11 && j == 3) || (i == 12 && j == 4) || (i == 13 && j == 5) || (i == 14 && j == 6) || (i == 15 && j == 7) || (i == 16 && j == 8)
				|| (i == 17 && j == 9) || (i == 18 && j == 10) || (i == 19 && j == 11) || (i == 20 && j == 12) || (i == 9 && j == 0) || (i == 10 && j == 1)
				|| (i == 11 && j == 2) || (i == 12 && j == 3) || (i == 13 && j == 4) || (i == 14 && j == 5) || (i == 15 && j == 6) || (i == 16 && j == 7)
				|| (i == 17 && j == 8) || (i == 18 && j == 9) || (i == 19 && j == 10) || (i == 20 && j == 11) || (i > 8 && i < 12 && j > 8 && j < 12)) {
				alturaPredios[i][j] = 0;
			}
			else {
				alturaPredios[i][j] = (rand() % 75) + 25;
			}
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

	for (int i = 0; i < N_SIGNS; i++) {
		signsX[i] = rand() % Q_PREDIOS;
		signsZ[i] = rand() % Q_PREDIOS;
		while (alturaPredios[signsX[i]][signsZ[i]] == 0 || temSign[signsX[i]][signsZ[i]] == 1) {
			signsX[i] = rand() % Q_PREDIOS;
			signsZ[i] = rand() % Q_PREDIOS;
		}
		temSign[signsX[i]][signsZ[i]] = 1;
		signTypes[i] = rand() % 7 + 1;
		orientasao[i] = rand() % 2;
		//printf("%d\n", orientasao[i]);
	}
	for (int i = 0; i < N_AROS; i++) {
		arosX[i] = rand() % Q_PREDIOS;
		arosZ[i] = rand() % Q_PREDIOS;
		while (alturaPredios[arosX[i]][arosZ[i]] == 0) {
			arosX[i] = rand() % Q_PREDIOS;
			arosZ[i] = rand() % Q_PREDIOS;
		}
		alturaAro[i] = rand() % 80 + 10;
		aroTypes[i] = rand() % 3 + 1;
	}

	/*for (int i = 0; i < Q_PREDIOS; i++) {
		for (int j = 0; j < Q_PREDIOS; j++) {
			printf("%d", temSign[i][j]);
		}
		printf("\n");
	}*/
	luzesLocais[8][0] = 496 + 13.2 * cos(rotasaoLampada * PI / 180);
	luzesLocais[8][2] = 496 + 13.2 * sin(rotasaoLampada * PI / 180);
	luzesLocais[9][0] = 496 + 13.2 * cos((rotasaoLampada + 90) * PI / 180);
	luzesLocais[9][2] = 496 + 13.2 * sin((rotasaoLampada + 90) * PI / 180);
	luzesLocais[10][0] = 496 + 13.2 * cos((rotasaoLampada + 180) * PI / 180);
	luzesLocais[10][2] = 496 + 13.2 * sin((rotasaoLampada + 180) * PI / 180);
	luzesLocais[11][0] = 496 + 13.2 * cos((rotasaoLampada + 270) * PI / 180);
	luzesLocais[11][2] = 496 + 13.2 * sin((rotasaoLampada + 270) * PI / 180);

	atualizaInimigo(1);
	atualizaInimigo(2);
	atualizaInimigo(3);

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.81f, 0.91f, 0.98f, 1.0f);
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

unsigned int getTextureId(char* name) {
	int i;

	for (i = 0; i < NTEXTURES; ++i)
	{
		if (strncmp(name, flareTextureNames[i], strlen(name)) == 0)
			return i;
	}
	return -1;
}
void    loadFlareFile(FLARE_DEF* flare, char* filename)
{
	int     n = 0;
	FILE* f;
	char    buf[256];
	int fields;

	memset(flare, 0, sizeof(FLARE_DEF));

	f = fopen(filename, "r");
	if (f)
	{
		fgets(buf, sizeof(buf), f);
		sscanf(buf, "%f %f", &flare->fScale, &flare->fMaxSize);

		while (!feof(f))
		{
			char            name[8] = { '\0', };
			double          dDist = 0.0, dSize = 0.0;
			float			color[4];
			int				id;

			fgets(buf, sizeof(buf), f);
			fields = sscanf(buf, "%4s %lf %lf ( %f %f %f %f )", name, &dDist, &dSize, &color[3], &color[0], &color[1], &color[2]);
			if (fields == 7)
			{
				for (int i = 0; i < 4; ++i) color[i] = clamp(color[i] / 255.0f, 0.0f, 1.0f);
				id = getTextureId(name);
				if (id < 0) printf("Texture name not recognized\n");
				else
					flare->element[n].textureId = id;
				flare->element[n].fDistance = (float)dDist;
				flare->element[n].fSize = (float)dSize;
				memcpy(flare->element[n].matDiffuse, color, 4 * sizeof(float));
				++n;
			}
		}

		flare->nPieces = n;
		fclose(f);
	}
	else printf("Flare file opening error\n");
}
