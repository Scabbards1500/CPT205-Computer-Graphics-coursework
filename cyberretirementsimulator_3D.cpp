#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <stdlib.h>
#include<math.h>
#include <iostream>
#include "vector"
#define PI 3.14
using namespace std;

const int GL_MULTISAMPLE = 0x809D;

// camera
int intWinWidth = 600; //Default window size
int intWinHeight = 400;
int iswindwillrotate = 0;

float fltFOV = 60; //Field Of View
float eyeX = 0.0; //Camera position
float eyeY = 0.0;
float eyeZ = -500.0;
float centerX = 0.0; //Look At reference point
float centerY = 0.0;
float centerZ = 0.0;
float fltXUp = 0.0; //Up vector
float fltYUp = 1.0;
float fltZUp = 0.0;


GLfloat move_speed = 20;
GLfloat eye_rotation = 90;
GLfloat eye_updown = 0;

//world
int weather = 0;
int daytime = 0; 
int day;




//speed
int updatespeed = 16;

//-------------------------------------------------basic paint------------------------------------
void glColorRGB(int r, int g, int b) {
	glColor3f(r / 255.0f, g / 255.0f, b / 255.0f);
}


void inside(void a(), void b(), GLenum face, GLenum test)
{
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glCullFace(face);
	a();
	glDepthMask(GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glCullFace(GL_BACK);
	b();
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);
	b();
	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(test, 0, 1);
	glDisable(GL_DEPTH_TEST);
	glCullFace(face);
	a();
	glDisable(GL_STENCIL_TEST);
}

// fixes up the depth buffer with A's depth values
void fixup(void a())
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_ALWAYS);
	a();
	// reset depth func
	glDepthFunc(GL_LESS);
}

void drawTruncatedCone(int baseRadius1, int baseRadius2, int topRadius1, int topRadius2, int height, int segments) {
	// material

	GLfloat diffuseMaterial[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat specularMaterial[] = { 0.2, 0.2, 0.2, 1.0 }; 
	GLfloat shininess = 16.0; 
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= segments; ++i) {
		double theta = 2.0 * PI * static_cast<double>(i) / static_cast<double>(segments);
		double x1 = baseRadius1 * cos(theta);
		double z1 = baseRadius2 * sin(theta);
		double x2 = topRadius1 * cos(theta);
		double z2 = topRadius2 * sin(theta);

		glNormal3d(x1, 0.0, z1);
		glVertex3d(x1, -height / 2.0, z1);  // Bottom vertex

		glNormal3d(x2, 0.0, z2);
		glVertex3d(x2, height / 2.0, z2);   // Top vertex
	}
	glEnd();

	// Draw the top and bottom circles
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0, -height / 2.0, 0.0);  // Center of the bottom circle
	for (int i = 0; i <= segments; ++i) {
		double theta = 2.0 * PI * static_cast<double>(i) / static_cast<double>(segments);
		double x = baseRadius1 * cos(theta);
		double z = baseRadius2 * sin(theta);
		glNormal3d(0.0, -1.0, 0.0);
		glVertex3d(x, -height / 2.0, z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0, height / 2.0, 0.0);  // Center of the top circle
	for (int i = 0; i <= segments; ++i) {
		double theta = 2.0 * PI * static_cast<double>(i) / static_cast<double>(segments);
		double x = topRadius1 * cos(theta);
		double z = topRadius2 * sin(theta);
		glNormal3d(0.0, 1.0, 0.0);
		glVertex3d(x, height / 2.0, z);
	}
	glEnd();
}


//--------------------------------------------read texture--------------------------------------------------

//texture

struct image {
	GLint imageWidth;
	GLint imageHeight;
	GLint pixelLength;
};
image loadTexture[10];

vector<GLubyte*>p; // Similar to GLubyte* for program 3 but for 2 images (so a vector)

