//////////////////////////////////////////////////////////////////////////
//  作者:梅军
//  Copyright Reserved
//	2009年7月3日
//////////////////////////////////////////////////////////////////////////

#include "TextureManager.h"
#include <FreeImage.h>

#ifdef _DEBUG
#pragma comment( lib, "libfreeimage_d.lib" )
#else
#pragma comment( lib, "libfreeimage.lib" )
#endif

#pragma comment( lib, "libglew.lib" )
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )

CTextureManager::CTextureManager(void)
{
}

CTextureManager::~CTextureManager(void)
{
}

BOOL CTextureManager::CreateTextureFromFile( LPSTR lpFilename, GLuint& texture )
{
	FreeImage_Initialise();
	// TODO: 在此添加实现代码
	FIBITMAP          *dib         = NULL;
	FIMULTIBITMAP     *mul         = NULL;
	DWORD             index        = 0;
	BOOL              bMultiBitmap = FALSE;
	FREE_IMAGE_FORMAT fif          = FreeImage_GetFileType( lpFilename,0);

	if(fif == FIF_JPEG)
		dib = FreeImage_Load(fif, lpFilename,JPEG_CMYK);
	else if(fif == FIF_ICO)
	{
		bMultiBitmap = TRUE;
		mul = FreeImage_OpenMultiBitmap(fif, lpFilename, FALSE,TRUE,TRUE,ICO_DEFAULT);

		dib = FreeImage_LockPage(mul,index);
		for( int i = 0;i < FreeImage_GetPageCount( mul ); i++ )
		{
			FIBITMAP *tmp = FreeImage_LockPage( mul, i );
			if( ( FreeImage_GetWidth(tmp) > FreeImage_GetWidth(dib) ) ||
				( FreeImage_GetBPP(tmp)   > FreeImage_GetBPP(dib) ) )
			{
				FreeImage_UnlockPage(mul,dib,FALSE);
				dib   = tmp;
				index = i;
				continue;
			}
			FreeImage_UnlockPage( mul, tmp, FALSE );
		}
		FreeImage_CloseMultiBitmap( mul, 0 );

		mul = FreeImage_OpenMultiBitmap( fif, lpFilename, FALSE, TRUE, TRUE, ICO_MAKEALPHA );
		dib = FreeImage_LockPage( mul,index );
	}
	else if(fif == FIF_GIF)
	{
		mul = FreeImage_OpenMultiBitmap( fif, lpFilename, FALSE,TRUE,TRUE, GIF_PLAYBACK );
		dib = FreeImage_LockPage( mul, 0 );
	}
	else
		dib = FreeImage_Load( fif, lpFilename, 0 );

	int bpp       = FreeImage_GetBPP( dib );
	int width     = FreeImage_GetWidth( dib );
	int height    = FreeImage_GetHeight( dib );
	int imageSize = width * height * bpp / 8;

	if( bpp < 24 )
	{
		FIBITMAP* p = FreeImage_ConvertTo24Bits( dib );
		FreeImage_Unload( dib );
		dib = p;
	}
	BYTE *pData = FreeImage_GetBits( dib );
	
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	if( bpp > 24 )
		gluBuild2DMipmaps( GL_TEXTURE_2D, bpp / 8, width, height,
					   GL_BGRA, GL_UNSIGNED_BYTE, pData );
	else
		gluBuild2DMipmaps( GL_TEXTURE_2D, bpp / 8, width, height,
					   GL_BGR, GL_UNSIGNED_BYTE, pData );

	if( !bMultiBitmap )
		FreeImage_Unload( dib );
	else
	{
		FreeImage_UnlockPage( mul, dib, FALSE );
		FreeImage_CloseMultiBitmap( mul, 0 );
	}
	return TRUE;
}

BOOL CTextureManager::CreateTextureFromMemory( LPVOID pData, GLuint uiDataSize, GLuint& texture )
{
	FIBITMAP         *pFiBitmap			= NULL;
	FIMULTIBITMAP    *pFiMultiBitmap	= NULL;
	DWORD             dwIndex			= 0;
	BOOL              bMultiBitmap		= FALSE;

	FreeImage_Initialise();
	FIMEMORY* fi_mem			= FreeImage_OpenMemory( (BYTE*)pData, uiDataSize );
	FREE_IMAGE_FORMAT FiFormat  = FreeImage_GetFileTypeFromMemory( fi_mem, uiDataSize );

	if( FiFormat == FIF_UNKNOWN )
	{
		FreeImage_CloseMemory( fi_mem );
		return FALSE;
	}

	if(FiFormat == FIF_JPEG)
		pFiBitmap = FreeImage_LoadFromMemory( FiFormat, fi_mem, JPEG_CMYK );
	else if( FiFormat == FIF_GIF )
		pFiBitmap = FreeImage_LoadFromMemory( FiFormat, fi_mem, GIF_PLAYBACK );
	else if( FiFormat == FIF_ICO )
		pFiBitmap = FreeImage_LoadFromMemory( FiFormat, fi_mem, ICO_MAKEALPHA );
	else
		pFiBitmap = FreeImage_LoadFromMemory( FiFormat, fi_mem, 0 );

	int bpp        = FreeImage_GetBPP( pFiBitmap );
	if( bpp < 32 )
	{
		FIBITMAP* p = FreeImage_ConvertTo32Bits( pFiBitmap );
		FreeImage_Unload( pFiBitmap );
		pFiBitmap = p;
		bpp = 32;
	}
	int iWidth     = FreeImage_GetWidth( pFiBitmap );
	int iHeight    = FreeImage_GetHeight( pFiBitmap );

	BYTE* pImageData = FreeImage_GetBits( pFiBitmap );

	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
	if( bpp > 24 )
		gluBuild2DMipmaps( GL_TEXTURE_2D, bpp / 8, iWidth, iHeight,
			GL_BGRA, GL_UNSIGNED_BYTE, pImageData );
	else
		gluBuild2DMipmaps( GL_TEXTURE_2D, bpp / 8, iWidth, iHeight,
			GL_BGR, GL_UNSIGNED_BYTE, pImageData );

	if( bMultiBitmap )
	{
		FreeImage_UnlockPage( pFiMultiBitmap, pFiBitmap, FALSE );
		FreeImage_CloseMultiBitmap( pFiMultiBitmap, 0 );
	}
	else
	{
		FreeImage_Unload( pFiBitmap );	
	}
	FreeImage_CloseMemory( fi_mem );	
	FreeImage_DeInitialise();

	return TRUE;
}