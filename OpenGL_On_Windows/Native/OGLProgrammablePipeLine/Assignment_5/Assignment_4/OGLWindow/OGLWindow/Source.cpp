#include <Windows.h>
#include<stdio.h>
#include <gl\glew.h>

#include<gl/GL.h>
#include<Math.h>

#include "vmath.h"


#pragma comment(lib,"glew32.lib")
#pragma comment(lib, "opengl32.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT  600


#define TEX_WIDTH 64
#define TEX_HEIGHT 64

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


GLuint gVao_rectangle_position;
GLuint gVao_rectangle_texture;

GLuint gVbo_position;
GLuint gVbo_texture;

GLuint gMVPUniform;

mat4 gPerspectiveProjectionMatrix;


GLuint gTexture_sampler_function;


GLuint gTexture_white;
GLubyte gCheckboard_pixels[TEX_WIDTH][TEX_HEIGHT][4];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void IntializeOpenGL(HWND, HDC*, HGLRC*);

	void Update(void);

	void Render(HDC hdc);

	void UnIntializeOpenGL(HWND, HDC*, HGLRC*);



	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szAppName[] = TEXT("Create CheckBoard texture using Procedural Textrure in Perspective (Single Vao)");
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

	return msg.wParam;
}

void Update()
{
	
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
		"in vec2 vTexture0_Coord;"\
		"out vec2 out_texture0_coord;"\
		"uniform mat4 u_mvp_matrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_texture0_coord = vTexture0_Coord;"\
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
		"\n"\
		"in vec2 out_texture0_coord;"\
		"out vec4 FragColor;"\
		"uniform sampler2D u_texture0_sampler;"\
		"void main(void)"\
		"{"\
		"FragColor = texture(u_texture0_sampler,out_texture0_coord);"\
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
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

	//Get Sampler Function helper
	gTexture_sampler_function = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");

	//VERTICES , COLORS ,SHADERS ATTRIBS ,VBO ,VAO INITIALIZATION


	/*Rectangle*/
	glGenVertexArrays(1, &gVao_rectangle_position);
	glBindVertexArray(gVao_rectangle_position);

	/*position*/
	glGenBuffers(1, &gVbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_position);

	glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/

	/*texture*/
	glGenBuffers(1, &gVbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_texture);

	glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2/*s,t*/, GL_FLOAT, GL_FALSE, 0, NULL);
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
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE); /*Turning oFF cull , as we are doing animation , and so need to paint the back of object when rotating*/

	//LOAD TEXTURES 
	CreateWhiteTexture();
	LoadGLTextureFromProcedureTexture();
	//

	gPerspectiveProjectionMatrix = mat4::identity();
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void CreateWhiteTexture()
{
	int i = 0, j = 0, c = 0;

	for (i = 0; i < TEX_HEIGHT; i++)
	{
		for (j = 0; j < TEX_WIDTH; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

			gCheckboard_pixels[i][j][0] = (GLubyte)c;
			gCheckboard_pixels[i][j][1] = (GLubyte)c;
			gCheckboard_pixels[i][j][2] = (GLubyte)c;
			gCheckboard_pixels[i][j][3] = (GLubyte)255;
		}
	}
}

void LoadGLTextureFromProcedureTexture()
{

	glGenTextures(1, &gTexture_white);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, gTexture_white);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0/*LOD*/, GL_RGBA, TEX_WIDTH, TEX_HEIGHT, 0/*border width*/, GL_RGBA, GL_UNSIGNED_BYTE, gCheckboard_pixels);

	glGenerateMipmap(GL_TEXTURE_2D);

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


	mat4  modelviewmatrix = mat4::identity();
	mat4 modelviewPorojectionMatrix = mat4::identity();
	mat4 rotateMatrix = mat4::identity();

	/*DRAWING RECTANGLE*/ 

	modelviewmatrix = vmath::translate(0.0f, 0.0f, -3.6f);
	modelviewPorojectionMatrix = gPerspectiveProjectionMatrix * modelviewmatrix;

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewPorojectionMatrix);



		const GLfloat rectangleVertices_front[] =
		{
			0.0f, 1.0f, 0.0,
			-2.0f, 1.0f, 0.0f,
			-2.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0
		};
		const GLfloat rectangleCTexcocords_front[] =
		{
			1.0f, 1.0f, 
			0.0f, 1.0f, 
			0.0f, 0.0f, 
			1.0f, 0.0f
		};
	
	
	/*Fill the Vertices*/
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices_front), &rectangleVertices_front, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3/*x,y,z*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/

	/*Fill Tex coord*/
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_texture);

	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleCTexcocords_front), &rectangleCTexcocords_front, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2/*s,t*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	//*BINDE With Rectangle Cordinates*//
	glBindVertexArray(gVao_rectangle_position);

	//Bind with Rectangle Texture
	 glActiveTexture(GL_TEXTURE0); // corresponds to VDG_ATTRIBUTE_TEXTURE0
	 glBindTexture(GL_TEXTURE_2D, gTexture_white);
	 glUniform1i(gTexture_sampler_function, 0);
	//
	
	 glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	/*END OF DRAWING TRIANGLE*/



	const GLfloat rectangleVertices_deviated[] =
	{
		2.41421f, 1.0f, -1.41421f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		2.41421f, -1.0f, -1.41421f
	};
	const GLfloat rectangleCTexcocords_deviated[] =
	{
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
	};



	/*Fill the Vertices*/
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices_deviated), &rectangleVertices_deviated, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3/*x,y,z*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/

	/*Fill Tex coord*/
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_texture);

	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleCTexcocords_deviated), &rectangleCTexcocords_deviated, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2/*s,t*/, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/**/


	//*BINDE With Rectangle Cordinates*//
	glBindVertexArray(gVao_rectangle_position);

	//Bind with Rectangle Texture
	glActiveTexture(GL_TEXTURE0); // corresponds to VDG_ATTRIBUTE_TEXTURE0
	glBindTexture(GL_TEXTURE_2D, gTexture_white);
	glUniform1i(gTexture_sampler_function, 0);
	//
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	glBindTexture(GL_TEXTURE_2D, 0);
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
	if (gVao_rectangle_position)
	{
		glDeleteVertexArrays(1, &gVao_rectangle_position);
		gVao_rectangle_position = 0;
	}
	if (gVao_rectangle_texture)
	{
		glDeleteVertexArrays(1, &gVao_rectangle_position);
		gVao_rectangle_position = 0;
	}


	//DESTROY VBO
	if (gVbo_position)
	{
		glDeleteVertexArrays(1, &gVbo_position);
		gVbo_position = 0;
	}
	if (gVbo_texture)
	{
		glDeleteVertexArrays(1, &gVbo_texture);
		gVbo_texture = 0;
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

	gPerspectiveProjectionMatrix = vmath::perspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 30.0f);

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
