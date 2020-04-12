// Copyright 2020, Charles Tytler


/** Functions to open external windows **/

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
    if (parameter.event == 'UpdateGlobalSettings') {
        console.log("Set Global Settings: ", parameter.payload)
        $SD.api.setGlobalSettings($SD.uuid, parameter.payload);
    }

    if (parameter.event == "RequestInstalledModules") {
        sendPayloadToPlugin({ event: "RequestInstalledModules", dcs_install_path: parameter.payload });
    }

    if (parameter.event == 'RequestIdLookup') {
        sendPayloadToPlugin({ event: "RequestIdLookup", dcs_install_path: parameter.payload.dcs_install_path, module: parameter.payload.module });
    }

    if (parameter.event == 'ImportDcsCommand') {
        settings["button_id"] = parameter.payload.button_id;
        settings["device_id"] = parameter.payload.device_id;
        settings["press_value"] = parameter.payload.click_value;
        settings["release_value"] = "0";
        settings["dcs_id_increment_monitor"] = parameter.payload.dcs_id;
        settings["increment_value"] = parameter.payload.click_value;
        settings["increment_min"] = parameter.payload.limit_min;
        settings["increment_max"] = parameter.payload.limit_max;
        if (parameter.payload.switch_direction == "1st_to_2nd") {
            settings["send_when_first_state_value"] = parameter.payload.click_value;
        }
        if (parameter.payload.switch_direction == "2nd_to_1st") {
            settings["send_when_second_state_value"] = parameter.payload.click_value;
        }
        console.log('setSettings....', settings);
        $SD.api.setSettings($SD.uuid, settings);
        updateUI(settings);
    }

    if (parameter.event == 'ImportImageChange') {
        settings["dcs_id_compare_monitor"] = parameter.payload.dcs_id;
        console.log('setSettings....', settings);
        $SD.api.setSettings($SD.uuid, settings);
        updateUI(settings);
    }

    if (parameter.event == 'ImportTextChange') {
        settings["dcs_id_string_monitor"] = parameter.payload.dcs_id;
        console.log('setSettings....', settings);
        $SD.api.setSettings($SD.uuid, settings);
        updateUI(settings);
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

    if (parameter.event == 'updateGlobalSettings') {
        console.log("Set Global Settings: ", parameter.payload)
        $SD.api.setGlobalSettings($SD.uuid, parameter.payload);
    }

    if (parameter.event == 'refreshDcsState') {
        sendPayloadToPlugin({ event: "RequestDcsStateUpdate" });
    }

    console.log("Callback from comms window: ", parameter);
}

function sendToCommsWindowDcsGameState(current_game_state) {
    if (window.commsWindow) {
        window.commsWindow.gotDcsGameState(current_game_state);
    }
}