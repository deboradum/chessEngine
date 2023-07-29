#include "helperFunctions.h"
#include "piece.h"

using namespace std;

vector<string> convertFenPP(string fenPP) {
    vector<string> result;
    string line = "";

    for (char& c : fenPP) {
        // Inserts current line to vector on delimiter.
        if (c == '/') {
            result.push_back(line);
            line = "";
            continue;
        }
        // Appends to current line.
        line.push_back(c);
    }
    // Inserts last line to vector.
    result.push_back(line);

    return result;
}

void setFen(int fenIndex, fen &f, string s) {
    switch (fenIndex) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            f.piecePlacement = s;
            break;
        case 3:
            f.activeColor = s;
            break;
        case 4:
            f.castlingRights = s;
            break;
        case 5:
            f.enPassantTargets = s;
            break;
        case 6:
            f.halfMoves = stoi(s);
            break;
        case 7:
            f.fullMoves = stoi(s);
            break;
        default:
            if (s == "moves") break;
            if (s.length() == 4 || s.length() == 5) f.movesMade.push_back(s);
            break;
        }
}

fen parsePosition(string positionString) {
    fen f;
    int fenIndex = 0;
    string fenElement = "";
    for (char& c : positionString) {
        /*  A received position consists of >= 8 parts:
            position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves .....
            The first two can be discarded.
        */
       // Inserts current line to vector on delimiter.
        if (c == ' ') {
            setFen(fenIndex, f, fenElement);
            fenElement = "";
            fenIndex++;
        } else {
             // Appends to current line.
            fenElement.push_back(c);
        }
    }
    setFen(fenIndex, f, fenElement);

    return f;
}

void setPiece(int rank, int file, char pieceChar, bitset<5> pieceColor, vector< vector<bitset<5> > > &square) {
    assert(validPieceChar(pieceChar));

    piece::Piece p;
    bool whitePiece = p.White == pieceColor;
    switch (tolower(pieceChar)) {
        case 'k':
            square[rank][file] = whitePiece ? p.White | p.King : p.Black | p.King;
            break;
        case 'q':
            square[rank][file] = whitePiece ? p.White | p.Queen : p.Black | p.Queen;
            break;
        case 'r':
            square[rank][file] = whitePiece ? p.White | p.Rook : p.Black | p.Rook;
            break;
        case 'b':
            square[rank][file] = whitePiece ? p.White | p.Bishop : p.Black | p.Bishop;
            break;
        case 'n':
            square[rank][file] = whitePiece ? p.White | p.Knight : p.Black | p.Knight;
            break;
        case 'p':
            square[rank][file] = whitePiece ? p.White | p.Pawn : p.Black | p.Pawn;
            break;
    }

    return;
}

int positionToRank(string squarePos) {
    assert (squarePos.length() == 2);
    int rank = squarePos[1] - '0';

    return 8-rank;
}

int positionToFile(string squarePos) {
    assert (squarePos.length() == 2);
    switch (squarePos[0]) {
    case 'a':
        return 0;
    case 'b':
        return 1;
    case 'c':
        return 2;
    case 'd':
        return 3;
    case 'e':
        return 4;
    case 'f':
        return 5;
    case 'g':
        return 6;
    case 'h':
        return 7;
    default:
        return -1;
    }
}

string indexToPosition(int rank, int file) {
    string pos = "";
    switch (file) {
    case 0:
        pos.push_back('a');
        break;
    case 1:
        pos.push_back('b');
        break;
    case 2:
        pos.push_back('c');
        break;
    case 3:
        pos.push_back('d');
        break;
    case 4:
        pos.push_back('e');
        break;
    case 5:
        pos.push_back('f');
        break;
    case 6:
        pos.push_back('g');
        break;
    case 7:
        pos.push_back('h');
        break;
    default:
        break;
    }

    pos.push_back(8-rank + '0');

    assert(pos.length() == 2);

    return pos;
}

