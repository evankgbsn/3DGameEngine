#include "TextureManager.h"

#include "../../Utils/Logger.h"
#include "Texture.h"

TextureManager* TextureManager::instance = nullptr;

std::unordered_set<std::string> TextureManager::defaultTextureNames = std::unordered_set<std::string>({ "0X00000000", "0XFFFFFFFF", "Grey"});

void TextureManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new TextureManager();
		instance->LoadDefaultTextures();
	}
	else
	{
		Logger::Log(std::string("Calling TextureManager::Initialize() before TextureManager::Terminate()."), Logger::Category::Warning);
	}
}

void TextureManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
	}
	else
	{
		Logger::Log(std::string("Calling TextureManager::Terminate() before TextureManager::Initialize()."), Logger::Category::Warning);
	}
}

Texture* const TextureManager::LoadTexture(const std::string& filePath, const std::string& name)
{
	Texture* ret = nullptr;
	if (instance != nullptr)
	{
		if (instance->textures.find(name) == instance->textures.end())
		{
			ret = new Texture(filePath, name);
			instance->textures.insert(std::make_pair(name, ret));
		}
		else
		{
			Logger::Log(std::string("A texture with the name ") + name + " already exists. TextureManager::LoadTexture.", Logger::Category::Warning);
			return instance->textures[name];
		}
	}
	else
	{
		Logger::Log(std::string("Calling TextureManager::LoadTextrue() before TextureManager::Initialize()."), Logger::Category::Warning);
	}

	return ret;
}

Texture* const TextureManager::GetTexture(const std::string& name)
{
	Texture* ret = nullptr;
	if (instance != nullptr)
	{
		if (instance->textures.find(name) != instance->textures.end())
		{
			ret = instance->textures.find(name)->second;
		}
		else
		{
			Logger::Log(std::string("A texture with the name ") + name + " deos not exists. TextureManager::GetTexture.", Logger::Category::Warning);
		}
	}
	else
	{
		Logger::Log(std::string("Calling TextureManager::GetTextrue() before TextureManager::Initialize()."), Logger::Category::Warning);
	}

	return ret;
}

void TextureManager::UnloadTexture(const std::string& name)
{
	if (instance != nullptr)
	{
		if (instance->textures.find(name) != instance->textures.end())
		{
			delete instance->textures.find(name)->second;
			instance->textures.erase(instance->textures.find(name));
		}
		else
		{
			Logger::Log(std::string("A texture with the name ") + name + " deos not exists. TextureManager::UnloadTexture.", Logger::Category::Warning);
		}
	}
	else
	{
		Logger::Log(std::string("Calling TextureManager::UnloadTextrue() before TextureManager::Initialize()."), Logger::Category::Warning);
	}
}

bool TextureManager::TextureLoaded(const std::string& name)
{
	if (instance != nullptr)
	{
		return instance->textures.find(name) != instance->textures.end();
	}
	else
	{
		Logger::Log(std::string("Calling TextureManager::TextureLoaded() before TextureManager::Initialize()."), Logger::Category::Warning);
		return false;
	}
}

TextureManager::TextureManager() :
	textures(std::unordered_map<std::string, Texture*>())
{

}

TextureManager::~TextureManager()
{
	for (const auto& texture : textures)
	{
		delete texture.second;
	}

	textures.clear();

	instance = nullptr;
}

void TextureManager::LoadDefaultTextures()
{
	LoadTexture("Assets/Texture/Woman.png", "Woman");
}
