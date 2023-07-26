#pragma once
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
        vector<string> movesMade = {};

        // Constructor.
        Board(string startPosFen, vector<string> moves);
        // Prints board to stdout.
        void printBoard();
    
    private:
        // Sets up board.
        void setupBoard(vector<string> fen);
        // Makes a move on the board.
        void makeMove(string move);
    };
}