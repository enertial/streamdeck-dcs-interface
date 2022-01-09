/**
 * Streamdeck Application Interfaces.
 * 
 * Definition of these functions is implemented within the StreamdeckWebsocket object.
 * 
 */

export default interface StreamdeckApi {
    commFns: StreamdeckCommFns;
    buttonSettings: StreamdeckButtonSettings;
    globalSettings: StreamdeckGlobalSettings;
}

export interface StreamdeckCommFns {
    // Messages sent to the Streamdeck Application:
    // Gets settings for the current Streamdeck button.
    getSettings: () => void;
    // Sets settings for the current Streamdeck button.
    setSettings(payload: StreamdeckButtonSettings): void;
    // Gets global settings for all buttons of the Streamdeck plugin.
    getGlobalSettings(): void;
    // Sets global settings for all buttons of the Streamdeck plugin.
    setGlobalSettings(payload: StreamdeckGlobalSettings): void;
    // Logs a message to the Elgato Streamdeck error log.
    logMessage(message: string): void;

    // Messages sent to the C++ plugin:
    // Request a list of modules installed for the Simulator.
    requestModuleList(path: string): void,
    // Request reference data for an individual module.
    requestModule(filename: string): void,
}

export interface StreamdeckButtonSettings {
    // Simulator command settings
    send_address: string;
    press_value: string;
    release_value: string;
    disable_release_check: boolean;
    // Title Monitor settings
    dcs_id_string_monitor: string;
    string_monitor_max_string_length: number;
    string_monitor_vertical_spacing: string;
    string_monitor_passthrough_check: boolean;
    string_monitor_mapping: string;
}

export function defaultButtonSettings(): StreamdeckButtonSettings {
    return {
        send_address: "",
        press_value: "",
        release_value: "",
        disable_release_check: false,
        dcs_id_string_monitor: "",
        string_monitor_max_string_length: 0,
        string_monitor_vertical_spacing: "0",
        string_monitor_passthrough_check: true,
        string_monitor_mapping: "",
    };
}

export interface StreamdeckGlobalSettings {
    // Connection settings to Simulator export.
    ip_address: string,
    listener_port: string,
    send_port: string,
    // Control reference query settings.
    last_search_query: string,
    last_selected_module: string,
    dcs_install_path: string,
}

export function defaultGlobalSettings(): StreamdeckGlobalSettings {
    return {
        ip_address: "",
        listener_port: "",
        send_port: "",
        last_search_query: "",
        last_selected_module: "",
        dcs_install_path: ""
    };
}

