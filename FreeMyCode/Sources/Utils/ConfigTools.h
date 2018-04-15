#ifndef CONFIGTOOLS_HEADER
#define CONFIGTOOLS_HEADER

#include "stdafx.h"
#include <vector>
#include "LoggingTools.h"

struct SupportedExtension {
	SupportedExtension(std::string _name, std::string line_com, std::string bloc_start, std::string bloc_end);
	enum properties{ Extension, Single_Comment, Bloc_Start , Bloc_End};
	std::string extension;
	std::string single_line_comment;
	std::string bloc_comment_start;
	std::string bloc_comment_end;
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
	//void write_conf_file(std::string out_filepath);
	

};


#endif // !CONFIGTOOLS_HEADER