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

void Board::setupBoard(fen f) {
    setupBoardLayout(f);
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
}

void Board::setupBoardLayout(fen f) {
    activeColor = f.activeColor;
    if (!activeColor.compare("w")) {
        activeColorBits = p.White;
    } else {
        activeColorBits = p.Black;
    }
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

vector< moveStruct > Board::generateMoves() {
    vector< moveStruct > moveList;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bitset<5> piece = square[rank][file];
            if (!p.isColor(piece, activeColorBits)) continue;

            if (p.isType(piece, p.King)) {

            } else if (p.isType(piece, p.Queen)) {
                vector < moveStruct> queenMoves = generateQueenMoves(rank, file);
                moveList.insert(moveList.end(), queenMoves.begin(), queenMoves.end());
            } else if (p.isType(piece, p.Rook)) {
                vector < moveStruct> rookMoves = generateRookMoves(rank, file);
                moveList.insert(moveList.end(), rookMoves.begin(), rookMoves.end());
            } else if (p.isType(piece, p.Bishop)) {
                vector < moveStruct> bishopMoves = generateBishopMoves(rank, file);
                moveList.insert(moveList.end(), bishopMoves.begin(), bishopMoves.end());
            } else if (p.isType(piece, p.Knight)) {

            } else if (p.isType(piece, p.Pawn)) {

            } else {
                continue;
            }
        }
    }

    return moveList;
}

vector< moveStruct > Board::generateRookMoves(int rank, int file) {
    vector< moveStruct > moveList;
    bitset<5> currPiece = square[rank][file];
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].north; rankOffset++) {
        if (!p.isType(square[rank-rankOffset][file], p.None)) {
            // Check if same color, if not, add capture to move list and break, else break.
            if (!p.isColor(currPiece, activeColorBits)) {
                moveStruct m;
                m.startSquare = indexToPosition(rank, file);
                m.endSquare = indexToPosition(rank-rankOffset, file);
                moveList.push_back(m);
            }

            break;
        }
        moveStruct m;
        m.startSquare = indexToPosition(rank, file);
        m.endSquare = indexToPosition(rank-rankOffset, file);
        moveList.push_back(m);
    }
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].south; rankOffset++) {
        if (!p.isType(square[rank+rankOffset][file], p.None)) {
            // Check if same color, if not, add capture to move list and break, else break.
            if (!p.isColor(currPiece, activeColorBits)) {
                moveStruct m;
                m.startSquare = indexToPosition(rank, file);
                m.endSquare = indexToPosition(rank+rankOffset, file);
                moveList.push_back(m);
            }

            break;
        }
        moveStruct m;
        m.startSquare = indexToPosition(rank, file);
        m.endSquare = indexToPosition(rank+rankOffset, file);
        moveList.push_back(m);
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].east; fileOffset++) {
        if (!p.isType(square[rank][file+fileOffset], p.None)) {
            // Check if same color, if not, add capture to move list and break, else break.
            if (!p.isColor(currPiece, activeColorBits)) {
                moveStruct m;
                m.startSquare = indexToPosition(rank, file);
                m.endSquare = indexToPosition(rank, file+fileOffset);
                moveList.push_back(m);
            }

            break;
        }
        moveStruct m;
        m.startSquare = indexToPosition(rank, file);
        m.endSquare = indexToPosition(rank, file+fileOffset);
        moveList.push_back(m);
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].west; fileOffset++) {
        if (!p.isType(square[rank][file-fileOffset], p.None)) {
            // Check if same color, if not, add capture to move list and break, else break.
            if (!p.isColor(currPiece, activeColorBits)) {
                moveStruct m;
                m.startSquare = indexToPosition(rank, file);
                m.endSquare = indexToPosition(rank, file-fileOffset);
                moveList.push_back(m);
            }

            break;
        }
        moveStruct m;
        m.startSquare = indexToPosition(rank, file);
        m.endSquare = indexToPosition(rank, file-fileOffset);
        moveList.push_back(m);
    }

    return moveList;
}

vector< moveStruct > Board::generateBishopMoves(int rank, int file) {
    vector< moveStruct > moveList;
    bitset<5> currPiece = square[rank][file];
    for (int offset = 1; offset <= SquaresToEdge[rank][file].northEast; offset++) {
        if (!p.isType(square[rank-offset][file+offset], p.None)) {
            if (!p.isColor(currPiece, activeColorBits)) {
                moveStruct m;
                m.startSquare = indexToPosition(rank, file);
                m.endSquare = indexToPosition(rank-offset, file+offset);
                moveList.push_back(m);
            }
            break;
        }
        moveStruct m;
        m.startSquare = indexToPosition(rank, file);
        m.endSquare = indexToPosition(rank-offset, file+offset);
        moveList.push_back(m);
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southEast; offset++) {
        if (!p.isType(square[rank+offset][file+offset], p.None)) {
            if (!p.isColor(currPiece, activeColorBits)) {
                moveStruct m;
                m.startSquare = indexToPosition(rank, file);
                m.endSquare = indexToPosition(rank+offset, file+offset);
                moveList.push_back(m);
            }
            break;
        }
        moveStruct m;
        m.startSquare = indexToPosition(rank, file);
        m.endSquare = indexToPosition(rank+offset, file+offset);
        moveList.push_back(m);
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southWest; offset++) {
        if (!p.isType(square[rank+offset][file-offset], p.None)) {
            if (!p.isColor(currPiece, activeColorBits)) {
                moveStruct m;
                m.startSquare = indexToPosition(rank, file);
                m.endSquare = indexToPosition(rank+offset, file-offset);
                moveList.push_back(m);
            }
            break;
        }
        moveStruct m;
        m.startSquare = indexToPosition(rank, file);
        m.endSquare = indexToPosition(rank+offset, file-offset);
        moveList.push_back(m);
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].northWest; offset++) {
        if (!p.isType(square[rank-offset][file-offset], p.None)) {
            if (!p.isColor(currPiece, activeColorBits)) {
                moveStruct m;
                m.startSquare = indexToPosition(rank, file);
                m.endSquare = indexToPosition(rank-offset, file-offset);
                moveList.push_back(m);
            }
            break;
        }
        moveStruct m;
        m.startSquare = indexToPosition(rank, file);
        m.endSquare = indexToPosition(rank-offset, file-offset);
        moveList.push_back(m);
    }

    return moveList;
}

vector< moveStruct > Board::generateQueenMoves(int rank, int file) {
    vector < moveStruct> moveList = generateRookMoves(rank, file);
    vector < moveStruct> bishopMoves = generateBishopMoves(rank, file);
    moveList.insert(moveList.end(), bishopMoves.begin(), bishopMoves.end());

    return moveList;
}
