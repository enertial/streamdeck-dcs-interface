// Copyright 2020, Charles Tytler


/**
 *  Sends data to plugin which is received with the "sendToPlugin" callback function in C++.
 * 
 *  payload - Function expects a payload with at least an "event" field.
 */
function sendPayloadToPlugin(payload) {
    if ($SD.connection && $SD.connection.readyState === 1) {
        const json = {
            action: $SD.actionInfo['action'],
            event: 'sendToPlugin',
            context: $SD.uuid,
            payload: payload
        };

        $SD.connection.send(JSON.stringify(json));
    }
}

/**
 * 'sendSettingsToPlugin' is a wrapper to send current settings to the plugin
 *
 */
function sendSettingsToPlugin() {
    console.log("sendSettingsToPlugin", settings);
    const payload = { event: 'SettingsUpdate', settings: settings };
    sendPayloadToPlugin(payload);
}


/**
 * 'ReceivedPayloadFromPlugin' handles all messages sent from Plugin to Property Inspector.
 *
 */
function callbackReceivedPayloadFromPlugin(payload) {
    if (payload.event == 'DebugDcsGameState') {
        sendToCommsWindowDcsGameState(payload.current_game_state);
    }

    if (payload.event == 'InstalledModules') {
        sendToIdLookupWindowInstalledModules(payload.installed_modules);
        sendToConfigWindow(payload);
    }

    if (payload.event == 'Clickabledata') {
        sendToIdLookupWindowClickabledata(payload.clickabledata);
    }
}

