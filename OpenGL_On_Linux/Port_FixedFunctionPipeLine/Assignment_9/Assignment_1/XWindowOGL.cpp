#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<vector>
#include<string.h>


#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include<SOIL/SOIL.h>


#define TRUE 1
#define FALSE 0

#define BUFFER_SIZE 256
#define S_EQUAL 0

#define NR_POINT_COORDS 3
#define NR_TEXTURE_COORDS 2
#define NR_NOTMAL_COORDS 3
#define NR_FACE_TOKENS 3


#define MOKEY_X_TRANSLATE 0.0f
#define MONKEYHEAD_Y_TRANSLATE -0.0f
#define MOKEYHEAD_Z_TRANSLATE -5.0f

#define MOKEY_X_SCALE_FACTOR 1.5f
#define MONKEYHEAD_Y_SCALE_FACTOR 1.5f
#define MOKEYHEAD_Z_SCALE_FACTOR 1.5f

#define START_ANGLE_POS 0.0f
#define END_ANGLE_POS 360.f
#define MOKEYHEAD_ANGLE_INCREMENT 1.0f


using namespace std;


std::vector< std::vector< float > > g_vertices;

std::vector< std::vector< float > > g_texture;

std::vector< std::vector< float > > g_normals;

std::vector< std::vector< int > > g_face_tri, g_face_texture, g_face_normals;

FILE *g_fp_meshFile = NULL;
FILE *g_fp_logfile = NULL;

char line[BUFFER_SIZE];

bool gbFullScreen = false;

GLfloat g_rotate;

Display *gpDisplay =NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 800;
int giWindowHeight = 600;

GLXContext gGlXContext;

bool IsLkeyPressed = false;


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

void Update(void)
{

	g_rotate = g_rotate + MOKEYHEAD_ANGLE_INCREMENT;

	if (g_rotate >= END_ANGLE_POS)
		g_rotate = START_ANGLE_POS;
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

XStoreName(gpDisplay,gWindow,"Monkey Head (Wire Frame)");

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
void LoadMeshData(void);
void UnInitialize(void);
void Resize(int , int);
/**/
	g_fp_logfile = fopen("MONKEYHEADLOADER.LOG", "w");

	if (!g_fp_logfile)
		UnInitialize();


	gGlXContext = glXCreateContext(gpDisplay,gpXVisualInfo, NULL, GL_TRUE);

	glXMakeCurrent(gpDisplay, gWindow, gGlXContext);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	LoadMeshData();

	Resize(giWindowWidth, giWindowHeight);

}
void LoadMeshData(void)
{
	void UnInitialize(void);

	g_fp_meshFile = fopen("MonkeyHead.OBJ", "r");

	if (!g_fp_meshFile)
		UnInitialize();

        char sep_space[] ={" "};

	char sep_fslash[] ={"/"};

	char *first_token = NULL;
	char *token = NULL;

	char *face_tokens[NR_FACE_TOKENS];

	int nr_tokens;

	char *token_vertex_index = NULL;

	char *token_texture_index = NULL;

	char *token_normal_index = NULL;

	
	while (fgets(line,BUFFER_SIZE,g_fp_meshFile)!= NULL)
	{
		first_token = strtok(line, sep_space);

		if (strcmp(first_token, "v") == S_EQUAL)
		{
			std::vector<float> vec_point_coords(NR_POINT_COORDS);

			for (int i = 0; i != NR_POINT_COORDS; i++)
				vec_point_coords[i] = atof(strtok(NULL, sep_space));
			
			g_vertices.push_back(vec_point_coords);
			

		}
		else if (strcmp(first_token, "vt") == S_EQUAL)
		{
			std::vector<float> vec_texture_coords(NR_TEXTURE_COORDS);

			for (int i = 0; i != NR_TEXTURE_COORDS; i++)
				vec_texture_coords[i] = atof(strtok(NULL, sep_space));
			g_texture.push_back(vec_texture_coords);
			
		}
		else if(strcmp(first_token,"vn") == S_EQUAL)
		{
			std::vector<float> vec_normal_coord(NR_NOTMAL_COORDS);
			
			for (int i = 0; i != NR_NOTMAL_COORDS; i++)
				vec_normal_coord[i] = atof(strtok(NULL, sep_space));
			g_normals.push_back(vec_normal_coord);
			
		}
		else if(strcmp(first_token,"f") == S_EQUAL)
		{
			std::vector<int> triangle_vertex_indices(3), texture_vertex_indices(3), normal_vertex_indices(3);

			memset((void*)face_tokens, 0, NR_FACE_TOKENS);
			nr_tokens = 0;

			while (token = strtok(NULL,sep_space))
			{
				if (strlen(token) < 3)
					break;

				face_tokens[nr_tokens] = token;
				nr_tokens++;
			}

			for (int i = 0; i != NR_FACE_TOKENS; ++i)
			{

				token_vertex_index = strtok(face_tokens[i], sep_fslash);
				token_texture_index = strtok(NULL, sep_fslash);
				token_normal_index = strtok(NULL, sep_fslash);
				triangle_vertex_indices[i] = atoi(token_vertex_index);
				texture_vertex_indices[i] = atoi(token_texture_index);
				normal_vertex_indices[i] = atoi(token_normal_index);
			}

			g_face_tri.push_back(triangle_vertex_indices);
			g_face_texture.push_back(texture_vertex_indices);
			g_face_normals.push_back(normal_vertex_indices);
		}

		memset((void*)line, (int)'\0', BUFFER_SIZE);
	}

	fclose(g_fp_meshFile);
	g_fp_meshFile = NULL;

fprintf(g_fp_logfile,"g_vertices:%lu g_texture:%lu g_normals:%lu g_face_tri:%lu \n", g_vertices.size(), g_texture.size(),g_normals.size(), g_face_tri.size());	

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

if(g_fp_logfile)
{
	fclose(g_fp_logfile);
	g_fp_logfile = NULL;
}

}


void Render()
{ 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(MOKEY_X_TRANSLATE, MONKEYHEAD_Y_TRANSLATE, MOKEYHEAD_Z_TRANSLATE);
	glRotatef(g_rotate, 0.0f, 1.0f, 0.0f);
	glScalef(MOKEY_X_SCALE_FACTOR, MONKEYHEAD_Y_SCALE_FACTOR, MOKEYHEAD_Z_SCALE_FACTOR);

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i != g_face_tri.size(); ++i)
	{
		glBegin(GL_TRIANGLES);
		for (int j = 0; j != g_face_tri[i].size(); j++)
		{
			int vi = g_face_tri[i][j] - 1;
			glVertex3f(g_vertices[vi][0], g_vertices[vi][1], g_vertices[vi][2]);
		}
		glEnd();
	}
	
	glXSwapBuffers(gpDisplay,gWindow);

}

