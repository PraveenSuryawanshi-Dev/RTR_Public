#include <Windows.h>
#include<gl/GL.h>
#include<Math.h>
#pragma comment(lib, "opengl32.lib")
#define WIN_WIDTH 500
#define WIN_HEIGHT 500
#define PI 3.1415926535898

bool gbIsGameDone = false;
bool gbIsFullScreen = false;
bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;

LONG glPreviousStyle;
WINDOWPLACEMENT  gwndplacement;
MONITORINFO mi;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void IntializeOpenGL(HWND, HDC*, HGLRC*);

	void Render(HDC hdc);

	void UnIntializeOpenGL(HWND, HDC*, HGLRC*);



	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szAppName[] = TEXT("DEADLY HOLLOW APPLCATION USING OPENGL");
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

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Render(HDC hdc)
{
	void DrawDeadlyHollow();

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DrawDeadlyHollow();

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


void DrawDeadlyHollow()
{
	void DrawTriangle(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
	void DrawMedian(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
	GLfloat CalculateRadius(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
	void DrawCircle(GLfloat, GLfloat);
	void DrawPoint(GLfloat center, GLfloat pointSize);


	GLfloat x1 = 0.0f;
	GLfloat x2 = -0.8f;
	GLfloat x3 = 0.8f;
	GLfloat y1 = 0.8f;
	GLfloat y2 = -0.8f;
	GLfloat y3 = -0.8f;


	GLfloat radius;
	GLfloat Ycordinate; // y coordinate of median of base

	glColor3f(1.0f, 1.0f, 1.0f);
	DrawTriangle(x1, y1, x2, y2, x3, y3);
	DrawMedian(x1, y1, x2, y2, x3, y3);
	radius = CalculateRadius(x1, y1, x2, y2, x3, y3);
	Ycordinate = (y2 + y3) / 2.0f;
	DrawCircle(radius, Ycordinate);
	DrawPoint(Ycordinate+radius, 5.0f);
}
void DrawPoint(GLfloat center,GLfloat pointSize)
{
	glPointSize(pointSize);
	glBegin(GL_POINTS);
	glVertex3f(0.0f, center, 0.0f);
	glEnd();
}
GLfloat CalculateRadius(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3)
{
	GLfloat sideA;
	GLfloat sideB;
	GLfloat radius;

	sideA = sqrt(pow((x3 - x1), 2) + pow((y3 - y1), 2));
	sideB = sqrt(pow((x3 - x2), 2) + pow((y3 - y2), 2));

	radius = sideB * (sqrt((2 * sideA - sideB) / (2 * sideA + sideB))) / 2;

	return radius;
}
void DrawCircle(GLfloat radius, GLfloat ym)
{
	float angle;
	GLint CirclePoints = 1000;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < CirclePoints; i++)
	{
		angle = 2 * PI * i / CirclePoints;
		glVertex3f((radius * cos(angle) + 0.0f), (radius * sin(angle) + (radius + ym)), 0.0f);
	}
	glEnd();
}
void DrawMedian(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(x1, y1, 0.0f);
	glVertex3f((x2 + x3) / 2.0f, (y2 + y3) / 2.0f, 0.0f);
	glEnd();
}
void DrawTriangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(x1, y1, 0.0f);
	glVertex3f(x2, y2, 0.0f);
	glVertex3f(x2, y2, 0.0f);
	glVertex3f(x3, y3, 0.0f);
	glVertex3f(x3, y3, 0.0f);
	glVertex3f(x1, y1, 0.0f);
	glEnd();
}