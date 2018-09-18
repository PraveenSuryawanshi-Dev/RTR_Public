#include<Windows.h>
#include<stdio.h>
#include<d3d11.h>
#include<d3dcompiler.h>
#pragma warning(disable:4838)
#include "../XNAMath_204/xnamath.h"

#include "../DirectXTK-master/Inc/WICTextureLoader.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")


#define TEX_WIDTH 64
#define TEX_HEIGHT 64


#define SAFE_LOG(text){FILE *fp= NULL; fp=fopen("dxlog.txt","w+");if(fp!=NULL){fprintf(fp,"LOG %s",text); fclose(fp);fp=NULL;}}
#define SAFE_RELEASE(mem){if(mem){mem->Release();mem=NULL;}}
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Initialize();
HRESULT Resize(int width, int height);
void Render();
void Update();
void UnInitialize();
void ToggleeFullScreen();


BOOL gbDone = FALSE;
BOOL gbActiveWindow = FALSE;
BOOL gbEscKeyPressed = FALSE;

HWND ghwnd = 0;

BOOL gbFullScreen = FALSE;

HRESULT hr = S_OK;

//DirectX declarations
typedef struct _cbuffer_
{
	XMMATRIX ModelViewProjectionMatrixl;

}CBUFFER;

IDXGISwapChain *pIDXGISwapChain = NULL;
ID3D11Device *pID3D11Device = NULL;
ID3D11DeviceContext *pID3D11DeviceContext = NULL;
ID3D11RenderTargetView *pID3D11RenderTargetView = NULL;
ID3D11DepthStencilView *pID3D11DepthStencilView = NULL;

ID3D11RasterizerState *pID3D11RasterizerState = NULL;


ID3D11VertexShader *pID3D11VertexShader = NULL;
ID3D11PixelShader *pID3D11PixelShader = NULL;
ID3D11InputLayout *pID3D11InputLayout = NULL;


ID3D11SamplerState *pID3D11SamplerState = NULL;



ID3D11Buffer *pID3D11BufferQuadVertices = NULL;
ID3D11Buffer *pID3D11BufferQuadTexCoord = NULL;

ID3D11ShaderResourceView *pID3D11ShaderResourceViewCheckerBoard = NULL;

float quad_angle = 0.0f;

ID3D11Buffer *pID3D11ConstantBuffer = NULL;

float gColor[4];

