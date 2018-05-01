#pragma once
#ifndef LICENSE_WRITER_HEADER
#define LICENSE_WRITER_HEADER

#include "stdafx.h"
#include "Command_Line_parser.h"
#include "ConfigTools.h"
#include "DirectoryAnalyser.h"
#include "LoggingTools.h"
#include <vector>
#include <string>
#include <sstream>

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
	//std::vector<std::string> write_license(void);
	//std::vector<std::string> find_prev_licenses(void);
	//bool found_prev_license(std::string file_path);
	void build_formatted_license_list(std::vector<std::string>* file_list);

};

struct FormattedLicense {
	std::vector<std::string> targeted_extensions;
	ostringstream for_lic;
	void generate(std::string ext, ConfObject& config, CommandLineParser& parser);
	bool match_extension(std::string in_ext);
};

struct Tag {
	enum TagType {Standard, Custom};
	TagType type;
	std::string name;
	std::vector<std::string> values;
	Tag(std::string _name, std::vector<std::string> _values, TagType _type = TagType::Standard);
};


class SecondaryInput {
	ConfObject* config;
	CommandLineParser* parser;
	logger::Logger* log;
	std::vector<Tag*> available_tags;
	ofstream get_tags_block();
public:
	SecondaryInput(ConfObject* _config, CommandLineParser* _parser, logger::Logger* _log = NULL);
	~SecondaryInput();
	void parse_secondary_input_file();
};


#endif