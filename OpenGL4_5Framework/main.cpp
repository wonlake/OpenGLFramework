//////////////////////////////////////////////////////////////////////////
//  ����:÷��
//  Copyright Reserved
//	2009��7��3��
//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "TextureManager.h"
#include "Camera.h"
#include "resource.h"
#include "ShapeUtility.h"
#include "ShaderUtility.h"
#include "utility.h"
#include "ScreenQuadShader.h"
#include "MRTShader.h"
#include "BillboardText.h"

#define MAX_LOADSTRING 100

#define FAR_DIST 100.0f
#define NEAR_DIST 0.1f

// ȫ�ֱ���:
TCHAR szTitle[]		  = TEXT("OpenGL3_3Framework");
TCHAR szWindowClass[] = TEXT("OpenGL");

HDC		g_hDC  = NULL;
HGLRC	g_hRC  = NULL; 
HWND	g_hWnd = NULL;

int		g_dwWindowWidth  = 640;
int		g_dwWindowHeight = 480;

GLuint  g_Texture		 = 0;
GLuint  g_TreeTexture	 = 0;

GLuint g_FBO			 = 0;
GLuint g_MRTRenderBuffer = 0;

#define RT_SCENE	0
#define RT_DEPTH	1
#define RT_NORMAL	2

GLuint g_TexRT[3]		= { 0 };
GLuint g_vboQuad[3]		= { 0 };
GLuint g_vaoQuad[3]		= { 0 };
GLuint g_iboQuad[3]		= { 0 };
ShapeInfo* g_pQuad[3]	= { NULL };

GLuint	g_vboBox	    = 0;
GLuint	g_vaoBox		= 0;
GLuint	g_iboBox		= 0;
ShapeInfo* g_pShapeBox  = NULL;

MRTShader*		  g_pMRTShader    = NULL;
ScreenQuadShader* g_pQuadShader   = NULL;
CBillboardText*	 g_pBillboardText = NULL;
CTrackBallCamera g_Camera;

#define ID_FPS	1000

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

BOOL SetupPixelFormat( HDC hDC );
BOOL InitGL( HWND hWnd );
VOID Render();
VOID GameLoop();
DWORD PlayMusic( LPVOID lParam );
VOID  LoadResources( HWND hWnd );
VOID ReleaseResources();

int WINAPI WinMain( HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR    lpCmdLine,
	int       nCmdShow )
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: �ڴ˷��ô��롣
	MSG msg;
	// ��ʼ��ȫ���ַ���
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	// ����Ϣѭ��:
	BOOL fMessage;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while( msg.message != WM_QUIT )
	{  
		fMessage = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);
		if(fMessage)	
		{ 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else  
		{
			Render();
		}
	}
	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON1) );
	wcex.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon( wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1) );

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	RECT rc;
	SetRect( &rc, 0, 0, g_dwWindowWidth, g_dwWindowHeight );        
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, false );

	HWND hWnd = CreateWindow( szWindowClass, 
		szTitle, 
		WS_OVERLAPPEDWINDOW,
		100, 100, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL );

	if( !hWnd )
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:						// ��������
		{
			SetTimer( hWnd, ID_FPS, 1000, NULL );
			g_hDC = GetDC( hWnd );		// ��ȡ��ǰ���ڵ��豸���
			SetupPixelFormat( g_hDC );	// ������ʾģʽ��װ����
			InitGL( hWnd );
			glewInit();
			LoadResources( hWnd );

			return 0;
		}

	case WM_TIMER:
		{
			if( g_pBillboardText )
			{
				DWORD dwFPS = g_pBillboardText->GetFPS();
				static TCHAR strFPS[20] = { 0 };
				_stprintf( strFPS, _T("��ǰ֡��:%d"), dwFPS );
				g_pBillboardText->UpdateText( strFPS );
			}
			break;
		}

	case WM_SIZE:									// ���ڳߴ�仯
		{
			g_dwWindowHeight = HIWORD(lParam);			// ���ڵĸ�
			g_dwWindowWidth  = LOWORD(lParam);			// ���ڵĿ�
			if ( g_dwWindowHeight == 0)	
				g_dwWindowHeight = 1;					// ��ֹ��0��

			glViewport( 0, 0, g_dwWindowWidth, g_dwWindowHeight );
			g_Camera.Init( g_dwWindowWidth, g_dwWindowHeight );

			return 0;
		}

	case WM_DESTROY:
		{
			KillTimer( hWnd, ID_FPS );
			ReleaseResources();
			PostQuitMessage(0);
			break;
		}

	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_ESCAPE:
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
				break;
			}
			break;
		}

	default:
		{
			if( !g_Camera.HandleMessage( hWnd, message, wParam, lParam ) )
				return DefWindowProc( hWnd, message, wParam, lParam );
		}
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// ��ʼ��OpenGL����
BOOL InitGL(HWND hwnd)
{		
	// ������ɫģʽ
	glShadeModel( GL_SMOOTH );
	// ����������ֵ
	glClearDepth( 1.0f );
	// ������Ȳ��Ժ���
	glDepthFunc( GL_LEQUAL );
	//��������Ȳ���
	glEnable( GL_DEPTH_TEST );
	// ��������ӳ��
	glEnable( GL_TEXTURE_2D );

	return TRUE;
}

