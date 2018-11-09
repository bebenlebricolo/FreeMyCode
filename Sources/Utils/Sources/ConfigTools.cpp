/*
Configuration tools that might be used for configuration files handling.
It basically relies on the rapidjson implementation

----------------------------------------------------------------------------
Version	|	 Date	 |	Comments
----------------------------------------------------------------------------
0.1     | 04/04/2018 | Wrote the ConfObject parse_conf_file method and implemented first methods of classes
0.11    | 14/04/2018 | Added logging facilities

*/


#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include "ConfigTools.h"
#include "LoggingTools.h"

// Use Rapidjson external library
#include "filereadstream.h"
#include "document.h"
#include "istreamwrapper.h"

#include FS_INCLUDE



using namespace rapidjson;

namespace fs = FS_CPP;
using namespace std;

static const char*  CONF_NODE = "Configuration";
static const char*  LANG_NODE = "Languages";
static const char*  LANG_SL_COM = "Single line comment";
static const char*  LANG_BL_COM_OP = "Bloc comment opening";
static const char*  LANG_BL_COM_CL = "Bloc comment closing";
static const char*  LANG_EXT_ARRAY = "Extension array";
static const char*  LANG_EXT_NODE = "Extension";

static const char*  TAGS_NODE = "Tags";

static const char* MARK_SL_COM ="single line comment";
static const char* MARK_BL_COM_OP ="block comment start";
static const char* MARK_BL_COM_CL ="block comment end";



// ##########################################
// CommentMarker structure's implementation
// ##########################################

// Comment Markers stores information about comment patterns of a given set of languages (e.g : /* ; */ ; //  for C-style languages)
CommentMarkers::CommentMarkers() : 
    sgLine(MARK_SL_COM, "", CommentTag::single),
    bStart(MARK_BL_COM_OP,"", CommentTag::block),
    bEnd(MARK_BL_COM_CL, "", CommentTag::block),
    isPlainText(true) {
}

CommentMarkers::CommentMarkers(std::string _single_line_comment, std::string _block_comment_start, std::string _block_comment_end) :
    sgLine(MARK_SL_COM, _single_line_comment, CommentTag::single),
    bStart(MARK_BL_COM_OP,_block_comment_start,CommentTag::block),
    bEnd(MARK_BL_COM_CL,_block_comment_end,CommentTag::block) {
    checkIfPlainText();
}

// Checks if we are targetting plain text (no comment markers) or something else
bool CommentMarkers::checkIfPlainText()
{
    if (sgLine.value == "" && bStart.value == "" && bEnd.value == "") {
        isPlainText = true;
    }
    else {
        isPlainText = false;
    }
	return isPlainText;
}

void CommentMarkers::reset()
{
    isPlainText = false;
    sgLine.value = "";
	bStart.value = "";
	bEnd.value = "";
}

// Returns comment markers in the form of a vector 
void CommentMarkers::vectorizeMembers(std::vector<CommentTag> *vec)
{
    vec->push_back(sgLine);
    vec->push_back(bStart);
    vec->push_back(bEnd );
}


bool CommentMarkers::checkForMissingCommentMarker(ostringstream *errorMessage)
{
    bool errorFound = false;
    if (sgLine.value == "" )
    {
        (*errorMessage) << "Single line comment is empty !";
        errorFound = true;
    }
    if (bStart.value == "" || bEnd.value == "")
    {
        (*errorMessage) << "Block comment marker are empty! ";
        errorFound = true;
    }
    return errorFound;
}


// ##################################################
// #### SupportedExtension structure implementation
// ##################################################

SupportedExtension::SupportedExtension(
	std::vector<std::string> _ext_list,
	std::string line_com,
	std::string block_start,
	std::string block_end 
	) :
		extension(_ext_list),
		markers(line_com,
				block_start,
				block_end) 
		{}
SupportedExtension::SupportedExtension(
	std::string _single_ext,
	std::string line_com,
	std::string block_start,
	std::string block_end
	) :
		extension(),
		markers(line_com,
				block_start,
				block_end) 

{
	extension.push_back(_single_ext);
}

