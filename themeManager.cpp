#include "themeManager.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

ThemeManager::ThemeManager() {
    // Khởi tạo tên các chủ đề
    themeNames[THEME_ANIMALS] = "Dong vat";
    themeNames[THEME_FOOD] = "Thuc an";
    themeNames[THEME_NATURE] = "Thien nhien";
    themeNames[THEME_TECHNOLOGY] = "Cong nghe";
    themeNames[THEME_SPORTS] = "The thao";
    
    loadThemeWords();
}

ThemeManager::~ThemeManager() {
    // Destructor
}

void ThemeManager::loadThemeWords() {
    loadWordsFromFile("animals.txt", THEME_ANIMALS);
    loadWordsFromFile("food.txt", THEME_FOOD);
    loadWordsFromFile("nature.txt", THEME_NATURE);
    loadWordsFromFile("technology.txt", THEME_TECHNOLOGY);
    loadWordsFromFile("sports.txt", THEME_SPORTS);
}

void ThemeManager::loadWordsFromFile(const std::string& filename, Theme theme) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Khong the mo file: " << filename << std::endl;
        return;
    }
    
    std::string word;
    while (std::getline(file, word)) {
        if (!word.empty()) {
            themeWords[theme].push_back(word);
        }
    }
    file.close();
}

std::vector<std::string> ThemeManager::getThemeNames() const {
    std::vector<std::string> names;
    for (const auto& pair : themeNames) {
        names.push_back(pair.second);
    }
    return names;
}

std::vector<std::string> ThemeManager::getWordsByTheme(Theme theme) const {
    auto it = themeWords.find(theme);
    if (it != themeWords.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

std::string ThemeManager::getRandomWordByTheme(Theme theme) const {
    auto words = getWordsByTheme(theme);
    if (words.empty()) {
        return "hangman"; // fallback
    }
    
    srand((unsigned int)time(0));
    return words[rand() % words.size()];
}

std::string ThemeManager::getThemeName(Theme theme) const {
    auto it = themeNames.find(theme);
    if (it != themeNames.end()) {
        return it->second;
    }
    return "Unknown";
}

int ThemeManager::getThemeCount() const {
    return themeNames.size();
}

