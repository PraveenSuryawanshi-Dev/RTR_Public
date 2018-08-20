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

GLboolean gIsLKeyPressed = GL_FALSE;
GLboolean gIsXKeyPressed = GL_FALSE;
GLboolean gIsYKeyPressed = GL_FALSE;
GLboolean gIsZKeyPressed = GL_FALSE;

GLfloat angle = 0.0f;



typedef struct material
{
    GLUquadric *quadric;
	GLfloat ambient_material[4];
	GLfloat diffuse_material[4];
	GLfloat specular_material[4];
	GLfloat shininess;

}MATERIAL;

MATERIAL  material[6][4];

GLfloat light_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat light_difuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat light_position[] = { 0.0f,0.0f,0.0f,0.0f };


GLfloat light_model_ambient_model[] = { 0.2f,0.2f,0.2f,0.0f };
GLfloat light_model_local_viwer[] = { 0.0f };

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
				case XK_L:
				case XK_l:
					 if(gIsLKeyPressed)
					   {
						glDisable(GL_LIGHTING);		
						gIsLKeyPressed = false;
					   }
					 else
					   {
						glEnable(GL_LIGHTING);
						gIsLKeyPressed = true;
					   }
					 break;			
											
				case XK_X:
				case XK_x:
					light_position[0] = 0.0f;
					light_position[1] = 0.0f;
					light_position[2] = 0.0f;
					if (gIsXKeyPressed == GL_TRUE)
					{
						gIsXKeyPressed = GL_FALSE;
					}
					else
					{
						gIsXKeyPressed = GL_TRUE;
						gIsYKeyPressed = GL_FALSE;
						gIsZKeyPressed = GL_FALSE;
					}
					break;


				case XK_Y:
				case XK_y:
					light_position[0] = 0.0f;
					light_position[1] = 0.0f;
					light_position[2] = 0.0f;
					if (gIsYKeyPressed == GL_TRUE)
					{
						gIsYKeyPressed = GL_FALSE;
					}
					else
					{
						gIsYKeyPressed = GL_TRUE;
						gIsXKeyPressed = GL_FALSE;
						gIsZKeyPressed = GL_FALSE;
					}
					break;

				case XK_Z:
				case XK_z:
					light_position[0] = 0.0f;
					light_position[1] = 0.0f;
					light_position[2] = 0.0f;
					if (gIsZKeyPressed == GL_TRUE)
					{
						gIsZKeyPressed = GL_FALSE;
					}
					else
					{
						gIsZKeyPressed = GL_TRUE;
						gIsXKeyPressed = GL_FALSE;
						gIsYKeyPressed = GL_FALSE;
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

XStoreName(gpDisplay,gWindow,"3d Sphere 3 Lights");

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
void InitializeMaterials();
/**/
	
InitializeMaterials();

gGlXContext = glXCreateContext(gpDisplay,gpXVisualInfo, NULL, GL_TRUE);

glXMakeCurrent(gpDisplay, gWindow, gGlXContext);

glClearColor(0.25f, 0.25f, 0.25f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient_model);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_local_viwer);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_difuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); 
	glEnable(GL_LIGHT0);

Resize(giWindowWidth, giWindowHeight);

}


void Resize(int width, int height)
{
	if(height == 0)
		height =1;

glViewport(0,0,(GLsizei)width,(GLsizei)height);

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);


}



void UnInitialize()
{
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			gluDeleteQuadric(material[i][j].quadric);
			material[i][j].quadric = NULL;
		}
	}

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

void Update()
{
	if (angle >= 360.0f) 
		angle = 0.0f;
	else
		angle = angle + 0.1f;

}