// Look for a given extension in memory
bool SupportedExtension::match_ext(std::string _ext)
{
	for (unsigned int i = 0; i < extension.size(); i++) {
		if (_ext == extension[i])
			return true;
	}
	return false;
}



/*------------------------------------------------------------------------
--------------------- ConfObject class implementation---------------------
--------------------------------------------------------------------------*/
ConfObject* ConfObject::_instance = nullptr;

ConfObject::ConfObject() {
    if (_instance != nullptr)
    {
        delete _instance;
    }
    _instance = this;
}

// Now that we have a dedicated pointer to a logger, we need to properly delete it.
ConfObject::~ConfObject() {
	for (unsigned i = 0; i < tags_vect.size(); i++) {
		delete(tags_vect[i]);
	}
    _instance = nullptr;
}



// Parses a file using the rapidjson library
// Note: this function is obviously super long (...)
bool ConfObject::parse_conf_file(std::string filepath) {

	logger::Logger* log_ptr = logger::getLogger();
	if (!fs::exists(filepath)) {
		log_ptr->logWarning("Filepath doesn't exist", __LINE__, __FILE__, __func__, "ConfObject");
		// Nothing to be parsed
		return false;
	}

	// Documentation : using file stream instead of C FILE pointers
	// http://rapidjson.org/md_doc_stream.html#FileStreams
	ifstream file_stream(filepath);
	IStreamWrapper isw(file_stream);

	// rapidjson documentation : http://rapidjson.org/md_doc_stream.html#FileStreams
	// Reading / Parsing file streams

	Document doc;

	// Buffer variables
	string cur_ext;
	// Single line comment
	string sl_com;
	// Block comment start
	string bl_com_st;
	// Block comment end
	string bl_com_end;


	doc.ParseStream(isw);
	file_stream.close();
	log_ptr->logInfo("Opened json file, read data and load data in memory", __LINE__, __FILE__, __func__, "ConfObject");

	// If we found a Configuration (root) node
	if (doc.HasMember(CONF_NODE)) {
		rapidjson::Value *config_node = &(doc[CONF_NODE]);
		log_ptr->logInfo("Found <" + string(CONF_NODE) + "> Node in file", __LINE__, __FILE__, __func__, "ConfObject");

		// If we found a Language node in the config file
		if (config_node->HasMember(LANG_NODE)) {
			rapidjson::Value *languages_node = &((*config_node)[LANG_NODE]);
			log_ptr->logInfo("Found <" + string(LANG_NODE) + "> Node in file", __LINE__, __FILE__, __func__, "ConfObject");

			// Iterate over found languages
			for (SizeType i = 0; i < languages_node->Size(); i++) {
				// If we found an extension array :
				if ((*languages_node)[i].HasMember(LANG_EXT_ARRAY)) {
					rapidjson::Value* Ext_array = &((*languages_node)[i][LANG_EXT_ARRAY]);
					log_ptr->logInfo("Found <" + string(LANG_EXT_ARRAY) + "> Node in file", __LINE__, __FILE__, __func__, "ConfObject");

					if ((*languages_node)[i].HasMember(LANG_SL_COM)) sl_com = (*languages_node)[i][LANG_SL_COM].GetString();
					if ((*languages_node)[i].HasMember(LANG_BL_COM_OP)) bl_com_st = (*languages_node)[i][LANG_BL_COM_OP].GetString();
					if ((*languages_node)[i].HasMember(LANG_BL_COM_CL)) bl_com_end = (*languages_node)[i][LANG_BL_COM_CL].GetString();

					std::vector<std::string> _ext_v;
					// Iterate over extensions array
					for (unsigned int ext = 0; ext < Ext_array->Size(); ext++) {
						cur_ext = (*Ext_array)[ext].GetString();
						_ext_v.push_back(cur_ext);
					}
					SupportedExtension new_ext(_ext_v, sl_com, bl_com_st, bl_com_end);
					extension_vect.push_back(new_ext);
				}
				// Else : we didn't find the "Extension array" node
				else {
					if ((*languages_node)[i].HasMember(LANG_EXT_NODE)) cur_ext = (*languages_node)[i][LANG_EXT_NODE].GetString();
					if ((*languages_node)[i].HasMember(LANG_SL_COM)) sl_com = (*languages_node)[i][LANG_SL_COM].GetString();
					if ((*languages_node)[i].HasMember(LANG_BL_COM_OP)) bl_com_st = (*languages_node)[i][LANG_BL_COM_OP].GetString();
					if ((*languages_node)[i].HasMember(LANG_BL_COM_CL)) bl_com_end = (*languages_node)[i][LANG_BL_COM_CL].GetString();

					SupportedExtension new_ext(cur_ext, sl_com, bl_com_st, bl_com_end);
					extension_vect.push_back(new_ext);
				}

				// Reinitialising buffer strings before next loop
				sl_com = "";
				bl_com_end = "";
				bl_com_st = "";
				cur_ext = "";
			}
			log_ptr->logInfo("JSON file parsing successfull", __LINE__, __FILE__, __func__, "ConfObject");

			// Parsing successfull
		}
		else {
			log_ptr->logError("Cannot find <" + string(LANG_NODE) + "> Node in file", __LINE__, __FILE__, __func__, "ConfObject");
			// Parsing unsuccessfull -> no "Language" node found
			return false;
		}

		// If we found a Tag node with Tags informations
		if (config_node->HasMember(TAGS_NODE)) {
			log_ptr->logInfo("Entered the \" " + string(TAGS_NODE) + " \" node of config file", __LINE__, __FILE__, __func__, "ConfObject");
			const Value& tags_node = (*config_node)[TAGS_NODE];
			ProtoTag* newTag = nullptr;
			bool foundError = false;

			for (Value::ConstMemberIterator itr = tags_node.MemberBegin(); itr != tags_node.MemberEnd(); itr++) {

				// Parsing current node depending on found type
				if (itr->value.IsString()) {
					newTag = FormattingTags::parseLine(itr);
				}
				else if (itr->value.IsArray()) {
					newTag = FormattingTags::parseArray(itr);
				}
				else if (itr->value.IsObject()) {
					newTag = FormattingTags::parseObject(itr);
				}

				// NewTag is pushed to available tags list
				if (newTag != nullptr) {
					tags_vect.push_back(newTag);
				}
				else
				{
					log_ptr->logWarning("Cannot parse current json node < " + string(itr->name.GetString()) + " >",
						__LINE__, __FILE__, __func__, "ConfObject");
					foundError = true;
				}
			}
			if (foundError == false) {
				log_ptr->logInfo("Successfully parsed the \" " + string(TAGS_NODE) + " \" node of config file",
					__LINE__, __FILE__, __func__, "ConfObject");
			}
			else
			{
				log_ptr->logWarning("Config tags parsing encountered at least one error. Results might be partial.",
					__LINE__, __FILE__, __func__, "ConfObject");
			}
		}
	}
	else {
		return false;
	}
	return true;
}


