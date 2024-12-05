#define FREEGLUT_STATIC // Define a static library for calling functions
#include <GL/freeglut.h> // Include the header file
#include <cmath>
#include <iostream>
#include <math.h>


GLint w = 600, h = 800;

//Utility functions
void rgb2color3f(float colorrgb[3], float color3f[3]) {
	color3f[0] = colorrgb[0] / 255.0f;
	color3f[1] = colorrgb[1] / 255.0f;
	color3f[2] = colorrgb[2] / 255.0f;
}

void Drawshape(float points[][2], int pointcount, float color[3]) {
	float color3f[3];
	rgb2color3f(color, color3f);
	glColor3fv(color3f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < pointcount; i++) {
		glVertex2f(points[i][0], points[i][1]);
	}
	glEnd();
}

void Draweclips(float positionx, float positiony, int radius1, int radius2, bool color) {
	if (color) {
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(positionx, positiony); // center
	}
	else {
		glBegin(GL_LINE_LOOP);
	}
	for (int i = 0; i <= 360; i++) {
		float angle = 2 * 3.1415926 * i / 360;
		float x = positionx + radius1 * cos(angle);
		float y = positiony + radius2 * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
}

// print text
void writetext(char text[], float color[], float position[]) {
	float color3f[3];
	rgb2color3f(color, color3f);
	glColor3fv(color3f);
	glRasterPos2fv(position);
	int len = strlen(text);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
	}
}


//Particles
// rain particle
const int numParticles = 80;
struct Particle {
	float x, y;
	float vy;
};
Particle particles[numParticles];

// star particle
const int numStars = 80; 
struct Star {
	float x, y; // position
	float alpha; // transparency
	float fadeSpeed; 
};
Star stars[numStars];

//backgroud 
int weather = 1;
void sky() {
	//sunset
	float color1[3];
	float color2[3];
	if (weather == 3) {
		color1[0] = 255, color1[1] = 153, color1[2] = 153;
		color2[0] = 255, color2[1] = 225, color2[2] = 102;
	}
	//rain
	else if (weather == 2) {
		color1[0] = 51, color1[1] = 204, color1[2] = 255;
		color2[0] = 204, color2[1] = 238, color2[2] = 255;
	}
	//night
	else if (weather == 1) {
		color1[0] = 0, color1[1] = 51, color1[2] = 153;
		color2[0] = 153, color2[1] = 204, color2[2] = 255;
	}
	float s1[] = { 0,0 };
	float s2[] = { 600,0 };
	float s3[] = { 600,500 };
	float s4[] = { 0,500 };
	float color3f1[3];
	rgb2color3f(color1, color3f1);
	glColor3fv(color3f1);
	glBegin(GL_POLYGON);
	glVertex2fv(s1);
	glVertex2fv(s2);
	float color3f2[3];
	rgb2color3f(color2, color3f2);
	glColor3fv(color3f2);
	glVertex2fv(s3);
	glVertex2fv(s4);
	glEnd();
}

void cb() {
	// cb
	float bk1[][2] = { {115,207}, {108,383}, {300,411},{296,82} };
	float bk1color[3] = { 153,204,255 };
	float bk2[][2] = { {296,82},{300,411},{550,400},{530,190} };
	float bk2color[3] = { 179, 217, 255 };
	float pt1[][2] = { {100,200},{95, 308},{157, 273},{172,154} };
	float pt1color[3] = { 61, 61, 92 };
	float pt2[][2] = { {192,131},{210,320},{300,275},{295,60} };
	float pt3[][2] = { {90,350},{90,400},{300,420},{300,370},{145,330} };
	float pt4[][2] = { {300,370},{300,420},{551,435},{550,330} };
	float pt5[][2] = { {295,60},{300,275},{430,305},{475,220},{500,230},{520,310},{550,330},{545,175},{400,110},{405,160},{375,150},{357,87} };
	float pt5color[3] = { 102, 102, 153 };
	Drawshape(bk1, 4, bk1color);
	Drawshape(bk2, 4, bk2color);
	Drawshape(pt1, 4, pt1color);
	Drawshape(pt2, 4, pt1color);
	Drawshape(pt3, 5, pt1color);
	Drawshape(pt4, 4, pt5color);
	Drawshape(pt5, 12, pt5color);
	glEnd();
}

