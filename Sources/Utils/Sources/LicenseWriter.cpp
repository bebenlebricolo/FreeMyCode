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
#include <sstream>
#include <fstream>
#include <cstdio>

#include "LicenseWriter.h"
#include "PathUtils.h"
#include "LoggingTools.h"
#include "DirectoryAnalyser.h"

// rapidjson headers
#include "filereadstream.h"
#include "document.h"
#include "istreamwrapper.h"

#include FS_INCLUDE


using namespace FS_CPP;
using namespace rapidjson;
using namespace std;
namespace fs = FS_CPP;

static const char* TAGS_NODE = "Tags";
static const char* FREEMYCODE_TAGS = "FreeMyCode tags";


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


FormattedLicense* LicenseWriter::getLicenseByExt(std::string extension)
{
	for (unsigned int itr = 0; itr < form_lic_list.size(); itr++)
	{
		if (form_lic_list[itr]->match_extension(extension)) {
			return form_lic_list[itr];
		}
	}
	return nullptr;
}


std::vector<std::string> LicenseWriter::write_license(void) {
	std::vector<std::string>* file_list = DirectoryAnalyser::get_files_in_dir(parser->get_arg("Directory"), config->get_supported_ext_list());

	// TODO : check fo previous licenses in every files before attempting to write in them.

	// Wrong files : List of files which cannot be modified (for any reason)
	std::vector<std::string> wrongFiles;
	FormattedLicense *targetedLicense = nullptr;

    // Select both append and prepend write modes boolean
	bool selectBoth = false;
	string userInputBuffer;

	// Check whether both positioning flag are set or not
	if (parser->get_flag("--append") == true
		&& parser->get_flag("--prepend") == true) {
		log->logWarning("Both prepend and append flags are set", __LINE__, __FILE__, __func__, "LicenseWriter");
		cout << "Should we write stuff at the beginning (append) AND end of the file (append) ? y/n : ";
		cin >> userInputBuffer;
		if (userInputBuffer == "y") selectBoth = true;
		cout << "\nSelect Both = " << (selectBoth ? "true" : "false") << endl;
		if (selectBoth == false){
			cout << "Which positioning shall we use? A -> append , P -> prepend (A/P) : ";
			do {
				cin >> userInputBuffer;
				if (userInputBuffer == "A"){
					parser->overrideFlag("--prepend", false);
					cout << "Append selected." << endl;
					log->logInfo("Conflict resolved : append flag selected.", __LINE__, __FILE__, __func__, "LicenseWriter");
					break;
				}
				else if (userInputBuffer == "P") {
					parser->overrideFlag("--append", false);
					cout << "Prepend selected." << endl;
					log->logInfo("Conflict resolved : prepend flag selected.", __LINE__, __FILE__, __func__, "LicenseWriter");
					break;
				}
			} while (true);
		}
	}


	for (unsigned int i = 0; i < file_list->size(); i++) {
		string currentFile = (*file_list)[i];
		string currentExtension = pathutils::get_extension(currentFile);
		targetedLicense = getLicenseByExt(currentExtension);
		if (targetedLicense == nullptr) {
			log->logWarning("Cannot find any previously built license for this extension < " + currentExtension + " >. Maybe try to rebuild the list?",
				__LINE__, __FILE__, __func__, "LicenseWriter");
			log->logError("Encountered error in file : " + pathutils::get_filename(currentFile),
				__LINE__, __FILE__, __func__, "LicenseWriter");
			wrongFiles.push_back(currentFile);
			continue;
		}
		else {
			log->logInfo("Found FormattedLicense for extension < " + currentExtension + " >.",
				__LINE__, __FILE__, __func__, "LicenseWriter");


			if (parser->get_flag("--append") == true) {
				ofstream file;
				file.open(currentFile, ios::out | ios::app);
				file << targetedLicense->for_lic.str();
				file.close();
			}
			if (parser->get_flag("--prepend") == true ) {
				ifstream file;
				ofstream tmpFile;
				string tmpPath = currentFile + ".tmp";
				string bufferString;

				// Create a temporary file
				tmpFile.open(tmpPath, ios::out);
				// Load original file
				file.open(currentFile, ios::in);
				// Write license to temporary
				tmpFile << targetedLicense->for_lic.str() << endl;
				// And do a strict copy from the original file
				tmpFile << file.rdbuf();
				file.close();
				tmpFile.close();
				if (file.is_open() == false)
				{
					// Remove original file
					remove(currentFile);
					if (exists(currentFile) == true) {
						log->logError("Cannot remove file : " + pathutils::get_filename(currentFile) + " from disk",
							__LINE__, __FILE__, __func__, "LicenseWriter");
						wrongFiles.push_back(currentFile);
					}
					else
					{

						if (rename(tmpPath.c_str(), currentFile.c_str()) != 0)
						{
							log->logError("Cannot rename file : " + pathutils::get_filename(currentFile),
								__LINE__, __FILE__, __func__, "LicenseWriter");
							wrongFiles.push_back(currentFile);
						}
						else
						{
							log->logInfo("Successfully modified file : " + pathutils::get_filename(currentFile),
								__LINE__, __FILE__, __func__, "LicenseWriter");
						}
					}

				}
				else
				{
					log->logError("Cannot close file : " + pathutils::get_filename(currentFile),
						__LINE__, __FILE__, __func__, "LicenseWriter");
					wrongFiles.push_back(currentFile);
				}
			}
		}
	}
	return wrongFiles;
}

