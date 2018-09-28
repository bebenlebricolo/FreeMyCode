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