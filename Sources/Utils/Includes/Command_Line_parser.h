#include <vector>
#include <string>
#include <iostream>

#include "stdafx.h"
#include "LoggingTools.h"

using namespace std;

// Generic structure for parser flags/result(Argument)
struct GlobalHook {
	string description;
	string usage;
	GlobalHook();
	GlobalHook(string _description, string _usage);
	void help_request(unsigned int indent_spaces = 4);
	void usage_request();
};

// Parser's flag container
struct ParserFlags : public GlobalHook{
	ParserFlags(vector<string> flags_aliases ,
		string _descrption = "Standard ParserFlag description" ,
		string _usage = "Standard ParserFlag Usage line", bool is_terminal = false);
	vector<string> aliases;
	bool state;
	bool match_flag(string flag, bool auto_set = false);
	void init(vector<string> flags_aliases);
	
	// Prints out internal states of flag (activated / unactivated)
	void introspective();
	void help_request(int indent_spaces = 4);
	
	// If flag is terminal, it will tell the program not to continue execution after usage has been printed out
	bool is_terminal();

private:
	string description;
	bool terminal_flag = false;
};

// Parser's result container. 
// Embeds data about argument (such as file path) and some flags which are related to its function
// E.g : logger parser result will embed logging file path and logging flags such as 'Verbose' , 'Force debug output', etc.
struct ParserResult : public GlobalHook{
	ParserResult();
	ParserResult(string _name,  string _description, string _usage);
	
	string raw_args;
	vector<ParserFlags> available_flags;
	
	bool contain_flag(string flag);
	bool flag_state(string flag);
	void set_flag(string flag);
	bool is_full();
	void set_arg(string arg);
	void introspective();
	void overrideFlag(string flagName, bool flagState);

	void help_request(unsigned int indent_spaces = 4);
	void usage_request();
	string get_arg();
	bool match_name(string _name);
	bool has_terminal_flags();

protected :
	string name;
	string arg;
	bool full;
};

// Class implementation of our CommandLineParser
// Embeds parser results as well as printing facilities (could print all activated flags, all filled args, etc.)
class CommandLineParser {
		
	vector<ParserResult*> Result;
	string all_flags;
	string all_args;

	ParserResult Global_flags;
	bool is_a_flag(string parsed_arg);
	bool is_flag_valid(string _parsed_arg, ParserResult** _temp_PR);
	int find_next_PR_index(int _target_id);
	void push_arg(ParserResult** target, string argument);
	void push_flag(ParserResult** target, string flag);
	bool found_globals(int argc, char * argv[]);
	bool found_terminals();
	void show_globals();
    static CommandLineParser *instance;
    CommandLineParser();
	~CommandLineParser();

public:
	bool parse_arguments(int argc, char * argv[]);
	void show_results();
	bool get_flag(string flag);
	string get_arg(string name);
	void overrideFlag(string flagName, bool flagState);
	void add_container(std::vector<ParserResult*>* Result_vector);
	void add_container(ParserResult* Result_object);
	ParserResult* find_owner(string flag);

    static CommandLineParser *getParser();
    static void destroyParser();
};