void ReadImage(const char path[256], GLint& imagewidth, GLint& imageheight, GLint& pixellength) {
	GLubyte* pixeldata;
	FILE* pfile;
	fopen_s(&pfile, path, "rb");
	if (pfile == 0) exit(0);
	fseek(pfile, 0x0012, SEEK_SET);
	fread(&imagewidth, sizeof(imagewidth), 1, pfile);
	fread(&imageheight, sizeof(imageheight), 1, pfile);
	pixellength = imagewidth * 3;
	while (pixellength % 4 != 0)pixellength++;
	pixellength *= imageheight;
	pixeldata = (GLubyte*)malloc(pixellength);
	if (pixeldata == 0) exit(0);
	fseek(pfile, 0x0036, SEEK_SET);
	fread(pixeldata, pixellength, 1, pfile);
	p.push_back(pixeldata); // Similar to glDrawPixels for program 3
	fclose(pfile);
}

GLuint texture[9]; void when_in_mainloop() { // idle callback function
	glutPostRedisplay(); // force OpenGL to redraw the current window
}

void textureinit() {
	const char* pictures[9] = {  // Define a char pointer for the file name to be used 
	"D:/Files/XJTLU/Y3/CPT205-2324-S1-Computer Graphics/Assignment2/texture/grass.bmp",
	"D:/Files/XJTLU/Y3/CPT205-2324-S1-Computer Graphics/Assignment2/texture/sea.bmp",
	//sky
	"D:/Files/XJTLU/Y3/CPT205-2324-S1-Computer Graphics/Assignment2/texture/rainsky2.bmp",
	"D:/Files/XJTLU/Y3/CPT205-2324-S1-Computer Graphics/Assignment2/texture/sunnysky.bmp",
	"D:/Files/XJTLU/Y3/CPT205-2324-S1-Computer Graphics/Assignment2/texture/blue.bmp",
	"D:/Files/XJTLU/Y3/CPT205-2324-S1-Computer Graphics/Assignment2/texture/starrysky.bmp",
	//buildings
	"D:/Files/XJTLU/Y3/CPT205-2324-S1-Computer Graphics/Assignment2/texture/tile.bmp",
	"D:/Files/XJTLU/Y3/CPT205-2324-S1-Computer Graphics/Assignment2/texture/wall.bmp",
	"D:/Files/XJTLU/Y3/CPT205-2324-S1-Computer Graphics/Assignment2/texture/flower.bmp"
	};
	for (int i = 0; i < 9; i++) {
		ReadImage(pictures[i], loadTexture[i].imageWidth, loadTexture[i].imageHeight, loadTexture[i].pixelLength);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &texture[i]);     // Creat texture for the number i texture
		glBindTexture(GL_TEXTURE_2D, texture[i]);    // bind the coresponding texture to the unique ID
		glTexImage2D(GL_TEXTURE_2D, 0, 3, loadTexture[i].imageWidth, loadTexture[i].imageHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, p[i]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);    // Apply GL_MODULATE to combine the texture with material color
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
}

void drawcylinder_tx(int baseradius, int topradius, int height,int slice, int textureindex) {
	glBindTexture(GL_TEXTURE_2D, texture[textureindex]);
	GLUquadricObj* cyliner = gluNewQuadric();
	gluQuadricDrawStyle(cyliner, GLU_FILL);
	gluQuadricNormals(cyliner, GLU_SMOOTH);
	gluQuadricTexture(cyliner, GL_TRUE);
	gluCylinder(cyliner, baseradius, topradius, height, slice, 1);
	gluDeleteQuadric(cyliner);
}

void drawsquare_tx(int textureindex,int zoom) {
	glBindTexture(GL_TEXTURE_2D, texture[textureindex]); 
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 0.0); glVertex3f(-2.0* zoom, 0.0, -1.0 * zoom);
	glTexCoord2f(0.0, 0.0); glVertex3f(-2.0 * zoom, 0.0, 1.0 * zoom);
	glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 0.0, 1.0 * zoom);
	glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 0.0, -1.0 * zoom);
	glEnd();
}