// Simply look for a given extension in memory.
// If the passed extension matches at least one extension in the ConfObject, return true.
bool ConfObject::is_extension_supported(string extension) {
	logger::Logger* log_ptr = logger::getLogger();
	for (auto& ext : extension_vect) {
		for (unsigned int i = 0; i < ext.extension.size(); i++) {
			if (ext.extension[i] == extension) return true;
		}
		log_ptr->logInfo("Found supported extension <" + extension + "> ", __LINE__, __FILE__, __func__, "ConfObject");
	}
	log_ptr->logWarning("Unsupported extension <" + extension + "> ", __LINE__, __FILE__, __func__, "ConfObject");

	return false;
}

// Get targeted extension comment marker string (e.g : C -> block opening : /* )
const string ConfObject::get_ext_property(string targeted_ext, SupportedExtension::properties prop_type) {
	logger::Logger* log_ptr = logger::getLogger();	
	for (auto& ext : extension_vect) {
		for (unsigned int i = 0; i < ext.extension.size(); i++) {
			if (ext.extension[i] == targeted_ext) {
				switch (prop_type) {
				case SupportedExtension::properties::Bloc_End:
					if (ext.markers.bStart.value == "") {
						log_ptr->logWarning("Extension : " + targeted_ext + " : Does not have bloc comment closing marker", __LINE__, __FILE__, __func__, "ConfObject");
						return "";
					}
					else {
						log_ptr->logDebug("Extension : " + targeted_ext + " : Closing block comment marker <" + ext.markers.bEnd.value + "> ", __LINE__, __FILE__, __func__, "ConfObject");
						return ext.markers.bEnd.value;
					}
					break;
				case SupportedExtension::properties::Bloc_Start:
					if (ext.markers.bStart.value == "") {
						log_ptr->logWarning("Extension : " + targeted_ext + " : Does not have bloc comment opening marker", __LINE__, __FILE__, __func__, "ConfObject");
						return "";
					}
					else {
						log_ptr->logDebug("Extension : " + targeted_ext + " : Found Opening block comment marker <" + ext.markers.bStart.value + "> ", __LINE__, __FILE__, __func__, "ConfObject");
						return ext.markers.bStart.value;
					}
					break;
				case SupportedExtension::properties::Single_Comment:
					if (ext.markers.sgLine.value == "") {
						log_ptr->logWarning("Extension : " + targeted_ext + " : Does not have single line comment marker ", __LINE__, __FILE__, __func__, "ConfObject");
						return "";
					}
					else {
						log_ptr->logDebug("Extension : " + targeted_ext + " : Found Single line comment marker <" + ext.markers.sgLine.value + "> ", __LINE__, __FILE__, __func__, "ConfObject");
						return ext.markers.sgLine.value;
					}
					break;
				default:
					return targeted_ext;
				}
			}
		}
	}
	log_ptr->logWarning("Extension doesn't provide supported properties", __LINE__, __FILE__, __func__, "ConfObject");
	return "";
}

