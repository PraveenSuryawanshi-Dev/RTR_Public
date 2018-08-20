#include<Windows.h>
#include<stdio.h>

#include<d3d11.h>
#include<d3dcompiler.h>

#pragma warning(disable:4838)
#include"xnamath/xnamath.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"gdi32.lib")

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

#define SAFE_LOG(text){FILE *fp=NULL; fp=fopen("dxlog.txt","w+");	if(fp!=NULL){	fprintf(fp,"LOG %s",text); fp=NULL;	}	}

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


struct CBUFFER
{
	XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX gOrthographicProjectionMatrix;
FLOAT gColor[4];

IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;

ID3D11Buffer *gpID3D11Buffer_vertexBuffer = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11InputLayout *gpID3D11InputLayout = NULL;

int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance, LPSTR lpszcmdLine , int nCmdShow)
{
	HRESULT hr = S_OK;
	MSG msg = {0};
	TCHAR szAppName[] = TEXT("DX Template");
	TCHAR szClassName[] = TEXT("DX WINDOW [Ortho Triangle] ");
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

	hr = Initialize();
	if(FAILED(hr))
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
	HRESULT hr = S_OK;
	switch(imsg)
	{
		case WM_ACTIVATE:
		if(HIWORD(wparam) == 0)
			gbActiveWindow = TRUE;
		else
			gbActiveWindow = FALSE;
		break;

		case WM_ERASEBKGND:
		return(0);
		
		
		case WM_SIZE:
		if(gpID3D11DeviceContext)
		{
			hr = Resize(LOWORD(lparam),HIWORD(lparam));
			if(FAILED(hr))
			{
				SAFE_LOG("Resize function failed.")
				return hr;
			}
		}
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
	HRESULT hr = S_OK;
	
	IDXGIAdapter *pAdapter = NULL;
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT d3dDriverTypesCount = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);

	HMODULE hModule = NULL;
	
	UINT iflag = 0;

	D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	
	UINT d3dFeatureLevelCount = 1;
	
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void *)&dxgiSwapChainDesc,sizeof(DXGI_SWAP_CHAIN_DESC));
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
	
	
	D3D_FEATURE_LEVEL d3dFeatureLevel_accquired = D3D_FEATURE_LEVEL_10_0;

	D3D_DRIVER_TYPE d3dDriverType;
	
	for(UINT i =0; i < d3dDriverTypesCount ; i ++)
	{
		d3dDriverType =	d3dDriverTypes[i];
		
		hr = D3D11CreateDeviceAndSwapChain(pAdapter,
		d3dDriverType,
		hModule,
		iflag,
		&d3dFeatureLevel,
		d3dFeatureLevelCount,
		D3D11_SDK_VERSION,
		&dxgiSwapChainDesc,
		&gpIDXGISwapChain,
		&gpID3D11Device,
		&d3dFeatureLevel_accquired,	
		&gpID3D11DeviceContext);
		
		if(SUCCEEDED(hr))
		break;
	}
	
	if(FAILED(hr))
	{
		SAFE_LOG("D3D11CreateDeviceAndSwapChain function failed.");
		return hr;
	}
	
	

	//create vertex shader
	const char *vssc=
	"cbuffer ConstantBuffer"\
	"{"\
		"float4x4 worldProjectionMatrix;"\
	"}"\
	"float4 main(float4 pos: POSITION) : SV_POSITION"\
	"{"\
		"float4 position = mul(worldProjectionMatrix,pos);"\
		"return (position);"\
	"}";
	
	ID3DBlob *pID3DBlob_vs = NULL;
	ID3DBlob *pID3DBlob_erro = NULL;
	
	////corresponds to  glCompileShader
	hr = D3DCompile
	(	vssc,
		lstrlenA(vssc)+1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_vs,
		&pID3DBlob_erro
	);
	
			
	if(FAILED(hr))
	{
		if(pID3DBlob_erro != NULL)
		{
			SAFE_LOG((char *) pID3DBlob_erro->GetBufferPointer());
			return hr;
		}
	}
	
	//corresponds to  glCreateShader(GL_VERTEX_SHADER) && glShaderSource
	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_vs->GetBufferPointer(),pID3DBlob_vs->GetBufferSize(),NULL,&gpID3D11VertexShader);
	if(FAILED(hr))
	{
		SAFE_LOG("CreateVertexShader function() failed.");
		return hr;
	}
	//corresponds to  glAttachShader()
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader,0,0);
	
		
	ID3DBlob *pID3DBlob_ps= NULL;
	pID3DBlob_erro = NULL;
	const char *pssrc=
	"float4 main(void) : SV_TARGET"\
	"{"\
		"return (float4(1.0f,1.0f,1.0f,1.0f));"\
	"}";
	
	//corresponds to  glCompileShader
	hr = D3DCompile(
	pssrc,
	lstrlenA(pssrc)+1,
	"PS",
	NULL,
	D3D_COMPILE_STANDARD_FILE_INCLUDE,
	"main",
	"ps_5_0",
	0,
	0,
	&pID3DBlob_ps,
	&pID3DBlob_erro
	);
	
	if(FAILED(hr))
	{
		if(pID3DBlob_erro != NULL)
		{
			SAFE_LOG((char*)pID3DBlob_erro->GetBufferPointer());
			return hr;
		}
	}
	
	//corresponds to  glCreateShader(GL_FRAGMENT_SHADER) && glShaderSource
	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_ps->GetBufferPointer(),pID3DBlob_ps->GetBufferSize(),NULL,&gpID3D11PixelShader);
	if(FAILED(hr))
	{
		SAFE_LOG("CreatePixelShader function failed.");
		return hr;
	}
	
	//corresponds to  glAttachShader
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader,0,0);
	pID3DBlob_ps->Release();
	pID3DBlob_ps = NULL;
	
	//create input layout : corresponds to glBindAttribLocation
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[1];
	ZeroMemory(inputElementDesc,sizeof(D3D11_INPUT_ELEMENT_DESC));
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;
	
	
	hr = gpID3D11Device->CreateInputLayout(inputElementDesc,1,pID3DBlob_vs->GetBufferPointer(),pID3DBlob_vs->GetBufferSize(),&gpID3D11InputLayout);
	if(FAILED(hr))
	{
		SAFE_LOG("CreateInputLayout function failed.")
		return hr;
	}
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);
	pID3DBlob_vs->Release();
	pID3DBlob_vs = NULL;
	//
	
	
	//corresponds to glGetUniformLocation
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc,0,&gpID3D11Buffer_ConstantBuffer);
	if(FAILED(hr))
	{
		SAFE_LOG("CreateBuffer function failed.")
	}
	gpID3D11DeviceContext->VSSetConstantBuffers(0,1,&gpID3D11Buffer_ConstantBuffer);
	//
	
	
	
	float vertices[]=
	{
		//TOP
		0.0f ,50.0f,0.0f,
		//RIGHT
		50.0f,-50.0f,0.0f,
		//LEFT
		-50.0f,-50.0f,0.0f,
	};
	
	
	//create vbo
	ZeroMemory(&bufferDesc,sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * ARRAYSIZE(vertices);
	bufferDesc.BindFlags =D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc,NULL,&gpID3D11Buffer_vertexBuffer);
	
	if(FAILED(hr))
	{
		SAFE_LOG("CreateBuffer function failed.")
		return hr;
	}
	
	
	//copy vertices to vbo
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource,sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_vertexBuffer,NULL,D3D11_MAP_WRITE_DISCARD,NULL,&mappedSubresource);
	memcpy(mappedSubresource.pData, vertices,sizeof(vertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_vertexBuffer,NULL);

	
	
	
	

	
	
	gColor[0] = 0.0f;
	gColor[1] = 0.0f;
	gColor[2] = 1.0f;
	gColor[3] = 1.0f;
	
	gOrthographicProjectionMatrix = XMMatrixIdentity();
	
	hr=Resize(WIN_WIDTH,WIN_HEIGHT);
	
	if(FAILED(hr))
	{
		SAFE_LOG("Resize function failed.")
		return hr;
	}

	return S_OK;
}



