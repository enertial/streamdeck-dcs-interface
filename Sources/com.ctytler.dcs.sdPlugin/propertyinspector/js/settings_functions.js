// Copyright 2020, Charles Tytler

/**
 * The global variable "global_settings" stores in memory settings which should be constant for all Streamdeck button
 * contexts, set via the external windows.
 * 
 * This also includes ease-of-use values such as "last selected module" so users don't have to reselect the same module
 * for each button.
 */
var global_settings;

/**
 * This function is called any time global settings are changed or requested.
 * Upon device connection, the global settings are requested from "index_pi.js".
 * 
 * @param global_settings Received global settings from Streamdeck program.
 */
function callbackReceivedGlobalSettings(stored_global_settings) {
    global_settings = addDefaultGlobalSettings(stored_global_settings);
}



/** Functions to set default settings **/


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
    if (!settings.hasOwnProperty("dcs_install_path")) {
        settings["dcs_install_path"] = "C:\\Program Files\\Eagle Dynamics\\DCS World";
    }
    if (!settings.hasOwnProperty("last_selected_module")) {
        settings["last_selected_module"] = "";
    }
    return settings;
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
        if (!settings.hasOwnProperty("disable_release_check")) {
            settings["disable_release_check"] = false;
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


/** Functions to display optional settings sections **/

function callbackDisableRelease(isChecked) {
    console.log("Disable release: ", isChecked);
    var release_elem = document.getElementById("release_value");
    if (isChecked) {
        release_elem.disabled = true;
        release_elem.style.backgroundColor = "#222222";
        release_elem.style.color = "#333333";
    }
    else {
        release_elem.disabled = false;
        release_elem.style.backgroundColor = "";
        release_elem.style.color = "";
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



/** Functions to clear settings values **/



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