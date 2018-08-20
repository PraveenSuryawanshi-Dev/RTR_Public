#include <Windows.h>
#include<stdio.h>
#include <gl\glew.h>

//#include<gl/GL.h>
#include<Math.h>

#include "vmath.h"
#include"resource.h"


#pragma comment(lib,"glew32.lib")
#pragma comment(lib, "opengl32.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT  600

using namespace vmath;


enum
{
	VDG_ATTRIBUTE_POSITION = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};


bool gbIsGameDone = false;
bool gbIsFullScreen = false;
bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;

LONG glPreviousStyle;
WINDOWPLACEMENT  gwndplacement;
MONITORINFO mi;


FILE *gpLogFile = NULL;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gTexture_sampler_function;
GLuint gTexture_marble;

GLuint gVao_cube;
GLuint gVbo_cube;

mat4 gPerspectiveProjectionMatrix;

GLfloat gCubeAngle = 0.0f;

bool gbAnimate;
bool gbLight;


GLfloat lightAmbient[] = { 0.25f,0.25f,0.25f,0.25f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };


GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 128.0f;


GLuint model_matrix_uniform;
GLuint view_matrix_uniform;
GLuint projection_matrix_uniform;



GLuint La_uniform;
GLuint Ld_uniform;
GLuint Ls_uniform;
GLuint light_position_uniform;
GLuint L_KeyPressed_uniform;

GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

int LoadGLTexture(GLuint *texture, TCHAR imageResourceID[]);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void IntializeOpenGL(HWND, HDC*, HGLRC*);

	void Update(void);

	void Render(HDC hdc);

	void UnIntializeOpenGL(HWND, HDC*, HGLRC*);



	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szAppName[] = TEXT("Interleaved VCNT Linear Array using Sub buffer data");
	HDC hdc = NULL;
	HGLRC hglrc = NULL;

	/*Create Log File*/
	if (fopen_s(&gpLogFile, "OpenGL_Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting..."), TEXT("ERROR"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
	}
	else
	{
		fprintf(gpLogFile, "Log File Created Successfully \n");
	}


	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclassex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclassex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclassex.hInstance = hInstance;
	wndclassex.lpfnWndProc = WndProc;
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.lpszClassName = szAppName;
	wndclassex.lpszMenuName = NULL;
	wndclassex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	if (!RegisterClassEx(&wndclassex))
	{
		MessageBox(NULL, TEXT("ERROR IN REGISTERING CLASS"), TEXT("ERROR"), MB_ICONERROR);
		return 0;
	}



	HWND hWnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		szAppName,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
		);


	ShowWindow(hWnd, iCmdShow);

	SetForegroundWindow(hWnd);

	SetFocus(hWnd);

	IntializeOpenGL(hWnd, &hdc, &hglrc);


	while (gbIsGameDone == false)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				gbIsGameDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}
		else
		{
			if (gbIsActiveWindow == true)
			{
				if (gbIsEscapeKeyPressed == true)
					gbIsGameDone = true;

				if (gbAnimate == true)
					Update();

				Render(hdc);
			}
		}


	}

	UnIntializeOpenGL(hWnd, &hdc, &hglrc);

	return msg.wParam;
}

