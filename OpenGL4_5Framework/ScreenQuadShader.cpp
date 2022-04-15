//
// Copyright (C) Mei Jun 2011
//

#include "ScreenQuadShader.h"
#include "utility.h"

ScreenQuadShader::ScreenQuadShader()
{
	uiProgram = 0;
}

ScreenQuadShader::~ScreenQuadShader()
{
	if( uiProgram )
	{
		glDeleteProgram( uiProgram );
		uiProgram = 0;
	}
}

bool ScreenQuadShader::Create()
{
	GLuint uiVSShader = 0;
	GLuint uiPSShader = 0;

	static char strVS[] =
	{
		"#version 330\n"
		"\n"
		"in vec3 pos;\n"
		"in vec2 tex;\n"
		"\n"
		"uniform mat4x4 g_matWVP;\n"
		"\n"
		"out vec2 _tex;\n"
		"\n"
		"void main()\n"
		"{\n"
		"  gl_Position = g_matWVP * vec4(pos, 1.0f);\n"
		"  _tex = tex;\n"
		"}\n"
	};
	
	static char strPS[] = 
	{
		"#version 330\n"
		"\n"
		"uniform sampler2D SceneMap;\n"
		"\n"
		"in vec2 _tex;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	vec4 SceneColor = texture2D( SceneMap, _tex ); \n"
		"	gl_FragColor = SceneColor;\n"
		"}\n"
	};
	
	uiVSShader = CreateShader( GL_VERTEX_SHADER, strVS );
	uiPSShader = CreateShader( GL_FRAGMENT_SHADER, strPS );

	uiProgram = CreateProgram( uiPSShader, uiVSShader );

	if( uiVSShader )
		glAttachShader( uiProgram, uiVSShader );

	if( uiPSShader )
		glAttachShader( uiProgram, uiPSShader );

	if( uiProgram )
		return true;
	else
		return false;
}