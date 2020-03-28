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

function callbackReceivedGlobalSettings(global_settings) {
    global_settings = addDefaultGlobalSettings(global_settings);
    sendToCommsWindowSettings(global_settings);
}

function callbackReceivedPayloadFromPlugin(payload) {
    if (payload.event == 'DebugDcsGameState') {
        sendToCommsWindowDcsGameState(payload.current_game_state);
    }
}

/**
 * Adds default values to any unpopulated settings fields according to action type.
 */
function addDefaultSettings(action, settings) {
    console.log("Adding Default Settings to existing: ", settings);

    /* DEFAULT SETTINGS SPECIFIED HERE */
    if (action.includes("switch")) {
        document.getElementById("switch_button_settings").hidden = false;
        if (!settings.hasOwnProperty("send_when_first_state_value")) {
            settings["send_when_first_state_value"] = "1";
        }
        if (!settings.hasOwnProperty("send_when_second_state_value")) {
            settings["send_when_second_state_value"] = "-1";
        }
    }
    else if (action.includes("increment")) {
        document.getElementById("increment_button_settings").hidden = false;
        if (!settings.hasOwnProperty("increment_value")) {
            settings["increment_value"] = "0.1";
        }
        if (!settings.hasOwnProperty("increment_min")) {
            settings["increment_min"] = "0";
        }
        if (!settings.hasOwnProperty("increment_max")) {
            settings["increment_max"] = "1";
        }
        if (!settings.hasOwnProperty("increment_cycle_allowed_check")) {
            settings["increment_cycle_allowed_check"] = false;
        }
    }
    else {
        console.log("Momentary button");
        document.getElementById("momentary_button_settings").hidden = false;
        if (!settings.hasOwnProperty("release_value")) {
            settings["release_value"] = "0";
        }
        if (!settings.hasOwnProperty("press_value")) {
            settings["press_value"] = "1";
        }
    }

    if (!settings.hasOwnProperty("dcs_id_comparison_value")) {
        settings["dcs_id_comparison_value"] = "0";
    }
    if (!settings.hasOwnProperty("string_monitor_vertical_spacing")) {
        settings["string_monitor_vertical_spacing"] = "0";
    }
    if (!settings.hasOwnProperty("string_monitor_passthrough_check")) {
        settings["string_monitor_passthrough_check"] = true;
    }

    return settings;
}

function addDefaultGlobalSettings(settings) {
    if (!settings.hasOwnProperty("ip_address")) {
        settings["ip_address"] = "127.0.0.1";
    }
    if (!settings.hasOwnProperty("listener_port")) {
        settings["listener_port"] = "1725";
    }
    if (!settings.hasOwnProperty("send_port")) {
        settings["send_port"] = "26027";
    }
    return settings;
}

/**
 * Opens an external window when button is clicked.
 */
function callbackHelpButtonPress() {
    if (!window.helpWindow || window.helpWindow.closed) {
        window.helpWindow = window.open('../helpDocs/helpWindow.html', 'Help Documentation');
    }
}

/**
 * Opens an external window when button is clicked.
 */
function callbackCommsSettingsButtonPress() {
    if (!window.commsWindow || window.commsWindow.closed) {
        window.commsWindow = window.open('comms_window.html', 'Comms Settings');
    }
}

/**
 * @function callbackStringMonitorPassthroughCheck
 * Shows/hides textbox for entering string display mapping.
 */
function callbackStringMonitorPassthroughCheck(isChecked) {
    if (isChecked) {
        document.getElementById("string_monitor_mapping_div").hidden = true;
    }
    else {
        document.getElementById("string_monitor_mapping_div").hidden = false;
    }
    console.log("String Monitor Passtrhough Callback isChecked: ", isChecked);
}

/**
 *  Clears setting for DCS Command Button ID, disabling the feature.
 */
function callbackClearDcsCommand() {
    delete settings["button_id"];
    $SD.api.setSettings($SD.uuid, settings);
    button_id.value = "";
    // Don't need to send to plugin as it always reads in latest settings on button push.
    console.log("Clear DCS Command", settings);
}

/**
 *  Clears setting for DCS ID Compare Monitor, disabling the feature.
 */
function callbackClearCompareMonitor() {
    delete settings["dcs_id_compare_monitor"];
    $SD.api.setSettings($SD.uuid, settings);
    dcs_id_compare_monitor.value = "";
    sendSettingsToPlugin();
    console.log("Clear DCS ID Compare Monitor", settings);
}

/**
 *  Clears setting for DCS ID String Monitor, disabling the feature.
 */
function callbackClearStringMonitor() {
    delete settings["dcs_id_string_monitor"];
    $SD.api.setSettings($SD.uuid, settings);
    dcs_id_string_monitor.value = "";
    sendSettingsToPlugin();
    console.log("Clear DCS ID String Monitor", settings);
}


/** Functions for communication with external Comms Settings window **/

/** Callback functions for working with external window **/
function gotCallbackFromCommsWindow(parameter) {
    if (parameter.event == "requestGlobalSettings") {
        $SD.api.getGlobalSettings($SD.uuid);
    }

    if (parameter.event == 'updateConnectionSettings') {
        console.log("Set Global Settings: ", parameter.payload)
        $SD.api.setGlobalSettings($SD.uuid, parameter.payload);
    }

    if (parameter.event == 'refreshDcsState') {
        sendPayloadToPlugin({ event: "RequestDcsStateUpdate" });
    }

    console.log("Callback from comms window: ", parameter);
}

function sendToCommsWindowSettings(global_settings) {
    if (window.commsWindow) {
        window.commsWindow.gotGlobalSettingsFromPI(global_settings);
    }
}

function sendToCommsWindowDcsGameState(current_game_state) {
    if (window.commsWindow) {
        window.commsWindow.gotDcsGameState(current_game_state);
    }
}