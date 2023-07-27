#pragma once
#include "fen.h"
#include <_ctype.h>
#include <cctype>
#include <iostream>
#include <string>
#include <bitset>
#include <cstddef>

using namespace std;

vector<string> convertFen(string fen);

void setFen(int fenIndex, fen &f, string s);

fen parsePosition(string positionString);

void setPiece(int rank, int file, char pieceChar, vector< vector<bitset<5> > > &square);

int positionToRank(string squarePos);

int positionToFile(string squarePos);

string indexToPosition(int rank, int file);

bool validPieceChar(char pieceChar);

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

vector< vector< numSquaresStruct > > numSquaresToEdges();

struct moveStruct {
    string startSquare;
    string endSquare;
};

string moveStructToMoveString(moveStruct m);

void printMoveList(vector< moveStruct > moveList);
