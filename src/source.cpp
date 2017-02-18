//--------------------------------------------------------------------------------------
// File: Tutorial022.cpp
//
// This application displays a triangle using Direct3D 11
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "../includes/resource.h"
#include "../includes/simplevertex.h"
#include "../includes/grlLoader.h"
#include "../includes/player.h"
#include "../includes/stopwatchmicro.h"
#include "../includes/controller.h"
#include "../includes/sound.h"
#include "../includes/render_to_texture.h"

#define PI 3.14159265
#define MAX_VEL 1.00


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
/*struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};*/


//---------------------------------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------------------------------
HINSTANCE													g_hInst = NULL;
HWND														g_hWnd = NULL;
D3D_DRIVER_TYPE												g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL											g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*												g_pd3dDevice = NULL;
ID3D11DeviceContext*										g_pImmediateContext = NULL;
IDXGISwapChain*												g_pSwapChain = NULL;
ID3D11RenderTargetView*										g_pRenderTargetView = NULL;
ID3D11VertexShader*											g_pVertexShader = NULL;
ID3D11PixelShader*											g_pPixelShader = NULL;
ID3D11VertexShader*											g_pAnimateVertexShader = NULL;
ID3D11PixelShader*											g_pAnimatePixelShader = NULL;
ID3D11InputLayout*											g_pVertexLayout = NULL;
ID3D11Buffer*												g_pVertexBuffer = NULL;
ID3D11Buffer*												g_pConstantBuffer11 = NULL;
ID3D11BlendState*											g_BlendState;
ID3D11SamplerState*											g_Sampler = NULL;
ID3D11SamplerState*											SamplerScreen = NULL;

ID3D11VertexShader*											g_pVertexShader_screen = NULL;
ID3D11PixelShader*											g_pPixelShader_screen = NULL;

ID3D11Buffer*												g_pVertexBuffer_screen = NULL;

ID3D11ShaderResourceView*									g_Texture = NULL;
//ID3D11ShaderResourceView*									g_Texture2 = NULL;
ID3D11ShaderResourceView*									g_Tex_BG = NULL;
ID3D11ShaderResourceView*									g_grassTexture = NULL;

level*														first;

vector<ID3D11Buffer*>										g_VertexBufferList;

unordered_map<string, texture*>*							texMap;

RenderTextureClass*											RenderToTexture;


static const int											WIDTH = 1280;
static const int											HEIGHT = 1280;
float														x_impulse;
float														y_impulse;
bool														isJumping = true;
VS_CONSTANT_BUFFER											VsConstData;
player														g_player = player();
ID3D11ShaderResourceView*									g_playerTexture = NULL;

CXBOXController												*gamepad = new CXBOXController(1);

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
void draw_text(HDC DC, char text[], int x, int y, int r, int g, int b);
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();
void Update();
void CheckCollisions();
void printStuff();
void Render_To_Texture();
void Render_To_Screen();

