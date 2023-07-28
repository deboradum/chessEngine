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
    attackedSquares = generateAttackedSquares();
    possibleMoves = generateMoves();
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
    // Normal move.
    if (move.length() == 4) {
        string start = move.substr(0, 2);
        string end = move.substr(2, 3);

        square[positionToRank(end)][positionToFile(end)] = square[positionToRank(start)][positionToFile(start)];
        square[positionToRank(start)][positionToFile(start)] = EMPTY_SQUARE;
    // In case of pawn promotion.
    } else if (move.length() == 5) {
        string start = move.substr(0, 2);
        string end = move.substr(2, 2);
        char promotedTo = move[4];

        square[positionToRank(start)][positionToFile(start)] = EMPTY_SQUARE;
        setPiece(positionToRank(end), positionToFile(end), promotedTo, square);
    }

    return;
}

vector< moveStruct > Board::generateMoves() {
    vector< moveStruct > moveList;
    if (inCheck()) {

    }

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
    vector < moveStruct> castlingMoves = generateCastlingMoves();
    moveList.insert(moveList.end(), castlingMoves.begin(), castlingMoves.end());

    return moveList;
}

vector< moveStruct > Board::generateCastlingMoves() {
    vector< moveStruct > moveList;

    if (castlingRights == "-") {
        cout << "No one can castle!" << endl;
    } else {
        cout << "Castling available: " << castlingRights << endl;
    }

    return moveList;
}

vector< moveStruct > Board::generateKingMoves(int rank, int file) {
    vector< moveStruct > moveList;
    // Move north
    if (isLegalSquare(rank-1, file) && (isEmptySquare(rank-1, file) || isEnemy(rank-1, file))) {
        // If found move is not an attacked square.
        if (find(attackedSquares.begin(), attackedSquares.end(), indexToPosition(rank-1, file)) == attackedSquares.end()) {
            moveList.push_back(createMoveStruct(rank, file, rank-1, file));
        }
    }
    // Move north east
    if (isLegalSquare(rank-1, file+1) && (isEmptySquare(rank-1, file+1) || isEnemy(rank-1, file+1))) {
        // If found move is not an attacked square.
        if (find(attackedSquares.begin(), attackedSquares.end(), indexToPosition(rank-1, file+1)) == attackedSquares.end()) {
            moveList.push_back(createMoveStruct(rank, file, rank-1, file+1));
        }
    }
    // Move east
    if (isLegalSquare(rank, file+1) && (isEmptySquare(rank, file+1) || isEnemy(rank, file+1))) {
        // If found move is not an attacked square.
        if (find(attackedSquares.begin(), attackedSquares.end(), indexToPosition(rank, file+1)) == attackedSquares.end()) {
            moveList.push_back(createMoveStruct(rank, file, rank, file+1));
        }
    }
    if (isLegalSquare(rank+1, file+1) && (isEmptySquare(rank+1, file+1) || isEnemy(rank+1, file+1))) {
        // If found move is not an attacked square.
        if (find(attackedSquares.begin(), attackedSquares.end(), indexToPosition(rank+1, file+1)) == attackedSquares.end()) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file+1));
        }
    }
    // Move south
    if (isLegalSquare(rank+1, file) && (isEmptySquare(rank+1, file) || isEnemy(rank+1, file))) {
        // If found move is not an attacked square.
        if (find(attackedSquares.begin(), attackedSquares.end(), indexToPosition(rank+1, file)) == attackedSquares.end()) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file));
        }
    }
    // Move south west
    if (isLegalSquare(rank+1, file-1) && (isEmptySquare(rank+1, file-1) || isEnemy(rank+1, file-1))) {
        // If found move is not an attacked square.
        if (find(attackedSquares.begin(), attackedSquares.end(), indexToPosition(rank+1, file-1)) == attackedSquares.end()) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file-1));
        }
    }
    // Move south west
    if (isLegalSquare(rank, file-1) && (isEmptySquare(rank, file-1) || isEnemy(rank, file-1))) {
        // If found move is not an attacked square.
        if (find(attackedSquares.begin(), attackedSquares.end(), indexToPosition(rank, file-1)) == attackedSquares.end()) {
            moveList.push_back(createMoveStruct(rank, file, rank, file-1));
        }
    }
    // Move north west
    if (isLegalSquare(rank-1, file-1) && (isEmptySquare(rank-1, file-1) || isEnemy(rank-1, file-1))) {
        // If found move is not an attacked square.
        if (find(attackedSquares.begin(), attackedSquares.end(), indexToPosition(rank-1, file-1)) == attackedSquares.end()) {
            moveList.push_back(createMoveStruct(rank, file, rank-1, file-1));
        }
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
        if ((p.isColor(activeColorBits, p.White) && rank == 1) || p.isColor(activeColorBits, p.Black) && rank == 6) {
            vector< moveStruct > promotionMoves = addPromotionOptions(rank, file, rank-1*rankDirection, file);
            moveList.insert(moveList.end(), promotionMoves.begin(), promotionMoves.end());
        } else {
            moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file));
        }
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
        if ((p.isColor(activeColorBits, p.White) && rank == 1) || p.isColor(activeColorBits, p.Black) && rank == 6) {
            vector< moveStruct > promotionMoves = addPromotionOptions(rank, file, rank-1*rankDirection, file+1);
            moveList.insert(moveList.end(), promotionMoves.begin(), promotionMoves.end());
        } else {
            moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file+1));
        }
    }
    if (isLegalSquare(rank-1*rankDirection, file-1) && !isEmptySquare(rank-1*rankDirection, file-1) && isEnemy(rank-1*rankDirection, file-1)) {
        if ((p.isColor(activeColorBits, p.White) && rank == 1) || p.isColor(activeColorBits, p.Black) && rank == 6) {
            vector< moveStruct > promotionMoves = addPromotionOptions(rank, file, rank-1*rankDirection, file-1);
            moveList.insert(moveList.end(), promotionMoves.begin(), promotionMoves.end());
        } else {
            moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file-1));
        }
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

