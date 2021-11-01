:: Build script for streamdeck-dcs.
:: Instructions: You must call this file from the "Developer Command Prompt for VS"
::               For details see https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line

:: Change directory to the project root (directory above this batch file location)
cd /D "%~dp0"\..

:: Configure the environment for Visual Studio
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

:: Build C++ executable:
devenv Sources\Windows\com.ctytler.dcs.sdPlugin.sln /build "Release|x64"

:: Run unit tests, only continue if all tests pass
Sources\Windows\x64\Release\Test.exe
if %errorlevel% neq 0 echo "Canceling plugin build due to failed unit tests" && pause && exit /b %errorlevel%

:: Copy C++ executable to StreamDeck Plugin package:
copy Sources\Windows\x64\Release\streamdeck_dcs_interface.exe Sources\com.ctytler.dcs.sdPlugin\bin\

:: Remove any prior build of the Plugin:
del Release\com.ctytler.dcs.streamDeckPlugin

:: Build StreamDeck Plugin:
Tools\DistributionTool.exe -b -i Sources\com.ctytler.dcs.sdPlugin -o Release

:: Pause for keypress to allow user to view output
pause

:: Plugin installer named "com.ctytler.dcs.streamDeckPlugin" will be output to Release/ directory
