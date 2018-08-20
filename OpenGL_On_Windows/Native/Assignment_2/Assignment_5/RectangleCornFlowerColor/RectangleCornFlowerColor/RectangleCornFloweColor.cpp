
#include<Windows.h>
#include<gl/GL.h>
#include <winuser.h>
#pragma comment(lib,"opengl32.lib")


#define WIN_WIDTH 800
#define WIN_HEIGHT 600




bool gbIsGameDone = false;
bool gbIsFullScreen = false;
bool gbIsActiveWindow = false;
bool gbIsEscapeKeyPressed = false;

WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
DWORD dwStyle;



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void ToggleScreen(HWND);

void IntializeOpenGL(HWND, HDC *, HGLRC *);

void UnIntializeOpenGL(HWND, HDC *, HGLRC *);

void Render(HDC);

void Resize(int, int);








int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iWndShow)
{
	MSG msg;
	TCHAR szAppName[] = TEXT("RECTANGLE CORN FLOWER WINDOW APPLICATION");
	WNDCLASSEX wce;

	HGLRC hglrc = NULL;
	HDC hdc = NULL;


	wce.cbSize = sizeof(WNDCLASSEX);
	wce.cbWndExtra = 0;
	wce.cbClsExtra = 0;
	wce.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wce.lpszClassName = szAppName;
	wce.hInstance = hInstance;
	wce.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wce.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wce.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wce.cbWndExtra = 0;
	wce.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wce.lpfnWndProc = WndProc;
	wce.lpszMenuName = NULL;

	if (FAILED(RegisterClassEx(&wce)))
	{
		MessageBox(NULL, TEXT("FAILED TO REGISTER WNDCLASSEX"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return 0;
	}

	HWND hWnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		szAppName,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);


	ShowWindow(hWnd, SW_SHOW);

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



void IntializeOpenGL(HWND hwnd, HDC *hdc, HGLRC *hglrc)
{

	PIXELFORMATDESCRIPTOR pfd;

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	*hdc = GetDC(hwnd);


	int indexPixelFormatDescriptor = ChoosePixelFormat(*hdc, &pfd);

	if (indexPixelFormatDescriptor == 0)
	{
		ReleaseDC(hwnd, *hdc);
		hdc = NULL;
		MessageBox(hwnd, TEXT("ERROR GETTING PIXEL FORMAT DESCRIPTOR"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return;
	}

	if (SetPixelFormat(*hdc, indexPixelFormatDescriptor, &pfd) == FALSE)
	{
		ReleaseDC(hwnd, *hdc);
		hdc = NULL;
		MessageBox(hwnd, TEXT("ERROR SETTING PIXEL FORMAT DESCRIPTOR"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return;
	}

	*hglrc = wglCreateContext(*hdc);

	if (hglrc == NULL)
	{
		wglDeleteContext(*hglrc);
		ReleaseDC(hwnd, *hdc);
		hdc = NULL;
		hglrc = NULL;
		MessageBox(hwnd, TEXT("ERROR IN CREATING HANDLE TO GRAPHICS LIBRARY RENDERING CONTEXT"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return;
	}

	if (wglMakeCurrent(*hdc, *hglrc) == FALSE)
	{
		wglDeleteContext(*hglrc);
		*hglrc = NULL;
		ReleaseDC(hwnd, *hdc);
		*hdc = NULL;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Resize(WIN_WIDTH, WIN_HEIGHT);
	SwapBuffers(*hdc);
}


void Render(HDC hdc)
{
	void DrawCornFloweRectangle();

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode((GL_MODELVIEW));
	glLoadIdentity();

	DrawCornFloweRectangle();
	SwapBuffers(hdc);
}


void UnIntializeOpenGL(HWND hwnd, HDC *hdc, HGLRC *hglrc)
{

	wglDeleteContext(*hglrc);
	hglrc = NULL;

	ReleaseDC(hwnd, *hdc);
	hdc = NULL;


	DestroyWindow(hwnd);
	hwnd = NULL;

	PostQuitMessage(0);
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	void Resize(int, int);

	void ToggleScreen(HWND);


	switch (msg)
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

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



void ToggleScreen(HWND hwnd)
{

	if (gbIsFullScreen)
	{
		SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &wpPrev);
		SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(true);
		gbIsFullScreen = false;
		return;
	}


	dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW)
	{
		MONITORINFO mi = { sizeof(MONITORINFO) };
		if (GetWindowPlacement(hwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITORINFOF_PRIMARY), &mi))
		{
			SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	ShowCursor(false);
	gbIsFullScreen = true;

}


void Resize(int width, int height)
{
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void DrawCornFloweRectangle()
{
	glBegin(GL_QUADS);

	glColor3f((100.0f/255.0f), (149.0f / 255.0f), (237.0f / 255.0f));

	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);


	glEnd();
}