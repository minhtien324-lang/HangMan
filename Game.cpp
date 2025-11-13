#include "Game.h"
#include <SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <cmath>

namespace {
    const SDL_Color COLOR_BACKGROUND{245, 245, 245, 255};
    const SDL_Color COLOR_TEXT{30, 30, 30, 255};
    const SDL_Color COLOR_SUBTEXT{80, 80, 80, 255};
    const SDL_Color COLOR_ALERT{220, 20, 60, 255};
    const SDL_Color COLOR_BUTTON{230, 230, 230, 255};
    const SDL_Color COLOR_BUTTON_DISABLED{200, 200, 200, 255};
    const SDL_Color COLOR_BUTTON_SELECTED{160, 190, 255, 255};
    const SDL_Color COLOR_BUTTON_BORDER{90, 90, 90, 255};
}
Game::Game() : window(nullptr), renderer(nullptr), wrongGuesses(0), isRunning(true),
    state(STATE_THEME_SELECT), difficulty(DIFF_EASY), selectedTheme(ThemeManager::THEME_ANIMALS),
    maxAttempts(MAX_ATTEMPTS), themeMenuIndex(0), difficultyMenuIndex(0),
    showingResult(false), lastWin(false), playedGames(0),
    audioLoaded(false), wonGames(0) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    window = SDL_CreateWindow("Hangman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! Error: " << TTF_GetError() << std::endl;
        exit(1);
    }

    fontLarge = TTF_OpenFont("roboto.ttf", 36);
    fontMedium = TTF_OpenFont("roboto.ttf", 24);
    fontSmall = TTF_OpenFont("roboto.ttf", 18);
    if (!fontLarge || !fontMedium || !fontSmall) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if (audioManager.initialize()) {
        const std::vector<std::string> searchPaths{
            "audio",
            "assets/audio",
            "assets/sounds",
            "sounds"
        };
        for (const auto& path : searchPaths) {
            if (audioManager.loadFromDirectory(path)) {
                audioLoaded = true;
                break;
            }
        }
        if (!audioLoaded) {
            std::cerr << "Warning: audio assets not found. Expected files like 'audio/" << "background_music.mp3" << "'." << std::endl;
        }
    } else {
        std::cerr << "Warning: failed to initialize audio. Sound will be disabled." << std::endl;
    }

    initializeLetterButtons();
}

void Game::renderButton(const std::string& text, const SDL_Rect& rect, bool selected, TTF_Font* font) {
    SDL_Color fill = selected ? COLOR_BUTTON_SELECTED : COLOR_BUTTON;
    drawRect(rect, fill, COLOR_BUTTON_BORDER);
    drawTextCentered(text, rect, COLOR_TEXT, font);
}

void Game::drawRect(const SDL_Rect& rect, SDL_Color fill, SDL_Color border) {
    SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
    SDL_RenderDrawRect(renderer, &rect);
}

