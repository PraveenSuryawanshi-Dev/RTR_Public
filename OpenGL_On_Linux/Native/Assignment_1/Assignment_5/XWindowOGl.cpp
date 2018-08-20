#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

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
GLXContext gGlXContext;

int giWindowWidth = 800;
int giWindowHeight = 600;
int x = 0;
int y = 0;


int main(void)
{

/*
FUNCTION PROTOTYPES
*/
void CreateWindow(void);
void ToggleFullScreen(void);
void Initialize(void);
void Render(void);
void Resize(int , int ,int, int);
void UnInitialize(void);
/**/

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
				
				case XK_1:/*LEFT TOP*/
					x = 0;
					y = giWindowHeight / 2;
					Resize(x, y, giWindowWidth / 2, giWindowHeight / 2);
					break;

				case XK_2:/*RIGHT TOP*/
					x = giWindowWidth / 2;
					y = giWindowHeight / 2;
					Resize(x, y, giWindowWidth / 2, giWindowHeight / 2);
					break;

				case XK_3:/*LEFT BOTTOM*/
					x = 0;
					y = 0;
					Resize(x, y, giWindowWidth / 2, giWindowHeight / 2);
					break;

				case XK_4:/*RIGHT BOTTOM*/
					x = giWindowWidth / 2;
					y = 0;
					Resize(x, y, giWindowWidth / 2, giWindowHeight / 2);
					break;

				case XK_5:/*LEFT HALF*/
					x = 0;
					y = 0;
					Resize(x, y, giWindowWidth / 2, giWindowHeight);
					break;

				case XK_6:
					/*RIGHT HALF*/
					x = giWindowWidth / 2;
					y = 0;
					Resize(x, y, giWindowWidth / 2, giWindowHeight);
					break;

				case XK_7:
					/*UPPER HALF*/
					x = 0;
					y = giWindowHeight / 2;
					Resize(x, y, giWindowWidth, giWindowHeight / 2);
					break;

				case XK_8:/*LOWER HALF*/
					x = 0;
					y = 0;
					Resize(x, y, giWindowWidth, giWindowHeight / 2);
					break;

				case XK_9:/*CENTER*/
					x = 0;
					y = 0;
					Resize(x, y, giWindowWidth, giWindowHeight);
					break;


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
			     giWindowWidth = event.xconfigure.width;
			     giWindowHeight = event.xconfigure.height;
			     Resize(x,y,giWindowWidth, giWindowHeight);
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

	Render();
}

/**/


UnInitialize();
return (0);
}



void CreateWindow(void)
{
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
	GLX_RED_SIZE,1,
	GLX_GREEN_SIZE,1,
	GLX_BLUE_SIZE,1,
	GLX_ALPHA_SIZE,1,	
	None,
};

gpDisplay = XOpenDisplay(NULL);
if(gpDisplay == NULL )
{
	printf("ERROR : Unable to open X Display. \nExitting Now...\n");
	UnInitialize();
	exit(0);
}


defaultScreen = XDefaultScreen(gpDisplay);

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

XStoreName(gpDisplay,gWindow,"View Port");

Atom windowManagerDelete = XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);

XMapWindow(gpDisplay, gWindow);
}


void ToggleFullScreen(void)
{

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

}


void Initialize()
{
/*FUNCTION PROTOTYPES*/
void Resize(int, int ,int , int);
/**/

gGlXContext = glXCreateContext(gpDisplay,gpXVisualInfo, NULL, GL_TRUE);

glXMakeCurrent(gpDisplay, gWindow, gGlXContext);

glClearColor(0.0f,0.0f,0.0f,0.0f);

Resize(x,y,giWindowWidth, giWindowHeight);

}


void Resize(int x , int y ,int width, int height)
{
	if(height == 0)
		height =0;

glViewport(x,y,(GLsizei)width,(GLsizei)height);

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0f,(GLfloat)giWindowWidth/(GLfloat)giWindowHeight,0.1f,100.0f);

}



void UnInitialize()
{

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

}



void Render()
{
	/*FUNCTION PROTOTYPES*/
	void DrawMultiColoredTriangle();
	/**/
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f,0.0f,-3.0f);	
	DrawMultiColoredTriangle();
	
	glFlush();
}

void DrawMultiColoredTriangle()
{
	glBegin(GL_TRIANGLES);

	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);

	glColor3f(0.0f,1.0f,0.0f);	
	glVertex3f(-1.0f,-1.0f,0.0f);

	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,0.0f);

	glEnd();
}
