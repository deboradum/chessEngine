#include <iostream>
#include <string>
using namespace std;

static class Piece {
    public:
        const int None = 0;
        const int King = 1;
        const int Queen = 2;
        const int Rook = 3;
        const int Bishop = 4;
        const int Knight = 5;

        const int White = 8;
        const int Black = 16;
};

class Board {
    public:
        int square[64] = {};
        vector<string> movesMade = {};

        // Constructor.
        Board(string startPos, vector<string> moves) {
            fenToBoard(startPos);

            for (string move : moves) {
                makeMove(move);
                movesMade.push_back(move);
            }

            cout << square[0] << square[1] << square[2] << square[63] << endl;
        }
    
    private:
        void fenToBoard(string fen) {
            
        }

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
