export default interface StreamdeckGlobalSettings {
    version: number;
    connection_settings: SimulatorConnectionSettings[],
    // Control reference query settings.
    last_search_query: string,
    last_selected_module: string,
    dcs_install_path: string,
    dcs_bios_install_path: string,
}

interface SimulatorConnectionSettings {
    // Note: protocol values must align with enum labels defined in:
    // Sources/backend-cpp/SimulatorInterface/SimulatorProtocolTypes.h
    protocol: "DCS_BIOS" | "DCS_ExportScript",
    enable: boolean,
    ip_address: string,
    listener_port: string,
    send_port: string,
    multicast_address: string,
}

export function defaultGlobalSettings(): StreamdeckGlobalSettings {
    return {
        version: 1,
        connection_settings: [
            {
                protocol: "DCS_BIOS",
                enable: false,
                ip_address: "127.0.0.1",
                listener_port: "5010",
                send_port: "7778",
                multicast_address: "239.255.50.10",
            },
            {
                protocol: "DCS_ExportScript",
                enable: false,
                ip_address: "127.0.0.1",
                listener_port: "1725",
                send_port: "26027",
                multicast_address: "",
            },
        ],
        last_search_query: "",
        last_selected_module: "",
        dcs_install_path: "",
        dcs_bios_install_path: ""
    };
}

export interface OldStreamdeckGlobalSettings {
    // General UI settings
    autoOpenConfigureWindow: boolean;
    // Connection settings to Simulator export.
    ip_address: string,
    listener_port: string,
    send_port: string,
    // Control reference query settings.
    last_search_query: string,
    last_selected_module: string,
    dcs_install_path: string,
    dcs_bios_install_path: string,
}
