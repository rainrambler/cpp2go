# Seccheck

## About

Seccheck is a security analysis tool for C and C++ code.
The original code repository is based on Cppcheck source code.

## Manual

seccheck [options] [path or source code files]

## Compiling

Any C++ compiler should work.

To build the GUI, you need Qt.

When building the command line tool, [PCRE](http://www.pcre.org/) is normally used.
PCRE is optional.

There are multiple compilation choices:
* qmake - cross platform build tool
* Windows: Visual Studio or Qt Creator or MinGW
* gnu make
* g++

### qmake

You can use the gui/gui.pro file to build the GUI.

```shell
cd gui
qmake
make
```

### Visual Studio 2010

Use the seccheck_vs2010.sln file. The rules are normally enabled.

To compile with rules (PCRE dependency):
* the PCRE dll is needed. It can be downloaded from [here](http://cppcheck.sourceforge.net/pcre-8.10-vs.zip).

To compile without rules (no dependencies):
* remove the preprocessor define `HAVE_RULES` from the project
* remove the pcre.lib from the project

## Webpage

https://github.com/rainrambler/seccheck

