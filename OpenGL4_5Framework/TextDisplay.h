//
// Copyright (C) Mei Jun 2011
//

#pragma once

#include "FontManager.h"
#include "Camera.h"
#include "ScreenQuadShader.h"

class TextDisplay
{
public:
	TextDisplay();
	~TextDisplay(void);
public:
	FontManager*				m_pFontManager;

	BOOL						m_bInit;

public:
	GLuint						m_uiVertexBuffer;

	GLuint						m_uiVertexArray;

	GLuint						m_uiTexture;
	
	ScreenQuadShader*			m_pShader;

public:
	void Display();

	void SetText( int iXPos, int iYPos, int iFontSize, 
		LPCWSTR lpText, unsigned int uiColor );

	VOID SetupInput( int iXPos, int iYPos, float Left, float Top,
		float Right, float Bottom );

	BOOL SetupShaders();
	
	static size_t RawToBitmap( void* pImageData, size_t uiImageDataSize, 
		void* pBitmap, size_t uiBitmapSize,
		size_t uiWidth, size_t uiHeight, int iBytesPerPixel );

	void SetFontFile( const char* pFontFile );

public:
	// �Զ��嶥���ʽ	
	struct CUSTOMVERTEX
	{
		FLOAT x, y, z;	   // ��������
		FLOAT u, v;        // ��������
	};

	struct ConstantBuffer
	{
		float matWorldViewProj[16];
		float matWorldView[16];
	};

	static ConstantBuffer s_Buffer;
};

