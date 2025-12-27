//
// Created by 15201 on 12/20/2025.
//

#ifndef UNDEROCEAN_AUDIOMANAGER_H
#define UNDEROCEAN_AUDIOMANAGER_H
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

class AudioManager {
public:
    static AudioManager& getInstance();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    void playMusic(const std::string& path, bool loop = true, float volume = 100.0f);
    void stopMusic();
    void setMusicVolume(float volume);
    void playSound(const std::string& path, float volume = 100.0f);
    void setSoundVolume(float volume) { masterSoundVolume_ = volume; }
    void cleanupSounds();
private:
    AudioManager() = default;
    ~AudioManager();
    sf::Music music_;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers_;
    std::vector<std::unique_ptr<sf::Sound>> activeSounds_;
    float masterSoundVolume_ = 100.0f;
    std::string currentMusicPath_;
};
#endif //UNDEROCEAN_AUDIOMANAGER_H