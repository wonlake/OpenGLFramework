//////////////////////////////////////////////////////////////////////////
//  ����:÷��
//  Copyright Reserved
//	2009��7��3��
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
