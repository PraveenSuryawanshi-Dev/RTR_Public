#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>

#include "vmath.h"
#include "Sphere.h"

using namespace vmath;

using namespace std;


bool gbFullScreen = false;

Display *gpDisplay =NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 800;
int giWindowHeight = 600;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB=NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGlXContext;


enum
{
	VDG_ATTRIBUTE_POSITION = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};


FILE *gpLogFile = NULL;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;


float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint gNumElements;
GLuint gNumVertices;


GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;


GLuint model_matrix_uniform;
GLuint view_matrix_uniform;
GLuint projection_matrix_uniform;
mat4 gPerspectiveProjectionMatrix;



GLuint La_uniform;
GLuint Ld_uniform;
GLuint Ls_uniform;
GLuint light_position_uniform;
GLuint L_KeyPressed_uniform;

GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;





GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };
bool gbLight;


GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;


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
/*CREATE LOG FILE*/
gpLogFile = fopen("OPENGL_LOG.txt","w");
if(gpLogFile == NULL)
{

	printf("ERROR : Failed to Create File\n");
	return 0;
}


fprintf(gpLogFile,"Log File Creted Successfully\n");

/**/

//variable declaration

static bool bIsAKeyPressed = false;
static bool bIsLKeyPressed = false;

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

				/*case XK_A:
				case XK_a://A or a
					if (bIsAKeyPressed == false)
					{
						gbAnimate = true;
						bIsAKeyPressed = true;
					}
					else
					{
						gbAnimate = false;
						bIsAKeyPressed = false;
					}
					break;*/

				case XK_L:
				case XK_l: //for L or l
					if (bIsLKeyPressed == false)
					{
						gbLight = true;
						bIsLKeyPressed = true;
					}
					else
					{
						gbLight = false;
						bIsLKeyPressed = false;
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

void Update()
{
}

void CreateWindow(void)
{
/*FUNCTION PROTO TYPES*/
void UnInitialize(void);
/**/


XSetWindowAttributes winAttribs;
GLXFBConfig *pGLXFBConfigs=NULL;
GLXFBConfig bestGLXFBConfig;

XVisualInfo *pTempXVisualInfo=NULL;
int iNumFBConfigs=0;

int i;

int defaultScreen;
int defaultDepth;
int styleMask;


static int frameBufferAttributes[]=
{		
		GLX_X_RENDERABLE,True,
		GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE,GLX_TRUE_COLOR,
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE,8,
		GLX_BLUE_SIZE,8,
		GLX_ALPHA_SIZE,8,
		GLX_DEPTH_SIZE,24,
		GLX_STENCIL_SIZE,8,
		GLX_DOUBLEBUFFER,True,
		GLX_SAMPLE_BUFFERS,1,
		GLX_SAMPLES,4,
		None
};

gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
		printf("ERROR : Unable To Obtain X Display.\n");
		UnInitialize();
		exit(1);
	}
	
	// get a new framebuffer config that meets our attrib requirements
	pGLXFBConfigs=glXChooseFBConfig(gpDisplay,DefaultScreen(gpDisplay),frameBufferAttributes,&iNumFBConfigs);
	if(pGLXFBConfigs==NULL)
	{
		printf( "Failed To Get Valid Framebuffer Config. Exitting Now ...\n");
		UnInitialize();
		exit(1);
	}
	printf("%d Matching FB Configs Found.\n",iNumFBConfigs);
	
	// pick that FB config/visual with the most samples per pixel
	int bestFramebufferconfig=-1,worstFramebufferConfig=-1,bestNumberOfSamples=-1,worstNumberOfSamples=999;
	for(i=0;i<iNumFBConfigs;i++)
	{
		pTempXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,pGLXFBConfigs[i]);
		if(pTempXVisualInfo)
		{
			int sampleBuffer,samples;
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLE_BUFFERS,&sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLES,&samples);
			printf("Matching Framebuffer Config=%d : Visual ID=0x%lu : SAMPLE_BUFFERS=%d : SAMPLES=%d\n",i,pTempXVisualInfo->visualid,sampleBuffer,samples);
			if(bestFramebufferconfig < 0 || sampleBuffer && samples > bestNumberOfSamples)
			{
				bestFramebufferconfig=i;
				bestNumberOfSamples=samples;
			}
			if( worstFramebufferConfig < 0 || !sampleBuffer || samples < worstNumberOfSamples)
			{
				worstFramebufferConfig=i;
			    worstNumberOfSamples=samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
	bestGLXFBConfig = pGLXFBConfigs[bestFramebufferconfig];
	// set global GLXFBConfig
	gGLXFBConfig=bestGLXFBConfig;
	
	// be sure to free FBConfig list allocated by glXChooseFBConfig()
	XFree(pGLXFBConfigs);
	
	gpXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,bestGLXFBConfig);
	printf("Chosen Visual ID=0x%lu\n",gpXVisualInfo->visualid );
	

	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,
					    RootWindow(gpDisplay,gpXVisualInfo->screen),
					    gpXVisualInfo->visual,
					    AllocNone);
										
	winAttribs.event_mask=StructureNotifyMask | KeyPressMask | ButtonPressMask |
						  ExposureMask | VisibilityChangeMask | PointerMotionMask;
	
	styleMask=CWBorderPixel | CWEventMask | CWColormap;
	gColormap=winAttribs.colormap;										           
	
	gWindow=XCreateWindow(gpDisplay,
						  RootWindow(gpDisplay,gpXVisualInfo->screen),
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
		printf("Failure In Window Creation.\n");
		UnInitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay,gWindow,"Single Light with per Fragment using Blinn Modification Shading model on a Steady sphere");
	
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_WINDOW_DELETE",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
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
void UnInitialize(void);
/**/
	
	// create a new GL context 3.1 for rendering
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");
	
	GLint attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB,3,
		GLX_CONTEXT_MINOR_VERSION_ARB,1,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0 };
		
	gGlXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);

	if(!gGlXContext) 
	{
		GLint attribs[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB,1,
			GLX_CONTEXT_MINOR_VERSION_ARB,0,
			0 };
		printf("Failed To Create GLX 4.5 context. Hence Using Old-Style GLX Context\n");

		gGlXContext = glXCreateContextAttribsARB(gpDisplay,gGLXFBConfig,0,True,attribs);
	}
	else 
	{
		printf("OpenGL Context 3.1 Is Created.\n");
	}
	if(!glXIsDirect(gpDisplay,gGlXContext))
	{
		printf("Indirect GLX Rendering Context Obtained\n");
	}
	else
	{
		printf("Direct GLX Rendering Context Obtained\n" );
	}
	
	glXMakeCurrent(gpDisplay,gWindow,gGlXContext);

	GLenum error = glewInit();

	if(error != GLEW_OK)
	{
		UnInitialize();
		return ;	
	}

	fprintf(gpLogFile, "Vertex Shader Compilation Log : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	//CREAT VERTEXT SHADER OBJECT
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//CREATE SOURCE CODE FOR VERTEX SHADER OBJECT

	/*

	*/
	const GLchar *vertexShaderSourcecode =
		"#version 330" \
		"\n" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
	
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_lighting_enabled;" \
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Ls;" \
		"uniform vec4 u_light_position;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		
		"flat out int u_lighting_enabled_out;" \
		"out mat4 u_model_matrix_out;" \
		"out mat4 u_view_matrix_out;" \
		"out vec4 vPosition_out;" \
		"out vec3 vNormal_out;" \
		"out vec4 u_light_position_out;" \
		"out vec3 u_La_out;" \
		"out vec3 u_Ka_out;" \
		"out vec3 u_Ld_out;" \
		"out vec3 u_Kd_out;" \
		"out vec3 u_Ls_out;" \
		"out vec3 u_Ks_out;" \
		"out float u_material_shininess_out;" \
		"void main(void)" \
		"{" \
			"u_lighting_enabled_out = u_lighting_enabled;"\
			"u_model_matrix_out = u_model_matrix;"\
			"u_view_matrix_out = u_view_matrix;"\
			"vPosition_out = vPosition;"\
			"vNormal_out = vNormal;" \
			"u_light_position_out = u_light_position;"\
			"u_La_out = u_La;"\
			"u_Ka_out = u_Ka;"\
			"u_Ld_out = u_Ld;"\
			"u_Kd_out =u_Kd;"\
			"u_Ls_out =u_Ls;"\
			"u_Ks_out =u_Ks;"\
			"u_material_shininess_out =u_material_shininess;"\

			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";

	//BIND vertexShaderSourcecode CODE TO gVertexShaderObject
	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourcecode, NULL);

	//COMPILE VERTEX SHADER
	glCompileShader(gVertexShaderObject);

	/****************/


	GLint infoLogLength = 0;
	GLint iShaderCompilationStatus = 0;
	char *szInfoLog = NULL;
	GLsizei written = 0;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompilationStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(infoLogLength);

			if (szInfoLog != NULL)
			{

				glGetShaderInfoLog(gVertexShaderObject, infoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}
	}


	//Create FRAGMENT SHADER OBJECT
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//CREATE SOURCE CODE FOR FRAGMENT SHADER
	const GLchar *fragmentShaderSourceCode =
		"#version 330"\
		"\n"\
		"out vec4 FragColor;" \
		"flat in int u_lighting_enabled_out;" \
		
		"in mat4 u_model_matrix_out;" \
		"in mat4 u_view_matrix_out;" \
		"in vec4 vPosition_out;" \
		"in vec3 vNormal_out;" \
		"in vec4 u_light_position_out;" \
		"in vec3 u_La_out;" \
		"in vec3 u_Ka_out;" \
		"in vec3 u_Ld_out;" \
		"in vec3 u_Kd_out;" \
		"in vec3 u_Ls_out;" \
		"in vec3 u_Ks_out;" \
		"in float u_material_shininess_out;" \

		"void main(void)" \
		"{" \
			"vec3 phong_ads_color;" \
			"if(u_lighting_enabled_out == 1)" \
			"{" \
				"vec4 eye_coordinates=u_view_matrix_out * u_model_matrix_out * vPosition_out;" \
				"vec3 transformed_normals=normalize(mat3(u_view_matrix_out * u_model_matrix_out) * vNormal_out);" \
				"vec3 light_direction = normalize(vec3(u_light_position_out) - eye_coordinates.xyz);" \
				"float tn_dot_ld = max(dot(transformed_normals, light_direction),0.0);" \
				"vec3 ambient = u_La_out * u_Ka_out;" \
				"vec3 diffuse = u_Ld_out * u_Kd_out * tn_dot_ld;" \
				/*"vec3 reflection_vector = reflect(-light_direction, transformed_normals);" */
				"vec3 viewer_vector = normalize(-eye_coordinates.xyz);"\
				"vec3 H= normalize(u_Ls_out + viewer_vector);"\
				"vec3 specular = u_Ls_out * u_Ks_out * pow(max(dot(H, vNormal_out), 0.0), u_material_shininess_out);" \
				/*"vec3 specular = u_Ls_out * u_Ks_out * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess_out);" PHONG LIGHTING \*/
				"phong_ads_color=ambient + diffuse + specular;" \
			"}" \
			"else" \
			"{" \
				"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
			"}" \

			"FragColor = vec4(phong_ads_color, 1.0);" \
		"}";

	//BIND fragmentShaderSourceCode to gFragmentShaderObject
	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	//COMPILE FRAGMENT SHADER
	glCompileShader(gFragmentShaderObject);

	infoLogLength = 0;
	iShaderCompilationStatus = 0;
	szInfoLog = NULL;
	written = 0;

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompilationStatus);
	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(sizeof(infoLogLength));

			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(gFragmentShaderObject, infoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}
	}

	// *** SHADER PROGRAM ****//
	//CREATE SHADER PROGRAM OBJECT
	gShaderProgramObject = glCreateProgram();

	//ATTACH VERTEX SHADER PROGRAM TO SHADER PROGRAM
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	//ATTACH FRAGMENT SHADER PROGRAM TO SHADER PROGRAM
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITH VERTEX SHADER POSITION ATTRIBUTE
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_POSITION, "vPosition");
	
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH TEXTURE SHADER ATTRIBUTE
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_NORMAL, "vNormal");


	//LINK THE SHADER
	glLinkProgram(gShaderProgramObject);

	infoLogLength = 0;
	iShaderCompilationStatus = 0;
	szInfoLog = NULL;
	written = 0;

	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(sizeof(infoLogLength));
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(gShaderProgramObject, infoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Shader Program Link Log : %s \n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}

		}
	}


	//GET MVP UNIFORM LOCATION
	model_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	view_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	// 'L' HANDLE KEY TO TOGGLE LIGHT.
	L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject, "u_lighting_enabled");

	//AMBIENT COLOR TOKEN
	La_uniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	
	// DIFUSE COLOR TOKEN
	Ld_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	
	// SPECULAR COLOR TOKEN
	Ls_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");
	
	// LIGHT POSITION TOKEN
	light_position_uniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");;

	// AMBIENT REFLECTIVE TOKEN
	Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	
	// DIFUUSE REFLECTIVE TOKEN
	Kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	
	// SPECULAR REFLECTIVE TOKEN
	Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	
	//SHINYNESS COLOR TOKEN
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");;

	/*VERTICES ,COLOR ,NORMAL  INITIALIZATION*/

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);

	//GET NUMBER OF VERTICSE FROM SPHERE.
	gNumVertices = getNumberOfSphereVertices();
	//GET NUMBER OF SPHERE ELEMENTS
	gNumElements = getNumberOfSphereElements();


	//VERTICES , COLORS ,SHADERS ATTRIBS ,VBO ,VAO INITIALIZATION
	
	/*SPHERE*/
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	/*POSITIONS*/
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/

	/*NORMALS*/
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);

	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	glBindVertexArray(0);
	/**/


	/*ELEMENT VBO*/
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	/**/




	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE); /*Turning oFF cull , as we are doing animation , and so need to paint the back of object when rotating*/

	gPerspectiveProjectionMatrix = mat4::identity();
	//bAnimate = false;
	gbLight = false;


	Resize(giWindowWidth, giWindowHeight);
}


void Resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gPerspectiveProjectionMatrix = vmath::perspective(45.05f,(GLfloat)width /(GLfloat)height, 0.1f, 100.0f);
}



void UnInitialize()
{

        GLXContext currentContext=glXGetCurrentContext();

	if(currentContext!=NULL && currentContext==gGlXContext)
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
		gpXVisualInfo=NULL;
	}
	
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
	
	
	//DESTROY VAO
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}


	//DESTROY VBO
	if (gVbo_sphere_position)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}
	if (gVbo_sphere_normal)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	// DESTROY ELEMENT VBO
	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}

	//DETACH THE FRAGMENT SHADER OBJECT
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	//DETACH THE VERTEX SHADER OBJECT
	glDetachShader(gShaderProgramObject, gVertexShaderObject);


	//DELETE VERTEX SHADER OBJECT
	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;

	//DELETE FRAGMENT SHADER OBJECT
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	//DELETE SHADER PROGRAM OBJECT
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	//UNLINK THE PROGRAM
	glUseProgram(0);


	if(gpLogFile != NULL)
	{
		fprintf(gpLogFile,"File Closed Successfully \n");
		fclose(gpLogFile);
		gpLogFile = NULL;
	}

}



void Render()
{
	/*RULE TO ANIMATE OBJECTS
	1. Translate.
	2. Rotate.
	3. Scale.
	*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//START USING SHADER OBJECT	
	glUseProgram(gShaderProgramObject);
	//DRAWING
	//


	if (gbLight == true)
	{
		// set 'u_lighting_enabled' uniform
		glUniform1i(L_KeyPressed_uniform, 1);

		// setting light's properties
		glUniform3fv(La_uniform, 1, lightAmbient);
		glUniform3fv(Ld_uniform, 1, lightDiffuse);
		glUniform3fv(Ls_uniform, 1, lightSpecular);
		glUniform4fv(light_position_uniform, 1, lightPosition);

		// setting material's properties
		glUniform3fv(Ka_uniform, 1, material_ambient);
		glUniform3fv(Kd_uniform, 1, material_diffuse);
		glUniform3fv(Ks_uniform, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shininess);
	}
	else
	{
		// set 'u_lighting_enabled' uniform
		glUniform1i(L_KeyPressed_uniform, 0);
	}


	/*DRAWING SPHERE*/
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	modelMatrix = translate(0.0f, 0.0f, -2.0f);

	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	// BIND VAO
	glBindVertexArray(gVao_sphere);

	//START DRAWING
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// UNBING VAO
	glBindVertexArray(0);

	//STOP USING SHADER
	glUseProgram(0);

	glXSwapBuffers(gpDisplay,gWindow);
}
