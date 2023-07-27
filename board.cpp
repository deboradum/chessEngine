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
    if (rank != 0) {
        if (p.isType(square[rank-1][file], p.None) || !p.isColor(square[rank-1][file], activeColorBits)) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file));   
        }
    }
    // Move north east
    if (rank != 0 && file != 7) {
        if (p.isType(square[rank-1][file+1], p.None) || !p.isColor(square[rank-1][file+1], activeColorBits)) {
            moveList.push_back(createMoveStruct(rank, file, rank-1, file+1));   
        }
    }
    // Move east
    if (file != 7) {
        if (p.isType(square[rank][file+1], p.None) || !p.isColor(square[rank][file+1], activeColorBits)) {
            moveList.push_back(createMoveStruct(rank, file, rank, file+1));   
        }
    }
    // Move south east
    if (rank != 7 && file != 7) {
        if (p.isType(square[rank+1][file+1], p.None) || !p.isColor(square[rank+1][file+1], activeColorBits)) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file+1));   
        }
    }
    // Move south
    if (rank != 7 && p.isType(square[rank+1][file], p.None)) {
        if (p.isType(square[rank+1][file], p.None) || !p.isColor(square[rank+1][file], activeColorBits)) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file));   
        }
    }
    // Move south west
    if (rank != 7 && file != 0) {
        if (p.isType(square[rank+1][file-1], p.None) || !p.isColor(square[rank+1][file-1], activeColorBits)) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file-1));   
        }
    }
    // Move south west
    if (file != 0) {
        if (p.isType(square[rank][file-1], p.None) || !p.isColor(square[rank][file-1], activeColorBits)) {
            moveList.push_back(createMoveStruct(rank, file, rank, file-1));   
        }
    }
    // Move north west
    if (rank != 0 && file != 0) {
        if (p.isType(square[rank-1][file-1], p.None) || !p.isColor(square[rank-1][file-1], activeColorBits)) {
            moveList.push_back(createMoveStruct(rank, file, rank-1, file-1));   
        }
    }


    return moveList;
}

vector< moveStruct > Board::generateRookMoves(int rank, int file) {
    vector< moveStruct > moveList;
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].north; rankOffset++) {
        if (!p.isType(square[rank-rankOffset][file], p.None)) {
            // Check if same color, if not, add capture to move list and break, else break.
            if (!p.isColor(square[rank-rankOffset][file], activeColorBits)) {
                moveList.push_back(createMoveStruct(rank, file, rank-rankOffset, file));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank-rankOffset, file));
    }
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].south; rankOffset++) {
        if (!p.isType(square[rank+rankOffset][file], p.None)) {
            // Check if same color, if not, add capture to move list and break, else break.
            if (!p.isColor(square[rank+rankOffset][file], activeColorBits)) {
                moveList.push_back(createMoveStruct(rank, file, rank+rankOffset, file));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank+rankOffset, file));
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].east; fileOffset++) {
        if (!p.isType(square[rank][file+fileOffset], p.None)) {
            // Check if same color, if not, add capture to move list and break, else break.
            if (!p.isColor(square[rank][file+fileOffset], activeColorBits)) {
                moveList.push_back(createMoveStruct(rank, file, rank, file+fileOffset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank, file+fileOffset));
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].west; fileOffset++) {
        if (!p.isType(square[rank][file-fileOffset], p.None)) {
            // Check if same color, if not, add capture to move list and break, else break.
            if (!p.isColor(square[rank][file-fileOffset], activeColorBits)) {
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
        if (!p.isType(square[rank-offset][file+offset], p.None)) {
            if (!p.isColor(square[rank-offset][file+offset], activeColorBits)) {
                moveList.push_back(createMoveStruct(rank, file, rank-offset, file+offset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank-offset, file+offset));
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southEast; offset++) {
        if (!p.isType(square[rank+offset][file+offset], p.None)) {
            if (!p.isColor(square[rank+offset][file+offset], activeColorBits)) {
                moveList.push_back(createMoveStruct(rank, file, rank+offset, file+offset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank+offset, file+offset));
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southWest; offset++) {
        if (!p.isType(square[rank+offset][file-offset], p.None)) {
            if (!p.isColor(square[rank+offset][file-offset], activeColorBits)) {
                moveList.push_back(createMoveStruct(rank, file, rank+offset, file-offset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank+offset, file-offset));
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].northWest; offset++) {
        if (!p.isType(square[rank-offset][file-offset], p.None)) {
            if (!p.isColor(square[rank-offset][file-offset], activeColorBits)) {
                moveList.push_back(createMoveStruct(rank, file, rank-offset, file-offset));
            }
            break;
        }
        moveList.push_back(createMoveStruct(rank, file, rank-offset, file-offset));
    }

    return moveList;
}

vector< moveStruct > Board::generateQueenMoves(int rank, int file) {
    vector < moveStruct> moveList = generateRookMoves(rank, file);
    vector < moveStruct> bishopMoves = generateBishopMoves(rank, file);
    moveList.insert(moveList.end(), bishopMoves.begin(), bishopMoves.end());

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
    if (p.isType(square[rank-1*rankDirection][file], p.None)) {
        moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file));
    }

    // Two step first move
    if (p.isColor(activeColorBits, p.White)) {
        if (rank == 6) {
            if (p.isType(square[rank-2*rankDirection][file], p.None)) {
                moveList.push_back(createMoveStruct(rank, file, rank-2*rankDirection, file));
            }  
        }
    } else if (p.isColor(activeColorBits, p.Black)) {
        if (rank == 1) {
            if (p.isType(square[rank-2*rankDirection][file], p.None)) {
                moveList.push_back(createMoveStruct(rank, file, rank-2*rankDirection, file));
            }  
        }
    }

    // Capture
    if (file < 8 && !p.isType(square[rank-1*rankDirection][file+1], p.None) && !p.isColor(square[rank-1*rankDirection][file+1], activeColorBits)) {
        moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file+1));
    }
    if (file > 0 && !p.isType(square[rank-1*rankDirection][file-1], p.None) && !p.isColor(square[rank-1*rankDirection][file-1], activeColorBits)) {
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
