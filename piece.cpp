#include "piece.h"
#include <bitset>

using namespace std;
using namespace piece;

bool Piece::isType(bitset<5> queryPiece, bitset<5> targetPiece) {
    // Discards the color bits from the pieces.
    bitset<5> queryPieceNC = bitset<5> {"00111"} & queryPiece;
    bitset<5> targetPieceNC = bitset<5> {"00111"} & targetPiece;

    return queryPieceNC == targetPieceNC;
}
