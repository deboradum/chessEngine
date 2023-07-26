#include "helperFunctions.h"

using namespace std;

vector<string> convertFen(string fen) {
    vector<string> result;
    string line = "";

    for (char& c : fen) {
        // Inserts current line to vector on delimiter.
        if (c == '/') {
            result.push_back(line);
            line = "";
            continue;
        }
        // Appends to current line.
        line.push_back(c);
    }
    // Inserts last line to vector.
    result.push_back(line);

    return result;
}
