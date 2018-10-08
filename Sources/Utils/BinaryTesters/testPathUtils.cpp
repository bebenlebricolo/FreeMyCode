#include "stdafx.h"
#include <iostream>
#include "PathUtils.h"
#include FS_INCLUDE

namespace fs = FS_CPP;
static const char* valid_options = "-j";
using namespace std;
namespace pu = pathutils;

static void printUsage();
static void joinPaths(int argc, char **argv);;
//static vector<string> filterInput(int argc, char** argv);

void parse_option(int argc, char* argv[])
{
    if (argc > 1)
    {

        if (argv[1][0] == '-')
        {
            // option detected
            switch (argv[1][1])
            {
            case 'j':
                joinPaths(argc, argv);
                break;
            case 'h':
            case 'u':
            default:
                printUsage();
                break;
            }
        }
        else
        {
            cerr << "Wrong option detected. Please use one of the following options with the '-' starting character : " << valid_options << endl;
            printUsage();
        }
    }
    else
    {
        cerr << "No input detected. Please provide arguments to command" << endl;
        printUsage();
    }
}


int main(int argc, char* argv[])
{

    parse_option(argc, argv);
    cin.ignore();
    return 0;
}

void printUsage()
{
    cout << "Usage : " << endl;
    cout << " -j \"<path 1>\" \"<path 2>\" ... : joins filesystem paths" << endl;
    cout << " -u / -h                  :  Displays this help " << endl << endl;

}


/*vector<string> filterInput(int argc, char** argv)
{
    vector<string> fileList;
    for (int i = 2; i < argc; i++)
    {
        string arg = argv[i];
        if (!fs::exists(arg))
        {
            continue;
        }
        fileList.push_back(arg);
    }
    return fileList;
}*/

void joinPaths(int argc, char** argv)
{
    cout << "Starting test tool for PathUtils library : Joining paths together" << endl << endl;
    if (argc < 4)
    {
        cerr << "Please provide at least two file paths." << endl;
    }
    else
    {
        cout << "Path 1 = " << argv[2] << endl;
        cout << "Path 2 = " << argv[3] << endl;

        cout << "Output path = \"" << pu::join(argv[2], argv[3]) << "\""<< endl;
        cout << "Ending program." << endl;
    }
}
