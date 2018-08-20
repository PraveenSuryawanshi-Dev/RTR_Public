#include<Windows.h>
#include<stdio.h>
#include<d3d11.h>


#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"d3d11.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define LOG_FILE_NAME "DXLOG.txt"
#define SAFE_LOG(TEXT){ FILE* fp= NULL; fopen_s(&fp,LOG_FILE_NAME,"a+"); if(fp){fprintf(fp,"\n%s\n",TEXT); fclose(fp);}}



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT Initialize();
HRESULT Resize(int, int);
void Render();
void UnInitialize();



HWND ghwnd = NULL;
bool gbDone = false;
bool gbisActiveWidow = false;
bool gbisEscapeKeyPressed = false;


IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;


float gClearColor[4];

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevInstance, LPSTR lpszCmdLine, int ncmdShow)
{
	MSG msg;
	WNDCLASSEX wndclass;
	TCHAR szClassName[] = TEXT("D3D11 DXCLASS");
	TCHAR szAppNAme[] = TEXT("D3D11 DXWindow");
	HRESULT hr = S_OK;

	_unlink(LOG_FILE_NAME);

	ZeroMemory((void*)&wndclass, sizeof(WNDCLASSEX));
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = (WNDPROC)WndProc;
	wndclass.hInstance = hinstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, TEXT("fail to register class"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return 1;
	}



	ghwnd = CreateWindow(szClassName, szAppNAme, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hinstance, NULL);

	ShowWindow(ghwnd, ncmdShow);

	//UpdateWindow(ghwnd);

	SetForegroundWindow(ghwnd);

	SetFocus(ghwnd);

	hr = Initialize();
	if (FAILED(hr))
		SAFE_LOG("Initialize() failed.")

	else
		SAFE_LOG("Initialize() Succeeded.")


		while (gbDone == false)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					gbDone = true;
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				Render();

				if (gbisActiveWidow == true)
				{
					if (gbisEscapeKeyPressed == true)
						gbDone = true;
				}


			}
		}

	UnInitialize();

	return ((int)msg.wParam);

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT imsg, WPARAM wparam, LPARAM lparam)
{
	HRESULT hr = S_OK;

	switch (imsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wparam))
			gbisActiveWidow = true;
		else
			gbisActiveWidow = false;
		break;

	case WM_ERASEBKGND:
		return (0);


	case WM_KEYDOWN:
		break;


	case WM_SIZE:
		if (gpID3D11DeviceContext)
		{
			hr = Resize(LOWORD(lparam), HIWORD(lparam));
			if (FAILED(hr))
				SAFE_LOG("Resize() failed.")
			else
				SAFE_LOG("Resize() Succeeded.")
		}
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CLOSE:
		UnInitialize();
		break;

	default:
		break;

	}

	return DefWindowProc(hwnd, imsg, wparam, lparam);
}

HRESULT Initialize()
{

	HRESULT hr = S_OK;

	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	int d3dDriverTypes_count = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);

	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	UINT numFeatureLevels = 1;

	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;


	UINT createDeviceFlags = 0;
	D3D_DRIVER_TYPE d3dDriverType;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));

	dxgiSwapChainDesc.BufferCount = 1;

	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	dxgiSwapChainDesc.OutputWindow = ghwnd;

	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;

	dxgiSwapChainDesc.Windowed = TRUE;


	for (UINT driverTypeIndex = 0; driverTypeIndex < d3dDriverTypes_count; driverTypeIndex++)
	{

		d3dDriverType = d3dDriverTypes[driverTypeIndex];

		hr = D3D11CreateDeviceAndSwapChain(NULL,
			d3dDriverType,
			NULL,
			createDeviceFlags,
			&d3dFeatureLevel_required,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&dxgiSwapChainDesc,
			&gpIDXGISwapChain,
			&gpID3D11Device,
			&d3dFeatureLevel_acquired,
			&gpID3D11DeviceContext);

		if (SUCCEEDED(hr))
			break;


	}

	if (FAILED(hr))
	{
		SAFE_LOG("D3D11CreateDeviceAndSwapChain() failed.")
			return hr;
	}
	else
	{
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
			SAFE_LOG("choosen driver is of : hardware type. ")
		else
			if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
				SAFE_LOG("choosen driver is of : warp type. ")
			else
				if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
					SAFE_LOG("choosen driver is of : reference type. ")
				else
					SAFE_LOG("choosen driver is of : unknown type. ")


					if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
						SAFE_LOG("the supported highest feature level is : 11.0 .")
					else
						if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
							SAFE_LOG("the supported highest feature level is : 10.0 .")
						else
							SAFE_LOG("the supported highest feature level is : unknown .")
	}


	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 1.0f;
	gClearColor[3] = 1.0f;


	hr = Resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
		SAFE_LOG("Resize() failed.")
	else
		SAFE_LOG("Resize() Succeeded.")

	return hr;
}


HRESULT Resize(int width, int height)
{
	HRESULT hr = S_OK;

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	
	
	gpIDXGISwapChain->ResizeBuffers(1,width,height,DXGI_FORMAT_R8G8B8A8_UNORM,0);

	ID3D11Texture2D *pID3DTexture2D_BackBuffer=NULL;
	gpIDXGISwapChain->GetBuffer(0,_uuidof(ID3D11Texture2D),(LPVOID*)&pID3DTexture2D_BackBuffer);

	hr = gpID3D11Device->CreateRenderTargetView(pID3DTexture2D_BackBuffer,NULL,&gpID3D11RenderTargetView);

	if(FAILED(hr))
	SAFE_LOG("CreateRenderTargetView() failed.\n")
	else
	SAFE_LOG("CreateRenderTargetView() Succeeded.\n")

	pID3DTexture2D_BackBuffer->Release();
	pID3DTexture2D_BackBuffer = NULL;

	gpID3D11DeviceContext->OMSetRenderTargets(1,&gpID3D11RenderTargetView,NULL);

	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX=0;
	d3dViewPort.TopLeftY =0;
	d3dViewPort.Width = (float) width;
	d3dViewPort.Height =( float) height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth =1.0f;

	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);

	return hr;
}

void Render()
{
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView,gClearColor);


	gpIDXGISwapChain->Present(0,0);

}

void UnInitialize()
{

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

}