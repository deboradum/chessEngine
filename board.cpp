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

#include <chrono>
using namespace std::chrono;

using namespace std;
using namespace board;

void Board::setupBoard(fen f) {
    auto start = high_resolution_clock::now();

    setupBoardLayout(f.piecePlacement);
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
    for (string move : f.movesMade) {
        if (!move.substr(0, 2).compare("a8") && p.isColor(activeColorBits, p.Black) ||
            !move.substr(0, 2).compare("a1") && p.isColor(activeColorBits, p.White)) qRookMoved = true;
        if (!move.substr(0, 2).compare("h8") && p.isColor(activeColorBits, p.Black) ||
            !move.substr(0, 2).compare("h1") && p.isColor(activeColorBits, p.White)) kRookMoved = true;
        if (!move.substr(0, 2).compare("e8") && p.isColor(activeColorBits, p.Black) ||
            !move.substr(0, 2).compare("e1") && p.isColor(activeColorBits, p.White)) kingMoved = true;
        makeMove(move);
        movesMade.push_back(move);
    }
    attackedSquares = generateAttackedSquares();
    generateKingAttackLines();
    legalMoves = generateMoves();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken to setup board: " << duration.count() << " microseconds" << endl;
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

void Board::setupBoardLayout(string piecePlacement) {
    square = vector< vector<bitset<5> > >(8, vector < bitset<5> >(8, {0}));

    int rank = 0;
    for (string line : convertFenPP(piecePlacement)) {
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
        if (isCastlingMove(move)) {
            if (!move.compare("e1c1")) {
                square[7][3] = square[7][0];
                square[7][0] = EMPTY_SQUARE;
            } else if (!move.compare("e1g1")) {
                square[7][5] = square[7][7];
                square[7][7] = EMPTY_SQUARE;
            } else if (!move.compare("e8c8")) {
                square[0][3] = square[0][0];
                square[0][0] = EMPTY_SQUARE;
            } else if (!move.compare("e8g8")) {
                square[0][5] = square[0][7];
                square[0][7] = EMPTY_SQUARE;
            }
        }
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
    bool resolveCheck = inCheck() ? true : false;

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bitset<5> piece = square[rank][file];

            if (!p.isColor(piece, activeColorBits)) continue;

            if (p.isType(piece, p.King)) {
                vector < moveStruct> kingMoves = generateKingMoves(rank, file);
                moveList.insert(moveList.end(), kingMoves.begin(), kingMoves.end());
            } else if (p.isType(piece, p.Queen)) {
                vector < moveStruct> queenMoves = generateQueenMoves(rank, file, resolveCheck);
                moveList.insert(moveList.end(), queenMoves.begin(), queenMoves.end());
            } else if (p.isType(piece, p.Rook)) {
                vector < moveStruct> rookMoves = generateRookMoves(rank, file, resolveCheck);
                moveList.insert(moveList.end(), rookMoves.begin(), rookMoves.end());
            } else if (p.isType(piece, p.Bishop)) {
                vector < moveStruct> bishopMoves = generateBishopMoves(rank, file, resolveCheck);
                moveList.insert(moveList.end(), bishopMoves.begin(), bishopMoves.end());
            } else if (p.isType(piece, p.Knight)) {
                vector < moveStruct> knightMoves = generateKnightMoves(rank, file, resolveCheck);
                moveList.insert(moveList.end(), knightMoves.begin(), knightMoves.end());
            } else if (p.isType(piece, p.Pawn)) {
                vector < moveStruct> pawnMoves = generatePawnMoves(rank, file, resolveCheck);
                moveList.insert(moveList.end(), pawnMoves.begin(), pawnMoves.end());
            } else {
                continue;
            }
        }
    }
    if (castlingRights != "-") {
        vector < moveStruct> castlingMoves = generateCastlingMoves();
        moveList.insert(moveList.end(), castlingMoves.begin(), castlingMoves.end());
    } else {
        cout << "No castling available!"<< endl;
    }

    return moveList;
}