void drawball_tx(int textureindex, int radius) {
	glBindTexture(GL_TEXTURE_2D, texture[textureindex]);
	GLUquadric* quadricObj = gluNewQuadric(); // Create a quadric surface object
	gluQuadricDrawStyle(quadricObj, GLU_FILL);
	gluQuadricNormals(quadricObj, GLU_SMOOTH);
	gluQuadricTexture(quadricObj, GL_TRUE);
	GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = { 100.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glNormal3d(0.0, 1.0, 0.0);
	gluSphere(quadricObj, radius, 80, 80);
	gluDeleteQuadric(quadricObj); // object must be deleted or it will be created every call of the
}

//----------------------------------------texture usage----------------------------------
float backgroundrotate = 0;
void skyball(int textureindex, int radius){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	glScalef(100, 100, 100);
	glRotatef(90, 1, 0, 0);
	glRotatef(backgroundrotate, 0, 0, 1);
	drawball_tx(textureindex,radius);
	glPopMatrix();
	glDisable(GL_BLEND);
}


void testball() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	glColor4f(51.0f/255.0f, 204.0f / 255.0f, 255.0f / 255.0f, 0.5); // 设置颜色的 alpha 分量，控制透明度
	glTranslatef(0.0, -320, 0.0);
	glScalef(300, 30, 300);
	glutSolidCube(20);

	glPopMatrix();
	glDisable(GL_BLEND);
}

float skytransparency = 0;
//weather system
//0 clear
//1 rainy
void skychange() {
	if (weather == 0) {
		glColor4f(1, 1, 1, 0.5-skytransparency);
		skyball(5, 70);
		glColor4f(1, 1, 1, 0.5+skytransparency);
		skyball(3, 60);
	}
	if (weather == 1) {
		glColor4f(1, 1, 1, 0.5 - skytransparency);
		skyball(5, 70);
		glColor4f(1, 1, 1, 0.5 + skytransparency);
		skyball(2, 60);
	}
	
}


int wave = 0;
void sea() {
	glColor4f(1, 1, 1, 0.5);
	for (int i = 0; i < 6; i++) {
			glPushMatrix();
			glTranslatef(-2000 + i * 1600, -10.0, -800+wave);
			glScalef(800, 10, 800);
			drawsquare_tx(1,1);
			glPopMatrix();
	}
	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glTranslatef(-2000 + i * 1600, -10.0, 800 + wave);
		glScalef(800, 10, 800);
		drawsquare_tx(1,1);
		glPopMatrix();
	}

	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glTranslatef(-2000 + i * 1600, -10.0, 2400 + wave);
		glScalef(800, 10, 800);
		drawsquare_tx(1,1);
		glPopMatrix();
	}
	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glTranslatef(-2000 + i * 1600, -10.0, -2400 + wave);
		glScalef(800, 10, 800);
		drawsquare_tx(1,1);
		glPopMatrix();
	}
	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glTranslatef(-2000 + i * 1600, -10.0, 4000 + wave);
		glScalef(800, 10, 800);
		drawsquare_tx(1,1);
		glPopMatrix();
	}
	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glTranslatef(-2000 + i * 1600, -10.0, 5600 + wave);
		glScalef(800, 10, 800);
		drawsquare_tx(1,1);
		glPopMatrix();
	}


}

float growth = 0;
void flower() {
	glColor4f(1, 1, 1, 0.5);
	for (int i = 0; i < 4; i++) {
	glPushMatrix();
	glTranslatef(550-i*180, 80, 2500);
	glRotatef(90, 1, 0, 0);
	glScalef(10+growth, 10 + growth, 10 + growth);
	drawsquare_tx(4, 2);
	glPopMatrix();
	}

	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glTranslatef(550 - i * 180, 80, 2200);
		glRotatef(90, 1, 0, 0);
		glScalef(10 + growth, 10 + growth, 10 + growth);
		drawsquare_tx(8, 2);
		glPopMatrix();
	}

	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glTranslatef(550 - i * 180, 80, 1900);
		glRotatef(90, 1, 0, 0);
		glScalef(10 + growth, 10 + growth, 10 + growth);
		drawsquare_tx(4, 2);
		glPopMatrix();
	}
	
	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glTranslatef(550 - i * 180, 80, 1600);
		glRotatef(90, 1, 0, 0);
		glScalef(10 + growth, 10 + growth, 10 + growth);
		drawsquare_tx(8, 2);
		glPopMatrix();
	}
}

