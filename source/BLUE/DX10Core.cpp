#include "stdafx.h"

#include <tchar.h>

#include <DXErr.h>

#include "DX10Core.h"
#include "DXCore.h"
#include "DXUtils.h"
#include "MeshObject.h"
#include "Object.h"

namespace BLUE
{

	const D3D10_DEPTH_STENCIL_DESC defaultDepthStencilStateDesc = 
	{
		true,								// DepthEnable					
		D3D10_DEPTH_WRITE_MASK_ALL,			// DepthWriteMask				
		D3D10_COMPARISON_LESS_EQUAL,		// DepthFunc						
											
		false,								// StencilEnable				
		0xff,								// StencilReadMask				
		0xff,								// StencilWriteMask				
		{
			D3D10_STENCIL_OP_ZERO,			// FrontFace.StencilFailOp		
			D3D10_STENCIL_OP_ZERO,			// FrontFace.StencilDepthFailOp			
			D3D10_STENCIL_OP_ZERO,			// FrontFace.StencilPassOp	
			D3D10_COMPARISON_LESS_EQUAL		// FrontFace.StencilFunc	
		},
		{	
			D3D10_STENCIL_OP_ZERO,			// BackFace.StencilFailOp		
			D3D10_STENCIL_OP_ZERO,			// BackFace.StencilDepthFailOp		
			D3D10_STENCIL_OP_ZERO,			// BackFace.StencilPassOp	
			D3D10_COMPARISON_LESS_EQUAL		// BackFace.StencilFunc			
		}
	};

IDXCore * CDX10Core::GetInstance()
{
	if( g_pDXCore )
		return g_pDXCore;
	else
		return ( g_pDXCore = new CDX10Core );		
}

CDX10Core::CDX10Core()
: IDXCore()
, m_pFont( NULL )
, m_bShowEngineInfo( false )
{	
	m_pDevice					= NULL;
	m_pSwapChain				= NULL;	
	m_pRenderTargetView			= NULL;	
	m_pDepthStencilView			= NULL;	

	m_pDepthStencilState = NULL;
	m_pRasterizerState = NULL;
	
	m_pBasicEffect						= NULL;
	m_pWorldMatrixEffectVariable		= NULL;
	m_pProjectionMatrixEffectVariable	= NULL;
	m_pViewMatrixEffectVariable			= NULL;

	memset(m_pVertexBuffers, NULL, sizeof(ID3D10Buffer*) * VBT_COUNT);	
	m_pIndexBuffer	= NULL;

	currentVBPos = 0;
	currentIBPos = 0;
	
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProjection);
}

CDX10Core::~CDX10Core( void )
{
	CleanupDevice();
}

void CDX10Core::CleanupDevice()
{	

	SAFE_DX_RELEASE( m_pFont );

	SAFE_DX_RELEASE( m_pBasicEffect);

	SAFE_DX_RELEASE( m_pRenderTargetView );
	SAFE_DX_RELEASE( m_pDepthStencilView );
	
	for(int i = 0; i < VBT_COUNT; ++i)
	{
		SAFE_DX_RELEASE( m_pVertexBuffers[i] );
	}
	SAFE_DX_RELEASE( m_pIndexBuffer );

	SAFE_DX_RELEASE( m_pRasterizerState );
	SAFE_DX_RELEASE( m_pDepthStencilState );

	SAFE_DX_RELEASE( m_pSwapChain );		
	SAFE_DX_RELEASE( m_pDevice );
}

void CDX10Core::ResetDevice()
{
	CleanupDevice();
	Init( m_hWnd, m_initDesc );	
}

void *CDX10Core::GetDevice()
{
	return m_pDevice;
}

void *CDX10Core::GetSwapChain()
{
	return m_pSwapChain;
}