void Board::generateKingAttackLines() {
    string kingPos = getKingPos(activeColorBits);
    int kingRank = positionToRank(kingPos);
    int kingFile = positionToFile(kingPos);
    numSquaresStruct s2e = SquaresToEdge[kingRank][kingFile];
    isKingAttackLine[kingRank][kingFile] = true;
    setVerticalAttackInfo(kingRank, kingFile, s2e);
    setHorizontalAttackInfo(kingRank, kingFile, s2e);
    setDiagonalAttackInfo(kingRank, kingFile, s2e);
}

void Board::setVerticalAttackInfo(int rank, int file, numSquaresStruct s2e) {
    bool friendlyPieceFound = false;
    for (int rankOffset = 1; rankOffset <= s2e.north; rankOffset++) {
        if (!isEnemy(rank-rankOffset, file) && !isEmptySquare(rank-rankOffset, file)) {
            if (friendlyPieceFound) break;
            friendlyPieceFound = true;
        }
        if (isEnemy(rank-rankOffset, file) && !isEmptySquare(rank-rankOffset, file)) {
            if (friendlyPieceFound) {
                for (int rankOffset2 = 1; rankOffset2 <= rankOffset; rankOffset2++) {
                    isKingPinnedLine[rank-rankOffset2][file] = true;
                }
            } else {
                for (int rankOffset2 = 1; rankOffset2 <= rankOffset; rankOffset2++) {
                    isKingAttackLine[rank-rankOffset2][file] = true;
                }
                break;
            }
            break;
        }
    }
    friendlyPieceFound = false;
    for (int rankOffset = 1; rankOffset <= s2e.south; rankOffset++) {
        if (!isEnemy(rank+rankOffset, file) && !isEmptySquare(rank+rankOffset, file)) {
            if (friendlyPieceFound) break;
            friendlyPieceFound = true;
        }
        if (isEnemy(rank+rankOffset, file) && !isEmptySquare(rank+rankOffset, file)) {
            if (friendlyPieceFound) {
                for (int rankOffset2 = 1; rankOffset2 <= rankOffset; rankOffset2++) {
                    isKingPinnedLine[rank+rankOffset2][file] = true;
                }
            } else {
                for (int rankOffset2 = 1; rankOffset2 <= rankOffset; rankOffset2++) {
                    isKingAttackLine[rank+rankOffset2][file] = true;
                }
                break;
            }
            break;
        }
    }
}

void Board::setHorizontalAttackInfo(int rank, int file, numSquaresStruct s2e) {
    bool friendlyPieceFound = false;
    for (int fileOffset = 1; fileOffset <= s2e.west; fileOffset++) {
        if (!isEnemy(rank, file-fileOffset) && !isEmptySquare(rank, file-fileOffset)) {
            if (friendlyPieceFound) break;
            friendlyPieceFound = true;
        }
        if (isEnemy(rank, file-fileOffset) && !isEmptySquare(rank, file-fileOffset)) {
            if (friendlyPieceFound) {
                for (int fileOffset2 = 1; fileOffset2 <= fileOffset; fileOffset2++) {
                    isKingPinnedLine[rank][file-fileOffset2] = true;
                }
            } else {
                for (int fileOffset2 = 1; fileOffset2 <= fileOffset; fileOffset2++) {
                    isKingAttackLine[rank][file-fileOffset2] = true;
                }
                break;
            }
            break;
        }
    }
    friendlyPieceFound = false;
    for (int fileOffset = 1; fileOffset <= s2e.west; fileOffset++) {
        if (!isEnemy(rank, file+fileOffset) && !isEmptySquare(rank, file+fileOffset)) {
            if (friendlyPieceFound) break;
            friendlyPieceFound = true;
        }
        if (isEnemy(rank, file+fileOffset) && !isEmptySquare(rank, file+fileOffset)) {
            if (friendlyPieceFound) {
                for (int fileOffset2 = 1; fileOffset2 <= fileOffset; fileOffset2++) {
                    isKingPinnedLine[rank][file+fileOffset2] = true;
                }
            } else {
                for (int fileOffset2 = 1; fileOffset2 <= fileOffset; fileOffset2++) {
                    isKingAttackLine[rank][file+fileOffset2] = true;
                }
                break;
            }
            break;
        }
    }
}

