#include "Game.h"
#include "Constants.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

Game::Game() : attemptsLeft(MAX_ATTEMPTS), isRunning(true) {
    InitializeGame();
}
void Game::InitializeGame() {
    srand(static_cast<unsigned int>(time(0)));
    secretWord = WORD_LIST[rand() % (sizeof(WORD_LIST) / sizeof(WORD_LIST[0]))];
    guessedWord = std::string(secretWord.length(), '_');
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
    std::cout << HANGMAN_ART[MAX_ATTEMPTS - attemptsLeft] << std::endl;

    std::cout << "Welcome to Hangman!" << std::endl;
    std::cout << "Attempts left: " << attemptsLeft << std::endl;
    std::cout << "Word: ";
    for (char c : guessedWord) {
        std::cout << c << " ";
    }
    std::cout << std::endl;
}

void Game::HandleInput() {
    char guess;
    std::cout << "Enter your guess (a-z): ";
    std::cin >> guess;
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
        std::cout << "Congratulations! You guessed the word: " << secretWord << std::endl;
    } else if (attemptsLeft <= 0) {
        std::cout << "Game over! The word was: " << secretWord << std::endl;
    }
}

bool Game::IsGameOver() {
    return (guessedWord == secretWord) || (attemptsLeft <= 0);
}
