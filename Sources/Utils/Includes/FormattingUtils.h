#ifndef FORMATTING_UTILS
#define FORMATTING_UTILS

#include <sstream>
#include <vector>
#include <string>

#include "rapidjson.h"
#include "document.h"

using namespace std;

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
private:
	char _delimiter;
public:
	char delimiter() const;
	void set_delimiter(char newDelim);
	Indent indent;
	Formatter();
};

// ############################
// Secondary Input tags data structure
// ############################

namespace FormattingTags
{
	struct ProtoTag {
		enum TagType {
			Line,
			Array,
			Object
		};
		std::string name;
		TagType type;
		ProtoTag(std::string &_name, TagType _type);
		ProtoTag(const char* _name, TagType _type);
		string printNameAndDelim(Formatter &_format);
		virtual ostringstream* buildFormattedBlock(Formatter &_format) = 0;
		virtual std::string getProperty(std::string name) = 0;
	};
	struct TagLine : public ProtoTag {
		std::string value;
		TagLine(std::string &_name, std::string &_value);
		TagLine(const char* _name, const char* _value);
		ostringstream* buildFormattedBlock(Formatter &_format);
		std::string getProperty(std::string name);
	};

	struct TagArray : public ProtoTag {
		std::vector<std::string> val;
		TagArray(std::string &name, std::vector<std::string> &data);
		ostringstream* buildFormattedBlock(Formatter &_format);
		std::string getProperty(std::string name);
	};

	struct TagObject : public ProtoTag {
		std::vector<TagLine*> keys;
		std::vector<TagArray*> arrays;
		std::vector<TagObject*> obj;
		TagObject(std::string &name);
		~TagObject();
		ostringstream* buildFormattedBlock(Formatter &_format);
		std::string getProperty(std::string name);
	};

	TagLine* parseLine(rapidjson::Value::ConstMemberIterator &itr);
	TagObject* parseObject(rapidjson::Value::ConstMemberIterator &itr);
	TagArray* parseArray(rapidjson::Value::ConstMemberIterator& itr);
	// Returns a pointer to the corresponding node (if any)
	//std::string extractProperty(ProtoTag* inputTag, std::string &name);
}






#endif