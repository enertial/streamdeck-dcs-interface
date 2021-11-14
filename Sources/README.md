# Directory Structure

```
Sources
├── com.ctytler.dcs.sdPlugin  Plugin package where the built frontend and backend are combined
│   ├── manifest.json           Definition of Streamdeck plugin metadata
│   ├── bin                     Location for compiled C++ and lua scripts called by plugin
│   ├── helpDocs                Help documentation within plugin
│   ├── images                  Default icon images
│   └── propertyinspector       Javascript and html used by plugin (Button settings and windows)
├── backend-cpp               The backend of the plugin (Manages Simulator/Streamdeck State), written in C++
│   ├── ElgatoSD                Elgato Streamdeck SDK source and utilities
│   ├── SimulatorInterface      Classes for interacting with the simulator state
│   ├── StreamdeckContext       Classes for maintaining state of individual Streamdeck buttons
│   │   ├── ExportMonitors      Classes that monitor simulator export data for individual buttons
│   │   ├── SendActions         Classes that define button press and release actions
│   ├── StreamdeckInterface     Executable that interfaces C++ code with Streamdeck plugin
│   ├── Test                    Unit test infrastructure and target
│   ├── Utilities               Generic utilities for UDP socket and string manipulation
│   ├── Vendor                  Third party source code
│   └── Windows                 Visual Studio solution settings
└── frontend-react-js         The frontend of the plugin (Configuration window), written in ReactJS
```

### StreamDeck SDK Reference
This project (particluarly the contents of ElgatoSD, StreamdeckInterface, Vendor, Windows, and com.ctytler.dcs.sdPlugin) was originally derived from the SDK sample "CPU": https://github.com/elgatosf/streamdeck-cpu