void OpenConsole()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	OpenConsole();

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}
	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
		}
	}

	CleanupDevice();

	return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	//RECT rc = { 0, 0, 640, 480 };
	RECT rc = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"TutorialWindowClass", L"EckENGINE :^)",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;


	grlLoader loader = grlLoader();
	first = loader.loadGRL("C:\\Users\\gamel\\Documents\\Matt\\CAPSTONE\\NEw\\Johnson_HW3\\GameEngineFramework\\grl\\videodemo.grl");

	texMap = loader.getTextureMap();
	texMap->count("C:\\Users\\gamel\\Documents\\Matt\\CAPSTONE\\NEw\\Johnson_HW3\\images\\platform.png");

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Compile the vertex shader
	pVSBlob = NULL;
	hr = CompileShaderFromFile(L"shader.fx", "VS_screen", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader_screen);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	pVSBlob = NULL;
	hr = CompileShaderFromFile(L"animate_shader.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pAnimateVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}



	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Compile the pixel shader
	pPSBlob = NULL;
	hr = CompileShaderFromFile(L"shader.fx", "PS_screen", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader_screen);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	pPSBlob = NULL;
	hr = CompileShaderFromFile(L"animate_shader.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pAnimatePixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1,1,0),XMFLOAT2(0,0)},
		{ XMFLOAT3(1,1,0),XMFLOAT2(1,0)},
		{ XMFLOAT3(-1,-1,0),XMFLOAT2(0,1)},
		{ XMFLOAT3(1,1,0),XMFLOAT2(1,0)},
		{ XMFLOAT3(1,-1,0),XMFLOAT2(1,1)},
		{ XMFLOAT3(-1,-1,0),XMFLOAT2(0,1)}
	};

	//initialize d3dx verexbuff:
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer_screen);
	if (FAILED(hr))
		return FALSE;

	ZeroMemory(&bd, sizeof(bd));

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// Set primitive topology
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	SimpleVertex* sv;

	bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;

	for (int i = 0; i < first->objects.size(); i++) {
		gObject& gO = first->objects[i];
		sv = gO.getSV();
		
		bd.ByteWidth = sizeof(SimpleVertex) * gO.getNumVertices();
		//bd.ByteWidth = sizeof(SimpleVertex) * 6;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = gO.getSV();
		hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
		if (FAILED(hr))
			return hr;
		g_VertexBufferList.push_back(g_pVertexBuffer);
		gO.setVB(g_pVertexBuffer);

		// Set vertex buffer
		//UINT stride = sizeof(SimpleVertex);
		//UINT offset = 0;
		//g_pImmediateContext->IASetVertexBuffers(0, 1, &g_VertexBufferList[i], &stride, &offset);
	}

	
	bd.ByteWidth = sizeof(SimpleVertex) * g_player.getNumVertices();
	//bd.ByteWidth = sizeof(SimpleVertex) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = g_player.getSV();
	//InitData.pSysMem = vertices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;
	//g_VertexBufferList.push_back(g_pVertexBuffer);
	g_player.setVB(g_pVertexBuffer);

	


	// Set primitive topology
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// Supply the vertex shader constant data.
	VsConstData.WorldMatrix.x = 0;
	VsConstData.WorldMatrix.y = 0;
	VsConstData.WorldMatrix.z = 1;
	VsConstData.WorldMatrix.w = 1;

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Create the buffer.
	hr = g_pd3dDevice->CreateBuffer(&cbDesc, NULL, &g_pConstantBuffer11);
	if (FAILED(hr))
		return hr;

	for (int i = 0; i < first->objects.size(); i++) {
		gObject &obj = first->objects[i];
		string textureURL = obj.getTexture().getURL();
		std::wstring widestr = std::wstring(textureURL.begin(), textureURL.end());
		const wchar_t* szName = widestr.c_str();

		ID3D11ShaderResourceView* srv = NULL;
				
		hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, szName, NULL, NULL, &srv, NULL);
		if (FAILED(hr))
			return hr;

		obj.getTexture().setSRV(srv);
	}

	/*
	gObject &bg = first->objects[0];
	string textureURL = bg.getTexture().getURL();
	std::wstring widestr = std::wstring(textureURL.begin(), textureURL.end());
	const wchar_t* szName = widestr.c_str();

	
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, szName, NULL, NULL, &g_Texture, NULL);
	if (FAILED(hr))
		return hr;

	bg.getTexture().setSRV(g_Texture);

	*/

	//Player stuff
	/*********************/
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"images\\guy.png", NULL, NULL, &g_playerTexture, NULL);
	if (FAILED(hr))
		return hr;
	g_player.getTexture().setSRV(g_playerTexture);

	/*********************/

	/*
	gObject &grass = first->objects[1];
	textureURL = grass.getTexture().getURL();
	widestr = std::wstring(textureURL.begin(), textureURL.end());
	szName = widestr.c_str();

	if (FAILED(hr))
		return hr;
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, szName, NULL, NULL, &g_grassTexture, NULL);
	if (FAILED(hr))
		return hr;

	grass.getTexture().setSRV(g_grassTexture);
	gObject& grass2 = first->objects[2];
	grass2.getTexture().setSRV(g_grassTexture);
	*/

	// Create the sample state

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_Sampler);
	if (FAILED(hr))
		return hr;

	// Create the screen sample state

	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &SamplerScreen);
	if (FAILED(hr))
		return hr;

	//blendstate:
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.AlphaToCoverageEnable = FALSE;
	blendStateDesc.IndependentBlendEnable = FALSE;
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	g_pd3dDevice->CreateBlendState(&blendStateDesc, &g_BlendState);

	RenderToTexture = new RenderTextureClass;

	RenderToTexture->Initialize(g_pd3dDevice, g_hWnd, -1, -1, FALSE, DXGI_FORMAT_R8G8B8A8_UNORM, TRUE);


	float blendFactor[] = { 0, 0, 0, 0 };
	UINT sampleMask = 0xffffffff;
	g_pImmediateContext->OMSetBlendState(g_BlendState, blendFactor, sampleMask);

	return S_OK;
}



