
#include "client/common/AudioManager.h"

#include <iostream>
#include <algorithm>
AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}
AudioManager::~AudioManager() {
    activeSounds_.clear();
    soundBuffers_.clear();
}
void AudioManager::playMusic(const std::string& path, bool loop, float volume) {
    if (!music_.openFromFile("assets/" + path)) {
        std::cerr << "Failed to load music: " << path << std::endl;
        return;
    }
    music_.setLoop(loop);
    music_.setVolume(volume);
    music_.play();
}
void AudioManager::stopMusic() {
    music_.stop();
}
void AudioManager::setMusicVolume(float volume) {
    music_.setVolume(std::clamp(volume, 0.0f, 100.0f));
}
void AudioManager::playSound(const std::string& rawPath, float volume) {
    std::string path = "assets/" + rawPath;
    auto it = soundBuffers_.find(path);
    if (it == soundBuffers_.end()) {
        auto buffer = std::make_unique<sf::SoundBuffer>();
        if (!buffer->loadFromFile(path)) {
            std::cerr << "Failed to load sound: " << path << std::endl;
            return;
        }
        it = soundBuffers_.emplace(path, std::move(buffer)).first;
    }
    auto sound = std::make_unique<sf::Sound>();
    sound->setBuffer(*it->second);
    sound->setVolume(std::clamp(volume * masterSoundVolume_ / 100.0f, 0.0f, 100.0f));
    sound->play();
    activeSounds_.push_back(std::move(sound));
}
void AudioManager::cleanupSounds() {
    std::erase_if(activeSounds_,
                  [](const std::unique_ptr<sf::Sound>& s) {
                      return s->getStatus() != sf::Sound::Playing;
                  });
}