void grassland() {
	//grassland1
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(500, 30.0, 2000);
	glRotatef(-90, 1, 0, 0);
	drawcylinder_tx(1350, 0, 1,36, 0);
	glPopMatrix();
	//grassland2
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(-1000, 55.0, 2000);
	glRotatef(-90, 1, 0, 0);
	drawcylinder_tx(750,0,5, 36, 0);
	glPopMatrix();
}




void windmill() {
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(-1000, 400.0, 1700);
	glRotatef(-90, 1, 0, 0);
	drawcylinder_tx(200, 0, 100, 6,6);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(-1000, 0.0, 1700);
	glRotatef(-90, 1, 0, 0);
	drawcylinder_tx(200, 120, 400,4, 7);
	glPopMatrix();
}


void texturedisplay() {
	glEnable(GL_TEXTURE_2D);
	sea();
	grassland();
	windmill();
	skychange();
	testball();
	flower();
	glDisable(GL_TEXTURE_2D);
}

//----------------------------------------------geometry---------------------------------

void sand() {
	//land1
	glPushMatrix();
	glTranslatef(0, 10.0, 2000);
	glColorRGB(255, 195, 77);
	drawTruncatedCone(1600, 1600, 1500, 1500, 20, 20);
	glPopMatrix();
	//land2
	glPushMatrix();
	glTranslatef(-1500, 40.0, 2000);
	glColorRGB(230, 153, 0);
	drawTruncatedCone(800, 800, 800, 800, 20, 20);
	glPopMatrix();
}


void pond() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	glTranslatef(650, 40.0, 2000);
	glColor4f(77/255.0, 166 / 255.0, 255 / 255.0, 0.8);
	drawTruncatedCone(550, 1000, 550, 1000, 5, 20);
	glPopMatrix();
	glDisable(GL_BLEND);
}

void farmland() {
	//plant
	glPushMatrix();
	glTranslatef(-250, 40.0, 2200);
	glScalef(800 / 10.0, 10 / 10.0, 1500 / 10.0);
	glColorRGB(77,51, 25);
	glutSolidCube(10);
	glPopMatrix();
	//animal
	glPushMatrix();
	glTranslatef(-250, 40.0, 1200);
	glScalef(800.0 / 10.0, 10.0 / 10.0, 500.0 / 10.0);
	glColorRGB(134, 179, 0);
	glutSolidCube(10);
	glPopMatrix();
}

	GLfloat emmission[] = { 0.4, 0.3, 0.1 };
	GLfloat no_emmission[] = { 0.0, 0.0, 0.0 };
void lighthouse() {
	//ground
	glPushMatrix();
	glScalef(1.5, 1.5, 1.5);
	glPushMatrix();
	glTranslatef(-800, 40.0+100, 1300);
	glColorRGB(255, 255, 204);
	drawTruncatedCone(100, 100, 70, 70, 800, 20);
	glPopMatrix();

	//red ring
	glPushMatrix();
	glTranslatef(-800, 400.0 + 100, 1300);
	glColorRGB(179, 0, 0);
	drawTruncatedCone(120, 120, 120, 120, 50, 20);
	glPopMatrix();

	//red cone
	glPushMatrix();
	glTranslatef(-800, 420.0 + 100, 1300);
	glColorRGB(179, 0, 0);
	drawTruncatedCone(90, 90, 80, 80, 150, 20);
	glPopMatrix();


	if (daytime > 1200) {
		glMaterialfv(GL_FRONT, GL_EMISSION, emmission); // Only emmission at night
	}
	else
	{
		glMaterialfv(GL_FRONT, GL_EMISSION, no_emmission);
	}
	glPushMatrix();
	glTranslatef(-800, 500.0 + 100, 1300);
	glColorRGB(204, 255, 255);
	drawTruncatedCone(60, 60, 60, 60, 100, 6);
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, no_emmission);

	glPushMatrix();
	glTranslatef(-800, 550.0 + 100, 1300);
	glColorRGB(179, 0, 0);
	glRotatef(-90, 1, 0, 0);
	glutSolidCone(70, 50, 20, 20);
	glPopMatrix();

	glPopMatrix();
}


