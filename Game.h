#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include <string>
#include <algorithm>
#include "fileReader.h"
#include <SDL_ttf.h>
class Game {
public:
    Game();
    ~Game();
    void run();
    TTF_Font* font;
private:
    const int MAX_ATTEMPTS = 6;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_Texture*> hangmanImages;
    std::string secretWord;
    std::string guessedWord;
    std::vector<char> guessedLetters;
    int wrongGuesses;
    bool isRunning;
    void loadImages();
    void processInput(char guess);
    void renderGame();
    bool isGameOver();
};

#endif
