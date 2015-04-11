//
// Copyright (C) Mei Jun 2011
//

#include "BillboardText.h"
#include "utility.h"

#include <tchar.h>

#pragma comment( lib, "winmm.lib" )

CBillboardText::CBillboardText()
{
	m_dwSize	= 20;
	m_TextColor = RGB(255, 0, 0);
	m_BackColor = RGB(255, 255, 255);
	
	memset( m_vPos, 0, sizeof(m_vPos) );
	m_dwFPS = 0;
	m_pTextDisplay = NULL;

	m_mapFontNames["ËÎÌå"]		= "C:\\Windows\\Fonts\\simsun.ttc";
	m_mapFontNames["¿¬Ìå"]		= "C:\\Windows\\Fonts\\simkai.ttf";
	m_mapFontNames["·ÂËÎ"]		= "C:\\Windows\\Fonts\\simfang.ttf";	
	m_mapFontNames["ºÚÌå"]		= "C:\\Windows\\Fonts\\simhei.ttf";
	m_mapFontNames["ÐÂ·ÂËÎ"]		= "C:\\Windows\\Fonts\\simsun.ttc";
	m_mapFontNames["Î¢ÈíÑÅºÚ"]	= "C:\\Windows\\Fonts\\msyh.ttf";
}

CBillboardText::~CBillboardText(void)
{	
	SAFE_DELETE( m_pTextDisplay );
}

VOID CBillboardText::UpdateText( LPCTSTR lpText )
{
	if( m_pTextDisplay )
	{
		m_pTextDisplay->SetText( m_vPos[0], m_vPos[1],
			m_dwSize, lpText, m_TextColor );
	}	
}

VOID CBillboardText::SetFontProperty( LPCSTR lpFontName, DWORD dwSize, 
	COLORREF TextColor, COLORREF BackColor )
{
	std::map<std::string, std::string>::iterator iter = 
		m_mapFontNames.find( lpFontName );
	if( iter != m_mapFontNames.end() )
	{
		if( m_pTextDisplay == NULL )
		{
				m_pTextDisplay = new TextDisplay();
		}
		m_pTextDisplay->SetFontFile( iter->second.c_str() );
	}

	m_dwSize	= dwSize;
	m_TextColor = TextColor;
	m_BackColor = BackColor;

}

void CBillboardText::Render()
{	
	static DWORD last = 0;
	static DWORD cur = timeGetTime();
	static DWORD frame = 0;
	static DWORD uFrame = 0;

	if( cur - last > 1000 )
	{
		last = cur;
		m_dwFPS = frame;
		frame = 0;
	}
	cur = timeGetTime();
	++frame;
			
	if( m_pTextDisplay != NULL )
		m_pTextDisplay->Display();
}

VOID CBillboardText::SetDisplayPos( XMVECTOR vPos )
{
	memcpy( m_vPos, &vPos, sizeof(vPos) );
}

DWORD CBillboardText::GetFPS()
{
	return m_dwFPS;
}