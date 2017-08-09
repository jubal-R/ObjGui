# ObjGui
ObjGui is a GUI frontend for objdump made for readability and easy analysis.

## Screenshot
![ObjGui](https://github.com/jubal-R/ObjGui/blob/master/screenshots/objGui.png)

## Features
- Sidebar navigation to jump between functions
- Output seperated into tabs
- Highlighting for readability

## Dependencies
qt5-default qt5-qmake objdump

## Building ObjGui
`git clone https://github.com/jubal-R/ObjGui.git`  
`mkdir build`  
`cd build`  
`qmake -qt=qt5 ../ObjGui/src/ObjGui.pro`  
`make`

## To Be Added
- Follow jump/call instructions
- Edit and patch binaries
- User preferences
- Searching

## License
ObjGui is licensed under the GNU General Public License version 3.0.
