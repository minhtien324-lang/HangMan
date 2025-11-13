#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include "fileReader.h"
#include "themeManager.h"
#include "audioManager.h"
#include <SDL_ttf.h>
class Game {
public:
    Game();
    ~Game();
    void run();
    TTF_Font* fontLarge;
    TTF_Font* fontMedium;
    TTF_Font* fontSmall;
private:
    enum GameState { STATE_THEME_SELECT, STATE_DIFFICULTY_SELECT, STATE_PLAYING, STATE_CONFIRM_EXIT };
    enum Difficulty { DIFF_EASY, DIFF_MEDIUM, DIFF_HARD };
    const int MAX_ATTEMPTS = 6;
    SDL_Window* window;
    SDL_Renderer* renderer;
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
    AudioManager audioManager;
    bool audioLoaded;
    std::vector<SDL_Rect> themeButtonRects;
    std::vector<SDL_Rect> difficultyButtonRects;
    struct LetterButton {
        SDL_Rect rect;
        std::string label;
        bool disabled;
    };
    std::array<LetterButton, 26> letterButtons;
    SDL_Rect buttonNew;
    SDL_Rect buttonRestart;
    SDL_Rect buttonQuit;
    bool showingResult;
    bool lastWin;
    int playedGames;
    int wonGames;
    std::string displayWord;
    void processInput(char guess);
    void renderGame();
    bool isGameOver();
    void renderThemeMenu();
    void renderDifficultyMenu();
    void renderConfirmExit();
    void startNewGame();
    void resetRound();
    void initializeLetterButtons();
    void resetLetterButtons();
    void setLetterButtonState(char letter, bool disabled);
    bool handleLetterClick(int x, int y);
    void renderButton(const std::string& text, const SDL_Rect& rect, bool selected, TTF_Font* font);
    void drawText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font);
    void drawTextCentered(const std::string& text, const SDL_Rect& rect, SDL_Color color, TTF_Font* font);
    void drawRect(const SDL_Rect& rect, SDL_Color fill, SDL_Color border);
    void drawScaffold();
    void drawHangedMan();
    std::string buildDisplayedWord() const;
};

#endif