vector< moveStruct > Board::addPromotionOptions(int beginRank, int beginFile, int endRank, int endFile) {
    vector< moveStruct > moveList;
    moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "Q"));
    moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "R"));
    moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "B"));
    moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "N"));

    return moveList;
}

bool Board::isEnemy(int rank, int file) {
    return !p.isColor(square[rank][file], activeColorBits);
}

bool Board::isEmptySquare(int rank, int file) {
    return p.isType(square[rank][file], p.None);
}

vector < string > Board::generateAttackedSquares() {
    vector < string > attackedSquareList;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bitset<5> piece = square[rank][file];
            if (p.isColor(piece, activeColorBits)) continue;

            if (p.isType(piece, p.King)) {
                vector < string > kingAttacks = generateKingAttacks(rank, file);
                attackedSquareList.insert(attackedSquareList.end(), kingAttacks.begin(), kingAttacks.end());
            } else if (p.isType(piece, p.Queen)) {
                vector < string > queenAttacks = generateQueenAttacks(rank, file);
                attackedSquareList.insert(attackedSquareList.end(), queenAttacks.begin(), queenAttacks.end());
            } else if (p.isType(piece, p.Rook)) {
                vector < string > rookAttacks = generateRookAttacks(rank, file);
                attackedSquareList.insert(attackedSquareList.end(), rookAttacks.begin(), rookAttacks.end());
            } else if (p.isType(piece, p.Bishop)) {
                vector < string > bishopAttacks = generateBishopAttacks(rank, file);
                attackedSquareList.insert(attackedSquareList.end(), bishopAttacks.begin(), bishopAttacks.end());
            } else if (p.isType(piece, p.Knight)) {
                vector < string > knightAttacks = generateKnightAttacks(rank, file);
                attackedSquareList.insert(attackedSquareList.end(), knightAttacks.begin(), knightAttacks.end());
            } else if (p.isType(piece, p.Pawn)) {
                vector < string > pawnAttacks = generatePawnAttacks(rank, file);
                attackedSquareList.insert(attackedSquareList.end(), pawnAttacks.begin(), pawnAttacks.end());
            } else {
                continue;
            }
        }
    }

    return attackedSquareList;
}

vector < string > Board::generateKingAttacks(int rank, int file) {
    vector < string > attackedSquaresList;
    // Move north
    if (isLegalSquare(rank-1, file)) {
        attackedSquaresList.push_back(indexToPosition(rank-1, file));
    }
    // Move north east
    if (isLegalSquare(rank-1, file+1)) {
        attackedSquaresList.push_back(indexToPosition(rank-1, file+1));
    }
    // Move east
    if (isLegalSquare(rank, file+1)) {
        attackedSquaresList.push_back(indexToPosition(rank, file+1));
    }
    // Move south east
    if (isLegalSquare(rank+1, file+1)) {
        attackedSquaresList.push_back(indexToPosition(rank+1, file+1));
    }
    // Move south
    if (isLegalSquare(rank+1, file)) {
        attackedSquaresList.push_back(indexToPosition(rank+1, file));
    }
    // Move south west
    if (isLegalSquare(rank+1, file-1)) {
        attackedSquaresList.push_back(indexToPosition(rank+1, file-1));
    }
    // Move south west
    if (isLegalSquare(rank, file-1)) {
        attackedSquaresList.push_back(indexToPosition(rank, file-1));
    }
    // Move north west
    if (isLegalSquare(rank-1, file-1)) {
        attackedSquaresList.push_back(indexToPosition(rank-1, file-1));
    }

    return attackedSquaresList;
}

vector< string > Board::generateQueenAttacks(int rank, int file) {
    vector < string> attackedSquaresList = generateRookAttacks(rank, file);
    vector < string> bishopAttacks = generateBishopAttacks(rank, file);
    attackedSquaresList.insert(attackedSquaresList.end(), bishopAttacks.begin(), bishopAttacks.end());

    return attackedSquaresList;
}

