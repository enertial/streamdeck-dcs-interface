// This callback function is called by the Streamdeck application populating socketSettings to use.
function connectElgatoStreamDeckSocket(
    inPort,
    inUUID,
    inMessageType,
    inApplicationInfo,
    inActionInfo
) {
    // Store settings to the window object, so the Config Window when opened can access it.
    const inAction = JSON.parse(inActionInfo);
    window.socketSettings = {
        port: inPort,
        propertyInspectorUUID: inUUID,
        registerEvent: inMessageType,
        info: inApplicationInfo,
        action: inAction["action"],
    };
    window.settings = inAction["settings"];
    console.log("Received websocket settings:", socketSettings);
    console.log("Received settings:", settings);
    console.log("Received actionInfo:", inActionInfo);
    document.getElementById("uuid").innerHTML = "UUID: " + socketSettings.propertyInspectorUUID;
    // document.getElementById("uuid").hidden = false;
}

function connectStreamdeckWebsocket() {
    StreamDeck.getInstance().connect([
        socketSettings.port,
        socketSettings.propertyInspectorUUID,
        socketSettings.registerEvent,
        socketSettings.info,
        socketSettings.action]);
}

function disconnectStreamdeckWebsocket() {
    if ($SD.connection) {
        $SD.connection.close();
        $SD.connection = null;
    }
}

function handleButtonPress() {
    // Only open if socketSettings have been populated and if there is not already a Configure window open.
    if (window.socketSettings && (!window.configWindow || window.configWindow.closed)) {
        window.configWindow = window.open(
            "../settingsUI/index.html",
            "Button Configuration"
        );
    }
}
