/*
Configuration tools that might be used for configuration files handling.
It basically relies on the rapidjson implementation

----------------------------------------------------------------------------
Version	|	 Date	 |	Comments
----------------------------------------------------------------------------
0.1     | 04/04/2018 | Wrote the ConfObject parse_conf_file method and implemented first methods of classes

*/


#include "stdafx.h"
#include "ConfigTools.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <streambuf>

namespace fs = std::experimental::filesystem;
using namespace std;

static const char* CONF_NODE = "Configuration";
static const char*  LANG_NODE = "Languages";
static const char*  LANG_SL_COM = "Single line comment";
static const char*  LANG_BL_COM_OP = "Bloc comment opening";
static const char*  LANG_BL_COM_CL = "Bloc comment closing";
static const char*  LANG_EXT_ARRAY = "Extension array";
static const char*  LANG_EXT_NODE = "Extension";




SupportedExtension::SupportedExtension(
	std::string _name,
	std::string line_com,
	std::string bloc_start,
	std::string bloc_end) :
	extension(_name),
	single_line_comment(line_com),
	bloc_comment_start(bloc_start),
	bloc_comment_end(bloc_end){}


#ifdef USE_JSON_CPP
// Attempt to use the JsonCpp external library
bool ConfObject::parse_conf_file(std::string in_filepath) {
	if (fs::exists(in_filepath)) {
		// Open file as ifstream
		std::ifstream in(in_filepath);
		Json::Value root;
		in >> root;

		const Json::Value languages = root["Languages"];
		for (unsigned i = 0; languages.size(); i++) {
			const Json::Value extension = languages[i]["extension"];
			const Json::Value bloc_com_start = languages[i]["Bloc comment opening"];
			const Json::Value bloc_com_end = languages[i]["Bloc comment closing"];
			const Json::Value line_com = languages[i]["Single line comment"];
			extension_vect.push_back(
				SupportedExtension(
					extension.asString(),
					line_com.asString(),
					bloc_com_start.asString(),
					bloc_com_end.asString()
				));
		}
		in.close();
		return true;
	}
}
#else
// Use Rapidjson instead
#include "rapidjson\filereadstream.h"
#include "rapidjson\document.h"
#include "rapidjson\istreamwrapper.h"


using namespace rapidjson;

// Parses a file using the rapidjson library
bool ConfObject::parse_conf_file(std::string filepath) {
	if (!fs::exists(filepath)) {
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

	// If we found a Configuration (root) node
	if (doc.HasMember(CONF_NODE)){
		rapidjson::Value *config_node = &(doc[CONF_NODE]);

		// If we found a Language node in the config file
		if (config_node->HasMember(LANG_NODE)) {
			rapidjson::Value *languages_node = &((*config_node)[LANG_NODE]);

			// Iterate over found languages
			for (SizeType i = 0; i < languages_node->Size(); i++) {
				// If we found an extension array :
				if ((*languages_node)[i].HasMember(LANG_EXT_ARRAY)) {
					rapidjson::Value* Ext_array = &((*languages_node)[i][LANG_EXT_ARRAY]);
			
					if ((*languages_node)[i].HasMember(LANG_SL_COM)) sl_com = (*languages_node)[i][LANG_SL_COM].GetString();
					if ((*languages_node)[i].HasMember(LANG_BL_COM_OP)) bl_com_st = (*languages_node)[i][LANG_BL_COM_OP].GetString();
					if ((*languages_node)[i].HasMember(LANG_BL_COM_CL)) bl_com_end = (*languages_node)[i][LANG_BL_COM_CL].GetString();

					// Iterate over extensions array
					for (unsigned int ext = 0; ext < Ext_array->Size(); ext++) {
						cur_ext = (*Ext_array)[ext].GetString();
						SupportedExtension new_ext(cur_ext,sl_com,bl_com_st,bl_com_end);
						extension_vect.push_back(new_ext);
					}
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
			}
			// Parsing successfull
			return true;
		}
		else {
			// Parsing unsuccessfull -> no "Language" node found
			return false;
		}
	}
	
}


#endif

// Simply look for a given extension in memory.
// If the passed extension matches at least one extension in the ConfObject, return true.
bool ConfObject::is_extension_supported(string extension) {
	for (auto& ext : extension_vect) {
		if (ext.extension == extension) return true;
	}
	return false;
}

const string ConfObject::get_ext_property(string targeted_ext, SupportedExtension::properties prop_type) {
	for (auto& ext : extension_vect) {
		if (ext.extension == targeted_ext) {
			switch (prop_type) {
			case SupportedExtension::properties::Bloc_End:
				return ext.bloc_comment_end;
				break;
			case SupportedExtension::properties::Bloc_Start:
				return ext.bloc_comment_start;
				break;
			case SupportedExtension::properties::Single_Comment:
				return ext.single_line_comment;
				break;
			default:
				return targeted_ext;
			}
		}
	}
	// TODO find a way to (safely) handle the case where targeted_ext doesn't match anything
	return "";
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

// Add an element to extension_vector of ConfObject.
// If an extension overlaps a previously added one, then it will be overwritten by the new one
void ConfObject::add_element(SupportedExtension new_language_spec) {
	string new_ext = new_language_spec.extension;
	// look for previous extension with the same name and overwrite it with new data
	for (auto& ext : extension_vect) {
		if (ext.extension == new_ext) {
			ext = new_language_spec;
		}
	}
	// No extension with the same name was found -> we can safely add a new one
	extension_vect.push_back(new_language_spec);
}