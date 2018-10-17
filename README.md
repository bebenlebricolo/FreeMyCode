# FreeMyCode
## Generalities
FreeMyCode is a command line tool which allows to write a specific pattern of text (such as a license notice header ) in all files of a given directory, recursively.
Some more informations could be included, in the form of a yaml-like data structure, directly in native comments in each file, allowing the user to add specific informations about its project.

## Key Features
### Wide range of supported languages :
This piece of software handles most common types of languages (such as C, C++, Python, C#, Html, Xml, etc.) and is easily configurable via a configuration file, written in JSON.

### Provides a simplified command line interface written in Python3
FreeMyCode project embeds many scripts which are used to launch the main executable as well as library testers.
Those scripts are written in Python3, or in Linux shell script / Batch depending on the targeted platform.
Input is as simple as : python3 LaunchFreeMyCode.py <Install directory> <Targeted Directory> <User ressources directory>

### Automatically detects previously licensed files
This feature is one the most critical ones, as it provides a safe way to protect external libraries which have already been licensed by their authors. It detects comments blocks (single line or block comments if available in currently targeted language) and compares it sequentially with licenses spectrums in database (/Install/Ressources/Spectrums)

Note : It is still not possible to manually exclude directories, this feature should be added soon. All parsed files may contain a notice header.

Note 2 : FreeMyCode could only detect licenses which it already knows about (please refer to Install/Ressources/AvailableLicenses/ directory to see what licenses notice headers have already been evaluated.

### Multi-platform support
FreeMyCode is available on Microsoft Windows 10 platform and GNU/Linux platforms (tested under Debian and Ubuntu, Virtual machines &  native ). It uses CMake as a multi-platform compilation tool.
Supports Microsoft Visual Studio project files & Makefiles.

## External libraries used :
  - Rapidjson  : [Github repo](https://github.com/Tencent/rapidjson/)
### Functionalities to be implemented: (TODO)
- [x] Check for previous license files
   * Prevent re-licensing of a file
   * Compute Licenses spectrums
   * Embed all known licenses spectrums in one file (Licences.spec)
- [x] Prevent multiple license writing in a file
- [ ] Implement GUI with Qt
- [x] Use tags to add specific content
    ```
    FreeMyCode :
       Version : 0.1
       Author : bebenlebricolo
       Url : https://github.com/bebenlebricolo/FreeMyCode
    ```
- [x] Finalise Secondary Input tags formatting
- [x] Supports FreeMyCode specific tags as well (FreeMyCode version)
- [ ] Exclude directories functionality (exclude path) 
   * This is to prevent re-licensing of external libs, for example
- [x] Add the LicenseWriter component
   * Writes license file according to the parsed arguments
  
### Corrections and improvements : (TODO)
- [ ] Harmonize logger usage
     * Upgrade logger as singleton
     * Do not pass logger ptr as parameter anymore (unnecessary noise)
 - [x] Input simplification (make it easier to use !)
     * [x] Switch input scripts (.bat / .bash) to Python3
          - More flexible and provides support for more complex input algorithms (path checks, automatically retrieves generic files such as config.json / Secondary Input.json & other
 
## Cross platform adaptations
- [x] Provide gcc support
     * Create MakeFiles
- [x] Port code to GNU/Linux systems
- [x] Provide back support for Windows systems after code adaptations (CMake used)
     * [x] Repair compilation issues such as filesystem specific handlings
