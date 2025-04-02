#include "Game.h"
#include <iostream>
using namespace std;
int main() {
    try {
        Game game;
        game.Run();
    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
