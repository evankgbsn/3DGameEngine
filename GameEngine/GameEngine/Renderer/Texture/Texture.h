#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture
{

public:

	Texture();

	Texture(const std::string& path);

	~Texture();

	Texture(const Texture&) = delete;

	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) = delete;

	Texture& operator=(Texture&&) = delete;

	int GetWidth() const;

	int GetHeight() const;

	void Bind(unsigned int bindingPoint);

private:

	void LoadTexture();

	std::string path;

	int width;

	int height;

	int channels;

	unsigned int textureID;

};


#endif // TEXTURE_H