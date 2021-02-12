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

/*
Here below are the utilities to parse command line strings and remaps informations into the relevant container.
It includes a directory parser result, a License parser result.
Each ParserResult class has dedicated flags which handles the parsed flags and activates relevant booleans.

----------------------------------------------------------------------------
  Version	|	 Date	 |	Comments
----------------------------------------------------------------------------
    0.1     | 16/03/2018 |	First functional version of this tool. Some flaws are still noticeable (all commands are not parsed correctly yet)
    0.11    | 16/03/2018 |	Now supports almost any configuration, but args should be put in a special order to be parsed correctly. Flags are insensitive top positioning
    0.12    | 17/03/2018 |	Fixed bug when "directory license config Logfile" doesn't parse "config".
    0.13	| 19/03/2018 |  Created two CommandLineParser methods (push_arg and push_flag) to make the code shorter
    0.14	| 19/03/2018 |  Implemented first support for help requests and usage requests (-h --help -U --Usage)
                                -> Note : Still some work to do for it to catch everything : directory(ok) license(ok) config(ok) logfile (ko)
    0.15	| 20/03/2018 |  Fixed last bug and implemented CommandLineParser :: get_arg & get_flag
    0.16	| 22/03/2018 |  Added external initiaisation functionalities: it is now possible to init the parser from outside this file
                             -> Simplifies usage and extends compatibility (all ParserResult are direct ParserResult objects, and not classes which inherits from it)
    0.20	| 14/04/2018 |  Added logging facilities
    0.21	| 30/04/2018 |  Fixed a bug occuring when too much arguments is passed to the parser. If so, all "overflowing inputs" are discarded
*/


#include "stdafx.h"
#include <iostream>
#include <vector>
#include "CommandLineParser.h"
#include FS_INCLUDE

using namespace std;
namespace fs = FS_CPP;

static void indent(unsigned int spaces = 0) {
    string indent_line = "";
    for (unsigned int i = 0; i < spaces; i++) {
        indent_line += " ";
    }
    std::cout << indent_line;
}

static void print_line(unsigned int indent_spaces = 4, unsigned int length = 40, const char print_character = '-') {
    string line;
    for (unsigned i = 0; i < length; i++) {
        line += print_character;
    }
    indent(indent_spaces);
    cout << line << endl;
}

CommandLineParser* CommandLineParser::instance = nullptr;

CommandLineParser::CommandLineParser() {
    Global_flags.available_flags.push_back(ParserFlags({ "-h","--help" }, "help flag : displays the help section", "", true));
    Global_flags.available_flags.push_back(ParserFlags({ "-U","--Usage" }, "Usage flag ; displays the usage section", "", true));
    Global_flags.available_flags.push_back(ParserFlags({ "-sr" , "--show-results" }, "Show results flag : displays current state of flags and args", ""));
}
CommandLineParser* CommandLineParser::getParser()
{
    if (instance == nullptr)
    {
        instance = new CommandLineParser;
    }
    return instance;
}

// Parse the given arguments and checks if at least one flag is considered as a global flag
// E.g: "Help , Usage" are global flags. They do not apply to any specific target
bool CommandLineParser::found_globals(int argc, char * argv[]) {
    logger::Logger *logsys = logger::getLogger();
    bool temp_result = false;
    for (int i = 0; i < argc; i++) {
        string flag = string(argv[i]);
        // If it's a flag :
        if (flag[0] == '-') {
            logsys->logDebug("Found a flag <" + flag + "> ", __LINE__, __FILE__, __func__, "CommandLineParser");
            if (Global_flags.contain_flag(flag)) {
                logsys->logDebug("Flag <" + flag + "> is a Global flag", __LINE__, __FILE__, __func__, "CommandLineParser");
                Global_flags.set_flag(flag);
                temp_result |= true;
            }
            // else (it's not a flag), continue to next loop cycle
        }
    }
    return temp_result;
}

// Checks if flag is "terminal" -> if it is, then stop execution and print Usage section
bool CommandLineParser::found_terminals() {
    return Global_flags.has_terminal_flags();
}

