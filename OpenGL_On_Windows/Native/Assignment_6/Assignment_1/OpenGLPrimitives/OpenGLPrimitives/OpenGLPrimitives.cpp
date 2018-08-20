#include <Windows.h>
#include<gl/GL.h>
#include<Math.h>
#include<stdio.h>
#include<GL/glu.h>

#pragma comment(lib,"glu32.lib")
#pragma comment(lib, "opengl32.lib")
#define WIN_WIDTH  800
#define WIN_HEIGHT  600

bool gbIsGameDone = false;
bool gbIsFullScreen = false;
bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;

float anglePyramid = 0.0f;

LONG glPreviousStyle;
WINDOWPLACEMENT  gwndplacement;
MONITORINFO mi;

typedef struct u_Points
{
	GLfloat x;
	GLfloat y;

}U_POINTS;
U_POINTS Points[16];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	void IntializeOpenGL(HWND, HDC*, HGLRC*);

	void Render(HDC hdc);

	void UnIntializeOpenGL(HWND, HDC*, HGLRC*);



	MSG msg;
	WNDCLASSEX wndclassex;
	TCHAR szAppName[] = TEXT("OPENGL PRIMITIVES APPLCATION USING OPENGL");
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

	Points[0].x = -0.3f; Points[0].y = 0.3f;
	Points[1].x = -0.1f; Points[1].y = 0.3f;
	Points[2].x = 0.1f; Points[2].y = 0.3f;
	Points[3].x = 0.3f; Points[3].y = 0.3f;

	Points[4].x = -0.3f; Points[4].y = 0.1f;
	Points[5].x = -0.1f; Points[5].y = 0.1f;
	Points[6].x = 0.1f; Points[6].y = 0.1f;
	Points[7].x = 0.3f; Points[7].y = 0.1f;

	Points[8].x = -0.3f; Points[8].y = -0.1f;
	Points[9].x = -0.1f; Points[9].y = -0.1f;
	Points[10].x = 0.1f; Points[10].y = -0.1f;
	Points[11].x = 0.3f; Points[11].y = -0.1f;

	Points[12].x = -0.3f; Points[12].y = -0.3f;
	Points[13].x = -0.1f; Points[13].y = -0.3f;
	Points[14].x = 0.1f; Points[14].y = -0.3f;
	Points[15].x = 0.3f; Points[15].y = -0.3f;
}

