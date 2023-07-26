#pragma once
#include <bitset>

using namespace std;

struct fen {
    string piecePlacement;
    string activeColor;
    string castlingRights;
    string enPassantTargets;
    int halfMoves;
    int fullMoves;
    vector<string> movesMade = {};
};
