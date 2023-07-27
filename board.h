#pragma once
#include "fen.h"
#include "helperFunctions.h"
#include "piece.h"
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
        piece::Piece p;
        vector< vector<bitset<5> > > square = {{bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                               {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}}};
        string activeColor;
        bitset<5> activeColorBits;
        string castlingRights;
        string enPassantTargets;
        int halfMoves;
        int fullMoves;
        vector<string> movesMade = {};

        vector< vector< numSquaresStruct > > SquaresToEdge = numSquaresToEdges();

        void setupBoard(fen f);
        // Prints board to stdout.
        void printBoard();

        vector< moveStruct > generateMoves();

        vector< moveStruct > generateQueenMoves(int rank, int file);
        vector< moveStruct > generateRookMoves(int rank, int file);
        vector< moveStruct > generateBishopMoves(int rank, int file);
    
    private:
        // Sets up board.
        void setupBoardLayout(fen f);
        // Makes a move on the board.
        void makeMove(string move);
    };
}