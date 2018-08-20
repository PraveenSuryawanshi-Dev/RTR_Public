#include <Windows.h>
#include<gl/GL.h>
#include<Math.h>
#pragma comment(lib, "opengl32.lib")
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.1415926535898


bool gbIsGameDone = false;
bool gbIsFullScreen = false;

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
	TCHAR szAppName[] = TEXT("MULTI-COLOR CONCENTRIC TRIANGLES APPLCATION USING OPENGL");
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
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			Render(hdc);
		}


	}

	UnIntializeOpenGL(hWnd, &hdc, &hglrc);

	return msg.wParam;
}
void IntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{

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

	SwapBuffers(*hdc);
}

void Render(HDC hdc)
{
	void DrawTriangle(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat x1 = 0.0f;
	GLfloat y1 = 0.9f;
	GLfloat x2 = -0.9f;
	GLfloat y2 = -0.9f;
	GLfloat  x3 = 0.9f;
	GLfloat y3 = -0.9f;

	GLfloat color[10][3] = {
		{ 1.0f, 0.0f, 0.0f },//red
		{ 0.0f, 1.0f, 0.0f },//green
		{ 0.0f, 0.0f,1.0f },//blue

		{ 0.0f, 1.0f, 1.0f },//cyan
		{ 1.0f, 0.0f, 1.0f },//magenta
		{ 1.0f, 1.0f, 0.0f },//yellow

		{ 1.0f, 1.0f, 1.0f },//white
		{ (255.0f / 255.0f), (16.0f / 255.0f), (0.0f / 255.0f) },//orange
		{ 0.5f, 0.5f, 0.5f },//gray

		{ (165.0f / 255.0f), (42.0f / 255.0f), (42.0f / 255.0f) }//brown

	};

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i = 0; i <=11; i++)
	{
		glColor3f(color[i][0], color[i][1], color[i][2]);
		DrawTriangle(x1,y1,x2,y2,x3,y3);

		if (y1 > 0)
		{

		y1 = y1 - 0.09f;

		x2 = x2 + 0.09f;
		y2 = y2 + 0.09f;

		x3 = x3 - 0.09f;
		y3 = y3 + 0.09f;
		}

		if (y1 <= 0)
		{
			y1 = y1 + 0.09f;

			x2 = x2 - 0.09f;
			y2 = y2 - 0.09f;

			x3 = x3 + 0.09f;
			y3 = y3 - 0.09f;
		}

		

	}
	SwapBuffers(hdc);

}
void UnIntializeOpenGL(HWND hwnd, HDC* hdc, HGLRC* hglrc)
{


	wglDeleteContext(*hglrc);
	*hglrc = NULL;

	ReleaseDC(hwnd, *hdc);
	*hdc = NULL;

	DestroyWindow(hwnd);

	PostQuitMessage(0);

	return;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	void Resize(int, int);

	void ToggleScreen(HWND);


	switch (message)
	{

	case WM_DESTROY:
		gbIsGameDone = true;
		break;




	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;


	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE: gbIsGameDone = true;
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


void DrawTriangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3)
{
	glBegin(GL_LINES);
	glVertex3f(x1, y1, 0.0f);
	glVertex3f(x2, y2, 0.0f);
	glVertex3f(x2, y2, 0.0f);
	glVertex3f(x3, y3, 0.0f);
	glVertex3f(x3, y3, 0.0f);
	glVertex3f(x1, y1, 0.0f);
	glEnd();
}