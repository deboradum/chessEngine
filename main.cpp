#include "board.h"

#include <_ctype.h>
#include <cctype>
#include <iostream>
#include <string>
#include <bitset>
#include <cstddef>

using namespace std;

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
                board::Board b("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", {"f2f3"});
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
