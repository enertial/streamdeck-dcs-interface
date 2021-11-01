:: Build script for streamdeck-dcs.
:: Instructions: You must call this file from the "Developer Command Prompt for VS"
::               For details see https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line

:: Build C++ executable:
devenv %cd%\Sources\Windows\com.ctytler.dcs.sdPlugin.sln /build "Release|x64"

:: Copy C++ executable to StreamDeck Plugin package:
copy Sources\Windows\x64\Release\streamdeck_dcs_interface.exe Sources\com.ctytler.dcs.sdPlugin\bin\

:: Remove any prior build of the Plugin:
del Release\com.ctytler.dcs.streamDeckPlugin

:: Build StreamDeck Plugin:
Tools\DistributionTool.exe -b -i Sources\com.ctytler.dcs.sdPlugin -o Release