// ��Ⱦ
VOID Render()										 
{
	// �趨��ȾĿ��
	glBindFramebuffer( GL_FRAMEBUFFER, g_FBO );
	ShaderHelper* pShaderInfo = g_pMRTShader;
	// �趨��ɫ������
	{
		// ������ɫ����
		glUseProgram( pShaderInfo->uiProgram );
		// ָ�����ƻ�����
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, 
			GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers( 3, attachments );

		// �趨����ϵ������任
		XMMATRIX matView;
		g_Camera.GetViewMatrix( &matView );
		XMMATRIX matProj = XMMatrixPerspectiveFovLH( XM_PI / 4.0f, 
			(float)g_dwWindowWidth / (float)g_dwWindowHeight, NEAR_DIST, FAR_DIST );	

		XMMATRIX matTranslate = XMMatrixTranslation( 0.0f, 0.0f, 40.0f );
		XMMATRIX matWV		  = matView * matTranslate;
		XMMATRIX matWVP		  = matWV * matProj;
		XMVECTOR determinant  = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
		XMMATRIX matWVInv	  = XMMatrixInverse( &determinant, matWV );
		XMMATRIX matNormal	  = XMMatrixTranspose( matWVInv );

		GLint iWV	  = glGetUniformLocation(pShaderInfo->uiProgram, "g_matWV");
		GLint iWVP	  = glGetUniformLocation(pShaderInfo->uiProgram, "g_matWVP");
		GLint iNormal = glGetUniformLocation(pShaderInfo->uiProgram, "g_matNormal");

		glUniformMatrix4fv( iWV, 1, GL_FALSE, (float*)&(matWV) );
		glUniformMatrix4fv( iWVP, 1, GL_FALSE, (float*)&(matWVP) );	
		glUniformMatrix4fv( iNormal, 1, GL_FALSE, (float*)&(matNormal) );
	}

	//���������
	glClearColor( 0.5f, 0.5f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	ShapeInfo* pShapeInfo = g_pShapeBox;
	// ����ͼ��
	{
		// �趨�������
		glUniform1i( glGetUniformLocation( pShaderInfo->uiProgram, "testMap"), 0 );
		glUniform1i( glGetUniformLocation( pShaderInfo->uiProgram, "treeMap"), 1 );
		// ������ɫ������
		glUniform1f( glGetUniformLocation( pShaderInfo->uiProgram, "g_FarDist" ), FAR_DIST );
		// ��������Ԫ0
		glActiveTexture( GL_TEXTURE0 );
		// ������
		glBindTexture( GL_TEXTURE_2D, g_Texture );
		// ��������Ԫ1
		glActiveTexture( GL_TEXTURE1 );
		// ������
		glBindTexture( GL_TEXTURE_2D, g_TreeTexture );
		// �󶨶����������
		glBindVertexArray( g_vaoBox );
		// �󶨶��������������
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_iboBox );
		// ����ͼԪ
		glDrawElements( GL_TRIANGLES, pShapeInfo->uiNumIndices,  
			GL_UNSIGNED_SHORT, (GLvoid*)0 );
		// ȡ���󶨶����������
		glBindVertexArray( 0 );		
	}
	// ��ԭ��ȾĿ��
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	pShaderInfo = g_pQuadShader;
	// �趨��ɫ������
	{
		// ������ɫ����
		glUseProgram( pShaderInfo->uiProgram );
		// ָ�����ƻ�����
		glDrawBuffer( GL_COLOR_ATTACHMENT0 );
		// �趨ͶӰ����
		XMMATRIX matOrtho = XMMatrixOrthographicLH(
			g_dwWindowWidth, g_dwWindowHeight, NEAR_DIST, FAR_DIST );
		GLint iWVP	  = glGetUniformLocation(pShaderInfo->uiProgram, "g_matWVP");
		glUniformMatrix4fv( iWVP, 1, GL_FALSE, (float*)&(matOrtho) );
	}

	//�������
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// ����
	{
		// ��ȡ�����ñ���
		GLint uiScene = glGetUniformLocation( pShaderInfo->uiProgram, "SceneMap" );
		glUniform1i( uiScene, 0 );	
		for( int i = 0; i < 3; ++i )
		{
			pShapeInfo = g_pQuad[i];
			// ��������Ԫ0
			glActiveTexture( GL_TEXTURE0 );
			// ������
			glBindTexture( GL_TEXTURE_2D, g_TexRT[i] );
			// �󶨶����������
			glBindVertexArray( g_vboQuad[i] );
			// �󶨶��������������
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_iboQuad[i] );
			// ����ͼԪ
			glDrawElements( GL_TRIANGLES, pShapeInfo->uiNumIndices,  
				GL_UNSIGNED_SHORT, (GLvoid*)0 );
			// ȡ���󶨶����������
			glBindVertexArray( 0 );
		}
	}
	g_pBillboardText->Render();
	// �����󱸻�����
	SwapBuffers( g_hDC );
}

