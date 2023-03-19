#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>

#define MY_PI (3.1415926)
#define NumSnow 1000
#define TARGET_FPS 60	
#define KEY_EXIT	27 // Escape key.
#define KEY_QUIT	113// q key
#define KEY_STOP	115// s key

const unsigned int FRAME_TIME = 1000 / TARGET_FPS;
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;
unsigned int frameStartTime = 0;

static float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
static float diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static float lightPosition[] = { 0.0f, 0.0f, 1.5f, 1.0f };
static float lightPosition1[] = { 0.0f, 4.0f, 1.5f, 1.0f };
static float lightPosition2[] = { 0.0f, -50.f, -48.f, 1.0f };

float x_plane1, x_plane2, x_plane3, x_plane4;
float z_plane1, z_plane2, z_plane3, z_plane4;

char snow_should_stop = 0;
int currentFPS;
int activeNumSnow = NumSnow;
typedef struct Snow
{
	//x0,y0,z0为雪片起始坐标，t0为起始时间
	//x,y,z为当前坐标，t为当前下落时间
	//active=1表示当前雪片为活跃状态，=0表示非活跃状态
	float x0, y0, z0, t0,t,x,y,z;
	char active;
}Snow;
Snow snows[NumSnow];
	
void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);

void main(int argc, char **argv);
void init(void);
void think(void);

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;

