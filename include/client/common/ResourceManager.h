#ifndef UNDEROCEAN_RESOURCEMANAGER_H
#define UNDEROCEAN_RESOURCEMANAGER_H

#include <cassert>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
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
    inline static std::mutex resourceMutex_;
    static bool hasPreloaded() {
        return hasPreLoaded_;
    }
    static void preload();  //preload commonly used assets to avoid runtime stutter, independent thread
    static sf::Texture& getTexture(const std::string& name) {
        auto fullPath = makeAssetPath(name);
        if (auto it = textures_.find(fullPath); it != textures_.end())
            return *(it->second);
        std::scoped_lock lock(resourceMutex_);
        auto tex = std::make_unique<sf::Texture>();
        if (!tex->loadFromFile(fullPath)) {
            assert(false && "Texture file not found!");
        }
        auto& ref = *tex;
        textures_[fullPath] = std::move(tex);
        return *textures_[fullPath];
    }
    static sf::Font& getFont(const std::string& name) {
        auto fullPath = makeAssetPath(name);
        if (auto it = fonts_.find(fullPath); it != fonts_.end())
            return it->second;
        std::scoped_lock lock(resourceMutex_);
        fontsData_[fullPath] = readFileBytes(fullPath);
        auto& buffer = fontsData_[fullPath];
        auto& font = fonts_[fullPath];
        if (!font.loadFromMemory(buffer.data(), buffer.size())) {
            assert(false && "Font file not found!");
        }
        disableFontSmooth(font);
        return fonts_[fullPath];
    }
private:
    inline static bool hasPreLoaded_ { false };
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
        // for (unsigned size = 8; size < 256; ++size) {
        //     font.getGlyph(U'A', size, false);
        //     auto& tex = const_cast<sf::Texture&>(font.getTexture(size));
        //     tex.setSmooth(false);
        // }
        const char *ASCII_CHARS = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
        for (unsigned size = 8; size <= 128; ++size) {
            for (const char* c = ASCII_CHARS; *c != '\0'; ++c) {
                font.getGlyph(*c, size, false);
                auto& tex = const_cast<sf::Texture&>(font.getTexture(size));
                tex.setSmooth(false);
            }
        }
    }
    inline static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures_;
    inline static std::map<std::string, sf::Font> fonts_;
    inline static std::map<std::string, std::vector<sf::Uint8>> fontsData_;
};
inline void ResourceManager::preload() {
    ResourceManager::getFont("fonts/font4.ttf");  //for flash screen
    ResourceManager::getFont("fonts/font0.otf");
    std::thread([] {
        namespace fs = std::filesystem;
        const std::string assetRoot = "assets";
        for (const auto& entry : fs::recursive_directory_iterator(assetRoot)) {
            if (!entry.is_regular_file()) continue;
            const auto& path = entry.path();
            const std::string ext = path.extension().string();
            const std::string relPath =
                fs::relative(path, assetRoot).generic_string();
            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") {
                getTexture(relPath);
            }
            else if (ext == ".ttf" || ext == ".otf") {
                getFont(relPath);
            }
        }
        hasPreLoaded_ = true;
    }).detach();
}
#endif // UNDEROCEAN_RESOURCEMANAGER_H