void CDX10Core::InitBasicEffects()
{
	HRESULT hr = D3DX10CreateEffectFromFile( _T("effects/BasicShaders.fx"),
		NULL, NULL,
		"fx_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, m_pDevice,
		NULL, NULL,
		&m_pBasicEffect,
		NULL, NULL);
	if ( FAILED ( hr ) )
	{
		MessageBox(m_hWnd, _T("Could not load effect file!"), _T("DirectX 10 shader Error"), MB_OK );
		exit(1);
	}

	m_pBasicTechnique = m_pBasicEffect->GetTechniqueByName("BasicRender");
	
	m_pViewMatrixEffectVariable			= m_pBasicEffect->GetVariableByName( "View" )->AsMatrix();
	m_pProjectionMatrixEffectVariable	= m_pBasicEffect->GetVariableByName( "Projection" )->AsMatrix();
	m_pWorldMatrixEffectVariable		= m_pBasicEffect->GetVariableByName( "World" )->AsMatrix();
}

void CDX10Core::InitMatrices(UINT width, UINT height)
{
	D3DXVECTOR3 camera[3] = {
		D3DXVECTOR3(0.0f, 0.0f, -5.0f),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f) 
	};
	
	D3DXMatrixIdentity(&m_matWorld);

	D3DXMatrixLookAtLH(&m_matView, &camera[0], &camera[1], &camera[2]);

	D3DXMatrixPerspectiveFovLH(&m_matProjection, (float)D3DX_PI * 0.5f, (float)width/(float)height, 0.1f, 500.0f);
}

bool CDX10Core::Init( HWND hWnd )
{
	m_hWnd = hWnd;

	SetDefaultSettings();

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = m_initDesc.bufferCount;
	sd.BufferDesc.Width = m_initDesc.bufferDesc.width;
	sd.BufferDesc.Height = m_initDesc.bufferDesc.height;
	sd.BufferDesc.Format = ( DXGI_FORMAT ) m_initDesc.bufferDesc.format;
	sd.BufferDesc.RefreshRate.Numerator = m_initDesc.bufferDesc.refreshRate.numerator;
	sd.BufferDesc.RefreshRate.Denominator = m_initDesc.bufferDesc.refreshRate.denominator;
	sd.BufferDesc.ScanlineOrdering			= ( DXGI_MODE_SCANLINE_ORDER ) m_initDesc.bufferDesc.scanlineOrdering;
	sd.BufferDesc.Scaling					= ( DXGI_MODE_SCALING )m_initDesc.bufferDesc.scaling;
	sd.BufferUsage = m_initDesc.bufferUsage;
	sd.OutputWindow = m_initDesc.hOutputWindow;
	sd.SampleDesc.Count = m_initDesc.sampleDesc.count;
	sd.SampleDesc.Quality = m_initDesc.sampleDesc.quality;
	sd.Windowed = m_initDesc.bWindowed;
	sd.Flags = m_initDesc.flags;
	sd.SwapEffect = ( DXGI_SWAP_EFFECT ) m_initDesc.swapEffect;
		
	UINT createDeviceFlags	= 0;
//#if defined(DEBUG) || defined(_DEBUG)  
//	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
//#endif

	HR( D3D10CreateDeviceAndSwapChain1( 
		NULL, 
		D3D10_DRIVER_TYPE_HARDWARE, 
		NULL, 
		createDeviceFlags, 
		D3D10_FEATURE_LEVEL_10_1, 
		D3D10_1_SDK_VERSION, 
		&sd, 
		&m_pSwapChain, 
		&m_pDevice ) );

	InitRenderAndDepthStencilTargets( sd.BufferDesc.Width, sd.BufferDesc.Height );

	// Set the viewport transform.
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = sd.BufferDesc.Width;
	vp.Height   = sd.BufferDesc.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pDevice->RSSetViewports( 1, &vp );

	InitMatrices(vp.Width, vp.Height);

	InitRasterizerState();
	InitDepthStencilState();

	InitBasicEffects();

	InitializeBuffers();

	InitFont();

	m_bInitialized = true;
	return true;
}

