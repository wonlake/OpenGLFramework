#pragma once

#include <windows.h>
#include <stdio.h>
#include <gl/glew.h>
struct ShapeInfo
{
	GLuint					uiElementSize;
	GLuint					uiNumVertices;
	GLuint					uiNumIndices;
	GLbyte*					pVertices;
	GLbyte*					pIndices;

	~ShapeInfo()
	{
		if( pVertices != NULL )
		{
			delete[] pVertices;
			pVertices = NULL;
		}

		if( pIndices != NULL )
		{
			delete[] pIndices;
			pIndices = NULL;
		}
	}
};

ShapeInfo* CreateBox( float xLength, float yLength, float zLength,
	float xOffset = 0, float yOffset = 0, float zOffset = 0 )
{
	struct CUSTOMVERTEX
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
	
	ShapeInfo* pShapeInfo = new ShapeInfo();
	pShapeInfo->uiElementSize = sizeof( CUSTOMVERTEX );
	pShapeInfo->uiNumIndices = 36;
	pShapeInfo->uiNumVertices = 24;

	CUSTOMVERTEX vertices[24] =
	{
		//前
		{ -xLength / 2.0f,  yLength / 2.0f, -zLength / 2.0f, 0.0f, 0.0f, -1.0f, 0.0, 0.0 },
		{  xLength / 2.0f,  yLength / 2.0f, -zLength / 2.0f, 0.0f, 0.0f, -1.0f, 1.0, 0.0 },
		{ -xLength / 2.0f, -yLength / 2.0f, -zLength / 2.0f, 0.0f, 0.0f, -1.0f, 0.0, 1.0 },
		{  xLength / 2.0f, -yLength / 2.0f, -zLength / 2.0f, 0.0f, 0.0f, -1.0f, 1.0, 1.0 },

		//后
		{  xLength / 2.0f,  yLength / 2.0f, zLength / 2.0f, 0.0f, 0.0f, 1.0f, 0.0, 0.0 },
		{ -xLength / 2.0f,  yLength / 2.0f, zLength / 2.0f, 0.0f, 0.0f, 1.0f, 1.0, 0.0 },
		{  xLength / 2.0f, -yLength / 2.0f, zLength / 2.0f, 0.0f, 0.0f, 1.0f, 0.0, 1.0 },
		{ -xLength / 2.0f, -yLength / 2.0f, zLength / 2.0f, 0.0f, 0.0f, 1.0f, 1.0, 1.0 },

		//左
		{ -xLength / 2.0f,  yLength / 2.0f,  zLength / 2.0f, -1.0f, 0.0f, 0.0f, 0.0, 0.0 },
		{ -xLength / 2.0f,  yLength / 2.0f, -zLength / 2.0f, -1.0f, 0.0f, 0.0f, 1.0, 0.0 },
		{ -xLength / 2.0f, -yLength / 2.0f,  zLength / 2.0f, -1.0f, 0.0f, 0.0f, 0.0, 1.0 },
		{ -xLength / 2.0f, -yLength / 2.0f, -zLength / 2.0f, -1.0f, 0.0f, 0.0f, 1.0, 1.0 },

		//右
		{ xLength / 2.0f,  yLength / 2.0f, -zLength / 2.0f, 1.0f, 0.0f, 0.0f, 0.0, 0.0 },
		{ xLength / 2.0f,  yLength / 2.0f,  zLength / 2.0f, 1.0f, 0.0f, 0.0f, 1.0, 0.0 },
		{ xLength / 2.0f, -yLength / 2.0f, -zLength / 2.0f, 1.0f, 0.0f, 0.0f, 0.0, 1.0 },
		{ xLength / 2.0f, -yLength / 2.0f,  zLength / 2.0f, 1.0f, 0.0f, 0.0f, 1.0, 1.0 },

		//上
		{ -xLength / 2.0f, yLength / 2.0f,  zLength / 2.0f, 0.0f, 1.0f, 0.0f, 0.0, 0.0 },
		{  xLength / 2.0f, yLength / 2.0f,  zLength / 2.0f, 0.0f, 1.0f, 0.0f, 1.0, 0.0 },
		{ -xLength / 2.0f, yLength / 2.0f, -zLength / 2.0f, 0.0f, 1.0f, 0.0f, 0.0, 1.0 },
		{  xLength / 2.0f, yLength / 2.0f, -zLength / 2.0f, 0.0f, 1.0f, 0.0f, 1.0, 1.0 },

		//下
		{ -xLength / 2.0f, -yLength / 2.0f, -zLength / 2.0f, 0.0f, -1.0f, 0.0f, 0.0, 0.0 },
		{  xLength / 2.0f, -yLength / 2.0f, -zLength / 2.0f, 0.0f, -1.0f, 0.0f, 1.0, 0.0 },
		{ -xLength / 2.0f, -yLength / 2.0f,  zLength / 2.0f, 0.0f, -1.0f, 0.0f, 0.0, 1.0 },
		{  xLength / 2.0f, -yLength / 2.0f,  zLength / 2.0f, 0.0f, -1.0f, 0.0f, 1.0, 1.0 },
	};

	for( int i = 0; i < 24; ++i )
	{
		vertices[i].x += xOffset;
		vertices[i].y += yOffset;
		vertices[i].z += zOffset;
		vertices[i].v = 1.0f - vertices[i].v;
	}

	pShapeInfo->pVertices = new GLbyte[pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices];
	memcpy( pShapeInfo->pVertices, vertices, pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices );

	WORD indices[36] =
	{
		0, 1, 2,
		2, 1, 3,

		4, 5, 6,
		6, 5, 7,

		8, 9, 10,
		10, 9, 11,

		12, 13, 14,
		14, 13, 15,

		16, 17, 18,
		18, 17, 19,

		20, 21, 22,
		22, 21, 23,
	};

	pShapeInfo->pIndices = new GLbyte[sizeof(WORD) * pShapeInfo->uiNumIndices];
	memcpy( pShapeInfo->pIndices, indices, sizeof(WORD) * pShapeInfo->uiNumIndices );

	return pShapeInfo;
}

