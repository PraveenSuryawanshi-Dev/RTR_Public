#include <Windows.h>
#include<gl/GL.h>
#include<Math.h>
#include<stdio.h>
#include<GL/glu.h>

#pragma comment(lib,"glu32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"Winmm.lib")

GLfloat SPEED = 0.0003f;

#define WIN_WIDTH  800
#define WIN_HEIGHT  600


bool _IsGameDone = false;
bool _IsFullScreen = false;
bool _IsActiveWindow = false;
bool _IsEscapeKeyPressed = false;



LONG _PreviousStyle;
WINDOWPLACEMENT  _wndplacement;
MONITORINFO _monitorInfo;

/*
Official Color codes of Indian Flag :
Saffron: (RGB: 255, 153, 51) (hex code: #FF9933)
White: (RGB: 255, 255, 255) (hex code: #FFFFFF
Green: (RGB: 19, 136, 8) (hex code: #138808)
*/

GLfloat i_MoveLeftToRight= -3.0f;
GLfloat n_MoveTopToDown = 3.0f;
GLfloat i_MoveBottomUp = -3.0f;
GLfloat a_MoveRightToLeft = 3.1f;


GLfloat safron_R = 0.0f;
GLfloat safron_G = 0.0f;
GLfloat safron_B = 0.0f;
GLfloat green_R = 0.0f;
GLfloat green_G = 0.0f;
GLfloat green_B = 0.0f;

GLfloat  i = -3.0f, j = -3.0f, k = -3.0f;
GLfloat l = -3.0f, m = -3.0f, n = -3.0f;