void grass() {
	float grsup[][2] = { {0,377},{0, 540},{600,560}, {600,428} };
	float grsdown[][2] = { {0,641},{0, 800},{600,800}, {600,732} };
	float grsupcolor[3] = { 153, 204, 0 };
	float grsdowncolor[3] = { 51, 204, 51 };
	Drawshape(grsup, 4, grsupcolor);
	Drawshape(grsdown, 4, grsdowncolor);
	glEnd();
}

void river() {
	float grs[][2] = { {0,505},{0, 641},{600,732}, {600,569} };
	float grscolor[3] = { 102, 204, 255 };
	Drawshape(grs, 4, grscolor);
	glEnd();
}

void bank() {
	float bk[][2] = { {0,524},{0, 540},{600,568}, {600,539} };
	float bkcolor[3] = { 184, 150, 116 };
	Drawshape(bk, 4, bkcolor);
	glEnd();
}

void steps() {
	float stp1[][2] = { {260 - 30 - 30,500},{240 - 30 - 30, 520},{360 - 30 - 30,520}, {380 - 30 - 30,500} };
	float stp2[][2] = { {260 - 30 ,500-30},{240 - 30 , 520 - 30},{360 - 30 ,520 - 30}, {380 - 30 ,500 - 30} };
	float stp3[][2] = { {260 ,500 - 30 - 30},{240, 520 - 30 - 30},{360 ,520 - 30 - 30}, {380,500 - 30 - 30} };
	float stp4[][2] = { {260 + 30 ,500 - 30 - 30 - 30},{240  + 30, 520 - 30 - 30 - 30},{360 + 30 ,520 - 30 - 30 - 30}, {380  + 30,500 - 30 - 30 - 30} };
	float bkcolor[3] = { 255, 212, 138 };
	Drawshape(stp1, 4, bkcolor);
	Drawshape(stp2, 4, bkcolor);
	Drawshape(stp3, 4, bkcolor);
	Drawshape(stp4, 4, bkcolor);
	glEnd();
}

void tree() {
	float trunkcl[3] = { 77, 38, 0 };
	float color[3];
	//tree1
	float t2color[3] = { 51, 77, 0 };
	rgb2color3f(t2color, color);
	glColor3fv(color);
	Draweclips(550, 350, 70, 140, true);
	float trunkpt1[][2] = { {542,330 },{542,520 },{558,520 },{558,330 } };
	Drawshape(trunkpt1, 4, trunkcl);
	//tree2
	float t1color[3] = { 102, 153, 0 };
	rgb2color3f(t1color, color);
	glColor3fv(color);
	Draweclips(480, 400, 60, 100, true);
	float trunkpt2[][2] = { {475,390 },{475,520 },{485,520 },{485,390 } };
	Drawshape(trunkpt2, 4, trunkcl);

	//tree3
	rgb2color3f(t1color, color);
	glColor3fv(color);
	Draweclips(50, 450, 30, 50, true);
	float trunkpt3[][2] = { {47,450 },{47,510 },{53,510 },{53,450 } };
	Drawshape(trunkpt3, 4, trunkcl);
	glEnd();
}

void Background() {
	cb();
	river();
	grass();
	bank();
	steps();
	tree();
}


//animate and interaction
//timer
float newpagex = 600; 
float cloudX = 100; 
bool papersignal = false;
bool textsignal = false;
float currentLine = 1;
float angle = -4;
bool drive = false;
bool angleup = true;
float starangle = 0;
GLfloat scale = 1.0f;  
bool scaleUp = true;  

