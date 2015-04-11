//////////////////////////////////////////////////////////////////////////
//  作者:梅军
//  Copyright Reserved
//	2009年7月3日
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#define GLEW_STATIC
#include <GL/glew.h>

class CTextureManager
{
public:
	CTextureManager(void);
	~CTextureManager(void);

public:
	static BOOL CreateTextureFromFile( LPSTR lpFilename, GLuint& texture );

	static BOOL CreateTextureFromMemory( LPVOID pData, GLuint uiDataSize, GLuint& texture );
};
