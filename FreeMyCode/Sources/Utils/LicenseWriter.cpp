/*
The LicenseWriter tool is designed to preformat licenses in memory. 
It is supplied by instances of ConfObject, LoggingTools and CommandLineParser classes.
It basically takes a raw text file containing the basic License (non formatted) and uses knowledge from
the ConfObject instance to format a new version of the license in memory, according to the targeted file-type extension.

It also supports adding tags such as the author's name, license url, etc.. to the formatted License.
TODO : add support for previous license detection and analyse (prevent multiple licensing of a file)
	-> Detect previous FreeMyCode headers
	-> Detect previous Licenses in files (usage of external libraries, third-party code, etc.)
	-> Support for File and directory exclusion (same purpose as above)
----------------------------------------------------------------------------
Version	|	 Date	 |	Comments
----------------------------------------------------------------------------
0.1		| 23/04/2018 | Able to format a basic License file and decline it in many flavours. Keep files in RAM (no disk writing)
*/

#include "stdafx.h"
#include "LicenseWriter.h"
#include "DirectoryAnalyser.h"
#include <sstream>
#include <fstream>
#include <filesystem>

#include "rapidjson\filereadstream.h"
#include "rapidjson\document.h"
#include "rapidjson\istreamwrapper.h"

#include "PathUtils.h"
#include "LoggingTools.h"

using namespace std::experimental::filesystem;
using namespace rapidjson;
using namespace std;

static const char* TAGS_NODE = "Tags";
static const char* NODES_ARRAY[] = {
	"Author",
	"License name",
	"License url",
	"Date",
	"Organisation",
	"Organisation name",
	"Organisation website"
};
static int NODES_ARRAY_DIM = 7;

static const char* LIST_NODES[] = {
	"Author list",
};
static int LIST_NODES_SIZE = 1;



/*
**************************************************************************************
**************************************************************************************
LicenseWriter class definition
**************************************************************************************
**************************************************************************************
*/

LicenseWriter::LicenseWriter(CommandLineParser* _parser, ConfObject* _config, logger::Logger* _log) :
	parser(_parser), config(_config), log(_log) {
	if (_log == NULL) {
		log = logger::Logger::get_logger();
	}

	// If we found a Secondary Input argument when parsing input
	// build a new Secondary Input instance
	string second_in_path = parser->get_arg("Secondary Input");
	if (second_in_path != "" &&
		exists(second_in_path))
	{
		second_in = new SecondaryInput(_config, _parser, log);
	}
	else {
		log->logInfo("Could not find Secondary Input file.", __LINE__, __FILE__, __func__, "LicenseWritter");
		second_in = NULL;
	}
}

