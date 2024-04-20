#include "Shader.h"

#include "../../Utils/Logger.h"

#include <GL/glew.h>

#include <filesystem>
#include <fstream>

const std::string& Shader::GetName() const
{
	return name;
}

unsigned int Shader::GetId() const
{
	return shaderProgramId;
}

Shader::Shader(const std::string& n, std::filesystem::directory_entry vertShader, std::filesystem::directory_entry fragShader,
	std::filesystem::directory_entry tesseShader, std::filesystem::directory_entry tesscShader,
	std::filesystem::directory_entry geoShader) :
	name(n),
	vertShaderBuffer(std::vector<char>()),
	fragShaderBuffer(std::vector<char>()),
	tesseShaderBuffer(std::vector<char>()),
	tesscShaderBuffer(std::vector<char>()),
	geoShaderBuffer(std::vector<char>()),
	shaderProgramId()
{

	auto readShaderFileToBuffer = [](std::filesystem::directory_entry shaderDirEntry, std::vector<char>& buffer)
		{
			if (std::filesystem::exists(shaderDirEntry.path()))
			{
				std::ifstream file(shaderDirEntry.path().string(), std::ios::ate | std::ios::binary);

				if (!file.is_open())
				{
					Logger::Log(std::string("Could not read shader file ") + shaderDirEntry.path().string(), Logger::Category::Error);
					throw std::runtime_error((std::string("Could not read shader file ") + shaderDirEntry.path().string()).c_str());
					return;
				}

				size_t fileSize = (size_t)file.tellg();
				buffer.resize(fileSize);
				file.seekg(0);
				file.read(buffer.data(), fileSize);
				file.close();

				buffer.push_back('\0');

				Logger::Log("Loaded " + shaderDirEntry.path().filename().string(), Logger::Category::Success);
			}
		};

	readShaderFileToBuffer(vertShader, vertShaderBuffer);
	readShaderFileToBuffer(fragShader, fragShaderBuffer);
	readShaderFileToBuffer(tesseShader, tesseShaderBuffer);
	readShaderFileToBuffer(tesscShader, tesscShaderBuffer);
	readShaderFileToBuffer(geoShader, geoShaderBuffer);

	auto createShader = [](const std::vector<char>& buffer, int shaderType) -> unsigned int
		{
			unsigned int shaderId = UINT_MAX;

			const char* shaderSource = buffer.data();

			if (buffer.size() > 0)
			{
				shaderId = glCreateShader(shaderType);
				glShaderSource(shaderId, 1, &shaderSource, NULL);
				glCompileShader(shaderId);
			}

			return shaderId;
		};

	std::vector<unsigned int> shaders =
	{
		createShader(vertShaderBuffer, GL_VERTEX_SHADER),
		createShader(fragShaderBuffer, GL_FRAGMENT_SHADER),
		createShader(tesseShaderBuffer, GL_TESS_EVALUATION_SHADER),
		createShader(tesscShaderBuffer, GL_TESS_CONTROL_SHADER),
		createShader(geoShaderBuffer, GL_GEOMETRY_SHADER)
	};

	shaderProgramId = glCreateProgram();

	for (const auto& shader : shaders)
	{
		if (shader != UINT_MAX)
		{
			glAttachShader(shaderProgramId, shader);
		}
	}

	glLinkProgram(shaderProgramId);

	GLint linkStatus;
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<GLchar> infoLog(infoLogLength);
		glGetProgramInfoLog(shaderProgramId, infoLogLength, nullptr, infoLog.data());
		Logger::Log(std::string("Shader program linking failed: ") + (const char*)infoLog.data());
	}

	for (const auto& shader : shaders)
	{
		if (shader != UINT_MAX)
		{
			glDeleteShader(shader);
		}
	}
}

Shader::~Shader()
{

}
