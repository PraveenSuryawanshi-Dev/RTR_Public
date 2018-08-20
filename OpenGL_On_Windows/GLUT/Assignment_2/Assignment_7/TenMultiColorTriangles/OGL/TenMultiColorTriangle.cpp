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
	glutCreateWindow("TEN MULTICOLOT TRIANGLES WINDOW"); //open the window with "OpenGL First Window : Hello World" in the title bar

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
	void DrawTriangle(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat x1 = 0.0f;
	GLfloat y1 = 0.9f;
	GLfloat x2 = -0.9f;
	GLfloat y2 = -0.9f;
	GLfloat  x3 = 0.9f;
	GLfloat y3 = -0.9f;

	GLfloat color[10][3] = {
		{ 1.0f, 0.0f, 0.0f },//red
		{ 0.0f, 1.0f, 0.0f },//green
		{ 0.0f, 0.0f,1.0f },//blue

		{ 0.0f, 1.0f, 1.0f },//cyan
		{ 1.0f, 0.0f, 1.0f },//magenta
		{ 1.0f, 1.0f, 0.0f },//yellow

		{ 1.0f, 1.0f, 1.0f },//white
		{ (255.0f / 255.0f), (16.0f / 255.0f), (0.0f / 255.0f) },//orange
		{ 0.5f, 0.5f, 0.5f },//gray

		{ (165.0f / 255.0f), (42.0f / 255.0f), (42.0f / 255.0f) }//brown

	};

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i = 0; i <= 11; i++)
	{
		glColor3f(color[i][0], color[i][1], color[i][2]);
		DrawTriangle(x1, y1, x2, y2, x3, y3);

		if (y1 > 0)
		{

			y1 = y1 - 0.09f;

			x2 = x2 + 0.09f;
			y2 = y2 + 0.09f;

			x3 = x3 - 0.09f;
			y3 = y3 + 0.09f;
		}

		if (y1 <= 0)
		{
			y1 = y1 + 0.09f;

			x2 = x2 - 0.09f;
			y2 = y2 - 0.09f;

			x3 = x3 + 0.09f;
			y3 = y3 - 0.09f;
		}



	}
	glutSwapBuffers();
}

void DrawTriangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3)
{
	glBegin(GL_LINES);
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

