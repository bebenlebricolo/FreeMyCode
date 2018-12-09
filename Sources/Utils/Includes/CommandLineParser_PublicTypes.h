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