bool CDX10Core::Init( HWND hWnd, const DXInitDesc &initDesc )
{
	m_bInitialized = false;
	// Save new window handle for future use
	m_hWnd = hWnd;

	// Save new init description for future use
	m_initDesc = initDesc;

	// Use init description in building specific init description
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = initDesc.bufferCount;
	sd.BufferDesc.Width = initDesc.bufferDesc.width;
	sd.BufferDesc.Height = initDesc.bufferDesc.height;
	sd.BufferDesc.Format = ( DXGI_FORMAT )initDesc.bufferDesc.format;
	sd.BufferDesc.RefreshRate.Numerator = initDesc.bufferDesc.refreshRate.numerator;
	sd.BufferDesc.RefreshRate.Denominator = initDesc.bufferDesc.refreshRate.denominator;
	sd.BufferUsage = initDesc.bufferUsage;
	sd.OutputWindow = initDesc.hOutputWindow;
	sd.SampleDesc.Count = initDesc.sampleDesc.count;
	sd.SampleDesc.Quality = initDesc.sampleDesc.quality;
	sd.Windowed = initDesc.bWindowed;

	// Create d3d device, swap chain
	
	HR( D3D10CreateDeviceAndSwapChain1( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0,
		D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice ) );
	
	InitRenderAndDepthStencilTargets( sd.BufferDesc.Width, sd.BufferDesc.Height );

	// Set the viewport transform.
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = sd.BufferDesc.Width;
	vp.Height   = sd.BufferDesc.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pDevice->RSSetViewports( 1, &vp );

	InitMatrices(vp.Width, vp.Height);

	InitRasterizerState();

	InitDepthStencilState();

	InitBasicEffects();

	InitializeBuffers();

	InitFont();

	m_bInitialized = true;
	return true;
}

void CDX10Core::InitRenderAndDepthStencilTargets(int width, int height)
{
	ID3D10Texture2D* pBuffer = NULL;
	HR( m_pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBuffer ) );
	
	// Create a render target view
	HR( m_pDevice->CreateRenderTargetView( pBuffer, NULL, &m_pRenderTargetView ) );

	pBuffer->Release();	
	
	// Create DepthStencil target view
	D3D10_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = width;
	depthStencilDesc.Height    = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;
		
	HR( m_pDevice->CreateTexture2D( &depthStencilDesc, 0, &pBuffer ) );	
	HR( m_pDevice->CreateDepthStencilView( pBuffer, 0, &m_pDepthStencilView ) );
	
	pBuffer->Release();
	
	m_pDevice->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );

}

void CDX10Core::InitDepthStencilState()
{
	D3D10_DEPTH_STENCIL_DESC depthStencilStateDesc = defaultDepthStencilStateDesc;
	HR( m_pDevice->CreateDepthStencilState( &depthStencilStateDesc, &m_pDepthStencilState ) );
	m_pDevice->OMSetDepthStencilState( m_pDepthStencilState, 0 );

}

void CDX10Core::ApplyDepthStencilState()
{
	m_pDevice->OMSetDepthStencilState( m_pDepthStencilState, 0 );
}

void CDX10Core::InitRasterizerState()
{	
	D3D10_RASTERIZER_DESC rasterizerState;
	rasterizerState.CullMode = D3D10_CULL_BACK;
	rasterizerState.FillMode = D3D10_FILL_SOLID;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;

	m_pDevice->CreateRasterizerState( &rasterizerState, &m_pRasterizerState);
}

void CDX10Core::BeginDraw()
{
	// Just clear the backbuffer
	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1 }; //red,green,blue,alpha
	m_pDevice->ClearRenderTargetView( m_pRenderTargetView, D3DXCOLOR(0xffffffff) );			
	m_pDevice->ClearDepthStencilView( m_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0 );

	ApplyDepthStencilState();
	ApplyRasterizerState();
}

void CDX10Core::EndDraw()
{	
	HR( m_pSwapChain->Present( 0, 0 ) );
}

void CDX10Core::SetWireframe(bool bWireframe)
{
	D3D10_RASTERIZER_DESC rasterizerStateDesc;
	m_pRasterizerState->GetDesc(&rasterizerStateDesc);
	rasterizerStateDesc.FillMode = bWireframe ? D3D10_FILL_WIREFRAME: D3D10_FILL_SOLID;
		
	SAFE_DX_RELEASE(m_pRasterizerState);
	m_pDevice->CreateRasterizerState( &rasterizerStateDesc, &m_pRasterizerState);	
}

