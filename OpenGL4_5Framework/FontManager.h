//
// Copyright (C) Mei Jun 2011
//

#pragma once

#include <map>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H

class FontManager
{
public:
	FontManager();
	~FontManager(void);

public:
	struct BMP_DESC
	{		
		unsigned char* pBuffer;
		int   dwWidth;
		int   dwHeight;
		int	iXOffset;
		int iYOffset;
		int iXAdvance;
		int iYAdvance;
	};
	std::map<wchar_t, BMP_DESC> m_mapFonts;
	std::string		m_strFontFileName;

public:
	void SetFontFile( const char* pFontFile );
	unsigned char* CreateFontTexture( wchar_t iUnicode, size_t uiFontSize,
		BMP_DESC* pDesc );
};