void Render(HDC hdc)
{

	void  DrawFirstFigure();
	void DrawSecondFigure();
	void DrawThirdFigure();
	void DrawFourthFigure();
	void  DrawFifthFigure();
	void DrawSixthFigure();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glPointSize(2.0f);
	
	glPushMatrix();

	/*1*/
	glTranslatef(-1.7f, 0.5f, -4.0f);
	DrawFirstFigure();
	/**/


	glPopMatrix();
	glPushMatrix();
	
	
	/*2*/
	glTranslatef(0.0f, 0.5f, -4.0f);
	DrawSecondFigure();
	/**/

	glPopMatrix();
	glPushMatrix();
	
	/*3*/
	glTranslatef(1.5f, 0.5f, -4.0f);
	DrawThirdFigure();
	/**/

	glPopMatrix();
	glPushMatrix();
	
	
	/*4*/
	glTranslatef(-1.7f, -0.5f, -4.0f);
	DrawFourthFigure();
	/**/
	

	glPopMatrix();
	glPushMatrix();
	
	
	
	/*5*/
	glTranslatef(0.0f, -0.5f, -4.0f);
	DrawFifthFigure();
	/**/

	glPopMatrix();

	

	glPopMatrix();
	
	/*6*/
	glTranslatef(1.5f, -0.5f, -4.0f);
	DrawSixthFigure();
	/**/


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


void  DrawFirstFigure()
{
	glBegin(GL_POINTS);
	for (auto i = 0; i < 16; i++)
	{
		glVertex3f(Points[i].x, Points[i].y, 0.0f);
	}
	glEnd();
}

void DrawSecondFigure()
{
	auto j = 3;
	glBegin(GL_LINES);
	for (auto i = 0; i < 11; i = i + 4)
	{
		glVertex3f(Points[i].x, Points[i].y, 0.0f);
		glVertex3f(Points[j].x, Points[j].y, 0.0f);
		j = j + 4;
	}
	j = 12;
	for (auto i = 0; i < 3; i++)
	{
		glVertex3f(Points[i].x, Points[i].y, 0.0f);
		glVertex3f(Points[j].x, Points[j].y, 0.0f);


		j = j++;
	}
	glEnd();

	glBegin(GL_LINES);
	for (auto i = 0; i < 13; i++)
	{
		if (i % 4 == 0)
			continue;

		glVertex3f(Points[i].x, Points[i].y, 0.0f);
		glVertex3f(Points[i + 3].x, Points[i + 3].y, 0.0f);
	}
	glEnd();
}


void DrawThirdFigure()
{
	int x1 = 0, x2 = 1, x3 = 5, x4 = 4;
	for (auto i = 0; i < 11; i++)
	{
		if (x1 != 3 && x1 != 7 && x1 != 11 && x1 != 15) {

			glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(Points[x1].x, Points[x1].y, 0.0f);
			glVertex3f(Points[x2].x, Points[x2].y, 0.0f);
			glVertex3f(Points[x3].x, Points[x3].y, 0.0f);
			glVertex3f(Points[x4].x, Points[x4].y, 0.0f);
			glEnd();


		}
		x1++;
		x2++;
		x3++;
		x4++;
	}
}


void DrawFourthFigure()
{
	auto j = 3;
	glBegin(GL_LINES);
	for (auto i = 0; i < 15; i = i + 4)
	{
		glVertex3f(Points[i].x, Points[i].y, 0.0f);
		glVertex3f(Points[j].x, Points[j].y, 0.0f);
		j = j + 4;
	}
	j = 12;
	for (auto i = 0; i < 4; i++)
	{
		glVertex3f(Points[i].x, Points[i].y, 0.0f);
		glVertex3f(Points[j].x, Points[j].y, 0.0f);


		j = j++;
	}
	glEnd();

	glBegin(GL_LINES);
	for (auto i = 0; i < 13; i++)
	{
		if (i % 4 == 0)
			continue;

		glVertex3f(Points[i].x, Points[i].y, 0.0f);
		glVertex3f(Points[i + 3].x, Points[i + 3].y, 0.0f);
	}
	glEnd();
}

void  DrawFifthFigure()
{
	int j = 12;
	 glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
	for (auto i = 0; i < 5; i++)
	{
		glVertex3f(Points[0].x, Points[0].y, 0.0f);
		glVertex3f(Points[j].x, Points[j].y, 0.0f);
		glVertex3f(Points[j+1].x, Points[j+1].y, 0.0f);
		j++;
	}
	j = 3;
	for (auto i = 0; i < 4; i++)
	{
		glVertex3f(Points[0].x, Points[0].y, 0.0f);
		glVertex3f(Points[j].x, Points[j].y, 0.0f);
		glVertex3f(Points[j + 4].x, Points[j + 4].y, 0.0f);
		j = j + 4;
	}
	glEnd();

}
void DrawSixthFigure()
{
	int x1 = 0, x2 = 1, x3 = 5, x4 = 4;
	for (auto i = 0; i < 11; i++)
	{
		if (x1 != 3 && x1 != 7 && x1 != 11 && x1 != 15) {

			glBegin(GL_POLYGON);
			if (x1 == 0 || x1 == 4 || x1 == 8 || x1 == 12)
				glColor3f(1.0f, 0.0f, 0.0f);

			if (x1 == 1 || x1 == 5 || x1 == 9 || x1 == 13)
				glColor3f(0.0f, 1.0f, 0.0f);

			if (x1 == 2 || x1 == 6 || x1 == 10 || x1 == 14)
				glColor3f(0.0f, 0.0f, 1.0f);

			glVertex3f(Points[x1].x, Points[x1].y, 0.0f);
			glVertex3f(Points[x2].x, Points[x2].y, 0.0f);
			glVertex3f(Points[x3].x, Points[x3].y, 0.0f);
			glVertex3f(Points[x4].x, Points[x4].y, 0.0f);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(Points[x1].x, Points[x1].y, 0.0f);
			glVertex3f(Points[x2].x, Points[x2].y, 0.0f);
			glVertex3f(Points[x3].x, Points[x3].y, 0.0f);
			glVertex3f(Points[x4].x, Points[x4].y, 0.0f);
			glEnd();


		}
		x1++;
		x2++;
		x3++;
		x4++;
	}
}