void Game::drawText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font) {
    if (!font) {
        font = fontMedium;
    }
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst { x, y, surface->w, surface->h };
    SDL_FreeSurface(surface);
    if (!texture) return;
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

void Game::drawTextCentered(const std::string& text, const SDL_Rect& rect, SDL_Color color, TTF_Font* font) {
    if (!font) {
        font = fontMedium;
    }
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect dst{
        rect.x + (rect.w - surface->w) / 2,
        rect.y + (rect.h - surface->h) / 2,
        surface->w,
        surface->h
    };
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

void Game::drawScaffold() {
    const int originX = 80;
    const int originY = 60;
    const int canvasHeight = 400;

    auto mapX = [&](int x) { return originX + x; };
    auto mapY = [&](int y) { return originY + (canvasHeight - y); };
    auto drawLine = [&](int x1, int y1, int x2, int y2, int thickness) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        int sx1 = mapX(x1);
        int sy1 = mapY(y1);
        int sx2 = mapX(x2);
        int sy2 = mapY(y2);
        for (int i = 0; i < thickness; ++i) {
            SDL_RenderDrawLine(renderer, sx1, sy1 + i, sx2, sy2 + i);
        }
    };

    drawLine(40, 55, 180, 55, 10);
    drawLine(165, 60, 165, 365, 10);
    drawLine(160, 360, 100, 360, 10);
    drawLine(100, 365, 100, 330, 10);
    drawLine(100, 330, 100, 310, 1);
}

void Game::drawHangedMan() {
    const int originX = 80;
    const int originY = 60;
    const int canvasHeight = 400;

    auto mapX = [&](int x) { return originX + x; };
    auto mapY = [&](int y) { return originY + (canvasHeight - y); };
    auto drawSegment = [&](int x1, int y1, int x2, int y2) {
        SDL_RenderDrawLine(renderer, mapX(x1), mapY(y1), mapX(x2), mapY(y2));
    };

    SDL_SetRenderDrawColor(renderer, COLOR_ALERT.r, COLOR_ALERT.g, COLOR_ALERT.b, COLOR_ALERT.a);

    if (wrongGuesses >= 1) {
        const int cx = mapX(100);
        const int cy = mapY(290);
        const int radius = 20;
        constexpr double PI = 3.14159265358979323846;
        for (int deg = 0; deg < 360; ++deg) {
            double rad = deg * PI / 180.0;
            double nextRad = (deg + 1) * PI / 180.0;
            int x1 = static_cast<int>(cx + radius * std::cos(rad));
            int y1 = static_cast<int>(cy + radius * std::sin(rad));
            int x2 = static_cast<int>(cx + radius * std::cos(nextRad));
            int y2 = static_cast<int>(cy + radius * std::sin(nextRad));
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
    }
    if (wrongGuesses >= 2) {
        drawSegment(100, 270, 100, 170);
    }
    if (wrongGuesses >= 3) {
        drawSegment(100, 250, 80, 250);
        drawSegment(80, 250, 60, 210);
        drawSegment(60, 210, 60, 190);
    }
    if (wrongGuesses >= 4) {
        drawSegment(100, 250, 120, 250);
        drawSegment(120, 250, 140, 210);
        drawSegment(140, 210, 140, 190);
    }
    if (wrongGuesses >= 5) {
        drawSegment(100, 170, 80, 170);
        drawSegment(80, 170, 70, 140);
        drawSegment(70, 140, 70, 80);
        drawSegment(70, 80, 60, 80);
    }
    if (wrongGuesses >= 6) {
        drawSegment(100, 170, 120, 170);
        drawSegment(120, 170, 130, 140);
        drawSegment(130, 140, 130, 80);
        drawSegment(130, 80, 140, 80);
    }
}

std::string Game::buildDisplayedWord() const {
    std::ostringstream oss;
    for (size_t i = 0; i < guessedWord.size(); ++i) {
        char c = guessedWord[i];
        oss << (c == '_' ? '_' : static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
        if (i + 1 < guessedWord.size()) {
            oss << ' ';
        }
    }
    return oss.str();
}

void Game::renderThemeMenu() {
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_RenderClear(renderer);

    drawTextCentered("Chon Chu De", SDL_Rect{0, 40, 800, 60}, COLOR_TEXT, fontLarge);

    std::vector<std::string> themeNames = themeManager.getThemeNames();
    const std::string quitLabel = "Thoat Game";

    int themeCount = static_cast<int>(themeNames.size());
    int totalOptions = themeCount + 1; // + quit
    if (themeMenuIndex >= totalOptions) {
        themeMenuIndex = totalOptions - 1;
    }

    const int columns = 3;
    const int rows = 2;
    const int buttonW = 200;
    const int buttonH = 110;
    const int gapX = 30;
    const int gapY = 28;
    const int startX = 60;
    const int startY = 120;

    themeButtonRects.clear();
    int idx = 0;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            SDL_Rect rect{
                startX + col * (buttonW + gapX),
                startY + row * (buttonH + gapY),
                buttonW,
                buttonH
            };
            if (idx < themeCount) {
                themeButtonRects.push_back(rect);
                renderButton(themeNames[idx], rect, themeMenuIndex == idx, fontMedium);
            }
            ++idx;
        }
    }

    SDL_Rect quitRect{
        startX,
        startY + rows * (buttonH + gapY),
        buttonW,
        buttonH
    };
    themeButtonRects.push_back(quitRect);
    renderButton(quitLabel, quitRect, themeMenuIndex == themeCount, fontMedium);

    drawTextCentered("Su dung phim mui ten hoac chuot de chon.", SDL_Rect{0, 430, 800, 30}, COLOR_SUBTEXT, fontSmall);
    drawTextCentered("Nhan Enter de xac nhan.", SDL_Rect{0, 460, 800, 30}, COLOR_SUBTEXT, fontSmall);

    SDL_RenderPresent(renderer);
}

void Game::renderDifficultyMenu() {
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_RenderClear(renderer);

    drawTextCentered("Chon do kho", SDL_Rect{0, 100, 800, 60}, COLOR_TEXT, fontLarge);

    const std::vector<std::string> options{
        "De (tu ngan)",
        "Vua (tu vua)",
        "Kho (tu dai)",
        "Quay lai"
    };

    difficultyButtonRects.clear();
    const int startX = 260;
    const int startY = 180;
    const int buttonW = 280;
    const int buttonH = 70;
    const int gapY = 20;

    for (size_t i = 0; i < options.size(); ++i) {
        SDL_Rect rect{
            startX,
            startY + static_cast<int>(i) * (buttonH + gapY),
            buttonW,
            buttonH
        };
        difficultyButtonRects.push_back(rect);
        renderButton(options[i], rect, difficultyMenuIndex == static_cast<int>(i), fontMedium);
    }

    drawTextCentered("Chon do kho phu hop, chuot hoac Enter de bat dau.", SDL_Rect{0, 500, 800, 30}, COLOR_SUBTEXT, fontSmall);

    SDL_RenderPresent(renderer);
}

void Game::renderConfirmExit() {
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_RenderClear(renderer);

    drawTextCentered("Ban co chac muon thoat? (Y/N)", SDL_Rect{0, 260, 800, 40}, COLOR_TEXT, fontMedium);
    drawTextCentered("Nhan Y de thoat, N de quay lai.", SDL_Rect{0, 320, 800, 30}, COLOR_SUBTEXT, fontSmall);

    SDL_RenderPresent(renderer);
}

void Game::startNewGame() {
    std::vector<std::string> words = themeManager.getWordsByTheme(selectedTheme);
    if (words.empty()) {
        secretWord = "HANGMAN";
    } else {
        std::vector<std::string> filtered;
        for (const auto& w : words) {
            size_t len = w.size();
            if (difficulty == DIFF_EASY && len <= 4) filtered.push_back(w);
            else if (difficulty == DIFF_MEDIUM && len >= 5 && len <= 7) filtered.push_back(w);
            else if (difficulty == DIFF_HARD && len >= 8) filtered.push_back(w);
        }
        if (filtered.empty()) filtered = words;

        srand(static_cast<unsigned int>(time(nullptr)));
        if (!filtered.empty()) {
            secretWord = filtered[rand() % filtered.size()];
        } else {
            secretWord = "hangman";
        }
    }

    std::transform(secretWord.begin(), secretWord.end(), secretWord.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });

    if (difficulty == DIFF_EASY) maxAttempts = 6;
    else if (difficulty == DIFF_MEDIUM) maxAttempts = 6;
    else maxAttempts = 6;

    resetRound();
}