void Board::setDiagonalAttackInfo(int rank, int file, numSquaresStruct s2e) {
    bool friendlyPieceFound = false;
    for (int offset = 1; offset <= s2e.northEast; offset++) {
        if (!isEnemy(rank-offset, file+offset) && !isEmptySquare(rank-offset, file+offset)) {
            if (friendlyPieceFound) break;
            friendlyPieceFound = true;
        }
        if (isEnemy(rank-offset, file+offset) && !isEmptySquare(rank-offset, file+offset)) {
            if (friendlyPieceFound) {
                for (int offset2 = 1; offset2 <= offset; offset2++) {
                    isKingPinnedLine[rank-offset2][file+offset2] = true;
                }
            } else {
                for (int offset2 = 1; offset2 <= offset; offset2++) {
                    isKingAttackLine[rank-offset2][file+offset2] = true;
                }
                break;
            }
            break;
        }
    }
    friendlyPieceFound = false;
    for (int offset = 1; offset <= s2e.southEast; offset++) {
        if (!isEnemy(rank+offset, file+offset) && !isEmptySquare(rank+offset, file+offset)) {
            if (friendlyPieceFound) break;
            friendlyPieceFound = true;
        }
        if (isEnemy(rank+offset, file+offset) && !isEmptySquare(rank+offset, file+offset)) {
            if (friendlyPieceFound) {
                for (int offset2 = 1; offset2 <= offset; offset2++) {
                    isKingPinnedLine[rank+offset2][file+offset2] = true;
                }
            } else {
                for (int offset2 = 1; offset2 <= offset; offset2++) {
                    isKingAttackLine[rank+offset2][file+offset2] = true;
                }
                break;
            }
            break;
        }
    }
    friendlyPieceFound = false;
    for (int offset = 1; offset <= s2e.southWest; offset++) {
        if (!isEnemy(rank+offset, file-offset) && !isEmptySquare(rank+offset, file-offset)) {
            if (friendlyPieceFound) break;
            friendlyPieceFound = true;
        }
        if (isEnemy(rank+offset, file-offset) && !isEmptySquare(rank+offset, file-offset)) {
            if (friendlyPieceFound) {
                for (int offset2 = 1; offset2 <= offset; offset2++) {
                    isKingPinnedLine[rank+offset2][file-offset2] = true;
                }
            } else {
                for (int offset2 = 1; offset2 <= offset; offset2++) {
                    isKingAttackLine[rank+offset2][file-offset2] = true;
                }
                break;
            }
            break;
        }
    }
    friendlyPieceFound = false;
    for (int offset = 1; offset <= s2e.northWest; offset++) {
        if (!isEnemy(rank-offset, file-offset) && !isEmptySquare(rank-offset, file-offset)) {
            if (friendlyPieceFound) break;
            friendlyPieceFound = true;
        }
        if (isEnemy(rank-offset, file-offset) && !isEmptySquare(rank-offset, file-offset)) {
            if (friendlyPieceFound) {
                for (int offset2 = 1; offset2 <= offset; offset2++) {
                    isKingPinnedLine[rank-offset2][file-offset2] = true;
                }
            } else {
                for (int offset2 = 1; offset2 <= offset; offset2++) {
                    isKingAttackLine[rank-offset2][file-offset2] = true;
                }
                break;
            }
            break;
        }
    }
}

vector< moveStruct > Board::generateCastlingMoves() {
    cout << "Castling available: " << castlingRights << endl;
    if (p.isColor(activeColorBits, p.White)) {
        return generateCastlingMovesW();
    } else {
        return generateCastlingMovesB();
    }
}

vector< moveStruct > Board::generateCastlingMovesW() {
    vector< moveStruct > moveList;
    if (castlingRights.find("Q") != string::npos && queenCastleLegalW()) {
        moveList.push_back(createMoveStruct(7, 4, 7, 2));
    }
    if (castlingRights.find("K") != string::npos && kingCastleLegalW()) {
        moveList.push_back(createMoveStruct(7, 4, 7, 6));
    }

    return moveList;
}

vector< moveStruct > Board::generateCastlingMovesB() {
    vector< moveStruct > moveList;
    if (castlingRights.find("q") != string::npos && queenCastleLegalB()) {
        moveList.push_back(createMoveStruct(0, 4, 0, 2));
    }
    if (castlingRights.find("k") != string::npos && kingCastleLegalB()) {
        moveList.push_back(createMoveStruct(0, 4, 0, 6));
    }

    return moveList;
}

