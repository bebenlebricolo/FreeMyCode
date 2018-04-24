#include "LicenseWriter.h"
#include "DirectoryAnalyser.h"
#include <sstream>
#include <fstream>
#include "PathUtils.h"
#include "LoggingTools.h"

using namespace std;

LicenseWriter::LicenseWriter(CommandLineParser* _parser, ConfObject* _config, logger::Logger* _log) :
	parser(_parser), config(_config), log(_log) {
	if (_log == NULL) {
		log = logger::Logger::get_logger();
	}
}


/*
std::vector<std::string> LicenseWriter::write_license(void) {
	std::vector<std::string>* file_list = DirectoryAnalyser::get_files_in_dir(parser->get_arg("Directory"), config->get_supported_ext_list());
	
	// Load license file in memory
	ifstream license(parser->get_arg("License"));

	for (unsigned int i = 0; i < file_list->size(); i++) {
		if (found_prev_license((*file_list)[i])) {

		}
	}
}*/

void LicenseWriter::build_formatted_license_list(std::vector<std::string>* file_list, ifstream* license) {
	for (unsigned int i = 0; i < file_list->size(); i++) {
		if (form_lic_list.size() == 0) {
			FormattedLicense f_lic;
			f_lic.generate(pathutils::get_extension((*file_list)[i]), *config, *parser);
			form_lic_list.push_back(f_lic);
		}
	}
}


void FormattedLicense::generate(string _ext, ConfObject& config, CommandLineParser& parser) {
	logger::Logger* log = logger::Logger::get_logger();
	for_lic.seekp(0, ios::end);
	if (for_lic.tellp != 0) {
		log->logError("Trying to re-write formatted license but is not empty! Aborting function", __LINE__, __FILE__, __func__, "FormattedLicense");
		// for_lic not empty!!
		// Don't re-write it!
		return;
	}
	
	vector<string> _ext_list_from_config = config.find_language_spec(_ext).extension;
	if (_ext_list_from_config[0] != "") {
		log->logInfo("Found multiple languages matching " + _ext + " extension",  __LINE__, __FILE__, __func__, "FormattedLicense");
		targeted_extensions = _ext_list_from_config;
	}
	else {
		log->logWarning("Didn't find any language spec in config that match the \"" + _ext + "\" extension",  __LINE__, __FILE__, __func__, "FormattedLicense");
		targeted_extensions.push_back(_ext);
	}

	ifstream license_file;
	string buffer_string;
	bool use_block_comment = false;
	bool use_single_line_comment = false;

	string bloc_start = config.get_bloc_comment_start(_ext);
	string bloc_end = config.get_bloc_comment_end(_ext);
	string single_com = config.get_single_line_com(_ext);

	license_file.open(parser.get_arg("License"));
	if ((bloc_start != "") 
		&& (bloc_end != ""))
	{
		use_block_comment = true;
		log->logInfo("Using block comments",  __LINE__, __FILE__, __func__, "FormattedLicense");
	}
	else if (single_com != "")
	{
		use_single_line_comment = true;
		log->logInfo("Using single line comments",  __LINE__, __FILE__, __func__, "FormattedLicense");
	}

	// Add block comment starting tag
	if (use_block_comment) {
		for_lic << bloc_start << endl;
	}

	// Copy License file content to formatted one
	while (getline(license_file, buffer_string)) {
		for_lic << (use_single_line_comment ? (single_com + " ") : "") << buffer_string << endl;
	}
	
	// Add block comment closing tag
	if (use_block_comment) {
		for_lic << bloc_end << endl;
	}
}

bool FormattedLicense::match_extension(string in_ext) {
	for (unsigned int i = 0; i < targeted_extensions.size(); i++) {
		if (in_ext == targeted_extensions[i]) {
			return true;
		}
	}
}