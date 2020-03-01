/* Streamdeck Interface - For DCS Interaction
/* Copyright 2020 Charles Tytler

/**
 * This library provides functions to read in user settings from the Streamdeck Property Inspector
 * for each instance of a DCS button created and interface with the C++ MyStreamDeckPlugin class.
 */

/**
 * @function sendEventToPlugin
 * Sends a JSON payload with a specified event string to the plugin.
 * 
 * These messages are handled within the plugin via MyStreamDeckPlugin::SendToPlugin()
 * where the event field can be used to identify appropriate action to take.
 * 
 * @param {String} event String used to identify the reason for sending message.
 * @param {Json} inPayload Payload containing data to be transferred to plugin.
 */
function sendEventToPlugin(event, inPayload) {
    if ($SD.connection && $SD.connection.readyState === 1) {
        inPayload.event = event;
        const json = {
            action: $SD.actionInfo['action'],
            event: 'sendToPlugin',
            context: $SD.uuid,
            payload: inPayload
        }
        $SD.connection.send(JSON.stringify(json));
    }
}

/**
 * @function registerExportIdUpdate
 * Callback function that will send updated "export_id_*" value from the Property Inspector
 * to the plugin whenever a user changes the field in order to register/unregister that
 * export_id for DCS monitoring.
 * 
 * @param {String} updated_item ID for the spdi item in the Property Inspector which was updated.
 * @param {String} updated_value New value set by Streamdeck user.
 */
function registerExportIdUpdate(updated_item, updated_value) {
    console.log(updated_item, updated_value);
    if (!isNaN(updated_value)) {
        var payload = {
            key: updated_item,
            value: updated_value,
        }
        sendEventToPlugin('updateRegisteredExportId', payload)
    }
}


/**
 * Additional Help/Documentation Functions
 */

function callbackHelpButtonPress() {
    if (!window.xtWindow || window.xtWindow.closed) {
        window.xtWindow = window.open('../helpDocs/helpWindow.html', 'External Window');
    }
}