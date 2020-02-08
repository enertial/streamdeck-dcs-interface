# streamdeck-dcs

**NOTE: Work in Progress**

`DCS` is a plugin for the Streamdeck that allows communication with DCS via lua UDP sockets.


# Description

`DCS` is a plugin that allows you to create buttons and interfaces that update with DCS events.

# Installation

In the Release folder, you can find the file `com.ctytler.dcs.streamDeckPlugin`. If you double-click this file on your machine, Stream Deck will install the plugin.

# Source code

The Sources/DCS directory contains the library for communicating with DCS.  
The Sources folder contains the source code of the plugin.  
The Streamdeck plugin details are modified via `Sources\com.ctytler.dcs.sdPlugin\manifest.json`.

# Build instructions

To build the C++ `dcs_interface.exe` target you must open the solution file `Sources\Windows\com.ctytler.dcs.sdPlugin.sln` with Visual Studio C++ and Build the project in the Release configuration.  
  
To build the Streamdeck plugin you can run the `build_release_plugin.bat` to generate the Streamdeck plugin `Release\com.ctytler.dcs.streamDeckPlugin`.
