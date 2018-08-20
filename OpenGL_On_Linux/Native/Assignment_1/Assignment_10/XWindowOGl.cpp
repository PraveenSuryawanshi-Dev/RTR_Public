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

int giWindowWidth = 800;
int giWindowHeight = 600;

GLXContext gGlXContext;

static int year = 0;
static int day = 0;

GLUquadric *quadric = NULL;

int main(void)
{

/*
FUNCTION PROTOTYPES
*/
void CreateWindow(void);
void ToggleFullScreen(void);
void Initialize(void);
void Render(void);
void Resize(int, int);
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
char ascii[32];

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


			XLookupString(&event.xkey,ascii,sizeof(ascii),NULL,NULL);
			
			switch(ascii[0])
			{
				case 'D': day = (day + 6) % 360;
					  break;
	
				case 'd': day = (day - 6) % 360;
					  break;
				
				case 'Y': year = (year + 3) % 360;
					  break;

				case 'y': year = (year - 3) % 360;
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
	GLX_RED_SIZE,8,
	GLX_GREEN_SIZE,8,
	GLX_BLUE_SIZE,8,
	GLX_ALPHA_SIZE,8,
	GLX_DEPTH_SIZE,24,
	GLX_DOUBLEBUFFER,True,	
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

XStoreName(gpDisplay,gWindow,"Solar System");

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

}


void Resize(int width, int height)
{
	if(height == 0)
		height =0;

glViewport(0,0,(GLsizei)width,(GLsizei)height);

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);


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
{ 	/*FUNCTION PROTOTYPES*/
	void DrawGalaxy();	
	/**/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DrawGalaxy();

	glXSwapBuffers(gpDisplay,gWindow);
}


void DrawGalaxy()
{
	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glPushMatrix();


	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	glColor3f(1.0f, 1.0f, 0.0f); //YELLOW COLOR

								 /*3rd param :  slices like longitudess
								 4th param : stacks like latitudes.
								 more the 3rd and 4th params, i.e more the subdivisions ,more circular.
								 */
	gluSphere(quadric, 0.75f, 30.0f, 30.0f);
	
	glPopMatrix();
	glPushMatrix();


	glRotatef((GLfloat)year, 0.0f, 1.0f, 0.0f);
	glTranslatef(1.5f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	quadric = gluNewQuadric();
	glColor3f(0.4f, 0.9f, 1.0f);
	gluSphere(quadric, 0.2f, 20.0f, 20.0f);

	glPopMatrix();

}