vector< string > Board::generateRookAttacks(int rank, int file) {
    vector < string > attackedSquaresList;
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].north; rankOffset++) {
        if (!isEmptySquare(rank-rankOffset, file)) {
            attackedSquaresList.push_back(indexToPosition(rank-rankOffset, file));
            break;
        }
        attackedSquaresList.push_back(indexToPosition(rank-rankOffset, file));
    }
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].south; rankOffset++) {
        if (!isEmptySquare(rank+rankOffset, file)) {
            attackedSquaresList.push_back(indexToPosition(rank+rankOffset, file));
            break;
        }
        attackedSquaresList.push_back(indexToPosition(rank+rankOffset, file));
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].east; fileOffset++) {
        if (!isEmptySquare(rank, file+fileOffset)) {
            attackedSquaresList.push_back(indexToPosition(rank, file+fileOffset));
            break;
        }
        attackedSquaresList.push_back(indexToPosition(rank, file+fileOffset));
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].west; fileOffset++) {
        if (!isEmptySquare(rank, file-fileOffset)) {
            attackedSquaresList.push_back(indexToPosition(rank, file-fileOffset));
            break;
        }
        attackedSquaresList.push_back(indexToPosition(rank, file-fileOffset));
    }

    return attackedSquaresList;
}

vector< string > Board::generateBishopAttacks(int rank, int file) {
    vector< string > attackedSquaresList;
    for (int offset = 1; offset <= SquaresToEdge[rank][file].northEast; offset++) {
        if (!isEmptySquare(rank-offset, file+offset)) {
            attackedSquaresList.push_back(indexToPosition(rank-offset, file+offset));
            break;
        }
        attackedSquaresList.push_back(indexToPosition(rank-offset, file+offset));
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southEast; offset++) {
        if (!isEmptySquare(rank+offset, file+offset)) {
            attackedSquaresList.push_back(indexToPosition(rank+offset, file+offset));
            break;
        }
        attackedSquaresList.push_back(indexToPosition(rank+offset, file+offset));
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southWest; offset++) {
        if (!isEmptySquare(rank+offset, file-offset)) {
            attackedSquaresList.push_back(indexToPosition(rank+offset, file-offset));
            break;
        }
        attackedSquaresList.push_back(indexToPosition(rank+offset, file-offset));
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].northWest; offset++) {
        if (!isEmptySquare(rank-offset, file-offset)) {
            attackedSquaresList.push_back(indexToPosition(rank-offset, file-offset));
            break;
        }
        attackedSquaresList.push_back(indexToPosition(rank-offset, file-offset));
    }

    return attackedSquaresList;
}

vector< string > Board::generateKnightAttacks(int rank, int file) {
    vector< string > attackedSquaresList;

    if (isLegalSquare(rank+1, file+2)) {
        attackedSquaresList.push_back(indexToPosition(rank+1, file+2));
    }
    if (isLegalSquare(rank+1, file-2)) {
        attackedSquaresList.push_back(indexToPosition(rank+1, file-2));
    }
    if (isLegalSquare(rank-1, file+2)) {
        attackedSquaresList.push_back(indexToPosition(rank-1, file+2));
    }
    if (isLegalSquare(rank-1, file-2)) {
        attackedSquaresList.push_back(indexToPosition(rank-1, file-2));
    }
    if (isLegalSquare(rank+2, file+1)) {
        attackedSquaresList.push_back(indexToPosition(rank+2, file+1));
    }
    if (isLegalSquare(rank+2, file-1)) {
        attackedSquaresList.push_back(indexToPosition(rank+2, file-1));
    }
    if (isLegalSquare(rank-2, file+1)) {
        attackedSquaresList.push_back(indexToPosition(rank-2, file+1));
    }
    if (isLegalSquare(rank-2, file-1)) {
        attackedSquaresList.push_back(indexToPosition(rank-2, file-1));
    }

    return attackedSquaresList;
}

vector< string > Board::generatePawnAttacks(int rank, int file) {
    vector < string > attackedSquaresList;
    // Determine direction since Pawns can only move forward in a single direction. 
    int rankDirection = 1;
    if (p.isColor(activeColorBits, p.White)) {
        rankDirection = -1;
    }
    // Capture
    if (isLegalSquare(rank-1*rankDirection, file+1)) {
        attackedSquaresList.push_back(indexToPosition(rank-1*rankDirection, file+1));
    }
    if (isLegalSquare(rank-1*rankDirection, file-1)) {
        attackedSquaresList.push_back(indexToPosition(rank-1*rankDirection, file-1));
    }

    return attackedSquaresList;
}

bool Board::inCheck() {
    string kingPos = getKingPos(activeColorBits);
    if (find(attackedSquares.begin(), attackedSquares.end(), kingPos) != attackedSquares.end()) {
        cout << "In check!" << endl;
    }
    return find(attackedSquares.begin(), attackedSquares.end(), kingPos) != attackedSquares.end();
}

string Board::getKingPos(bitset<5> color) {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (square[rank][file] == (color | p.King)) {
                return indexToPosition(rank, file);
            }
        }
    }
    return "";
}

string Board::generateMove() {
    if (!possibleMoves.size()) {
        return "XX";
    }; // No moves should not be possible I dont think

    int randomIndex = rand() % possibleMoves.size();
    moveStruct m = possibleMoves[randomIndex];
    string move = moveStructToMoveString(m);

    return move;
}
