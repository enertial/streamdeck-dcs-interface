// Copyright 2020, Charles Tytler

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
function callbackIdLookupButtonPress() {
    if (!window.idLookupWindow || window.idLookupWindow.closed) {
        window.idLookupWindow = window.open('id_lookup_window.html', 'ID Lookup');
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




/** Functions for communication with external ID Lookup window **/

/** Callback functions for working with external window **/
function gotCallbackFromIdLookupWindow(parameter) {
    if (parameter.event == "RequestInstalledModules") {
        sendPayloadToPlugin({ event: "RequestInstalledModules", dcs_install_path: parameter.payload });
    }

    if (parameter.event == 'RequestIdLookup') {
        sendPayloadToPlugin({ event: "RequestIdLookup", dcs_install_path: parameter.payload.dcs_install_path, module: parameter.payload.module });
    }

    console.log("Callback from ID lookup window: ", parameter);
}

function sendToIdLookupWindowInstalledModules(installed_modules_list) {
    if (window.idLookupWindow) {
        window.idLookupWindow.gotInstalledModules(installed_modules_list);
    }
}

function sendToIdLookupWindowClickabledata(clickabledata_elements) {
    if (window.idLookupWindow) {
        window.idLookupWindow.gotClickabledata(clickabledata_elements);
    }
}


/** Functions for communication with external Comms Settings window **/

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