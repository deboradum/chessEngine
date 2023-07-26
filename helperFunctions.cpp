#include "helperFunctions.h"

using namespace std;

vector<string> convertFen(string fenPP) {
    vector<string> result;
    string line = "";

    for (char& c : fenPP) {
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

fen parsePosition(string positionString) {
    fen f;
    int part = 0;
    string partString = "";
    for (char& c : positionString) {
        // Inserts current line to vector on delimiter.
        if (c == ' ') {
            switch (part) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                f.piecePlacement = partString;
                break;
            case 3:
                f.activeColor = partString;
                break;
            case 4:
                f.castlingRights = partString;
                break;
            case 5:
                f.enPassantTargets = partString;
                break;
            case 6:
                f.halfMoves = stoi(partString);
                break;
            case 7:
                f.fullMoves = stoi(partString);
                break;
            default:
                break;
            }

            partString = "";
            part++;
            continue;
        }
        // Appends to current line.
        partString.push_back(c);
    }

    f.movesMade = vector<string>{};

    return f; 
}