void Render()
{ 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	double row = -0.8f;
	double col =  0.8f;

	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{

			glPushMatrix();

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material[i][j].ambient_material);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material[i][j].diffuse_material);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material[i][j].specular_material);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material[i][j].shininess);

			if (gIsXKeyPressed == GL_TRUE)
			{
				glPushMatrix();
				glRotatef(angle, 1.0f, 0.0f, 0.0f);
				light_position[1] = angle;
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glPopMatrix();
			}else

			if (gIsYKeyPressed == GL_TRUE)
			{
				glPushMatrix();
				glRotatef(angle, 0.0f, 1.0f, 0.0f);
				light_position[0] = angle;
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glPopMatrix();
			}else

			if (gIsZKeyPressed == GL_TRUE)
			{
				glPushMatrix();
				glRotatef(angle, 0.0f, 0.0f, 1.0f);
				light_position[0] = angle;
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
				glPopMatrix();
			}
			else
			{
				light_position[0] = 0.0f;
				light_position[1] = 0.0f;
				light_position[2] = 1.0f;
				glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			}

			glTranslatef(row, col, -3.0f);
			row = row + 0.5f;

			glPolygonMode(GL_FRONT_AND_BACK, GLU_FILL);
			material[i][j].quadric = gluNewQuadric();
			gluSphere(material[i][j].quadric, 0.156f, 30.0f, 30.0f);

			glPopMatrix();

		}
		 row = -0.8f;
		 col = col - 0.333f;
	}
glXSwapBuffers(gpDisplay,gWindow);
}