void main(int argc, char **argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Animation");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}
void drawSnows(float x,float y,float z,float R)
{
	int n = 80;     
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glVertex3f(R * cos(2 * MY_PI * i / n)+x, R * sin(2 * MY_PI * i / n)+y,z);
	}
	glEnd();
}
void renderBitmapString(
	float x,
	float y,
	void* font,
	char* str) {

	char* c;
	glRasterPos2f(x, y);
	for (c = str; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(0.0f, 0.0f, 10.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	
	glEnable(GL_COLOR_MATERIAL);

	//背景
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(17.f / 255.f, 119.f / 255.f, 176.f / 255.f);
	glColorMaterial(GL_FRONT, GL_AMBIENT);
	glColor3f(17.f / 255.f, 119.f / 255.f, 176.f / 255.f);

	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-100, 100, -50);
	glVertex3f(-100, -100, -50);
	glVertex3f(100, -100, -50);
	glVertex3f(100, 100, -50);
	glEnd();
	//帽子
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(129.f / 255.f, 60.f / 255.f, 133.f / 255.f);
	glColorMaterial(GL_FRONT, GL_AMBIENT);
	glColor3f(129.f / 255.f, 60.f / 255.f, 133.f / 255.f);

	glPushMatrix();
	glTranslatef(0, 0.5, 0.2);
	glRotatef(-90,1,0,0);
	glutSolidCone(0.3, 2,100, 100);
	glPopMatrix();
	
	//球体
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(0.5, 0.5, 0.5);
	glColorMaterial(GL_FRONT, GL_AMBIENT);
	glColor3f(0.8, 0.8, 0.8);

	glPushMatrix();
	glTranslatef(0, -1, 0);
	glutSolidSphere(0.7, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0.1, 0.2);
	glutSolidSphere(0.5, 50, 50);
	glPopMatrix();
	//眼睛
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(0.0, 0.0, 0.0);
	glColorMaterial(GL_FRONT, GL_AMBIENT);
	glColor3f(0.0, 0.0, 0.0);


	glPushMatrix();
	glTranslatef(-0.2, 0.2, 0.7);
	glutSolidSphere(0.07, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.2, 0.2, 0.7);
	glutSolidSphere(0.07, 50, 50);
	glPopMatrix();
	//鼻子
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(242.f/255.f, 123.f/255.f, 31.f/255.f);
	glColorMaterial(GL_FRONT, GL_AMBIENT);
	glColor3f(242.f / 255.f, 123.f / 255.f, 31.f / 255.f);

	glPushMatrix();
	glTranslatef(0.0, 0.1, 0.7);
	glutSolidSphere(0.07, 5, 5);
	glPopMatrix();
	//多边形平面
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(0.8, 0.8, 0.8);
	glColorMaterial(GL_FRONT, GL_AMBIENT);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_POLYGON); 
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(x_plane1, -2.f, z_plane1);
	glVertex3f(x_plane2, -2.f, z_plane2);
	glVertex3f(x_plane3, -2.f, z_plane3);
	glVertex3f(x_plane4, -2.f, z_plane4);
	glEnd();

	//雪
	int i = 0;
	for (i = 0; i < NumSnow; i++)
	{
		drawSnows(snows[i].x, snows[i].y, snows[i].z, 0.05);
	}
	char fps[10];
	_itoa_s(currentFPS, fps,10,10);
	char str[20] = "current fps = ";
	for (int i = 14; i < 20; i++)
	{
		str[i] = fps[i - 14];
	}
	glColor3f(0.0, 0.0, 0.0);
	renderBitmapString(-4.9, 3.8, GLUT_BITMAP_TIMES_ROMAN_24, str);
	char ans[10];
	_itoa_s(activeNumSnow, ans, 10, 10);
	char str2[30] = "  particles:      of 1000";
	for (int i = 0; i < strlen(ans); i++) {
		str2[i + 13] = ans[i];
	}
	renderBitmapString(-4.9, 3.5, GLUT_BITMAP_TIMES_ROMAN_24, str2);
	char str3[30] = "Scene controls:";
	renderBitmapString(-4.9, 3.2, GLUT_BITMAP_TIMES_ROMAN_24, str3);
	char str4[30] = "  s: toggle snow";
	renderBitmapString(-4.9, 2.9, GLUT_BITMAP_TIMES_ROMAN_24, str4);
	char str5[30] = "  q: quit";
	renderBitmapString(-4.9, 2.6, GLUT_BITMAP_TIMES_ROMAN_24, str5);
	glutSwapBuffers();
}
void reshape(int width, int h)
{
	if (h == 0)
		h = 1;
	float ratio = width * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, h);
	gluPerspective(45 * h * 1.0/ SCREEN_HEIGHT, ratio, 1, 100);
	glMatrixMode(GL_MODELVIEW);
}
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
		/*
			TEMPLATE: Add any new character key controls here.

			Rather than using literals (e.g. "d" for diagnostics), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
		*/
	case KEY_QUIT:
	case KEY_EXIT:
		exit(0);
		break;
	case KEY_STOP: 
		if (snow_should_stop == 0)
		{
			snow_should_stop = 1;
			
		}
		else {
			snow_should_stop = 0;
			activeNumSnow = NumSnow;
			for (int i = 0; i < NumSnow; i++) {
				snows[i].active = 1;
			}
		}
		break;
	}
}
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	currentFPS = 1000 / frameTimeElapsed;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}
void init(void)
{
	glClearColor(92.f/255.f, 179.f/255.f, 204.f/255.f, 1.0);
	gluLookAt(0.0f, 0.0f, 10.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);//开启深度测试
	glShadeModel(GL_SMOOTH);//设置明暗处理,有两种选择模式：GL_FLAT（不渐变）和GL_SMOOTH（渐变过渡）
	/** 设置0号光源 */
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0); 

	/** 设置1号光源 */
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
	glEnable(GL_LIGHT1); 

	/** 设置2号光源 */
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition2);
	glEnable(GL_LIGHT2); 


	srand(time(NULL));
	//随机化初始平面的四个顶点坐标
	x_plane1 = -5 + rand()%5-2;
	x_plane2 =  -5 + rand() % 5 - 2;
	x_plane3 =  5 + rand() % 5 - 2;
	x_plane4 =  5 + rand() % 5 - 2;

	z_plane1 = -5 + rand() % 5 - 2;
	z_plane2 = +5 + rand() % 5 - 2;
	z_plane3 = +5 + rand() % 5 - 2;
	z_plane4 = -5 + rand() % 5 - 2;

	int i = 0;
	int scaleX = 100;
	int scaleY = 2500;
	int scaleZ = 5;
	//随机生成每个雪片的起始坐标和起始时间
	for (i = 0; i < NumSnow; i++)
	{
		snows[i].active = 1;

		snows[i].x0 = 0.1f * (rand() % scaleX - (scaleX / 2));
		snows[i].y0 = 5;
		snows[i].z0 = rand() % scaleZ - (scaleZ / 2);
		//若起始时间为负数则暂时不应显示，表示雪花还没有生成
		snows[i].t0 = rand()%1000 - 999;

		snows[i].x = snows[i].x0;
		snows[i].y = snows[i].y0;
		snows[i].z = snows[i].z0;
		snows[i].t = snows[i].t0;
	}
}
void think(void)
{
	int i = 0;
	for (i = 0; i < NumSnow; i++)
	{
		//x坐标归位
		snows[i].x += 0.00001*(rand()%2000-1000);
		//z坐标归位
		snows[i].z = snows[i].z0;
		if (snows[i].t < 0) {
			//如果t小于0的话说明该雪花当前不应被显示，所以将其y坐标设为一个极大值
			snows[i].y = 100;
		}
		else {
			//如果t大于等于0则正常显示，使用重力加速度公式
			snows[i].y = snows[i].y0 - (0.1 * 0.00098 * snows[i].t * snows[i].t);
		}
		//如果雪片的y坐标低于平面
		if (snows[i].y <= -2.f)
		{
			if (!snow_should_stop) {
				//重置y坐标和时间信息
				snows[i].x = snows[i].x0;
				snows[i].y = snows[i].y0;
				snows[i].t = snows[i].t0;
			}
			else {
				if (snows[i].active == 1) {
					snows[i].active = 0;
					snows[i].y = -100;
					activeNumSnow--;
				}
			}
		}
		//每个雪花的下落时间自增1个单位
		snows[i].t += 1;
	}
}
