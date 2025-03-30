#include "audio.h"
#include <iostream>

void Audio::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio lỗi: " << Mix_GetError() << std::endl;
    }
}

void Audio::loadSound(const std::string& id, const std::string& filePath) {
    Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());
    if (!chunk) {
        std::cerr << "Không load được hiệu ứng " << filePath << ": " << Mix_GetError() << std::endl;
    } else {
        soundEffects[id] = chunk;
    }
}

void Audio::playSound(const std::string& id) {
    if (soundEffects.count(id)) {
        Mix_PlayChannel(-1, soundEffects[id], 0);
    }
}

void Audio::loadMusic(const std::string& filePath) {
    backgroundMusic = Mix_LoadMUS(filePath.c_str());
    if (!backgroundMusic) {
        std::cerr << "Không load được nhạc nền: " << Mix_GetError() << std::endl;
    }
}

void Audio::playMusic() {
    if (backgroundMusic) {
        Mix_PlayMusic(backgroundMusic, -1); // Lặp vô hạn
    }
}

void Audio::stopMusic() {
    Mix_HaltMusic();
}

void Audio::cleanup() {
    for (auto& pair : soundEffects) {
        Mix_FreeChunk(pair.second);
    }
    soundEffects.clear();

    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }

    Mix_CloseAudio();
}
Mix_Chunk* Audio::getChunk(const std::string& name) {
    if (soundEffects.count(name)) return soundEffects[name];
    return nullptr;
}
