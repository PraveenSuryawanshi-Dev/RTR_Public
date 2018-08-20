#include <Windows.h>
#include<stdio.h>
#include <gl\glew.h>

#include<gl/GL.h>
#include<Math.h>

#include "vmath.h"
#include "Sphere.h"


#pragma comment(lib,"glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"Sphere.lib")

int WIN_WIDTH = 800;
int WIN_HEIGHT = 600;


int w = 0, h = 0;

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



GLfloat sphere_angle = 0.0f;

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
GLuint light_rotation_uniform;

GLuint L_KeyPressed_uniform;
		
GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

GLboolean gIsXKeyPressed = GL_FALSE;
GLboolean gIsYKeyPressed = GL_FALSE;
GLboolean gIsZKeyPressed = GL_FALSE;

typedef struct material
{
	GLfloat ambient_material[4];
	GLfloat diffuse_material[4];
	GLfloat specular_material[4];
	GLfloat shininess;

}MATERIAL;

MATERIAL  material[6][4];


GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 0.0f,0.0f,0.0f,0.0f };

bool gbLight;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void IntializeOpenGL(HWND, HDC*, HGLRC*);

	void Update(void);

	void Render(HDC hdc);

	void UnIntializeOpenGL(HWND, HDC*, HGLRC*);



	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szAppName[] = TEXT("Material Demo 24 Sphere");
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

	RECT rect;
	GetClientRect(hWnd, &rect);
	
	WIN_WIDTH = rect.right - rect.left;
	WIN_HEIGHT = rect.bottom - rect.top;

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
	if (sphere_angle >= 720.0f)
		sphere_angle = 360.0f;
	else
		sphere_angle = sphere_angle + 0.06f;
}
void IntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{
	void InitializeMaterials();
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
	/*const GLchar *vertexShaderSourcecode =
		"#version 430" \
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
		"}";*/

		const GLchar *vertexShaderSourcecode =
			"#version 430" \
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
			"uniform mat4 u_light_rotation;"\

			"uniform vec3 u_Ka;" \
			"uniform vec3 u_Kd;" \
			"uniform vec3 u_Ks;" \
			"uniform float u_material_shininess;" \
			"out vec3 phong_ads_color;" \

			"void main(void)" \
			"{" \
				"if(u_lighting_enabled == 1)" \
				"{" \
					"vec4 temp= u_light_position*u_light_rotation;"\

					"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
					"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
					"vec3 light_direction = normalize(vec3(temp) - eye_coordinates.xyz);" \
					"float tn_dot_ld = max(dot(transformed_normals, light_direction),0.0);" \
					"vec3 ambient = u_La * u_Ka;" \
					"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
					"vec3 reflection_vector = reflect(-light_direction, transformed_normals);" \
					"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
					"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" \
					"phong_ads_color=ambient + diffuse + specular;" \
				"}" \
				"else" \
				"{" \
					"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
				"}" \

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
				UnIntializeOpenGL(hwnd, hdc, hglrc);
				exit(0);
			}
		}
	}


	//Create FRAGMENT SHADER OBJECT
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//CREATE SOURCE CODE FOR FRAGMENT SHADER
/*	const GLchar *fragmentShaderSourceCode =
		"#version 430"\
		"\n" \
	
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
				"vec3 reflection_vector = reflect(-light_direction, transformed_normals);" \
				"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
				"vec3 specular = u_Ls_out * u_Ks_out * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess_out);" \
				"phong_ads_color=ambient + diffuse + specular;" \
			"}" \
			"else" \
			"{" \
				"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
			"}" \

			"FragColor = vec4(phong_ads_color, 1.0);" \
		"}";
	*/

		const GLchar *fragmentShaderSourceCode =
			"#version 430"\
			"\n" \
			"in vec3 phong_ads_color;" \
			"out vec4 FragColor;" \
			"void main(void)" \
			"{" \
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
				UnIntializeOpenGL(hwnd, hdc, hglrc);
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
	light_position_uniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

	light_rotation_uniform = glGetUniformLocation(gShaderProgramObject, "u_light_rotation");
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


	glClearColor(0.25f, 0.25f, 0.25f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	
	glDepthFunc(GL_LEQUAL);
	
	glShadeModel(GL_SMOOTH);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_CULL_FACE); /*Turning oFF cull , as we are doing animation , and so need to paint the back of object when rotating*/


	gPerspectiveProjectionMatrix = mat4::identity();

	gbLight = false;

	InitializeMaterials();

	
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

	if(gbLight)
		glUniform1i(L_KeyPressed_uniform, 1);
	else
		glUniform1i(L_KeyPressed_uniform, 0);


	//DRAWING SPHERE
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 rotateMatrix = mat4::identity();


	viewMatrix = mat4::identity();
	//model view
	
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	

	// setting light's properties
	glUniform3fv(La_uniform, 1, lightAmbient);
	glUniform3fv(Ld_uniform, 1, lightDiffuse);
	glUniform3fv(Ls_uniform, 1, lightSpecular);
	glUniform4fv(light_position_uniform, 1, lightPosition);


	//double row = -4.0f;
	//double col = 4.4f;

	int col = 5;
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			GLfloat newWidth = (w / 4.0f);
			GLfloat newHeight = h / 6.0f;

			glViewport((newWidth*j), newHeight*col, newWidth, newHeight);
			gPerspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)newWidth / (GLfloat)newHeight, 0.1f, 100.0f);

			// setting material's properties
			glUniform3fv(Ka_uniform, 1, material[i][j].ambient_material);
			glUniform3fv(Kd_uniform, 1, material[i][j].diffuse_material);
			glUniform3fv(Ks_uniform, 1, material[i][j].specular_material);
			glUniform1f(material_shininess_uniform, material[i][j].shininess);
			//


			rotateMatrix = mat4::identity();
			modelMatrix = vmath::translate(0.0f, 0.0f, -2.0f);
			//modelMatrix = vmath::translate((GLfloat)row, (GLfloat)col, -13.0f);
			if (gIsXKeyPressed == GL_TRUE)
			{
				rotateMatrix = vmath::rotate(sphere_angle, 1.0f, 0.0f, 0.0f);
				//modelMatrix = modelMatrix * rotateMatrix;

				lightPosition[1] = sphere_angle;
				glUniform4fv(light_position_uniform, 1, lightPosition);
				glUniformMatrix4fv(light_rotation_uniform, 1, GL_FALSE, rotateMatrix);
			}
			else
			if (gIsYKeyPressed == GL_TRUE)
			{
				rotateMatrix = vmath::rotate(sphere_angle, 0.0f, 1.0f, 0.0f);
				//modelMatrix = modelMatrix * rotateMatrix;

				lightPosition[0] = sphere_angle;
				glUniform4fv(light_position_uniform, 1, lightPosition);
				glUniformMatrix4fv(light_rotation_uniform, 1, GL_FALSE, rotateMatrix);
			}
			else
			if (gIsZKeyPressed == GL_TRUE)
			{
				rotateMatrix = vmath::rotate(sphere_angle, 0.0f, 0.0f, 1.0f);
				//modelMatrix = modelMatrix * rotateMatrix;

				lightPosition[0] = sphere_angle;
				glUniform4fv(light_position_uniform, 1, lightPosition);
				glUniformMatrix4fv(light_rotation_uniform, 1, GL_FALSE, rotateMatrix);
			}
			else
			{
				lightPosition[3] = 0.0f;
				lightPosition[1] = 0.0f;
				lightPosition[2] = 1.0f;
				glUniform4fv(light_position_uniform, 1, lightPosition);
			}
			
			glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
			//row = row + 2.8f;
		
			
			// BIND VAO
			glBindVertexArray(gVao_sphere);

			//START DRAWING
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
			glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
			// UNBING VAO
			glBindVertexArray(0);

		}

		col--;

		//row = -4.0f;
		//col = col -1.8f;
	}



	//STOP USING SHADER
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

		case 88:
			lightPosition[0] = 0.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = 0.0f;
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


		case 89:
			lightPosition[0] = 0.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = 0.0f;
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

		case 90:
			lightPosition[0] = 0.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = 0.0f;
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

	w = width;
	h = height;

	//glViewport(0, 0, (GLsizei)width, (GLsizei)height);

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

	material[2][1].specular_material[0] = 0.774597f; material[2][1].specular_material[1] = 0.774597f;
	material[2][1].specular_material[2] = 0.774597f; material[2][1].specular_material[3] = 1.0f;
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

	material[1][2].specular_material[0] = 0.50196078f; material[1][2].specular_material[1] = 0.50196078f;
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
	material[0][3].shininess = 0.07812f* 128.0f;


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