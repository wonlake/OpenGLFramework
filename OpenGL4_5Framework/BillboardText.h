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
	//�����ı�
	//
	VOID UpdateText( LPCTSTR lpText );
	//
	//������������
	//
	VOID SetFontProperty( LPCSTR lpFontName, DWORD dwSize,
		COLORREF TextColor, COLORREF BackColor );
	//
	//������ʾλ��
	//
	VOID SetDisplayPos( XMVECTOR vPos );
	//
	//��ȡFPS
	//
	DWORD GetFPS();

protected:
	//
	//�����С
	//
	DWORD						m_dwSize;
	//
	//�ı���ɫ
	//
	COLORREF					m_TextColor;
	//
	//������ɫ
	//
	COLORREF					m_BackColor;
		
	DWORD						m_dwFPS;

	FLOAT						m_vPos[4];

	TextDisplay*				m_pTextDisplay;

	std::map<std::string, std::string> m_mapFontNames;
};