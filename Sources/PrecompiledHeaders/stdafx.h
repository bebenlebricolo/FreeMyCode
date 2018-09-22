// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//



#pragma once

#include "targetver.h"

#include <stdio.h>
//#include <tchar.h>
#include <vector>
#include <string>

// Handle Microsoft Visual Studio compiler
#ifdef _MSC_VER 
    #if _MSC_VER <= 1913
        #define EXPERIMENTAL_FS
    #endif
#endif // _MSC_VER

// Handle GNU GCC / G++ Compilers versions
#ifdef __GNUC__
    #if __GNUC__ < 8
        #define EXPERIMENTAL_FS
    #endif
#endif



#ifdef EXPERIMENTAL_FS
    #include <experimental/filesystem>
    #define FS_CPP std::experimental::filesystem
#else
    #include <filesystem>
    #define FS_CPP std::filesystem
#endif // !EXPERIMENTAL
#include <fstream>
#include <iostream>

