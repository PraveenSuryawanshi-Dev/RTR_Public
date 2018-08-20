#include <GL/freeglut.h>

//global variable declaration
bool bFullscreen = false; //variable to toggle for fullscreen

int year = 0, day = 0;
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
	glutCreateWindow("PLANET WINDOW"); //open the window with "OpenGL First Window : Hello World" in the title bar

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

	//code

	//to clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glPushMatrix();
	glutWireSphere(1.0f, 20.0f, 16.0f); // draw sun
	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f); // rotate around Y axis.
	glTranslatef(2.0f, 0.0f, 0.0f);

	
	glRotatef((GLfloat)day, 0.0, 1.0f, 0.0f); // rotate around Y axis
	glutWireSphere(0.2f, 10.0f, 8.0f); // draw planet
	glPopMatrix();

	//to process buffered OpenGL Routines
	
	
	glutSwapBuffers();
}

void initialize(void)
{
	//code
	//to select clearing (background) clear
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel (GL_FLAT);
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

	case 'd':
			day = (day + 10) % 360;
			glutPostRedisplay();
		break;
	case 'D':
			day = (day - 10) % 360;
			glutPostRedisplay();
		break;

	case 'y':
			year = (year + 5) % 360;
			glutPostRedisplay();
		break;
	case 'Y':
			year = (year - 5) % 360;
			glutPostRedisplay();
		break;
	default:
		MessageBox(nullptr, TEXT("WRONG KEY IS PRESSED !!!"), TEXT("ENTER A VALID CHOICE"), MB_OK);
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 20.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void uninitialize(void)
{
	// code
}

