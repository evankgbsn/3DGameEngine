#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture
{

public:

	Texture();

	Texture(const std::string& path, const std::string& name);

	~Texture();

	Texture(const Texture&) = delete;

	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) = delete;

	Texture& operator=(Texture&&) = delete;

	int GetWidth() const;

	int GetHeight() const;

	void Bind(unsigned int bindingPoint);

	const std::string& GetName() const;

private:

	friend class TextureManager;

	Texture(const std::string& name, unsigned int textureID);

	void LoadTexture();

	std::string path;

	int width;

	int height;

	int channels;

	unsigned int textureID;

	std::string name;

};


#endif // TEXTURE_H