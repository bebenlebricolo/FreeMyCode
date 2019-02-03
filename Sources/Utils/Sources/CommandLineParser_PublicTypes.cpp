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
#include "CommandLineParser_PublicTypes.h"

using namespace std;

ParserSlot ParserSlotArray[] = 
{
    {GENERIC                    , "Generic"                     },
    {PROJECT_DIRECTORY          , "Project directory"           },
    {TARGETED_LICENSE           , "Targeted license"            },
    {CONFIG_FILE                , "Configuration file"          },
    {LOGGING_FILE               , "Logging file     "           },
    {SECONDARY_INPUT_FILE       , "Secondary input file"        },
    {SPECTRUMS_DIRECTORY        , "Spectrums directory"         },
    {LICENSES_NOTICES_DIRECTORY , "Licenses notices directory"  }
};


const uint8_t PARSER_SLOT_ARRAY_SIZE = sizeof(ParserSlotArray) / sizeof(ParserSlot) ;


string convert_ParserResultElement_to_string(ParserResultElement element)
{
    for (uint8_t i = 0 ; i < PARSER_SLOT_ARRAY_SIZE ; i++)
    {
        if(ParserSlotArray[i].elem == element)
        {
            return ParserSlotArray[i].slot_name;
        }
    }
    throw string("Element %d could not be found in ParserSlotArray[]");
    return "";
}


