#ifndef UNDEROCEAN_RESOURCEMANAGER_H
#define UNDEROCEAN_RESOURCEMANAGER_H

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

class ResourceManager {
public:
    ResourceManager() = delete;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;
    static sf::Texture& getTexture(const std::string& name) {
        auto fullPath = makeAssetPath(name);
        if (auto it = textures_.find(fullPath); it != textures_.end()) return *(it->second);
        textures_[fullPath] = std::make_unique<sf::Texture>();
        auto& texture = *textures_[fullPath];
        if (!texture.loadFromFile(fullPath)) {
            assert(false && "Texture file not found!");
        }
        return texture;
    }
    static sf::Font& getFont(const std::string& name) {
        auto fullPath = makeAssetPath(name);
        if (auto it = fonts_.find(fullPath); it != fonts_.end()) return it->second;
        fonts_[fullPath] = sf::Font();
        auto& font = fonts_[fullPath];
        fontsData_[fullPath] = readFileBytes(fullPath);
        auto& buffer = fontsData_[fullPath];
        if (!font.loadFromMemory(buffer.data(), buffer.size())) {
            assert(false && "Font file not found or failed to load!");
        }
        disableFontSmooth(font);
        return font;
    }
private:
    static std::vector<sf::Uint8> readFileBytes(const std::string& path) {
        FILE* fp = nullptr;
        fopen_s(&fp, path.c_str(), "rb");
        assert(fp && "Asset file not found!");
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        std::vector<sf::Uint8> buffer(size);
        fread(buffer.data(), 1, size, fp);
        fclose(fp);
        return buffer;
    }
    static std::string makeAssetPath(const std::string& name) {
        return "assets/" + name;
    }
    static void disableFontSmooth(sf::Font& font) {
        for (unsigned size = 8; size < 256; ++size) {
            font.getGlyph(U'A', size, false);
            auto& tex = const_cast<sf::Texture&>(font.getTexture(size));
            tex.setSmooth(false);
        }
    }
    inline static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures_;
    inline static std::map<std::string, sf::Font> fonts_;
    inline static std::map<std::string, std::vector<sf::Uint8>> fontsData_;
};

#endif // UNDEROCEAN_RESOURCEMANAGER_H
