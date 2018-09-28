#ifndef PARSING_UTILS_HEADER
#define PARSING_UTILS_HEADER

#include "stdafx.h"

#include <vector>
#include <string>

// Breaks a string line into a collection of words
void tokenizeWords(std::string &line, std::vector<std::string> &outputVector);

// Looks for a char in a reference char list
// Input  : input char ; reference char list ; reference char list size
// Output : returns 'true'  if input char is in list
//          returns 'false' otherwise
bool findCharInList(char in, const char* refString, size_t refSize);


#endif