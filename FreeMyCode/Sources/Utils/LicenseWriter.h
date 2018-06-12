#pragma once
#ifndef LICENSE_WRITER_HEADER
#define LICENSE_WRITER_HEADER

#include "stdafx.h"
#include "Command_Line_parser.h"
#include "ConfigTools.h"
#include "DirectoryAnalyser.h"
#include "LoggingTools.h"
#include "FormattingUtils.h"
#include <vector>
#include <string>
#include <sstream>
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"

using namespace FormattingTags;


struct FormattedLicense;
class SecondaryInput;

class LicenseWriter {
	CommandLineParser* parser;
	ConfObject* config;
	logger::Logger* log;
	SecondaryInput* second_in;
	std::vector<FormattedLicense *> form_lic_list;	// Formatted licenses list

public:
	~LicenseWriter();
	LicenseWriter(CommandLineParser* _parser, ConfObject* _config, logger::Logger* _log = NULL);
	std::vector<std::string> write_license(void);
	//std::vector<std::string> find_prev_licenses(void);
	//bool found_prev_license(std::string file_path);
	void build_formatted_license_list(std::vector<std::string>* file_list);
	FormattedLicense* getLicenseByExt(std::string extension);

};

struct FormattedLicense {
	std::vector<std::string> targeted_extensions;
	ostringstream for_lic;
	void generate(std::string ext, ConfObject& config, CommandLineParser& parser, ostringstream* tagsBlock = nullptr);
	bool match_extension(std::string in_ext);
};


// ############################
// Secondary input class declaration
// ############################


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