#include "Game.h"
#include "Constants.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
using namespace std;
Game::Game() : attemptsLeft(MAX_ATTEMPTS), isRunning(true) {
    InitializeGame();
}
void Game::InitializeGame() {
    srand(static_cast<unsigned int>(time(0)));
    secretWord = WORD_LIST[rand() % (sizeof(WORD_LIST) / sizeof(WORD_LIST[0]))];
    guessedWord = string(secretWord.length(), '_');
}
void Game::Run() {
    while (isRunning) {
        RenderGame();
        HandleInput();
        UpdateGame();
        if (IsGameOver()) {
            RenderGame();
            isRunning = false;
        }
    }
}

void Game::RenderGame() {
    system("cls");
    cout << HANGMAN_ART[MAX_ATTEMPTS - attemptsLeft] << endl;

    cout << "Welcome to Hangman!" <<endl;
    cout << "Attempts left: " << attemptsLeft << endl;
    cout << "Word: ";
    for (char c : guessedWord) {
        cout << c << " ";
    }
    cout << endl;
}

void Game::HandleInput() {
    char guess;
    cout << "Enter your guess (a-z): ";
    cin >> guess;
    guess = tolower(guess);

    bool correctGuess = false;
    for (size_t i = 0; i < secretWord.length(); ++i) {
        if (secretWord[i] == guess) {
            guessedWord[i] = guess;
            correctGuess = true;
        }
    }

    if (!correctGuess) {
        --attemptsLeft;
    }
}

void Game::UpdateGame() {
    if (guessedWord == secretWord) {
        cout << "Congratulations! You guessed the word: " << secretWord <<endl;
    } else if (attemptsLeft <= 0) {
        cout << "Game over! The word was: " << secretWord <<endl;
    }
}

bool Game::IsGameOver() {
    return (guessedWord == secretWord) || (attemptsLeft <= 0);
}
