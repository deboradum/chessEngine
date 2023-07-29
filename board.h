#pragma once
#include "fen.h"
#include "helperFunctions.h"
#include "piece.h"
#include <_ctype.h>
#include <cctype>
#include <iostream>
#include <string>
#include <bitset>
#include <cstddef>
#include <tuple>

using namespace std;

namespace board {
    class Board {
    public:
        // Piece object used for piece metadata.
        piece::Piece p;
        // 2D vector grid representing the chess board.
        vector<vector<bitset<5>>> square = vector< vector<bitset<5> > >(8, vector < bitset<5> >(8, {0}));
        // Color of the player whose turn it currently is. Can either be 'w' or
        // 'b'.
        string activeColor;
        // Color bitset of the player whose turn it currently is. Can either be
        // '01000' (white) or '10000' (black).
        bitset<5> activeColorBits;
        // String representing castle rights. See FEN notation for more info.
        string castlingRights;
        // String representing En Passant targets. See FEN notation for more
        // info.
        string enPassantTargets;
        // Number of half moves. See FEN notation for more info.
        int halfMoves;
        // Number of full moves. See FEN notation for more info.
        int fullMoves;

        // True if the king has moved, false otherwise. Used for determining
        // castle rights.
        bool kingMoved = false;
        // True if queen side rook has moved, false otherwise. Used for
        // determining castle rights.
        bool qRookMoved = false;
        // True if king side rook has moved, false otherwise. Used for
        // determining castle rights.
        bool kRookMoved = false;

        // Vector containing the moves made.
        vector<string> movesMade = {};
        // Vector containing the attacked square positions.
        vector<string> attackedSquares;
        // Vector containing all legal moves.
        vector<moveStruct> legalMoves;

        // 2d vector containing a struct holding the number of squares until the edge
        // in all eight directions.
        vector< vector< numSquaresStruct > > SquaresToEdge = numSquaresToEdges();
        // 2d vector containing a boolean value on whether or not the square is
        // in the line of a direct king attack by a queen, bishop or rook.
        vector< vector< bool > > isKingAttackLine = vector< vector< bool > >(8, vector< bool >(8, false));
        // 2d vector containing a boolean value on whether or not the square is
        // part of a pinned piece line. When a piece on this line is moved, a
        // direct attack on the king happens, thus making a move like this illegal.
        vector< vector< bool > > isKingPinnedLine = vector< vector< bool > >(8, vector< bool >(8, false));

        void setVerticalAttackInfo(int rank, int file, numSquaresStruct s2e);
        void setHorizontalAttackInfo(int rank, int file, numSquaresStruct s2e);
        void setDiagonalAttackInfo(int rank, int file, numSquaresStruct s2e);

        // Sets up board.
        // f: A fen struct containing all neccesary information about the position.
        void setupBoard(fen f);
        // Prints board to stdout.
        void printBoard();

        // Generates all legal moves.
        // Returns a vector containing all the legal moves.
        vector< moveStruct > generateMoves();
        // Generates the best move from all the possible legal moves.
        // Returns the best move found.
        string generateBestMove();
        // performs a move on the board.
        // move: The move that is to be performed.
        void makeMove(string move);
        // Checks if the player is currently in check.
        // Returns true if player is currently in check. False otherwise.
        bool inCheck();

    private:
        // Sets up the board layout.
        // f: A fen struct containing all neccesary information about the position.
        void setupBoardLayout(string piecePlacement);

        void setupBoardFen(fen f);

        void setupBoardMoves(fen f);

        void setupBoardAttackInfo();

        // Finds all squares that are currently attacked by the enemy.
        // Returns a vector containing all the squares that are attacked.
        vector< string > generateAttackedSquares();
        // Finds all squares the enemy king is currently attacking.
        // rank: The rank index of the king.
        // file: the file index of the king.
        // Returns a vector containing all the squares the king is currently attacking.
        vector< string > generateKingAttacks(int rank, int file);
        // Finds all squares the enemy queen is currently attacking.
        // rank: The rank index of the queen.
        // file: the file index of the queen.
        // Returns a vector containing all the squares the queen is currently attacking.
        vector< string > generateQueenAttacks(int rank, int file);
        // Finds all squares the enemy rook is currently attacking.
        // rank: The rank index of the rook.
        // file: the file index of the rook.
        // Returns a vector containing all the squares the rook is currently attacking.
        vector< string > generateRookAttacks(int rank, int file);
        // Finds all squares the enemy bishop is currently attacking.
        // rank: The rank index of the bishop.
        // file: the file index of the bishop.
        // Returns a vector containing all the squares the bishop is currently attacking.
        vector< string > generateBishopAttacks(int rank, int file);
        // Finds all squares the enemy knight is currently attacking.
        // rank: The rank index of the knight.
        // file: the file index of the knight.
        // Returns a vector containing all the squares the knight is currently attacking.
        vector< string > generateKnightAttacks(int rank, int file);
        // Finds all squares the enemy pawn is currently attacking.
        // rank: The rank index of the pawn.
        // file: the file index of the pawn.
        // Returns a vector containing all the squares the pawn is currently attacking.
        vector< string > generatePawnAttacks(int rank, int file);
        // Checks if a position is on an attacking square.
        // rank: The rank index of the position.
        // file: the file index of the position.
        // Returns true if the specified square is currently attacked.
        bool isPositionAttacked(int rank, int file);

