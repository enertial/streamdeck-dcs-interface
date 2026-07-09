/**
 * Streamdeck Application Interfaces.
 * 
 * Definition of these functions is implemented within the StreamdeckWebsocket object.
 * 
 */

import { ModuleControlsJson } from "../DcsBios/ControlReferenceInterface";
import StreamdeckButtonSettings from "./StreamdeckButtonSettings";
import StreamdeckGlobalSettings from "./StreamdeckGlobalSettings";

export default interface StreamdeckApi {
    commFns: StreamdeckCommFns;
    buttonSettings: StreamdeckButtonSettings;
    globalSettings: StreamdeckGlobalSettings;
    moduleList: string[];
    moduleControlRefs: ModuleControlsJson | undefined;
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
    setGlobalSettings(setting: string, value: string | boolean): void;
    // Logs a message to the Elgato Streamdeck error log.
    logMessage(message: string): void;

    // Messages sent to the C++ plugin:
    // Send button settings to plugin.
    sendSettingsToPlugin(settings: StreamdeckButtonSettings): void,
    // Request the full state of the simulation in memory.
    requestSimulationState(): void,
    // Request a list of modules installed for the Simulator.
    requestModuleList(path: string): void,
    // Request reference data for an individual module.
    requestModule(filename: string): void,
}
