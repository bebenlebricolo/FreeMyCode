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


#define EXPERIMENTAL
#ifdef EXPERIMENTAL
    #include <experimental/filesystem>
#else
    #include <filesystem>
#endif // !EXPERIMENTAL
#include <fstream>
#include <iostream>



// TODO: faites référence ici aux en-têtes supplémentaires nécessaires au programme