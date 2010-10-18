#include "stdafx.h"

#include "DX11TextComponent.h"
#include "DXUtils.h"
#include <tchar.h>
#include <assert.h>
#include <d3d11.h>

namespace BLUE
{

const D3D11_INPUT_ELEMENT_DESC CDX11TextComponent::s_InputLayout[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

const SimpleVertex CDX11TextComponent::s_VertexArray[] =
{
    { CVector3( -1.0f, -1.0f, 1.0f ), CVector2( 1.0f, 1.0f )},
    { CVector3(  1.0f, -1.0f, 1.0f ), CVector2( 0.0f, 1.0f )},
    { CVector3(  1.0f,  1.0f, 1.0f ), CVector2( 0.0f, 0.0f )},
    { CVector3( -1.0f,  1.0f, 1.0f ), CVector2( 1.0f, 0.0f )}
};

const SHORT CDX11TextComponent::s_FacesIndexArray[] =
{
    3, 1, 0,
    2, 1, 3
};

CDX11TextComponent::CDX11TextComponent() :
m_pD2DFactory( NULL ),
m_pRenderTarget( NULL ),
m_pTextBrush( NULL ),
m_pDWriteFactory( NULL ),
m_pTextFormat( NULL ),
m_pTextTexture( NULL ),
m_pTextureResourceView( NULL )
{

}

CDX11TextComponent::~CDX11TextComponent()
{
	SAFE_DX_RELEASE( m_pD2DFactory )
	SAFE_DX_RELEASE( m_pRenderTarget )
	SAFE_DX_RELEASE( m_pTextBrush )
	SAFE_DX_RELEASE( m_pDWriteFactory )
	SAFE_DX_RELEASE( m_pTextFormat )
	SAFE_DX_RELEASE( m_pTextTexture )
	SAFE_DX_RELEASE( m_pTextureResourceView )
}

void CDX11TextComponent::Initialize( ID3D11Device *pDevice )
{
	//get the dpi information
	HDC screen = GetDC( 0 );
	m_fDpiScaleX = GetDeviceCaps( screen, LOGPIXELSX ) / 96.0f;
	m_fDpiScaleY = GetDeviceCaps( screen, LOGPIXELSY ) / 96.0f;
	ReleaseDC( 0, screen );

	HRESULT hr;

	// Create Direct2D factory.

	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&m_pD2DFactory
		);


	// Create a shared DirectWrite factory.

	if( SUCCEEDED( hr ) )
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof( IDWriteFactory ),
			reinterpret_cast<IUnknown**>( &m_pDWriteFactory )
			);
	}

	if( SUCCEEDED( hr ) )
	{
		hr = m_pDWriteFactory->CreateTextFormat(
			_T( "Times New Roman" ),                // Font family name.
			NULL,                       // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			72.0f,
			_T( "en-us" ),
			&m_pTextFormat
			);
	}



	// Center align (horizontally) the text.
	if( SUCCEEDED( hr ) )
	{
		hr = m_pTextFormat->SetTextAlignment( DWRITE_TEXT_ALIGNMENT_CENTER );
	}

	if( SUCCEEDED( hr ) )
	{
		hr = m_pTextFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
	}

	CreateDeviceResources( pDevice );

}

void CDX11TextComponent::SetFont( TCHAR *fontFamily, FontWeight eWeight, FontStyle eStyle, FontStretch eStretch, float fSize)
{
	DWRITE_TEXT_ALIGNMENT textAlignment = m_pTextFormat->GetTextAlignment();

	SAFE_DX_RELEASE( m_pTextFormat )
	
	HRESULT hr = m_pDWriteFactory->CreateTextFormat(
		fontFamily,                
		NULL,		// Font collection (NULL sets it to use the system font collection).
		( DWRITE_FONT_WEIGHT ) eWeight,
		( DWRITE_FONT_STYLE ) eStyle,
		( DWRITE_FONT_STRETCH ) eStretch,
		fSize,
		_T( "en-us" ),
		&m_pTextFormat
		);
	
	if ( SUCCEEDED( hr ) )
	{
		hr = m_pTextFormat->SetParagraphAlignment( DWRITE_PARAGRAPH_ALIGNMENT_CENTER );
	}

	assert( SUCCEEDED( hr ) );
	if(FAILED(hr))
	{
		SAFE_DX_RELEASE( m_pTextFormat )	
	}
}

void CDX11TextComponent::GetFontFamily( TCHAR *fontFamilyName, unsigned int nSize )
{
	m_pTextFormat->GetFontFamilyName( fontFamilyName, nSize );
}

CDX11TextComponent::FontWeight CDX11TextComponent::GetFontWeight()
{
	return ( FontWeight ) m_pTextFormat->GetFontWeight();	
}

CDX11TextComponent::FontStyle CDX11TextComponent::GetFontStyle()
{
	return ( FontStyle ) m_pTextFormat->GetFontStyle();
}

