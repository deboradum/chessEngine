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
    piece::Piece Piece;
    int rank = 0;
    for (string line : fenPP) {
        int file = 0;
        for (char& c : line) {
            if (isdigit(c)) {
                file += int(c)-48;
                continue;
            }
            switch (tolower(c)) {
                case 'k':
                    if (isupper(c)) {
                        square[rank][file] = Piece.White | Piece.King;
                    } else {
                        square[rank][file] = Piece.Black | Piece.King;
                    }
                    break;
                case 'q':
                    if (isupper(c)) {
                        square[rank][file] = Piece.White | Piece.Queen;
                    } else {
                        square[rank][file] = Piece.Black | Piece.Queen;
                    }
                    break;
                case 'r':
                    if (isupper(c)) {
                        square[rank][file] = Piece.White | Piece.Rook;
                    } else {
                        square[rank][file] = Piece.Black | Piece.Rook;
                    }
                    break;
                case 'b':
                    if (isupper(c)) {
                        square[rank][file] = Piece.White | Piece.Bishop;
                    } else {
                        square[rank][file] = Piece.Black | Piece.Bishop;
                    }
                    break;
                case 'n':
                    if (isupper(c)) {
                        square[rank][file] = Piece.White | Piece.Knight;
                    } else {
                        square[rank][file] = Piece.Black | Piece.Knight;
                    }
                    break;
                case 'p':
                    if (isupper(c)) {
                        square[rank][file] = Piece.White | Piece.Pawn;
                    } else {
                        square[rank][file] = Piece.Black | Piece.Pawn;
                    }
                    break;
            }
            file++;
        }
        rank++;
    }
}

void Board::makeMove(string move) {

    return;
}
