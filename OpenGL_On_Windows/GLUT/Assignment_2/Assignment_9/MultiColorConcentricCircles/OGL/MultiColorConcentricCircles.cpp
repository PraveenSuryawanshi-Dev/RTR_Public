#include <GL/freeglut.h>
#include <cmath>

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
	glutCreateWindow("MULTI COLOR CONCENTRIC CIRCLES WINDOW"); //open the window with "OpenGL First Window : Hello World" in the title bar

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
	void DrawMultiColoredCircles(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
	//code

	//to clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);

	//to process buffered OpenGL Routines
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	DrawMultiColoredCircles(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f); // red
	DrawMultiColoredCircles(0.0f, 0.0f, 0.9f, 0.0f, 1.0f, 0.0f); // green
	DrawMultiColoredCircles(0.0f, 0.0f, 0.8f, 0.0f, 0.0f, 1.0f); //blue

	DrawMultiColoredCircles(0.0f, 0.0f, 0.7f, 0.0f, 1.0f, 1.0f); //cyan
	DrawMultiColoredCircles(0.0f, 0.0f, 0.6f, 1.0f, 0.0f, 1.0f); // magenta
	DrawMultiColoredCircles(0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f); // yellow

	DrawMultiColoredCircles(0.0f, 0.0f, 0.4f, 1.0f, 1.0f, 1.0f); // white
	DrawMultiColoredCircles(0.0f, 0.0f, 0.3f, (255.0f / 255.0f), (16.0f / 255.0f), (0.0f / 255.0f)); // orange
	DrawMultiColoredCircles(0.0f, 0.0f, 0.2f, 0.5f, 0.5f, 0.5f); //gray

	DrawMultiColoredCircles(0.0f, 0.0f, 0.1f, (165.0f / 255.0f), (42.0f / 255.0f), (42.0f / 255.0f)); // brown

	glutSwapBuffers();
}
void DrawMultiColoredCircles(GLfloat h, GLfloat k, GLfloat radius, GLfloat red, GLfloat green, GLfloat blue)
{

	glColor3f(red, green, blue);
	glBegin(GL_LINES);
	GLfloat step = 0.0f;
	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	while (step <= 360)
	{
		x = h + radius *cos(step);
		y = k + radius *sin(step);

		glVertex3f(x, y, 0.0f);
		step += 0.001f;
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

