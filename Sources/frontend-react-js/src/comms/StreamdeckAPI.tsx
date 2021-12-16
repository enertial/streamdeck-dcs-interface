/**
 * Streamdeck Application Interfaces.
 * 
 * Definition of these functions is implemented within the StreamdeckWebsocket object.
 * 
 */

interface StreamdeckApi {
    // Gets settings for the current Streamdeck button.
    getSettings: () => void,
    // Sets settings for the current Streamdeck button.
    setSettings(payload: object): void,
    // Gets global settings for all buttons of the Streamdeck plugin.
    getGlobalSettings(): void,
    // Sets global settings for all buttons of the Streamdeck plugin.
    setGlobalSettings(payload: object): void,
    // Logs a message to the Elgato Streamdeck error log.
    logMessage(message: string): void,
    // Sends a message to the C++ plugin executable.
    sendToPlugin(action: string, payload: object): void,
}

export default StreamdeckApi;
