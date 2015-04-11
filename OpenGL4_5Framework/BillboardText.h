//
// Copyright (C) Mei Jun 2011
//

#pragma once

#include "TextDisplay.h"

class CBillboardText
{
public:
	CBillboardText();
	~CBillboardText(void);
	
	void Render();	
	//
	//更新文本
	//
	VOID UpdateText( LPCTSTR lpText );
	//
	//设置字体属性
	//
	VOID SetFontProperty( LPCSTR lpFontName, DWORD dwSize,
		COLORREF TextColor, COLORREF BackColor );
	//
	//设置显示位置
	//
	VOID SetDisplayPos( XMVECTOR vPos );
	//
	//获取FPS
	//
	DWORD GetFPS();

protected:
	//
	//字体大小
	//
	DWORD						m_dwSize;
	//
	//文本颜色
	//
	COLORREF					m_TextColor;
	//
	//背景颜色
	//
	COLORREF					m_BackColor;
		
	DWORD						m_dwFPS;

	FLOAT						m_vPos[4];

	TextDisplay*				m_pTextDisplay;

	std::map<std::string, std::string> m_mapFontNames;
};