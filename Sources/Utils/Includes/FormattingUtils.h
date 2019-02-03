/*

------------------
@<FreeMyCode>
FreeMyCode version : 1.0 RC alpha
    Author : bebenlebricolo
    Contributors : 
        FoxP
    License : 
        name : GPL V3
        url : https://www.gnu.org/licenses/quick-guide-gplv3.html
    About the author : 
        url : https://github.com/bebenlebricolo
    Date : 16/10/2018 (16th of October 2018)
    Motivations : This is part of a Hobby ; I wanted a tool to help open-source developers to keep their hard work traceable and protected.
<FreeMyCode>@
------------------

FreeMyCode is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

// Indentation related class
// Handles basic indentation facilites such as generating indent strings (length is a multiple of indent value)
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

// Bundles which embeds a default delimiter (such as ';' ) and Indent class
// -> It is used to generate user's custom information section
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
		virtual ~ProtoTag();
		string printNameAndDelim(Formatter &_format);
		virtual ostringstream* buildFormattedBlock(Formatter &_format) = 0;
		virtual std::string getProperty(std::string name) = 0;
	};
	struct TagLine : public ProtoTag {
		std::string value;
		TagLine(std::string &_name, std::string &_value);
		TagLine(const char* _name, const char* _value);
		virtual ~TagLine();
		ostringstream* buildFormattedBlock(Formatter &_format);
		std::string getProperty(std::string name);
	};

	struct TagArray : public ProtoTag {
		std::vector<std::string> val;
		TagArray(std::string &name, std::vector<std::string> &data);
		virtual ~TagArray();
		ostringstream* buildFormattedBlock(Formatter &_format);
		std::string getProperty(std::string name);
	};

	struct TagObject : public ProtoTag {
		std::vector<TagLine*> keys;
		std::vector<TagArray*> arrays;
		std::vector<TagObject*> obj;
		TagObject(std::string &name);
		virtual ~TagObject();
		ostringstream* buildFormattedBlock(Formatter &_format);
		std::string getProperty(std::string name);
	};

	TagLine* parseLine(rapidjson::Value::ConstMemberIterator &itr);
	TagObject* parseObject(rapidjson::Value::ConstMemberIterator &itr);
	TagArray* parseArray(rapidjson::Value::ConstMemberIterator& itr);
}

#endif