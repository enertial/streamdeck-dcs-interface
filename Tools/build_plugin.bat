:: Build script for streamdeck-dcs.
:: Instructions: You must call this file from the "Developer Command Prompt for VS"
::               For details see https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line

:: Change directory to the project root (directory above this batch file location)
cd /D "%~dp0"\..

:: Build C++ executable:
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" .\Sources\backend-cpp\Windows\com.ctytler.dcs.sdPlugin.sln /p:Configuration="Release"
if %errorlevel% neq 0 echo "Canceling plugin build due to failed backend build" && pause && exit /b %errorlevel%

:: Run unit tests, only continue if all tests pass
Sources\backend-cpp\Windows\x64\Release\Test.exe
if %errorlevel% neq 0 echo "Canceling plugin build due to failed unit tests" && pause && exit /b %errorlevel%

:: Copy C++ executable and DLLs to StreamDeck Plugin package:
echo. && echo *** C++ binary compilation complete, published to Sources/com.ctytler.dcs.sdPlugin/bin/ *** && echo.
copy Sources\backend-cpp\Windows\x64\Release\streamdeck_dcs_interface.exe Sources\com.ctytler.dcs.sdPlugin\bin\
copy Sources\backend-cpp\Windows\x64\Release\*.dll Sources\com.ctytler.dcs.sdPlugin\bin\

:: Remove any prior build of the Plugin:
echo. && echo *** Removing any previous builds of com.ctytler.dcs.streamDeckPlugin from Release/ ***
del Release\com.ctytler.dcs.streamDeckPlugin && echo ...Successfully removed

:: Build the ReactJS user interface:
cd Sources\frontend-react-js && call npm run build
cd ..\..
echo *** React JS build complete, published to Sources/com.ctytler.dcs.sdPlugin/settingsUI/ *** && echo.

:: Build StreamDeck Plugin:
echo *** Building com.ctytler.dcs.streamDeckPlugin to Release/ *** && echo.
Tools\DistributionTool.exe -b -i Sources\com.ctytler.dcs.sdPlugin -o Release
echo. && echo  *** Build complete *** && echo.

:: Pause for keypress to allow user to view output
pause

:: Plugin installer named "com.ctytler.dcs.streamDeckPlugin" will be output to Release/ directory
