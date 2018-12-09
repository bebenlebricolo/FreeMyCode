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