bool Board::kingCastleLegalW() {
    if (kingMoved) return false;
    if (kRookMoved) return false;
    if (square[7][7] != (p.White | p.Rook)) return false;
    if (!p.isType(square[7][6], p.None) || !p.isType(square[7][5], p.None)) return false;
    if (isPositionAttacked(7, 6) || isPositionAttacked(7, 5)) return false;

    return true;
}

bool Board::queenCastleLegalW() {
    if (kingMoved) return false;
    if (kRookMoved) return false;
    if (square[7][0] != (p.White | p.Rook)) return false;
    if (!p.isType(square[7][1], p.None) || !p.isType(square[7][2], p.None) || !p.isType(square[7][3], p.None)) return false;
    if (isPositionAttacked(7, 1) || isPositionAttacked(7, 2) || isPositionAttacked(7, 3)) return false;

    return true;
}

bool Board::kingCastleLegalB() {
    if (kingMoved) return false;
    if (kRookMoved) return false;
    if (square[0][7] != (p.White | p.Rook)) return false;
    if (!p.isType(square[0][6], p.None) || !p.isType(square[0][5], p.None)) return false;
    if (isPositionAttacked(0, 6) || isPositionAttacked(0, 5)) return false;

    return true;
}

bool Board::queenCastleLegalB() {
    if (kingMoved) return false;
    if (kRookMoved) return false;
    if (square[0][0] != (p.White | p.Rook)) return false;
    if (!p.isType(square[0][1], p.None) || !p.isType(square[0][2], p.None) || !p.isType(square[0][3], p.None)) return false;
    if (isPositionAttacked(0, 1) || isPositionAttacked(0, 2) || isPositionAttacked(0, 3)) return false;

    return true;
}

vector< moveStruct > Board::generateKingMoves(int rank, int file) {
    vector< moveStruct > moveList;
    // Move north
    if (isLegalSquare(rank-1, file) && (isEmptySquare(rank-1, file) || isEnemy(rank-1, file))) {
        // If found move is not an attacked square.
        if (!isPositionAttacked(rank-1, file)) {
            moveList.push_back(createMoveStruct(rank, file, rank-1, file));
        }
    }
    // Move north east
    if (isLegalSquare(rank-1, file+1) && (isEmptySquare(rank-1, file+1) || isEnemy(rank-1, file+1))) {
        // If found move is not an attacked square.
        if (!isPositionAttacked(rank-1, file+1)) {
            moveList.push_back(createMoveStruct(rank, file, rank-1, file+1));
        }
    }
    // Move east
    if (isLegalSquare(rank, file+1) && (isEmptySquare(rank, file+1) || isEnemy(rank, file+1))) {
        // If found move is not an attacked square.
        if (!isPositionAttacked(rank, file+1)) {
            moveList.push_back(createMoveStruct(rank, file, rank, file+1));
        }
    }
    // Move south east
    if (isLegalSquare(rank+1, file+1) && (isEmptySquare(rank+1, file+1) || isEnemy(rank+1, file+1))) {
        // If found move is not an attacked square.
        if (!isPositionAttacked(rank+1, file+1)) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file+1));
        }
    }
    // Move south
    if (isLegalSquare(rank+1, file) && (isEmptySquare(rank+1, file) || isEnemy(rank+1, file))) {
        // If found move is not an attacked square.
        if (!isPositionAttacked(rank+1, file)) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file));
        }
    }
    // Move south west
    if (isLegalSquare(rank+1, file-1) && (isEmptySquare(rank+1, file-1) || isEnemy(rank+1, file-1))) {
        // If found move is not an attacked square.
        if (!isPositionAttacked(rank+1, file-1)) {
            moveList.push_back(createMoveStruct(rank, file, rank+1, file-1));
        }
    }
    // Move west
    if (isLegalSquare(rank, file-1) && (isEmptySquare(rank, file-1) || isEnemy(rank, file-1))) {
        // If found move is not an attacked square.
        if (!isPositionAttacked(rank, file-1)) {
            moveList.push_back(createMoveStruct(rank, file, rank, file-1));
        }
    }
    // Move north west
    if (isLegalSquare(rank-1, file-1) && (isEmptySquare(rank-1, file-1) || isEnemy(rank-1, file-1))) {
        // If found move is not an attacked square.
        if (!isPositionAttacked(rank-1, file-1)) {
            moveList.push_back(createMoveStruct(rank, file, rank-1, file-1));
        }
    }

    return moveList;
}