void fence()
{
	int segments = 100;
	int baseRadius = 1300;
	for (int i = 0; i <= segments; ++i) {
		double theta = 2.0 * PI * static_cast<double>(i) / static_cast<double>(segments);
		double x = baseRadius * cos(theta);
		double z = baseRadius * sin(theta);

		if (!(40 < i && i < 60))
		{
			glPushMatrix();
			glTranslatef(x, 0, z + 2000);

			glPushMatrix();
			//each fance
			glRotatef((360 /100)*i, 0, 1, 0);
			glTranslatef(0.0, 50.0, 0.0);
			glScalef(80.0 / 5, 5.0 / 5, 10.0 / 5);
			glColor3f(0.7, 0.3, 0.0);
			glutSolidCube(5);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0, 30.0, 0.0);
			glScalef(7.0 / 5, 60.0 / 5, 7.0 / 5);
			glColor3f(0.7, 0.3, 0.0);
			glutSolidCube(5);
			glPopMatrix();

			glPopMatrix();

		}
	}

}

void windmillfan() {
	glPushMatrix;
	glRotatef(iswindwillrotate, 0, 0, 1);
	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glRotatef(-90 * i, 0, 0, 1);
		glTranslatef(0, 150, 0);
		glScalef(5, 25, 1);
		glColorRGB(255, 230, 179);
		glutSolidCube(10);
		glPopMatrix();
	}
	for (int i = 0; i < 5; i++) {
		glPushMatrix();
		glRotatef(-90 * i, 0, 0, 1);
		glTranslatef(0, 0, -10);
		glScalef(0.5, 40, 0.5);
		glColorRGB(102, 51, 1);
		glutSolidCube(10);
		glPopMatrix();
	}
	glPopMatrix();
}

void movewindmall() {
	glPushMatrix();
	glTranslatef(-1500, 400, 1500);
	glRotatef(20, 1, 0, 0);
	windmillfan();
	glPopMatrix();
}

void background() {
	glPushMatrix();
	glTranslatef(500, 0, 0);
	sand();
	lighthouse();
	fence();
	pond();
	farmland();
	movewindmall();
	glPopMatrix();
}

//--------------------------------------------------weather system-----------------------------------------
//--------------------------------------------------weather system-----------------------------------------

const int numParticles = 500;
const float gravity = 0.01;
const float wind = 0.00002;

struct Rain {
	float x, y, z;
	float vy;
};

Rain particles[numParticles];


void initRaindrop() {  
	
	for (int i = 0; i < numParticles; i++) {
		particles[i].x = rand() % 10001 - 5000;
		particles[i].y = rand() % 5000;
		particles[i].z = rand() % 10001 - 5000;
		particles[i].vy = rand() % 100+1;
	}
}

void drawRain() {
	glColor3f(1.0, 1.0, 1.0); 
	glBegin(GL_LINES);
	for (int i = 0; i < numParticles; i++) {
		glVertex3f(particles[i].x, particles[i].y, particles[i].z);
		glVertex3f(particles[i].x + 5, particles[i].y - 90, particles[i].z);  // 下降5个像素，模拟雨滴下落
	}
	glEnd();

}


