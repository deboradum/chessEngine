#include "board.h"
#include "fen.h"
#include "helperFunctions.h"

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
        board::Board b;
        
        // Send some engine info to GUI
        cout << "id name EugineEngineV1" << endl
             << "id author Pepijn van Wijk" << endl;
        
        cout << "uciok" << endl;

        // Read option settings from GUI
        while (getline(cin, msg)) {
            if (msg == "quit") {
                cout << "Received quit, exitting..." << endl;
                exit(2);
            }
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
                fen f = parsePosition(msg);
                b.setupBoard(f);
                // b.printBoard();
            }
            if (msg.rfind("go", 0) == 0) {
                vector< moveStruct > moveList = b.generateMoves();
                if (!moveList.size()) {
                    cout << "No moves found" << endl;
                    continue;
                }; // No moves should nmot be possible I dont think
                // printMoveList(moveList);
                // cout << endl;

                int randomIndex = rand() % moveList.size();
                moveStruct m = moveList[randomIndex];
                string move = moveStructToMoveString(m);
                cout << "bestmove " << move << endl;
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
