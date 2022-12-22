@echo off
rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem                                Build Options
rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set game_executable=bezier_test.exe
rem set build_options= -DBUILD_DEVELOPER=1 -DBUILD_DEBUG=1 -DBUILD_RELEASE=0 -DBUILD_WIN32=1
rem set common_compiler_flags= /GL- -MTd -nologo -Gm- -GR- 
rem -Gs- -Gy- -Gw- -EHa- -Od -W3 -wd4201 -wd4100 -wd4189 /Zi -D_CRT_SECURE_NO_WARNINGS /FS %build_options% -I../../data_desk/ -I../../common/
set common_compiler_flags= /EHsc /MTd /std:c++17
set common_linker_flags= -opt:ref -incremental:no /Debug:fastlink
set libs_to_link_to= 
rem ..\thirdparty\libs\glm_static.lib ..\thirdparty\libs\fmtd.lib
rem set game_linker_flags= user32.lib gdi32.lib winmm.lib 

rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem                               Make Directories
rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem pushd data_desk
rem if not exist build mkdir build
rem if not exist generated mkdir generated
rem popd
rem pushd game
rem if not exist .\build mkdir build
rem if not exist .\build\data mkdir build\data
rem popd



rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem                           Build Game Exectuable
rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem start /b /wait "" "xcopy" .\data .\build\data /y /s /e /q
rem pushd build
echo --- Compiling game executable.

start /b /wait "" "cl" %common_compiler_flags% /I ..\thirdparty\include\ /I ..\include bezier_test.cpp ..\src\slurp.cpp ..\src\log.cpp ..\thirdparty\src\fmt\os.cc ..\thirdparty\src\fmt\format.cc /link %common_linker_flags% %libs_to_link_to% /OUT:%game_executable%

echo --- Build completed.
