#include "board.h"
#include "fen.h"
#include "helperFunctions.h"
#include "piece.h"
#include "constants.h"
#include <_ctype.h>
#include <cctype>
#include <iostream>
#include <string>
#include <bitset>
#include <cstddef>

using namespace std;
using namespace board;

Board::Board(fen f) {
    setupBoard(f);
    for (string move : f.movesMade) {
        makeMove(move);
        movesMade.push_back(move);
    }
}

void Board::printBoard() {
    for (vector<bitset<5> > line : Board::square) {
        for (bitset<5> piece : line) {
            cout << piece << " ";
        }
        cout << "\n";
    }
    cout << "Active color: " << activeColor << endl
         << "Castling rights: " << castlingRights << endl
         << "En Passant targets: " << enPassantTargets << endl
         << "Number of half moves: " << halfMoves << endl
         << "Number of full moves: " << fullMoves << endl
         << "Moves made: ";
    
    for (string el : movesMade) {
        cout << el << " ";
    };
    cout << endl;

    cout << "Squares to edges: " << endl;
    for (vector< numSquaresStruct > line : SquaresToEdge) {
        for (numSquaresStruct s : line) {
            cout << s.northEast << " ";
        }
        cout << endl;
    };
}

void Board::setupBoard(fen f) {
    activeColor = f.activeColor;
    castlingRights = f.castlingRights;
    enPassantTargets = f.enPassantTargets;
    fullMoves = f.fullMoves;
    halfMoves = f.halfMoves;

    int rank = 0;
    for (string line : convertFen(f.piecePlacement)) {
        int file = 0;
        for (char& c : line) {
            if (isdigit(c)) {
                file += int(c)-'0';
                continue;
            }
            setPiece(rank, file, c, square);
            file++;
        }
        rank++;
    }
}

void Board::makeMove(string move) {
    assert(move.length() == 4);
    string start = move.substr(0, 2);
    string end = move.substr(2, 4);

    square[positionToRank(end)][positionToFile(end)] = square[positionToRank(start)][positionToFile(start)];
    square[positionToRank(start)][positionToFile(start)] = EMPTY_SQUARE;

    return;
}
