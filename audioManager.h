#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <string>

struct Mix_Music;
typedef Mix_Music Mix_Music;
struct Mix_Chunk;

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool initialize();
    bool loadFromDirectory(const std::string& basePath);

    void setMusicVolume(int volume);   // 0-128
    void setEffectVolume(int volume);  // 0-128

    void playLetterClick();
    void playBackgroundMusic(bool loop = true);
    void stopBackgroundMusic();
    void playWinMusic();
    void playLoseMusic();

    bool isInitialized() const { return initialized; }

private:
    void freeAudio();

    Mix_Music* backgroundMusic;
    Mix_Music* winMusic;
    Mix_Music* loseMusic;
    Mix_Chunk* letterClickSound;
    int musicVolume;
    int effectVolume;
    bool initialized;
    bool assetsLoaded;
};

#endif


