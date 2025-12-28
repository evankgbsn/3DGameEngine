#ifndef OPENGL_DEBUG_H
#define OPENGL_DEBUG_H

#include "Logger.h"

#include <cstdio>
#include "Gl/glew.h"

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	userParam = userParam;
	length = length;
	id = id;
	source = source;

	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
	{
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			Logger::Log(std::string("OpenGL: ") + std::string("Severity: ") + std::to_string(severity) + " " + message, Logger::Category::Error);
			break;
		default:
			Logger::Log(std::string("OpenGL: ") + std::string("Severity: ") + std::to_string(severity) + " " + message, Logger::Category::Warning);
			break;
		}
	}
}

class GLDebug
{
public:
	GLDebug()
	{
		//----------------------------------------------------------------
		// Enable debug messages..	
		//----------------------------------------------------------------
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
		//----------------------------------------------------------------
	}
};

#endif