//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
	for (int i = 0; i < g_VertexBufferList.size(); i++) {
		if (g_VertexBufferList[i]) 
			g_VertexBufferList[i]->Release();	
	}
	FreeConsole();

}

///////////////////////////////////
//		This Function is called every time the Left Mouse Button is down
///////////////////////////////////
void OnLBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{

}
///////////////////////////////////
//		This Function is called every time the Right Mouse Button is down
///////////////////////////////////
void OnRBD(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{

}
///////////////////////////////////
//		This Function is called every time a character key is pressed
///////////////////////////////////
void OnChar(HWND hwnd, UINT ch, int cRepeat)
{

}
///////////////////////////////////
//		This Function is called every time the Left Mouse Button is up
///////////////////////////////////
void OnLBU(HWND hwnd, int x, int y, UINT keyFlags)
{


}
///////////////////////////////////
//		This Function is called every time the Right Mouse Button is up
///////////////////////////////////
void OnRBU(HWND hwnd, int x, int y, UINT keyFlags)
{


}
///////////////////////////////////
//		This Function is called every time the Mouse Moves
///////////////////////////////////
void OnMM(HWND hwnd, int x, int y, UINT keyFlags)
{

	if ((keyFlags & MK_LBUTTON) == MK_LBUTTON)
	{
	}

	if ((keyFlags & MK_RBUTTON) == MK_RBUTTON)
	{
	}
}


BOOL OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
{

	return TRUE;
}
void OnTimer(HWND hwnd, UINT id)
{

}

bool canJump = true;
bool canMove = true;

bool a_press = 0;
bool d_press = 0;
bool space_press = 0;
bool left_ = 0;
bool right_ = 0;
bool w_press = 0;
bool s_press = 0;
bool up_press = 0;
bool down_press = 0;

bool moving = 1;

//*************************************************************************
void OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	switch (vk)
	{
	case 87: //w
		w_press = 0;
		break;
	case 65: //a
		a_press = 0;
		break;
	case 83: //s
		s_press = 0;
		break;
	case 68: //d
		d_press = 0;
		break;
	case 32: //space
		space_press = 0;
		break;
	case 37:
		left_ = 0;
		break;
	case 39:
		right_ = 0;
		break;
	case 38: //up
		up_press = 0;
		break;
	case 40: //down
		down_press = 0;
		break;
	default:break;

	}

}
void OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{

	switch (vk)
	{
	case 27:
		PostQuitMessage(0);
		break;
	case 87: //w
		w_press = 1;
		break;
	case 65: //a
		a_press = 1;
		break;
	case 83: //s
		s_press = 1;
		break;
	case 68: //d
		d_press = 1;
		break;
	case 32: //space
		space_press = 1;
		break;
	case 38: //up
		up_press = 1;
		break;
	case 37:
		left_ = 1;
		moving = 0; 
		break;
	case 39:
		right_ = 1;
		moving = 1;
		break;
	case 40: //down
		down_press = 1;
		break;
	case 80: //p
		printStuff();
		break;
	case 90: //z
		start_music(L"sound\\giveyouup.mp3");
		break;
	default:break;
	}
}



//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	SCROLLINFO si;



	switch (message)
	{
		/*
		#define HANDLE_MSG(hwnd, message, fn)    \
		case (message): return HANDLE_##message((hwnd), (wParam), (lParam), (fn))
		*/

		//HANDLE_MSG(hwnd, WM_CHAR, OnChar);
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLBD);
		HANDLE_MSG(hwnd, WM_LBUTTONUP, OnLBU);
		HANDLE_MSG(hwnd, WM_MOUSEMOVE, OnMM);
		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
		HANDLE_MSG(hwnd, WM_KEYDOWN, OnKeyDown);
		HANDLE_MSG(hwnd, WM_KEYUP, OnKeyUp);

	case WM_ERASEBKGND:
		return (LRESULT)1;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