vector< moveStruct > Board::generateQueenMoves(int rank, int file, bool resolveCheck) {
    vector < moveStruct> moveList = generateRookMoves(rank, file, resolveCheck);
    vector < moveStruct> bishopMoves = generateBishopMoves(rank, file, resolveCheck);
    moveList.insert(moveList.end(), bishopMoves.begin(), bishopMoves.end());

    return moveList;
}

vector< moveStruct > Board::generateRookMoves(int rank, int file, bool resolveCheck) {
    vector< moveStruct > moveList;
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].north; rankOffset++) {
        int attackRank = rank-rankOffset;
        int attackFile = file;
        if (!isEmptySquare(attackRank, attackFile)) {
            if (isEnemy(attackRank, attackFile)) {
                if (resolveCheck) {
                    if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                } else {
                    if (isKingPinnedLine[rank][file]) {
                        if (isKingPinnedLine[attackRank][attackFile]) {
                            moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                        }
                    } else {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                }
            }
            break;
        }
        if (resolveCheck) {
            if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if (isKingPinnedLine[attackRank][attackFile]) {
                    moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        }
    }
    for (int rankOffset = 1; rankOffset <= SquaresToEdge[rank][file].south; rankOffset++) {
        int attackRank = rank+rankOffset;
        int attackFile = file;
        if (!isEmptySquare(attackRank, attackFile)) {
            if (isEnemy(attackRank, attackFile)) {
                if (resolveCheck) {
                    if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                } else {
                    if (isKingPinnedLine[rank][file]) {
                        if (isKingPinnedLine[attackRank][attackFile]) {
                            moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                        }
                    } else {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                }
            }
            break;
        }
        if (resolveCheck) {
            if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if (isKingPinnedLine[attackRank][attackFile]) {
                    moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        }
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].east; fileOffset++) {
        int attackRank = rank;
        int attackFile = file + fileOffset;
        if (!isEmptySquare(attackRank, attackFile)) {
            if (isEnemy(attackRank, attackFile)) {
                if (resolveCheck) {
                    if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                } else {
                    if (isKingPinnedLine[rank][file]) {
                        if (isKingPinnedLine[attackRank][attackFile]) {
                            moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                        }
                    } else {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                }
            }
            break;
        }
        if (resolveCheck) {
            if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if (isKingPinnedLine[attackRank][attackFile]) {
                    moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        }
    }
    for (int fileOffset = 1; fileOffset <= SquaresToEdge[rank][file].west; fileOffset++) {
        int attackRank = rank;
        int attackFile = file - fileOffset;
        if (!isEmptySquare(attackRank, attackFile)) {
            if (isEnemy(attackRank, attackFile)) {
                if (resolveCheck) {
                    if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                } else {
                    if (isKingPinnedLine[rank][file]) {
                        if (isKingPinnedLine[attackRank][attackFile]) {
                            moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                        }
                    } else {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                }
            }
            break;
        }
        if (resolveCheck) {
            if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if (isKingPinnedLine[attackRank][attackFile]) {
                    moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        }
    }

    return moveList;
}

vector< moveStruct > Board::generateBishopMoves(int rank, int file, bool resolveCheck) {
    vector< moveStruct > moveList;
    for (int offset = 1; offset <= SquaresToEdge[rank][file].northEast; offset++) {
        int attackRank = rank - offset;
        int attackFile = file + offset;
        if (!isEmptySquare(attackRank, attackFile)) {
            if (isEnemy(attackRank, attackFile)) {
                if (resolveCheck) {
                    if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                } else {
                    if (isKingPinnedLine[rank][file]) {
                        if (isKingPinnedLine[attackRank][attackFile]) {
                            moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                        }
                    } else {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                }
            }
            break;
        }
        if (resolveCheck) {
            if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if (isKingPinnedLine[attackRank][attackFile]) {
                    moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        }
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southEast; offset++) {
        int attackRank = rank + offset;
        int attackFile = file + offset;
        if (!isEmptySquare(attackRank, attackFile)) {
            if (isEnemy(attackRank, attackFile)) {
                if (resolveCheck) {
                    if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                } else {
                    if (isKingPinnedLine[rank][file]) {
                        if (isKingPinnedLine[attackRank][attackFile]) {
                            moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                        }
                    } else {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                }
            }
            break;
        }
        if (resolveCheck) {
            if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if (isKingPinnedLine[attackRank][attackFile]) {
                    moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        }
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].southWest; offset++) {
        int attackRank = rank + offset;
        int attackFile = file - offset;
        if (!isEmptySquare(attackRank, attackFile)) {
            if (isEnemy(attackRank, attackFile)) {
                if (resolveCheck) {
                    if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                } else {
                    if (isKingPinnedLine[rank][file]) {
                        if (isKingPinnedLine[attackRank][attackFile]) {
                            moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                        }
                    } else {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                }
            }
            break;
        }
        if (resolveCheck) {
            if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if (isKingPinnedLine[attackRank][attackFile]) {
                    moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        }
    }
    for (int offset = 1; offset <= SquaresToEdge[rank][file].northWest; offset++) {
        int attackRank = rank - offset;
        int attackFile = file - offset;
        if (!isEmptySquare(attackRank, attackFile)) {
            if (isEnemy(attackRank, attackFile)) {
                if (resolveCheck) {
                    if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                } else {
                    if (isKingPinnedLine[rank][file]) {
                        if (isKingPinnedLine[attackRank][attackFile]) {
                            moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                        }
                    } else {
                        moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                    }
                }
            }
            break;
        }
        if (resolveCheck) {
            if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if (isKingPinnedLine[attackRank][attackFile]) {
                    moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, attackRank, attackFile));
            }
        }
    }

    return moveList;
}

vector< moveStruct > Board::generateKnightMoves(int rank, int file, bool resolveCheck) {
    vector< moveStruct > moveList;

    if (isLegalSquare(rank+1, file+2) && (isEmptySquare(rank+1, file+2) || isEnemy(rank+1, file+2))) {
        if (resolveCheck) {
            if (isKingAttackLine[rank+1][file+2] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, rank+1, file+2));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if(isKingPinnedLine[rank+1][file+2]) {
                    moveList.push_back(createMoveStruct(rank, file, rank+1, file+2));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, rank+1, file+2));
            }
        }

    }
    if (isLegalSquare(rank+1, file-2) && (isEmptySquare(rank+1, file-2) || isEnemy(rank+1, file-2))) {
        if (resolveCheck) {
            if (isKingAttackLine[rank+1][file-2] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, rank+1, file-2));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if(isKingPinnedLine[rank+1][file-2]) {
                    moveList.push_back(createMoveStruct(rank, file, rank+1, file-2));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, rank+1, file-2));
            }
        }
    }
    if (isLegalSquare(rank-1, file+2) && (isEmptySquare(rank-1, file+2) || isEnemy(rank-1, file+2))) {
        if (resolveCheck) {
            if (isKingAttackLine[rank-1][file+2] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, rank-1, file+2));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if(isKingPinnedLine[rank-1][file+2]) {
                    moveList.push_back(createMoveStruct(rank, file, rank-1, file+2));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, rank-1, file+2));
            }
        }
    }
    if (isLegalSquare(rank-1, file-2) && (isEmptySquare(rank-1, file-2) || isEnemy(rank-1, file-2))) {
        if (resolveCheck) {
            if (isKingAttackLine[rank-1][file-2] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, rank-1, file-2));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if(isKingPinnedLine[rank-1][file-2]) {
                    moveList.push_back(createMoveStruct(rank, file, rank-1, file-2));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, rank-1, file-2));
            }
        }
    }
    if (isLegalSquare(rank+2, file+1) && (isEmptySquare(rank+2, file+1) || isEnemy(rank+2, file+1))) {
        if (resolveCheck) {
            if (isKingAttackLine[rank+2][file+1] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, rank+2, file+1));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if(isKingPinnedLine[rank+2][file+1]) {
                    moveList.push_back(createMoveStruct(rank, file, rank+2, file+1));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, rank+2, file+1));
            }
        }
    }
    if (isLegalSquare(rank+2, file-1) && (isEmptySquare(rank+2, file-1) || isEnemy(rank+2, file-1))) {
        if (resolveCheck) {
            if (isKingAttackLine[rank+2][file-1] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, rank+2, file-1));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if(isKingPinnedLine[rank+2][file-1]) {
                    moveList.push_back(createMoveStruct(rank, file, rank+2, file-1));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, rank+2, file-1));
            }
        }
    }
    if (isLegalSquare(rank-2, file+1) && (isEmptySquare(rank-2, file+1) || isEnemy(rank-2, file+1))) {
        if (resolveCheck) {
            if (isKingAttackLine[rank-2][file+1] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, rank-2, file+1));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if(isKingPinnedLine[rank-2][file+1]) {
                    moveList.push_back(createMoveStruct(rank, file, rank-2, file+1));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, rank-2, file+1));
            }
        }
    }
    if (isLegalSquare(rank-2, file-1) && (isEmptySquare(rank-2, file-1) || isEnemy(rank-2, file-1))) {
        if (resolveCheck) {
            if (isKingAttackLine[rank-2][file-1] && !isKingPinnedLine[rank][file]) {
                moveList.push_back(createMoveStruct(rank, file, rank-2, file-1));
            }
        } else {
            if (isKingPinnedLine[rank][file]) {
                if(isKingPinnedLine[rank-2][file-1]) {
                    moveList.push_back(createMoveStruct(rank, file, rank-2, file-1));
                }
            } else {
                moveList.push_back(createMoveStruct(rank, file, rank-2, file-1));
            }
        }
    }

    return moveList;
}

