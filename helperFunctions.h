#pragma once
#include "fen.h"
#include <_ctype.h>
#include <cctype>
#include <iostream>
#include <string>
#include <bitset>
#include <cstddef>

using namespace std;

struct numSquaresStruct {
    int north;
    int northEast;
    int east;
    int southEast;
    int south;
    int southWest;
    int west;
    int northWest;
};

struct moveStruct {
    string startSquare;
    string endSquare;
};

// Convers a FEN piece placement string to a vector, with each element representing
// the pieces on a single rank of the chess board.
// fenPP: The FEN piece placement string.
// Returns a vector with each element representing the pieces on a single rank of
// the chess board.
vector<string> convertFenPP(string fenPP);

void setFen(int fenIndex, fen &f, string s);

fen parsePosition(string positionString);

// TODO: Make this a method on board object in stead of helper function!
void setPiece(int rank, int file, char pieceChar, vector< vector<bitset<5> > > &square);

// Gets the rank index from a position string.
// squarePos: The position string of a square.
// Returns the rank index of the square
int positionToRank(string squarePos);

// Gets the file index from a position string.
// squarePos: The position string of a square.
// Returns the file index of the square
int positionToFile(string squarePos);

// Convers a board square to its string position representation.
// rank: The rank index.
// file: The file index.
// Returns the string position of thr rank- and file index.
string indexToPosition(int rank, int file);

// Checks if a character is a valid character representing one of the six pieces.
// pieceChar: The character that is to be checked.
// Returns true if valid, false otherwise.
bool validPieceChar(char pieceChar);

vector< vector< numSquaresStruct > > numSquaresToEdges();

// Creates a structure representing a move.
// beginRank: The rank index of the current rank.
// beginFile: The file index of the current file.
// endRank: The rank index of the new rank.
// endFile: The rank index of the new file.
// Returns a struct representing the move.
moveStruct createMoveStruct(int beginRank, int beginFile, int endRank, int endFile);
// Creates a structure representing a move with a pawn promotion.
// beginRank: The rank index of the current rank.
// beginFile: The file index of the current file.
// endRank: The rank index of the new rank.
// endFile: The rank index of the new file.
// promotion: The piece that the pawn will be promoted to. (Q, R, B or N).
// Returns a struct representing the move.
moveStruct createMoveStructPromotion(int beginRank, int beginFile, int endRank, int endFile, string promotion);

// Converts a moveStruct to a string representing the move.
// m: The moveStruct
// Returns a string representing the move.
string moveStructToMoveString(moveStruct m);

// Prints all moves to stdout.
// moveList: The list with moves to be printed.
void printMoveList(vector< moveStruct > moveList);

// Checks if a square is legal (within a 8x8 grid).
// rank: The rank index of the square to be checked.
// file: The file index of the square to be checked.
// Returns true if legal, false otherwise.
bool isLegalSquare(int rank, int file);
