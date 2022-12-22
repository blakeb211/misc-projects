@echo off



rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem                                Build Options
rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set game_executable=dungeoneer_win32.exe
set build_options= -DBUILD_DEVELOPER=1 -DBUILD_DEBUG=1 -DBUILD_RELEASE=0 -DBUILD_WIN32=1
set common_compiler_flags= /GL- -MTd -nologo -Gm- -GR- -Gs- -Gy- -Gw- -EHa- -Od -W3 -wd4201 -wd4100 -wd4189 /Zi -D_CRT_SECURE_NO_WARNINGS /FS %build_options% -I../../data_desk/ -I../../common/
set common_linker_flags= -opt:ref -incremental:no opengl32.lib /Debug:fastlink
set game_linker_flags= user32.lib gdi32.lib winmm.lib 



rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem                               Make Directories
rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pushd data_desk
if not exist build mkdir build
if not exist generated mkdir generated
popd
pushd game
if not exist .\build mkdir build
if not exist .\build\data mkdir build\data
popd



rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem                            Build Data Desk Layer
rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pushd data_desk
pushd build
echo --- Building Data Desk custom layer.
start /b /wait "" "cl.exe" -nologo /Zi -I../../common/ ../source/dungeoneer_data_desk.c /LD
popd
popd



rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem                            Do Data Desk Parse
rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pushd data_desk
set files= 
for %%i in (data\*) do ( call set "files=%%files%% %%i" )
echo --- Running Data Desk pass and generating code.
rem start /b /wait "" "data_desk.exe" --custom build\dungeoneer_data_desk.dll %files%
popd



rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem                           Build Game Exectuable
rem  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pushd game
start /b /wait "" "xcopy" .\data .\build\data /y /s /e /q
pushd build
echo --- Compiling game executable.
start /b /wait "" "cl" %common_compiler_flags% ..\source\win32\win32_main.c -I../source/ /link %common_linker_flags% %game_linker_flags% /OUT:%game_executable%
popd
popd



echo --- Build completed.