#include "FileReader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::vector<std::string> FileReader::ReadWords(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Khong the mo file: " + filePath);
    }

    std::vector<std::string> words;
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string word;
        while (ss >> word) {
            words.push_back(word);
        }
    }

    if (words.empty()) {
        throw std::runtime_error("File rong hoac khong hop le");
    }

    return words;
}
