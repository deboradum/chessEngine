#include "board.h"
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
         << "Number of full moves: " << fullMoves << endl;
}

void Board::setupBoard(fen f) {
    activeColor = f.activeColor;
    castlingRights = f.castlingRights;
    enPassantTargets = f.enPassantTargets;
    fullMoves = f.fullMoves;
    halfMoves = f.halfMoves;

    vector<string> fenPP = convertFen(f.piecePlacement);
    int rank = 0;
    for (string line : fenPP) {
        int file = 0;
        for (char& c : line) {
            if (isdigit(c)) {
                file += int(c)-48;
                continue;
            }
            setPiece(rank, file, c, square);
            file++;
        }
        rank++;
    }
}

void Board::makeMove(string move) {

    return;
}
