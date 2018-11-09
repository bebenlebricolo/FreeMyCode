#ifndef CONFIGTOOLS_HEADER
#define CONFIGTOOLS_HEADER

#include "stdafx.h"
#include <vector>
#include <string>
#include "LoggingTools.h"
#include "FormattingUtils.h"

using namespace FormattingTags;

struct CommentTag {
    enum commentType { single, block ,unknown};
    std::string name;
    std::string value;
    commentType type;
    CommentTag() : name(""), value("") , type(unknown) {};
    CommentTag(std::string n, std::string v, commentType _t) : name(n) , value(v) , type(_t){}
};

// Simple comment markers pack
struct CommentMarkers
{
    CommentTag sgLine;
    CommentTag bStart;
    CommentTag bEnd;
    bool isPlainText;
	
    bool checkIfPlainText();
    CommentMarkers();
    CommentMarkers(std::string _single_line_comment, std::string _block_comment_start, std::string _block_comment_end);

    void reset();
    void vectorizeMembers(std::vector<CommentTag> *vec);
    bool checkForMissingCommentMarker(ostringstream *errorMessage);
};


struct SupportedExtension {
	SupportedExtension(std::vector<std::string> _ext_list, std::string line_com, std::string bloc_start, std::string bloc_end);
	SupportedExtension(std::string _single_ext, std::string line_com, std::string bloc_start, std::string bloc_end);
	enum properties{ Single_Comment, Bloc_Start , Bloc_End};
	std::vector<std::string> extension;
	CommentMarkers markers;
	bool match_ext(std::string _ext);
};

struct FormattingTag {
	std::string name;
	std::vector<std::string> values;
	FormattingTag(std::string _name, std::vector<std::string> _values) :name(_name), values(_values) {}
};

class ConfObject {
	std::vector<SupportedExtension> extension_vect;
	// general template used to get a property inside config file
	const std::string get_ext_property(std::string targeted_extension, SupportedExtension::properties prop_type);
	std::vector<ProtoTag*> tags_vect;
    static ConfObject* _instance;
	ConfObject();
	~ConfObject();
public:
	const std::string get_supported_ext_list();
	bool parse_conf_file(std::string in_filepath);
	bool is_extension_supported(std::string extension);
	const std::string get_bloc_comment_start(std::string targeted_extension);
	const std::string get_bloc_comment_end(std::string targeted_extension);
	const std::string get_single_line_com(std::string targeted_extension);
	FormattingTags::ProtoTag* get_tag(std::string tag_name);

	void add_element(SupportedExtension new_language_spec);
	SupportedExtension find_language_spec(std::string _ext);
	//void write_conf_file(std::string out_filepath);
    static ConfObject* getConfig();
    static void removeConfig();


};



#endif // !CONFIGTOOLS_HEADER