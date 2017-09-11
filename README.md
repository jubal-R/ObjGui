# ObjGui
ObjGui is a GUI disassembler and binary analysis tool utilizing GNU binutils. ObjGui uses objdump to disassemble executables, analyzes the data, and then provides an easy way to navigate through the disassembly. ObjGui is meant to be a handy tool for debugging and reverse engineering.

## Screenshot
![ObjGui](https://github.com/jubal-R/ObjGui/blob/master/screenshots/objGui.png)

## Features
- Sidebar navigation to jump between functions
- Go to address feature to follow jumps and calls
- Get file offset of any line of disassembly
- Find all calls to any address or function
- Search disassembly data
- Support for custom objdump binaries

## TODO
- Function renaming
- Stripped binary analysis
- Psuedo code
- Patching of binaries

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

## Acknowledgements
### Fonts
Noto Sans <https://www.google.com/get/noto/>  
Anonymous Pro <http://www.marksimonson.com/fonts/view/anonymous-pro>
### Icons
<https://material.io/icons>

## License
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.