void Update()
{
	gCubeAngle = gCubeAngle + 0.3f;

	if (gCubeAngle >= 360.0f)
		gCubeAngle = gCubeAngle - 360.0f;

}
void IntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{
	void Resize(int, int);
	void LoadGLTextureFromProcedureTexture();
	void CreateWhiteTexture();
	void UnIntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;


	*hdc = GetDC(hwnd);

	int iIndexPixelFormatDescriptor = ChoosePixelFormat(*hdc, &pfd);

	if (iIndexPixelFormatDescriptor == 0)
	{
		MessageBox(hwnd, TEXT("ERROR TO CHOOSE PIXEL FORMAT DESCRIPTOR"), TEXT("ERROR"), MB_ICONERROR);
		return;
	}


	if (SetPixelFormat(*hdc, iIndexPixelFormatDescriptor, &pfd) == FALSE)
	{
		MessageBox(hwnd, TEXT("ERROR TO SET PIXEL FORMAT"), TEXT("ERROR"), MB_ICONERROR);
		ReleaseDC(hwnd, *hdc);

		*hdc = NULL;
		return;
	}

	*hglrc = wglCreateContext(*hdc);

	if (*hglrc == NULL)
	{
		MessageBox(hwnd, TEXT("ERROR TO SET PIXEL FORMAT"), TEXT("ERROR"), MB_ICONERROR);

		wglDeleteContext(*hglrc);
		ReleaseDC(hwnd, *hdc);

		*hglrc = NULL;
		*hdc = NULL;
		return;
	}

	if (wglMakeCurrent(*hdc, *hglrc) == FALSE)
	{
		MessageBox(hwnd, TEXT("ERROR TO wgl Make Current"), TEXT("ERROR"), MB_ICONERROR);

		wglDeleteContext(*hglrc);
		ReleaseDC(hwnd, *hdc);

		*hglrc = NULL;
		*hdc = NULL;
		return;

	}

	GLenum error = glewInit();

	if (error != GLEW_OK)
	{
		wglDeleteContext(*hglrc);
		hglrc = NULL;
		ReleaseDC(hwnd, *hdc);
		hdc = NULL;
		MessageBox(hwnd, TEXT("ERROR TO glewInit"), TEXT("ERROR"), MB_ICONERROR);
		return;
	}

	fprintf(gpLogFile, "Vertex Shader Compilation Log : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	//CREAT VERTEXT SHADER OBJECT
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//CREATE SOURCE CODE FOR VERTEX SHADER OBJECT

	/*

	*/
	const GLchar *vertexShaderSourcecode =
		"#version 430" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"in vec2 vTexture0_Coord;"\
		"in vec4 vColor;"\

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
		"out vec2 out_texture0_coord;"\
		"out vec4 vColor_out;"\

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
		"out_texture0_coord = vTexture0_Coord;"\
		"vColor_out = vColor;"\
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
				UnIntializeOpenGL(hwnd, hdc, hglrc);
				exit(0);
			}
		}
	}


	//Create FRAGMENT SHADER OBJECT
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//CREATE SOURCE CODE FOR FRAGMENT SHADER
	const GLchar *fragmentShaderSourceCode =
		"#version 430"\
		"\n" \
		"uniform sampler2D u_texture0_sampler;"\

		"in vec2 out_texture0_coord;"\
		"out vec4 FragColor;" \
		"flat in int u_lighting_enabled_out;" \

		"in mat4 u_model_matrix_out;" \
		"in mat4 u_view_matrix_out;" \
		"in vec4 vPosition_out;" \
		"in vec3 vNormal_out;"\
		"in vec4 vColor_out;"\
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
			"vec3 reflection_vector = reflect(-light_direction, transformed_normals);" \
			"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
			"vec3 specular = u_Ls_out * u_Ks_out * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess_out);" \
			"phong_ads_color=ambient + diffuse + specular;" \

			/*"FragColor = vec4(phong_ads_color, 1.0);" \*/
			"FragColor = texture(u_texture0_sampler,out_texture0_coord) * vColor_out * vec4(phong_ads_color,1.0);"\
		"}" \
		"else" \
		"{" \
			/*"FragColor = vec4(1.0,1.0,1.0,1.0);"\*/
			"FragColor = texture(u_texture0_sampler,out_texture0_coord) * vColor_out * vec4(1.0,1.0,1.0,1.0);"\
		"}" \

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
				//fclose(gpLogFile);
				UnIntializeOpenGL(hwnd, hdc, hglrc);
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
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH COLOR SHADER ATTRIBUTE
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_COLOR, "vColor");
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH TEXTURE SHADER ATTRIBUTE
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_NORMAL, "vNormal");
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH TEXTURE SHADER ATTRIBUTE
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
	

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
				UnIntializeOpenGL(hwnd, hdc, hglrc);
				exit(0);
			}

		}
	}


	//GET MVP UNIFORM LOCATION
	
	gTexture_sampler_function = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");

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
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
	//

	//VERTICES , COLORS ,SHADERS ATTRIBS ,VBO ,VAO INITIALIZATION

	
	

	/*const GLfloat cubeVCNT[] =
	{
		//VERTICES
		//top face			
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f ,1.0f,
		1.0f, 1.0f ,1.0f,

		//bottom FACE		
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		//front Face		
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		//back Face			
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

		//left Face			
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,

		//right Face		
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		

		//COLOR
		//top face blue		
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 01.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		//bottom FACE cyan	
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		//front Face yellow	
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		//back Face magenta	
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,

		//left Face red		
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		//right Face green	
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		//NORMALS
		//top face				
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		//bottom FACE			
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		//front Face			
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		//back Face				
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		//left Face				
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		//TEXTURES
		//top Texcoords
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		//bottom Texcoords
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		//front Texcoords
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		//back Texcoords
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		//left Texcoords
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		//right Color
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};*/
	

