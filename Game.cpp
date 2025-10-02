#include "Game.h"
#include "textureManager.h"
#include <SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <sstream>
Game::Game() : window(nullptr), renderer(nullptr), wrongGuesses(0), isRunning(true) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Hangman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,0);

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;
        exit(1);
    }
    font = TTF_OpenFont("roboto.ttf", 12);
    if (!font) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        exit(1);
    }

    // Initial state: show menu
    state = STATE_MENU;
    difficulty = DIFF_EASY;
    maxAttempts = MAX_ATTEMPTS;
    menuIndex = 0;

    // We will start game after user picks difficulty
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

void Game::renderMenu() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {0,0,0,0};
    SDL_Color highlightColor = {200, 0, 0, 0};
    const char* title = "Chon do kho";
    const char* opt1 = "1) De (tu ngan)";
    const char* opt2 = "2) Vua (tu vua)";
    const char* opt3 = "3) Kho (tu dai)";
    const char* opt4 = "0) Thoat game";

    SDL_Surface* s1 = TTF_RenderText_Solid(font, title, textColor);
    SDL_Surface* s2 = TTF_RenderText_Solid(font, opt1, (menuIndex==0)?highlightColor:textColor);
    SDL_Surface* s3 = TTF_RenderText_Solid(font, opt2, (menuIndex==1)?highlightColor:textColor);
    SDL_Surface* s4 = TTF_RenderText_Solid(font, opt3, (menuIndex==2)?highlightColor:textColor);
    SDL_Surface* s5 = TTF_RenderText_Solid(font, opt4, (menuIndex==3)?highlightColor:textColor);

    SDL_Texture* t1 = SDL_CreateTextureFromSurface(renderer, s1);
    SDL_Texture* t2 = SDL_CreateTextureFromSurface(renderer, s2);
    SDL_Texture* t3 = SDL_CreateTextureFromSurface(renderer, s3);
    SDL_Texture* t4 = SDL_CreateTextureFromSurface(renderer, s4);
    SDL_Texture* t5 = SDL_CreateTextureFromSurface(renderer, s5);

    SDL_Rect r1 = {300, 150, s1->w, s1->h};
    SDL_Rect r2 = {300, 220, s2->w, s2->h};
    SDL_Rect r3 = {300, 260, s3->w, s3->h};
    SDL_Rect r4 = {300, 300, s4->w, s4->h};
    SDL_Rect r5 = {300, 340, s5->w, s5->h};

    SDL_RenderCopy(renderer, t1, NULL, &r1);
    SDL_RenderCopy(renderer, t2, NULL, &r2);
    SDL_RenderCopy(renderer, t3, NULL, &r3);
    SDL_RenderCopy(renderer, t4, NULL, &r4);
    SDL_RenderCopy(renderer, t5, NULL, &r5);

    SDL_FreeSurface(s1); SDL_FreeSurface(s2); SDL_FreeSurface(s3); SDL_FreeSurface(s4); SDL_FreeSurface(s5);
    SDL_DestroyTexture(t1); SDL_DestroyTexture(t2); SDL_DestroyTexture(t3); SDL_DestroyTexture(t4); SDL_DestroyTexture(t5);

    SDL_RenderPresent(renderer);
}

void Game::renderConfirmExit() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {0,0,0,0};
    const char* title = "Ban co chac muon thoat? (Y/N)";

    SDL_Surface* s1 = TTF_RenderText_Solid(font, title, textColor);
    SDL_Texture* t1 = SDL_CreateTextureFromSurface(renderer, s1);
    SDL_Rect r1 = {220, 240, s1->w, s1->h};
    SDL_RenderCopy(renderer, t1, NULL, &r1);

    SDL_FreeSurface(s1);
    SDL_DestroyTexture(t1);
    SDL_RenderPresent(renderer);
}

void Game::startNewGame() {
    std::vector<std::string> words;
    try {
        words = FileReader::ReadWords("words.txt");
    } catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << std::endl;
    }

    // Filter words by difficulty
    std::vector<std::string> filtered;
    for(const auto& w : words) {
        size_t len = w.size();
        if(difficulty == DIFF_EASY && len <= 4) filtered.push_back(w);
        else if(difficulty == DIFF_MEDIUM && len >= 5 && len <= 7) filtered.push_back(w);
        else if(difficulty == DIFF_HARD && len >= 8) filtered.push_back(w);
    }

    if(filtered.empty()) filtered = words; // fallback

    srand((unsigned int)time(0));
    if(!filtered.empty()) {
        secretWord = filtered[rand() % filtered.size()];
    } else {
        secretWord = "hangman";
    }

    guessedLetters.clear();
    wrongGuesses = 0;
    guessedWord = std::string(secretWord.length(), '_');

    // attempts per difficulty
    if(difficulty == DIFF_EASY) maxAttempts = 6;
    else if(difficulty == DIFF_MEDIUM) maxAttempts = 6;
    else maxAttempts = 6;
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

