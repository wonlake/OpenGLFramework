//
// Copyright (C) Mei Jun 2011
//

#include "MRTShader.h"
#include "utility.h"

MRTShader::MRTShader()
{
	uiProgram = 0;
}

MRTShader::~MRTShader()
{
	if( uiProgram )
	{
		glDeleteProgram( uiProgram );
		uiProgram = 0;
	}
}

bool MRTShader::Create()
{
	GLuint uiVSShader = 0;
	GLuint uiPSShader = 0;

	static char strVS[] =	
	{
		"#version 460\n"
		"\n"
		"in vec3 pos;\n"
		"in vec3 normal;\n"
		"in vec2 tex;\n"
		"\n"
		"uniform mat4x4 g_matWVP;\n"
		"uniform mat4x4 g_matWV;\n"
		"uniform mat4x4 g_matNormal;\n"
		"\n"
		"out vec3 _normal;\n"
		"out vec3 _tex;\n"
		"\n"
		"void main()\n"
		"{\n"
		"  gl_Position = g_matWVP * vec4(pos, 1.0f);\n"
		"  _tex.xy = tex;\n"
		"  vec4 depth = g_matWV * vec4(pos, 1.0f);\n"
		"  _normal = (g_matNormal * vec4(normal, 1.0f)).xyz;\n"
		"  _tex.z = depth.z;\n"
		"}\n"
	};

	static char strPS[] = 
	{
		"#version 460\n"
		"\n"
		"uniform float g_FarDist;\n"
		"uniform sampler2D testMap;\n"
		"uniform sampler2D treeMap;\n"
		"\n"
		"in vec3 _normal;\n"
		"in vec3 _tex;\n"
		"out vec4 FragData[3];\n"
		"\n"
		"void main()\n"
		"{\n"
		"	float depth = _tex.z / g_FarDist;\n"
		"	vec4 color1 = texture2D( testMap, _tex.xy );\n"
		"	vec4 color2 = texture2D( treeMap, _tex.xy );\n"
		"	vec3 n = normalize(_normal);\n"
		"	n = (n + 1.0f) * 0.5f;\n"
		"\n"
		"	FragData[0] = color1 * 0.8f + color2 * 0.2f;\n"
		"	FragData[1] = vec4( depth, depth, depth, 1.0f );\n"
		"	FragData[2] = vec4( n.x, n.y, n.z, 1.0f );\n"
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