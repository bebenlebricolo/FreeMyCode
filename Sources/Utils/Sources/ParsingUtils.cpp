/*

------------------
@<FreeMyCode>
FreeMyCode version : 1.0 RC alpha
    Author : bebenlebricolo
    Contributors : 
        FoxP
    License : 
        name : GPL V3
        url : https://www.gnu.org/licenses/quick-guide-gplv3.html
    About the author : 
        url : https://github.com/bebenlebricolo
    Date : 16/10/2018 (16th of October 2018)
    Motivations : This is part of a Hobby ; I wanted a tool to help open-source developers to keep their hard work traceable and protected.
<FreeMyCode>@
------------------

FreeMyCode is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "ParsingUtils.h"

#include <algorithm>
#include <cstring>
#include <locale>

using namespace std;

static const unsigned int maxIllegalChar = 50;
static bool wayToSort(const string &a, const string &b) { return a < b; }

// Extract words from a line 
void tokenizeWords(std::string &line, std::vector<std::string> &outputVector)
{
    string word;
    for (unsigned int i = 0; i < line.length(); i++)
    {
        char currentChar = line[i];
        std::locale loc;
        if (std::isalnum(currentChar, loc) == false)
        {
            if (word.length() > 0)
            {
                outputVector.push_back(word);
            }
            word.clear();
        }
        else
        {
            word += currentChar;
        }
    }

    // Finally, sort the final list
    sort(outputVector.begin(), outputVector.end(), wayToSort);
}

// Finds a char in a reference list
bool findCharInList(char in, const char* refString, size_t refSize)
{
    // Secure input
    size_t tempSize = strnlen(refString, refSize + 1);
    for (unsigned int i = 0; i < tempSize; i++)
    {
        if (in == refString[i])
        {
            return true;
        }
    }
    return false;
}