HRESULT Resize(int width ,int height)
{
	HRESULT hr = S_OK;
	
	if(gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}
	
	gpIDXGISwapChain->ResizeBuffers(1,width,height,DXGI_FORMAT_R8G8B8A8_UNORM,0);
	
	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID*)&pID3D11Texture2D_BackBuffer);
	
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer,NULL,&gpID3D11RenderTargetView);
	if(FAILED(hr))
	{
		SAFE_LOG("CreateRenderTargetView function failed.")
		return hr;
	}
	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;
	
	gpID3D11DeviceContext->OMSetRenderTargets(1,&gpID3D11RenderTargetView,NULL);
	
	//set viewPort
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX =0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (FLOAT) width;
	d3dViewPort.Height = (FLOAT) height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	
	gpID3D11DeviceContext->RSSetViewports(1,&d3dViewPort);
	
	// set orthographic matrix
	if (width <= height)
		gOrthographicProjectionMatrix = XMMatrixOrthographicOffCenterLH(-100.0f, 100.0f, -100.0f * ((float)height / (float)width), 100.0f * ((float)height / (float)width), -100.0f, 100.0f);
	else
		gOrthographicProjectionMatrix = XMMatrixOrthographicOffCenterLH(-100.0f * ((float)width / (float)height), 100.0f * ((float)width / (float)height), -100.0f, 100.0f, -100.0f, 100.0f);

	
	return S_OK;
}


void Render()
{
	UINT stride = sizeof(FLOAT) * 3;
	UINT offset = 0;
	
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView,gColor);
	
	gpID3D11DeviceContext->IASetVertexBuffers(0,1,&gpID3D11Buffer_vertexBuffer , &stride, &offset);
	
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX wvpMatrix = XMMatrixIdentity();
	
	wvpMatrix = worldMatrix * viewMatrix * gOrthographicProjectionMatrix;
	
	CBUFFER cbuffer;
	ZeroMemory(&cbuffer,sizeof(CBUFFER));
	cbuffer.WorldViewProjectionMatrix = wvpMatrix;
	
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer , 0 ,NULL,&cbuffer,0,0);
	
	gpID3D11DeviceContext->Draw(3,0);
	
	gpIDXGISwapChain->Present(0,0);
}


void Update()
{
}

void UnInitialize()
{

	
	if(gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}
	
	if(gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}
	
	if(gpID3D11Buffer_vertexBuffer)
	{
			gpID3D11Buffer_vertexBuffer->Release();
			gpID3D11Buffer_vertexBuffer = NULL;
	}
	
	if(gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}
	
	if(gpID3D11VertexShader)
	{
			gpID3D11VertexShader->Release();
			gpID3D11VertexShader = NULL;
	}
	
	if(gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}
	
	if(gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}
	
	if(gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}
	if(gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}
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