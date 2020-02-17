# streamdeck-dcs

**NOTE: Work in Progress**

`DCS` is a plugin for the Streamdeck that allows communication with DCS via lua UDP sockets.


# Description

`DCS` is a plugin that allows you to create buttons and interfaces that update with DCS events.

# Installation

In the Release folder, you can find the file `com.ctytler.dcs.streamDeckPlugin`. If you double-click this file on your machine, Stream Deck will install the plugin.

# Source code

The Sources folder contains the source code of the plugin.  
 - Sources/MyStreamDeckPlugin.{cpp,h} - StreamDeck C++ API (based on the streamdeck-cpu example)
 - Sources/DcsInterface.{cpp,h} - Interface between plugin and DCS
 - Sources/DcsInterfaceParams.h - Defines the UDP ports to communicate with DCS
The Streamdeck plugin details are modified via `Sources\com.ctytler.dcs.sdPlugin\manifest.json`.

# Build from source instructions
A build script is included which will build both the C++ executable which handles the communcation with DCS as well as the package for the StreamDeck plugin:  
`build_plugin.bat`  
You must call this file from the "Developer Command Prompt for VS" in order for the Visual C++ target build step to work.

You may also need to install the Boost C++ library.