bool isLeft_i_RenderingDone = false;
bool is_n_RenderingDone = false;
bool is_d_RenderingDone = false;
bool isRight_i_RenderingDone = false;
bool is_a_RenderingDone = false;
bool is_FlagDone_RenderingDone = false;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void IntializeOpenGL(HWND, HDC*, HGLRC*);

	void Update();

	void Render(HDC hdc);

	void UnIntializeOpenGL(HWND, HDC*, HGLRC*);



	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szAppName[] = TEXT("DYNAMIC INDIA APPLCATION USING OPENGL");
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


	while (_IsGameDone == false)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				_IsGameDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}
		else
		{
			if (_IsActiveWindow == true)
			{
				if (_IsEscapeKeyPressed == true)
					_IsGameDone = true;

				Update();
				Render(hdc);
			}
		}


	}

	UnIntializeOpenGL(hWnd, &hdc, &hglrc);

	return msg.wParam;
}
void IntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{
	void Resize(int, int);

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

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Render(HDC hdc)
{
	void DRAW_I();
	void DRAW_N();
	void DRAW_D();
	void DRAW_A();
	void DrawIndianFlag();


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLineWidth(5.0f);

	glPushMatrix();

	glTranslatef(i_MoveLeftToRight, 0.0f, -3.5f);
	DRAW_I();

	glPopMatrix();
	glPushMatrix();

	glTranslatef(-1.2f, n_MoveTopToDown, -3.5f);
	DRAW_N();

	glPopMatrix();
	glPushMatrix();

	glTranslatef(-0.5f, 0.0f, -3.5f);
	DRAW_D();

	glPopMatrix();
	glPushMatrix();

	glTranslatef(0.2f, i_MoveBottomUp, -3.5f);
	DRAW_I();

	glPopMatrix();
	glPushMatrix();

	glTranslatef(a_MoveRightToLeft, 0.0f, -3.5f);
	DRAW_A();

	glPopMatrix();

	glTranslatef(-2.0f, 0.0f, -3.5f);
	DrawIndianFlag();

	SwapBuffers(hdc);

}

void Update()
{
	if (i_MoveLeftToRight <= -1.5f)
		i_MoveLeftToRight = i_MoveLeftToRight + SPEED;
	else
		isLeft_i_RenderingDone = true;

	if (isLeft_i_RenderingDone)
	{
		if (n_MoveTopToDown >= 0.0f)
			n_MoveTopToDown = n_MoveTopToDown - SPEED;
		else
			is_n_RenderingDone = true;
	}

	if (isLeft_i_RenderingDone && is_n_RenderingDone)
	{
		if (safron_R <= 255.0f)
			safron_R = safron_R + 0.05f;
		else
			is_d_RenderingDone = true;

		if (safron_G <= 153.0f)
			safron_G = safron_G + 0.05f;

		if (safron_B <= 51.0f)
			safron_B = safron_B + 0.05f;

		if (green_R <= 19.0f)
			green_R = safron_R + 0.05f;

		if (green_G <= 136.0f)
			green_G = safron_G + 0.05f;

		if (green_B <= 255.0f)
			green_B = safron_B + 0.05f;
	}
	
	if (isLeft_i_RenderingDone && is_n_RenderingDone && is_d_RenderingDone)
	{
		if (i_MoveBottomUp <= 0.0f)
			i_MoveBottomUp = i_MoveBottomUp + SPEED;
		else
			isRight_i_RenderingDone = true;
	}
		
	if (isLeft_i_RenderingDone && is_n_RenderingDone && is_d_RenderingDone && isRight_i_RenderingDone)
	{
		if (a_MoveRightToLeft >= 0.9f)
			a_MoveRightToLeft = a_MoveRightToLeft - SPEED;
		else
			is_a_RenderingDone = true;
	}

	if (isLeft_i_RenderingDone && is_n_RenderingDone && is_d_RenderingDone && isRight_i_RenderingDone && is_a_RenderingDone)
	{
		if (i <= 3.15f)
			i = i + SPEED;
		else
			is_FlagDone_RenderingDone = true;

		if (j <= 3.15f)
			j = j + SPEED;

		if (k <= 3.15f)
			k = k + SPEED;
	}

	if (isLeft_i_RenderingDone && is_n_RenderingDone && is_d_RenderingDone && isRight_i_RenderingDone && is_a_RenderingDone && is_FlagDone_RenderingDone)
	{
		if (l <= 2.63f)
			l = l + SPEED;
		
		if (m <= 2.63f)
			m = m + SPEED;
	
		if (n <= 2.63f)
			n = n + SPEED;
	}
}

void UnIntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{
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

	case WM_CREATE:
		PlaySound(TEXT("music.wav"), NULL, SND_LOOP | SND_ASYNC | SND_FILENAME);
		break;

	case   WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			_IsActiveWindow = true;
		else
			_IsActiveWindow = false;
		break;

	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;


	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE: _IsEscapeKeyPressed = true;
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
	if (_IsFullScreen)
	{
		SetWindowLong(hwnd, GWL_STYLE, _PreviousStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &_wndplacement);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		_IsFullScreen = false;
		return;
	}

	_PreviousStyle = GetWindowLong(hwnd, GWL_STYLE);

	if (_PreviousStyle & WS_OVERLAPPEDWINDOW)
	{
		_monitorInfo = { sizeof(MONITORINFO) };

		if (GetWindowPlacement(hwnd, &_wndplacement) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITORINFOF_PRIMARY), &_monitorInfo))
		{
			SetWindowLong(hwnd, GWL_STYLE, _PreviousStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP, _monitorInfo.rcMonitor.left, _monitorInfo.rcMonitor.top, _monitorInfo.rcMonitor.right - _monitorInfo.rcMonitor.left, _monitorInfo.rcMonitor.bottom - _monitorInfo.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		}

	}
	ShowCursor(FALSE);
	_IsFullScreen = true;

}

