#include <GL/freeglut.h>
#include <cmath>
#define PI 3.1415926535898f

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
	glutCreateWindow("CIRCLE GL LINE FUNCTION WINDOW"); //open the window with "OpenGL First Window : Hello World" in the title bar

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
	void DrawCircleWithGLLines();

	//code

	//to clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);

	//to process buffered OpenGL Routines
	glMatrixMode((GL_MODELVIEW));
	glLoadIdentity();

	DrawCircleWithGLLines();

	glutSwapBuffers();
}
void DrawCircleWithGLLines()
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);


	auto circle_points = 10000;
	auto angle = 0.0;
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	for (auto i = 0; i < circle_points; i++)
	{
		angle = 2 * PI * i / circle_points;
		glVertex2f(cos(angle), sin(angle));
	}
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