XMMATRIX xmPerspectiveProjectionMatrix;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszcmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	TCHAR szAppName[] = TEXT("DX Template");
	TCHAR szClassName[] = TEXT("DX WINDOW");
	WNDCLASSEX wndclass = { 0 };
	HWND hwnd = 0;

	ZeroMemory(&wndclass, sizeof(WNDCLASSEX));
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = (WNDPROC)WndProc;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szClassName;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	if (!RegisterClassEx(&wndclass))
	{
		SAFE_LOG("Register class function failed");
		return 1;
	}

	ghwnd = CreateWindow(szClassName, szAppName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(ghwnd, nCmdShow);

	SetForegroundWindow(ghwnd);

	SetFocus(ghwnd);

	if (Initialize() != S_OK)
	{
		SAFE_LOG("Faile to Initialize DX function")
			return 1;
	}

	while (gbDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
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

			if (gbActiveWindow)
			{
				if (gbEscKeyPressed)
					gbDone = TRUE;
			}
		}
	}

	UnInitialize();

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT imsg, WPARAM wparam, LPARAM lparam)
{

	switch (imsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wparam) == 0)
			gbActiveWindow = TRUE;
		else
			gbActiveWindow = FALSE;
		break;

	case WM_SIZE:
		if (pID3D11DeviceContext)
		{
			if (FAILED(Resize(LOWORD(lparam), HIWORD(lparam))))
			{
				SAFE_LOG("WndProc : Resize function failed.")
			}

		}
		break;

	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_ESCAPE:
			gbEscKeyPressed = TRUE;
			break;

		}
		break;

	case WM_CHAR:
		switch (TCHAR(wparam))
		{
		case 'F':
		case 'f':
			if (gbFullScreen == TRUE)
			{
				ToggleeFullScreen();
				gbFullScreen = FALSE;
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

	return (DefWindowProc(hwnd, imsg, wparam, lparam));
}


HRESULT LoadD3DCheckerBoardTexture(ID3D11ShaderResourceView **ppID3D11ShaderResourceView)
{
	BYTE checkerBoardTexture[TEX_WIDTH*TEX_HEIGHT*4];
	UINT bufferLen = TEX_WIDTH * TEX_HEIGHT * 4;

	int	c = 0;
	int depth = 4;
	for (int j = 0; j < TEX_WIDTH; j++)
	{
		for (int k = 0; k < TEX_HEIGHT; k++)
		{
			c = ((j & 0x8) == 0 ^ (k & 0x8) == 0 ? 1 : 0) * 255;

			checkerBoardTexture[0 + depth * (j + TEX_WIDTH * (k))] = (byte)c;
			checkerBoardTexture[1 + depth * (j + TEX_WIDTH * (k))] = (byte)c;
			checkerBoardTexture[2 + depth * (j + TEX_WIDTH * (k))] = (byte)c;
			checkerBoardTexture[3 + depth * (j + TEX_WIDTH * (k))] = (byte)255;
		}
	}



	DWORD Written = 0;
	BITMAPFILEHEADER bfh;
	BITMAPINFO bi;
	BITMAPINFOHEADER bih;

	memset(&bfh, 0, sizeof(bfh));
	bfh = (BITMAPFILEHEADER&)checkerBoardTexture[0];
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof(bfh) + bufferLen + sizeof(BITMAPINFOHEADER);
	bfh.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

	memset(&bih, 0, sizeof(bih));
	bih.biSize = sizeof(bih);
	bih.biWidth = TEX_WIDTH;
	bih.biHeight = TEX_HEIGHT;
	bih.biPlanes = 1;
	bih.biBitCount = 32;

	bi.bmiHeader = bih;


	UINT bitmapSize = sizeof(BYTE)* (sizeof(bfh) + sizeof(bih) + bufferLen);
	BYTE *bitmap = (BYTE*)malloc(bitmapSize);

	memcpy_s(bitmap, bitmapSize, &bfh, sizeof(bfh));
	memcpy_s(bitmap + sizeof(bfh), bitmapSize, &bih, sizeof(bih));
	memcpy_s(bitmap + (sizeof(bfh) + sizeof(bih)), bitmapSize, checkerBoardTexture, bufferLen);

	HRESULT hr = DirectX::CreateWICTextureFromMemory(pID3D11Device, pID3D11DeviceContext, (const uint8_t*)bitmap, (size_t)bitmapSize, NULL, ppID3D11ShaderResourceView, 0);
	if (FAILED(hr))
	{
		SAFE_LOG("LoadD3DTexture CreateWICTextureFromFile function failed.")
		return S_FALSE;
	}

	return S_OK;

}
HRESULT Initialize()
{

	D3D11_INPUT_ELEMENT_DESC  inputElementDesc[2];
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource = { 0 };
	D3D11_RASTERIZER_DESC rasterizerDesc;

	D3D_DRIVER_TYPE d3dDriverTypes[] = {

		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	D3D_FEATURE_LEVEL d3dFeatureLevelSet = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevelAcquired = D3D_FEATURE_LEVEL_10_0;

	int d3dDriverTypesCount = (sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]));
	D3D_DRIVER_TYPE d3dDriverType;

	DXGI_SWAP_CHAIN_DESC  dxgiSwapChainDesc;
	ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	for (int i = 0; i < d3dDriverTypesCount; i++)
	{
		d3dDriverType = d3dDriverTypes[i];

		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			d3dDriverType,
			NULL,
			0,
			&d3dFeatureLevelSet,
			1,
			D3D11_SDK_VERSION,
			&dxgiSwapChainDesc,
			&pIDXGISwapChain,
			&pID3D11Device,
			&d3dFeatureLevelAcquired,
			&pID3D11DeviceContext
		);

		if (SUCCEEDED(hr))
			break;
	}


	if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		SAFE_LOG("Initialize : D3D_DRIVER_TYPE_HARDWARE found")
	else
	{
		SAFE_LOG("Initialize : D3D_DRIVER_TYPE_HARDWARE not found")
			return S_FALSE;
	}

	if (d3dFeatureLevelAcquired == d3dFeatureLevelSet)
		SAFE_LOG("Initialize : D3D11_FEATURE_LEVEL_11_0 found")
	else
	{
		SAFE_LOG("Initialize : D3D11_FEATURE_LEVEL_11_0 not found")
			return S_FALSE;
	}


	ID3DBlob *pID3DBlob_vssrc = NULL;
	ID3DBlob *pID3DBlob_pssrc = NULL;
	ID3DBlob *pID3DBlob_error = NULL;

	CONST CHAR *vs =
		"cbuffer ConstantBuffer"\
		"{"\
			"float4x4  worldViewProjectionMatrix;"\
		"}"\

		"struct vs_out"\
		"{"
		"float4 position : SV_POSITION;"\
		"float2 texcoord : TEXCOORD;"\
		"};"\

		"vs_out main(float4 position : POSITION ,float2 texcoord :TEXCOORD)"\
		"{"\
			"vs_out vsout;"\
			"vsout.position = mul(worldViewProjectionMatrix,position);"\
			"vsout.texcoord = texcoord;"\
			"return vsout;"\
		"}";

	hr = D3DCompile(vs, lstrlenA(vs) + 1, "VS", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &pID3DBlob_vssrc, &pID3DBlob_error);

	if (FAILED(hr))
	{
		if (pID3DBlob_error != NULL)
		{
			SAFE_LOG((char*)pID3DBlob_error->GetBufferPointer())
				return S_FALSE;
		}
	}
	if (FAILED(pID3D11Device->CreateVertexShader(pID3DBlob_vssrc->GetBufferPointer(), pID3DBlob_vssrc->GetBufferSize(), NULL, &pID3D11VertexShader)))
	{
		SAFE_LOG("Initialize : CreateVertexShader function failed.")
			return S_FALSE;
	}

	pID3D11DeviceContext->VSSetShader(pID3D11VertexShader, NULL, 0);


	pID3DBlob_error = NULL;
	pID3DBlob_pssrc = NULL;
	CONST CHAR *ps =
		"Texture2D itexture2D;"\
		"SamplerState isampler;"\
		"float4 main(float4 position:SV_POSITION, float2 texcoord : TEXCOORD): SV_TARGET"\
		"{"\
		"return itexture2D.Sample(isampler,texcoord);"\
		"}";

	hr = D3DCompile(ps, lstrlenA(ps) + 1, "PS", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &pID3DBlob_pssrc, &pID3DBlob_error);

	if (FAILED(hr))
	{

		if (pID3DBlob_error != NULL)
		{
			char *s = (char*)pID3DBlob_error->GetBufferPointer();
			SAFE_LOG((char*)pID3DBlob_error->GetBufferPointer());
			return S_FALSE;
		}

	}

	if (FAILED(pID3D11Device->CreatePixelShader(pID3DBlob_pssrc->GetBufferPointer(), pID3DBlob_pssrc->GetBufferSize(), NULL, &pID3D11PixelShader)))
	{
		SAFE_LOG("Initialize : CreatePixelShader function failed.")
			return S_FALSE;;
	}

	pID3D11DeviceContext->PSSetShader(pID3D11PixelShader, NULL, 0);



	ZeroMemory(&inputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC));
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InstanceDataStepRate = 0;


	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].InstanceDataStepRate = 0;


	if (FAILED(pID3D11Device->CreateInputLayout(inputElementDesc, 2, pID3DBlob_vssrc->GetBufferPointer(), pID3DBlob_vssrc->GetBufferSize(), &pID3D11InputLayout)))
	{
		SAFE_LOG("Initialize : CreateInputLayout function failed.")
			return S_FALSE;
	}

	pID3D11DeviceContext->IASetInputLayout(pID3D11InputLayout);

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(pID3D11Device->CreateBuffer(&bufferDesc, NULL, &pID3D11ConstantBuffer)))
	{
		SAFE_LOG("Initialize : CreateInputLayout function failed.")
			return S_FALSE;
	}

	pID3D11DeviceContext->VSSetConstantBuffers(0, 1, &pID3D11ConstantBuffer);

	/////////////////////////////////////////////////////////////////////////////////////////////////////



	//vertices for quad
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * 18;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (FAILED(pID3D11Device->CreateBuffer(&bufferDesc, NULL, &pID3D11BufferQuadVertices)))
	{
		SAFE_LOG("Initialize : CreateBuffer function failed.")
			goto ON_ERROR;
	}
	

	//color for quad
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * 12;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (FAILED(pID3D11Device->CreateBuffer(&bufferDesc, NULL, &pID3D11BufferQuadTexCoord)))
	{
		SAFE_LOG("Initialize : CreateBuffer function failed.")
			goto ON_ERROR;
	}


	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;

	pID3D11Device->CreateRasterizerState(&rasterizerDesc, &pID3D11RasterizerState);

	pID3D11DeviceContext->RSSetState(pID3D11RasterizerState);


	hr = LoadD3DCheckerBoardTexture(&pID3D11ShaderResourceViewCheckerBoard);
	if (FAILED(hr))
	{
		SAFE_LOG("Initialize : LoadD3DTexture function failed.")
			return S_FALSE;
	}


	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	if (FAILED(pID3D11Device->CreateSamplerState(&samplerDesc, &pID3D11SamplerState)))
	{
		SAFE_LOG("Initialize : CreateSamplerState function failed.")
			goto ON_ERROR;
	}


	gColor[0] = 0.0f;
	gColor[1] = 0.0f;
	gColor[2] = 0.0f;
	gColor[3] = 1.0f;

	xmPerspectiveProjectionMatrix = XMMatrixIdentity();

	if (FAILED(Resize(WIN_WIDTH, WIN_HEIGHT)))
	{
		SAFE_LOG("Initialize : Resize function failed.")
			goto ON_ERROR;
	}

	SAFE_RELEASE(pID3DBlob_error)
		SAFE_RELEASE(pID3DBlob_vssrc)
		SAFE_RELEASE(pID3DBlob_pssrc)
		return S_OK;

