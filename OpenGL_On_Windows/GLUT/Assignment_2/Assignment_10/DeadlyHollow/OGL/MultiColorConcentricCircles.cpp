#include <GL/freeglut.h>
#include <cmath>
#define PI 3.1415926535898


//global variable declaration
bool bFullscreen = false; //variable to toggle for fullscreen

int main(int argc, char** argv)
{
	//function prototypes
	void display(void);
	void resize(int, int);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void initialize(void);
	void uninitialize(void);

	//code
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowSize(600, 600); //to declare initial window size
	glutInitWindowPosition(100, 100); //to declare initial window position
	glutCreateWindow("DEADLY HOLLOW CIRCLES WINDOW"); //open the window with "OpenGL First Window : Hello World" in the title bar

	initialize();

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	//	return(0); 
}

void display(void)
{
	void DrawDeadlyHollow();

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DrawDeadlyHollow();

	glutSwapBuffers();
}
void DrawDeadlyHollow()
{
	void DrawTriangle(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
	void DrawMedian(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
	GLfloat CalculateRadius(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
	void DrawCircle(GLfloat, GLfloat);
	void DrawPoint(GLfloat center, GLfloat pointSize);


	GLfloat x1 = 0.0f;
	GLfloat x2 = -0.8f;
	GLfloat x3 = 0.8f;
	GLfloat y1 = 0.8f;
	GLfloat y2 = -0.8f;
	GLfloat y3 = -0.8f;


	GLfloat radius;
	GLfloat Ycordinate; // y coordinate of median of base

	glColor3f(1.0f, 1.0f, 1.0f);
	DrawTriangle(x1, y1, x2, y2, x3, y3);
	DrawMedian(x1, y1, x2, y2, x3, y3);
	radius = CalculateRadius(x1, y1, x2, y2, x3, y3);
	Ycordinate = (y2 + y3) / 2.0f;
	DrawCircle(radius, Ycordinate);
	DrawPoint(Ycordinate + radius, 5.0f);
}
void DrawPoint(GLfloat center, GLfloat pointSize)
{
	glPointSize(pointSize);
	glBegin(GL_POINTS);
	glVertex3f(0.0f, center, 0.0f);
	glEnd();
}
GLfloat CalculateRadius(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3)
{
	GLfloat sideA;
	GLfloat sideB;
	GLfloat radius;

	sideA = sqrt(pow((x3 - x1), 2) + pow((y3 - y1), 2));
	sideB = sqrt(pow((x3 - x2), 2) + pow((y3 - y2), 2));

	radius = sideB * (sqrt((2 * sideA - sideB) / (2 * sideA + sideB))) / 2;

	return radius;
}
void DrawCircle(GLfloat radius, GLfloat ym)
{
	float angle;
	GLint CirclePoints = 1000;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < CirclePoints; i++)
	{
		angle = 2 * PI * i / CirclePoints;
		glVertex3f((radius * cos(angle) + 0.0f), (radius * sin(angle) + (radius + ym)), 0.0f);
	}
	glEnd();
}
void DrawMedian(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(x1, y1, 0.0f);
	glVertex3f((x2 + x3) / 2.0f, (y2 + y3) / 2.0f, 0.0f);
	glEnd();
}
void DrawTriangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(x1, y1, 0.0f);
	glVertex3f(x2, y2, 0.0f);
	glVertex3f(x2, y2, 0.0f);
	glVertex3f(x3, y3, 0.0f);
	glVertex3f(x3, y3, 0.0f);
	glVertex3f(x1, y1, 0.0f);
	glEnd();
}

void initialize(void)
{
	//code
	//to select clearing (background) clear
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void keyboard(unsigned char key, int x, int y)
{
	//code
	switch (key)
	{
	case 27: // Escape
		glutLeaveMainLoop();
		break;
	case 'F':
	case 'f':
		if (bFullscreen == false)
		{
			glutFullScreen();
			bFullscreen = true;
		}
		else
		{
			glutLeaveFullScreen();
			bFullscreen = false;
		}
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	//code
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;
	default:
		break;
	}
}

void resize(int width, int height)
{
	if (height == 0)
		height = 0;
	
	glViewport(0, 0, width, height);
}

void uninitialize(void)
{
	// code
}

