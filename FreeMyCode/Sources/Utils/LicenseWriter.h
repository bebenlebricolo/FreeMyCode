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

class LicenseWriter {
	CommandLineParser* parser;
	ConfObject* config;
	logger::Logger* log;
	std::vector<FormattedLicense> form_lic_list;	// Formatted licenses list
public:
	LicenseWriter(CommandLineParser* _parser, ConfObject* _config, logger::Logger* _log = NULL);
	//std::vector<std::string> write_license(void);
	//std::vector<std::string> find_prev_licenses(void);
	//bool found_prev_license(std::string file_path);
	void build_formatted_license_list(std::vector<std::string>* file_list, ifstream* license);

};

struct FormattedLicense {
	std::vector<std::string> targeted_extensions;
	ostringstream for_lic;
	void generate(std::string ext, ConfObject& config, CommandLineParser& parser);
	bool match_extension(std::string in_ext);
};
#endif