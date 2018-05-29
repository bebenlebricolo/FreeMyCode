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
static const char* VERSION_NODE = "FreeMyCode version";

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
	ostringstream* tagsBlock = second_in->getTagsFormattedBlock();
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

	license_file.open(parser.get_arg("License"));
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

TagLine* SecondaryInput::parseLine(Value::ConstMemberIterator& itr)
{
	TagLine *newLine = NULL;
	if (itr->value.IsString())
	{
		logger::Logger *log_ptr = logger::Logger::get_logger();
		string name = itr->name.GetString();
		string value = itr->value.GetString();
		newLine = new TagLine(name, value);
		log_ptr->logDebug("Parsed new TagLine object < " + name + " >", __LINE__, __FILE__, __func__, "SecondaryInput");
	}
	return newLine;
}

TagArray* SecondaryInput::parseArray(Value::ConstMemberIterator& itr) {
	TagArray* current_tag = NULL;
	if (itr->value.IsArray()) {
		string current_key = itr->name.GetString();
		vector<string> array_values;
		// Iterate over members of the array
		for (SizeType j = 0; j < itr->value.Size(); j++) {
			array_values.push_back(itr->value[j].GetString());
		}
		current_tag = new TagArray(current_key, array_values);
	}
	return current_tag;
}

TagObject* SecondaryInput::parseObject(Value::ConstMemberIterator &itr)
{
	TagObject *obj = NULL;
	if (itr->value.IsObject())
	{
		log->logInfo("Populating new TagObject node",
			__LINE__, __FILE__, __func__, "SecondaryInput");
		string objName = itr->name.GetString();
		obj = new TagObject(objName);
		if (obj != NULL)
		{
			string name = itr->name.GetString();
			for (Value::ConstMemberIterator sub = itr->value.MemberBegin();
				sub != itr->value.MemberEnd(); sub++)
			{
				if (sub->value.IsString())
				{
					TagLine *newLine = parseLine(sub);
					if (newLine != NULL) obj->keys.push_back(newLine);
					else log->logError("Allocation error : Cannot allocate memory for a TagLine object",
											__LINE__, __FILE__, __func__, "SecondaryInput");
					log->logDebug("Found new Line tag in secondary input file. Name : " + newLine->name + " ; Value : " + newLine->value , __LINE__, __FILE__, __func__);
				}
				else if (sub->value.IsArray())
				{
					TagArray *newArray = parseArray(sub);
					if (newArray != NULL)
					{
						obj->arrays.push_back(newArray);
						log->logDebug("Successfully parsed new TagArray < " + newArray->name + " >", __LINE__, __FILE__, __func__);
					}
					else log->logError("Allocation error : Returned array is NULL",
						__LINE__, __FILE__, __func__, "SecondaryInput");
				}
				else if (sub->value.IsObject())
				{
					TagObject *newObject = parseObject(sub);
					if (newObject != NULL)
					{
						obj->obj.push_back(newObject);
						log->logDebug("Successfully parsed new TagObject < " + newObject->name + " >", __LINE__, __FILE__, __func__);
					}
					else log->logError("Allocation error : returned TagObject ptr is NULL",
						__LINE__, __FILE__, __func__, "SecondaryInput");
				}
			}
		}
		else
		{
			log->logError("Allocation error : cannot allocate memory for new Object tag", __LINE__, __FILE__, __func__,"SecondaryInput");
		}
	}
	return obj;
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

static TagObject* getFreeMyCodeTag(ConfObject* config) {
	string name = "FreeMyCode";
	TagObject* tag = new TagObject(name);
	vector<string> *version = config->get_tag(VERSION_NODE);
	if (version != nullptr) {
		if (version->size() == 1) {
			tag->keys.push_back(new TagLine(VERSION_NODE,(*version)[0].c_str()));
		}
		else
		{
			logger::Logger::get_logger()->logWarning("Version node in Config file has multiple values",__LINE__, __FILE__, __func__);
		}
	}
	else
	{
		logger::Logger::get_logger()->logWarning("Config file has no < " + string(VERSION_NODE) + " > node", __LINE__, __FILE__, __func__);
	}
	return tag;
}


// Builds a text block containing all preformatted tags data
ostringstream* SecondaryInput::getTagsFormattedBlock() {
	ostringstream *out = new ostringstream();
	ostringstream *curTag = NULL;
	Formatter _format;
	_format.set_delimiter(':');
	if (out != NULL)
	{	
		TagObject *freeMyCode_Tag = getFreeMyCodeTag(config);
		if (freeMyCode_Tag != nullptr) {
			*out << freeMyCode_Tag->buildFormattedBlock(_format)->str();
		}

		for (unsigned int i = 0; i < available_tags.size(); i++) {
			curTag = available_tags[i]->buildFormattedBlock(_format);
			if (curTag != NULL) {
				*out << curTag->str();
			}
		}
		if (parser->get_flag("--verbose")) {
			cout << endl << "VERBOSE MODE - Printing result of Secondary Input parsing and Formatting:" << endl << endl;
			cout << out->str() << endl;
		}
	}
	return out;
}



/*
**************************************************************************************
**************************************************************************************
Classes Formatter and Indent definitions
**************************************************************************************
**************************************************************************************
*/

Indent::Indent() : indent(default_indent){}
Indent::Indent(unsigned int &_init) : indent(_init) {}

unsigned int Indent::operator() () const { return indent; }
unsigned int& Indent::operator++(int) {
	indent += default_indent;
	return indent;
}

unsigned int& Indent::operator--(int) {
	if (indent >= default_indent) {
		indent -= default_indent;
	}
	else indent = 0; 
	return indent;
}

void Indent::reset() { indent = default_indent; }

// Generates a string filled with whitespaces
string Indent::buildString() const {
	string out;
	for (unsigned i = 0; i < indent; i++) {
		out += ' ';
	}
	return out;
}

Formatter::Formatter() : indent() {}
char Formatter::delimiter() const { return _delimiter; }
void Formatter::set_delimiter(char newDelim) { _delimiter = newDelim; }

/*
**************************************************************************************
**************************************************************************************
ProtoTags and derivatives definitions
**************************************************************************************
**************************************************************************************
*/

ProtoTag::ProtoTag(string &_name) : name(_name) {}
ProtoTag::ProtoTag(const char* _name) : name(_name) {}

string ProtoTag::printNameAndDelim(Formatter &_format)
{
	string out = name + " " + _format.delimiter() + " ";
	return out;
}

TagLine::TagLine(string &_name, string &_value) : ProtoTag(_name), value(_value) {}
TagLine::TagLine(const char* _name , const char* _value) : ProtoTag(_name),value(_value) {}

ostringstream* TagLine::buildFormattedBlock(Formatter &_format)
{
	ostringstream *out = new ostringstream();
	*out << _format.indent.buildString() 
		 << printNameAndDelim(_format) 
		 << value
		 << endl;
	return out;
}

TagArray::TagArray(string &_name, vector<string> &data) : ProtoTag(_name), val(data)
{}

ostringstream* TagArray::buildFormattedBlock(Formatter &_format)
{
	// Copy incoming formatter object as we need to modify it.
	Formatter format = _format;
	ostringstream *out = new ostringstream();
	if (out != NULL)
	{
		logger::Logger::get_logger()->logDebug("Building text block for TagArray object", __LINE__, __FILE__, __func__);
		*out << format.indent.buildString()
			<< printNameAndDelim(format)
			<< endl;
			

		format.indent++;
		for (unsigned int i = 0; i < val.size(); i++)
		{
			*out << format.indent.buildString()
				<< val[i] << endl;
		}
	}
	else
	{
		logger::Logger::get_logger()->logWarning("Allocation error : cannot allocate memory for TagArray text block", __LINE__, __FILE__, __func__);
	}
	return out;
}

TagObject::TagObject(string &_name) : ProtoTag(_name)
{}

ostringstream* TagObject::buildFormattedBlock(Formatter &_format)
{
	// Copy incoming formatter object as we need to modify it.
	Formatter format = _format;
	ostringstream *out = new ostringstream();
	if (out != NULL)
	{
		unsigned int i = 0;
		logger::Logger::get_logger()->logDebug("Building text block for TagObject object", __LINE__, __FILE__, __func__);
		*out << format.indent.buildString()
			<< printNameAndDelim(format)
			<< endl;

		format.indent++;
		// Single Lines first
		// TODO: keep input order?
		ostringstream *newStream = NULL;
		for (i = 0; i < keys.size(); i++) {
			newStream = keys[i]->buildFormattedBlock(format);
			if (newStream != NULL) {
				*out << newStream->str();
				delete newStream;
			}
		}

		for (i = 0; i < arrays.size(); i++) {
			newStream = arrays[i]->buildFormattedBlock(format);
			if (newStream != NULL) {
				*out << newStream->str() ;
				delete newStream;
			}
		}

		for (i = 0; i < obj.size(); i++) {
			newStream = obj[i]->buildFormattedBlock(format);
			if (newStream != NULL) {
				*out << newStream->str();
				delete newStream;
			}
		}
	}
	else
	{
		logger::Logger::get_logger()->logWarning("Allocation error : cannot allocate memory for TagArray text block", __LINE__, __FILE__, __func__);
	}
	return out;
}

TagObject::~TagObject()
{
	unsigned i = 0;
	for (i = 0; i < keys.size(); i++)
	{
		delete keys[i];
	}
	for (i = 0; i < arrays.size(); i++)
	{
		delete arrays[i];
	}
	for (i = 0; i < obj.size(); i++)
	{
		delete obj[i];
	}
}