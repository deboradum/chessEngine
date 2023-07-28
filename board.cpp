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
                vector < moveStruct> kingMoves = generateKingMoves(rank, file);
                moveList.insert(moveList.end(), kingMoves.begin(), kingMoves.end());
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
                vector < moveStruct> knightMoves = generateKnightMoves(rank, file);
                moveList.insert(moveList.end(), knightMoves.begin(), knightMoves.end());
            } else if (p.isType(piece, p.Pawn)) {
                vector < moveStruct> pawnMoves = generatePawnMoves(rank, file);
                moveList.insert(moveList.end(), pawnMoves.begin(), pawnMoves.end());
            } else {
                continue;
            }
        }
    }

    return moveList;
}

vector< moveStruct > Board::generateKingMoves(int rank, int file) {
    vector< moveStruct > moveList;
    // Move north
    if ((isEmptySquare(rank-1, file) || isEnemy(rank-1, file)) && isLegalSquare(rank-1, file)) {
        moveList.push_back(createMoveStruct(rank, file, rank-1, file));
    }
    // Move north east
    if ((isEmptySquare(rank-1, file+1) || isEnemy(rank-1, file+1)) && isLegalSquare(rank-1, file+1)) {
        moveList.push_back(createMoveStruct(rank, file, rank-1, file+1));
    }
    // Move east
    if ((isEmptySquare(rank, file+1) || isEnemy(rank, file+1)) && isLegalSquare(rank, file+1)) {
        moveList.push_back(createMoveStruct(rank, file, rank, file+1));
    }
    // Move south east
    if ((isEmptySquare(rank+1, file+1) || isEnemy(rank+1, file+1)) && isLegalSquare(rank+1, file+1)) {
        moveList.push_back(createMoveStruct(rank, file, rank+1, file+1));
    }
    // Move south
    if ((isEmptySquare(rank+1, file) || isEnemy(rank+1, file)) && isLegalSquare(rank+1, file)) {
        moveList.push_back(createMoveStruct(rank, file, rank+1, file));
    }
    // Move south west
    if ((isEmptySquare(rank+1, file-1) || isEnemy(rank+1, file-1)) && isLegalSquare(rank+1, file-1)) {
        moveList.push_back(createMoveStruct(rank, file, rank+1, file-1));
    }
    // Move south west
    if ((isEmptySquare(rank, file-1) || isEnemy(rank, file-1)) && isLegalSquare(rank, file-1)) {
        moveList.push_back(createMoveStruct(rank, file, rank, file-1));
    }
    // Move north west
    if ((isEmptySquare(rank-1, file-1) || isEnemy(rank-1, file-1)) && isLegalSquare(rank-1, file-1)) {
        moveList.push_back(createMoveStruct(rank, file, rank-1, file-1));
    }

    return moveList;
}

vector< moveStruct > Board::generateQueenMoves(int rank, int file) {
    vector < moveStruct> moveList = generateRookMoves(rank, file);
    vector < moveStruct> bishopMoves = generateBishopMoves(rank, file);
    moveList.insert(moveList.end(), bishopMoves.begin(), bishopMoves.end());

    return moveList;
}

