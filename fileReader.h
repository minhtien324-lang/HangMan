#ifndef FILE_READER_H
#define FILE_READER_H

#include <vector>
#include <string>

class FileReader {
public:
    static std::vector<std::string> ReadLines(const std::string& filePath);
    static std::vector<std::string> ReadWords(const std::string& filePath);
};

#endif