void sunlight(GLfloat light_x, GLfloat light_y, GLfloat light_z) {
	//GLfloat mat_specular[] = { 1, 1, 1, 1 };
	GLfloat mat_shininess[] = { 30 };
	GLfloat light_position[] = { light_x, light_y, light_z, 0.0 }; // Directional light
	GLfloat light_diffuse[] = { 251.0f / 255.0f , 188.0f / 255.0f, 81.0f / 255.0f, 0.5 };
	GLfloat light_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void moonlight(GLfloat light_x, GLfloat light_y, GLfloat light_z) {
	//GLfloat mat_specular[] = { 1, 1, 1, 1 };
	GLfloat mat_shininess[] = { 10 };
	GLfloat light_position[] = { light_x, light_y, light_z, 0.0 }; // Directional light
	GLfloat light_diffuse[] = { 102 / 255.0f , 153 / 255.0f, 255 / 255.0f, 0.5 };
	GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

//light source
int sunx = 0, suny = 0;
int moonx = 0, moony = 0;
int sun_moonz = -100;
BOOLEAN fog = FALSE;
void sun_moon() {
	int baseRadius = 1500;
	//sun
	double theta = 2.0 * PI * static_cast<double>(daytime) / static_cast<double>(2400);
	double sunx = baseRadius * cos(theta);
	double suny = baseRadius * sin(theta);

	glMaterialfv(GL_FRONT, GL_EMISSION, emmission);
	glPushMatrix();
	glTranslatef(sunx, suny, sun_moonz);
	glColorRGB(255, 255, 204);
	glutSolidSphere(50.0, 20, 20);
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, no_emmission);
	//moon
	double theta2 = 2.0 * PI * static_cast<double>(daytime + 1200) / static_cast<double>(2400);
	double moonx = baseRadius * cos(theta2);
	double moony = baseRadius * sin(theta2);
	glMaterialfv(GL_FRONT, GL_EMISSION, emmission);
	glPushMatrix();
	glTranslatef(moonx, moony, sun_moonz);
	glColorRGB(255, 153, 51);
	glutSolidSphere(30.0, 20, 20);
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, no_emmission);

	if (suny > 0) {
		glEnable(GL_LIGHT0);
		sunlight(sunx, suny, sun_moonz);
	}else{
		glDisable(GL_LIGHT0);
	}
	if (moony > 0) {
		glEnable(GL_LIGHT1);
		moonlight(moonx, moony, sun_moonz);
		//night fog
		if (fog) {
			glEnable(GL_FOG); // Enable fog --> The brightness of an object is varied as a function of its distance to the viewing position
			{
				glFogi(GL_FOG_MODE, GL_LINEAR);
				glFogf(GL_FOG_COLOR, (0.5, 0.5, 0.5, 0.2));
				glFogf(GL_FOG_DENSITY, 0.2);
				glHint(GL_FOG_HINT, GL_DONT_CARE);
				glFogf(GL_FOG_START, 1000);  // Day - fog further
				glFogf(GL_FOG_END, 10000);
			}
		}
		else
		{
			glDisable(GL_FOG);
		}
	}else {
		glDisable(GL_LIGHT1);
		glDisable(GL_FOG);
	}


}

//weather system
//0 clear
//1 rainy
void weather_system() {
	if (weather == 1) {
		drawRain();
	}
}

//------------------------------------------------------animation------------------------------------------------
//------------------------------------------------------animal--------------------------------------------------

//duck

GLfloat leg_rotation = 0;
const int numDucks = 5;

struct Duck {
	float x, y, z;
	float vy;
	int dir;
};

Duck duckparticles[numDucks];


void initDuck() {
	for (int i = 0; i < numDucks; i++) {
		duckparticles[i].x = rand() % 1500 - 300;
		duckparticles[i].z = rand() % 2000;
		duckparticles[i].dir = (rand() % 2 == 0) ? -1 : 1;
	}
}

void drawBody(void)
{
	glPushMatrix(); // Body
	{
		glTranslatef(0, 5, 0);
		glScalef(2, 1, 1.5);
		glColorRGB(255, 255, 204);
		glutSolidCube(10);
	}
	glPopMatrix();
}

void drawLeg(bool flip)
{
	glPushMatrix(); // Leg
	{
		if (flip) glRotatef(-leg_rotation, 0, 0, 1);
		else glRotatef(leg_rotation, 0, 0, 1);
		glTranslatef(0, -3, 0);
		glScalef(0.5, 0.7, 0.5);
		glColorRGB(255, 102, 0);
		glutSolidCube(8);
	}
	glPopMatrix();
}