void DRAW_I()
{
	glBegin(GL_LINES);
	glColor3f(255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f); //safron
	glVertex3f(0.0, 0.8, 0.0f);

	glColor3f(19.0f / 255.0f, 136.0f / 255.0f, 8.0f / 255.0f); //green
	glVertex3f(0.0f, -0.8, 0.0f);
	glEnd();

}
void DRAW_N()
{
	glBegin(GL_LINES);
	glColor3f(255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f); //safron
	glVertex3f(0.0f, 0.8f, 0.0f);

	glColor3f(19.0f / 255.0f, 136.0f / 255.0f, 8.0f / 255.0f); //green
	glVertex3f(0.0f, -0.8f, 0.0f);
	glEnd();


	glBegin(GL_LINES);
	glColor3f(255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f); //safron
	glVertex3f(0.0f, 0.8f, 0.0f);

	glColor3f(19.0f / 255.0f, 136.0f / 255.0f, 8.0f / 255.0f); //green
	glVertex3f(0.5f, -0.8f, 0.0f);
	glEnd();


	glBegin(GL_LINES);
	glColor3f(19.0f / 255.0f, 136.0f / 255.0f, 8.0f / 255.0f); //green
	glVertex3f(0.5f, -0.8f, 0.0f);

	glColor3f(255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f); //safron
	glVertex3f(0.5f, 0.8f, 0.0f);
	glEnd();

}

void DRAW_D()
{

	glBegin(GL_LINES);
	glColor3f((safron_R / 255.0f), (safron_G / 255.0f), (safron_B/ 255.0f)); //safron
	glVertex3f(0.0f, 0.8f, 0.0f);

	glColor3f((green_R / 255.0f), (green_G / 255.0f), (green_B / 255.0f)); //green
	glVertex3f(0.0f, -0.8f, 0.0f);
	glEnd();

	

	glBegin(GL_LINES);
	glColor3f((safron_R / 255.0f), (safron_G / 255.0f), (safron_B / 255.0f)); //safron
	glVertex3f(-0.05f, 0.8f, 0.0f);

	glColor3f((safron_R / 255.0f), (safron_G / 255.0f), (safron_B / 255.0f)); //safron
	glVertex3f(0.51f, 0.8f, 0.0f);
	glEnd();


	glBegin(GL_LINES);
	glColor3f((green_R / 255.0f), (green_G / 255.0f), (green_B / 255.0f)); //green
	glVertex3f(-0.05f, -0.8f, 0.0f);

	glColor3f((green_R / 255.0f), (green_G / 255.0f),( green_B / 255.0f)); //green
	glVertex3f(0.51f, -0.8f, 0.0f);
	glEnd();


	glBegin(GL_LINES);
	glColor3f((safron_R / 255.0f), (safron_G / 255.0f), (safron_B / 255.0f)); //safron
	glVertex3f(0.5f, 0.8f, 0.0f);

	glColor3f((green_R / 255.0f), (green_G / 255.0f), (green_B / 255.0f)); //green
	glVertex3f(0.5f, -0.8f, 0.0f);
	glEnd();


}
void DRAW_A()
{
	glBegin(GL_LINES);
	glColor3f(255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f); //safron
	glVertex3f(0.0f, 0.8f, 0.0f);
	glColor3f(19.0f / 255.0f, 136.0f / 255.0f, 8.0f / 255.0f); //green
	glVertex3f(-0.5, -0.8, 0.f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f); //safron
	glVertex3f(0.0f, 0.8f, 0.0f);
	glColor3f(19.0f / 255.0f, 136.0f / 255.0f, 8.0f / 255.0f); //green
	glVertex3f(0.5, -0.8, 0.f);
	glEnd();

}


void DrawIndianFlag()
{
	glBegin(GL_LINES);
	glColor3f(255.0f / 255.0f, 153.0f / 255.0f, 51.0f / 255.0f); //safron
	glVertex3f(l, -0.01f, 0.0f);
	glVertex3f(i, -0.01f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f); //white
	glVertex3f(m, -0.02f, 0.0f);
	glVertex3f(j, -0.02f, 0.0f);

	glColor3f(19.0f / 255.0f, 136.0f / 255.0f, 8.0f / 255.0f); //green
	glVertex3f(n, -0.03f, 0.0f);
	glVertex3f(k, -0.03f, 0.0f);
	glEnd();


}