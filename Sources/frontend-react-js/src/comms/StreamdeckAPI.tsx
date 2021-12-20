/**
 * Streamdeck Application Interfaces.
 * 
 * Definition of these functions is implemented within the StreamdeckWebsocket object.
 * 
 */

export interface StreamdeckApi {
    // Gets settings for the current Streamdeck button.
    getSettings: () => void;
    // Sets settings for the current Streamdeck button.
    setSettings(payload: object): void;
    // Gets global settings for all buttons of the Streamdeck plugin.
    getGlobalSettings(): void;
    // Sets global settings for all buttons of the Streamdeck plugin.
    setGlobalSettings(payload: object): void;
    // Logs a message to the Elgato Streamdeck error log.
    logMessage(message: string): void;
    // Sends a message to the C++ plugin executable.
    sendToPlugin(action: string, payload: object): void,
    // ~~~ The below are wrappers around sendToPlugin() ~~~
    // Request a list of modules installed for the Simulator.
    requestModuleList(path: string): void,
    // Request reference data for an individual module.
    requestModule(filename: string): void,
}

export interface StreamdeckButtonSettings {
    action?: string;
}

export interface StreamdeckGlobalSettings {
    // Connection settings to Simulator export.
    ip_address?: string,
    listener_port?: string,
    send_port?: string,
    // Control reference query settings.
    last_search_query?: string,
    last_selected_module?: string,
    dcs_install_path?: string,
}