// Show the global messages if needed
// Priorities apply between commands (Help > usage)
void CommandLineParser::show_globals() {
    logger::Logger *logsys = logger::getLogger();
    // First check for Help requests
    if (Global_flags.flag_state("--help")) {
        logsys->logDebug("Flag < --help > found", __LINE__, __FILE__, __func__, "CommandLineParser");
        string line = "##################################";
        cout << endl << line << " <<-- Help section -->> " << line << endl;
        for (unsigned int i = 0; i < Result.size(); i++) {
            Result[i]->help_request();
        }
        return ;
    }
    // Then look for usages requests
    if (Global_flags.flag_state("--Usage")) {
        logsys->logDebug("Flag < --Usage > found", __LINE__, __FILE__, __func__, "CommandLineParser");
        string line = "##################################";
        cout << endl << line << " <<-- Usage section -->> " << line << endl << endl;
        for (unsigned int i = 0; i < Result.size(); i++) {
            Result[i]->usage_request();
        }
        cout << endl << endl;
        return;
    }
}


// Checks user input. Passed args should point to something reachable (existing files / directories) for it to complete
errorType CommandLineParser::check_args()
{
    errorType _rc = NO_ERROR;
    string current_file;
    logger::Logger *logger = logger::Logger::get_logger();

    for(unsigned int i = 0 ;  i < Result.size() ; i ++)
    {
        switch(Result[i]->get_content_type())
        {
            case ParserResult::ContentType::EXISTING_FILE_OR_DIRECTORY :
                current_file = Result[i]->get_arg();
                if(fs::exists(current_file) == false)
                {
                    logger->logError("Cannot find " + current_file + ". No such file or directory here." );
                    _rc = NO_SUCH_FILE_OR_DIRECTORY;
                }
                else
                {
                    logger->logDebug("Found " + current_file + " !");
                }
                break;
            case ParserResult::ContentType::FILE_TO_BE_CREATED :
                if(fs::exists(current_file) == false)
                {
                    logger->logWarning("Logfile does not exist yet. Creation pending.");
                }
                break;
            default:
                // Regular use cases ; do not raise errors
                break;
        }
    }
    return _rc;
}

// Singleton implementation destroying feature
void CommandLineParser::destroyParser()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

CommandLineParser::~CommandLineParser() {
    for (unsigned int i = 0; i < Result.size(); i++) {
        delete(Result[i]);
    }
}

// Main method of our command line parser
// Extracts user's input and filters them by type (flag/arg); then fill
// Parser results containers with them.
bool CommandLineParser::parse_arguments(int argc, char * argv[]) {
    logger::Logger *logsys = logger::getLogger();

    // Input checking section
    // If anything goes wrong here, stop execution of program and exit.
    if(argc < 2 )
    {
        logsys->logError("No arguments given. Aborting execution");
        Global_flags.set_flag("--Usage");
    }

    // Look for global flags first
    // If found, show messages and short-circuit the parsing
    if (found_globals(argc,argv)) {
        show_globals();
    }
    if (found_terminals()) {
        return false;
    }


    // Then initialise "real" parsing work
    ParserResult* current_target;
    current_target = Result[0];
    int target_index = 0;
    ParserResult * temp_PR = NULL;
    // Used to store a string which has not been assigned to a type yet
    string previous_arg = "";

    string parsed_arg;
    for (int i = 1; i < argc; i++) {
        // String casting is necessary here to compare const char array to a const text written ( conversion differences?)
        parsed_arg = string(argv[i]);

        if (is_a_flag(parsed_arg)) {

            // Go to next argument if the flag is not valid
            if (!is_flag_valid(parsed_arg, &temp_PR)) {
                logsys->logWarning("Invalid flag <" + parsed_arg + "> ", __LINE__, __FILE__, __func__, "CommandLineParser");
                continue;
            }
            // Then flag is valid, hurray! Continue execution
            else
            {
                // If different, it means the target has changed since last use
                if (temp_PR != current_target)
                {
                    // If current target is null -> previous step might have set it to null (in the regular argument handling section)
                    if (current_target == NULL || current_target->is_full()) {
                        current_target = temp_PR;

                        // If previous_arg is not null and current target is not full -> the last regular arg had no target when parsed!
                        // Then assign the previous arg to the new current_target.
                        if (previous_arg != "" && !current_target->is_full()) {
                            push_arg(&current_target, previous_arg);
                            previous_arg = "";
                        }
                        push_flag(&temp_PR, parsed_arg);
                        // Then continue to next loop
                        continue;
                    }
                    else {
                        current_target = temp_PR;
                        push_flag(&temp_PR, parsed_arg);
                        // Then continue to next loop
                        continue;
                    }
                }
                else {
                    push_flag(&temp_PR, parsed_arg);
                    // Then continue to next loop
                    continue;
                }
            }
        }

        // Else if not a flag -> then it's a regular argument
        else {
            if (current_target == NULL) {
                // Yes
                if (previous_arg != "") {
                    int next_PR = find_next_PR_index(target_index);
                    if (next_PR != -1) {
                        current_target = Result[next_PR];
                        push_arg(&current_target, previous_arg);
                    }
                    else {
                        Global_flags.set_flag("--Usage");
                        show_globals();
                        return false;
                    }
                }
                previous_arg = parsed_arg;
            }
            else {
                if (current_target->is_full()) {
                    current_target = NULL;
                    previous_arg = parsed_arg;
                }
                else {
                    push_arg(&current_target, parsed_arg);
                    previous_arg = "";
                }
            }
        }
    }
    // When we finished iterating on the arg vector, if something is still in the buffer
    // try to send it to the current target
    if (previous_arg != "") {
        if (current_target == NULL || current_target->is_full()) {
            int next_PR = find_next_PR_index(target_index);
            if (next_PR != -1) {
                current_target = Result[next_PR];
                push_arg(&current_target, previous_arg);
            }
            else {
                Global_flags.set_flag("--Usage");
                show_globals();
                return false;
            }
        }
    }

    // Different treatment for show result flag
    if (Global_flags.flag_state("--show-results")) {
        show_results();
    }
    return true;
}

