#include "audioManager.h"

#include <SDL_mixer.h>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace {
    constexpr int DEFAULT_FREQUENCY = 44100;
    constexpr Uint16 DEFAULT_FORMAT = MIX_DEFAULT_FORMAT;
    constexpr int DEFAULT_CHANNELS = 2;
    constexpr int DEFAULT_CHUNK_SIZE = 2048;

    const char* CLICK_FILE = "click.mp3";
    const char* BACKGROUND_FILE = "background.mp3";
    const char* WIN_FILE = "win.mp3";
    const char* LOSE_FILE = "lose.mp3";
}

AudioManager::AudioManager()
    : backgroundMusic(nullptr),
      winMusic(nullptr),
      loseMusic(nullptr),
      letterClickSound(nullptr),
      musicVolume(MIX_MAX_VOLUME / 2),
      effectVolume(MIX_MAX_VOLUME / 2),
      initialized(false),
      assetsLoaded(false) {
}

AudioManager::~AudioManager() {
    freeAudio();
    if (initialized) {
        Mix_CloseAudio();
        Mix_Quit();
    }
}

bool AudioManager::initialize() {
    if (initialized) {
        return true;
    }

    int mixFlags = MIX_INIT_MP3 | MIX_INIT_OGG;
    int result = Mix_Init(mixFlags);
    if ((result & mixFlags) != mixFlags) {
        std::cerr << "Warning: SDL_mixer failed to initialize requested codecs: " << Mix_GetError() << std::endl;
    }

    if (Mix_OpenAudio(DEFAULT_FREQUENCY, DEFAULT_FORMAT, DEFAULT_CHANNELS, DEFAULT_CHUNK_SIZE) < 0) {
        std::cerr << "SDL_mixer could not open audio: " << Mix_GetError() << std::endl;
        return false;
    }

    Mix_AllocateChannels(16);
    initialized = true;
    return true;
}

bool AudioManager::loadFromDirectory(const std::string& basePath) {
    if (!initialized) {
        std::cerr << "AudioManager::loadFromDirectory called before initialize()." << std::endl;
        return false;
    }

    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path root(basePath);
    if (!fs::exists(root, ec)) {
        return false;
    }

    freeAudio();

    auto backgroundPath = root / BACKGROUND_FILE;
    auto clickPath = root / CLICK_FILE;
    auto winPath = root / WIN_FILE;
    auto losePath = root / LOSE_FILE;

    if (fs::exists(backgroundPath, ec)) {
        backgroundMusic = Mix_LoadMUS(backgroundPath.string().c_str());
        if (!backgroundMusic) {
            std::cerr << "Failed to load background music '" << backgroundPath << "': " << Mix_GetError() << std::endl;
        }
    }

    if (fs::exists(winPath, ec)) {
        winMusic = Mix_LoadMUS(winPath.string().c_str());
        if (!winMusic) {
            std::cerr << "Failed to load win music '" << winPath << "': " << Mix_GetError() << std::endl;
        }
    }

    if (fs::exists(losePath, ec)) {
        loseMusic = Mix_LoadMUS(losePath.string().c_str());
        if (!loseMusic) {
            std::cerr << "Failed to load lose music '" << losePath << "': " << Mix_GetError() << std::endl;
        }
    }

    if (fs::exists(clickPath, ec)) {
        letterClickSound = Mix_LoadWAV(clickPath.string().c_str());
        if (!letterClickSound) {
            std::cerr << "Failed to load click sound '" << clickPath << "': " << Mix_GetError() << std::endl;
        } else {
            Mix_VolumeChunk(letterClickSound, effectVolume);
        }
    }

    Mix_VolumeMusic(musicVolume);
    assetsLoaded = backgroundMusic || letterClickSound || winMusic || loseMusic;
    return assetsLoaded;
}

void AudioManager::setMusicVolume(int volume) {
    musicVolume = std::clamp(volume, 0, MIX_MAX_VOLUME);
    Mix_VolumeMusic(musicVolume);
}

void AudioManager::setEffectVolume(int volume) {
    effectVolume = std::clamp(volume, 0, MIX_MAX_VOLUME);
    if (letterClickSound) {
        Mix_VolumeChunk(letterClickSound, effectVolume);
    }
}

void AudioManager::playLetterClick() {
    if (letterClickSound) {
        Mix_PlayChannel(-1, letterClickSound, 0);
    }
}

void AudioManager::playBackgroundMusic(bool loop) {
    if (!backgroundMusic) {
        return;
    }
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
    Mix_PlayMusic(backgroundMusic, loop ? -1 : 0);
    Mix_VolumeMusic(musicVolume);
}

void AudioManager::stopBackgroundMusic() {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
}

void AudioManager::playWinMusic() {
    if (!winMusic) {
        return;
    }
    stopBackgroundMusic();
    Mix_PlayMusic(winMusic, 0);
    Mix_VolumeMusic(musicVolume);
}

void AudioManager::playLoseMusic() {
    if (!loseMusic) {
        return;
    }
    stopBackgroundMusic();
    Mix_PlayMusic(loseMusic, 0);
    Mix_VolumeMusic(musicVolume);
}

void AudioManager::freeAudio() {
    if (letterClickSound) {
        Mix_FreeChunk(letterClickSound);
        letterClickSound = nullptr;
    }
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }
    if (winMusic) {
        Mix_FreeMusic(winMusic);
        winMusic = nullptr;
    }
    if (loseMusic) {
        Mix_FreeMusic(loseMusic);
        loseMusic = nullptr;
    }
    assetsLoaded = false;
}

