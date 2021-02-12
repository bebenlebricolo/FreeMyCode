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

#ifndef COMMANDLINEPARSER_PUBLICTYPES_HEADER
#define COMMANDLINEPARSER_PUBLICTYPES_HEADER

#include "stdafx.h"
#include <string>

// Enum used to tell the Command line parser module which kind of parsing slot it
// needs to take care of.
// Simply add more of them when needed
enum ParserResultElement {
    GENERIC,
    PROJECT_DIRECTORY,
    TARGETED_LICENSE,
    CONFIG_FILE,
    LOGGING_FILE,
    SECONDARY_INPUT_FILE,
    SPECTRUMS_DIRECTORY,
    LICENSES_NOTICES_DIRECTORY
};

struct ParserSlot
{
    ParserResultElement elem;
    std::string slot_name;
};


extern ParserSlot ParserSlotArray[] ;
std::string convert_ParserResultElement_to_string(ParserResultElement element) ;


#endif // COMMANDLINEPARSER_PUBLICTYPES_HEADER