# Property Inspector Javascript & HTML

This directory contains all of the details for settings and windows in the Streamdeck GUI itself.

### Debugging javascript:

On Windows, you will need to add a DWORD `html_remote_debugging_enabled` with value `1` in the registry `@ HKEY_CURRENT_USER\Software\Elgato Systems GmbH\StreamDeck`.  

After you relaunch the Stream Deck app, you can open http://localhost:23654/ in Chrome, where you will find a list of 'Inspectable pages' (plugins). Inspecting your plugin is pretty straightforward: Click the reverse DNS name of your plugin and Chrome shows it's developer tools, where you can further inspect used components and log messages.