vector< moveStruct > Board::generatePawnMoves(int rank, int file, bool resolveCheck) {
    vector< moveStruct > moveList;

    // Determine direction since Pawns can only move forward in a single direction.
    int rankDirection = -1;
    if (p.isColor(activeColorBits, p.White)) {
        rankDirection = 1;
    }

    // Single step
    if (isEmptySquare(rank-1*rankDirection, file)) {
        if ((p.isColor(activeColorBits, p.White) && rank == 1) || p.isColor(activeColorBits, p.Black) && rank == 6) {
            if (!isKingPinnedLine[rank][file]) {
                vector< moveStruct > promotionMoves = generatePromotionOptions(rank, file, rank-1*rankDirection, file, resolveCheck);
                moveList.insert(moveList.end(), promotionMoves.begin(), promotionMoves.end());
            }
        } else {
            if (resolveCheck) {
                if (isKingAttackLine[rank-1*rankDirection][file] && !isKingPinnedLine[rank][file]) {
                    moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file));
                }
            } else {
                if (!isKingPinnedLine[rank][file]) moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file));
            }
        }
    }

    // Two step first move
    if (p.isColor(activeColorBits, p.White)) {
        if (rank == 6 && isEmptySquare(rank-2*rankDirection, file) && isEmptySquare(rank-1*rankDirection, file)) {
            if (resolveCheck) {
                if (isKingAttackLine[rank-2*rankDirection][file] && !isKingPinnedLine[rank][file]) {
                    moveList.push_back(createMoveStruct(rank, file, rank-2*rankDirection, file));
                }
            } else {
                if (!isKingPinnedLine[rank][file]) moveList.push_back(createMoveStruct(rank, file, rank-2*rankDirection, file));
            }
        }
    } else if (p.isColor(activeColorBits, p.Black)) {
        if (rank == 1 && isEmptySquare(rank-2*rankDirection, file) && isEmptySquare(rank-1*rankDirection, file)) {
            if (resolveCheck) {
                if (isKingAttackLine[rank-2*rankDirection][file] && !isKingPinnedLine[rank][file]) {
                    moveList.push_back(createMoveStruct(rank, file, rank-2*rankDirection, file));
                }
            } else {
                if (!isKingPinnedLine[rank][file]) moveList.push_back(createMoveStruct(rank, file, rank-2*rankDirection, file));
            }
        }
    }

    // Capture
    if (isLegalSquare(rank-1*rankDirection, file+1) && !isEmptySquare(rank-1*rankDirection, file+1) && isEnemy(rank-1*rankDirection, file+1)) {
        if ((p.isColor(activeColorBits, p.White) && rank == 1) || p.isColor(activeColorBits, p.Black) && rank == 6) {
            vector< moveStruct > promotionMoves = generatePromotionOptions(rank, file, rank-1*rankDirection, file+1, resolveCheck);
            moveList.insert(moveList.end(), promotionMoves.begin(), promotionMoves.end());
        } else {
            if (resolveCheck) {
                if (isKingAttackLine[rank-1*rankDirection][file+1] && !isKingPinnedLine[rank][file]) {
                    moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file+1));
                }
            } else {
                if (!isKingPinnedLine[rank][file]) moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file+1));
            }
        }
    }
    if (isLegalSquare(rank-1*rankDirection, file-1) && !isEmptySquare(rank-1*rankDirection, file-1) && isEnemy(rank-1*rankDirection, file-1)) {
        if ((p.isColor(activeColorBits, p.White) && rank == 1) || p.isColor(activeColorBits, p.Black) && rank == 6) {
            if (!isKingPinnedLine[rank][file]) {
                vector< moveStruct > promotionMoves = generatePromotionOptions(rank, file, rank-1*rankDirection, file-1, resolveCheck);
                moveList.insert(moveList.end(), promotionMoves.begin(), promotionMoves.end());
            }
        } else {
            if (resolveCheck) {
                if (isKingAttackLine[rank-1*rankDirection][file-1] && !isKingPinnedLine[rank][file]) {
                    moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file-1));
                }
            } else {
                if (!isKingPinnedLine[rank][file]) moveList.push_back(createMoveStruct(rank, file, rank-1*rankDirection, file-1));
            }
        }
    }

    // En Passant
    if (enPassantTargets.length() == 2) {
        if (positionToRank(enPassantTargets) == rank - 1*rankDirection && abs(positionToFile(enPassantTargets) - file) == 1) {
            int epRank = positionToRank(enPassantTargets);
            int epFile = positionToFile(enPassantTargets);
            if (resolveCheck) {
                if (isKingAttackLine[epRank][epFile] && !isKingPinnedLine[rank][file]) {
                    moveList.push_back(createMoveStruct(rank, file, epRank, epFile));
                }
            } else {
                if (!isKingPinnedLine[rank][file]) moveList.push_back(createMoveStruct(rank, file, epRank, epFile));
            }
        }
    }

    return moveList;
}

