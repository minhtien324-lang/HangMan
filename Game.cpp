#include "Game.h"
#include "textureManager.h"
#include <SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>
Game::Game() : window(nullptr), renderer(nullptr), wrongGuesses(0), isRunning(true) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Hangman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 720,0);

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;
        exit(1);
    }
    font = TTF_OpenFont("roboto.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        exit(1);
    }

   try {
        std::vector<std::string> words = FileReader::ReadWords("words.txt");
        srand(time(0));
        secretWord = words[rand() % words.size()];
    } catch (const std::exception& e) {
        std::cerr << "Lỗi: " << e.what() << std::endl;
        secretWord = "hangman";
    }

    guessedWord = std::string(secretWord.length(), '_');
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    loadImages();
}

void Game::loadImages() {
    for(int i = 0; i < 7; i++) {
        std::string path = "hangman_" + std::to_string(i) + ".png";
        SDL_Texture* texture = TextureManager::LoadTexture(path, renderer);
        hangmanImages.push_back(texture);
    }
}

void Game::processInput(char guess) {
    guess = tolower(guess);
    if (std::find(guessedLetters.begin(), guessedLetters.end(), guess) != guessedLetters.end()) {
        std::cout << "Ban da doan chu '" << guess << "' roi!\n";
        return;
    }
    guessedLetters.push_back(guess);
    bool correct = false;
    for (size_t i = 0; i < secretWord.length(); i++) {
        if (secretWord[i] == guess) {
            guessedWord[i] = guess;
            correct = true;
        }
    }
    if (!correct) wrongGuesses++;
}

void Game::renderGame() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, hangmanImages[wrongGuesses], NULL, NULL);
    SDL_Color textColor = {0,0,0,0};
    std::string wordDisplay;
    for (char c : guessedWord) {
        wordDisplay += (c == '_') ? "_ " : std::string(1, c) + " ";
    }

    SDL_Surface* wordSurface = TTF_RenderText_Solid(font, wordDisplay.c_str(), textColor);
    SDL_Texture* wordTexture = SDL_CreateTextureFromSurface(renderer, wordSurface);
    SDL_Rect wordRect = {50, 450, wordSurface->w, wordSurface->h};
    SDL_RenderCopy(renderer, wordTexture, NULL, &wordRect);


    std::string guessedStr = "Da doan: ";
    for (char c : guessedLetters) {
        guessedStr += c;
        guessedStr += ' ';
    }

    SDL_Surface* guessedSurface = TTF_RenderText_Solid(font, guessedStr.c_str(), textColor);
    SDL_Texture* guessedTexture = SDL_CreateTextureFromSurface(renderer, guessedSurface);
    SDL_Rect guessedRect = {50, 480, guessedSurface->w, guessedSurface->h};
    SDL_RenderCopy(renderer, guessedTexture, NULL, &guessedRect);


    SDL_FreeSurface(wordSurface);
    SDL_FreeSurface(guessedSurface);
    SDL_DestroyTexture(wordTexture);
    SDL_DestroyTexture(guessedTexture);

    SDL_RenderPresent(renderer);
}

bool Game::isGameOver() {
    return guessedWord == secretWord || wrongGuesses >= 6;
}

void Game::run() {
    SDL_Event event;

    while(isRunning && !isGameOver()) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) isRunning = false;

            if(event.type == SDL_KEYDOWN) {
                char guess = event.key.keysym.sym;
                if(isalpha(guess)) {
                    processInput(guess);
                }
            }
        }
        renderGame();
        SDL_Delay(16);
    }
    if(guessedWord == secretWord) {
        std::cout << "Chuc mung! Ban da thang!\n";
    } else {
        std::cout << "Game over! Tu la: " << secretWord << "\n";
    }
    SDL_Delay(2000);
}

Game::~Game() {
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
    for(auto texture : hangmanImages) SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
