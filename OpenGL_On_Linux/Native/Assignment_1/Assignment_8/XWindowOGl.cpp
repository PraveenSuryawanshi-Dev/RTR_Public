#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<string.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>


using namespace std;


bool gbFullScreen = false;

Display *gpDisplay =NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 800;
int giWindowHeight = 600;

GLXContext gGlXContext;

GLfloat pyramideAngle =0.0f , pyramidRotationSpeed = 0.06f;
GLfloat cubeAngle =0.0f, cubeRotationSpeed = 0.07f;


FILE *logfp = NULL;
int main(void)
{

/*
FUNCTION PROTOTYPES
*/
void CreateWindow(void);
void ToggleFullScreen(void);
void Initialize(void);
void Update(void);
void Render(void);
void Resize(int, int);
void UnInitialize(void);
/**/


logfp =	fopen("praveen_OGLlog.txt","w");
	
if(logfp == NULL)
{
	printf("ERROR : Unable to open File.\nExitting Now...\n");
}
fwrite("entering in main() fucntion\n",sizeof(char)	,strlen("entering in main() fucntion\n"),logfp);

bool bDone = false;

int winWidth = giWindowWidth;
int winHeight = giWindowHeight;


/*INITIALIZE WINDOW*/
CreateWindow();
/**/


/*Initialize OPENGL*/
Initialize();
/**/

/*MESSAGE LOOP*/

XEvent event;
KeySym keysym;

while(bDone == false)
{

	while(XPending(gpDisplay))
	{
 	XNextEvent(gpDisplay,&event);	
		switch(event.type)
		{
			case MapNotify:
			     break;

			case KeyPress:
			     keysym = XkbKeycodeToKeysym(gpDisplay , event.xkey.keycode,0,0);
			     switch(keysym)
			     {
				case XK_Escape:
				     bDone = true;
				     exit(0);
	
				case XK_F:
				case XK_f:
					 if(gbFullScreen == false)
					   {
						ToggleFullScreen();
						gbFullScreen = true;
					   }
					 else
					   {
						ToggleFullScreen();
						gbFullScreen =false;
					   }
					 break;

				 default:
					 break;
			     }

			case ButtonPress:
			     switch(event.xbutton.button)
			     {
				case 1:
					break;
			
				case 2:
					break;

				case 3:
					break;
				default:
					break;
			     }
			     break; 

			case MotionNotify:
				break;

			case ConfigureNotify:
			     winWidth = event.xconfigure.width;
			     winHeight = event.xconfigure.height;
			     Resize(winWidth, winHeight);
			     break;

			case Expose:
			     break;

			case DestroyNotify:
			     break;

			case 33:
			     bDone = true;
			     exit(0);		
			     break;
	
			default:
			      break;
		}

	}

	Update();
	Render();
}

/**/


UnInitialize();

fwrite("exit from main() fucntion\n",sizeof(char),strlen("exit from main() fucntion\n"),logfp);
return (0);
}



