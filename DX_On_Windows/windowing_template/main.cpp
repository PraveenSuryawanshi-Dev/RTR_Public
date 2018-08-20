#include<Windows.h>
#include<stdio.h>

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"gdi32.lib")

#define SAFE_LOG(text){FILE *fp= NULL; fp=fopen("dxlog.txt","w+");if(fp!=NULL){fprintf(fp,"LOG %s",text); fp=NULL;}}

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HRESULT Initialize();
HRESULT Resize(int width ,int height);
void Render();
void Update();
void UnInitialize();
void ToggleeFullScreen();


BOOL gbDone = FALSE;
BOOL gbActiveWindow = FALSE;
BOOL gbEscKeyPressed = FALSE;

HWND ghwnd = 0;

BOOL gbFullScreen = FALSE;

int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance, LPSTR lpszcmdLine , int nCmdShow)
{
	MSG msg = {0};
	TCHAR szAppName[] = TEXT("DX Template");
	TCHAR szClassName[] = TEXT("DX WINDOW");
	WNDCLASSEX wndclass={0};
	HWND hwnd = 0;

	ZeroMemory(&wndclass,sizeof(WNDCLASSEX));
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = (WNDPROC) WndProc;
	wndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szClassName;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	if(!RegisterClassEx(&wndclass))
	{
		SAFE_LOG("Register class function failed");
		return 1;
	}

	ghwnd = CreateWindow(szClassName,szAppName,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,WIN_WIDTH,WIN_HEIGHT,NULL,NULL,hInstance,NULL);

	ShowWindow(ghwnd,nCmdShow);

	SetForegroundWindow(ghwnd);

	SetFocus(ghwnd);	

	if(Initialize() != S_OK)
	{
		SAFE_LOG("Faile to Initialize DX function")
		return 1;
	}

	while(gbDone == FALSE)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				gbDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			Update();

			Render();

			if(gbActiveWindow)
			{
				if(gbEscKeyPressed)
				gbDone = TRUE;
			}
		}
	}
	
	UnInitialize();

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT imsg, WPARAM wparam, LPARAM lparam)
{
	switch(imsg)
	{
		case WM_ACTIVATE:
		if(HIWORD(wparam) == 0)
			gbActiveWindow = TRUE;
		else
			gbActiveWindow = FALSE;
		break;

		case WM_SIZE:
		Resize(LOWORD(lparam),HIWORD(lparam));
		break;		
		
		case WM_KEYDOWN:
		switch(wparam)
		{
			case VK_ESCAPE:
			gbEscKeyPressed = TRUE;
			break;

			default:
			break;	
		}
		break;

		case WM_CHAR:
		switch(TCHAR(wparam))
		{
			case 'F':
			case 'f':
			if(gbFullScreen == TRUE)
			{
				ToggleeFullScreen();
				gbFullScreen  = FALSE;
			}
			else
			{
				ToggleeFullScreen();
				gbFullScreen = TRUE;
			}
			default:
			break;
		}
		break;

		case WM_CLOSE:
		UnInitialize();
		break;

		case WM_DESTROY:
		PostQuitMessage(0);
		break;

		default:
		break;
		
	}

	return ( DefWindowProc(hwnd,imsg,wparam,lparam));
}

HRESULT Initialize()
{
	return S_OK;
}



HRESULT Resize(int width ,int height)
{
	return S_OK;
}


void Render()
{
}


void Update()
{
}

void UnInitialize()
{
}

void ToggleeFullScreen()
{

	static DWORD dwStyle;
	static WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

	MONITORINFO mi;

	if(!gbFullScreen)
	{
		dwStyle = GetWindowLong(ghwnd,GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if(GetWindowPlacement(ghwnd,&wpPrev))
			{
				mi = { sizeof(MONITORINFO) };
				if(GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi))
				{
					SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
					SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
				}
			}
		}

		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);	
	}
}