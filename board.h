#pragma once
#include "fen.h"
#include <_ctype.h>
#include <cctype>
#include <iostream>
#include <string>
#include <bitset>
#include <cstddef>

using namespace std;

namespace board {
    class Board {
    public:
        vector< vector<bitset<5> > > square = {{bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}}};
        string activeColor;
        string castlingRights;
        string enPassantTargets;
        int halfMoves;
        int fullMoves;
        vector<string> movesMade = {};

        // Constructor.
        Board(fen f);
        // Prints board to stdout.
        void printBoard();
    
    private:
        // Sets up board.
        void setupBoard(fen f);
        // Makes a move on the board.
        void makeMove(string move);
    };
}