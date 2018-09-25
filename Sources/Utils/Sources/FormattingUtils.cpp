#include "stdafx.h"
#include <algorithm>

#include "FormattingUtils.h"
#include "LoggingTools.h"
#include "Command_Line_parser.h"

using namespace FormattingTags;
using namespace rapidjson;
/*
**************************************************************************************
**************************************************************************************
Classes Formatter and Indent definitions
**************************************************************************************
**************************************************************************************
*/

Indent::Indent() : indent(default_indent) {}
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

ProtoTag::~ProtoTag() {}
ProtoTag::ProtoTag(string &_name, TagType _type) : name(_name), type(_type) {}
ProtoTag::ProtoTag(const char* _name , TagType _type) : name(_name), type(_type) {}

string ProtoTag::printNameAndDelim(Formatter &_format)
{
	string out = name + " " + _format.delimiter() + " ";
	return out;
}

TagLine::TagLine(string &_name, string &_value) : ProtoTag(_name,Line), value(_value) {}
TagLine::TagLine(const char* _name, const char* _value) : ProtoTag(_name,Line), value(_value) {}
TagLine::~TagLine(){}

ostringstream* TagLine::buildFormattedBlock(Formatter &_format)
{
	ostringstream *out = new ostringstream();
	*out << _format.indent.buildString()
		<< printNameAndDelim(_format)
		<< value
		<< endl;
	return out;
}

std::string TagLine::getProperty(std::string _name)
{
	if (_name == name) return value;
	else return "";
}

TagArray::TagArray(string &_name, vector<string> &data) : ProtoTag(_name,Array), val(data)
{}
TagArray::~TagArray(){}

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

std::string TagArray::getProperty(std::string _name)
{
	std::vector<std::string>::iterator _itr = std::find(val.begin(), val.end(), _name);
	if (_itr != val.end()) {
		return *_itr;
	}
	else return "";
}

TagObject::TagObject(string &_name) : ProtoTag(_name,Object)
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
				*out << newStream->str();
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

std::string TagObject::getProperty(std::string _name)
{
	bool foundString = false;
	string out = "";
	unsigned int itr = 0;

	// Extract from keys (single line)
	for (itr = 0;
		(itr < keys.size() && foundString == false) ;
		itr++)
	{
		out = keys[itr]->getProperty(_name);
		if (out != "") {
			foundString = true;
			break;
		}
	}
	// If unsuccessful, try with arrays
	if (foundString == false) {
		for (itr = 0;
			(itr < arrays.size() && foundString == false);
			itr++)
		{
			out = arrays[itr]->getProperty(_name);
			if ( out != "") {
				foundString = true;
				break;
			}
		}
	}

	// Finally try with objects
	if (foundString == false) {
		for (itr = 0;
			(itr < obj.size() && foundString == false);
			itr++)
		{
			out = obj[itr]->getProperty(_name);
			if (out != "") {
				foundString = true;
				break;
			}
		}
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


// ############################################
// Parsing utilities
// ############################################

TagLine* FormattingTags::parseLine(Value::ConstMemberIterator& itr)
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

TagArray* FormattingTags::parseArray(Value::ConstMemberIterator& itr) {
	TagArray* current_tag = NULL;
	if (itr->value.IsArray()) {
		logger::Logger *log_ptr = logger::Logger::get_logger();
		string current_key = itr->name.GetString();
		vector<string> array_values;
		// Iterate over members of the array
		for (SizeType j = 0; j < itr->value.Size(); j++) {
			array_values.push_back(itr->value[j].GetString());
		}
		current_tag = new TagArray(current_key, array_values);
		log_ptr->logDebug("Parsed new TagArray object < " + current_key + " >", __LINE__, __FILE__, __func__, "SecondaryInput");
	}
	return current_tag;
}


TagObject* FormattingTags::parseObject(Value::ConstMemberIterator &itr)
{
	logger::Logger* log = logger::Logger::get_logger();
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
					log->logDebug("Found new Line tag in secondary input file. Name : " + newLine->name + " ; Value : " + newLine->value, __LINE__, __FILE__, __func__);
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
			log->logError("Allocation error : cannot allocate memory for new Object tag", __LINE__, __FILE__, __func__, "SecondaryInput");
		}
	}
	return obj;
}