// Returns a list of the supported extensions
const std::string ConfObject::get_supported_ext_list()
{
	string output;
	for (unsigned int i = 0; i < extension_vect.size(); i++) {
		for (unsigned int j = 0; j < extension_vect[i].extension.size(); j++) {
			output += extension_vect[i].extension[j] + " ";
		}
	}
	return output;
}


// Returns the bloc comment starting tag (= opening tag)
const string ConfObject::get_bloc_comment_start(string targeted_ext) {
	return get_ext_property(targeted_ext, SupportedExtension::properties::Bloc_Start);
}

// Returns the bloc comment ending tag (= closing tag)
const string ConfObject::get_bloc_comment_end(string targeted_ext) {
	return get_ext_property(targeted_ext, SupportedExtension::properties::Bloc_End);
}

// Returns the single line comment tag
const string ConfObject::get_single_line_com(string targeted_ext) {
	return get_ext_property(targeted_ext, SupportedExtension::properties::Single_Comment);
}

ProtoTag* ConfObject::get_tag(string tag_name) {
	for (unsigned int itr = 0; itr < tags_vect.size(); itr++) {
		if (tags_vect[itr]->name == tag_name) {
			return tags_vect[itr];
		}
	}
	return nullptr;
}

// Add an element to extension_vector of ConfObject.
// If an extension overlaps a previously added one, then it will be overwritten by the new one
void ConfObject::add_element(SupportedExtension new_language_spec) {
	logger::Logger* log_ptr = logger::getLogger();
	
	// look for previous extension with the same name and overwrite it with new data
	for (auto& ext : extension_vect) {
		if (ext.extension == new_language_spec.extension) {
			ext = new_language_spec;
		}
	}
	log_ptr->logDebug("Add supported extension to vector", __LINE__, __FILE__, __func__, "ConfObject");
	// No extension with the same name was found -> we can safely add a new one
	extension_vect.push_back(new_language_spec);
}

// Returns a copy of a supported extension which contains a given _ext
SupportedExtension ConfObject::find_language_spec(std::string _ext)
{
	for (unsigned int i = 0; i < extension_vect.size(); i++) {
		if (extension_vect[i].match_ext(_ext)) {
			return extension_vect[i];
		}
	}
	return SupportedExtension("", "", "", "");
}


ConfObject* ConfObject::getConfig()
{
    if (_instance == nullptr)
    {
        _instance = new ConfObject();
    }

    return _instance;
}

void ConfObject::removeConfig()
{
    if (_instance != nullptr)
    {
        delete _instance;
        _instance = nullptr;
    }
}