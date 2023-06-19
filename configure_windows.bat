mkdir bld
mkdir bld\windows
cd bld\windows
cmake.exe -G "Visual Studio 15 2017" -DMML_CONSOLE:INTERNAL=1 -DMML_SSL:INTERNAL=1 ..\..\
cd ../../
