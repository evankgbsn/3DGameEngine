#include "FontManager.h"

#include "../../Utils/SingletonHelpers.h"
#include "Font.h"

FontManager* FontManager::instance = nullptr;

void FontManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "FontManager");

	instance->LoadDefaultFonts();
}

void FontManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "FontManager");
}

void FontManager::LoadFont(const std::string& name, const std::string& path)
{
	if (instance != nullptr)
	{
		if (instance->fonts.find(name) == instance->fonts.end())
		{
			instance->fonts[name] = new Font(path);
		}
		else
		{
			Logger::Log("A font with the name: " + name + " has already been loaded.", Logger::Category::Warning);
		}
	}
	else
	{
		Logger::Log("Calling FontManager::LoadFont before FontManager::Initialize", Logger::Category::Error);
	}
}

void FontManager::UnloadFont(const std::string& name)
{
	if (instance != nullptr)
	{
		if (instance->fonts.find(name) != instance->fonts.end())
		{
			delete instance->fonts[name];

			instance->fonts.erase(instance->fonts.find(name));

			Logger::Log("Unloaded font: " + name, Logger::Category::Success);
		}
		else
		{
			Logger::Log("A font with the name: " + name + " has not been loaded.", Logger::Category::Warning);
		}
	}
	else
	{
		Logger::Log("Calling FontManager::LoadFont before FontManager::Initialize", Logger::Category::Error);
	}
}

Font* FontManager::GetFont(const std::string& name)
{
	if (instance != nullptr)
	{
		if (instance->fonts.find(name) != instance->fonts.end())
		{
			return instance->fonts.find(name)->second;
		}
		else
		{
			Logger::Log("Could Not find font: " + name + " FontManager::GetFont()", Logger::Category::Error);
		}
	}
	else
	{
		Logger::Log("Calling FontManager::GetFont before FontManager::Initialzie()", Logger::Category::Error);
	}

	return nullptr;
}

FontManager::FontManager() :
	fonts(std::unordered_map<std::string, Font*>())
{
	InitializeFreetype();
}

FontManager::~FontManager()
{
	for (auto& font : fonts)
	{
		if (font.second != nullptr)
		{
			delete font.second;
		}
	}

	TerminateFreetype();
}

void FontManager::InitializeFreetype()
{
	if (FT_Init_FreeType(&ft))
	{
		Logger::Log("Could not initialize freetype", Logger::Category::Error);
	}
}

void FontManager::TerminateFreetype()
{
	FT_Done_FreeType(ft);
}

void FontManager::LoadDefaultFonts()
{
	LoadFont("arial", "Assets/Font/arial.ttf");
	LoadFont("caveat", "Assets/Font/Caveat-VariableFont_wght.ttf");
	LoadFont("times", "Assets/Font/times.ttf");
	LoadFont("exo2", "Assets/Font/Exo2-VariableFont_wght.ttf");
}
