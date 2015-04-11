#pragma once

#include <windows.h>
#define GLEW_STATIC
#include <gl/glew.h>
#include <stdio.h>

class ShaderHelper
{
public:
	GLuint uiProgram;

public:
	virtual ~ShaderHelper(){};

	static VOID CheckShaderInfo( GLuint shader )
	{
		GLsizei	bufSize;
		GLsizei length;
		GLchar  *pLog;

		glGetShaderiv( shader, GL_INFO_LOG_LENGTH , &bufSize);
		if ( bufSize > 1 )
		{
			pLog = new GLchar[bufSize];
			if ( pLog != NULL)
			{
				glGetShaderInfoLog( shader, bufSize, &length, pLog );
				MessageBoxA( NULL, pLog, NULL, NULL );
				delete []pLog;
			}
			else
				MessageBoxA( NULL, "Could not allocate InfoLog buffer!", NULL, NULL );
		}
	}

	static VOID CheckProgramInfo( GLuint program )
	{
		GLsizei	bufSize;
		GLsizei length;
		GLchar  *pLog;

		glGetProgramiv( program, GL_INFO_LOG_LENGTH , &bufSize);
		if ( bufSize > 1 )
		{
			pLog = new GLchar[bufSize];
			if ( pLog != NULL)
			{
				glGetProgramInfoLog( program, bufSize, &length, pLog );
				MessageBoxA( NULL, pLog, NULL, NULL );
				delete []pLog;
			}
			else
				MessageBoxA( NULL, "Could not allocate InfoLog buffer!", NULL, NULL );
		}
	}

	static GLuint CreateShader( GLenum enumShaderType, const char* szShaderSource )
	{
		GLint bCompiled = 0;

		GLuint uiShader = glCreateShader( enumShaderType );

		glShaderSource( uiShader, 1, (const GLchar**)&szShaderSource, NULL );

		glCompileShader( uiShader );
		glGetShaderiv( uiShader, GL_COMPILE_STATUS, &bCompiled );

		if( !bCompiled )
		{
			CheckShaderInfo( uiShader );
			glDeleteShader( uiShader );
			return 0;
		}

		return uiShader;
	}

	static GLuint CreateShaderFromFile( GLenum enumShaderType, const char* lpFileName )
	{
		GLint bCompiled = 0;

		FILE* pFile = fopen( lpFileName, "rb" );
		fseek( pFile, 0, SEEK_END );

		int file_length = ftell( pFile );
		GLchar* pShaderSource = new GLchar[file_length + 1];
		fseek( pFile, 0, SEEK_SET );
		fread( pShaderSource, 1, file_length, pFile );
		pShaderSource[file_length] = '\0';

		fclose( pFile );

		GLuint uiShader = glCreateShader( enumShaderType );

		glShaderSource( uiShader, 1, (const GLchar**)&pShaderSource, NULL );

		delete []pShaderSource;

		glCompileShader( uiShader );
		glGetShaderiv( uiShader, GL_COMPILE_STATUS, &bCompiled );

		if( !bCompiled )
		{
			CheckShaderInfo( uiShader );
			glDeleteShader( uiShader );
			return 0;
		}

		return uiShader;
	}

	static GLuint CreateProgram( GLuint uiVSShader, GLuint uiPSShader )
	{
		GLuint uiProgram = glCreateProgram();

		if( uiVSShader )
			glAttachShader( uiProgram, uiVSShader );

		if( uiPSShader )
			glAttachShader( uiProgram, uiPSShader );

		glLinkProgram( uiProgram );
		GLint bLinked = 0;
		glGetProgramiv( uiProgram, GL_LINK_STATUS, &bLinked );

		if( !bLinked )
		{
			CheckProgramInfo( uiProgram );
			glDeleteProgram( uiProgram );

			return 0;
		}

		return uiProgram;
	}
};