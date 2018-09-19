#include "stdafx.h"
#include "ParsingUtils.h"

#include <algorithm>
#include <cstring>

using namespace std;

static const char* illegalCharacters = " .-,;:!^*+=\"'()_|[]}{#~&$€£%%/\\<>`";
static const unsigned int maxIllegalChar = 50;

static bool wayToSort(const string &a, const string &b) { return a < b; }

static bool isAnIllegalChar(char in)
{
    for (unsigned int i = 0 ; i < maxIllegalChar; i++)
    {
        if (in == illegalCharacters[i])
        {
            return true;
        }
    }
    return false;
}


void tokenizeWords(std::string &line, std::vector<std::string> &outputVector)
{
    string word;
    for (unsigned int i = 0; i < line.length(); i++)
    {
        char currentChar = line[i];
        if (isAnIllegalChar(currentChar))
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