void Game::renderResult() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Show final hangman image as background context
    if(wrongGuesses >= 0 && wrongGuesses < (int)hangmanImages.size() && hangmanImages[wrongGuesses]) {
        SDL_RenderCopy(renderer, hangmanImages[wrongGuesses], NULL, NULL);
    }

    SDL_Color textColor = {0,0,0,0};
    std::string title = (guessedWord == secretWord) ? "Chuc mung! Ban da thang!" : "Game Over";
    std::string reveal = std::string("Tu la: ") + secretWord;

    SDL_Surface* s1 = TTF_RenderText_Solid(font, title.c_str(), textColor);
    SDL_Surface* s2 = TTF_RenderText_Solid(font, reveal.c_str(), textColor);

    SDL_Texture* t1 = SDL_CreateTextureFromSurface(renderer, s1);
    SDL_Texture* t2 = SDL_CreateTextureFromSurface(renderer, s2);

    SDL_Rect r1 = {250, 200, s1->w, s1->h};
    SDL_Rect r2 = {250, 240, s2->w, s2->h};

    SDL_RenderCopy(renderer, t1, NULL, &r1);
    SDL_RenderCopy(renderer, t2, NULL, &r2);

    SDL_FreeSurface(s1); SDL_FreeSurface(s2);
    SDL_DestroyTexture(t1); SDL_DestroyTexture(t2);

    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
}

bool Game::isGameOver() {
    return guessedWord == secretWord || wrongGuesses >= maxAttempts;
}

void Game::run() {
    SDL_Event event;
    while(isRunning) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) isRunning = false;

            if(state == STATE_MENU) {
                if(event.type == SDL_KEYDOWN) {
                    SDL_Keycode key = event.key.keysym.sym;
                    if(key == SDLK_1) { difficulty = DIFF_EASY; startNewGame(); state = STATE_PLAYING; }
                    else if(key == SDLK_2) { difficulty = DIFF_MEDIUM; startNewGame(); state = STATE_PLAYING; }
                    else if(key == SDLK_3) { difficulty = DIFF_HARD; startNewGame(); state = STATE_PLAYING; }
                    else if(key == SDLK_0 || key == SDLK_ESCAPE) { state = STATE_CONFIRM_EXIT; }
                    else if(key == SDLK_UP) { if(menuIndex > 0) menuIndex--; }
                    else if(key == SDLK_DOWN) { if(menuIndex < 3) menuIndex++; }
                    else if(key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                        if(menuIndex == 0) { difficulty = DIFF_EASY; startNewGame(); state = STATE_PLAYING; }
                        else if(menuIndex == 1) { difficulty = DIFF_MEDIUM; startNewGame(); state = STATE_PLAYING; }
                        else if(menuIndex == 2) { difficulty = DIFF_HARD; startNewGame(); state = STATE_PLAYING; }
                        else if(menuIndex == 3) { state = STATE_CONFIRM_EXIT; }
                    }
                }
            } else if(state == STATE_CONFIRM_EXIT) {
                if(event.type == SDL_KEYDOWN) {
                    SDL_Keycode key = event.key.keysym.sym;
                    if(key == SDLK_y) { isRunning = false; }
                    else if(key == SDLK_n || key == SDLK_ESCAPE) { state = STATE_MENU; }
                }
            } else if(state == STATE_PLAYING && !isGameOver()) {
                if(event.type == SDL_KEYDOWN) {
                    char guess = (char)event.key.keysym.sym;
                    if(isalpha((unsigned char)guess)) {
                        processInput(guess);
                    }
                }
            }
        }

        if(state == STATE_MENU) {
            renderMenu();
        } else if(state == STATE_CONFIRM_EXIT) {
            renderConfirmExit();
        } else {
            renderGame();
            if(isGameOver()) {
                renderResult();
                state = STATE_MENU; // Return to menu after a short pause
            }
        }
        SDL_Delay(16);
    }
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
