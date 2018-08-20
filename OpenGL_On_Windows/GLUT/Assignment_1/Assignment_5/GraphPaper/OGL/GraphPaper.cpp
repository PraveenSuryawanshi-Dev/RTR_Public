#include <GL/freeglut.h>

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

	glutInitWindowSize(800, 600); //to declare initial window size
	glutInitWindowPosition(100, 100); //to declare initial window position
	glutCreateWindow("GRAPH PAPER WINDOW"); //open the window with "OpenGL First Window : Hello World" in the title bar

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

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	float y = 0;
	glColor3f(0.0f, 0.0f, 1.0f);
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	for (auto i = 1; i <= 20; i++)
	{
		glVertex3f(1.0f, y, 0.0f);
		glVertex3f(-1.0f, y, 0.0f);
		y = y + 0.05;

	}

	y = -0.0;
	for (auto i = 1; i <= 20; i++)
	{
		glVertex3f(1.0f, y, 0.0f);
		glVertex3f(-1.0f, y, 0.0f);
		y = y - 0.05;

	}


	float x = 0.0;
	for (auto i = 1; i <= 20; i++)
	{
		glVertex3f(x, 1.0f, 0.0f);
		glVertex3f(x, -1.0f, 0.0f);

		x = x - 0.05;

	}
	x = -0.0;
	for (auto i = 1; i <= 20;i++)
	{
		glVertex3f(x, 1.0f, 0.0f);
		glVertex3f(x, -1.0f, 0.0f);

		x = x + 0.05;

	}


	glEnd();

	glLineWidth(3.0f);
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);



	glEnd();

	glutSwapBuffers();
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

