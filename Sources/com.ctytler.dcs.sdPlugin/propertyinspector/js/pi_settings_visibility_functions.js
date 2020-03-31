// Copyright 2020, Charles Tytler

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