#ifndef OPENGL_DEBUG_H
#define OPENGL_DEBUG_H

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

	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
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
