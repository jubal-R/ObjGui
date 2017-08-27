# ObjGui
ObjGui is a GUI frontend for objdump made for readability and easy analysis. ObjGui uses objdump to disassemble executables, then provides an easy way to navigate through the disassembly. ObjGui is a handy tool for debugging and reverse engineering.

## Screenshot
![ObjGui](https://github.com/jubal-R/ObjGui/blob/master/screenshots/objGui.png)

## Features
- Sidebar navigation to jump between functions
- Support for custom objdump binaries
- Support for many objdump flags(syntax, demangle, etc.)
- Output separated into tabs
- Highlighting for readability

## Dependencies
qt5-default qt5-qmake objdump

## Building ObjGui
`git clone https://github.com/jubal-R/ObjGui.git`  
`mkdir build`  
`cd build`  
`qmake -qt=qt5 ../ObjGui/src/ObjGui.pro`  
`make`

## Supported File Formats
ELF, PE, Macho, etc.  
ObjGui will disassemble any file format supported by the objdump executable you point it at. If you need to disassemble a binary for any file formats not supported by your default objdump install, then you can compile your own objdump executable configured for your needs.  
objdump is part of GNU binutils and can be found here <https://ftp.gnu.org/gnu/binutils/>

## To Be Added
- Follow jump/call instructions
- Export disassembly
- Additional user preferences
- Searching

## Acknowledgements
Icons: <https://material.io/icons>

## License
ObjGui is licensed under the GNU General Public License version 3.0.  
<https://www.gnu.org/licenses/gpl-3.0.en.html>
