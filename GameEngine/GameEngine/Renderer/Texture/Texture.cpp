#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../../Utils/Logger.h"

#include <stb_image/stb_image.h>
#include <GL/glew.h>

Texture::Texture() :
	path("Assets/Texture/Woman.png")
{
	LoadTexture();
}

Texture::Texture(const std::string& p, const std::string& n) :
	path(p),
	name(n)
{
	LoadTexture();
}

Texture::~Texture()
{

}

int Texture::GetWidth() const
{
	return width;
}

int Texture::GetHeight() const
{
	return height;
}

void Texture::Bind(unsigned int bindingPoint)
{
	glActiveTexture(bindingPoint);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

const std::string& Texture::GetName() const
{
	return name;
}

void Texture::LoadTexture()
{
	stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

	if (!pixels)
	{
		Logger::Log(std::string("Failed to load image texture ") + path, Logger::Category::Error);
		return;
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (channels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		Logger::Log("Loaded Texture: " + path, Logger::Category::Success);
	}
	else if (channels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		Logger::Log("Loaded Texture: " + path, Logger::Category::Success);
	}

	free(pixels);
}