void drawHead(void)
{
	//head
	glPushMatrix(); 
	glTranslatef(0, -1, 0);
	glScalef(1, 1, 1);
	glColorRGB(255, 255, 204);
	glutSolidCube(9);
	glPopMatrix();

	//eyes
	glPushMatrix();
	glTranslatef(0, -1, -5);
	glScalef(1, 1, 1);
	glColorRGB(0, 0, 0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -1, 5);
	glScalef(1, 1, 1);
	glColorRGB(0, 0, 0);
	glutSolidCube(1);
	glPopMatrix();

	//mouse
	glPushMatrix();
	glTranslatef(5, 0, 0);
	glScalef(1.5, 0.8, 2);
	glColorRGB(255, 102, 0);
	glutSolidCube(3);
	glPopMatrix();
}

void drawduck() {
	glPushMatrix();
	{
		glScalef(4, 4, 4);
		drawBody();
		glPushMatrix(); // Left leg
		glTranslatef(0, 2, -5); // Left leg T R S matrix
		drawLeg(TRUE); // Make it flip so that wave opposite from right leg
		glPopMatrix();
		glPushMatrix(); // Right leg
		glTranslatef(0, 2, 5); // Right leg T R S matrix
		drawLeg(FALSE);
		glPopMatrix();
		glPushMatrix(); // Head
		glTranslatef(6, 15, 0); // Head T R S matrix
		drawHead();
		glPopMatrix();
	}
	glPopMatrix();
}

void generateduck() {
	for (int i = 0; i < numDucks; i++) {
		glPushMatrix();
		glTranslatef(duckparticles[i].x, 70, duckparticles[i].z + 1000);
		if (duckparticles[i].dir==1) {
			glRotatef(180, 0, 1, 0);
		}
		drawduck();
		glPopMatrix();
	}
}


//-----------------------------------------------basic---------------------------------------------------------
// -----------------------------------------------basic---------------------------------------------------------
//time
BOOLEAN skyup = true;
void update(int value) {
	//day time counter
	daytime += 1;
	if (daytime == 2400) {
		daytime = 0;
		day += 1;
	}

	wave += 2;
	if (wave == 1600) {
		wave = 0;
	}

	//rain
	for (int i = 0; i < numParticles; i++) {
		particles[i].y -= particles[i].vy;  
		if (particles[i].y < 0) {
			particles[i].x = rand() % 10001 - 5000;
			particles[i].y = rand() % 5000;
			particles[i].z = rand() % 10001 - 5000;
			particles[i].vy =rand() % 100+1;
		}
	}

	//duck
	if (leg_rotation < 40) {
		leg_rotation += 1;
	}if (leg_rotation == 40) {
		leg_rotation = 0;
	}
	for (int i = 0; i < numDucks; i++) {
		duckparticles[i].x -= duckparticles[i].dir *1;
		if (duckparticles[i].x <= -1500 || duckparticles[i].x >= 1500) {
			duckparticles[i].x *= -1;
		}
	}

	//windmill fan
	iswindwillrotate += 1;
	if (iswindwillrotate == 90) {
		iswindwillrotate = 0;
	}
	

	//skyball 
	backgroundrotate += 0.2;
	if (backgroundrotate == 360) {
		backgroundrotate = 0;
	}
	
	if (skytransparency >= 0.9999) {
		skyup = false;
	}
	else if (skytransparency <= -0.9999) {
		skyup = true;
	}
	if (skyup) {
		skytransparency += 1.0f/ 600.0f;
	}
	else if(!skyup){
		skytransparency -= 1.0f / 600.0f;
	}

	if (growth < 30)
	{
		growth += 0.01;
	}
	glutPostRedisplay();
	glutTimerFunc(updatespeed, update, 0);

}



//camera movements