// Pushes the new argument to the targeted parser result and reinit the pointer to Null
void CommandLineParser::push_arg(ParserResult** target, string argument) {
    (*target)->set_arg(argument);
    (*target)->raw_args += argument + " ";
    *target = NULL;
    all_args += argument + " ";
}

// Pushes the new flag to the targeted parser result. Does not reinit to Null since
// the target is temporary
void CommandLineParser::push_flag(ParserResult** target, string flag) {
    (*target)->set_flag(flag);
    (*target)->raw_args += flag + " ";
    all_flags += flag + " ";
}

// Checks if targeted string matches our registered flags structure
bool CommandLineParser::is_a_flag(string _parsed_arg) {
    if (_parsed_arg[0] == '-') return true;
    else return false;
}

// Check if the flag does exist in the list of valid flags
// if so return true
bool CommandLineParser::is_flag_valid(string _parsed_arg, ParserResult** _temp_PR) {
    if (Global_flags.contain_flag(_parsed_arg)) return true;
    *_temp_PR = find_owner(_parsed_arg);
    if (*_temp_PR != NULL ) return true;
    else return false;
}

// Find targeted flag owner (Parser Result)
// We do this to retrieve which Parser Result we are currently filling

ParserResult* CommandLineParser::find_owner(string flag) {
    for (unsigned int index = 0; index < Result.size(); index++) {
        if (Result[index]->contain_flag(flag)) return Result[index];
    }
    return NULL;
}

// Find next parser result to be filled
// Note: if the current Parser Result is full (arg section is filled), then we need to move on to the next Parser Result of the list
// Note2: if we get a flag related to another ParserResult than the current one, this is an indicator to start filling the next one
int CommandLineParser::find_next_PR_index(int _target_id) {
    logger::Logger *logsys = logger::getLogger();

    for (unsigned int i = _target_id; i < Result.size(); i++) {
        if (!Result[i]->is_full()) return i;
    }
    // We haven't found any free result!
    logsys->logError("Cannot find any ParserResult free! Program may not behave as expected! Aborting execution.", __LINE__, __FILE__, __func__, "CommandLineParser");

    return -1;
}

// Prints parser results content to stdout (debugging purpose)
void CommandLineParser::show_results() {
    cout << "\nInternal states of flags and args:" << endl ;
    cout << "---------------------------------------------" << endl;
    for (unsigned int target_index = 0; target_index < Result.size(); target_index++) {
        Result[target_index]->introspective();
    }
}

// Returns requested flag state
bool CommandLineParser::get_flag(string flag) {
    for (unsigned int R = 0; R < Result.size(); R++) {
        if (Result[R]->contain_flag(flag)) {
            return Result[R]->flag_state(flag);
        }
    }
    return false;
}