void CreateWindow(void)
{fwrite("entering in CreateWindow() fucntion\n",sizeof(char),strlen("entering in CreateWindow() fucntion\n"),logfp);

/*FUNCTION PROTO TYPES*/
void UnInitialize(void);
/**/


XSetWindowAttributes winAttribs;
int defaultScreen;
int defaultDepth;
int styleMask;


static int frameBufferAttributes[]=
{
	GLX_RGBA,
	GLX_RED_SIZE,8,
	GLX_GREEN_SIZE,8,
	GLX_BLUE_SIZE,8,
	GLX_ALPHA_SIZE,8,
	GLX_DEPTH_SIZE,24,
	GLX_DOUBLEBUFFER,True,	
};

gpDisplay = XOpenDisplay(NULL);
if(gpDisplay == NULL )
{
	printf("ERROR : Unable to open X Display. \nExitting Now...\n");
	UnInitialize();
	exit(0);
}


defaultScreen = XDefaultScreen(gpDisplay);

/*defaultDepth = DefaultDepth(gpDisplay,defaultScreen);

gpXVisualInfo = (XVisualInfo *) malloc(sizeof(XVisualInfo));
if(gpXVisualInfo == NULL)
{
	printf("ERROR : Unable to Allocate Memory for Visual Info. \nExitting Now... \n");
	UnInitialize();
	exit(0);
}

XMatchVisualInfo(gpDisplay, defaultScreen ,defaultDepth, TrueColor, gpXVisualInfo);
if(gpXVisualInfo == NULL)
{
	printf("ERROR : Unable to Open X Display. \nExitting Now... \n");
	UnInitialize();
	exit(0);

}*/

gpXVisualInfo = glXChooseVisual(gpDisplay,defaultScreen,frameBufferAttributes);



winAttribs.border_pixel = 0;
winAttribs.background_pixmap= 0;
winAttribs.colormap = XCreateColormap(gpDisplay, RootWindow(gpDisplay , gpXVisualInfo->screen),gpXVisualInfo->visual, AllocNone);

gColormap = winAttribs.colormap;

winAttribs.background_pixel= BlackPixel(gpDisplay,defaultScreen);
winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;


styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

gWindow = XCreateWindow(gpDisplay,
			RootWindow(gpDisplay, gpXVisualInfo->screen),
			0,
			0,
			giWindowWidth,
			giWindowHeight,
			0,
			gpXVisualInfo->depth,
			InputOutput,
			gpXVisualInfo->visual,
			styleMask,
			&winAttribs);
if(!gWindow)
{
	printf("ERROR :Failed To Create Main Window. \nExitting Now... \n");
	UnInitialize();
	exit(0);

}

XStoreName(gpDisplay,gWindow,"Log File Generation");

Atom windowManagerDelete = XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);

XMapWindow(gpDisplay, gWindow);

fwrite("exit from CreateWindow() fucntion\n",sizeof(char)	,strlen("exit from CreateWindow() fucntion\n"),logfp);
}


void ToggleFullScreen(void)
{fwrite("entering in ToggleFullScreen() fucntion\n",sizeof(char)	,strlen("entering in ToggleFullScreen() fucntion\n"),logfp);

Atom wm_state;
Atom fullscreen;
XEvent xev = {0};


wm_state = XInternAtom(gpDisplay,"_NET_WM_STATE",False);
memset(&xev,0,sizeof(xev));

xev.type = ClientMessage;
xev.xclient.window = gWindow;
xev.xclient.message_type = wm_state;
xev.xclient.format=32;
xev.xclient.data.l[0] = gbFullScreen ? 0 : 1;


fullscreen = XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);

xev.xclient.data.l[1] = fullscreen;

XSendEvent(gpDisplay,
	   RootWindow(gpDisplay,gpXVisualInfo->screen),
	   False,
	   StructureNotifyMask,
	   &xev);

fwrite("exit from ToggleFullScreen() fucntion\n",sizeof(char),strlen("exit from ToggleFullScreen() fucntion\n"),logfp);
}


void Initialize()
{fwrite("entering in Initialize() fucntion\n",sizeof(char),strlen("entering in Initialize() fucntion\n"),logfp);


/*FUNCTION PROTOTYPES*/
void Resize(int , int);
/**/


gGlXContext = glXCreateContext(gpDisplay,gpXVisualInfo, NULL, GL_TRUE);

glXMakeCurrent(gpDisplay, gWindow, gGlXContext);

glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
glClearDepth(1.0f);
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LEQUAL);
glShadeModel(GL_SMOOTH);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


Resize(giWindowWidth, giWindowHeight);

fwrite("exit form Initialize() fucntion\n",sizeof(char),strlen("exit form Initialize() fucntion\n"),logfp);
}


void Resize(int width, int height)
{fwrite("entering in Resize() fucntion\n",sizeof(char),strlen("entering in Resize() fucntion\n"),logfp);


	if(height == 0)
		height =0;

glViewport(0,0,(GLsizei)width,(GLsizei)height);

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0f,(GLfloat)giWindowWidth/(GLfloat)giWindowHeight,0.1f,100.0f);

fwrite("exit from Resize() fucntion\n",sizeof(char),strlen("exit from Resize() fucntion\n"),logfp);
}



