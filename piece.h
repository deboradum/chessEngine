#pragma once
#include <_ctype.h>
#include <cctype>
#include <iostream>
#include <string>
#include <bitset>
#include <cstddef>

using namespace std;

namespace piece {
    class Piece {
    public:
        const bitset<5> None{0};
        const bitset<5> King{1};
        const bitset<5> Queen{2};
        const bitset<5> Rook{3};
        const bitset<5> Bishop{4};
        const bitset<5> Knight{5};
        const bitset<5> Pawn{6};

        const bitset<5> White{8};
        const bitset<5> Black{16};

        bool isType(bitset<5> queryPiece, bitset<5> targetPiece);

        bool isColor(bitset<5> queryPiece, bitset<5> targetPiece);
};
} // namespace piece
