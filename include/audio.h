#ifndef AUDIO__H
#define AUDIO__H

#include <SDL_mixer.h>
#include <string>
#include <map>

struct Audio {
    std::map<std::string, Mix_Chunk*> soundEffects;
    Mix_Music* backgroundMusic = nullptr;

    void init(); // Khởi tạo audio
    void loadSound(const std::string& id, const std::string& filePath); // Load hiệu ứng
    void playSound(const std::string& id); // Phát hiệu ứng
    void loadMusic(const std::string& filePath); // Load nhạc nền
    void playMusic(); // Phát nhạc nền
    void stopMusic(); // Dừng nhạc nền
    void cleanup(); // Giải phóng tài nguyên
    Mix_Chunk* getChunk(const std::string& name);

};

#endif
