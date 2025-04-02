#ifndef GAME_H
#define GAME_H

#include <string>
using namespace std;
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

    string secretWord;
    string guessedWord;
    int attemptsLeft;
    bool isRunning;
};

#endif
