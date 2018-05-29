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
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"


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


// ############################
// Formatting stuff
// ############################

class Indent
{
private:
	unsigned int indent;
	static const unsigned int default_indent = 4;
public:
	unsigned int operator() () const;
	// postfixes operators ! Needs an (int)
	unsigned int& operator++ (int);
	unsigned int& operator-- (int);
	void reset();
	Indent();
	Indent(unsigned int &_init);
	string buildString() const;
};

class Formatter
{
private :
	char _delimiter;
public:
	char delimiter() const ;
	void set_delimiter(char newDelim);
	Indent indent;
	Formatter();
};

// ############################
// Secondary Input tags data structure
// ############################

struct ProtoTag {
	std::string name;
	ProtoTag(std::string &_name);
	ProtoTag(const char* _name);
	string printNameAndDelim(Formatter &_format);
	virtual ostringstream* buildFormattedBlock(Formatter &_format) = 0;
};
struct TagLine : public ProtoTag{
	std::string value;
	TagLine(std::string &_name, std::string &_value);
	TagLine(const char* _name,const char* _value);
	ostringstream* buildFormattedBlock(Formatter &_format);
};

struct TagArray : public ProtoTag{
	std::vector<std::string> val;
	TagArray(std::string &name, std::vector<std::string> &data);
	ostringstream* buildFormattedBlock(Formatter &_format);
};

struct TagObject : public ProtoTag {
	std::vector<TagLine*> keys;
	std::vector<TagArray*> arrays;
	std::vector<TagObject*> obj;
	TagObject(std::string &name);
	~TagObject();
	ostringstream* buildFormattedBlock(Formatter &_format);
};

// ############################
// Secondary input class declaration
// ############################


class SecondaryInput {
	ConfObject* config;
	CommandLineParser* parser;
	logger::Logger* log;
	std::vector<ProtoTag*> available_tags;
	TagLine* parseLine(rapidjson::Value::ConstMemberIterator &itr);
	TagObject* parseObject(rapidjson::Value::ConstMemberIterator &itr);
	TagArray* parseArray(rapidjson::Value::ConstMemberIterator& itr);
public:
	SecondaryInput(ConfObject* _config, CommandLineParser* _parser, logger::Logger* _log = NULL);
	~SecondaryInput();
	void parse_secondary_input_file();
	ostringstream* getTagsFormattedBlock();
};


#endif