void UnInitialize()
{fwrite("entering in UnInitialize() fucntion\n",sizeof(char),strlen("entering in UnInitialize() fucntion\n"),logfp);


GLXContext currentGLXContext;

currentGLXContext = glXGetCurrentContext();


if(currentGLXContext != NULL && currentGLXContext == gGlXContext)
{
	glXMakeCurrent(gpDisplay,0,0);
}


if(gGlXContext)
{
	glXDestroyContext(gpDisplay,gGlXContext);
}


if(gWindow)
{
	XDestroyWindow(gpDisplay,gWindow);
}

if(gColormap)
{
	XFreeColormap(gpDisplay,gColormap);
}


if(gpXVisualInfo)
{
	free(gpXVisualInfo);
	gpXVisualInfo = NULL;
}


if(gpDisplay)
{
	XCloseDisplay(gpDisplay);
	gpDisplay = NULL;
}

fwrite("exit from UnInitialize() fucntion\n",sizeof(char),strlen("exit from UnInitialize() fucntion\n"),logfp);
fclose(logfp);
}

void Update(void)
{fwrite("entering in Update() fucntion\n",sizeof(char),strlen("entering in Update() fucntion\n"),logfp);

	if(pyramideAngle >= 360.0f)
		pyramideAngle=0.0f;
	else
		pyramideAngle = pyramideAngle + pyramidRotationSpeed;
		
	if(cubeAngle >= 360.0f)
		cubeAngle =0.0f;
	else
		cubeAngle = cubeAngle + cubeRotationSpeed;

fwrite("exit from Update() fucntion\n",sizeof(char),strlen("exit from Update() fucntion\n"),logfp);	
}

void Render()
{fwrite("entering in Render() fucntion\n",sizeof(char),strlen("entering in Render() fucntion\n"),logfp);

	/*FUNCTION PROTOTYPES*/
	void DrawMultiColoredPyramid();
	void DrawCube();
	/**/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(-1.5f,0.0f,-6.0f);
	glRotatef(pyramideAngle,0.0f,1.0f,0.0f); // y axis
	DrawMultiColoredPyramid();


	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f,0.0f,-7.0f);
	glRotatef(cubeAngle,1.0f,0.0f,0.0f); // x -axis
	DrawCube();
	
	glXSwapBuffers(gpDisplay,gWindow);
fwrite("\nexit from Render() fucntion\n",sizeof(char),strlen("\nexit from Render() fucntion\n"),logfp);
}

void DrawMultiColoredPyramid()
{fwrite("entering in DrawMultiColoredPyramid() fucntion\n",sizeof(char),strlen("entering in DrawMultiColoredPyramid() fucntion\n"),logfp);

	glBegin(GL_TRIANGLES);

	/*FRONT FACE*/
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glColor3f(0.0f,1.0f,0.0f);	
	glVertex3f(-1.0f,-1.0f,1.0f);

	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	/*END FRONT FACE*/

	/*BACK FACE*/
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glColor3f(0.0f,1.0f,0.0f);	
	glVertex3f(-1.0f,-1.0f,-1.0f);

	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	/**/

	/*LEFT FACE*/
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);

	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	/**/

	/*RIGHT FACE*/
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);

	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	/**/

	glEnd();


	glBegin(GL_QUADS);
	
	/*BOTTOM FACE*/
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
		
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);

	glColor3f(1.0f,0.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	/**/

	glEnd();

fwrite("exit from DrawMultiColoredPyramid() fucntion\n",sizeof(char),strlen("exit from DrawMultiColoredPyramid() fucntion\n"),logfp);
}

void DrawCube()
{fwrite("entering in DrawCube() fucntion\n",sizeof(char),strlen("entering in DrawCube() fucntion\n"),logfp);

	glBegin(GL_QUADS);

	/*FRONT*/
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	/**/


	/**/
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	/**/


	/*LEFT*/
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	/**/

	/*RIGHT*/
	glColor3f(1.0f, 1.0f, .0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	/**/


	/*TOP*/
	glColor3f(0.0f, 1.0f, 1.0f); 
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	/**/


	/*BOTTOM*/
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	/**/

	glEnd();

fwrite("exit from DrawCube() fucntion\n",sizeof(char),strlen("exit from DrawCube() fucntion\n"),logfp);
}
