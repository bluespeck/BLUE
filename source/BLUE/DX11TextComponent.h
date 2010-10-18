#pragma once


#include <d2d1.h>
#include <d2d1helper.h>
#include <DWrite.h>
#include <d3d11.h>
#include "DXUtils.h"

namespace BLUE
{

class CDX11TextComponent
{
public:
	enum TextAlignment {
		TEXT_ALIGN_LEFT,
		TEXT_ALIGN_RIGHT,
		TEXT_ALIGN_CENTER
	};

	enum FontWeight
	{
		FONT_WEIGHT_THIN = 100,
		FONT_WEIGHT_EXTRA_LIGHT = 200,
		FONT_WEIGHT_ULTRA_LIGHT = 200,
		FONT_WEIGHT_LIGHT = 300,
		FONT_WEIGHT_NORMAL = 400,
		FONT_WEIGHT_REGULAR = 400,
		FONT_WEIGHT_MEDIUM = 500,
		FONT_WEIGHT_DEMI_BOLD = 600,
		FONT_WEIGHT_SEMI_BOLD = 600,
		FONT_WEIGHT_BOLD = 700,
		FONT_WEIGHT_EXTRA_BOLD = 800,
		FONT_WEIGHT_ULTRA_BOLD = 800,
		FONT_WEIGHT_BLACK = 900,
		FONT_WEIGHT_HEAVY = 900,
		FONT_WEIGHT_EXTRA_BLACK = 950,
		FONT_WEIGHT_ULTRA_BLACK = 950
	};

	enum FontStyle
	{
		FONT_STYLE_NORMAL,
		FONT_STYLE_OBLIQUE,
		FONT_STYLE_ITALIC
	};

	enum FontStretch
	{
		FONT_STRETCH_UNDEFINED = 0,
		FONT_STRETCH_ULTRA_CONDENSED = 1,
		FONT_STRETCH_EXTRA_CONDENSED = 2,
		FONT_STRETCH_CONDENSED = 3,
		FONT_STRETCH_SEMI_CONDENSED = 4,
		FONT_STRETCH_NORMAL = 5,
		FONT_STRETCH_MEDIUM = 5,
		FONT_STRETCH_SEMI_EXPANDED = 6,
		FONT_STRETCH_EXPANDED = 7,
		FONT_STRETCH_EXTRA_EXPANDED = 8,
		FONT_STRETCH_ULTRA_EXPANDED = 9
	};

	CDX11TextComponent();
	virtual ~CDX11TextComponent();
	void Initialize( ID3D11Device *pDevice );

	void SetFont( TCHAR *fontFamily, FontWeight eWeight, FontStyle eStyle, FontStretch eStretch, float fSize);
	void GetFontFamily( TCHAR *fontFamilyName, unsigned int nSize );
	FontWeight GetFontWeight();
	FontStyle GetFontStyle();
	FontStretch GetFontStretch();
	float GetFontSize();	
	void SetTextAlignment( TextAlignment eTextAlignment);
	TextAlignment GetTextAlignment( );

	void DrawTextToSurface( const TCHAR *text, float left, float top, float right, float bottom, bool bClearRect = false );
	void DrawTextSurface( ID3D11DeviceContext *pDeviceContext );

	void CreateDeviceResources( ID3D11Device *pDevice );

protected:
	// how much to scale a design that assumes 96-DPI pixels
	float m_fDpiScaleX;
	float m_fDpiScaleY;

	// Direct2D

	ID2D1Factory* m_pD2DFactory;
	ID2D1RenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pTextBrush;

	// Direct3D
	ID3D11Texture2D *m_pTextTexture;
	ID3D11ShaderResourceView *m_pTextureResourceView;
	ID3D11VertexShader	*m_pTextVS;
	ID3D11PixelShader	*m_pTextPS;
	ID3D11Buffer *m_pVertexBuffer;
    ID3D11InputLayout *m_pVertexLayout;
    ID3D11Buffer *m_pFacesIndexBuffer;

	static const D3D11_INPUT_ELEMENT_DESC s_InputLayout[];
    static const SimpleVertex s_VertexArray[];
	static const SHORT s_FacesIndexArray[];

	// DirectWrite

	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
};

}