void update(int value) {
	// cloud
	cloudX += 1;
	if (cloudX > 600) {
		cloudX = -100;
	}
	// card paper
	if (papersignal) {
		if (newpagex > 10) {
			newpagex -= 3;
		}
		else {
			newpagex = 10;
			papersignal = false;
			textsignal = true;
		}
	}
	// boat
	if (drive) {
		if (angle >= 4) {
			angleup = false;
		}
		else if (angle <= -4) {
			angleup = true;
		}
		if (angleup) {
			angle += 0.2;
		}
		else {
			angle -= 0.2;
		}
	}
	// rain
	for (int i = 0; i < numParticles; i++) {
		particles[i].y -= particles[i].vy;
		if (particles[i].y >= 800) {
			particles[i].y = 0;
			particles[i].x = (float)(rand() % 800);
			particles[i].vy = -(float)(rand() % 100) * 0.1;
		}
	}
	// hat 
	starangle += 1;
	if (scaleUp) {
		scale += 0.001;
		if (scale >= 1.1) {
			scaleUp = false;
		}
	}
	else {
		scale -= 0.001;
		if (scale <= 0.5) {
			scaleUp = true;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(16, update, 0);
}


//use KEYBOARD to drive the boat
float ori_x = 0;
float ori_y = 0;
void moveboat(int value) {
	if (drive) {
		ori_x = -2;
		ori_y = -0.2;
	}
}

void keyboard(unsigned char key, int x, int y) {
	//move boat 
	if (key == 'a') {
		drive = true;
		printf("drive");
		moveboat(0);
	}
	//change weather
	if (key == 'w') {
		weather += 1;
		if (weather > 3) {
			weather = 1;
		}
	}
}

// weather
void drawcloud(float radius, float x, float y) {
	glColor3f(1, 1, 1);
	float positionx1 = 300, positiony1 = 300;
	Draweclips(x, y, radius, radius, true);
	Draweclips(x + radius * 3, y, radius, radius, true);
	Draweclips(x + radius * 3 / 2, y - radius, radius, radius, true);
	float block[][2] = { {x,y - radius / 2},{x,y + radius},{x + radius * 3,y + radius},{x + radius * 3,y - radius / 2} };
	float color[] = { 255,255,255 };
	Drawshape(block, 4, color);
}

void cloud() {

	glPushMatrix();
	glTranslatef(cloudX * 2, -100, 0);
	drawcloud(25, 400, 500);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(cloudX, -100, 0);
	drawcloud(20, 300, 300);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(cloudX * 0.7 - 200, -100, 0);
	drawcloud(30, 400, 200);
	glPopMatrix();

}


//init rain
void initParticles() {
	srand(time(NULL));
	for (int i = 0; i < numParticles; i++) {
		particles[i].x = (float)(rand() % 600);
		particles[i].y = (float)(rand() % 800);
		particles[i].vy = -(float)(rand() % 100) * 0.1;
	}
}

void rain() {
	glColor3f(0.9, 1.0, 1.0);
	glBegin(GL_LINES);
	for (int i = 0; i < numParticles; i++) {
		glVertex2f(particles[i].x, particles[i].y);
		glVertex2f(particles[i].x + 5, particles[i].y - 30);  // ÏÂ½µ5¸öÏñËØ£¬Ä£ÄâÓêµÎÏÂÂä
	}
	glEnd();

}

//stars
void initStars() {
	srand(time(NULL));
	for (int i = 0; i < numStars; i++) {
		stars[i].x = (float)(rand() % 800); // random position
		stars[i].y = (float)(rand() % 600);
		stars[i].alpha = 0.8; 
		stars[i].fadeSpeed = (rand() % 1000) / 10000.0;
	}
}


void starcircle(float cx, float cy, float r, float rotatev, int colorchangevalue) {
	glRotatef(rotatev * 0.5, 0.1f, 0.0f, 1.0f); //version2
	//glRotatef(rotatev*0.5, 0.0f, 0.0f, 1.0f); //version1
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 360; i++) {
		if (i > colorchangevalue) {
			glColor3f(0.8, 0.8, 1);
		}
		else {
			glColor3f(0.4, 0.4, 0.8);

		}
		float theta = 2.0f * 3.1415926f * float(i) / float(360);
		float x = r * cosf(theta);
		float y = r * sinf(theta);
		glVertex2f(cx + x, cy + y);
	}
	glEnd();
}

void shiningstar() {

	for (int i = 0; i < numStars; i++) {
		glColor4f(1.0, 1.0, 1.0, stars[i].alpha);
		glBegin(GL_QUADS);
		glVertex2f(stars[i].x - 2, stars[i].y - 2);
		glVertex2f(stars[i].x + 2, stars[i].y - 2);
		glVertex2f(stars[i].x + 2, stars[i].y + 2);
		glVertex2f(stars[i].x - 2, stars[i].y + 2);
		glEnd();

		stars[i].alpha -= stars[i].fadeSpeed * 0.1; 
		if (stars[i].alpha < 0.0) {
			stars[i].x = (float)(rand() % 800); // replace star
			stars[i].y = (float)(rand() % 600);
			stars[i].alpha = 1.0;
		}
	}
}

void star() {
	glPushMatrix();
	glTranslatef(100, 100, 0);
	starcircle(0, 0, 50, starangle, 120);
	starcircle(0, 0, 20, starangle * 0.5, 20);
	starcircle(0, 0, 10, starangle * 0.6, 80);
	starcircle(0, 0, 80, starangle * 0.6, 100);
	starcircle(0, 0, 150, starangle * 0.2, 120);
	starcircle(0, 0, 200, starangle * 0.6, 150);
	starcircle(0, 0, 280, starangle * 0.4, 40);
	starcircle(0, 0, 350, starangle * 0.7, 180);
	starcircle(0, 0, 500, starangle * 0.5, 200);
	glEnd();
	glPopMatrix();
	shiningstar();
}

//boat
float addx = 0, addy = 0;
void boat() {
	glPushMatrix();
	glTranslatef(400 + addx, 600 + addy, 0);
	glPushMatrix();
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	addx += ori_x;
	addy += ori_y;
	float boat[][2] = { {0,0 - 20},{30,62 - 20},{140 ,79 - 20 },{200 ,27 - 20 } };
	float btcolor[3] = { 134, 89, 45 };
	Drawshape(boat, 4, btcolor);
	float flg[][2] = { {100,20 - 20},{100 ,-80 - 20},{140 ,0 - 20 } };
	float flgcolor[3] = { 255, 80, 80 };
	Drawshape(flg, 3, flgcolor);
	if (addx == -80) {
		papersignal = true;
	}
	glEnd();
	glPopMatrix();
	glPopMatrix();
}

// balloon
bool isballoonmov = false;
//float balloonY=0;
float vBalloon = 0;
float ellipseY = -30;
float ellipseX = -30;
void moveBalloon(int value) {
	if (isballoonmov) {
		if (ellipseY > 30) {
			vBalloon = -3;
		}
		else {
			vBalloon = 0;
		}
	}
}
float mousex, mousey;
bool flag = false;
float colorr = 0, colorg = 0, colorb = 0;

void balloonclick(GLint button, GLint state, GLint x, GLint y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { 
		mousex = x;
		mousey = y;
		isballoonmov = true;
		glutPostRedisplay();
		bool flag = true;
		colorr = rand() % 10 / 10.0;
		colorg = rand() % 10 / 10.0;
		colorb = rand() % 10 / 10.0;
		vBalloon = -3;
		ellipseY = y;
		ellipseX = x;
		moveBalloon(0);
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		bool flag = false;
	}
}

void drawBalloon() {
	glPushMatrix();
	float ellipseMajorAxis = 20.0;
	float ellipseMinorAxis = 30.0;

	//ellipseY = mousey+balloonY;
	ellipseY += vBalloon;

	glColor3f(colorr, colorg, colorb);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(ellipseX, ellipseY); 
	for (int i = 0; i <= 360; i++) {
		float angle = 2 * 3.1415926 * i / 360;
		float x = ellipseX + ellipseMajorAxis * cos(angle);
		float y = ellipseY + ellipseMinorAxis * sin(angle);
		glVertex2f(x, y);
	}
	glEnd();
	glColor3f(0.0, 0.0, 0.0);  
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex3f(ellipseX, ellipseY + 30, 0);  
	glVertex3f(ellipseX, ellipseY + 100, 0.0);  // top of string
	glEnd();
	glPopMatrix();
}

//hat
void hat() {
	glScalef(scale, scale, 1.0);
	float hat2[][2] = { {375,650 - 20},{375,710 - 20},{475,710 - 20},{475,650 - 20} };
	float hatcolor2[3] = { 102, 0, 124 };
	float color[3];
	rgb2color3f(hatcolor2, color);
	glColor3fv(color);
	Draweclips(425, 695 - 20, 50, 30, true);
	Drawshape(hat2, 4, hatcolor2);
	float hat1[][2] = { {350,650 - 20},{425,690 - 20},{500,650 - 20},{425,610 - 20} };
	float hatcolor1[3] = { 126, 40, 143 };
	Drawshape(hat1, 4, hatcolor1);
	float dc1[][2] = { {500,630},{497,660},{503,660} };
	float dccolor1[3] = { 255, 204, 0 };
	Drawshape(dc1, 3, dccolor1);

}

//card paper
void newpage() {
	glPushMatrix();
	//float move_x, move_y;
	////photoshop_to_opengl(newpagex,400,move_x,move_y);
	glTranslatef(newpagex, 10, 0);
	float paper[][2] = { {50,50},{50, 750},{550, 750},{550,50} };
	float papercolor[3] = { 255, 255, 255 };
	Drawshape(paper, 4, papercolor);
	float decoration1[][2] = { {60,60},{60, 740},{540, 740},{540,60} };
	float decorationcolor1[3] = { 102, 0, 124 };
	Drawshape(decoration1, 4, decorationcolor1);
	float paper2[][2] = { {70,70},{70, 730},{530, 730},{530,70} };
	float papercolor2[3] = { 255, 255, 255 };
	Drawshape(paper2, 4, papercolor2);
	float hatcolor2[3] = { 102, 0, 124 };
	float color[3];
	rgb2color3f(hatcolor2, color);
	glColor3fv(color);
	Draweclips(350, 150, 40, 40, true);
	Draweclips(400, 150, 40, 40, true);
	glColor3f(1, 1, 1);
	Draweclips(340, 160, 40, 40, true);
	Draweclips(410, 160, 40, 40, true);
	glColor3fv(color);
	Draweclips(420, 150 + 30, 40, 40, true);
	Draweclips(470, 150 + 30, 40, 40, true);
	glColor3f(1, 1, 1);
	Draweclips(340 + 70, 160 + 30, 40, 40, true);
	Draweclips(410 + 70, 160 + 30, 40, 40, true);
	hat();
	glPopMatrix();
}


//text
void welcometext() {
	float textcolor[] = { 0, 102, 0 };

	char text2[] = "press[a] to move the boat";
	float position2[] = { 180, 760 };
	writetext(text2, textcolor, position2);

	char text3[] = "press[w] to change the weather";
	float position3[] = { 165, 730 };
	writetext(text3, textcolor, position3);

}

void lettertext() {
	float textcolor[] = { 0,0,0 };
	char text1[] = "Dear Graduate Students,";
	float position1[] = { newpagex + 100, 160 };
	writetext(text1, textcolor, position1);
	char text2[] = "Congratulations on your well-deserved success!";
	float position2[] = { newpagex + 100, 240 };
	writetext(text2, textcolor, position2);
	char text3[] = "Your hard work and determination have paid off. ";
	float position3[] = { newpagex + 100, 320 };
	writetext(text3, textcolor, position3);
	char text4[] = "As you graduate, remember that this is just  ";
	float position4[] = { newpagex + 100, 400 };
	writetext(text4, textcolor, position4);
	char text5[] = "the beginning of your journey. Embrace the ";
	float position5[] = { newpagex + 100, 480 };
	writetext(text5, textcolor, position5);
	char text6[] = "challenges and opportunities that lie ahead with  ";
	float position6[] = { newpagex + 100, 560 };
	writetext(text6, textcolor, position6);
	char text7[] = " confidence and enthusiasm. ";
	float position7[] = { newpagex + 100, 640 };
	writetext(text7, textcolor, position7);
}


void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	sky();
	//night(behind the building)
	if (weather == 1) {
		star();
	}
	Background();
	welcometext();
	if (weather == 3) {
		cloud();
	}
	//rain
	else if (weather == 2) {
		rain();
	}
	boat();
	newpage();
	drawBalloon();
	lettertext();
	glFlush();
}

void init() {
	glClearColor(1.0, 1.0, 1.0, 1.0); //black background
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);
	initParticles();
	initStars();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(300, 100); 
	glutInitWindowSize(w, h);
	glutCreateWindow("test");
	init();
	glutDisplayFunc(myDisplay); 
	glutTimerFunc(0, update, 0);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(balloonclick);
	glutMainLoop();
}