// Build formatted licenses list and store them in memory
void LicenseWriter::build_formatted_license_list(std::vector<std::string>* file_list) {
	ostringstream* tagsBlock = nullptr;
	if (second_in != nullptr) {
		second_in->parse_secondary_input_file();
		 tagsBlock = second_in->getTagsFormattedBlock();
	}
	for (unsigned int i = 0; i < file_list->size(); i++) {
		bool match_previous_ext = false;
		string cur_ext = pathutils::get_extension((*file_list)[i]);
		if (form_lic_list.size() == 0) {
			FormattedLicense *f_lic = new FormattedLicense();
			if (f_lic != NULL) {
				f_lic->generate(cur_ext, *config, *parser , tagsBlock);
				form_lic_list.push_back(f_lic);
			}
		}
		// If we find at least one compatible formatted license (already built)
		// Don't build another one (prevent building several identical licenses)
		for (unsigned int j = 0; j < form_lic_list.size(); j++) {
			match_previous_ext |= form_lic_list[j]->match_extension(cur_ext);
		}
		if (match_previous_ext == false) {
			FormattedLicense *f_lic = new FormattedLicense();
			if (f_lic != NULL) {
				f_lic->generate(cur_ext, *config, *parser, tagsBlock);
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


void FormattedLicense::generate(string _ext, ConfObject& config,
	CommandLineParser& parser, ostringstream* tagsBlock )
{
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
		log->logDebug("Found multiple languages matching " + _ext + " extension",  __LINE__, __FILE__, __func__, "FormattedLicense");
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

	if ((bloc_start != "")
		&& (bloc_end != ""))
	{
		use_block_comment = true;
		log->logDebug("Using block comments",  __LINE__, __FILE__, __func__, "FormattedLicense");
	}
	else if (single_com != "")
	{
		use_single_line_comment = true;
		log->logDebug("Using single line comments",  __LINE__, __FILE__, __func__, "FormattedLicense");
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

	// Write Tags block before the license file
	if (tagsBlock != nullptr) {
		stringstream outputBlock (tagsBlock->str());
		string FreeMyCode_SectionOpen = config.get_tag("FreeMyCode tags")->getProperty("Section open");
		string FreeMyCode_SectionClose = config.get_tag("FreeMyCode tags")->getProperty("Section close");
		string separator = "------------------";

		for_lic << endl << (use_single_line_comment ? (single_com + " ") : "") << separator << endl;
		for_lic << (use_single_line_comment ? (single_com + " ") : "") << FreeMyCode_SectionOpen << endl;
		while (getline(outputBlock, buffer_string)) {
			for_lic << (use_single_line_comment ? (single_com + " ") : "") << buffer_string << endl;
		}
		for_lic << (use_single_line_comment ? (single_com + " ") : "") << FreeMyCode_SectionClose << endl;
		for_lic << (use_single_line_comment ? (single_com + " ") : "") << separator << endl << endl;

	}

	// Copy License file content to formatted one
	// First check if path points to something
	// Then check if file opening succeeded
	if (exists(parser.get_arg("License"))) {
		license_file.open(parser.get_arg("License"));
		if(license_file) {
			while (getline(license_file, buffer_string)) {
				for_lic << (use_single_line_comment ? (single_com + " ") : "") << buffer_string << endl;
			}
		}
		else {
			log->logError("Cannot open License file.", __LINE__, __FILE__, __func__, "LicenseWriter");
		}
	}
	else {
		log->logError("Path to License file is wrong. Please check your path",__LINE__,__FILE__,__func__,"LicenseWriter");
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
			itr != tags_node->MemberEnd(); itr++)
		{
			ProtoTag *newTag = NULL;
			if (itr->value.IsString())		newTag = parseLine(itr);
			else if (itr->value.IsArray())	newTag = parseArray(itr);
			else if (itr->value.IsObject())	newTag = parseObject(itr);

			if (newTag != NULL)
			{
				log->logDebug("Add new TagLine to stack", __LINE__, __FILE__, __func__);
				available_tags.push_back(newTag);
			}
			else log->logError("Returned TagLine ptr is NULL", __LINE__, __FILE__, __func__);
		}
	}
	else
	{
		log->logWarning("Secondary input file doesn't have a < Tags > root node. Format is not compatible", __LINE__, __FILE__, __func__);
	}
}


// Builds a text block containing all preformatted tags data
ostringstream* SecondaryInput::getTagsFormattedBlock() {
	ostringstream *out = new ostringstream();
	ostringstream *curTag = NULL;
	Formatter _format;
	if (out != NULL)
	{
		string delimiter = "Delimiter";
		TagObject *freeMyCode_Tag = static_cast<TagObject*>(config->get_tag(FREEMYCODE_TAGS));
		if (freeMyCode_Tag != nullptr) {
			_format.set_delimiter(freeMyCode_Tag->getProperty(delimiter).c_str()[0]);
			*out << "FreeMyCode version : " << freeMyCode_Tag->getProperty("Version") << endl;
		}
		else
		{
			_format.set_delimiter(':');
		}

		for (unsigned int i = 0; i < available_tags.size(); i++) {
			curTag = available_tags[i]->buildFormattedBlock(_format);
			if (curTag != NULL) {
				*out << curTag->str() ;
			}
		}
		if (parser->get_flag("--verbose")) {
			cout << endl << "VERBOSE MODE - Printing result of Secondary Input parsing and Formatting:" << endl << endl;
			cout << out->str() << endl;
		}
	}
	return out;
}