void KeyboardDisplay(unsigned char key, int x, int y)
{
	if (key == 's' || key == 'S') {
		eye_updown -= 2; //Camera up
	}
	if (key == 'w' || key == 'W'){
		eye_updown += 2; //Camera up
	}
			
	if (key == 'a' || key == 'A') {
		eye_rotation -= 2;
	}
	if (key == 'd' || key == 'D') {
		eye_rotation += 2;
	}
	if (key == 'q' || key == 'Q') {
		if (eyeY < 1000) {
			eyeY += 5;
		}
	}
	if (key == 'e' || key == 'E') {
		if (eyeY > -200) {
			eyeY -= 5;
		}
	}

	if (key == 'r' || key == 'R') {
		if (weather == 0) {
			weather = 1;
		}
		else {
			weather = 0;
		}
	}

	if (key == 'f' || key == 'F') {
		if (fog == FALSE) {
			fog = TRUE;
		}
		else if (fog == TRUE) {
			fog = FALSE;
		}
	}
	centerX = cos(eye_rotation / 180 * PI) * cos(eye_updown / 180 * PI) + eyeX;
	centerY =  sin(eye_updown / 180 * PI) + eyeY;
	centerZ =  sin(eye_rotation / 180 * PI) * cos(eye_updown / 180 * PI) + eyeZ;

	glutPostRedisplay();

}


void specialKeyboardKeys(int key, int xx, int yy) {
	// Move the position of camera and lookat point, to simulate walking
	GLfloat ori_Lookat_x = centerX, ori_Lookat_z = centerZ;	// Record the original position before moving
	switch (key)
	{
	case GLUT_KEY_LEFT:	// Move left //z- x+
		eyeX += move_speed * sin(eye_rotation / 180 * PI);
		eyeZ -= move_speed * cos(eye_rotation / 180 * PI);
		centerX += move_speed * sin(eye_rotation / 180 * PI);
		centerZ -= move_speed * cos(eye_rotation / 180 * PI);
		break;
	case GLUT_KEY_RIGHT: // Move right
		eyeX -= move_speed * sin(eye_rotation / 180 * PI);
		eyeZ += move_speed * cos(eye_rotation / 180 * PI);
		centerX -= move_speed * sin(eye_rotation / 180 * PI);
		centerZ += move_speed * cos(eye_rotation / 180 * PI);
		break;
	case GLUT_KEY_DOWN:	// Move back
		eyeX -= move_speed * cos(eye_rotation / 180 * PI);
		eyeZ -= move_speed * sin(eye_rotation / 180 * PI);
		centerX -= move_speed * cos(eye_rotation / 180 * PI);
		centerZ -= move_speed * sin(eye_rotation / 180 * PI); 
		break;
	case GLUT_KEY_UP: // Move forward
		eyeX += move_speed * cos(eye_rotation / 180 * PI);
		eyeZ += move_speed * sin(eye_rotation / 180 * PI);
		centerX += move_speed * cos(eye_rotation / 180 * PI);
		centerZ += move_speed * sin(eye_rotation / 180 * PI);
		break;
	}

	glutPostRedisplay();
}

void Mouseclick(GLint button, GLint state, GLint x, GLint y) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			updatespeed += 2;
		}
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			if (updatespeed > 2) {
				updatespeed -= 2;
			}
		}
	glutPostRedisplay();
}


void reshapeWindow(GLint intNewWidth, GLint intNewHeight)
{
	glViewport(0, 0, intNewWidth, intNewHeight);
}

void displayObject()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fltFOV, 1, 0.1, 10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX , eyeY , eyeZ , centerX, centerY, centerZ, fltXUp, fltYUp, fltZUp);
	glClearColor(153 / 255.0, 204 / 255.0, 255 / 255.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	background();
	sun_moon();
	texturedisplay();
	weather_system();
	generateduck();
	glutSwapBuffers();
}



void init() {
	glEnable(GL_LIGHTING);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glEnable(GL_MULTISAMPLE); // MSAA
	initRaindrop();
	initDuck();
	textureinit();
}


int main(int argc, char** argv)
{
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Cyber Retirement Simulator");
	init();

	glutDisplayFunc(displayObject);
	glutReshapeFunc(reshapeWindow);
	glutKeyboardFunc(KeyboardDisplay);
	glutSpecialFunc(specialKeyboardKeys);
	glutMouseFunc(Mouseclick);
	glutTimerFunc(0, update, 0);
	glutMainLoop();
}