// Delete secondary Input instance when destroying LicenseWriter
LicenseWriter::~LicenseWriter() {
	if (second_in != NULL) {
		delete(second_in);
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

// Build formatted licenses list and store them in memory
void LicenseWriter::build_formatted_license_list(std::vector<std::string>* file_list) {
	second_in->parse_secondary_input_file();
	for (unsigned int i = 0; i < file_list->size(); i++) {
		bool match_pref_license_ext = false;
		string cur_ext = pathutils::get_extension((*file_list)[i]);
		if (form_lic_list.size() == 0) {
			FormattedLicense *f_lic = new FormattedLicense();
			if (f_lic != NULL) {
				f_lic->generate(cur_ext, *config, *parser);
				form_lic_list.push_back(f_lic);
			}
		}
		// If we find at least one compatible formatted license (already built)
		// Don't build another one (prevent building several identical licenses)
		for (unsigned int j = 0; j < form_lic_list.size(); j++) {
			match_pref_license_ext |= form_lic_list[j]->match_extension(cur_ext);
		}
		if (match_pref_license_ext == false) {
			FormattedLicense *f_lic = new FormattedLicense();
			if (f_lic != NULL) {
				f_lic->generate(cur_ext, *config, *parser);
				form_lic_list.push_back(f_lic);
			}
		}
	}
}


/*
**************************************************************************************
**************************************************************************************
FormattedLicense struct definition
**************************************************************************************
**************************************************************************************
*/

void FormattedLicense::generate(string _ext, ConfObject& config, CommandLineParser& parser) {
	logger::Logger* log = logger::Logger::get_logger();
	for_lic.seekp(0, ios::end);
	if (0 != for_lic.tellp()) {
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

	// By default : use block comments instead of single for readability
	if (use_block_comment && use_single_line_comment) {
		use_single_line_comment = false;
	}

	// Otherwise, if we found a "force single line comment"
	if (parser.get_flag("--force-single-line")) {
		if (use_block_comment == true
			&& single_com != "") 
		{
			use_block_comment = false;
			use_single_line_comment = true;
		}
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

	// Log formatted license files!
	if (parser.get_flag("--log-formatted-license")) {
		for_lic.seekp(std::ios::beg);
		string stream_str;
		stringstream buffer;
		buffer << for_lic.str();
		log->logInfo("Dumping formatted license content for langage < " + _ext + " > :");
		while (getline(buffer,stream_str)) {
			log->logInfo(stream_str);
		}
	}
}

bool FormattedLicense::match_extension(string in_ext) {
	for (unsigned int i = 0; i < targeted_extensions.size(); i++) {
		if (in_ext == targeted_extensions[i]) {
			return true;
		}
	}
	return false;
}

/*
**************************************************************************************
**************************************************************************************
Tag struct definition
**************************************************************************************
**************************************************************************************
*/

Tag::Tag(string _name, vector<string> _values, Tag::TagType _type) 
	: name(_name),values(_values),type(_type){}

/*
**************************************************************************************
**************************************************************************************
SecondaryInput class definition
**************************************************************************************
**************************************************************************************
*/

SecondaryInput::SecondaryInput(ConfObject* _config, CommandLineParser* _parser, logger::Logger* _log)
	:	config(_config),
		parser(_parser),
		log(_log)
{
	if (_log == NULL) {
		log = logger::Logger::get_logger();
	}
}

// Safely destroys all remaining tags
SecondaryInput::~SecondaryInput() {
	for (unsigned int i = 0; i < available_tags.size(); i++) {
		if (available_tags[i] != NULL) {
			delete(available_tags[i]);
		}
	}
}

void SecondaryInput::parse_secondary_input_file() {
	string filepath = parser->get_arg("Secondary Input");
	
	// Documentation : using file stream instead of C FILE pointers
	// http://rapidjson.org/md_doc_stream.html#FileStreams
	ifstream file_stream(filepath);
	IStreamWrapper isw(file_stream);

	// rapidjson documentation : http://rapidjson.org/md_doc_stream.html#FileStreams
	// Reading / Parsing file streams

	Document doc;
	doc.ParseStream(isw);
	file_stream.close();
	log->logInfo("Opened json file, read data and load data in memory", __LINE__, __FILE__, __func__, "ConfObject");

	if (doc.HasMember(TAGS_NODE)) {
		// Enter the parsing function's body
		rapidjson::Value *tags_node = &(doc[TAGS_NODE]);
		log->logInfo("Found " + string(TAGS_NODE) + " node in SecondaryInput file", __LINE__, __FILE__, __func__, "SecondaryInput");

		for (Value::ConstMemberIterator itr = tags_node->MemberBegin();
			itr != tags_node->MemberEnd(); itr++) {
			if (itr->value.IsArray()) {
				string current_key = itr->name.GetString();
				vector<string> array_values;
				Tag* current_tag = NULL;
				bool found_standard = false;
				// Iterate over members of the array
				for (SizeType j = 0; j < itr->value.Size(); j++) {
					array_values.push_back(itr->value[j].GetString());
				}
				// Test if the key matches with any of the Standard array types
				for (unsigned i = 0; i < LIST_NODES_SIZE; i++) {
					if (LIST_NODES[i] == current_key) {
						log->logInfo("Found \" " + current_key + " \" array in file " , __LINE__, __FILE__, __func__, "SecondaryInput");
						current_tag = new Tag(itr->name.GetString(),array_values);
						found_standard = true;
						continue;
					}
				}
				
				// If test fails, this means we are facing a User-defined "Custom" Tag
				if (found_standard == false) 
				{
					log->logWarning("Found \" " + current_key + " \" array which is not in file -> Considered as \"Custom\" Tag ", __LINE__, __FILE__, __func__, "SecondaryInput");
					current_tag = new Tag(itr->name.GetString(), array_values, Tag::TagType::Custom);
				}
				// Finally populate the available_flags vector
				available_tags.push_back(current_tag);
				
				if (parser->get_flag("--verbose")) {
					for (unsigned printer = 0; printer < array_values.size(); printer++) {
						log->logInfo("Found " + current_key + " : " + array_values[printer], __LINE__, __FILE__, __func__, "SecondaryInput");
					}
				}
			}
			else if(itr->value.IsString()) {
				// Parse recursively and identify tags type
				Tag* current_tag = NULL;
				string current_key = itr->name.GetString();
				log->logInfo("Currently evaluated Tag is \" " + current_key + " \"", __LINE__, __FILE__, __func__, "SecondaryInput");
				vector<string> current_value = { itr->value.GetString() };
				bool found_standard = false;
				// Iterate over supported Nodes type
				for (unsigned i = 0; i < NODES_ARRAY_DIM; i++) {
					if (current_key == NODES_ARRAY[i]) {
						log->logInfo("Found corresponding Tag : \" " + current_key + " \"", __LINE__, __FILE__, __func__, "SecondaryInput");
						current_tag = new Tag(current_key, current_value);
						found_standard = true;
						continue;
					}
				}
				if (found_standard == false) 
				{
					log->logWarning("Found \" " + current_key + " \" key which is not in file -> Considered as \"Custom\" Tag ", __LINE__, __FILE__, __func__, "SecondaryInput");
					current_tag = new Tag(current_key, current_value, Tag::TagType::Custom);
				}
				available_tags.push_back(current_tag);
				if (parser->get_flag("--verbose")) {
					log->logInfo("Found " + current_key + " : " + current_value[0], __LINE__, __FILE__, __func__, "SecondaryInput");
				}
			}
		}
	}
}