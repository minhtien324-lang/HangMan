#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <vector>
#include <string>
#include <map>

class ThemeManager {
public:
    enum Theme {
        THEME_ANIMALS,      // Động vật
        THEME_FOOD,         // Thức ăn
        THEME_NATURE,       // Thiên nhiên
        THEME_TECHNOLOGY,   // Công nghệ
        THEME_SPORTS,       // Thể thao
        THEME_CARS
    };

    ThemeManager();
    ~ThemeManager();

    // Lấy danh sách tên các chủ đề
    std::vector<std::string> getThemeNames() const;

    // Lấy từ theo chủ đề
    std::vector<std::string> getWordsByTheme(Theme theme) const;

    // Lấy từ ngẫu nhiên theo chủ đề
    std::string getRandomWordByTheme(Theme theme) const;

    // Lấy tên chủ đề theo enum
    std::string getThemeName(Theme theme) const;

    // Lấy số lượng chủ đề
    int getThemeCount() const;

private:
    std::map<Theme, std::vector<std::string>> themeWords;
    std::map<Theme, std::string> themeNames;

    void loadThemeWords();
    void loadWordsFromFile(const std::string& filename, Theme theme);
};

#endif

