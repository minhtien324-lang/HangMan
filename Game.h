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
    enum GameState { STATE_MENU, STATE_PLAYING, STATE_CONFIRM_EXIT };
    enum Difficulty { DIFF_EASY, DIFF_MEDIUM, DIFF_HARD };
    const int MAX_ATTEMPTS = 6;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_Texture*> hangmanImages;
    std::string secretWord;
    std::string guessedWord;
    std::vector<char> guessedLetters;
    int wrongGuesses;
    bool isRunning;
    GameState state;
    Difficulty difficulty;
    int maxAttempts;
    int menuIndex; // 0: Easy, 1: Medium, 2: Hard, 3: Quit
    void loadImages();
    void processInput(char guess);
    void renderGame();
    bool isGameOver();
    void renderMenu();
    void renderConfirmExit();
    void renderResult();
    void startNewGame();
};

#endif
