/*
* @author: Praveen Suryawanshi
* @mailto: suryawanshi.praveen007@gmail.com
* please dont remove @author or modify it.
* any changes or bug fixes are done please annotate the locations where bug fixed.
*/


#include <Windows.h>
#include<stdio.h>
#include <gl\glew.h>

#include<gl/GL.h>
#include<Math.h>
#include"ModelParser.h"
#include "vmath.h"


#pragma comment(lib,"glew32.lib")
#pragma comment(lib, "opengl32.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT  600

using namespace ThreeDModelLoader;
using namespace vmath;


/*enum
{
	VDG_ATTRIBUTE_POSITION = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};*/


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



GLuint model_matrix_uniform;
GLuint view_matrix_uniform;
GLuint projection_matrix_uniform;
mat4 gPerspectiveProjectionMatrix;



GLuint La1_uniform;
GLuint Ld1_uniform;
GLuint Ls1_uniform;
GLuint light_position1_uniform;

GLuint La2_uniform;
GLuint Ld2_uniform;
GLuint Ls2_uniform;
GLuint light_position2_uniform;


GLuint L_KeyPressed_uniform;

GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;


GLfloat left_lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat left_lightDiffuse[] = { 1.0f,0.0f,0.0f,0.0f };
GLfloat left_lightSpecular[] = { 0.0f,0.0f,1.0f,0.0f };
GLfloat left_lightPosition[] = { -2.0f,1.0f,1.0f,0.0f };


GLfloat right_lightAmbient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat right_lightDiffuse[] = { 0.0f,0.0f,1.0f,0.0f };
GLfloat right_lightSpecular[] = { 1.0f,0.0f,0.0f,0.0f };
GLfloat right_lightPosition[] = { 2.0f,1.0f,1.0f,0.0f };

bool gbLight;


GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;

GLuint gVao_object;
GLuint gVbo_object_position;
GLuint gVbo_object_colors;
GLuint gVbo_object_normal;
GLuint gVbo_object_element;
GLuint gVbo_object_textures_element;
GLuint gVbo_object_normals_element;
float object_angle = 0.0f;

//odelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\pillar.obj");


//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\MonkeyHead.OBJ",true);
//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\MonkeyHead.OBJ");
//
//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\Triangle.OBJ",true);
ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\Triangle.OBJ");

//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\Cat.obj");

//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\GUN.obj");

//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\IceLandAir.OBJ",true);
 //ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\IceLandAir.OBJ");

///ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\cube.OBJ");

//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\skull.OBJ",true);
//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\skull.OBJ");

///ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\suzen.OBJ");
//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\CocaCola.OBJ");

//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\Lamborghini_Aventador.OBJ",true);
//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\Lamborghini_Aventador.OBJ");

//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\IronMan.OBJ",true);
//ModelParser *object_mesh = new ModelParser("C:\\Users\\Praveen Suryawanshi\\Documents\\OBJLoderUtility\\OBJLoderUtility\\IronMan.OBJ");
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void IntializeOpenGL(HWND, HDC*, HGLRC*);

	void Update(void);

	void Render(HDC hdc);

	void UnIntializeOpenGL(HWND, HDC*, HGLRC*);



	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szAppName[] = TEXT("OpenGL Programmable Pipeline Model Loading.");
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


				Update();

				Render(hdc);
			}
		}


	}

	UnIntializeOpenGL(hWnd, &hdc, &hglrc);

	return ((int)msg.wParam);
}

