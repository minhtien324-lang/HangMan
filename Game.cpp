#include "Game.h"
#include "Constants.h"
#include "fileReader.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
using namespace std;
Game::Game() : attemptsLeft(MAX_ATTEMPTS), isRunning(true) {
      try {
        vector<string> words = FileReader::ReadWords(WORDS_FILE_PATH);
        if (words.empty()) {
            throw runtime_error("Words file is empty");
        }

        srand(static_cast<unsigned int>(time(0)));
        secretWord = words[rand() % words.size()];
        guessedWord = string(secretWord.length(), '_');
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        // back neu ko doc dc file
        secretWord = "hangman";
        guessedWord = string(secretWord.length(), '_');
    }
}
void Game::InitializeGame() {
    srand(static_cast<unsigned int>(time(0)));
    secretWord = WORDS_FILE_PATH[rand() % (sizeof(WORDS_FILE_PATH) / sizeof(WORDS_FILE_PATH[0]))];
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
