#include <_ctype.h>
#include <cctype>
#include <iostream>
#include <string>
#include <bitset>
#include <cstddef>
using namespace std;

vector<string> convertFen(string fen) {
    vector<string> result;
    string line = "";

    for (char& c : fen) {
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
};

class Board {
    public:
        vector< vector<bitset<5>> > square = {{bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                              {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                              {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                              {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                              {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                              {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                              {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}},
                                              {bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}, bitset<5>{0}}};
        vector<string> movesMade = {};

        // Constructor.
        Board(string startPosFen, vector<string> moves) {
            setupBoard(convertFen(startPosFen));

            for (string move : moves) {
                makeMove(move);
                movesMade.push_back(move);
            }
        }

        void printBoard() {
            for (vector<bitset<5>> line : square) {
                for (bitset<5> piece : line) {
                    cout << piece << " ";
                }
                cout << "\n";
            }
        }
    
    private:
        void setupBoard(vector<string> fen) {
            Piece Piece;
            int rank = 0;
            for (string line : fen) {
                int file = 0;
                for (char& c : line) {
                    if (isdigit(c)) {
                        file += int(c)-48;
                        continue;
                    }
                    switch (tolower(c)) {
                        case 'k':
                            if (isupper(c)) {
                                square[rank][file] = Piece.White | Piece.King;
                            } else {
                                square[rank][file] = Piece.Black | Piece.King;
                            }
                            break;
                        case 'q':
                            if (isupper(c)) {
                                square[rank][file] = Piece.White | Piece.Queen;
                            } else {
                                square[rank][file] = Piece.Black | Piece.Queen;
                            }
                            break;
                        case 'r':
                            if (isupper(c)) {
                                square[rank][file] = Piece.White | Piece.Rook;
                            } else {
                                square[rank][file] = Piece.Black | Piece.Rook;
                            }
                            break;
                        case 'b':
                            if (isupper(c)) {
                                square[rank][file] = Piece.White | Piece.Bishop;
                            } else {
                                square[rank][file] = Piece.Black | Piece.Bishop;
                            }
                            break;
                        case 'n':
                            if (isupper(c)) {
                                square[rank][file] = Piece.White | Piece.Knight;
                            } else {
                                square[rank][file] = Piece.Black | Piece.Knight;
                            }
                            break;
                        case 'p':
                            if (isupper(c)) {
                                square[rank][file] = Piece.White | Piece.Pawn;
                            } else {
                                square[rank][file] = Piece.Black | Piece.Pawn;
                            }
                            break;
                    }
                    file++;
                }
                rank++;
            }
        }

        // Makes a move on the board.
        void makeMove(string move) {

        }
};

int main()
{
    string msg;
    getline(cin, msg); // Read message from GUI
    
    if (msg == "uci") {
        
        // Send some engine info to GUI
        cout << "id name EugineEngineV1" << endl
             << "id author Pepijn van Wijk" << endl; // The endls are very important


        cout << "uciok" << endl;

        // Read option settings from GUI
        while (getline(cin, msg)) {
            // Handle uciok.
            if (msg == "uciok") {
                break;
            }
            if (msg == "isready") {
                cout << "readyok" << endl;
            }
            if (msg == "ucinewgame") {
                // cout << "NEW GAME COMMAND RECEIVED" << endl;
            }
            if (msg.rfind("position", 0) == 0) {
                // cout << "POSTION COMMAND RECEIVED" << endl;
            }
            if (msg.rfind("go", 0) == 0) {
                vector<string> tmpVec = {"f2f3"};
                Board b("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", {"f2f3"});
                cout << "bestmove g1f3 ponder b8c6" << endl;
            }
            else {
                // cout << msg << endl;
                // Parse msg and store option settings
                // ...
            }      
        }
    }
    
    cout << "Press any key...";
    cin.get();
    return 0;
}
