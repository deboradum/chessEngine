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

        bool kingMoved = false;
        bool qRookMoved = false;
        bool kRookMoved = false;

        vector<string> movesMade = {};
        vector<string> attackedSquares;
        vector<moveStruct> possibleMoves;

        vector< vector< numSquaresStruct > > SquaresToEdge = numSquaresToEdges();

        void setupBoard(fen f);
        // Prints board to stdout.
        void printBoard();

        vector< moveStruct > generateMoves();

        bool inCheck();

        string generateMove();
    
    private:
        vector< string > generateAttackedSquares();
        vector< string > generateKingAttacks(int rank, int file);
        vector< string > generateQueenAttacks(int rank, int file);
        vector< string > generateRookAttacks(int rank, int file);
        vector< string > generateBishopAttacks(int rank, int file);
        vector< string > generateKnightAttacks(int rank, int file);
        vector< string > generatePawnAttacks(int rank, int file);
        bool isPositionAttacked(int rank, int file);

        vector< moveStruct > generateKingMoves(int rank, int file);
        vector< moveStruct > generateQueenMoves(int rank, int file);
        vector< moveStruct > generateRookMoves(int rank, int file);
        vector< moveStruct > generateBishopMoves(int rank, int file);
        vector< moveStruct > generateKnightMoves(int rank, int file);
        vector< moveStruct > generatePawnMoves(int rank, int file);
        vector< moveStruct > addPromotionOptions(int beginRank, int beginFile, int endRank, int endFile);
        vector< moveStruct > generateCastlingMoves();
        vector< moveStruct > generateCastlingMovesW();
        vector< moveStruct > generateCastlingMovesB();
        bool kingCastleLegalW();
        bool queenCastleLegalW();
        bool kingCastleLegalB();
        bool queenCastleLegalB();

        // Sets up board.
        void setupBoardLayout(fen f);
        // Makes a move on the board.
        void makeMove(string move);

        bool isEnemy(int rank, int file);

        bool isEmptySquare(int rank, int file);

        string getKingPos(bitset<5> color);
    };
}