void CDX10Core::ApplyRasterizerState()
{
	m_pDevice->RSSetState(m_pRasterizerState);
}

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 color;

	Vertex( D3DXVECTOR3 p, D3DXVECTOR4 c ) : pos(p), color(c) {}
};

void CDX10Core::InitializeBuffers()
{
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

	const UINT maxNumVertices = 150000;

	UINT numElements = 3;
	D3D10_PASS_DESC passDesc = {0};
	m_pBasicTechnique->GetPassByIndex( 0 )->GetDesc( &passDesc );

	m_pVertexLayout = NULL;
	if ( FAILED( m_pDevice->CreateInputLayout( layout,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pVertexLayout ) ) ) 
	{
		MessageBox(m_hWnd, _T("Error creating input layout"), _T("DX render error"), MB_OK);
		return;
	}
	
	//create vertex buffer
	//---------------------------------------------
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;	
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	bd.ByteWidth = sizeof( D3DXVECTOR3 ) * maxNumVertices; //total size of buffer in bytes
	if ( FAILED( m_pDevice->CreateBuffer( &bd, NULL, &m_pVertexBuffers[VBT_POSITION0] ) ) ) 
	{
		MessageBox(m_hWnd, _T("Could not create vertex positions buffer!"), _T("DX render error"), MB_OK);
		return;
	}
	
	bd.ByteWidth = sizeof( D3DXVECTOR4 ) * maxNumVertices; //total size of buffer in bytes
	if ( FAILED( m_pDevice->CreateBuffer( &bd, NULL, &m_pVertexBuffers[VBT_COLOR0] ) ) ) 
	{
		MessageBox(m_hWnd, _T("Could not create vertex colors buffer!"), _T("DX render error"), MB_OK);
		return;
	}

	bd.ByteWidth = sizeof( D3DXVECTOR3 ) * maxNumVertices; //total size of buffer in bytes
	if ( FAILED( m_pDevice->CreateBuffer( &bd, NULL, &m_pVertexBuffers[VBT_NORMAL0] ) ) ) 
	{
		MessageBox(m_hWnd, _T("Could not create vertex normals buffer!"), _T("DX render error"), MB_OK);
		return;
	}

	D3DXVECTOR4 *v = NULL;
	m_pVertexBuffers[VBT_COLOR0]->Map(D3D10_MAP_WRITE_DISCARD, 0, (void **)&v );
	for(UINT i = 0; i < maxNumVertices; ++i)
	{		
		v[i] = D3DXVECTOR4(0,0, 1, 1);
	}
	m_pVertexBuffers[VBT_COLOR0]->Unmap();	

	//create index buffer
	//---------------------------------------------	
	//UINT numIndices = pObj->m_pMesh->m_numIndices;
	bd.ByteWidth = sizeof( UINT ) * maxNumVertices; //total size of buffer in bytes
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	if ( FAILED( m_pDevice->CreateBuffer( &bd, NULL, &m_pIndexBuffer ) ) ) 
	{
		MessageBox(m_hWnd, _T("Could not create index buffer!"), _T("DX render error"), MB_OK);
	}

	currentIBPos = 0;
	currentVBPos = 0;
}

void CDX10Core::FillBuffers(CMeshObject *pObj)
{

	UINT numVertices = pObj->m_pMesh->m_numVertices;
	UINT numIndices = pObj->m_pMesh->m_numIndices;

	//fill vertex buffers with vertex data
	void* v = NULL;

	//lock vertex buffer for CPU use
	m_pVertexBuffers[VBT_POSITION0]->Map(D3D10_MAP_WRITE_DISCARD, 0, &v );
	memcpy(v, pObj->m_pMesh->m_pVertices, sizeof(D3DXVECTOR3) * numVertices);
	m_pVertexBuffers[VBT_POSITION0]->Unmap();

	m_pVertexBuffers[VBT_NORMAL0]->Map(D3D10_MAP_WRITE_DISCARD, 0, &v );
	memcpy(v, pObj->m_pMesh->m_pNormals, sizeof(D3DXVECTOR3) * numVertices);
	m_pVertexBuffers[VBT_NORMAL0]->Unmap();

	
	//fill vertex buffer with vertices
	UINT* indices = NULL;

	//lock index buffer for CPU use	
	m_pIndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**) &indices );
	memcpy(indices, pObj->m_pMesh->m_pIndices, sizeof(UINT) * numIndices);
	m_pIndexBuffer->Unmap();

}