void Game::processInput(char guess) {
    if (showingResult) return;
    char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(guess)));
    if (std::find(guessedLetters.begin(), guessedLetters.end(), upper) != guessedLetters.end()) {
        return;
    }

    guessedLetters.push_back(upper);
    if (audioLoaded) {
        audioManager.playLetterClick();
    }
    setLetterButtonState(upper, true);

    bool correct = false;
    for (size_t i = 0; i < secretWord.length(); ++i) {
        if (secretWord[i] == upper) {
            guessedWord[i] = upper;
            correct = true;
        }
    }
    if (!correct) {
        wrongGuesses += 1;
    }

    displayWord = buildDisplayedWord();

    if (isGameOver()) {
        showingResult = true;
        lastWin = (guessedWord == secretWord);
        playedGames += 1;
        if (lastWin) {
            wonGames += 1;
        }
        if (audioLoaded) {
            if (lastWin) {
                audioManager.playWinMusic();
            } else {
                audioManager.playLoseMusic();
            }
        }
    }
}

void Game::renderGame() {
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_RenderClear(renderer);

    drawScaffold();
    drawHangedMan();

    drawText("Hangman", 420, 24, COLOR_TEXT, fontLarge);
    drawText(displayWord, 80, 480, COLOR_TEXT, fontMedium);

    for (const auto& button : letterButtons) {
        SDL_Color fill = button.disabled ? COLOR_BUTTON_DISABLED : COLOR_BUTTON;
        drawRect(button.rect, fill, COLOR_BUTTON_BORDER);
        drawText(button.label, button.rect.x + 14, button.rect.y + 8, COLOR_TEXT, fontMedium);
    }

    drawRect(buttonNew, COLOR_BUTTON, COLOR_BUTTON_BORDER);
    drawText("New", buttonNew.x + 18, buttonNew.y + 8, COLOR_TEXT, fontMedium);

    drawRect(buttonRestart, COLOR_BUTTON, COLOR_BUTTON_BORDER);
    drawText("Restart", buttonRestart.x + 12, buttonRestart.y + 8, COLOR_TEXT, fontMedium);

    drawRect(buttonQuit, COLOR_BUTTON, COLOR_BUTTON_BORDER);
    drawText("Quit", buttonQuit.x + 26, buttonQuit.y + 8, COLOR_TEXT, fontMedium);

    if (showingResult) {
        SDL_Rect overlay{ 120, 140, 560, 280 };
        drawRect(overlay, SDL_Color{250, 250, 250, 255}, COLOR_BUTTON_BORDER);

        std::ostringstream msg;
        if (lastWin) {
            msg << "You've won! Congratulations!";
        } else {
            msg << "You've lost! The word was '" << secretWord << "'.";
        }
        std::ostringstream stats;
        stats << "That's " << wonGames << " out of " << playedGames << "!";

        drawText(msg.str(), overlay.x + 20, overlay.y + 30, COLOR_TEXT, fontMedium);
        drawText(stats.str(), overlay.x + 20, overlay.y + 80, COLOR_TEXT, fontMedium);
        drawText("Press Y for another round, N to close", overlay.x + 20, overlay.y + 140, COLOR_TEXT, fontMedium);
    }

    SDL_RenderPresent(renderer);
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
            } else if(state == STATE_PLAYING) {
                if(event.type == SDL_KEYDOWN) {
                    SDL_Keycode key = event.key.keysym.sym;
                    if (showingResult) {
                        if (key == SDLK_y) {
                            startNewGame();
                        } else if (key == SDLK_n) {
                            if (audioLoaded) {
                                audioManager.stopBackgroundMusic();
                            }
                            state = STATE_THEME_SELECT;
                        }
                    } else {
                        if (key >= SDLK_a && key <= SDLK_z) {
                            char guess = static_cast<char>('A' + (key - SDLK_a));
                            processInput(guess);
                        } else if (key == SDLK_ESCAPE) {
                            if (audioLoaded) {
                                audioManager.stopBackgroundMusic();
                            }
                            state = STATE_THEME_SELECT;
                        }
                    }
                } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    int mx = event.button.x;
                    int my = event.button.y;
                    auto pointIn = [&](const SDL_Rect& r) {
                        return mx >= r.x && mx < r.x + r.w && my >= r.y && my < r.y + r.h;
                    };
                    if (pointIn(buttonQuit)) {
                        if (audioLoaded) {
                            audioManager.stopBackgroundMusic();
                        }
                        state = STATE_THEME_SELECT;
                    } else if (!showingResult && pointIn(buttonNew)) {
                        startNewGame();
                    } else if (!showingResult && pointIn(buttonRestart)) {
                        resetRound();
                    } else if (!showingResult) {
                        handleLetterClick(mx, my);
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
        }
        SDL_Delay(16);
    }
}
void Game::initializeLetterButtons() {
    const int lettersPerRow = 4;
    const int buttonW = 60;
    const int buttonH = 44;
    const int gridLeft = 420;
    const int gridTop = 80;
    const int gridPadX = 18;
    const int gridPadY = 12;

    for (int i = 0; i < 26; ++i) {
        int row = i / lettersPerRow;
        int col = i % lettersPerRow;
        SDL_Rect rect{
            gridLeft + col * (buttonW + gridPadX),
            gridTop + row * (buttonH + gridPadY),
            buttonW,
            buttonH
        };
        std::string label = std::string(" ") + static_cast<char>('A' + i) + " ";
        letterButtons[static_cast<size_t>(i)] = { rect, label, false };
    }

    buttonNew = SDL_Rect{ 420, 480, 100, 44 };
    buttonRestart = SDL_Rect{ 540, 480, 120, 44 };
    buttonQuit = SDL_Rect{ 680, 480, 100, 44 };
    resetLetterButtons();
}

