//
// Copyright (C) Mei Jun 2011
//

#include "TextDisplay.h"
#include "Utility.h"
#include "TextureManager.h"

#include <vector>

TextDisplay::ConstantBuffer TextDisplay::s_Buffer;

TextDisplay::TextDisplay( )
{
	m_bInit			 = FALSE;
	m_uiTexture		 = 0;
	m_uiVertexBuffer = 0;
	m_uiVertexArray	 = 0;
	m_pFontManager	 = new FontManager();
	m_pShader		 = NULL;
}

TextDisplay::~TextDisplay(void)
{
	SAFE_DELETE( m_pFontManager );

	if( m_uiVertexBuffer )
		glDeleteBuffers( 1, &m_uiVertexBuffer );

	if( m_uiVertexArray )
		glDeleteVertexArrays( 1, &m_uiVertexArray );

	if( m_uiTexture )
		glDeleteTextures( 1, &m_uiTexture );
}

VOID TextDisplay::SetupInput( int iXPos, int iYPos, float Left, float Top,
	float Right, float Bottom )
{	
	//用于文本显示的顶点模板
	UINT numViewPorts = 1;
	GLint viewport[4] = { 0 };
	glGetIntegerv( GL_VIEWPORT, viewport );
	float fViewportWidth  = viewport[2];
	float fViewportHeight = viewport[3];

	static CUSTOMVERTEX Vertices[] = 
	{
		{ -fViewportWidth / 2.0f, fViewportHeight / 2.0f, 1.0f, 0.0f, 0.0f },
		{ -fViewportWidth / 2.0f, fViewportHeight / 2.0f, 1.0f, 1.0f, 0.0f },
		{ -fViewportWidth / 2.0f, fViewportHeight / 2.0f, 1.0f, 0.0f, 1.0f },
		{ -fViewportWidth / 2.0f, fViewportHeight / 2.0f, 1.0f, 1.0f, 1.0f },
	};
	
	//文本显示网格
	std::vector<CUSTOMVERTEX>	vecVertices;
	vecVertices.resize( 4 );
	if( m_uiTexture )
	{
		for( int i = 0; i < 4; ++i )
		{
			Vertices[i].x = -fViewportWidth / 2.0f;
			Vertices[i].y =	 fViewportHeight / 2.0f;

			vecVertices[i] = Vertices[i];
			if( i % 2 )
			{
				vecVertices[i].x = Vertices[i].x + iXPos + Right - Left;
				if( i / 2 )
				{
					vecVertices[i].y = Vertices[i].y - iYPos + Top - Bottom;
				}
				else
				{
					vecVertices[i].y = Vertices[i].y - iYPos;
				}
			}
			else
			{
				vecVertices[i].x = Vertices[i].x + iXPos;
				if( i / 2 )
				{
					vecVertices[i].y = Vertices[i].y - iYPos + Top - Bottom;
				}
				else
				{
					vecVertices[i].y = Vertices[i].y - iYPos;
				}
			}

			vecVertices[i].z = 1.0f;
		}
	}

	if( m_uiVertexBuffer )
	{
		glDeleteBuffers( 1, &m_uiVertexBuffer );
		m_uiVertexBuffer = 0;
	}

	// 创建顶点缓冲对象
	glGenBuffers( 1, &m_uiVertexBuffer );
	{
		// 绑定顶点缓冲对象
		glBindBuffer( GL_ARRAY_BUFFER, m_uiVertexBuffer );	
		// 向顶点缓冲对象填充顶点数据
		glBufferData( GL_ARRAY_BUFFER, sizeof(CUSTOMVERTEX) * 4,
			&vecVertices[0], GL_STATIC_DRAW );
		// 取消顶点缓冲对象绑定
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	// 创建顶点数组对象
	if( m_uiVertexArray )
		glDeleteVertexArrays( 1, &m_uiVertexArray );

	glGenVertexArrays( 1, &m_uiVertexArray );
	{
		// 绑定顶点数组对象
		glBindVertexArray( m_uiVertexArray );
		// 绑定顶点缓冲对象
		glBindBuffer( GL_ARRAY_BUFFER, m_uiVertexBuffer );
		// 指定shader程序
		ShaderHelper* pShaderInfo = m_pShader;
		// 获取自定义属性定位
		GLint uiPos		= glGetAttribLocation( pShaderInfo->uiProgram, "pos");
		GLint uiTex		= glGetAttribLocation( pShaderInfo->uiProgram, "tex" );
		// 启用顶点属性
		glEnableVertexAttribArray( uiPos );
		// 启用纹理坐标属性 
		glEnableVertexAttribArray( uiTex );
		// 数据偏移量
		int offset = 0;
		// 设定顶点X, Y, Z
		glVertexAttribPointer( uiPos, 3, GL_FLOAT, GL_FALSE, sizeof(CUSTOMVERTEX), (GLvoid*)offset );
		// 数据偏移量
		offset = 12;
		// 设定纹理坐标
		glVertexAttribPointer( uiTex, 2, GL_FLOAT, GL_FALSE, sizeof(CUSTOMVERTEX), (GLvoid*)offset );
		// 取消绑定顶点数组对象
		glBindVertexArray( 0 );
	}

	if( m_bInit )
		return;	
}

BOOL TextDisplay::SetupShaders()
{
	m_pShader = new ScreenQuadShader;
	m_pShader->Create();

	return TRUE;
}

void TextDisplay::Display()
{
	if( m_pShader == NULL )
		return;
	ShaderHelper* pShaderInfo = m_pShader;
	// 设定着色器参数
	{
		// 启用着色程序
		glUseProgram( pShaderInfo->uiProgram );
		// 指定绘制缓冲区
		glDrawBuffer( GL_COLOR_ATTACHMENT0 );
		// 设定投影矩阵
		XMMATRIX matOrtho = XMMatrixOrthographicLH(
			640, 480, 1.0f, 100.0f );
		GLint iWVP	  = glGetUniformLocation(pShaderInfo->uiProgram, "g_matWVP");
		glUniformMatrix4fv( iWVP, 1, GL_FALSE, (float*)&(matOrtho) );
	}
	
	// 绘制
	{
		// 获取并设置变量
		GLint uiScene = glGetUniformLocation( pShaderInfo->uiProgram, "SceneMap" );
		glUniform1i( uiScene, 0 );	

		// 激活纹理单元0
		glActiveTexture( GL_TEXTURE0 );
		// 绑定纹理
		glBindTexture( GL_TEXTURE_2D, m_uiTexture );
		// 绑定顶点数组对象
		glBindVertexArray( m_uiVertexArray );
		// 绘制图元
		glEnable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		glEnable( GL_DEPTH_TEST );
		// 取消绑定顶点数组对象
		glBindVertexArray( 0 );
	}
}

void TextDisplay::SetText( int iXPos, int iYPos, int iFontSize, 
	LPCWSTR lpText, unsigned int uiColor )
{
	int dwSize			= iFontSize;	
	size_t uiTextLength = wcslen( lpText );

	DWORD dwImageWidth  = uiTextLength * dwSize;
	float rows		    = dwImageWidth / 1024.0f;
	DWORD dwImageHeight = ceil( rows ) * dwSize * 2;

	if( rows > 1.0f )
		dwImageWidth = 1024;
	if( dwImageHeight / 512.0f > 1.0f )
		dwImageHeight = 512;

	int iBytesPerPixel	   = 4;
	size_t uiImageDataSize = dwImageHeight * dwImageWidth * iBytesPerPixel;
	BYTE* pImageData	   = new BYTE[uiImageDataSize];
	BYTE* pData			   = NULL;
	ZeroMemory( pImageData, uiImageDataSize );
	size_t XOffset	   = 0;
	size_t YOffset	   = dwSize;
	size_t dwMaxHeight = 0;
	
	BYTE *pBlue  = (BYTE*)&uiColor;
	BYTE* pGreen = pBlue + 1;
	BYTE* pRed	 = pBlue + 2;

	for( int i = 0; i < uiTextLength; ++i )
	{		
		FontManager::BMP_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );
		pData = m_pFontManager->CreateFontTexture(
			lpText[i], dwSize, &desc );
		size_t width = desc.dwWidth;
		size_t height = desc.dwHeight;

		int dwPitch = width * iBytesPerPixel;
		while( dwPitch % 4 != 0 )
			dwPitch++;
		if( XOffset + desc.iXAdvance > 1024 )
		{
			XOffset = 0;			
			YOffset += dwSize;
			dwMaxHeight = 0;
		}
		if( height > dwMaxHeight )
		{
			dwMaxHeight = height;
		}
		for( int h = 0; h < height; ++h )
		{
			for( int w = 0; w < width; ++w )
			{
				int X = (XOffset + desc.iXOffset + w) * iBytesPerPixel;
				int Y = (YOffset - desc.iYOffset + h) * dwImageWidth * iBytesPerPixel;
				if( Y + X >= uiImageDataSize - iBytesPerPixel )
					continue;
				{
					pImageData[Y + X]	  = *pBlue;
					pImageData[Y + X + 1] = *pGreen;
					pImageData[Y + X + 2] = *pRed;
				}
				if( iBytesPerPixel > 3 )
					pImageData[Y + X + 3] = pData[dwPitch * (height - h - 1) + w * iBytesPerPixel + 3];
			}
		}
		XOffset += desc.iXAdvance;
	}	

	{			
		size_t uiBitmapSize = RawToBitmap( pImageData, uiImageDataSize, NULL, 0,
			dwImageWidth, dwImageHeight, iBytesPerPixel );
		BYTE* pBitmapBuffer = new BYTE[uiBitmapSize];
		RawToBitmap( pImageData, uiImageDataSize, pBitmapBuffer, uiBitmapSize,
			dwImageWidth, dwImageHeight, iBytesPerPixel );

		if( m_uiTexture )
			glDeleteTextures( 1, &m_uiTexture );
		
		CTextureManager::CreateTextureFromMemory( pBitmapBuffer, uiBitmapSize, m_uiTexture );
		
		SAFE_DELETEARRAY( pBitmapBuffer );
	}
	SAFE_DELETEARRAY( pImageData );

	if( !m_bInit )
	{
		SetupShaders();				
	}

	SetupInput( iXPos, iYPos, 0, 0,
		dwImageWidth, dwImageHeight );	
	m_bInit = TRUE;	
}

