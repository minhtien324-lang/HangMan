#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include <string>
#include <algorithm>
#include "fileReader.h"
#include "themeManager.h"
#include <SDL_ttf.h>
class Game {
public:
    Game();
    ~Game();
    void run();
    TTF_Font* font;
    TTF_Font* fontSmall;
private:
    enum GameState { STATE_THEME_SELECT, STATE_DIFFICULTY_SELECT, STATE_PLAYING, STATE_CONFIRM_EXIT };
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
    ThemeManager::Theme selectedTheme;
    int maxAttempts;
    int themeMenuIndex; // 0-4: Themes, 5: Quit
    int difficultyMenuIndex; // 0: Easy, 1: Medium, 2: Hard, 3: Back
    ThemeManager themeManager;
    SDL_Texture* backgroundTexture;
    std::vector<SDL_Rect> themeButtonRects;
    std::vector<SDL_Rect> difficultyButtonRects;
    std::string guessMessage;
    void loadTextures();
    void processInput(char guess);
    void renderGame();
    bool isGameOver();
    void renderThemeMenu();
    void renderDifficultyMenu();
    void renderConfirmExit();
    void renderResult();
    void startNewGame();
    void renderButton(const char* text, int x, int y, bool selected, int w, int h);
};

#endif