void InitializeMaterials()
{
	/***emerald**************************************************************************************/
	material[0][0].ambient_material[0] = 0.0215f; material[0][0].ambient_material[1] = 0.1745f;
	material[0][0].ambient_material[2] = 0.0215f; material[0][0].ambient_material[3] = 1.0f;

	material[0][0].diffuse_material[0] = 0.07568f; material[0][0].diffuse_material[1] = 0.61424f;
	material[0][0].diffuse_material[2] = 0.07568f; material[0][0].diffuse_material[3] = 1.0f;

	material[0][0].specular_material[0] = 0.633f; material[0][0].specular_material[1] = 0.727811f;
	material[0][0].specular_material[2] = 0.633f; material[0][0].specular_material[3] = 1.0f;
	material[0][0].shininess = 0.6f * 128.0f;


	/***jade**************************************************************************************/
	material[1][0].ambient_material[0] = 0.135f; material[1][0].ambient_material[1] = 0.2225f;
	material[1][0].ambient_material[2] = 0.1575f; material[1][0].ambient_material[3] = 1.0f;

	material[1][0].diffuse_material[0] = 0.54f; material[1][0].diffuse_material[1] = 0.89f;
	material[1][0].diffuse_material[2] = 0.63f; material[1][0].diffuse_material[3] = 1.0f;

	material[1][0].specular_material[0] = 0.316228f; material[1][0].specular_material[1] = 0.7316228f;
	material[1][0].specular_material[2] = 0.316228f; material[1][0].specular_material[3] = 1.0f;
	material[1][0].shininess = 0.1f * 128.0f;


	/***obsidian**************************************************************************************/
	material[2][0].ambient_material[0] = 0.05375f; material[2][0].ambient_material[1] = 0.05f;
	material[2][0].ambient_material[2] = 0.06625f; material[2][0].ambient_material[3] = 1.0f;

	material[2][0].diffuse_material[0] = 0.18275f; material[2][0].diffuse_material[1] = 0.17f;
	material[2][0].diffuse_material[2] = 0.22525f; material[2][0].diffuse_material[3] = 1.0f;

	material[2][0].specular_material[0] = 0.332741f; material[2][0].specular_material[1] = 0.328634f;
	material[2][0].specular_material[2] = 0.346435f; material[2][0].specular_material[3] = 1.0f;
	material[2][0].shininess = 0.3f * 128.0f;


	/***pearl**************************************************************************************/
	material[3][0].ambient_material[0] = 0.25f; material[3][0].ambient_material[1] = 0.2075f;
	material[3][0].ambient_material[2] = 0.20725f; material[3][0].ambient_material[3] = 1.0f;

	material[3][0].diffuse_material[0] = 1.0f; material[3][0].diffuse_material[1] = 0.829f;
	material[3][0].diffuse_material[2] = 0.829f; material[3][0].diffuse_material[3] = 1.0f;

	material[3][0].specular_material[0] = 0.296648f; material[3][0].specular_material[1] = 0.296648f;
	material[3][0].specular_material[2] = 0.296648f; material[3][0].specular_material[3] = 1.0f;
	material[3][0].shininess = 0.088f * 128.0f;


	/***ruby**************************************************************************************/
	material[4][0].ambient_material[0] = 0.1745f; material[4][0].ambient_material[1] = 0.01175f;
	material[4][0].ambient_material[2] = 0.01175f; material[4][0].ambient_material[3] = 1.0f;

	material[4][0].diffuse_material[0] = 0.61424f; material[4][0].diffuse_material[1] = 0.04136f;
	material[4][0].diffuse_material[2] = 0.04136f; material[4][0].diffuse_material[3] = 1.0f;

	material[4][0].specular_material[0] = 0.727811f; material[4][0].specular_material[1] = 0.626959f;
	material[4][0].specular_material[2] = 0.626959f; material[4][0].specular_material[3] = 1.0f;
	material[4][0].shininess = 0.6f * 128.0f;


	/***turquoise**************************************************************************************/
	material[5][0].ambient_material[0] = 0.1f; material[5][0].ambient_material[1] = 0.18725f;
	material[5][0].ambient_material[2] = 0.1745f; material[5][0].ambient_material[3] = 1.0f;

	material[5][0].diffuse_material[0] = 0.396f; material[5][0].diffuse_material[1] = 0.74151f;
	material[5][0].diffuse_material[2] = 0.69102f; material[5][0].diffuse_material[3] = 1.0f;

	material[5][0].specular_material[0] = 0.297254f; material[5][0].specular_material[1] = 0.30829f;
	material[5][0].specular_material[2] = 0.306678f; material[5][0].specular_material[3] = 1.0f;
	material[5][0].shininess = 0.1f * 128.0f;

	/***brass**************************************************************************************/
	material[0][1].ambient_material[0] = 0.329412f; material[0][1].ambient_material[1] = 0.223529f;
	material[0][1].ambient_material[2] = 0.027451f; material[0][1].ambient_material[3] = 1.0f;

	material[0][1].diffuse_material[0] = 0.780392f; material[0][1].diffuse_material[1] = 0.568627f;
	material[0][1].diffuse_material[2] = 0.113725f; material[0][1].diffuse_material[3] = 1.0f;

	material[0][1].specular_material[0] = 0.992157f; material[0][1].specular_material[1] = 0.941176f;
	material[0][1].specular_material[2] = 0.807843f; material[0][1].specular_material[3] = 1.0f;
	material[0][1].shininess = 0.21794872f * 128.0f;


	/***bronze**************************************************************************************/
	material[1][1].ambient_material[0] = 0.2125f; material[1][1].ambient_material[1] = 0.1275f;
	material[1][1].ambient_material[2] = 0.054f; material[1][1].ambient_material[3] = 1.0f;

	material[1][1].diffuse_material[0] = 0.714f; material[1][1].diffuse_material[1] = 0.4284f;
	material[1][1].diffuse_material[2] = 0.4284f; material[1][1].diffuse_material[3] = 1.0f;

	material[1][1].specular_material[0] = 0.393548f; material[1][1].specular_material[1] = 0.271906f;
	material[1][1].specular_material[2] = 0.166721f; material[1][1].specular_material[3] = 1.0f;
	material[1][1].shininess = 0.2f * 128.0f;

	/***chrome**************************************************************************************/
	material[2][1].ambient_material[0] = 0.25f; material[2][1].ambient_material[1] = 0.25f;
	material[2][1].ambient_material[2] = 0.25f; material[2][1].ambient_material[3] = 1.0f;

	material[2][1].diffuse_material[0] = 0.4f; material[2][1].diffuse_material[1] = 0.4f;
	material[2][1].diffuse_material[2] = 0.4f; material[2][1].diffuse_material[3] = 1.0f;

	material[2][1].specular_material[0] = 0.774597f; material[2][1].specular_material[1] = 0.774597;
	material[2][1].specular_material[2] = 0.774597; material[2][1].specular_material[3] = 1.0f;
	material[2][1].shininess = 0.6f * 128.0f;

	/****copper*************************************************************************************/
	material[3][1].ambient_material[0] = 0.19125f; material[3][1].ambient_material[1] = 0.0735f;
	material[3][1].ambient_material[2] = 0.0225f; material[3][1].ambient_material[3] = 1.0f;

	material[3][1].diffuse_material[0] = 0.7038f; material[3][1].diffuse_material[1] = 0.27048f;
	material[3][1].diffuse_material[2] = 0.0828f; material[3][1].diffuse_material[3] = 1.0f;

	material[3][1].specular_material[0] = 0.256777f; material[3][1].specular_material[1] = 0.137622f;
	material[3][1].specular_material[2] = 0.086014f; material[3][1].specular_material[3] = 1.0f;
	material[3][1].shininess = 0.1f * 128.0f;

	/***gold**************************************************************************************/
	material[4][1].ambient_material[0] = 0.24725f; material[4][1].ambient_material[1] = 0.1995f;
	material[4][1].ambient_material[2] = 0.0745f; material[4][1].ambient_material[3] = 1.0f;

	material[4][1].diffuse_material[0] = 0.75164f; material[4][1].diffuse_material[1] = 0.60648f;
	material[4][1].diffuse_material[2] = 0.22648f; material[4][1].diffuse_material[3] = 1.0f;

	material[4][1].specular_material[0] = 0.628281f; material[4][1].specular_material[1] = 0.555802f;
	material[4][1].specular_material[2] = 0.366065f; material[4][1].specular_material[3] = 1.0f;
	material[4][1].shininess = 0.4f * 128.0f;


	/***silver**************************************************************************************/
	material[5][1].ambient_material[0] = 0.19225f; material[5][1].ambient_material[1] = 0.19225f;
	material[5][1].ambient_material[2] = 0.19225f; material[5][1].ambient_material[3] = 1.0f;

	material[5][1].diffuse_material[0] = 0.50754f; material[5][1].diffuse_material[1] = 0.50754f;
	material[5][1].diffuse_material[2] = 0.50754f; material[5][1].diffuse_material[3] = 1.0f;

	material[5][1].specular_material[0] = 0.508273f; material[5][1].specular_material[1] = 0.508273f;
	material[5][1].specular_material[2] = 0.508273f; material[5][1].specular_material[3] = 1.0f;
	material[5][1].shininess = 0.2f * 128.0f;


	/***black**************************************************************************************/
	material[0][2].ambient_material[0] = 0.0f; material[0][2].ambient_material[1] = 0.0f;
	material[0][2].ambient_material[2] = 0.0f; material[0][2].ambient_material[3] = 1.0f;

	material[0][2].diffuse_material[0] = 0.01f; material[0][2].diffuse_material[1] = 0.01f;
	material[0][2].diffuse_material[2] = 0.01f; material[0][2].diffuse_material[3] = 1.0f;

	material[0][2].specular_material[0] = 0.50f; material[0][2].specular_material[1] = 0.50f;
	material[0][2].specular_material[2] = 0.50f; material[0][2].specular_material[3] = 1.0f;
	material[0][2].shininess = 0.25f * 128.0f;

	/***cyan**************************************************************************************/
	material[1][2].ambient_material[0] = 0.0f; material[1][2].ambient_material[1] = 0.1f;
	material[1][2].ambient_material[2] = 0.06f; material[1][2].ambient_material[3] = 1.0f;

	material[1][2].diffuse_material[0] = 0.0f; material[1][2].diffuse_material[1] = 0.50980392f;
	material[1][2].diffuse_material[2] = 0.50980392f; material[1][2].diffuse_material[3] = 1.0f;

	material[1][2].specular_material[0] = 0.50196078; material[1][2].specular_material[1] = 0.50196078f;
	material[1][2].specular_material[2] = 0.50196078f; material[1][2].specular_material[3] = 1.0f;
	material[1][2].shininess = 0.25f * 128.0f;

	/***green**************************************************************************************/
	material[2][2].ambient_material[0] = 0.0f; material[2][2].ambient_material[1] = 0.0f;
	material[2][2].ambient_material[2] = 0.0f; material[2][2].ambient_material[3] = 1.0f;

	material[2][2].diffuse_material[0] = 0.1f; material[2][2].diffuse_material[1] = 0.35f;
	material[2][2].diffuse_material[2] = 0.1f; material[2][2].diffuse_material[3] = 1.0f;

	material[2][2].specular_material[0] = 0.45f; material[2][2].specular_material[1] = 0.55f;
	material[2][2].specular_material[2] = 0.45f; material[2][2].specular_material[3] = 1.0f;
	material[2][2].shininess = 0.25f * 128.0f;


	/***red**************************************************************************************/
	material[3][2].ambient_material[0] = 0.0f; material[3][2].ambient_material[1] = 0.0f;
	material[3][2].ambient_material[2] = 0.0f; material[3][2].ambient_material[3] = 1.0f;

	material[3][2].diffuse_material[0] = 0.5f; material[3][2].diffuse_material[1] = 0.0f;
	material[3][2].diffuse_material[2] = 0.0f; material[3][2].diffuse_material[3] = 1.0f;

	material[3][2].specular_material[0] = 0.7f; material[3][2].specular_material[1] = 0.6f;
	material[3][2].specular_material[2] = 0.6f; material[3][2].specular_material[3] = 1.0f;
	material[3][2].shininess = 0.25f * 128.0f;

	/***white**************************************************************************************/
	material[4][2].ambient_material[0] = 0.0f; material[4][2].ambient_material[1] = 0.0f;
	material[4][2].ambient_material[2] = 0.0f; material[4][2].ambient_material[3] = 1.0f;

	material[4][2].diffuse_material[0] = 0.55f; material[4][2].diffuse_material[1] = 0.55f;
	material[4][2].diffuse_material[2] = 0.55f; material[4][2].diffuse_material[3] = 1.0f;

	material[4][2].specular_material[0] = 0.70f; material[4][2].specular_material[1] = 0.70f;
	material[4][2].specular_material[2] = 0.70f; material[4][2].specular_material[3] = 1.0f;
	material[4][2].shininess = 0.25f * 128.0f;

	/***plastic**************************************************************************************/
	material[5][2].ambient_material[0] = 0.0f; material[5][2].ambient_material[1] = 0.0f;
	material[5][2].ambient_material[2] = 0.0f; material[5][2].ambient_material[3] = 1.0f;

	material[5][2].diffuse_material[0] = 0.5f; material[5][2].diffuse_material[1] = 0.5f;
	material[5][2].diffuse_material[2] = 0.0f; material[5][2].diffuse_material[3] = 1.0f;

	material[5][2].specular_material[0] = 0.60f; material[5][2].specular_material[1] = 0.60f;
	material[5][2].specular_material[2] = 0.50f; material[5][2].specular_material[3] = 1.0f;
	material[5][2].shininess = 0.25f * 128.0f;


	/***black**************************************************************************************/
	material[0][3].ambient_material[0] = 0.02f; material[0][3].ambient_material[1] = 0.02f;
	material[0][3].ambient_material[2] = 0.02f; material[0][3].ambient_material[3] = 1.0f;

	material[0][3].diffuse_material[0] = 0.01f; material[0][3].diffuse_material[1] = 0.01f;
	material[0][3].diffuse_material[2] = 0.01f; material[0][3].diffuse_material[3] = 1.0f;

	material[0][3].specular_material[0] = 0.50f; material[0][3].specular_material[1] = 0.50f;
	material[0][3].specular_material[2] = 0.50f; material[0][3].specular_material[3] = 1.0f;
	material[0][3].shininess = 0.07812 * 128.0f;


	/***cyan**************************************************************************************/
	material[1][3].ambient_material[0] = 0.0f; material[1][3].ambient_material[1] = 0.05f;
	material[1][3].ambient_material[2] = 0.05f; material[1][3].ambient_material[3] = 1.0f;

	material[1][3].diffuse_material[0] = 0.4f; material[1][3].diffuse_material[1] = 0.5f;
	material[1][3].diffuse_material[2] = 0.5f; material[1][3].diffuse_material[3] = 1.0f;

	material[1][3].specular_material[0] = 0.04f; material[1][3].specular_material[1] = 0.7f;
	material[1][3].specular_material[2] = 0.7f; material[1][3].specular_material[3] = 1.0f;
	material[1][3].shininess = 0.078125f * 128.0f;
	/********************************************************************************************/


	/***green**************************************************************************************/
	material[2][3].ambient_material[0] = 0.0f; material[2][3].ambient_material[1] = 0.05f;
	material[2][3].ambient_material[2] = 0.0f; material[2][3].ambient_material[3] = 1.0f;

	material[2][3].diffuse_material[0] = 0.4f; material[2][3].diffuse_material[1] = 0.5f;
	material[2][3].diffuse_material[2] = 0.4f; material[2][3].diffuse_material[3] = 1.0f;

	material[2][3].specular_material[0] = 0.04f; material[2][3].specular_material[1] = 0.7f;
	material[2][3].specular_material[2] = 0.04f; material[2][3].specular_material[3] = 1.0f;
	material[2][3].shininess = 0.078125f * 128.0f;

	/***red**************************************************************************************/
	material[3][3].ambient_material[0] = 0.05f; material[3][3].ambient_material[1] = 0.0f;
	material[3][3].ambient_material[2] = 0.0f; material[3][3].ambient_material[3] = 1.0f;

	material[3][3].diffuse_material[0] = 0.5f; material[3][3].diffuse_material[1] = 0.4f;
	material[3][3].diffuse_material[2] = 0.4f; material[3][3].diffuse_material[3] = 1.0f;

	material[3][3].specular_material[0] = 0.7f; material[3][3].specular_material[1] = 0.04f;
	material[3][3].specular_material[2] = 0.04f; material[3][3].specular_material[3] = 1.0f;
	material[3][3].shininess = 0.078125f * 128.0f;

	/***white**************************************************************************************/
	material[4][3].ambient_material[0] = 0.05f; material[4][3].ambient_material[1] = 0.05f;
	material[4][3].ambient_material[2] = 0.05f; material[4][3].ambient_material[3] = 1.0f;

	material[4][3].diffuse_material[0] = 0.5f; material[4][3].diffuse_material[1] = 0.5f;
	material[4][3].diffuse_material[2] = 0.5f; material[4][3].diffuse_material[3] = 1.0f;

	material[4][3].specular_material[0] = 0.7f; material[4][3].specular_material[1] = 0.7f;
	material[4][3].specular_material[2] = 0.7f; material[4][3].specular_material[3] = 1.0f;
	material[4][3].shininess = 0.078125f * 128.0f;

	/***yellow rubber**************************************************************************************/
	material[5][3].ambient_material[0] = 0.05f; material[5][3].ambient_material[1] = 0.05f;
	material[5][3].ambient_material[2] = 0.0f; material[5][3].ambient_material[3] = 1.0f;

	material[5][3].diffuse_material[0] = 0.5f; material[5][3].diffuse_material[1] = 0.5f;
	material[5][3].diffuse_material[2] = 0.4f; material[5][3].diffuse_material[3] = 1.0f;

	material[5][3].specular_material[0] = 0.7f; material[5][3].specular_material[1] = 0.7f;
	material[5][3].specular_material[2] = 0.04f; material[5][3].specular_material[3] = 1.0f;
	material[5][3].shininess = 0.078125f * 128.0f;
	/*******************************************************************************************/
}