//VERTICES , COLORS ,SHADERS ATTRIBS ,VBO ,VAO INITIALIZATION

	const GLfloat cubeVCNT[] =
	{
		/*VERTICES*/			/*COLOR*/				/*NORMALS*/				/*TEXTURES*/
		//top face				//top face blue			//top face				//top Texcoords
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		-1.0f, 1.0f ,1.0f,		0.0f, 01.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		1.0f, 1.0f ,1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,

		//bottom FACE			//bottom FACE cyan		//bottom FACE			//bottom Texcoords
		1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 1.0f, 1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,

		//front Face			//front Face yellow		//front Face			//front Texcoords
		1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		1.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,		1.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,

		//back Face				//back Face magenta		//back Face				//back Texcoords
		1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 1.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 1.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,	
		-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 1.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 1.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,

		//left Face				//left Face red			//left Face				//left Texcoords
		-1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,	
		-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,	

		//right Face			//right Face green		//right Face			//right Color
		1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,	
		1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f
		/**/		
	};
	

	/*cube*/
	glGenVertexArrays(1, &gVao_cube);
	glBindVertexArray(gVao_cube);

	/*Push Vertices, colors , normals, textures at once*/
	glGenBuffers(1, &gVbo_cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVCNT), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVCNT), cubeVCNT);
	//VERTEX
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	//COLOR
	glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);

	//NORMAL
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	//TEX CORDS
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(9 * sizeof(float)));
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	glBindVertexArray(0);
	/**/



	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE); /*Turning oFF cull , as we are doing animation , and so need to paint the back of object when rotating*/


	gPerspectiveProjectionMatrix = mat4::identity();

	gbAnimate = false;
	gbLight = false;


	//LOAD TEXTURES 
	LoadGLTexture(&gTexture_marble, MAKEINTRESOURCE(IDB_BITMAP_MARBLE));
	//

	Resize(WIN_WIDTH, WIN_HEIGHT);
}


void Render(HDC hdc)
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
		glUniform1i(L_KeyPressed_uniform,1);

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
		glUniform1i(L_KeyPressed_uniform, 0);
	}


	mat4  modelmatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 rotateMatrix = mat4::identity();

	/*DRAWING RECTANGLE*/

	modelmatrix = translate(0.0f, 0.0f, -5.0f);
	rotateMatrix = vmath::rotate(gCubeAngle, gCubeAngle, gCubeAngle); // arbitary axis
	modelmatrix = modelmatrix * rotateMatrix;
	
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelmatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);


	//*BINDE With Rectangle Cordinates*//
	glBindVertexArray(gVao_cube);

	//Bind with  Texture
	glActiveTexture(GL_TEXTURE0); // corresponds to VDG_ATTRIBUTE_TEXTURE0
	glBindTexture(GL_TEXTURE_2D, gTexture_marble);
	glUniform1i(gTexture_sampler_function, 0);
	//

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);


	glBindVertexArray(0);

	/*END OF DRAWING TRIANGLE*/



	//
	//STOP USING SHADER OBJECT
	glUseProgram(0);


	SwapBuffers(hdc);
}
void UnIntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{

	wglDeleteContext(*hglrc);
	*hglrc = NULL;

	ReleaseDC(hwnd, *hdc);
	*hdc = NULL;

	DestroyWindow(hwnd);
	hwnd = NULL;


	//DESTROY VAO
	if (gVao_cube)
	{
		glDeleteVertexArrays(1, &gVao_cube);
		gVao_cube = 0;
	}


	//DESTROY VBO
	if (gVbo_cube)
	{
		glDeleteVertexArrays(1, &gVbo_cube);
		gVbo_cube = 0;
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


	if (gpLogFile)
	{
		fprintf(gpLogFile, "Log File Successfully Closed \n");
		fclose(gpLogFile);
		gpLogFile = NULL;
	}
	return;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	void Resize(int, int);

	void ToggleScreen(HWND);

	void ToggleScreen(HWND);

	//variable declaration

	static bool bIsAKeyPressed = false;
	static bool bIsLKeyPressed = false;

	switch (message)
	{

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case   WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbIsActiveWindow = true;
		else
			gbIsActiveWindow = false;
		break;

	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;


	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:
			gbIsEscapeKeyPressed = true;
			break;

		case 0x46:
			ToggleScreen(hWnd);
			break;

		case 0x41://A or a
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
			break;

		case 0x4C: //for L or l
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

		default: break;
		}


	default: break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);

}
void Resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gPerspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}
void ToggleScreen(HWND hwnd)
{
	if (gbIsFullScreen)
	{
		SetWindowLong(hwnd, GWL_STYLE, glPreviousStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &gwndplacement);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbIsFullScreen = false;
		return;
	}

	glPreviousStyle = GetWindowLong(hwnd, GWL_STYLE);

	if (glPreviousStyle & WS_OVERLAPPEDWINDOW)
	{
		mi = { sizeof(MONITORINFO) };

		if (GetWindowPlacement(hwnd, &gwndplacement) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITORINFOF_PRIMARY), &mi))
		{
			SetWindowLong(hwnd, GWL_STYLE, glPreviousStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		}

	}
	ShowCursor(FALSE);
	gbIsFullScreen = true;

}
int LoadGLTexture(GLuint *texture, TCHAR imageResourceID[])
{
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = FALSE;

	glGenTextures(1, texture);
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitmap)
	{
		iStatus = TRUE;

		GetObject(hBitmap, sizeof(bmp), &bmp);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0/*LOD*/, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0/*border width*/, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);

	}

	return iStatus;
}