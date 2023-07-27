#include "helperFunctions.h"
#include "piece.h"

using namespace std;

vector<string> convertFen(string fenPP) {
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

            f.movesMade.push_back(s);
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

void setPiece(int rank, int file, char pieceChar, vector< vector<bitset<5> > > &square) {
    piece::Piece Piece;
    bool whitePiece = isupper(pieceChar);
    switch (tolower(pieceChar)) {
        case 'k':
            square[rank][file] = whitePiece ? Piece.White | Piece.King : Piece.Black | Piece.King;
            break;
        case 'q':
            square[rank][file] = whitePiece ? Piece.White | Piece.Queen : Piece.Black | Piece.Queen;
            break;
        case 'r':
            square[rank][file] = whitePiece ? Piece.White | Piece.Rook : Piece.Black | Piece.Rook;
            break;
        case 'b':
            square[rank][file] = whitePiece ? Piece.White | Piece.Bishop : Piece.Black | Piece.Bishop;
            break;
        case 'n':
            square[rank][file] = whitePiece ? Piece.White | Piece.Knight : Piece.Black | Piece.Knight;
            break;
        case 'p':
            square[rank][file] = whitePiece ? Piece.White | Piece.Pawn : Piece.Black | Piece.Pawn;
            break;
    }

    return;
}

int positionRank(string squarePos) {
    assert (squarePos.length() == 2);
    int rank = squarePos[1] - '0';

    return 8-rank;
}

int positionFile(string squarePos) {
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