// ����OpenGL���ظ�ʽ
BOOL SetupPixelFormat( HDC hDC )
{
	// ���ص��ʽ
	int nPixelFormat = -1;
	PIXELFORMATDESCRIPTOR pfd =
	{ 
		sizeof(PIXELFORMATDESCRIPTOR),	  // pfd�ṹ�Ĵ�С 
		1,							 	  // �汾�� 
		PFD_DRAW_TO_WINDOW |              // ֧���ڴ����л�ͼ 
		PFD_SUPPORT_OPENGL |              // ֧�� OpenGL 
		PFD_DOUBLEBUFFER,                 // ˫����ģʽ 
		PFD_TYPE_RGBA,                    // RGBA ��ɫģʽ 
		16,                               // 24 λ��ɫ��� 
		0, 0, 0, 0, 0, 0,                 // ������ɫλ 
		0,                                // û�з�͸���Ȼ��� 
		0,                                // ������λλ 
		0,                                // ���ۼӻ��� 
		0, 0, 0, 0,                       // �����ۼ�λ 
		16,                               // 32 λ��Ȼ���     
		0,                                // ��ģ�建�� 
		0,                                // �޸������� 
		PFD_MAIN_PLANE,                   // ���� 
		0,                                // ���� 
		0, 0, 0                           // ���Բ�,�ɼ��Ժ������ģ 
	}; 

	if( !(nPixelFormat = ChoosePixelFormat( hDC, &pfd )) )
	{ 
		MessageBox( NULL, TEXT("û�ҵ����ʵ���ʾģʽ"), NULL,
			MB_OK | MB_ICONEXCLAMATION );
		return FALSE;
	}
	// ���õ�ǰ�豸�����ص��ʽ
	SetPixelFormat( hDC, nPixelFormat, &pfd );
	// ��ȡ��Ⱦ�������
	g_hRC = wglCreateContext( hDC );
	// ������Ⱦ�������
	wglMakeCurrent( hDC, g_hRC );

	return TRUE;
}

