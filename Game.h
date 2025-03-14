#ifndef GAME_H
#define GAME_H

#include <string>

class Game {
public:
    Game();
    void Run();

private:
    void InitializeGame();
    void RenderGame();
    void HandleInput();
    void UpdateGame();
    bool IsGameOver();

    std::string secretWord;
    std::string guessedWord;
    int attemptsLeft;
    bool isRunning;
};

#endif