size_t TextDisplay::RawToBitmap( void* pImageData, size_t uiImageDataSize, 
	void* pBitmap, size_t uiBitmapSize,
	size_t uiWidth, size_t uiHeight, int iBytesPerPixel )
{
	if( (pImageData == NULL) || (iBytesPerPixel < 3) )
		return 0;

	int dwRowPitch = uiWidth * iBytesPerPixel;
	while( dwRowPitch % 4 != 0 )
		dwRowPitch++;
	if( uiBitmapSize == 0 )
	{
		uiBitmapSize = sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER) + dwRowPitch * uiHeight;
		return uiBitmapSize;
	}

	if( pBitmap == NULL )
		return 0;

	BYTE* pBitmapBuffer = (BYTE*)pBitmap;

	BITMAPINFOHEADER bi;
	bi.biSize                    =  sizeof(BITMAPINFOHEADER);  
	bi.biWidth                   =  uiWidth;  
	bi.biHeight                  =  uiHeight;  
	bi.biPlanes                  =  1;  
	bi.biBitCount                =  iBytesPerPixel * 8;  
	bi.biCompression             =  BI_RGB;  
	bi.biSizeImage               =  0;  
	bi.biXPelsPerMeter           =  0;  
	bi.biYPelsPerMeter           =  0;  
	bi.biClrImportant            =  0;  
	bi.biClrUsed                 =  0;

	BITMAPFILEHEADER bmfHdr;
	bmfHdr.bfType		=  0x4D42;  //  "BM"    
	bmfHdr.bfSize		=  uiBitmapSize;    
	bmfHdr.bfReserved1  =  0;    
	bmfHdr.bfReserved2  =  0;    
	bmfHdr.bfOffBits	=  (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	memcpy( pBitmapBuffer, &bmfHdr, sizeof(BITMAPFILEHEADER));
	memcpy( pBitmapBuffer + sizeof(BITMAPFILEHEADER),
		&bi, sizeof(BITMAPINFOHEADER));
	
	BYTE* pData = pBitmapBuffer + sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER);
	BYTE* pSrcData = (BYTE*)pImageData;

	size_t uiRowWidth = uiWidth * iBytesPerPixel;
	for( int Y = 0; Y < uiHeight; ++Y )
	{
		for( int X = 0; X < uiRowWidth; ++X )
		{
			*pData++ = *pSrcData++;

		}
		pData = pBitmapBuffer + bmfHdr.bfOffBits + dwRowPitch * (Y + 1);
	}

	return uiBitmapSize;
}

void TextDisplay::SetFontFile( const char* pFontFile )
{
	if( m_pFontManager )
		m_pFontManager->SetFontFile( pFontFile );
}