void Update()
{
	x_impulse = 0.0f;
	
	if (isJumping) {
		y_impulse -= 0.0001f;
	}
	else {
		if (y_impulse > -MAX_VEL)
			y_impulse -= 0.0001f;
		else
			y_impulse = -MAX_VEL;
	}
	//--------------------------------------------------------------------------------------
	// ***** CONTROLLER STUFF ***** //
	//--------------------------------------------------------------------------------------

	if (gamepad->IsConnected())
	{
		if (gamepad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) {}
		if (gamepad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
		{

			space_press = true;
		}
		else {
			space_press = false;
		}
			//cam.s = 0;

	}


	SHORT lx = gamepad->GetState().Gamepad.sThumbLX;
	SHORT ly = gamepad->GetState().Gamepad.sThumbLY;


	if (abs(ly) > 3000)
	{
		float angle_x = (float)ly / 32000.0;
		angle_x *= 0.05;
		//y_impulse += angle_x;
	}
	if (abs(lx) > 3000)
	{
		float angle_y = (float)lx / 32000.0;
		angle_y *= 0.01;
		x_impulse += angle_y;
	}

	//--------------------------------------------------------------------------------------
	// ***** PLAYER MOVEMENT ***** //
	//--------------------------------------------------------------------------------------

		//Jumping
	if (space_press && !isJumping) {
		y_impulse = 0.1;
		isJumping = true;
	}
	

		//Left and right controls with a & d
	if (a_press && d_press)
	{
		x_impulse = 0;
		g_player.changeState(animation::IDLE);
	}
	else if (a_press)
	{
		x_impulse = -0.05;
		g_player.changeState(animation::WALK,animation::LEFT);
	}
	else if (d_press)
	{
		x_impulse = 0.05;
		g_player.changeState(animation::WALK,animation::RIGHT);
	}
	else if (a_press || d_press)
	{
		x_impulse = 0;
	}
	else
	{
		g_player.changeState(animation::IDLE);
	}

	

		//Left and right controls with left arrow & and right arrow
	if (left_ && right_) x_impulse = 0;
	else if (left_) x_impulse = -0.01;
	else if (right_) x_impulse = 0.01;
	else if (left_ || right_) x_impulse = 0;

	SimpleVertex* sv = g_player.getSV();

	float scrollX = WIDTH / 2 - 10 / 2 - sv[0].Pos.x;
	float scrollY = HEIGHT / 2 - 10 / 2 - sv[0].Pos.y;

	//g_player.translate(x_impulse, y_impulse, 0, 0);
	XMFLOAT4* xPos = new XMFLOAT4(-x_impulse, 0, 0, 0);
	//g_player.getCol()->translate(pos);

	// x translation
	for (int i = 0; i < first->objects.size(); i++) {
		gObject &gO = first->objects[i];
		gO.translate(-x_impulse, 0, 0, 0);
		if (gO.getCol() != NULL) {
			gO.getCol()->translate(xPos);
			
			//cout << gO.getCol()->getPos()->x << " " << gO.getCol()->getPos()->y << " " << gO.getCol()->getPos()->z << endl;
			//cout << g_player.getCol()->getPos()->x << " " << g_player.getCol()->getPos()->y << " " << g_player.getCol()->getPos()->z << endl;
			//g_player.getCol()->translate(xPos);
		}

	}

	
	

	// check for player collisions
	//cout << y_impulse << endl;
	for (int j = 0; j < first->objects.size(); j++) {

		if (first->objects[j].getCol() != NULL && g_player.getCol()->collision(first->objects[j].getCol())) {
			//g_player.translate(-x_impulse, -y_impulse, 0, 0);
			//g_player.getCol()->translate(new XMFLOAT4(-x_impulse, -y_impulse, 0, 0));

			//move all objects in the world
			for (int i = 0; i < first->objects.size(); i++) {
				gObject &gO = first->objects[i];
				gO.translate(x_impulse, 0, 0, 0);
				xPos->x = x_impulse;
				if (gO.getCol() != NULL) {
					gO.getCol()->translate(xPos);
					//g_player.getCol()->translate(xPos);
				}

			}
		}

	}


	XMFLOAT4* yPos = new XMFLOAT4(0, -y_impulse, 0, 0);

	// y translation
	for (int i = 0; i < first->objects.size(); i++) {
		gObject &gO = first->objects[i];
		gO.translate(0, -y_impulse, 0, 0);
		if (gO.getCol() != NULL) {
			gO.getCol()->translate(yPos);
			//g_player.getCol()->translate(yPos);
		}

	}

	// check for player collisions
	//cout << y_impulse << endl;
	for (int j = 0; j < first->objects.size(); j++) {

		if (first->objects[j].getCol() != NULL && g_player.getCol()->collision(first->objects[j].getCol())) {
			isJumping = false;

			//g_player.translate(-x_impulse, -y_impulse, 0, 0);
			//g_player.getCol()->translate(new XMFLOAT4(-x_impulse, -y_impulse, 0, 0));

			//move all objects in the world
			for (int i = 0; i < first->objects.size(); i++) {
				gObject &gO = first->objects[i];
				gO.translate(0, y_impulse, 0, 0);
				yPos->y = y_impulse;
				if (gO.getCol() != NULL) {
					gO.getCol()->translate(yPos);
					//g_player.getCol()->translate(yPos);
				}

			}
			y_impulse = 0.0f;
		}
	}

}

void Render_To_Texture(long elapsed) {

	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	float scale_factor = 1.0;

	ID3D11RenderTargetView*			RenderTarget;

	RenderTarget = RenderToTexture->GetRenderTarget();
	g_pImmediateContext->ClearRenderTargetView(RenderTarget, ClearColor);
	g_pImmediateContext->OMSetRenderTargets(1, &RenderTarget, NULL);

	//for all non-animated rectangles:
	//g_pImmediateContext->PSSetSamplers(0, 1, &g_Sampler);
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);


	scale_factor = 0.01;

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	for (int i = 0; i < first->objects.size(); i++) {
		gObject &gO = first->objects[i];
		gO.draw(g_pImmediateContext, g_pConstantBuffer11, elapsed, g_Sampler);

	}

	//for animated objects:
	//g_pImmediateContext->PSSetSamplers(0, 1, &g_Sampler);
	g_pImmediateContext->VSSetShader(g_pAnimateVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pAnimatePixelShader, NULL, 0);

	g_player.draw(g_pImmediateContext, g_pConstantBuffer11, elapsed, g_Sampler);

	// Present the information rendered to the back buffer to the front buffer (the screen)
	g_pSwapChain->Present(0, 0);

}