// ������Դ
VOID LoadResources(HWND hWnd)
{
	// ��ʼ�����
	g_Camera.Init( g_dwWindowWidth, g_dwWindowHeight );

	// �����򵥼�����
	g_pShapeBox = CreateBox( 20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f );
	g_pQuad[0] = CreateZPlane( g_dwWindowWidth, g_dwWindowHeight );
	g_pQuad[1] = CreateZPlane( g_dwWindowWidth / 4, g_dwWindowHeight / 4, 
		-g_dwWindowWidth * 3 / 8, g_dwWindowHeight * 3 / 8 );
	g_pQuad[2] = CreateZPlane( g_dwWindowWidth / 4, g_dwWindowHeight / 4,
		-g_dwWindowWidth * 3 / 8, g_dwWindowHeight * 1 / 8 -  10 );

	// ��������
	CTextureManager::CreateTextureFromFile( "../../Res/NineGrids.png", g_Texture );
	CTextureManager::CreateTextureFromFile( "../../Res/NineGrids.png", g_TreeTexture );

	g_pMRTShader = new MRTShader;
	g_pMRTShader->Create();

	g_pQuadShader = new ScreenQuadShader;
	g_pQuadShader->Create();

	// ����RenderTarget
	glGenTextures( 3, g_TexRT );
	for( int i = 0; i < 3; ++i )
	{
		glBindTexture( GL_TEXTURE_2D, g_TexRT[i] );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 
			g_dwWindowWidth, g_dwWindowHeight, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	// ����RenderBuffer, ����ϵͳ�����Ȼ���
	glGenRenderbuffers( 1, &g_MRTRenderBuffer );
	{
		glBindRenderbuffer( GL_RENDERBUFFER, g_MRTRenderBuffer );
		// �趨���ݴ洢��ʽ
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
			g_dwWindowWidth, g_dwWindowHeight );
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	}

	// ����FBO,Ϊ��Ⱦ��������׼��
	glGenFramebuffers( 1, &g_FBO );
	{
		glBindFramebuffer( GL_FRAMEBUFFER, g_FBO );	
		// �ҽ�RenderTarget��RenderBuffer
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_TexRT[RT_SCENE], 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_TexRT[RT_DEPTH], 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_TexRT[RT_NORMAL], 0 );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_MRTRenderBuffer );
		//�����FBO�Ƿ�����
		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( status != GL_FRAMEBUFFER_COMPLETE )
			MessageBoxA( NULL, NULL, NULL, NULL );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	// �������㻺�����
	glGenBuffers( 1, &g_vboBox );
	{
		// �󶨶��㻺�����
		glBindBuffer( GL_ARRAY_BUFFER, g_vboBox );
		// ָ��������Դ
		ShapeInfo* pShapeInfo = g_pShapeBox;
		// �򶥵㻺�������䶥������
		glBufferData( GL_ARRAY_BUFFER, pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices,
			pShapeInfo->pVertices, GL_STATIC_DRAW );
		// ȡ�����㻺������
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	// �������㻺�����
	glGenBuffers( 3, g_vboQuad );
	for( int i = 0; i < 3; ++i )
	{
		// �󶨶��㻺�����
		glBindBuffer( GL_ARRAY_BUFFER, g_vboQuad[i] );
		// ָ��������Դ
		ShapeInfo* pShapeInfo = g_pQuad[i];		
		// �򶥵㻺�������䶥������
		glBufferData( GL_ARRAY_BUFFER, pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices,
			pShapeInfo->pVertices, GL_STATIC_DRAW );
		// ȡ�����㻺������
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	// �������������������
	glGenBuffers( 1, &g_iboBox );
	{
		// �󶨶��������������
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_iboBox );
		// ָ��������Դ
		ShapeInfo* pShapeInfo = g_pShapeBox;
		// �򶥵�������������������
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 2 * pShapeInfo->uiNumIndices, 
			pShapeInfo->pIndices, GL_STATIC_DRAW );
		// ȡ���󶨶��������������
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}

	// �������������������
	glGenBuffers( 3, g_iboQuad );
	for( int i = 0; i < 3; ++i )
	{
		// �󶨶��������������
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_iboQuad[i] );
		// ָ��������Դ
		ShapeInfo* pShapeInfo = g_pQuad[i];
		// �򶥵�������������������
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 2 * pShapeInfo->uiNumIndices, 
			pShapeInfo->pIndices, GL_STATIC_DRAW );
		// ȡ���󶨶��������������
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}

	// ���������������
	glGenVertexArrays( 1, &g_vaoBox );
	{
		// �󶨶����������
		glBindVertexArray( g_vaoBox );
		// �󶨶��㻺�����
		glBindBuffer( GL_ARRAY_BUFFER, g_vboBox );
		// ָ��������Դ
		ShapeInfo* pShapeInfo = g_pShapeBox;
		// ָ��shader����
		ShaderHelper* pShaderInfo = g_pMRTShader;
		// ��ȡ�Զ������Զ�λ
		GLint uiPos		= glGetAttribLocation( pShaderInfo->uiProgram, "pos");
		GLint uiNormal  = glGetAttribLocation( pShaderInfo->uiProgram, "normal" );
		GLint uiTex		= glGetAttribLocation( pShaderInfo->uiProgram, "tex" );
		// ���ö�������
		glEnableVertexAttribArray( uiPos );
		// ���ö��㷨������
		glEnableVertexAttribArray( uiNormal );
		// ���������������� 
		glEnableVertexAttribArray( uiTex );
		// ����ƫ����
		int offset = 0;
		// �趨����X, Y, Z
		glVertexAttribPointer( uiPos, 3, GL_FLOAT, GL_FALSE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// ����ƫ����
		offset = 12;
		// �趨���㷨��
		glVertexAttribPointer( uiNormal, 3, GL_FLOAT, GL_TRUE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// ����ƫ����
		offset = 24;
		// �趨��������
		glVertexAttribPointer( uiTex, 2, GL_FLOAT, GL_FALSE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// ȡ���󶨶����������
		glBindVertexArray( 0 );
	}

	// ���������������
	glGenVertexArrays( 3, g_vaoQuad );
	for( int i = 0; i < 3; ++i )
	{
		// �󶨶����������
		glBindVertexArray( g_vaoQuad[i] );
		// �󶨶��㻺�����
		glBindBuffer( GL_ARRAY_BUFFER, g_vboQuad[i] );
		// ָ��������Դ
		ShapeInfo* pShapeInfo = g_pQuad[i];
		// ָ��shader����
		ShaderHelper* pShaderInfo = g_pQuadShader;
		// ��ȡ�Զ������Զ�λ
		GLint uiPos		= glGetAttribLocation( pShaderInfo->uiProgram, "pos");
		GLint uiTex		= glGetAttribLocation( pShaderInfo->uiProgram, "tex" );			
		// ���ö�������
		glEnableVertexAttribArray( uiPos );
		// ���������������� 
		glEnableVertexAttribArray( uiTex );
		// ����ƫ����
		int offset = 0;
		// �趨����X, Y, Z
		glVertexAttribPointer( uiPos, 3, GL_FLOAT, GL_FALSE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// ����ƫ����
		offset = 24;
		// �趨��������
		glVertexAttribPointer( uiTex, 2, GL_FLOAT, GL_FALSE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// ȡ���󶨶����������
		glBindVertexArray( 0 );
	}

	g_pBillboardText = new CBillboardText;
	g_pBillboardText->SetFontProperty( "����", 20, 0xFF00FF00, 0xFFFFFFFF );
	g_pBillboardText->SetDisplayPos( XMVectorZero() );
	g_pBillboardText->UpdateText( TEXT("��ǰ֡��:0") );
}

// �ͷ���Դ
VOID ReleaseResources()
{	
	glDeleteTextures( 1, &g_Texture );
	glDeleteTextures( 1, &g_TreeTexture );

	glDeleteFramebuffers( 1, &g_FBO );
	glDeleteTextures( 3, g_TexRT );
	glDeleteRenderbuffers( 1, &g_MRTRenderBuffer );

	glDeleteBuffers( 1, &g_vboBox );
	glDeleteBuffers( 3, g_vboQuad );
	glDeleteBuffers( 1, &g_iboBox );
	glDeleteBuffers( 3, g_iboQuad );

	glDeleteVertexArrays( 3, g_vaoQuad );
	glDeleteVertexArrays( 1, &g_vaoBox );

	SAFE_DELETE( g_pShapeBox );
	for( int i = 0; i < 3; ++i )
		SAFE_DELETE( g_pQuad[i] );
	SAFE_DELETE( g_pMRTShader );
	SAFE_DELETE( g_pQuadShader );
}