
#include "Debug.h"

//==============================================================================

#ifdef _DEBUG
#include <fstream>
#include <string>
void message_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::string msg;

	msg += "Debug message (";
	msg += std::to_string(id);
	msg += "): ";
	msg += message;
	msg += "(";

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:				msg += "Source: API";                break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		msg += "Source: Window System";      break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:	msg += "Source: Shader Compiler";    break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:		msg += "Source: Third Party";        break;
	case GL_DEBUG_SOURCE_APPLICATION:		msg += "Source: Application";        break;
	case GL_DEBUG_SOURCE_OTHER:				msg += "Source: Other";              break;
	}

	msg += ", ";

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               msg += "Type: Error";                break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: msg += "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  msg += "Type: Undefined Behaviour";  break;
	case GL_DEBUG_TYPE_PORTABILITY:         msg += "Type: Portability";          break;
	case GL_DEBUG_TYPE_PERFORMANCE:         msg += "Type: Performance";          break;
	case GL_DEBUG_TYPE_MARKER:              msg += "Type: Marker";               break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          msg += "Type: Push Group";           break;
	case GL_DEBUG_TYPE_POP_GROUP:           msg += "Type: Pop Group";            break;
	case GL_DEBUG_TYPE_OTHER:               msg += "Type: Other";                break;
	}

	msg += ", ";

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:			msg += "Severity: high";             break;
	case GL_DEBUG_SEVERITY_MEDIUM:			msg += "Severity: medium";           break;
	case GL_DEBUG_SEVERITY_LOW:				msg += "Severity: low";              break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:	msg += "Severity: notification";     break;
	}

	msg += ")";

	std::ofstream log("log.txt", std::ios::app);

	if (type == GL_DEBUG_TYPE_ERROR)
	{
		log << "error: ";
	}
	else
	{
		log << "info: ";
	}

	log << message << std::endl;
	log.close();
}
#endif // _DEBUG

//==============================================================================
