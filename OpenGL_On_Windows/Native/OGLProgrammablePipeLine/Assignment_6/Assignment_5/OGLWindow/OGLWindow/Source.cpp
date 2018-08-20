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
bool gbIsQKeyPressed = false;

static bool bIsAKeyPressed = false;
static bool bIsLKeyPressed = false;
static bool bIsFkeyPressed = false;
static bool bIsVkeyPressed = false;
static bool bIsEscKeyPressed = false;


LONG glPreviousStyle;
WINDOWPLACEMENT  gwndplacement;
MONITORINFO mi;


FILE *gpLogFile = NULL;

GLuint gVertexShaderObject_vertexShader;
GLuint gFragmentShaderObject_vertexShader;
GLuint gShaderProgramObject_vertexShader;

GLuint gVertexShaderObject_fragmentShader;
GLuint gFragmentShaderObject_fragmentShader;
GLuint gShaderProgramObject_fragmentShader;

GLuint gVertexShaderObject_SimpleShader;
GLuint gFragmentShaderObject_SimpleShader;
GLuint gShaderProgramObject_SimpleShader;

GLuint model_matrix_uniform;
GLuint view_matrix_uniform;
GLuint projection_matrix_uniform;
mat4 gPerspectiveProjectionMatrix;



GLuint La1_uniform;
GLuint Ld1_uniform;
GLuint Ls1_uniform;
GLuint light_position1_uniform;
GLuint light_rotation1_uniform;

GLuint La2_uniform;
GLuint Ld2_uniform;
GLuint Ls2_uniform;
GLuint light_position2_uniform;
GLuint light_rotation2_uniform;

GLuint La3_uniform;
GLuint Ld3_uniform;
GLuint Ls3_uniform;
GLuint light_position3_uniform;
GLuint light_rotation3_uniform;
GLuint gbUseFragment_uniform;
GLuint gbUseVertex_uniform;


GLuint L_KeyPressed_uniform;

GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;


GLfloat light1_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat light1_difuse[] = { 1.0f,0.0f,0.0f,0.0f };
GLfloat light1_specular[] = { 1.0f,0.0f,0.0f,0.0f };
GLfloat light1_position[] = { 0.0f,0.0f,0.0f,0.0f };


GLfloat light2_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat light2_difuse[] = { 0.0f,1.0f,0.0f,0.0f };
GLfloat light2_specular[] = { 0.0f,1.0f,0.0f,0.0f };
GLfloat light2_position[] = { 0.0f,0.0f,0.0f,0.0f };


GLfloat light3_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat light3_difuse[] = { 0.0f,0.0f,1.0f,0.0f };
GLfloat light3_specular[] = { 0.0f,0.0f,1.0f,0.0f };
GLfloat light3_position[] = { 0.0f,0.0f,0.0f,0.0f };


bool gbLight;
GLfloat angleRed = 0.0f;
GLfloat angleBlue = 0.0f;
GLfloat angleGreen = 0.0f;


GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,0.0f };
GLfloat material_difuse[] = { 1.0f,1.0f,1.0f,0.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,0.0f };
GLfloat material_shinyness = 50.0f;

GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;
unsigned short sphere_elements[2280];
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
GLuint gNumElements;
GLuint gNumVertices;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void IntializeOpenGL(HWND, HDC*, HGLRC*);

	void Update(void);

	void Render(HDC hdc);

	void UnIntializeOpenGL(HWND, HDC*, HGLRC*);



	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szAppName[] = TEXT("Three Lights on Sphere using Fragment and Vertex Shader.");
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
				if (gbIsQKeyPressed == true)
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
	if (angleRed >= 720.0f)
		angleRed = 360.0f;
	else 
		angleRed = angleRed + 0.03f;


	if (angleBlue >= 720.0f)
		angleBlue = 360.0f;
	else 
		angleBlue = angleBlue + 0.03f;


	if (angleGreen >= 720.0f)
		angleGreen = 360.0f;
	else 
		angleGreen = angleGreen + 0.03f;
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
	gVertexShaderObject_vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//CREATE SOURCE CODE FOR VERTEX SHADER OBJECT

	/*

	*/
	const GLchar *vertexShaderSourcecode =
		"#version 430" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \

		"uniform int u_lighting_enabled;" \

		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \

		"uniform vec3 u_La1;" \
		"uniform vec3 u_Ld1;" \
		"uniform vec3 u_Ls1;" \
		"uniform vec4 u_light_position1;" \
		"uniform mat4 u_light_rotation1;"\

		"uniform vec3 u_La2;" \
		"uniform vec3 u_Ld2;" \
		"uniform vec3 u_Ls2;" \
		"uniform vec4 u_light_position2;" \
		"uniform mat4 u_light_rotation2;"\

		"uniform vec3 u_La3;" \
		"uniform vec3 u_Ld3;" \
		"uniform vec3 u_Ls3;" \
		"uniform vec4 u_light_position3;" \
		"uniform mat4 u_light_rotation3;"\

		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \

		"out vec3 phong_ads_color;" \
		"void main(void)" \
		"{" \

		"if(u_lighting_enabled == 1)"\
		"{"\
			"vec4 temp;"\
			"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
			"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
			
			"temp =u_light_position1 * u_light_rotation1;"\
			"vec3 light_direction1 = normalize(vec3(temp) - eye_coordinates.xyz);" \
			"float tn_dot_ld1 = max(dot(transformed_normals, light_direction1),0.0);" \
			"vec3 ambient1 = u_La1 * u_Ka;" \
			"vec3 diffuse1 = u_Ld1* u_Kd * tn_dot_ld1;" \
			"vec3 reflection_vector1 = reflect(-light_direction1, transformed_normals);" \
			"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
			"vec3 specular1 = u_Ls1 * u_Ks * pow(max(dot(reflection_vector1, viewer_vector), 0.0), u_material_shininess);" \

			"temp =u_light_position2 * u_light_rotation2;"\
			"vec3 light_direction2 = normalize(vec3(temp) - eye_coordinates.xyz);" \
			"float tn_dot_ld2 = max(dot(transformed_normals, light_direction2),0.0);" \
			"vec3 ambient2 = u_La2 * u_Ka;" \
			"vec3 diffuse2 = u_Ld2* u_Kd * tn_dot_ld2;" \
			"vec3 reflection_vector2 = reflect(-light_direction2, transformed_normals);" \
			"vec3 specular2 = u_Ls2 * u_Ks * pow(max(dot(reflection_vector2, viewer_vector), 0.0), u_material_shininess);" \
			
			"temp =u_light_position3 * u_light_rotation3;"\
			"vec3 light_direction3= normalize(vec3(temp) - eye_coordinates.xyz);" \
			"float tn_dot_ld3 = max(dot(transformed_normals, light_direction3),0.0);" \
			"vec3 ambient3 = u_La3 * u_Ka;" \
			"vec3 diffuse3 = u_Ld3 * u_Kd * tn_dot_ld3;" \
			
			"vec3 reflection_vector3 = reflect(-light_direction3, transformed_normals);" \
			
			"vec3 specular3 = u_Ls3 * u_Ks * pow(max(dot(reflection_vector3, viewer_vector), 0.0), u_material_shininess);" \
			
			"phong_ads_color= (ambient1+ambient2+ambient3) + (diffuse1+diffuse2+diffuse3)+ (specular1+specular2+specular3);" \
		"}"\
		"else"\
		"{"\
			"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
		"}"\
			
				"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
	"}";
			
	//BI	ND vertexShaderSourcecode CODE TO gVertexShaderObject
	glShaderSource(gVertexShaderObject_vertexShader, 1, (const GLchar **)&vertexShaderSourcecode, NULL);
			
	//CO	MPILE VERTEX SHADER
	glCompileShader(gVertexShaderObject_vertexShader);
			
	/***	*************/

			
	GLint infoLogLength = 0;
	GLint iShaderCompilationStatus = 0;
	char *szInfoLog = NULL;
	GLsizei written = 0;

	glGetShaderiv(gVertexShaderObject_vertexShader, GL_COMPILE_STATUS, &iShaderCompilationStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(infoLogLength);

			if (szInfoLog != NULL)
			{

				glGetShaderInfoLog(gVertexShaderObject_vertexShader, infoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				UnIntializeOpenGL(hwnd, hdc, hglrc);
				exit(0);
			}
		}
	}


	//Create FRAGMENT SHADER OBJECT
	gFragmentShaderObject_vertexShader = glCreateShader(GL_FRAGMENT_SHADER);

	//CREATE SOURCE CODE FOR FRAGMENT SHADER
	const GLchar *fragmentShaderSourceCode =
		"#version 430"\
		"\n" \
		"out vec4 FragColor;" \
		"in vec3 phong_ads_color;" \
		"void main(void)" \
		"{" \
			"FragColor = vec4(phong_ads_color, 1.0);" \
		"}";

	//BIND fragmentShaderSourceCode to gFragmentShaderObject
	glShaderSource(gFragmentShaderObject_vertexShader, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	//COMPILE FRAGMENT SHADER
	glCompileShader(gFragmentShaderObject_vertexShader);

	infoLogLength = 0;
	iShaderCompilationStatus = 0;
	szInfoLog = NULL;
	written = 0;

	glGetShaderiv(gFragmentShaderObject_vertexShader, GL_COMPILE_STATUS, &iShaderCompilationStatus);
	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(sizeof(infoLogLength));

			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(gFragmentShaderObject_vertexShader, infoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				UnIntializeOpenGL(hwnd, hdc, hglrc);
				exit(0);
			}
		}
	}

	// *** SHADER PROGRAM ****//
	//CREATE SHADER PROGRAM OBJECT
	gShaderProgramObject_vertexShader = glCreateProgram();

	//ATTACH VERTEX SHADER PROGRAM TO SHADER PROGRAM
	glAttachShader(gShaderProgramObject_vertexShader, gVertexShaderObject_vertexShader);

	//ATTACH FRAGMENT SHADER PROGRAM TO SHADER PROGRAM
	glAttachShader(gShaderProgramObject_vertexShader, gFragmentShaderObject_vertexShader);

	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITH VERTEX SHADER POSITION ATTRIBUTE
	glBindAttribLocation(gShaderProgramObject_vertexShader, VDG_ATTRIBUTE_POSITION, "vPosition");
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH TEXTURE SHADER ATTRIBUTE
	glBindAttribLocation(gShaderProgramObject_vertexShader, VDG_ATTRIBUTE_NORMAL, "vNormal");

	//LINK THE SHADER
	glLinkProgram(gShaderProgramObject_vertexShader);

	infoLogLength = 0;
	iShaderCompilationStatus = 0;
	szInfoLog = NULL;
	written = 0;

	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject_vertexShader, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(sizeof(infoLogLength));
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(gShaderProgramObject_vertexShader, infoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Shader Program Link Log : %s \n", szInfoLog);
				free(szInfoLog);
				UnIntializeOpenGL(hwnd, hdc, hglrc);
				exit(0);
			}

		}
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//CREAT VERTEXT SHADER OBJECT
	gVertexShaderObject_fragmentShader = glCreateShader(GL_VERTEX_SHADER);

	//CREATE SOURCE CODE FOR VERTEX SHADER OBJECT

	
	vertexShaderSourcecode =
		"#version 430" \
		"\n" \

		"uniform int u_lighting_enabled;" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light_position1;" \
		"uniform mat4 u_light_rotation1;"\
		"uniform vec4 u_light_position2;" \
		"uniform mat4 u_light_rotation2;"\
		"uniform vec4 u_light_position3;" \
		"uniform mat4 u_light_rotation3;"\

		"out vec3 transformed_normals;" \
		"out vec3 light_direction1;" \
		"out vec3 light_direction2;" \
		"out vec3 light_direction3;" \
		"out vec3 viewer_vector;" \

		"void main(void)" \
		"{" \
			"if(u_lighting_enabled == 1)" \
			"{" \
			"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
			"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
			"viewer_vector = -eye_coordinates.xyz;" \

			"vec4 temp1= u_light_position1 * u_light_rotation1 ;"\
			"vec4 temp2= u_light_position2 * u_light_rotation2 ;"\
			"vec4 temp3= u_light_position3 * u_light_rotation3 ;"\

				
			"light_direction1 = vec3(temp1) - eye_coordinates.xyz;" \
			"light_direction2 = vec3(temp2) - eye_coordinates.xyz;" \
			"light_direction3 = vec3(temp3) - eye_coordinates.xyz;" \
				
			"}" \
			"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		"}";

	//BIND vertexShaderSourcecode CODE TO gVertexShaderObject
	glShaderSource(gVertexShaderObject_fragmentShader, 1, (const GLchar **)&vertexShaderSourcecode, NULL);

	//COMPILE VERTEX SHADER
	glCompileShader(gVertexShaderObject_fragmentShader);

	/****************/

	
	 infoLogLength = 0;
	 iShaderCompilationStatus = 0;
	 szInfoLog = NULL;
	 written = 0;

	glGetShaderiv(gVertexShaderObject_fragmentShader, GL_COMPILE_STATUS, &iShaderCompilationStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(infoLogLength);

			if (szInfoLog != NULL)
			{

				glGetShaderInfoLog(gVertexShaderObject_fragmentShader, infoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				UnIntializeOpenGL(hwnd, hdc, hglrc);
				exit(0);
			}
		}
	}


	//Create FRAGMENT SHADER OBJECT
	gFragmentShaderObject_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//CREATE SOURCE CODE FOR FRAGMENT SHADER
	fragmentShaderSourceCode =
		"#version 430"\
		"\n" \

		"in vec3 transformed_normals;" \
		"in vec3 viewer_vector;" \
		"out vec4 FragColor;" \
	
		"in vec3 light_direction1;" \
		"in vec3 light_direction2;" \
		"in vec3 light_direction3;" \

		"uniform vec3 u_La1;" \
		"uniform vec3 u_Ld1;" \
		"uniform vec3 u_Ls1;" \

		"uniform vec3 u_La2;" \
		"uniform vec3 u_Ld2;" \
		"uniform vec3 u_Ls2;" \

		"uniform vec3 u_La3;" \
		"uniform vec3 u_Ld3;" \
		"uniform vec3 u_Ls3;" \


		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \
		"void main(void)" \
		"{" \
			"vec3 phong_ads_color;" \
			"if(u_lighting_enabled==1)" \
			"{" \
				"vec3 normalized_transformed_normals=normalize(transformed_normals);" \
				"vec3 normalized_viewer_vector=normalize(viewer_vector);" \

				"vec3 normalized_light_direction1 =normalize(light_direction1);"\
				"vec3 ambient1 = u_La1 * u_Ka;" \
				"float tn_dot_ld1 = max(dot(normalized_transformed_normals, normalized_light_direction1),0.0);" \
				"vec3 diffuse1 = u_Ld1 * u_Kd * tn_dot_ld1;" \
				"vec3 reflection_vector1 = reflect(-normalized_light_direction1, normalized_transformed_normals);" \
				"vec3 specular1 = u_Ls1 * u_Ks * pow(max(dot(reflection_vector1, normalized_viewer_vector), 0.0), u_material_shininess);" \
				
				"vec3 normalized_light_direction2 =normalize(light_direction2);"\
				"vec3 ambient2 = u_La2 * u_Ka;" \
				"float tn_dot_ld2 = max(dot(normalized_transformed_normals, normalized_light_direction2),0.0);" \
				"vec3 diffuse2 = u_Ld2 * u_Kd * tn_dot_ld2;" \
				"vec3 reflection_vector2 = reflect(-normalized_light_direction2, normalized_transformed_normals);" \
				"vec3 specular2 = u_Ls2 * u_Ks * pow(max(dot(reflection_vector2, normalized_viewer_vector), 0.0), u_material_shininess);" \
				

				"vec3 normalized_light_direction3 =normalize(light_direction3);"\
				"vec3 ambient3 = u_La3 * u_Ka;" \
				"float tn_dot_ld3 = max(dot(normalized_transformed_normals, normalized_light_direction3),0.0);" \
				"vec3 diffuse3 = u_Ld3 * u_Kd * tn_dot_ld3;" \
				"vec3 reflection_vector3 = reflect(-normalized_light_direction3, normalized_transformed_normals);" \
				"vec3 specular3 = u_Ls3 * u_Ks * pow(max(dot(reflection_vector3, normalized_viewer_vector), 0.0), u_material_shininess);" \

				"phong_ads_color= (ambient1+ambient2+ambient3) + (diffuse1+diffuse2+diffuse3) + (specular1+specular2+specular3);" \

			"}" \
			"else" \
			"{" \
				"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
			"}" \
			"FragColor = vec4(phong_ads_color, 1.0);" \
		"}";

	//BIND fragmentShaderSourceCode to gFragmentShaderObject
	glShaderSource(gFragmentShaderObject_fragmentShader, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	//COMPILE FRAGMENT SHADER
	glCompileShader(gFragmentShaderObject_fragmentShader);

	infoLogLength = 0;
	iShaderCompilationStatus = 0;
	szInfoLog = NULL;
	written = 0;

	glGetShaderiv(gFragmentShaderObject_fragmentShader, GL_COMPILE_STATUS, &iShaderCompilationStatus);
	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(sizeof(infoLogLength));

			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(gFragmentShaderObject_fragmentShader, infoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				UnIntializeOpenGL(hwnd, hdc, hglrc);
				exit(0);
			}
		}
	}

	// *** SHADER PROGRAM ****//
	//CREATE SHADER PROGRAM OBJECT
	gShaderProgramObject_fragmentShader = glCreateProgram();

	//ATTACH VERTEX SHADER PROGRAM TO SHADER PROGRAM
	glAttachShader(gShaderProgramObject_fragmentShader, gVertexShaderObject_fragmentShader);

	//ATTACH FRAGMENT SHADER PROGRAM TO SHADER PROGRAM
	glAttachShader(gShaderProgramObject_fragmentShader, gFragmentShaderObject_fragmentShader);

	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITH VERTEX SHADER POSITION ATTRIBUTE
	glBindAttribLocation(gShaderProgramObject_fragmentShader, VDG_ATTRIBUTE_POSITION, "vPosition");
	//PRE-LINK BINDING OF SHADER PROGRAM OBJECT WITTH TEXTURE SHADER ATTRIBUTE
	glBindAttribLocation(gShaderProgramObject_fragmentShader, VDG_ATTRIBUTE_NORMAL, "vNormal");

	//LINK THE SHADER
	glLinkProgram(gShaderProgramObject_fragmentShader);

	infoLogLength = 0;
	iShaderCompilationStatus = 0;
	szInfoLog = NULL;
	written = 0;

	iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject_fragmentShader, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(sizeof(infoLogLength));
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(gShaderProgramObject_fragmentShader, infoLogLength, &written, szInfoLog);
				fprintf(gpLogFile, "Shader Program Link Log : %s \n", szInfoLog);
				free(szInfoLog);
				UnIntializeOpenGL(hwnd, hdc, hglrc);
				exit(0);
			}

		}
	}


	/*VERTICES ,COLOR ,NORMAL  INITIALIZATION*/

	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);

	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();



	//VERTICES , COLORS ,SHADERS ATTRIBS ,VBO ,VAO INITIALIZATION


	/*cube*/
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);


	/*position*/
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*normals*/
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);

	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	/*element*/
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_element);

	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	
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

	

	//Decide Which Shader to use per Fragment or per Vertex shader for Lightinh
	//if (bIsVkeyPressed == true || gbLight == false)
	//{
		//GET MVP UNIFORM LOCATION
		model_matrix_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_model_matrix");
		view_matrix_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_view_matrix");
		projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_projection_matrix");
		L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_lighting_enabled");

		// 'L' HANDLE KEY TO TOGGLE LIGHT.

		//AMBIENT COLOR TOKEN
		La1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La1");

		// DIFUSE COLOR TOKEN
		Ld1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ld1");

		// SPECULAR COLOR TOKEN
		Ls1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ls1");

		// LIGHT POSITION TOKEN
		light_position1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_position1");

		// LIGHT rotation TOKEN
		light_rotation1_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_rotation1");




		//AMBIENT COLOR TOKEN
		La2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La2");

		// DIFUSE COLOR TOKEN
		Ld2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ld2");

		// SPECULAR COLOR TOKEN
		Ls2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ls2");

		// LIGHT POSITION TOKEN
		light_position2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_position2");


		// LIGHT rotation TOKEN
		light_rotation2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_rotation2");





		//AMBIENT COLOR TOKEN
		La3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La3");

		// DIFUSE COLOR TOKEN
		Ld3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ld3");

		// SPECULAR COLOR TOKEN
		Ls3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ls3");

		// LIGHT POSITION TOKEN
		light_position3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_position3");

		// LIGHT rotation TOKEN
		light_rotation3_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_light_rotation3");



		// AMBIENT REFLECTIVE TOKEN
		Ka_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ka");

		// DIFUUSE REFLECTIVE TOKEN
		Kd_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Kd");

		// SPECULAR REFLECTIVE TOKEN
		Ks_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_Ks");

		//SHINYNESS COLOR TOKEN
		material_shininess_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_material_shininess");


		glUseProgram(gShaderProgramObject_vertexShader);
	//}


	if (bIsFkeyPressed)
	{	
		//GET MVP UNIFORM LOCATION
		model_matrix_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_model_matrix");
		view_matrix_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_view_matrix");
		projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_projection_matrix");
		L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_lighting_enabled");

		// 'L' HANDLE KEY TO TOGGLE LIGHT.


	
		//AMBIENT COLOR TOKEN
		La1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_La1");

		// DIFUSE COLOR TOKEN
		Ld1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ld1");

		// SPECULAR COLOR TOKEN
		Ls1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ls1");

		// LIGHT POSITION TOKEN
		light_position1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_position1");

		// LIGHT rotation TOKEN
		light_rotation1_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_rotation1");


		//AMBIENT COLOR TOKEN
		La2_uniform = glGetUniformLocation(gShaderProgramObject_vertexShader, "u_La2");

		// DIFUSE COLOR TOKEN
		Ld2_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ld2");

		// SPECULAR COLOR TOKEN
		Ls2_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ls2");

		// LIGHT POSITION TOKEN
		light_position2_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_position2");


		// LIGHT rotation TOKEN
		light_rotation2_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_rotation2");





		//AMBIENT COLOR TOKEN
		La3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_La3");

		// DIFUSE COLOR TOKEN
		Ld3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ld3");

		// SPECULAR COLOR TOKEN
		Ls3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ls3");

		// LIGHT POSITION TOKEN
		light_position3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_position3");

		// LIGHT rotation TOKEN
		light_rotation3_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_light_rotation3");
		


		// AMBIENT REFLECTIVE TOKEN
		Ka_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ka");

		// DIFUUSE REFLECTIVE TOKEN
		Kd_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Kd");

		// SPECULAR REFLECTIVE TOKEN
		Ks_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_Ks");

		//SHINYNESS COLOR TOKEN
		material_shininess_uniform = glGetUniformLocation(gShaderProgramObject_fragmentShader, "u_material_shininess");
		
		glUseProgram(gShaderProgramObject_fragmentShader);
	}


	/*DRAWING SPHERE*/
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 rotateMatrix = mat4::identity();



	modelMatrix = translate(0.0f, 0.0f, -2.0f);


	
	// FILL DATA TO SHADERS

	if (gbLight == true)
	{	
		glUniform1i(L_KeyPressed_uniform, 1);
		// set 'u_lighting_enabled' uniform
		
		// setting light's properties
		rotateMatrix = vmath::rotate(angleRed, 1.0f, 0.0f, 0.0f);
		light1_position[1] = angleRed;

		glUniform3fv(La1_uniform, 1, light1_ambient);
		glUniform3fv(Ld1_uniform, 1, light1_difuse);
		glUniform3fv(Ls1_uniform, 1, light1_specular);
		glUniform4fv(light_position1_uniform, 1, light1_position);
		glUniformMatrix4fv(light_rotation1_uniform, 1, GL_FALSE,rotateMatrix);


		// setting light's properties
		rotateMatrix = mat4::identity();
		rotateMatrix = vmath::rotate(angleGreen, 0.0f, 1.0f, 0.0f);
		light2_position[0] = angleGreen;

		glUniform3fv(La2_uniform, 1, light2_ambient);
		glUniform3fv(Ld2_uniform, 1, light2_difuse);
		glUniform3fv(Ls2_uniform, 1, light2_specular);
		glUniform4fv(light_position2_uniform, 1, light2_position);
		glUniformMatrix4fv(light_rotation2_uniform, 1, GL_FALSE, rotateMatrix);


		// setting light's properties
		rotateMatrix = mat4::identity();
		rotateMatrix = vmath::rotate(angleBlue, 0.0f, 0.0f, 1.0f);
		light3_position[0] = angleBlue;

		glUniform3fv(La3_uniform, 1, light3_ambient);
		glUniform3fv(Ld3_uniform, 1, light3_difuse);
		glUniform3fv(Ls3_uniform, 1, light3_specular);
		glUniform4fv(light_position3_uniform, 1, light3_position);
		glUniformMatrix4fv(light_rotation3_uniform, 1, GL_FALSE,rotateMatrix);



		// setting material's properties
		glUniform3fv(Ka_uniform, 1, material_ambient);
		glUniform3fv(Kd_uniform, 1, material_difuse);
		glUniform3fv(Ks_uniform, 1, material_specular);
		glUniform1f(material_shininess_uniform, material_shinyness);
	}
	else
	{
		// set 'u_lighting_enabled' uniform
		glUniform1i(L_KeyPressed_uniform, 0);
	}


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
	if (gVbo_sphere_normal)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}
	if (gVbo_sphere_position)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}
	if (gVbo_sphere_element)
	{
		glDeleteVertexArrays(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}


	//DETACH THE FRAGMENT SHADER OBJECT
	glDetachShader(gShaderProgramObject_vertexShader, gFragmentShaderObject_vertexShader);

	//DETACH THE VERTEX SHADER OBJECT
	glDetachShader(gShaderProgramObject_vertexShader, gVertexShaderObject_vertexShader);


	//DELETE VERTEX SHADER OBJECT
	glDeleteShader(gVertexShaderObject_vertexShader);
	gVertexShaderObject_vertexShader = 0;

	//DELETE FRAGMENT SHADER OBJECT
	glDeleteShader(gFragmentShaderObject_vertexShader);
	gFragmentShaderObject_vertexShader = 0;

	//DELETE SHADER PROGRAM OBJECT
	glDeleteProgram(gShaderProgramObject_vertexShader);
	gShaderProgramObject_vertexShader = 0;



	//DETACH THE FRAGMENT SHADER OBJECT
	glDetachShader(gShaderProgramObject_fragmentShader, gFragmentShaderObject_fragmentShader);

	//DETACH THE VERTEX SHADER OBJECT
	glDetachShader(gShaderProgramObject_fragmentShader, gVertexShaderObject_fragmentShader);


	//DELETE VERTEX SHADER OBJECT
	glDeleteShader(gVertexShaderObject_fragmentShader);
	gVertexShaderObject_fragmentShader = 0;

	//DELETE FRAGMENT SHADER OBJECT
	glDeleteShader(gFragmentShaderObject_fragmentShader);
	gFragmentShaderObject_fragmentShader = 0;

	//DELETE SHADER PROGRAM OBJECT
	glDeleteProgram(gShaderProgramObject_fragmentShader);
	gShaderProgramObject_fragmentShader = 0;

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
		
		case 81:
			gbIsQKeyPressed = true;
			break;


		case VK_ESCAPE:
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

		case 70: //for f or F

			if (bIsFkeyPressed == false)
			{
				bIsFkeyPressed = true;
				bIsVkeyPressed = false;
			}
			break;

		case 86: //for V or v

			if (bIsVkeyPressed == false)
			{				
				bIsVkeyPressed = true;
				bIsFkeyPressed = false;
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