bool validPieceChar(char pieceChar) {
    char lowerPieceChar = tolower(pieceChar);
    return lowerPieceChar == 'k' || lowerPieceChar == 'q' || lowerPieceChar == 'r' || lowerPieceChar == 'b' || lowerPieceChar == 'n' || lowerPieceChar == 'p';
}

vector< vector< numSquaresStruct > > numSquaresToEdges() {
    vector< vector< numSquaresStruct > > numSquares(8, vector< numSquaresStruct >(8));
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file <8; file++) {
            int north = rank;
            int south = 7-rank;
            int west = file;
            int east = 7-file;

            numSquaresStruct s;
            s.north = north;
            s.northEast = min(north, east);
            s.east = east;
            s.southEast = min(south, east);
            s.south = south;
            s.southWest = min(south, west);
            s.west = west;
            s.northWest = min(north, west);

            numSquares[rank][file] = s;
        }
    }

    return numSquares;
}

string moveStructToMoveString(moveStruct m) {
    return m.startSquare + m.endSquare;
}

void printMoveList(vector< moveStruct > moveList) {
    for (moveStruct m : moveList) {
        cout << m.startSquare << m.endSquare << " ";
    }
}

moveStruct createMoveStruct(int beginRank, int beginFile, int endRank, int endFile) {
    moveStruct m;
    m.startSquare = indexToPosition(beginRank, beginFile);
    m.endSquare = indexToPosition(endRank, endFile);

    return m;
}

moveStruct createMoveStructPromotion(int beginRank, int beginFile, int endRank, int endFile, string promotion) {
    moveStruct m;
    m.startSquare = indexToPosition(beginRank, beginFile);
    m.endSquare = indexToPosition(endRank, endFile) + promotion;

    return m;
}

bool isLegalSquare(int rank, int file) {
    return (0 <= rank && rank <= 7) && (0 <= file && file <= 7);
}

void addKnightMove(bool resolveCheck, int beginRank, int beginFile, int attackRank,
                   int attackFile, vector< moveStruct > &moveList,
                   vector<vector<bool>> isKingAttackLine, vector<vector<bool>> isKingPinnedLine) {
    if (resolveCheck) {
        if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[beginRank][beginFile]) {
            moveList.push_back(createMoveStruct(beginRank, beginFile, attackRank, attackFile));
        }
    } else {
        if (isKingPinnedLine[beginRank][beginFile]) {
            if(isKingPinnedLine[attackRank][attackFile]) {
                moveList.push_back(createMoveStruct(beginRank, beginFile, attackRank, attackFile));
            }
        } else {
            moveList.push_back(createMoveStruct(beginRank, beginFile, attackRank, attackFile));
        }
    }
}

void addSlidingMove(bool resolveCheck, int beginRank, int beginFile, int attackRank,
                   int attackFile, vector< moveStruct > &moveList,
                   vector<vector<bool>> isKingAttackLine, vector<vector<bool>> isKingPinnedLine) {
    if (resolveCheck) {
        if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[beginRank][beginFile]) {
            moveList.push_back(createMoveStruct(beginRank, beginFile, attackRank, attackFile));
        }
    } else {
        if (isKingPinnedLine[beginRank][beginFile]) {
            if (isKingPinnedLine[attackRank][attackFile]) {
                moveList.push_back(createMoveStruct(beginRank, beginFile, attackRank, attackFile));
            }
        } else {
            moveList.push_back(createMoveStruct(beginRank, beginFile, attackRank, attackFile));
        }
    }
}

void addPawnMove(bool resolveCheck, int beginRank, int beginFile, int attackRank,
                   int attackFile, vector< moveStruct > &moveList,
                   vector<vector<bool>> isKingAttackLine, vector<vector<bool>> isKingPinnedLine) {
    if (resolveCheck) {
        if (isKingAttackLine[attackRank][attackFile] && !isKingPinnedLine[beginRank][beginFile]) {
            moveList.push_back(createMoveStruct(beginRank, beginFile, attackRank, attackFile));
        }
    } else {
        if (!isKingPinnedLine[beginRank][beginFile]) {
            moveList.push_back(createMoveStruct(beginRank, beginFile, attackRank, attackFile));
        }
    }
}