void Render_To_Screen() {
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
	// Clear the back buffer
	float ClearColor2[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // red, green, blue, alpha

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor2);
	// Clear the depth buffer to 1.0 (max depth)

	VS_CONSTANT_BUFFER constantbuffer;
	constantbuffer.WorldMatrix = XMFLOAT4(0,0,0,0);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer11, 0, NULL, &constantbuffer, 0, 0);
	// Render screen
	g_pImmediateContext->VSSetShader(g_pVertexShader_screen, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader_screen, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer11); 
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer11);

	ID3D11ShaderResourceView*           texture = RenderToTexture->GetShaderResourceView();// THE MAGIC
																						  //texture = g_pTextureRV;
	g_pImmediateContext->GenerateMips(texture);

	g_pImmediateContext->PSSetShaderResources(0, 1, &texture);
	g_pImmediateContext->VSSetShaderResources(0, 1, &texture);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer_screen, &stride, &offset);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_Sampler);
	g_pImmediateContext->VSSetSamplers(0, 1, &g_Sampler);

	g_pImmediateContext->Draw(6, 0);

	g_pSwapChain->Present(0, 0);
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------


void Render()
{

	static StopWatchMicro_ stopwatch;
	long elapsed = stopwatch.elapse_micro();
	//stopwatch.start();//restart

	Render_To_Texture(elapsed);
	Render_To_Screen();

	
	g_pSwapChain->Present(0, 0);

	
}

/*
void CheckCollisions()
{
	for (int i = 0; i < first->objects.size(); i++) 
	{
		gObject &gO = first->objects[i];
		if (gO.nearOrigin(WIDTH,HEIGHT))
		{
			if (g_player.collidesWith(&gO))
			{

			}
		}
	}

}
*/

void printStuff() {
	collider* otherCol = first->objects[1].getCol();
	collider* playerCol = g_player.getCol();

	SimpleVertex* otherVerts = otherCol->getSV();
	SimpleVertex point0 = otherVerts[0];
	XMFLOAT3 p0 = point0.Pos;

	SimpleVertex point1 = otherVerts[1];
	XMFLOAT3 p1 = point1.Pos;
	SimpleVertex point2 = otherVerts[2];
	XMFLOAT3 p2 = point2.Pos;

	SimpleVertex* playerPos = playerCol->getSV();
	SimpleVertex playerTL = playerPos[0];
	XMFLOAT3 v = playerTL.Pos;

	cout << "p0 x: " << p0.x << " y: " << p0.y << endl;
	cout << "p1 x: " << p1.x << " y: " << p1.y << endl;
	cout << "p2 x: " << p2.x << " y: " << p2.y << endl;
	cout << "player top left x: " << v.x << " y: " << v.y << endl;

	if (g_player.getCol()->collision(first->objects[1].getCol())) {
		cout << "suh dude" << endl;
	}
	else
		cout << "nah dude" << endl;

}