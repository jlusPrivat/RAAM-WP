# RAAM-WP
Remote Application Audio Mixer - Windows Program

RAAM is a collection of softwares that enable users to remotely control audio
settings via a network connection. It may be useful for Windows users, who
intend to level their games volume with the volume of a communication software
without switching to sndvol. RAAM is devided into multiple softwares:
	
* **RAAM-WP**: RAAM - Windows Program is the server software, to which a
client can connect, to be able to change the session and device volumes.
Only combatible with Windows 10.
* **RAAM-LP**: RAAM - Linux Program is the server software, to which a
client can connect, to be able to change the session and device volumes.
*Not in development yet*
* **RAAM-AA**: RAAM - Android Application is a client software, with which
the volumes can be remotely managed. *Not in development yet*
* **RAAM-IA**: RAAM - iOS Application is client software, with which
the volumes can be remotely managed. *Not in development yet*


## How to use
*Currently there are no ready to use applications.*


## How to contribute
Any positive contribution is highly welcomed. Currently only RAAM-WP with
the underlying protocol specifications exists. The focus is now on creating an
Android App for employing this potential and fixing / improving this software.

Server specs can be found
[here](https://github.com/jlusPrivat/RAAM-WP/blob/master/doc/serverCommands.md).

Please do not hesitate to contact me in case you wish to contribute.


### How to build
RAAM-WP is written in C++ using Qt. As it is common, you need to run qmake,
nmake (or similar) and copy the resources found in `resources/libs`
to the directory containing the executable. QMake should call
windeplyqt automatically.

### How to generate documentation
The Doxygen style was used to document classes and methods.

### How to translate
This software uses the translation mechanism of Qt - Linguist. To add a new
translation, you need to follow these steps:
1. In `src/RAAM-WP.pro` add another entry for the `TRANSLATIONS` variable
as seen above.
2. In `src/controller/language.cpp` add another line to the Language class
static member list `languages` like the previous examples in that file.
Make sure to manuall keep an alphanumeric sorting on that list based on the
full names.
3. Add a language specific Icon to `resources/imgs`. Make sure to name like
`flag-{short name as of step 2}.png`. Good, libre icons with a uniform design
can be found on [wikimedia](https://commons.wikimedia.org).
4. In `resources/resources.qrc` add the name of the flag icon
5. Run `lupdate -none` (the argument `none` ommits the line numbers in the
translation files. That help keeping a clean git track.
6. Translate the newly created language file using Linguist.
7. Run `lrelrease` to create/update the binary language files.
8. Recompile project and test results.

If any assistance is required, to not hesitate to contact me.

### Useful resources
* [Microsoft Core Audio APIs](https://docs.microsoft.com/en-us/windows/win32/coreaudio/core-audio-apis-in-windows-vista)
* (*My Bachelors thesis on this subject in German is still pending*)

### Checklist for publishing a release
1. Set Version in `src/utils/makros.h`
2. Set Version in `src/RAAM-WP.pro`
3. Rebuild project
4. Package in installer (*no concept yet*)
5. Tag current git master commit with version number
6. Add GitHub release with binaries
7. Change Version on version check server
