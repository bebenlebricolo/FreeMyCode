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