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
    font = TTF_OpenFont("roboto.ttf", 20);
    fontSmall = TTF_OpenFont("roboto.ttf", 36); // tạo thêm biến này cho menu ngoài
    if (!font) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        exit(1);
    }

    //
    state = STATE_THEME_SELECT;
    difficulty = DIFF_EASY;
    selectedTheme = ThemeManager::THEME_ANIMALS;
    maxAttempts = MAX_ATTEMPTS;
    themeMenuIndex = 0;
    difficultyMenuIndex = 0;


    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    loadTextures();
}

void Game::loadTextures() {
    backgroundTexture = TextureManager::LoadTexture("background.png", renderer);
    for(int i = 0; i < 7; i++) {
        std::string path = "hangman_" + std::to_string(i) + ".png";
        SDL_Texture* texture = TextureManager::LoadTexture(path, renderer);
        hangmanImages.push_back(texture);
    }
}

void Game::renderButton(const char* text, int x, int y, bool selected, int w, int h) {
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int textWidth = surface->w;
    int textHeight = surface->h;
    SDL_Rect buttonRect = { x, y, w, h };
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255);
    }
    SDL_RenderFillRect(renderer, &buttonRect);
    // Căn giữa text trong ô
    SDL_Rect textRect = { x + (w - textWidth) / 2, y + (h - textHeight) / 2, textWidth, textHeight };
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::renderThemeMenu() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    if(backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }
    SDL_Color textColor = {0,0,0,255};
    const char* title = "Chon Chu De";
    std::vector<std::string> themeNames = themeManager.getThemeNames();
    std::vector<const char*> options;
    for (const auto& name : themeNames) options.push_back(name.c_str());
    const char* quit = "Thoat Game";
    options.push_back(quit);

    SDL_Surface* s1 = TTF_RenderText_Solid(fontSmall, title, textColor);
    SDL_Texture* t1 = SDL_CreateTextureFromSurface(renderer, s1);
    SDL_Rect r1 = {50, 40, s1->w, s1->h};
    SDL_RenderCopy(renderer, t1, NULL, &r1);
    SDL_FreeSurface(s1);
    SDL_DestroyTexture(t1);

    int themeCount = themeManager.getThemeCount();
    int columns = 3;
    int rows = 2; // luôn vẽ tối đa 6 ô kể cả ít chủ đề hơn
    int buttonW = 220, buttonH = 160;
    int marginX = 45;
    int marginY = 30;
    int startX = 50;
    int startY = 110;
    themeButtonRects.clear();
    int idx = 0;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            int x = startX + col * (buttonW + marginX);
            int y = startY + row * (buttonH + marginY);
            SDL_Rect rect = {x, y, buttonW, buttonH};
            themeButtonRects.push_back(rect);
            if(idx < themeCount)
                renderButton(options[idx], rect.x, rect.y, themeMenuIndex == idx, buttonW, buttonH);
            ++idx;
        }
    }
    // Nút quit siêu to ở dưới góc trái
    int quitBtnX = startX;
    int quitBtnY = startY + 2 * (buttonH + marginY);
    SDL_Rect quitRect = {quitBtnX, quitBtnY, buttonW, buttonH};
    themeButtonRects.push_back(quitRect);
    renderButton(quit, quitRect.x, quitRect.y, (themeMenuIndex == (int)options.size()-1), buttonW, buttonH);
    SDL_RenderPresent(renderer);
}

