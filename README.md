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
 ________
# Building process
## Prerequisite
* CMake version 3.1
* Linux : GNU/GCC-G++ > 6  (check with gcc --version )
* Windows : Microsoft Visual Studio > 19
* Python 3 ( Multi-Platform Script )
## Step-by-step
* Clone the repo on your workspace
* make a new directory build/
* change directory to build/
* run ``cmake ../``
* Linux : run ``make install``  (if build completed, you should have a new FreeMyCode/Install/ folder)
* Windows : open project located under build/Project.sln (Project msvc solution file)
    * -> click on ALL_BUILD target, choose your release type and generate solution
    * -> Under CMake panel, look for Install > Project
    * -> Same as for Linux build process :  new directory Install sould be present in your repo
# Running the application
* Locate Install folder of FreeMyCode (if you get the Install folder as an archive, maybe you would like to look for a "FreeMyCode" install folder on your machine)
* Create a new directory next to your project's folder to be machined/licensed by FreeMyCode E.g: "UserRessourcesDir"
* Copy Secondary_input.template.json as Secondary_input.json into this folder (you may find the template file under <InstallFolder>/Ressources/Secondary_input.template.json)
* Modify this file and add your own kind of nodes (simply respect json format, you can use [this tool](https://jsonformatter.curiousconcept.com) for help )
* Copy the license you would like to use as License.txt into this folder (fill it with whatever text you want, or pick a license notice header under <InstallFolder>/Ressources/AvailableLicenses )
* Change dir to your user directory (or where you like) and run 
    ```python3 <LaunchFreeMyCode.py> <TargetedDirectory> <User Dir> ```
  * Command should look like something like this :
    `python.exe .\Scripts\Multi-Platform\LaunchFreeMyCode.py .\Install\ .\Sources\ .\AutoLicensingOutput\ `
  
  *Note : relative paths are supported by the Python script (supports only patterns like `. .. ./<somewhere> ../<somewhere> ./../../<somewhere> and <somewhere>/ `*
 
## More complex input : using special flags
LaunchFreeMyCode script has been written with (relative) simplicity in mind ; that is, it allows the user to lay back by dynamically retrieving required files to pass to FreeMyCode.
However, to get more control over FreeMyCode, it is still possible to modify how LaunchFreeMyCode passes arguments to FreeMyCode executable and trigger other behaviours such as force logging , force single line comments, etc.
A complete list of the flags and their effects is available both in LaunchFreeMyCode script (at the end), or directly from FreeMyCode executable, by running FreeMyCode -u or issuing a wrong command directly to it.

___

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
- [x] Harmonize logger usage 
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