ON_ERROR:
	SAFE_RELEASE(pID3DBlob_error)
		SAFE_RELEASE(pID3DBlob_vssrc)
		SAFE_RELEASE(pID3DBlob_pssrc)
		return S_FALSE;
}



HRESULT Resize(int width, int height)
{
	ID3D11Texture2D *pID3D11Texture2D_backBuffer = NULL;
	ID3D11Texture2D *pID3D11Texture2D_depthBuffer = NULL;
	D3D11_TEXTURE2D_DESC texture2DDesc = { 0 };
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilView;



	SAFE_RELEASE(pID3D11DepthStencilView)

		SAFE_RELEASE(pID3D11RenderTargetView)

		if (FAILED(pIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
		{
			SAFE_LOG("Resize : ResizeBuffers function failed.")
				goto ON_ERROR;
		}


	if (FAILED(pIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_backBuffer)))
	{
		SAFE_LOG("Resize : GetBuffer function failed.")
			goto ON_ERROR;
	}

	if (FAILED(pID3D11Device->CreateRenderTargetView(pID3D11Texture2D_backBuffer, NULL, &pID3D11RenderTargetView)))
	{
		SAFE_LOG("Resize : CreateRenderTargetView function failed.")
			goto ON_ERROR;
	}



	ZeroMemory(&texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2DDesc.Width = width;
	texture2DDesc.Height = height;
	texture2DDesc.ArraySize = 1;
	texture2DDesc.MipLevels = 1;
	texture2DDesc.SampleDesc.Count = 1;
	texture2DDesc.SampleDesc.Quality = 0;
	texture2DDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2DDesc.CPUAccessFlags = 0;
	texture2DDesc.MiscFlags = 0;

	if (FAILED(pID3D11Device->CreateTexture2D(&texture2DDesc, NULL, &pID3D11Texture2D_depthBuffer)))
	{
		SAFE_LOG("Resize : CreateTexture2D function failed.")
			goto ON_ERROR;
	}

	ZeroMemory(&depthStencilView, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilView.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	if (FAILED(pID3D11Device->CreateDepthStencilView(pID3D11Texture2D_depthBuffer, &depthStencilView, &pID3D11DepthStencilView)))
	{
		SAFE_LOG("Resize : CreateTexture2D function failed.")
			goto ON_ERROR;
	}

	pID3D11DeviceContext->OMSetRenderTargets(1, &pID3D11RenderTargetView, pID3D11DepthStencilView);


	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	pID3D11DeviceContext->RSSetViewports(1, &viewport);

	xmPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.05f), (FLOAT)width / (FLOAT)height, 1.0f, 30.0f);

	SAFE_RELEASE(pID3D11Texture2D_backBuffer)
		SAFE_RELEASE(pID3D11Texture2D_depthBuffer)
		return S_OK;

ON_ERROR:
	SAFE_RELEASE(pID3D11Texture2D_backBuffer)
	SAFE_RELEASE(pID3D11Texture2D_depthBuffer)
	return S_FALSE;
}


void Render()
{
	void RenderQuad();

	pID3D11DeviceContext->ClearRenderTargetView(pID3D11RenderTargetView, gColor);
	pID3D11DeviceContext->ClearDepthStencilView(pID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	RenderQuad();

	pIDXGISwapChain->Present(0, 0);
}



void RenderQuad()
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	float quad_vertices[] =
	{
		/*TRIANGLE FIRST FRONT*/
		//LEFT TOP
		-2.0f,1.0f,0.0f,
		//RIGHT TOP
		0.0f,1.0f,0.0f,
		//LEFT BOTTOM
		-2.0f,-1.0f,0.0f,

		/*TRIANGLE SECOND FRONT*/
		//LEFT BOTTOM
		-2.0f,-1.0f,0.0f,
		//RIGHT TOP
		0.0f,1.0f,0.0f,
		//RIGHT BOTTOM
		0.0f,-1.0f,0.0f
	};


	const FLOAT quad_texcords[] =
	{
		0.0f, 0.0f,
		1.0f,0.0f,
		0.0f,1.0f,

		0.0f,1.0f,
		1.0f,0.0f,
		1.0f,1.0f
	};


	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	pID3D11DeviceContext->Map(pID3D11BufferQuadVertices, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, quad_vertices, sizeof(quad_vertices));
	pID3D11DeviceContext->Unmap(pID3D11BufferQuadVertices, NULL);

	
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	pID3D11DeviceContext->Map(pID3D11BufferQuadTexCoord, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, quad_texcords, sizeof(quad_texcords));
	pID3D11DeviceContext->Unmap(pID3D11BufferQuadTexCoord, NULL);


	UINT strideVertices = sizeof(float) * 3; // 3 is for x, y, z
	UINT startOffsetVertices = 0;

	UINT strideTexcord = sizeof(float) * 2; // 2 is for u ,w 
	UINT startOffsetTexcord = 0;

	CBUFFER cbuffer;

	ZeroMemory(&cbuffer, sizeof(CBUFFER));
	

	pID3D11DeviceContext->IASetVertexBuffers(0, 1, &pID3D11BufferQuadVertices, &strideVertices, &startOffsetVertices);
	pID3D11DeviceContext->IASetVertexBuffers(1, 1, &pID3D11BufferQuadTexCoord, &strideTexcord, &startOffsetTexcord);

	pID3D11DeviceContext->PSSetShaderResources(0, 1, &pID3D11ShaderResourceViewCheckerBoard);

	//BIND SAMPLER
	pID3D11DeviceContext->PSSetSamplers(0, 1, &pID3D11SamplerState);


	pID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	XMMATRIX xmWorldMatrix = XMMatrixIdentity();
	XMMATRIX xmViewMatrix = XMMatrixIdentity();
	XMMATRIX xmTranslateMatrix = XMMatrixIdentity();


	XMMATRIX xmWordViewProjectionMatrix = XMMatrixIdentity();

	xmWorldMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);

	xmWordViewProjectionMatrix = xmWorldMatrix * xmViewMatrix  * xmPerspectiveProjectionMatrix;

	cbuffer.ModelViewProjectionMatrixl = xmWordViewProjectionMatrix;

	pID3D11DeviceContext->UpdateSubresource(pID3D11ConstantBuffer, 0, NULL, &cbuffer, 0, 0);

	pID3D11DeviceContext->Draw(6, 0);
	pID3D11DeviceContext->Draw(6, 6);
	pID3D11DeviceContext->Draw(6, 12);
	pID3D11DeviceContext->Draw(6, 18);
	pID3D11DeviceContext->Draw(6, 24);
	pID3D11DeviceContext->Draw(6, 30);


	//Draw Deviated Quad.

	float quad_vertices_deviated[] =
	{
		/*TRIANGLE FIRST FRONT*/
		//right TOP
		1.0f, 1.0f, 0.0f,
		//RIGHT TOP Depth
		2.41421f, 1.0f, 1.41421f,
		//LEFT BOTTOM
		1.0f, -1.0f, 0.0f,

		/*TRIANGLE SECOND FRONT*/
		//Riht BOTTOM
		1.0f, -1.0f, 0.0f,
		//RIGHT TOP Depth
		2.41421f, 1.0f, 1.41421f,
		//RIGHT BOTTOM Depth
		2.41421f, -1.0f, 1.41421f,
	};

	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	pID3D11DeviceContext->Map(pID3D11BufferQuadVertices, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, quad_vertices_deviated, sizeof(quad_vertices_deviated));
	pID3D11DeviceContext->Unmap(pID3D11BufferQuadVertices, NULL);


	

	pID3D11DeviceContext->IASetVertexBuffers(0, 1, &pID3D11BufferQuadVertices, &strideVertices, &startOffsetVertices);
	pID3D11DeviceContext->IASetVertexBuffers(1, 1, &pID3D11BufferQuadTexCoord, &strideTexcord, &startOffsetTexcord);

	pID3D11DeviceContext->PSSetShaderResources(0, 1, &pID3D11ShaderResourceViewCheckerBoard);

	//BIND SAMPLER
	pID3D11DeviceContext->PSSetSamplers(0, 1, &pID3D11SamplerState);


	pID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pID3D11DeviceContext->Draw(6, 0);
	pID3D11DeviceContext->Draw(6, 6);
	pID3D11DeviceContext->Draw(6, 12);
	pID3D11DeviceContext->Draw(6, 18);
	pID3D11DeviceContext->Draw(6, 24);
	pID3D11DeviceContext->Draw(6, 30);
}


void Update()
{

}

void UnInitialize()
{
		SAFE_RELEASE(pID3D11ShaderResourceViewCheckerBoard)
		SAFE_RELEASE(pID3D11DepthStencilView)
		SAFE_RELEASE(pID3D11ConstantBuffer)
		SAFE_RELEASE(pID3D11BufferQuadVertices)
		SAFE_RELEASE(pID3D11BufferQuadTexCoord)
		SAFE_RELEASE(pID3D11InputLayout)
		SAFE_RELEASE(pID3D11PixelShader)
		SAFE_RELEASE(pID3D11VertexShader)
		SAFE_RELEASE(pID3D11RenderTargetView)
		SAFE_RELEASE(pID3D11DeviceContext)
		SAFE_RELEASE(pID3D11Device)
		SAFE_RELEASE(pIDXGISwapChain)
}

void ToggleeFullScreen()
{

	static DWORD dwStyle;
	static WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

	MONITORINFO mi;

	if (!gbFullScreen)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev))
			{
				mi = { sizeof(MONITORINFO) };
				if (GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY),
					&mi))
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