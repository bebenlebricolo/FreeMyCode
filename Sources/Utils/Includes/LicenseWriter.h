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

#ifndef LICENSE_WRITER_HEADER
#define LICENSE_WRITER_HEADER

#include "stdafx.h"
#include <vector>
#include <string>
#include <sstream>

#include "CommandLineParser.h"
#include "ConfigTools.h"
#include "DirectoryAnalyser.h"
#include "LoggingTools.h"
#include "FormattingUtils.h"

// Third party libraries
#include "rapidjson.h"
#include "document.h"

using namespace FormattingTags;

// Forward declarations
struct FormattedLicense;
class SecondaryInput;

// Class whose role is to write formatted license text block to targeted file list
class LicenseWriter {
	CommandLineParser* parser;
	ConfObject* config;
	SecondaryInput* second_in;
	// Formatted licenses list
	std::vector<FormattedLicense *> form_lic_list;	

public:
	~LicenseWriter();
	LicenseWriter(CommandLineParser* _parser, ConfObject* _config);
	std::vector<std::string> write_license(std::vector<string> *fileList);
	void build_formatted_license_list(std::vector<std::string>* file_list);
	FormattedLicense* getLicenseByExt(std::string extension);

};

// Text block containing license header notice (or any piece of text) with proper language formatting 
struct FormattedLicense {
	std::vector<std::string> targeted_extensions;
	ostringstream for_lic;
	void generate(std::string ext, ConfObject& config, CommandLineParser& parser, ostringstream* tagsBlock = nullptr);
	bool match_extension(std::string in_ext);
};


// ############################
// Secondary input class declaration
// ############################

// Secondary Input class' role is to handle user input (custom informations such as author, etc...) 
// Provides parsing and formatting facilities.
class SecondaryInput {
	ConfObject* config;
	CommandLineParser* parser;
	logger::Logger* log;
	std::vector<ProtoTag*> available_tags;

public:
	SecondaryInput(ConfObject* _config, CommandLineParser* _parser, logger::Logger* _log = NULL);
	~SecondaryInput();
	void parse_secondary_input_file();
	ostringstream* getTagsFormattedBlock();
};


#endif