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

fen parsePosition(string positionString);