vector< moveStruct > Board::generatePromotionOptions(int beginRank, int beginFile, int endRank, int endFile, bool resolveCheck) {
    vector< moveStruct > moveList;
    if (resolveCheck) {
        if (isKingAttackLine[endRank][endFile] && !isKingPinnedLine[beginRank][beginFile]) {
            moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "Q"));
            moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "R"));
            moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "B"));
            moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "N"));
        }
    } else {
        moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "Q"));
        moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "R"));
        moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "B"));
        moveList.push_back(createMoveStructPromotion(beginRank, beginFile, endRank, endFile, "N"));
    }


    return moveList;
}

bool Board::isEnemy(int rank, int file) {
    return !p.isColor(square[rank][file], activeColorBits);
}

bool Board::isEmptySquare(int rank, int file) {
    return p.isType(square[rank][file], p.None);
}

bool Board::isPositionAttacked(int rank, int file) {
    return find(attackedSquares.begin(), attackedSquares.end(), indexToPosition(rank, file)) != attackedSquares.end();
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

string Board::generateBestMove() {
    if (!legalMoves.size()) {
        return "XX";
    }; // No moves should not be possible I dont think

    int randomIndex = rand() % legalMoves.size();
    moveStruct m = legalMoves[randomIndex];
    string move = moveStructToMoveString(m);

    return move;
}

bool Board::isCastlingMove(string move) {
    if (!move.compare("e1c1") || !move.compare("e1g1") ||
        !move.compare("e8c8") || !move.compare("e8g8")) {
            return true;
    }

    return false;
}