void CDX10Core::RenderMeshObject(CMeshObject *pObj, float dt)
{
	static float timeSpent = 0;
	timeSpent+= dt;
	if(!pObj || !pObj->m_pMesh || !pObj->m_pMesh->m_numVertices)
		return;

	FillBuffers(pObj);	

	D3DXMATRIX rotMatY, rotMatX, translate;
	D3DXMatrixRotationY(&rotMatY, 0.01f * 3.1416f * timeSpent);
	D3DXMatrixRotationX(&rotMatX,  0.3416f );
	D3DXMatrixTranslation(&translate, 0, 0, 80);
	m_matWorld = D3DXMATRIX((float *)pObj->m_matGlobal) * rotMatX * rotMatY * translate ;	

	m_pWorldMatrixEffectVariable->SetMatrix(m_matWorld);
	m_pViewMatrixEffectVariable->SetMatrix(m_matView);
	m_pProjectionMatrixEffectVariable->SetMatrix(m_matProjection);

	// Set the input layout
	m_pDevice->IASetInputLayout( m_pVertexLayout );

	ID3D10Buffer * pVBs[]={ m_pVertexBuffers[VBT_POSITION0], m_pVertexBuffers[VBT_COLOR0], m_pVertexBuffers[VBT_NORMAL0]};
	UINT strides[] = {sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR4), sizeof(D3DXVECTOR3)};
	UINT offsets[] = {0, 0, 0};
	m_pDevice->IASetVertexBuffers( 0, 3, pVBs, strides, offsets );

	// Set index buffer	
	m_pDevice->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Set primitive topology 
	m_pDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );	


	//get technique desc
	D3D10_TECHNIQUE_DESC techDesc;
	m_pBasicTechnique->GetDesc( &techDesc );

	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		//apply technique
		m_pBasicTechnique->GetPassByIndex( p )->Apply( 0 );

		//draw
		m_pDevice->DrawIndexed( pObj->m_pMesh->m_numIndices, 0, 0 );
	}	
}

void CDX10Core::Render( CObject *pObject, float dt )
{
	if( dt == 0 )
		return;

	switch(pObject->GetObjectType())
	{
	case OT_MESH:
		RenderMeshObject((CMeshObject *) pObject, dt);
		break;
	}
}

void CDX10Core::OnResize(int width, int height)
{		
	SAFE_DX_RELEASE( m_pRenderTargetView);
	SAFE_DX_RELEASE( m_pDepthStencilView);	
	
	// Resize the swap chain and recreate the render target view.

	HR(m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		
	InitRenderAndDepthStencilTargets( width, height );

	// Set the viewport transform.
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width    = width;
	vp.Height   = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pDevice->RSSetViewports( 1, &vp );

	InitMatrices(vp.Width, vp.Height);
	
	InitDepthStencilState();

	InitFont();

}

void CDX10Core::InitFont()
{
	// Create font
	if( m_pFont )
		SAFE_DX_RELEASE( m_pFont );

	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height          = 25;
	fontDesc.Width           = 12;
	fontDesc.Weight          = 0;
	fontDesc.MipLevels       = 1;
	fontDesc.Italic          = false;
	fontDesc.CharSet         = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality         = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy_s( fontDesc.FaceName, 32, _T( "Calibri" ) );

	HR( D3DX10CreateFontIndirect( m_pDevice, &fontDesc, &m_pFont ) );
}

void CDX10Core::OutputText( const TCHAR *text, float left, float top, DWORD color )
{	
	RECT rc ={ ( int )left, ( int )top, 0, 0 };
	
	m_pFont->DrawText( 0, text, -1, &rc, DT_NOCLIP, D3DXCOLOR( color ) );	
}

}//end namespace