void Update()
{
	if (object_angle >= 360.0f)
		object_angle = 0.0f;

	object_angle = object_angle + 0.1f;

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
		"in vec3 vColor;"\
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_lighting_enabled;" \
		"uniform vec3 u_La1;" \
		"uniform vec3 u_Ld1;" \
		"uniform vec3 u_Ls1;" \
		"uniform vec4 u_light_position1;" \
		"uniform vec3 u_La2;" \
		"uniform vec3 u_Ld2;" \
		"uniform vec3 u_Ls2;" \
		"uniform vec4 u_light_position2;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"out vec3 phong_ads_color;" \
		"void main(void)" \
		"{" \
		"if(u_lighting_enabled==1)" \
		"{" \
		"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
		"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \

		"vec3 light_direction1 = normalize(vec3(u_light_position1) - eye_coordinates.xyz);" \
		"float tn_dot_ld1 = max(dot(transformed_normals, light_direction1),0.0);" \
		"vec3 ambient1 = u_La1 * u_Ka;" \
		"vec3 diffuse1 = u_Ld1* u_Kd * tn_dot_ld1;" \
		"vec3 reflection_vector1 = reflect(-light_direction1, transformed_normals);" \
		"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
		"vec3 specular1 = u_Ls1 * u_Ks * pow(max(dot(reflection_vector1, viewer_vector), 0.0), u_material_shininess);" \

		"vec3 light_direction2 = normalize(vec3(u_light_position2) - eye_coordinates.xyz);" \
		"float tn_dot_ld2 = max(dot(transformed_normals, light_direction2),0.0);" \
		"vec3 ambient2 = u_La2 * u_Ka;" \
		"vec3 diffuse2 = u_Ld2* u_Kd * tn_dot_ld2;" \
		"vec3 reflection_vector2 = reflect(-light_direction2, transformed_normals);" \
		"vec3 specular2 = u_Ls2 * u_Ks * pow(max(dot(reflection_vector2, viewer_vector), 0.0), u_material_shininess);" \


		"phong_ads_color= (ambient1+ambient2) +( diffuse1+diffuse2 )+ (specular1+specular2);" \
		"}" \
		"else" \
		"{" \
		"phong_ads_color = vColor;" \
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
	La1_uniform = glGetUniformLocation(gShaderProgramObject, "u_La1");

	// DIFUSE COLOR TOKEN
	Ld1_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ld1");

	// SPECULAR COLOR TOKEN
	Ls1_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ls1");

	// LIGHT POSITION TOKEN
	light_position1_uniform = glGetUniformLocation(gShaderProgramObject, "u_light_position1");



	//AMBIENT COLOR TOKEN
	La2_uniform = glGetUniformLocation(gShaderProgramObject, "u_La2");

	// DIFUSE COLOR TOKEN
	Ld2_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ld2");

	// SPECULAR COLOR TOKEN
	Ls2_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ls2");

	// LIGHT POSITION TOKEN
	light_position2_uniform = glGetUniformLocation(gShaderProgramObject, "u_light_position2");




	// AMBIENT REFLECTIVE TOKEN
	Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");

	// DIFUUSE REFLECTIVE TOKEN
	Kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");

	// SPECULAR REFLECTIVE TOKEN
	Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");

	//SHINYNESS COLOR TOKEN
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");;

	/*VERTICES ,COLOR ,NORMAL  INITIALIZATION*/

	/*GLfloat pyramidVertices[] =
	{
		// Front face
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		//Back Face
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		//Right face
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		//Left face
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f


	};

	const GLfloat PyramidNormals[] =
	{
		// Front face
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,
		0.0f, 0.447214f, 0.894427f,

		//Back Face
		0.0f, 0.4472141f, -0.894427f,
		0.0f, 0.4472141f, -0.894427f,
		0.0f, 0.4472141f, -0.894427f,

		//Right face
		0.894427f, 0.4472141f, 0.0f,
		0.894427f, 0.4472141f, 0.0f,
		0.894427f, 0.4472141f, 0.0f,
		//Left face

		-0.894427f, 0.4472141f, 0.0f,
		-0.894427f, 0.4472141f, 0.0f,
		-0.894427f, 0.4472141f, 0.0f,
	};


	*/
	object_mesh->parse(ThreeDModelLoader::PROCESS_TYPE::GPU, ThreeDModelLoader::DRAW_TYPE::DRAW_USING_ELEMENTS);
	//VERTICES , COLORS ,SHADERS ATTRIBS ,VBO ,VAO INITIALIZATION


	/*cube*/
	//glGenVertexArrays(1, &gVao_object);
	//glBindVertexArray(gVao_object);


	/*position*/
	/*glGenBuffers(1, &gVbo_object_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_object_position);

	glBufferData(GL_ARRAY_BUFFER, object_mesh->getSizeOfVerticesArrayForDrawUsingArrays(), object_mesh->getVerticesArrayForDrawUsingArrays(), GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
	

	/*glGenBuffers(1, &gVbo_object_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_object_position);

	glBufferData(GL_ARRAY_BUFFER, object_mesh->getSizeOfVerticesArrayForDrawUsingElements(), object_mesh->getVerticesArrayForDrawUsingElements(), GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
	/**/


	/*Texture CoCoord*/
	/*
	glGenBuffers(1, &gVbo_object_colors);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_object_colors);

	glBufferData(GL_ARRAY_BUFFER, object_mesh->getSizeOfTexturesCoords(), object_mesh->getTexturesCoords(), GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, object_mesh->getTexCordArrayCount(), GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	*/
	/**/
	
	//for trial
	/*glGenBuffers(1, &gVbo_object_colors);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_object_colors);

	glBufferData(GL_ARRAY_BUFFER, object_mesh->getSizeOfTexturesArrayForDrawUsingElements(), object_mesh->getTexturesArrayForDrawUsingElements(), GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
	//


	/*normals*/
	/*glGenBuffers(1, &gVbo_object_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_object_normal);

	glBufferData(GL_ARRAY_BUFFER, object_mesh->getSizeOfNormalsArrayForDrawUsingElements(), object_mesh->getNormalsArrayForDrawUsingElements(), GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
	/**/


	/*ELEMENT VBO*/
	/*glGenBuffers(1, &gVbo_object_textures_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_object_textures_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object_mesh->getSizeOfTexturesIndicesArray(), object_mesh->getTexturesIndicesArray(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	*//**/

	/*ELEMENT VBO*/
	/*glGenBuffers(1, &gVbo_object_normals_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_object_normals_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object_mesh->getSizeOfNormalsIndicesArray(), object_mesh->getNormalsIndicesArray(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	*//**/

	/*ELEMENT VBO*/
	/*glGenBuffers(1, &gVbo_object_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_object_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object_mesh->getSizeOfIndicesArray(), object_mesh->getIndicesArray(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
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

	gbLight = false;

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



	if (gbLight == true)
	{
		// set 'u_lighting_enabled' uniform
		glUniform1i(L_KeyPressed_uniform, 1);

		// setting light's properties
		glUniform3fv(La1_uniform, 1, left_lightAmbient);
		glUniform3fv(Ld1_uniform, 1, left_lightDiffuse);
		glUniform3fv(Ls1_uniform, 1, left_lightSpecular);
		glUniform4fv(light_position1_uniform, 1, left_lightPosition);


		// setting light's properties
		glUniform3fv(La2_uniform, 1, right_lightAmbient);
		glUniform3fv(Ld2_uniform, 1, right_lightDiffuse);
		glUniform3fv(Ls2_uniform, 1, right_lightSpecular);
		glUniform4fv(light_position2_uniform, 1, right_lightPosition);

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
	mat4 rotateMatrix = mat4::identity();


	  modelMatrix = translate(-0.0f, 0.0f, -5.0f);
	 //modelMatrix = translate(0.0f, 0.0f, -1500.0f);
      //modelMatrix = translate(0.0f, -100.0f, -1000.0f); // for ironman test1
	
	rotateMatrix = vmath::rotate<GLfloat>(object_angle, 0.0f, 1.0f, 0.0f);

	modelMatrix = modelMatrix * rotateMatrix;

	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	// BIND VAO
	//glBindVertexArray(gVao_object);

	//START DRAWING
	//glDrawArrays(GL_TRIANGLES, 0, object_mesh->getSizeOfVerticesArrayForDrawUsingArrays());
	object_mesh->draw();
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_object_element);
	//glDrawElements(GL_TRIANGLES, object_mesh->getIndicesArrayCount(), GL_UNSIGNED_INT, 0);
	// UNBING VAO
	//glBindVertexArray(0);

	//STOP USING SHADER
	glUseProgram(0);

	SwapBuffers(hdc);
}
void UnIntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{

	if (hglrc != NULL)
	{
		wglDeleteContext(*hglrc);
		*hglrc = NULL;
	}

	if (hdc != NULL)
	{
		ReleaseDC(hwnd, *hdc);
		*hdc = NULL;
	}


	DestroyWindow(hwnd);
	hwnd = NULL;	




	//DESTROY VBO
	if (gVbo_object_position)
	{
		glDeleteVertexArrays(1, &gVbo_object_position);
		gVbo_object_position = 0;
	}
	if (gVbo_object_colors)
	{
		glDeleteVertexArrays(1, &gVbo_object_colors);
		gVbo_object_colors = 0;
	}
	if (gVbo_object_normal)
	{
		glDeleteVertexArrays(1, &gVbo_object_normal);
		gVbo_object_normal = 0;
	}
	if (gVbo_object_element)
	{
		
		glDeleteBuffers(1, &gVbo_object_element);
		gVbo_object_element = 0;
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

	gPerspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 6000.0f);

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
