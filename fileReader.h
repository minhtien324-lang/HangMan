#ifndef FILEREADER_H
#define FILEREADER_H

#include <vector>
#include <string>
using namespace std;
class FileReader {
public:
    static vector<string> ReadWords(const string& filePath);
};

#endif