// Prints all parsed args on stdout;
void CommandLineParser::print_all_registered_args()
{
    ParserResult *_tmpResult = nullptr;
    cout << "Printing all parsed results : " << endl;
    for(unsigned int i = 0 ; Result.size() ; i++)
    {
        _tmpResult = Result[i];
        cout << "Slot " <<  _tmpResult->get_name()
             << " args : " << _tmpResult->get_arg() << endl;
    }
}

// Get argument based on the ParserResult name (string)
string CommandLineParser::get_arg(ParserResultElement _elem) {
    for (unsigned int R = 0; R < Result.size(); R++) {
        if (Result[R]->match_type(_elem)) {
            return Result[R]->get_arg();
        }
    }
    return "";
}

// Get argument based on the ParserResult name (string)
string CommandLineParser::get_arg(string name) {
    for (unsigned int R = 0; R < Result.size(); R++) {
        if (Result[R]->match_name(name)) {
            return Result[R]->get_arg();
        }
    }
    return "";
}

// Manually override flag ; some flags may be overriden by hand to tweak (hack?) program's behaviour.
// It is also a way to prevent mutually excluding flags to take effect both at the same time
void CommandLineParser::overrideFlag(string flagName, bool flagState) {
    for (unsigned int R = 0; R < Result.size(); R++) {
        if (Result[R]->contain_flag(flagName)) {
            Result[R]->overrideFlag(flagName, flagState);
        }
    }
}

// Initializes the vector all at once.
// Note : if original Result vector is not empty, it will be overwritten by the new one.
// -> If we replace a vector of pointers A by a vector B (with A != B) , then we might
// end with some objects still living in memory -> memory leaks!
void CommandLineParser::add_container(std::vector<ParserResult*>* Result_vector) {
    if (Result_vector != NULL) {
    Result = *Result_vector;
    }
}

// Push back one ParserResult at a time
void CommandLineParser::add_container(ParserResult* Result_object) {
    if (Result_object != NULL) {
        Result.push_back(Result_object);
    }
}


// ParserFlags Object definition ########################################################
// Initialize with a vector of strings with the flags (case sensitive)
ParserFlags::ParserFlags(vector<string> flags_aliases, string _description, string _usage , bool isterminal) :
    GlobalHook(_description, _usage) , state(false) , terminal_flag(isterminal){
    init(flags_aliases);
}

void ParserFlags::init(vector<string> flags_aliases) {
    for (unsigned int i = 0; i < flags_aliases.size(); i++) {
        aliases.push_back(flags_aliases[i]);
    }
}

bool ParserFlags::match_flag(string flag, bool auto_set) {
    for (unsigned int i = 0; i < aliases.size(); i++) {
        if (aliases[i] == flag) {
            state = auto_set ? true : state;
            return true;
        }
    }
    return false;
}

// Gives a representation of what is contained in this object (internal states printing)
void ParserFlags::introspective(){
    cout << "*ParserFlags : " << endl;
    cout << "	-> Aliases : ";
    for (unsigned int alias_id = 0; alias_id < aliases.size(); alias_id++) {
        cout << aliases[alias_id] << " ";
        if ((alias_id % 4) > 3) cout << endl << "                ";
    }
    cout << endl;
    cout << "	-> State : " << state << endl ;
}

// Prints the help request based on parser flags content and description
void ParserFlags::help_request(int indent_spaces) {
    // indent 4 spaces
    indent(indent_spaces);
    cout << "Aliases : { ";
    for (unsigned int a_id = 0; a_id < aliases.size(); a_id++) {
        cout << aliases[a_id] << ((a_id != aliases.size() - 1)? " , " : "");
    }
    cout <<" }" << endl;
    GlobalHook::help_request(indent_spaces);
}

bool ParserFlags::is_terminal() {
    return terminal_flag;
}

// GlobalHook Object definition ###################################################
GlobalHook::GlobalHook():
    description("Standard GlobalHook description"),
    usage("Standard GlobalHook usage string")
{}
GlobalHook::GlobalHook(string _description, string _usage) :
    description(_description),
    usage(_usage)
{}

void GlobalHook::help_request(unsigned int indent_spaces) {
    indent(indent_spaces);
    cout << "Description : " << description << endl;
}

void GlobalHook::usage_request() {
    cout << usage << " ";
}


// ParserResult Object definition ##################################################
ParserResult::ParserResult() :
    GlobalHook("Standard ParserResult description" ,
                "Standard ParserResult usage"), elem(GENERIC),full(false)
{}