        //
        void generateKingAttackLines();

        // Generates all legal king moves.
        // rank: The rank index of the king.
        // file: the file index of the king.
        // Returns a vector containing all legal king moves.
        vector< moveStruct > generateKingMoves(int rank, int file);
        // Generates all legal queen moves.
        // rank: The rank index of the queen.
        // file: the file index of the queen.
        // Returns a vector containing all legal queen moves.
        vector< moveStruct > generateQueenMoves(int rank, int file, bool resolveCheck);
        // Generates all legal rook moves.
        // rank: The rank index of the rook.
        // file: the file index of the rook.
        // Returns a vector containing all legal rook moves.
        vector< moveStruct > generateRookMoves(int rank, int file, bool resolveCheck);
        // Generates all legal bishop moves.
        // rank: The rank index of the bishop.
        // file: the file index of the bishop.
        // Returns a vector containing all legal bishop moves.
        vector< moveStruct > generateBishopMoves(int rank, int file, bool resolveCheck);
        // Generates all legal knight moves.
        // rank: The rank index of the knight.
        // file: the file index of the knight.
        // Returns a vector containing all legal knight moves.
        vector< moveStruct > generateKnightMoves(int rank, int file, bool resolveCheck);
        // Generates all legal pawn moves.
        // rank: The rank index of the pawn.
        // file: the file index of the pawn.
        // Returns a vector containing all legal pawn moves.
        vector< moveStruct > generatePawnMoves(int rank, int file, bool resolveCheck);
        // Generates the move structures for the four different pawn promotion options.
        // beginRank: The current rank index of the pawn.
        // beginFile: The current file index of the pawn.
        // endRank: The new rank index of the pawn.
        // endFile: The new file index of the pawn.
        // Returns a vector containing the four different promotion moves.
        vector< moveStruct > generatePromotionOptions(int beginRank, int beginFile, int endRank, int endFile, bool resolveCheck);
        // Generates legal castling moves.
        // Returns a vector containing the legal castling moves.
        vector< moveStruct > generateCastlingMoves();
        // Generates legal castling moves for white.
        // Returns a vector containing the legal castling moves for white.
        vector< moveStruct > generateCastlingMovesW();
        // Generates legal castling moves for black.
        // Returns a vector containing the legal castling moves for black.
        vector< moveStruct > generateCastlingMovesB();
        // Checks if king side castling for white is legal.
        // Returns true if legal, false otherwise
        bool kingCastleLegalW();
        // Checks if queen side castling for white is legal.
        // Returns true if legal, false otherwise
        bool queenCastleLegalW();
        // Checks if king side castling for black is legal.
        // Returns true if legal, false otherwise
        bool kingCastleLegalB();
        // Checks if queen side castling for black is legal.
        // Returns true if legal, false otherwise
        bool queenCastleLegalB();

        // Checks if a move is a castling move.
        // move: The move that is to be checked.
        // Returns true if the move is a castling move, false otherwise.
        bool isCastlingMove(string move);

        // Checks if piece at a certain position is an enemy piece.
        // rank: The rank index of the piece.
        // file: The file index of the piece.
        // Returns true if the piece is an enemy piece, false otherwise.
        bool isEnemy(int rank, int file);

        // Checks if the square at a certain position is empty.
        // rank: The rank index of the square.
        // file: The file index of the square.
        // Returns true if the square is empty, false otherwise.
        bool isEmptySquare(int rank, int file);

        // Finds the position of a king.
        // color: The bitset of the king's color.
        // Returns the position string of the king. Returns 'XX' if no king with
        // specified color was found.
        string getKingPos(bitset<5> color);
    };
}