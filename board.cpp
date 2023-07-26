#include "board.h"
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

Board::Board(string startPosFen, vector<string> moves) {
    setupBoard(convertFen(startPosFen));
    for (string move : moves) {
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
}

void Board::setupBoard(vector<string> fen) {
    piece::Piece Piece;
    int rank = 0;
    for (string line : fen) {
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
