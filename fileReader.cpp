#include "fileReader.h"
#include <fstream>
#include <stdexcept>
using namespace std;
vector<string> FileReader::ReadWords(const string& filePath) {
    vector<string> words;
    ifstream file(filePath);

    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filePath);
    }

    string word;
    while (getline(file, word)) {
        if (!word.empty()) {
            words.push_back(word);
        }
    }

    return words;
}