void Game::renderDifficultyMenu() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    if(backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }

    SDL_Color textColor = {0,0,0,0};
    const char* title = "Chon do kho";
    const char* opt1 = "De (tu ngan)";
    const char* opt2 = "Vua (tu vua)";
    const char* opt3 = "Kho (tu dai)";
    const char* opt4 = "Quay lai";

    SDL_Surface* s1 = TTF_RenderText_Solid(fontSmall, title, textColor);
    SDL_Texture* t1 = SDL_CreateTextureFromSurface(renderer, s1);
    SDL_Rect r1 = {300, 100, s1->w, s1->h};
    SDL_RenderCopy(renderer, t1, NULL, &r1);
    SDL_FreeSurface(s1);
    SDL_DestroyTexture(t1);

    difficultyButtonRects.clear();
    SDL_Surface* s_opt1 = TTF_RenderText_Solid(font, opt1, {255,255,255,255});
    SDL_Surface* s_opt2 = TTF_RenderText_Solid(font, opt2, {255,255,255,255});
    SDL_Surface* s_opt3 = TTF_RenderText_Solid(font, opt3, {255,255,255,255});
    SDL_Surface* s_opt4 = TTF_RenderText_Solid(font, opt4, {255,255,255,255});

    difficultyButtonRects.push_back({300, 150, s_opt1->w + 20, s_opt1->h + 10});
    difficultyButtonRects.push_back({300, 210, s_opt2->w + 20, s_opt2->h + 10});
    difficultyButtonRects.push_back({300, 270, s_opt3->w + 20, s_opt3->h + 10});
    difficultyButtonRects.push_back({300, 330, s_opt4->w + 20, s_opt4->h + 10});

    SDL_FreeSurface(s_opt1);
    SDL_FreeSurface(s_opt2);
    SDL_FreeSurface(s_opt3);
    SDL_FreeSurface(s_opt4);

    renderButton(opt1, difficultyButtonRects[0].x, difficultyButtonRects[0].y, difficultyMenuIndex == 0, difficultyButtonRects[0].w, difficultyButtonRects[0].h);
    renderButton(opt2, difficultyButtonRects[1].x, difficultyButtonRects[1].y, difficultyMenuIndex == 1, difficultyButtonRects[1].w, difficultyButtonRects[1].h);
    renderButton(opt3, difficultyButtonRects[2].x, difficultyButtonRects[2].y, difficultyMenuIndex == 2, difficultyButtonRects[2].w, difficultyButtonRects[2].h);
    renderButton(opt4, difficultyButtonRects[3].x, difficultyButtonRects[3].y, difficultyMenuIndex == 3, difficultyButtonRects[3].w, difficultyButtonRects[3].h);


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
    // Lấy từ theo chủ đề đã chọn
    std::vector<std::string> words = themeManager.getWordsByTheme(selectedTheme);

    if (words.empty()) {
        // Fallback nếu không có từ nào
        secretWord = "hangman";
    } else {
        // do kho
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
    }

    guessedLetters.clear();
    wrongGuesses = 0;
    guessedWord = std::string(secretWord.length(), '_');

    if(difficulty == DIFF_EASY) maxAttempts = 6;
    else if(difficulty == DIFF_MEDIUM) maxAttempts = 6;
    else maxAttempts = 6;
}