// Overloaded constructor with name, description and usages.
ParserResult::ParserResult(ParserResultElement _elem,
                           string _name ,
                           ContentType _type,
                           string _description,
                           string _usage) :
                                 GlobalHook(_description , _usage),
                                 elem(_elem) ,
                                 c_type(_type),
                                 name(_name),
                                  full(false){}

// Helps to identify if arguments have been parsed properly
// Offers a simple way to debug stuff and see parsing problems.
void ParserResult::introspective() {

    cout << "\nParserResult : raw_args = " << (raw_args != "" ? raw_args : "nothing parsed") << endl;
    cout << "-> Parsed argument : " << arg << endl;
    cout << "-> is Full? : " << (full ? "true" : "false") << endl;
    for (unsigned int flag_index = 0; flag_index < available_flags.size(); flag_index++) {
        available_flags[flag_index].introspective();
    }
}

void ParserResult::overrideFlag(string flagName, bool flagState)
{
    for (unsigned int F = 0; F < available_flags.size(); F++) {
        if (available_flags[F].match_flag(flagName)) {
            available_flags[F].state = flagState;
        }
    }
}

// Returns true if the given flag matches one of the internal flag of the class.
bool ParserResult::contain_flag(string flag) {
    bool result = false;
    for (unsigned int i = 0; i < available_flags.size(); i++) {
        result |= available_flags[i].match_flag(flag);
    }
    return result;
}

// Returns the flag state.
// Usefull when we want to first parse flags and then to do specific actions if a particular flag is set
bool ParserResult::flag_state(string flag) {
    for (unsigned int i = 0; i < available_flags.size(); i++) {
        if (available_flags[i].match_flag(flag)) return available_flags[i].state;
    }
    return false;
}

// Activates flag and store result as is.
// Activated flags may be used to know which ParserResult we are supposed to find next (or provide information about the previously parsed one)
void ParserResult::set_flag(string flag) {
    for (unsigned int i = 0; i < available_flags.size(); i++) {
        available_flags[i].match_flag(flag, true);
    }
}

// Used in the CommandLineParser::parse_arguments() method to push arguments inside the targeted Result.
// Once the argument is set, consider the ParserResult to be full and force stepping to the next one
void ParserResult::set_arg(string argument) {
    arg = argument;
    full = true;
}

// Used in the CommandLineParser::parse_argument method logic to know whether this result has already been
// set in previous steps.
bool ParserResult::is_full() { return full; }


// Globals messages to be shown
void ParserResult::help_request(unsigned int indent_spaces) {
    print_line(0, 120);
    cout << "[ "<< name << " ] :" << endl;
    GlobalHook::help_request(indent_spaces);
    indent(indent_spaces);
    indent_spaces += 4;
    cout << "Available flags :" << endl;
    for (unsigned int flag_id = 0; flag_id < available_flags.size(); flag_id++) {
        available_flags[flag_id].help_request(indent_spaces);
        if(flag_id != available_flags.size() -1 ) print_line(indent_spaces, 10);
        //cout << endl;
    }
    cout << endl;
}

// Prints on the console the usage of the ParserResult and its flags.
// This method calls the method inherited and adds the Flag support functionality (propagate request downward)
void ParserResult::usage_request() {
    GlobalHook::usage_request();
}


// Returns the content type of this parser result
ParserResult::ContentType ParserResult::get_content_type() { return c_type; }

// Returns the parsed argument of the ParserResult
// Used when accessing the results after parsing
string ParserResult::get_arg() { return arg; }

// Returns the parser result's  slot name
string ParserResult::get_name() {return name; }

// Used to find a particular ParserResult by its type / element
// -> Used when we need to access the parser's results.
bool ParserResult::match_type(ParserResultElement _elem)
{
    if(_elem == elem) return true;
    else return false;
}


// Used to find a particular ParserResult by its name
// -> Used when we need to access the parser's results.
bool ParserResult::match_name(string _name) {
    if (_name == name) return true;
    else return false;
}

// If we match "Terminal Flags" -> do not even operate the application but return the flag's related content
// e.g : -h or --help may return the complete description of each flag / container to guide the user before
// completing the run.
bool ParserResult::has_terminal_flags() {
    for (unsigned int i = 0; i < available_flags.size(); i++) {
        if (available_flags[i].is_terminal() && available_flags[i].state == true) return true;
    }
    return false;
}
