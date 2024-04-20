#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <filesystem>
#include <vector>

class Shader
{
	
public:

	const std::string& GetName() const;

	unsigned int GetId() const;

private:
	
	friend class ShaderManager;

	Shader(const std::string& name, const std::filesystem::directory_entry vertShader, std::filesystem::directory_entry fragShader, 
		std::filesystem::directory_entry tesseShader, std::filesystem::directory_entry tesscShader,
		std::filesystem::directory_entry geoShader);

	Shader() = delete;

	~Shader();

	Shader(const Shader&) = delete;

	Shader& operator=(const Shader&) =  delete;

	Shader(Shader&&) = delete;

	Shader& operator=(Shader&&) =  delete;

	std::string name;

	std::vector<char> vertShaderBuffer;
	
	std::vector<char> fragShaderBuffer;

	std::vector<char> tesseShaderBuffer;

	std::vector<char> tesscShaderBuffer;

	std::vector<char> geoShaderBuffer;

	unsigned int shaderProgramId;

};

#endif // SHADER_H