void Game::resetLetterButtons() {
    for (auto& button : letterButtons) {
        button.disabled = false;
    }
}

void Game::setLetterButtonState(char letter, bool disabled) {
    char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(letter)));
    for (auto& button : letterButtons) {
        if (button.label.size() >= 3 && button.label[1] == upper) {
            button.disabled = disabled;
            break;
        }
    }
}

bool Game::handleLetterClick(int x, int y) {
    if (showingResult) return false;
    for (auto& button : letterButtons) {
        const SDL_Rect& r = button.rect;
        if (!button.disabled &&
            x >= r.x && x < r.x + r.w &&
            y >= r.y && y < r.y + r.h) {
            if (button.label.size() >= 3) {
                processInput(button.label[1]);
                return true;
            }
        }
    }
    return false;
}

void Game::resetRound() {
    guessedLetters.clear();
    wrongGuesses = 0;
    guessedWord.resize(secretWord.length());
    for (size_t i = 0; i < secretWord.size(); ++i) {
        char c = secretWord[i];
        if (std::isalpha(static_cast<unsigned char>(c))) {
            guessedWord[i] = '_';
        } else {
            guessedWord[i] = c;
        }
    }
    displayWord = buildDisplayedWord();
    resetLetterButtons();
    showingResult = false;
    if (audioLoaded) {
        audioManager.playBackgroundMusic();
    }
}

Game::~Game() {
    if (audioLoaded) {
        audioManager.stopBackgroundMusic();
    }
    if (fontLarge) {
        TTF_CloseFont(fontLarge);
    }
    if (fontMedium) {
        TTF_CloseFont(fontMedium);
    }
    if (fontSmall) {
        TTF_CloseFont(fontSmall);
    }
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