ShapeInfo* CreateXPlane( float yLength, float zLength,
	float xOffset = 0, float yOffset = 0, float zOffset = 0 )
{
	struct CUSTOMVERTEX
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
	
	ShapeInfo* pShapeInfo = new ShapeInfo();
	pShapeInfo->uiElementSize = sizeof( CUSTOMVERTEX );
	pShapeInfo->uiNumIndices = 6;
	pShapeInfo->uiNumVertices = 4;

	CUSTOMVERTEX vertices[4] =
	{
		{ 0.0f,  yLength / 2.0f,  zLength / 2.0f, 1.0f, 0.0f, 0.0f, 0.0, 0.0 },
		{ 0.0f,  yLength / 2.0f, -zLength / 2.0f, 1.0f, 0.0f, 0.0f, 1.0, 0.0 },
		{ 0.0f, -yLength / 2.0f,  zLength / 2.0f, 1.0f, 0.0f, 0.0f, 0.0, 1.0 },
		{ 0.0f, -yLength / 2.0f, -zLength / 2.0f, 1.0f, 0.0f, 0.0f, 1.0, 1.0 },
	};

	for( int i = 0; i < 4; ++i )
	{
		vertices[i].x += xOffset;
		vertices[i].y += yOffset;
		vertices[i].z += zOffset;
		vertices[i].v = 1.0f - vertices[i].v;
	}

	pShapeInfo->pVertices = new GLbyte[pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices];
	memcpy( pShapeInfo->pVertices, vertices, pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices );

	WORD indices[6] =
	{
		0, 1, 2,
		2, 1, 3,
	};

	pShapeInfo->pIndices = new GLbyte[sizeof(WORD) * pShapeInfo->uiNumIndices];
	memcpy( pShapeInfo->pIndices, indices, sizeof(WORD) * pShapeInfo->uiNumIndices );

	return pShapeInfo;
}

ShapeInfo* CreateYPlane( float xLength, float zLength,
	float xOffset = 0, float yOffset = 0, float zOffset = 0 )
{
	struct CUSTOMVERTEX
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

	ShapeInfo* pShapeInfo = new ShapeInfo();
	pShapeInfo->uiElementSize = sizeof( CUSTOMVERTEX );
	pShapeInfo->uiNumIndices = 6;
	pShapeInfo->uiNumVertices = 4;

	CUSTOMVERTEX vertices[4] =
	{
		{ -xLength / 2.0f, 0.0f,  zLength / 2.0f, 0.0f, 1.0f, 0.0f, 0.0, 0.0 },
		{  xLength / 2.0f, 0.0f,  zLength / 2.0f, 0.0f, 1.0f, 0.0f, 1.0, 0.0 },
		{ -xLength / 2.0f, 0.0f, -zLength / 2.0f, 0.0f, 1.0f, 0.0f, 0.0, 1.0 },
		{  xLength / 2.0f, 0.0f, -zLength / 2.0f, 0.0f, 1.0f, 0.0f, 1.0, 1.0 },
	};

	for( int i = 0; i < 4; ++i )
	{
		vertices[i].x += xOffset;
		vertices[i].y += yOffset;
		vertices[i].z += zOffset;
		vertices[i].v = 1.0f - vertices[i].v;
	}

	pShapeInfo->pVertices = new GLbyte[pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices];
	memcpy( pShapeInfo->pVertices, vertices, pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices );

	WORD indices[6] =
	{
		0, 1, 2,
		2, 1, 3,
	};

	pShapeInfo->pIndices = new GLbyte[sizeof(WORD) * pShapeInfo->uiNumIndices];
	memcpy( pShapeInfo->pIndices, indices, sizeof(WORD) * pShapeInfo->uiNumIndices );

	return pShapeInfo;
}

ShapeInfo* CreateZPlane( float xLength, float yLength,
	float xOffset = 0, float yOffset = 0, float zOffset = 0 )
{
	struct CUSTOMVERTEX
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
	
	ShapeInfo* pShapeInfo = new ShapeInfo();
	pShapeInfo->uiElementSize = sizeof( CUSTOMVERTEX );
	pShapeInfo->uiNumIndices = 6;
	pShapeInfo->uiNumVertices = 4;

	CUSTOMVERTEX vertices[4] =
	{
		{ -xLength / 2.0f,  yLength / 2.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0, 0.0 },
		{  xLength / 2.0f,  yLength / 2.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0, 0.0 },
		{ -xLength / 2.0f, -yLength / 2.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0, 1.0 },
		{  xLength / 2.0f, -yLength / 2.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0, 1.0 },
	};

	for( int i = 0; i < 4; ++i )
	{
		vertices[i].x += xOffset;
		vertices[i].y += yOffset;
		vertices[i].z += zOffset;
		vertices[i].v = 1.0f - vertices[i].v;
	}

	pShapeInfo->pVertices = new GLbyte[pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices];
	memcpy( pShapeInfo->pVertices, vertices, pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices );

	WORD indices[6] =
	{
		0, 1, 2,
		2, 1, 3,
	};

	pShapeInfo->pIndices = new GLbyte[sizeof(WORD) * pShapeInfo->uiNumIndices];
	memcpy( pShapeInfo->pIndices, indices, sizeof(WORD) * pShapeInfo->uiNumIndices );

	return pShapeInfo;
}