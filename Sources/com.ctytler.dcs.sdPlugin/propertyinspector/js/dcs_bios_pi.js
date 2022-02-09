let waitingForAutoOpenSetting = true;

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
    document.getElementById("port").innerHTML = "Port: " + socketSettings.port;
    document.getElementById("uuid").innerHTML = "UUID: " + socketSettings.propertyInspectorUUID;

    // Connect to Streamdeck and get stored Global settings.
    connectStreamdeckWebsocket();
    window.$SD.api = Object.assign({ send: SDApi.send }, SDApi.common, SDApi[inMessageType]);
    $SD.on("connected", (jsn) => {
        $SD.api.getGlobalSettings($SD.uuid);
    });

    $SD.on("didReceiveGlobalSettings", (jsn) => {
        console.log("Received global settings: ", jsn);
        window.globalSettings = jsn.payload.settings;
        if (globalSettings.hasOwnProperty("autoOpenConfigureWindow")) {
            document.getElementById("auto_open_checkbox").checked = globalSettings.autoOpenConfigureWindow;
            if (globalSettings.autoOpenConfigureWindow) {
                waitingForAutoOpenSetting && handleButtonPress();
            }
        }
        waitingForAutoOpenSetting = false;
    });
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
    // Only open if socketSettings have been populated.
    if (window.socketSettings) {
        // Only open window if there is not one already open.
        if (!window.configWindow || window.configWindow.closed) {
            disconnectStreamdeckWebsocket();
            window.configWindow = window.open(
                "../settingsUI/index.html",
                "Button Configuration"
            );
            var pollTimer = setInterval(function () {
                if (window.configWindow.closed) {
                    clearInterval(pollTimer);
                    connectStreamdeckWebsocket();
                    document.getElementById("message").innerHTML = "";
                }
            }, 500);
        }
    }
}

function handleConfigureWindowClose() {
    connectStreamdeckWebsocket();
}

function handleAutoOpenCheckboxChange() {
    const autoOpenChecked = document.getElementById("auto_open_checkbox").checked;
    if ($SD.connection) {
        $SD.api.setGlobalSettings($SD.uuid, Object.assign({}, globalSettings, { autoOpenConfigureWindow: autoOpenChecked }));
        globalSettings.autoOpenConfigureWindow = autoOpenChecked;
    }
    else if (window.configWindow) {
        document.getElementById("auto_open_checkbox").checked = globalSettings.autoOpenConfigureWindow;
        document.getElementById("message").innerHTML = "Must be set when Configure window is closed";
    }
}

/** the beforeunload event is fired, right before the PI will remove all nodes */
window.addEventListener('beforeunload', function (e) {
    e.preventDefault();
    if ($SD.connection) {
        globalSettings.autoOpenConfigureWindow = document.getElementById("auto_open_checkbox").checked;
        $SD.api.setGlobalSettings($SD.uuid, globalSettings);
    }
    // Don't set a returnValue to the event, otherwise Chromium with throw an error.  // e.returnValue = '';
});
