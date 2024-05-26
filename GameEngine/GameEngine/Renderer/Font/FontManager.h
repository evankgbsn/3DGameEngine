#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <unordered_map>
#include <string>

class Font;

class FontManager
{
public:

	static void Initialize();

	static void Terminate();

	static void LoadFont(const std::string& name, const std::string& path);

	static void UnloadFont(const std::string& name);

	static Font* GetFont(const std::string& name);

private:

	friend class SingletonHelpers;

	friend class Font;

	FontManager();

	~FontManager();

	FontManager(const FontManager&) = delete;

	FontManager& operator=(const FontManager&) = delete;

	FontManager(FontManager&&) = delete;

	FontManager& operator=(FontManager&&) = delete;

	void InitializeFreetype();

	void TerminateFreetype();

	void LoadDefaultFonts();

	static FontManager* instance;

	FT_Library ft;

	std::unordered_map<std::string, Font*> fonts;
};

#endif // FONTMANAGER_H