void Game::processInput(char guess) {
    guess = tolower(guess);
    if (std::find(guessedLetters.begin(), guessedLetters.end(), guess) != guessedLetters.end()) {
        guessMessage = std::string("Ban da doan chu : '") + guess + "' roi!";
        return;
    } else {
        guessMessage.clear();
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

    if (!guessMessage.empty()) {
        SDL_Surface* msgSurface = TTF_RenderText_Solid(font, guessMessage.c_str(), SDL_Color{255,0,0,0});
        SDL_Texture* msgTexture = SDL_CreateTextureFromSurface(renderer, msgSurface);
        SDL_Rect msgRect = {50, 520, msgSurface->w, msgSurface->h};
        SDL_RenderCopy(renderer, msgTexture, NULL, &msgRect);
        SDL_FreeSurface(msgSurface);
        SDL_DestroyTexture(msgTexture);
    }

    SDL_FreeSurface(wordSurface);
    SDL_FreeSurface(guessedSurface);
    SDL_DestroyTexture(wordTexture);
    SDL_DestroyTexture(guessedTexture);

    SDL_RenderPresent(renderer);
}

void Game::renderResult() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);


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

            if(state == STATE_THEME_SELECT) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    for (size_t i = 0; i < themeButtonRects.size(); ++i) {
                        if (x >= themeButtonRects[i].x && x <= themeButtonRects[i].x + themeButtonRects[i].w &&
                            y >= themeButtonRects[i].y && y <= themeButtonRects[i].y + themeButtonRects[i].h) {
                            if (i < themeManager.getThemeNames().size()) {
                                selectedTheme = (ThemeManager::Theme)i;
                                state = STATE_DIFFICULTY_SELECT;
                            } else {
                                state = STATE_CONFIRM_EXIT;
                            }
                        }
                    }
                }
                if(event.type == SDL_KEYDOWN) {
                    SDL_Keycode key = event.key.keysym.sym;
                    if(key == SDLK_UP) { if(themeMenuIndex > 0) themeMenuIndex--; }
                    else if(key == SDLK_DOWN) { if(themeMenuIndex < (int)themeManager.getThemeNames().size()) themeMenuIndex++; }
                    else if(key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                        if (themeMenuIndex < (int)themeManager.getThemeNames().size()) {
                            selectedTheme = (ThemeManager::Theme)themeMenuIndex;
                            state = STATE_DIFFICULTY_SELECT;
                        } else {
                            state = STATE_CONFIRM_EXIT;
                        }
                    } else if (key == SDLK_ESCAPE) {
                        state = STATE_CONFIRM_EXIT;
                    }
                }
            } else if(state == STATE_DIFFICULTY_SELECT) {
                 if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    if (x >= difficultyButtonRects[0].x && x <= difficultyButtonRects[0].x + difficultyButtonRects[0].w && y >= difficultyButtonRects[0].y && y <= difficultyButtonRects[0].y + difficultyButtonRects[0].h) {
                        difficulty = DIFF_EASY; startNewGame(); state = STATE_PLAYING;
                    } else if (x >= difficultyButtonRects[1].x && x <= difficultyButtonRects[1].x + difficultyButtonRects[1].w && y >= difficultyButtonRects[1].y && y <= difficultyButtonRects[1].y + difficultyButtonRects[1].h) {
                        difficulty = DIFF_MEDIUM; startNewGame(); state = STATE_PLAYING;
                    } else if (x >= difficultyButtonRects[2].x && x <= difficultyButtonRects[2].x + difficultyButtonRects[2].w && y >= difficultyButtonRects[2].y && y <= difficultyButtonRects[2].y + difficultyButtonRects[2].h) {
                        difficulty = DIFF_HARD; startNewGame(); state = STATE_PLAYING;
                    } else if (x >= difficultyButtonRects[3].x && x <= difficultyButtonRects[3].x + difficultyButtonRects[3].w && y >= difficultyButtonRects[3].y && y <= difficultyButtonRects[3].y + difficultyButtonRects[3].h) {
                        state = STATE_THEME_SELECT;
                    }
                }
                if(event.type == SDL_KEYDOWN) {
                    SDL_Keycode key = event.key.keysym.sym;
                    if(key == SDLK_UP) { if(difficultyMenuIndex > 0) difficultyMenuIndex--; }
                    else if(key == SDLK_DOWN) { if(difficultyMenuIndex < 3) difficultyMenuIndex++; }
                    else if(key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                        if(difficultyMenuIndex == 0) { difficulty = DIFF_EASY; startNewGame(); state = STATE_PLAYING; }
                        else if(difficultyMenuIndex == 1) { difficulty = DIFF_MEDIUM; startNewGame(); state = STATE_PLAYING; }
                        else if(difficultyMenuIndex == 2) { difficulty = DIFF_HARD; startNewGame(); state = STATE_PLAYING; }
                        else if(difficultyMenuIndex == 3) { state = STATE_THEME_SELECT; }
                    } else if (key == SDLK_ESCAPE) {
                        state = STATE_THEME_SELECT;
                    }
                }
            } else if(state == STATE_CONFIRM_EXIT) {
                if(event.type == SDL_KEYDOWN) {
                    SDL_Keycode key = event.key.keysym.sym;
                    if(key == SDLK_y) { isRunning = false; }
                    else if(key == SDLK_n || key == SDLK_ESCAPE) { state = STATE_THEME_SELECT; }
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

        if(state == STATE_THEME_SELECT) {
            renderThemeMenu();
        } else if(state == STATE_DIFFICULTY_SELECT) {
            renderDifficultyMenu();
        } else if(state == STATE_CONFIRM_EXIT) {
            renderConfirmExit();
        } else {
            renderGame();
            if(isGameOver()) {
                renderResult();
                state = STATE_THEME_SELECT;
            }
        }
        SDL_Delay(16);
    }
}

Game::~Game() {
    if (font) {
        TTF_CloseFont(font);
    }
    if (fontSmall) {
        TTF_CloseFont(fontSmall);
    }
    TTF_Quit();
    for(auto texture : hangmanImages) SDL_DestroyTexture(texture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
