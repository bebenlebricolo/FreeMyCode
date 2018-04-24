#ifndef CONFIGTOOLS_HEADER
#define CONFIGTOOLS_HEADER

#include "stdafx.h"
#include <vector>
#include "LoggingTools.h"

struct SupportedExtension {
	SupportedExtension(std::vector<std::string> _ext_list, std::string line_com, std::string bloc_start, std::string bloc_end);
	SupportedExtension(std::string _single_ext, std::string line_com, std::string bloc_start, std::string bloc_end);
	enum properties{ Single_Comment, Bloc_Start , Bloc_End};
	std::vector<std::string> extension;
	std::string single_line_comment;
	std::string bloc_comment_start;
	std::string bloc_comment_end;
	bool match_ext(std::string _ext);
};


class ConfObject {
	std::vector<SupportedExtension> extension_vect;
	// general template used to get a property inside config file
	const std::string get_ext_property(std::string targeted_extension, SupportedExtension::properties prop_type);
	logger::Logger* log_ptr;
public:
	const std::string get_supported_ext_list();
	ConfObject(logger::Logger* new_logger = NULL);
	~ConfObject();
	bool parse_conf_file(std::string in_filepath);
	bool is_extension_supported(std::string extension);
	const std::string get_bloc_comment_start(std::string targeted_extension);
	const std::string get_bloc_comment_end(std::string targeted_extension);
	const std::string get_single_line_com(std::string targeted_extension);

	void add_element(SupportedExtension new_language_spec);
	SupportedExtension find_language_spec(std::string _ext);
	//void write_conf_file(std::string out_filepath);
	

};


#endif // !CONFIGTOOLS_HEADER