vector< moveStruct > Board::generateRookMoves(int rank, int file) {
    vector< moveStruct > moveList;
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].north; rankOffset++) {
        if (!isEmptySquare(rank-rankOffset, file)) {
            if (isEnemy(rank-rankOffset, file)) {
                moveList.push_back(createMoveStruct(rank, file, rank-rankOffset, file));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank-rankOffset, file));
    }
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].south; rankOffset++) {
        if (!isEmptySquare(rank+rankOffset, file)) {
            if (isEnemy(rank+rankOffset, file)) {
                moveList.push_back(createMoveStruct(rank, file, rank+rankOffset, file));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank+rankOffset, file));
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].east; fileOffset++) {
        if (!isEmptySquare(rank, file+fileOffset)) {
            if (isEnemy(rank, file+fileOffset)) {
                moveList.push_back(createMoveStruct(rank, file, rank, file+fileOffset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank, file+fileOffset));
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].west; fileOffset++) {
        if (!isEmptySquare(rank, file-fileOffset)) {
            if (isEnemy(rank, file-fileOffset)) {
                moveList.push_back(createMoveStruct(rank, file, rank, file-fileOffset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank, file-fileOffset));
    }

    return moveList;
}

vector< moveStruct > Board::generateBishopMoves(int rank, int file) {
    vector< moveStruct > moveList;
    for (int offset = 1; offset <= SquaresToEdge[rank][file].northEast; offset++) {
        if (!isEmptySquare(rank-offset, file+offset)) {
            if (isEnemy(rank-offset, file+offset)) {
                moveList.push_back(createMoveStruct(rank, file, rank-offset, file+offset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank-offset, file+offset));
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southEast; offset++) {
        if (!isEmptySquare(rank+offset, file+offset)) {
            if (isEnemy(rank+offset, file+offset)) {
                moveList.push_back(createMoveStruct(rank, file, rank+offset, file+offset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank+offset, file+offset));
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southWest; offset++) {
        if (!isEmptySquare(rank+offset, file-offset)) {
            if (isEnemy(rank+offset, file-offset)) {
                moveList.push_back(createMoveStruct(rank, file, rank+offset, file-offset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank+offset, file-offset));
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].northWest; offset++) {
        if (!isEmptySquare(rank-offset, file-offset)) {
            if (isEnemy(rank-offset, file-offset)) {
                moveList.push_back(createMoveStruct(rank, file, rank-offset, file-offset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank-offset, file-offset));
    }

    return moveList;
}

vector< moveStruct > Board::generateKnightMoves(int rank, int file) {
    vector< moveStruct > moveList;

    if (isLegalSquare(rank+1, file+2) && (isEmptySquare(rank+1, file+2) || isEnemy(rank+1, file+2))) {
        moveList.push_back(createMoveStruct(rank, file, rank+1, file+2));
    }
    if (isLegalSquare(rank+1, file-2) && (isEmptySquare(rank+1, file-2) || isEnemy(rank+1, file-2))) {
        moveList.push_back(createMoveStruct(rank, file, rank+1, file-2));
    }
    if (isLegalSquare(rank-1, file+2) && (isEmptySquare(rank-1, file+2) || isEnemy(rank-1, file+2))) {
        moveList.push_back(createMoveStruct(rank, file, rank-1, file+2));
    }
    if (isLegalSquare(rank-1, file-2) && (isEmptySquare(rank-1, file-2) || isEnemy(rank-1, file-2))) {
        moveList.push_back(createMoveStruct(rank, file, rank-1, file-2));
    }
    if (isLegalSquare(rank+2, file+1) && (isEmptySquare(rank+2, file+1) || isEnemy(rank+2, file+1))) {
        moveList.push_back(createMoveStruct(rank, file, rank+2, file+1));
    }
    if (isLegalSquare(rank+2, file-1) && (isEmptySquare(rank+2, file-1) || isEnemy(rank+2, file-1))) {
        moveList.push_back(createMoveStruct(rank, file, rank+2, file-1));
    }
    if (isLegalSquare(rank-2, file+1) && (isEmptySquare(rank-2, file+1) || isEnemy(rank-2, file+1))) {
        moveList.push_back(createMoveStruct(rank, file, rank-2, file+1));
    }
    if (isLegalSquare(rank-2, file-1) && (isEmptySquare(rank-2, file-1) || isEnemy(rank-2, file-1))) {
        moveList.push_back(createMoveStruct(rank, file, rank-2, file-1));
    }

    return moveList;
}

vector< moveStruct > Board::generatePawnMoves(int rank, int file) {
    vector< moveStruct > moveList;

    // Determine direction since Pawns can only move forward in a single direction. 
    int rankDirection = -1;
    if (p.isColor(activeColorBits, p.White)) {
        rankDirection = 1;
    }

    // Single step
    if (isEmptySquare(rank-1*rankDirection, file)) {
        moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file));
    }

    // Two step first move
    if (p.isColor(activeColorBits, p.White)) {
        if (rank == 6 && isEmptySquare(rank-2*rankDirection, file) && isEmptySquare(rank-1*rankDirection, file)) {
            moveList.push_back(createMoveStruct(rank, file, rank-2*rankDirection, file));
        }  
    } else if (p.isColor(activeColorBits, p.Black)) {
        if (rank == 1 && isEmptySquare(rank-2*rankDirection, file) && isEmptySquare(rank-1*rankDirection, file)) {
            moveList.push_back(createMoveStruct(rank, file, rank-2*rankDirection, file));
        }
    }

    // Capture
    if (isLegalSquare(rank-1*rankDirection, file+1) && !isEmptySquare(rank-1*rankDirection, file+1) && isEnemy(rank-1*rankDirection, file+1)) {
        moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file+1));
    }
    if (isLegalSquare(rank-1*rankDirection, file-1) && !isEmptySquare(rank-1*rankDirection, file-1) && isEnemy(rank-1*rankDirection, file-1)) {
        moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file-1));
    }

    // En Passant
    if (enPassantTargets.length() == 2) {
        if (positionToRank(enPassantTargets) == rank - 1*rankDirection && abs(positionToFile(enPassantTargets) - file) == 1) {
            moveStruct m;
            m.startSquare = indexToPosition(rank, file);
            m.endSquare = enPassantTargets;
            moveList.push_back(m);
        }
    }

    return moveList;
}

bool Board::isEnemy(int rank, int file) {
    return !p.isColor(square[rank][file], activeColorBits);
}

bool Board::isEmptySquare(int rank, int file) {
    return p.isType(square[rank][file], p.None);
}
