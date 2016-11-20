//////////////////////////////////////////////////////////////////////////
//  作者:梅军
//  Copyright Reserved
//	2009年7月3日
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

// 全局变量:
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

	// TODO: 在此放置代码。
	MSG msg;
	// 初始化全局字符串
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	// 主消息循环:
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
	case WM_CREATE:						// 建立窗口
		{
			SetTimer( hWnd, ID_FPS, 1000, NULL );
			g_hDC = GetDC( hWnd );		// 获取当前窗口的设备句柄
			SetupPixelFormat( g_hDC );	// 调用显示模式安装功能
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
				_stprintf( strFPS, _T("当前帧速:%d"), dwFPS );
				g_pBillboardText->UpdateText( strFPS );
			}
			break;
		}

	case WM_SIZE:									// 窗口尺寸变化
		{
			g_dwWindowHeight = HIWORD(lParam);			// 窗口的高
			g_dwWindowWidth  = LOWORD(lParam);			// 窗口的宽
			if ( g_dwWindowHeight == 0)	
				g_dwWindowHeight = 1;					// 防止被0除

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

// “关于”框的消息处理程序。
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

// 初始化OpenGL环境
BOOL InitGL(HWND hwnd)
{		
	// 设置着色模式
	glShadeModel( GL_SMOOTH );
	// 设置深度清除值
	glClearDepth( 1.0f );
	// 设置深度测试函数
	glDepthFunc( GL_LEQUAL );
	//　启用深度测试
	glEnable( GL_DEPTH_TEST );
	// 启用纹理映射
	glEnable( GL_TEXTURE_2D );

	return TRUE;
}

// 渲染
VOID Render()										 
{
	// 设定渲染目标
	glBindFramebuffer( GL_FRAMEBUFFER, g_FBO );
	ShaderHelper* pShaderInfo = g_pMRTShader;
	// 设定着色器参数
	{
		// 启用着色程序
		glUseProgram( pShaderInfo->uiProgram );
		// 指定绘制缓冲区
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, 
			GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers( 3, attachments );

		// 设定坐标系及坐标变换
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

	//　清除背景
	glClearColor( 0.5f, 0.5f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	ShapeInfo* pShapeInfo = g_pShapeBox;
	// 绘制图形
	{
		// 设定纹理变量
		glUniform1i( glGetUniformLocation( pShaderInfo->uiProgram, "testMap"), 0 );
		glUniform1i( glGetUniformLocation( pShaderInfo->uiProgram, "treeMap"), 1 );
		// 设置着色器变量
		glUniform1f( glGetUniformLocation( pShaderInfo->uiProgram, "g_FarDist" ), FAR_DIST );
		// 激活纹理单元0
		glActiveTexture( GL_TEXTURE0 );
		// 绑定纹理
		glBindTexture( GL_TEXTURE_2D, g_Texture );
		// 激活纹理单元1
		glActiveTexture( GL_TEXTURE1 );
		// 绑定纹理
		glBindTexture( GL_TEXTURE_2D, g_TreeTexture );
		// 绑定顶点数组对象
		glBindVertexArray( g_vaoBox );
		// 绑定顶点索引缓冲对象
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_iboBox );
		// 绘制图元
		glDrawElements( GL_TRIANGLES, pShapeInfo->uiNumIndices,  
			GL_UNSIGNED_SHORT, (GLvoid*)0 );
		// 取消绑定顶点数组对象
		glBindVertexArray( 0 );		
	}
	// 还原渲染目标
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	pShaderInfo = g_pQuadShader;
	// 设定着色器参数
	{
		// 启用着色程序
		glUseProgram( pShaderInfo->uiProgram );
		// 指定绘制缓冲区
		glDrawBuffer( GL_COLOR_ATTACHMENT0 );
		// 设定投影矩阵
		XMMATRIX matOrtho = XMMatrixOrthographicLH(
			g_dwWindowWidth, g_dwWindowHeight, NEAR_DIST, FAR_DIST );
		GLint iWVP	  = glGetUniformLocation(pShaderInfo->uiProgram, "g_matWVP");
		glUniformMatrix4fv( iWVP, 1, GL_FALSE, (float*)&(matOrtho) );
	}

	//清除背景
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// 绘制
	{
		// 获取并设置变量
		GLint uiScene = glGetUniformLocation( pShaderInfo->uiProgram, "SceneMap" );
		glUniform1i( uiScene, 0 );	
		for( int i = 0; i < 3; ++i )
		{
			pShapeInfo = g_pQuad[i];
			// 激活纹理单元0
			glActiveTexture( GL_TEXTURE0 );
			// 绑定纹理
			glBindTexture( GL_TEXTURE_2D, g_TexRT[i] );
			// 绑定顶点数组对象
			glBindVertexArray( g_vboQuad[i] );
			// 绑定顶点索引缓冲对象
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_iboQuad[i] );
			// 绘制图元
			glDrawElements( GL_TRIANGLES, pShapeInfo->uiNumIndices,  
				GL_UNSIGNED_SHORT, (GLvoid*)0 );
			// 取消绑定顶点数组对象
			glBindVertexArray( 0 );
		}
	}
	g_pBillboardText->Render();
	// 交换后备缓冲区
	SwapBuffers( g_hDC );
}

// 设置OpenGL像素格式
BOOL SetupPixelFormat( HDC hDC )
{
	// 像素点格式
	int nPixelFormat = -1;
	PIXELFORMATDESCRIPTOR pfd =
	{ 
		sizeof(PIXELFORMATDESCRIPTOR),	  // pfd结构的大小 
		1,							 	  // 版本号 
		PFD_DRAW_TO_WINDOW |              // 支持在窗口中绘图 
		PFD_SUPPORT_OPENGL |              // 支持 OpenGL 
		PFD_DOUBLEBUFFER,                 // 双缓存模式 
		PFD_TYPE_RGBA,                    // RGBA 颜色模式 
		16,                               // 24 位颜色深度 
		0, 0, 0, 0, 0, 0,                 // 忽略颜色位 
		0,                                // 没有非透明度缓存 
		0,                                // 忽略移位位 
		0,                                // 无累加缓存 
		0, 0, 0, 0,                       // 忽略累加位 
		16,                               // 32 位深度缓存     
		0,                                // 无模板缓存 
		0,                                // 无辅助缓存 
		PFD_MAIN_PLANE,                   // 主层 
		0,                                // 保留 
		0, 0, 0                           // 忽略层,可见性和损毁掩模 
	}; 

	if( !(nPixelFormat = ChoosePixelFormat( hDC, &pfd )) )
	{ 
		MessageBox( NULL, TEXT("没找到合适的显示模式"), NULL,
			MB_OK | MB_ICONEXCLAMATION );
		return FALSE;
	}
	// 设置当前设备的像素点格式
	SetPixelFormat( hDC, nPixelFormat, &pfd );
	// 获取渲染描述句柄
	g_hRC = wglCreateContext( hDC );
	// 激活渲染描述句柄
	wglMakeCurrent( hDC, g_hRC );

	return TRUE;
}

// 加载资源
VOID LoadResources(HWND hWnd)
{
	// 初始化相机
	g_Camera.Init( g_dwWindowWidth, g_dwWindowHeight );

	// 创建简单几何体
	g_pShapeBox = CreateBox( 20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f );
	g_pQuad[0] = CreateZPlane( g_dwWindowWidth, g_dwWindowHeight );
	g_pQuad[1] = CreateZPlane( g_dwWindowWidth / 4, g_dwWindowHeight / 4, 
		-g_dwWindowWidth * 3 / 8, g_dwWindowHeight * 3 / 8 );
	g_pQuad[2] = CreateZPlane( g_dwWindowWidth / 4, g_dwWindowHeight / 4,
		-g_dwWindowWidth * 3 / 8, g_dwWindowHeight * 1 / 8 -  10 );

	// 创建纹理
	CTextureManager::CreateTextureFromFile( "../../Res/NineGrids.png", g_Texture );
	CTextureManager::CreateTextureFromFile( "../../Res/NineGrids.png", g_TreeTexture );

	g_pMRTShader = new MRTShader;
	g_pMRTShader->Create();

	g_pQuadShader = new ScreenQuadShader;
	g_pQuadShader->Create();

	// 创建RenderTarget
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

	// 创建RenderBuffer, 用于系统存放深度缓存
	glGenRenderbuffers( 1, &g_MRTRenderBuffer );
	{
		glBindRenderbuffer( GL_RENDERBUFFER, g_MRTRenderBuffer );
		// 设定数据存储格式
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
			g_dwWindowWidth, g_dwWindowHeight );
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	}

	// 创建FBO,为渲染到纹理做准备
	glGenFramebuffers( 1, &g_FBO );
	{
		glBindFramebuffer( GL_FRAMEBUFFER, g_FBO );	
		// 挂接RenderTarget及RenderBuffer
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_TexRT[RT_SCENE], 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_TexRT[RT_DEPTH], 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_TexRT[RT_NORMAL], 0 );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_MRTRenderBuffer );
		//　检测FBO是否正常
		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( status != GL_FRAMEBUFFER_COMPLETE )
			MessageBoxA( NULL, NULL, NULL, NULL );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	// 创建顶点缓冲对象
	glGenBuffers( 1, &g_vboBox );
	{
		// 绑定顶点缓冲对象
		glBindBuffer( GL_ARRAY_BUFFER, g_vboBox );
		// 指定数据来源
		ShapeInfo* pShapeInfo = g_pShapeBox;
		// 向顶点缓冲对象填充顶点数据
		glBufferData( GL_ARRAY_BUFFER, pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices,
			pShapeInfo->pVertices, GL_STATIC_DRAW );
		// 取消顶点缓冲对象绑定
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	// 创建顶点缓冲对象
	glGenBuffers( 3, g_vboQuad );
	for( int i = 0; i < 3; ++i )
	{
		// 绑定顶点缓冲对象
		glBindBuffer( GL_ARRAY_BUFFER, g_vboQuad[i] );
		// 指定数据来源
		ShapeInfo* pShapeInfo = g_pQuad[i];		
		// 向顶点缓冲对象填充顶点数据
		glBufferData( GL_ARRAY_BUFFER, pShapeInfo->uiElementSize * pShapeInfo->uiNumVertices,
			pShapeInfo->pVertices, GL_STATIC_DRAW );
		// 取消顶点缓冲对象绑定
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	// 创建顶点索引缓冲对象
	glGenBuffers( 1, &g_iboBox );
	{
		// 绑定顶点索引缓冲对象
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_iboBox );
		// 指定数据来源
		ShapeInfo* pShapeInfo = g_pShapeBox;
		// 向顶点索引缓冲对象填充数据
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 2 * pShapeInfo->uiNumIndices, 
			pShapeInfo->pIndices, GL_STATIC_DRAW );
		// 取消绑定顶点索引缓冲对象
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}

	// 创建顶点索引缓冲对象
	glGenBuffers( 3, g_iboQuad );
	for( int i = 0; i < 3; ++i )
	{
		// 绑定顶点索引缓冲对象
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_iboQuad[i] );
		// 指定数据来源
		ShapeInfo* pShapeInfo = g_pQuad[i];
		// 向顶点索引缓冲对象填充数据
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 2 * pShapeInfo->uiNumIndices, 
			pShapeInfo->pIndices, GL_STATIC_DRAW );
		// 取消绑定顶点索引缓冲对象
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}

	// 创建顶点数组对象
	glGenVertexArrays( 1, &g_vaoBox );
	{
		// 绑定顶点数组对象
		glBindVertexArray( g_vaoBox );
		// 绑定顶点缓冲对象
		glBindBuffer( GL_ARRAY_BUFFER, g_vboBox );
		// 指定数据来源
		ShapeInfo* pShapeInfo = g_pShapeBox;
		// 指定shader程序
		ShaderHelper* pShaderInfo = g_pMRTShader;
		// 获取自定义属性定位
		GLint uiPos		= glGetAttribLocation( pShaderInfo->uiProgram, "pos");
		GLint uiNormal  = glGetAttribLocation( pShaderInfo->uiProgram, "normal" );
		GLint uiTex		= glGetAttribLocation( pShaderInfo->uiProgram, "tex" );
		// 启用顶点属性
		glEnableVertexAttribArray( uiPos );
		// 启用顶点法线属性
		glEnableVertexAttribArray( uiNormal );
		// 启用纹理坐标属性 
		glEnableVertexAttribArray( uiTex );
		// 数据偏移量
		int offset = 0;
		// 设定顶点X, Y, Z
		glVertexAttribPointer( uiPos, 3, GL_FLOAT, GL_FALSE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// 数据偏移量
		offset = 12;
		// 设定顶点法线
		glVertexAttribPointer( uiNormal, 3, GL_FLOAT, GL_TRUE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// 数据偏移量
		offset = 24;
		// 设定纹理坐标
		glVertexAttribPointer( uiTex, 2, GL_FLOAT, GL_FALSE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// 取消绑定顶点数组对象
		glBindVertexArray( 0 );
	}

	// 创建顶点数组对象
	glGenVertexArrays( 3, g_vaoQuad );
	for( int i = 0; i < 3; ++i )
	{
		// 绑定顶点数组对象
		glBindVertexArray( g_vaoQuad[i] );
		// 绑定顶点缓冲对象
		glBindBuffer( GL_ARRAY_BUFFER, g_vboQuad[i] );
		// 指定数据来源
		ShapeInfo* pShapeInfo = g_pQuad[i];
		// 指定shader程序
		ShaderHelper* pShaderInfo = g_pQuadShader;
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
		glVertexAttribPointer( uiPos, 3, GL_FLOAT, GL_FALSE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// 数据偏移量
		offset = 24;
		// 设定纹理坐标
		glVertexAttribPointer( uiTex, 2, GL_FLOAT, GL_FALSE, pShapeInfo->uiElementSize, (GLvoid*)offset );
		// 取消绑定顶点数组对象
		glBindVertexArray( 0 );
	}

	g_pBillboardText = new CBillboardText;
	g_pBillboardText->SetFontProperty( "楷体", 20, 0xFF00FF00, 0xFFFFFFFF );
	g_pBillboardText->SetDisplayPos( XMVectorZero() );
	g_pBillboardText->UpdateText( TEXT("当前帧速:0") );
}

// 释放资源
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