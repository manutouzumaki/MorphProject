@ECHO OFF
IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
cl -nologo /Zi ..\code\win32main.cpp User32.lib Winmm.lib d3d11.lib d3dcompiler.lib
popd
