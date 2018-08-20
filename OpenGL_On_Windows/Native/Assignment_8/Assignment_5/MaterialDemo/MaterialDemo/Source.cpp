#include <Windows.h>
#include<gl/GL.h>
#include<Math.h>
#include<GL/glu.h>

#pragma comment(lib,"glu32.lib")
#pragma comment(lib, "opengl32.lib")
#define WIN_WIDTH  800
#define WIN_HEIGHT  600

bool gbIsGameDone = false;
bool gbIsFullScreen = false;
bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;

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

LONG glPreviousStyle;
WINDOWPLACEMENT  gwndplacement;
MONITORINFO mi;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{


	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void IntializeOpenGL(HWND, HDC*, HGLRC*);

	void Update();

	void Render(HDC hdc);

	void UnIntializeOpenGL(HWND, HDC*, HGLRC*);



	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szAppName[] = TEXT("Material Demo (Lights)");
	HDC hdc = NULL;
	HGLRC hglrc = NULL;

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
	if (angle >= 360.0f) 
		angle = 0.0f;
	else
		angle = angle + 0.1f;
}

void IntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{
	void Resize(int, int);
	void InitializeMaterials();

	InitializeMaterials();

	PIXELFORMATDESCRIPTOR pfd;
	RECT rect;

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
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

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Render(HDC hdc)
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

	SwapBuffers(hdc);
}
void UnIntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{

	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			gluDeleteQuadric(material[i][j].quadric);
			material[i][j].quadric = NULL;
		}
	}

	wglDeleteContext(*hglrc);
	*hglrc = NULL;

	ReleaseDC(hwnd, *hdc);
	*hdc = NULL;

	DestroyWindow(hwnd);
	hwnd = NULL;

	PostQuitMessage(0);

	return;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	void Resize(int, int);

	void ToggleScreen(HWND);


	void Resize(int, int);

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

		case 76:
			if (gIsLKeyPressed == GL_TRUE)
			{
				glDisable(GL_LIGHTING);
				gIsLKeyPressed = GL_FALSE;
			}
			else
			{
				glEnable(GL_LIGHTING);
				gIsLKeyPressed = GL_TRUE;

			}
			break;

		case 88:
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


		case 89:
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

		case 90:
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

		case VK_ESCAPE: gbIsEscapeKeyPressed = true;
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
	if (height == 0) height = 1;


	glViewport(0, 0, GLsizei(width), GLsizei(height));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

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