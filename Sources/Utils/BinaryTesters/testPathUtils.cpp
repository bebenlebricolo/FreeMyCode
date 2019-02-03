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