CDX11TextComponent::FontStretch CDX11TextComponent::GetFontStretch()
{
	return ( FontStretch ) m_pTextFormat->GetFontStretch();
}

float CDX11TextComponent::GetFontSize()
{
	return m_pTextFormat->GetFontSize();
}

void CDX11TextComponent::SetTextAlignment( CDX11TextComponent::TextAlignment eTextAlignment)
{
	m_pTextFormat->SetTextAlignment( (DWRITE_TEXT_ALIGNMENT)eTextAlignment);
}

CDX11TextComponent::TextAlignment CDX11TextComponent::GetTextAlignment()
{
	return ( TextAlignment ) m_pTextFormat->GetTextAlignment();
}

void CDX11TextComponent::DrawTextToSurface( const TCHAR *text, float left, float top, float right, float bottom, bool bClearRect )
{
	if( !m_pRenderTarget )
		return;

	m_pRenderTarget->BeginDraw();
	m_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
	if( bClearRect )
	{			
		m_pRenderTarget->Clear( D2D1::ColorF( D2D1::ColorF::White, 0.0f ) );
	}

	D2D1_RECT_F layoutRect = D2D1::RectF(
		left / m_fDpiScaleX,
		top / m_fDpiScaleY,
		(right - left) / m_fDpiScaleX,
		(bottom - top) / m_fDpiScaleY
		);

	m_pRenderTarget->DrawText(
		text,        // The string to render.
		_tcslen( text ),    // The string's length.
		m_pTextFormat,    // The text format.
		layoutRect,       // The region of the window where the text will be rendered.
		m_pTextBrush     // The brush used to draw the text.
		);

	m_pRenderTarget->EndDraw();
}

void CDX11TextComponent::DrawTextSurface( ID3D11DeviceContext *pDeviceContext )
{
	pDeviceContext->VSSetShader( m_pTextVS, NULL, 0 );    
    pDeviceContext->PSSetShader( m_pTextPS, NULL, 0 );
	pDeviceContext->PSSetShaderResources( 0, 1, &m_pTextureResourceView );
	{
		// Set vertex buffer
        UINT stride = sizeof( SimpleVertex );
        UINT offset = 0;            

        pDeviceContext->IASetVertexBuffers(
			0, // StartSlot
			1, // NumBuffers            
			&m_pVertexBuffer,
            &stride,
            &offset
        );
	}
	
}

void CDX11TextComponent::CreateDeviceResources( ID3D11Device *pDevice )
{
	if( !pDevice )
		return;
	
	HRESULT hr = S_OK;
	


	// Allocate an offscreen D3D surface for D2D to render the text into
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.Height = 512;
    texDesc.Width = 512;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

    hr = pDevice->CreateTexture2D( &texDesc, NULL, &m_pTextTexture );
	if( SUCCEEDED( hr ) )
    {
        // Convert the Direct2D texture into a Shader Resource View
        SAFE_DX_RELEASE( m_pTextureResourceView );
        hr = pDevice->CreateShaderResourceView( m_pTextTexture, NULL, &m_pTextureResourceView );
    }

	if( SUCCEEDED( hr ) )
    {
        D3D11_BUFFER_DESC bd;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof( s_VertexArray );
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = s_VertexArray;

        hr = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);        
    }
    if( SUCCEEDED( hr ) )
    {
        D3D11_BUFFER_DESC bd;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof( s_FacesIndexArray );
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = s_FacesIndexArray;

        hr = pDevice->CreateBuffer( &bd, &initData, &m_pFacesIndexBuffer );
    }

    if ( (SUCCEEDED(hr)) && pDevice )
    {
		ID3D10Blob *pBlob = NULL;

		// Load vertex shader
		CompileShaderFromFile( _T( "Text.fx" ), "VS", "vs_4_0", &pBlob );
		if(pBlob)
		{
			pDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pTextVS );
			SAFE_DX_RELEASE( pBlob )
		}
        // Load pixel shader
		CompileShaderFromFile( _T( "Text.fx" ), "PS", "vs_4_0", &pBlob );		
		if(pBlob)
		{
			pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pTextPS );
			SAFE_DX_RELEASE( pBlob )
		}
        
    }


	IDXGISurface *pDxgiSurface = NULL;
    hr = m_pTextTexture->QueryInterface( &pDxgiSurface );

    if (SUCCEEDED(hr))
    {
        // Create a D2D render target which can draw into our offscreen D3D
        // surface. Given that we use a constant size for the texture, we
        // fix the DPI at 96.
        D2D1_RENDER_TARGET_PROPERTIES props =
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat( DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED ),
                96,
                96
                );

        SAFE_DX_RELEASE( m_pRenderTarget );
        hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
            pDxgiSurface,
            &props,
            &m_pRenderTarget
            );
		if(!SUCCEEDED(hr))
			return;
    }
    


	// Create a black brush.
	if (SUCCEEDED(hr))
	{
		hr = m_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&m_pTextBrush
			);
	}
}

}