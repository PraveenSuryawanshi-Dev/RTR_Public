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
#include<SOIL/SOIL.h>


using namespace std;


bool gbFullScreen = false;

Display *gpDisplay =NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 800;
int giWindowHeight = 600;

GLXContext gGlXContext;

float anglePyramid = 0.0f;
float angleCube = 0.0f;

GLuint Texture_Stone;
GLuint Texture_Kundeli;

int main(void)
{

/*
FUNCTION PROTOTYPES
*/
void CreateWindow(void);
void ToggleFullScreen(void);
void Initialize(void);
void Render(void);
void Update();
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
				/*case '1': _digitPressed=1;
					  break;
	
				case '2': _digitPressed=2;
					  break;
				
				case '3': _digitPressed=3;
					  break;

				case '4': _digitPressed=4;
					  break;
				default : _digitPressed=0;
					  break;*/
	
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
return (0);
}



void CreateWindow(void)
{
/*FUNCTION PROTO TYPES*/
void UnInitialize(void);
int LoadGLTexture(GLuint *,char*);
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

XStoreName(gpDisplay,gWindow,"Simple Texture Loading on Cube and Pyramid");

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
int LoadGLTexture(GLuint *,char*);
/**/

gGlXContext = glXCreateContext(gpDisplay,gpXVisualInfo, NULL, GL_TRUE);

glXMakeCurrent(gpDisplay, gWindow, gGlXContext);

glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
glClearDepth(1.0f);
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LEQUAL);
glShadeModel(GL_SMOOTH);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

 //TEXTURE
glEnable(GL_TEXTURE_2D);

LoadGLTexture(&Texture_Stone,(char*)"Stone.bmp");
LoadGLTexture(&Texture_Kundeli,(char*)"Vijay_Kundali.bmp");

Resize(giWindowWidth, giWindowHeight);

}
int LoadGLTexture(GLuint *texture ,char *path)
{
	int iStatus = 0;

	glGenTextures(1, texture);
    

	// load an image 
	int width=0, height=0, channels=0;
        unsigned char *ht_map = SOIL_load_image
	(
		path,
		&width, &height, &channels,
		SOIL_LOAD_RGB
	);

	// check for an error during the load process 
	if( ht_map != NULL )
	{
		iStatus = 1;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, (GLsizei)width, (GLsizei)height, GL_RGB, GL_UNSIGNED_BYTE, (void*)ht_map);
		
		/* done with the heightmap, free up the RAM */
		SOIL_free_image_data( ht_map );
		
	}

	return iStatus;
}


void Resize(int width, int height)
{
	if(height == 0)
		height =1;

glViewport(0,0,(GLsizei)width,(GLsizei)height);

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);


}



void UnInitialize()
{

GLXContext currentGLXContext;

currentGLXContext = glXGetCurrentContext();


if (Texture_Stone)
{
	glDeleteTextures(1, &Texture_Stone);
	Texture_Stone = 0;
}

if (Texture_Kundeli)
{
	glDeleteTextures(1, &Texture_Kundeli);
	Texture_Kundeli = 0;
}


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
	void Draw3DRotatingPyramid();
	void Draw3DRotatingCube();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Draw3DRotatingPyramid();

	Draw3DRotatingCube();
	
	glXSwapBuffers(gpDisplay,gWindow);

}
void Update()
{
	if (anglePyramid >= 360.0f) anglePyramid = 0.0f;
	anglePyramid = anglePyramid + 0.25f;

	if (angleCube >= 360) angleCube = 0.0f;
	angleCube = angleCube + 0.60f;
}

void  Draw3DRotatingCube()
{

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 0.0f, -6.0f);
	glScalef(0.75f, 0.75f, 0.75f);
	glRotatef(angleCube, 1.0f, 1.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, Texture_Kundeli);
	glBegin(GL_QUADS);

	//Front face
	glTexCoord3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glTexCoord3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	
	glTexCoord3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	
	

	//Back face
	glTexCoord3f(0.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	glTexCoord3f(1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	glTexCoord3f(0.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	//

	
	//Left face
	glTexCoord3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glTexCoord3f(0.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glTexCoord3f(1.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	//


	//Right face
	glTexCoord3f(1.0f,0.0f,1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glTexCoord3f(1.0f,1.0f,1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glTexCoord3f(0.0f,1.0f,-1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glTexCoord3f(0.0f,0.0f,-1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	//
	

	//Top face
	glTexCoord3f(1.0f,1.0f,1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glTexCoord3f(1.0f,0.0f,-1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	glTexCoord3f(0.0f,0.0f,-1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	glTexCoord3f(0.0f,1.0f,1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	//
	
	
	//Bottom face
	glTexCoord3f(0.0f, 1.0f ,1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glTexCoord3f(0.0f ,0.0f,-1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glTexCoord3f(1.0f,0.0f,-1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glTexCoord3f(1.0f,1.0f,1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	//
	glEnd();
}

void Draw3DRotatingPyramid()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-1.5f, 0.0f, -6.0f);
	glRotatef(anglePyramid, 0.0f, 1.0f, 0.0f);


	glBindTexture(GL_TEXTURE_2D, Texture_Stone);

	glBegin(GL_TRIANGLES);

	// Front face
	glTexCoord3f(0.5f,1.0f,0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glTexCoord3f(0.0f,0.0f,1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glTexCoord3f(1.0f,0.0f,1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	//

	//Back Face
	glTexCoord3f(0.5f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glTexCoord3f(0.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glTexCoord3f(1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	//

	//Right face
	glTexCoord3f(0.5f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glTexCoord3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glTexCoord3f(1.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	//

	//Left face
	glTexCoord3f(0.5f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glTexCoord3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glTexCoord3f(1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	//
	glEnd();
}
