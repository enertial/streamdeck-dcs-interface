// Copyright 2020, Charles Tytler

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