function loaded() {
    restoreGlobalSettings(window.opener.global_settings);
};


function callbackUpdateConnectionSettings() {
    window.opener.global_settings["ip_address"] = document.getElementById("ip_address").value;
    window.opener.global_settings["listener_port"] = document.getElementById("listener_port").value;
    window.opener.global_settings["send_port"] = document.getElementById("send_port").value;
    sendmessage("updateGlobalSettings", window.opener.global_settings);
}

/**
 * Restores previously set global settings saved from a previous Streamdeck context.
 * 
 * @param settings Global settings json stored by Streamdeck program
 */
function restoreGlobalSettings(settings) {
    document.getElementById("ip_address").value = settings.ip_address;
    document.getElementById("listener_port").value = settings.listener_port;
    document.getElementById("send_port").value = settings.send_port;
    // Fields and button remain hidden until we've received settings from PI
    // to avoid showing the wrong information.
    document.getElementById("connection_settings_div").hidden = false;
    console.log("Restored global settings: ", settings);
}

/**
 * Sends a message to the plugin requesting a refresh of the DCS game state.
 */
function callbackRefreshDcsGameState() {
    sendmessage("refreshDcsState");
}

/**
 * Populates rows of table with the DCS ID and values from received DCS game state.
 * 
 * @param {json} current_game_state 
 */
function gotDcsGameState(current_game_state) {
    // Create rows in a new table body so it is easy to replace any old content.
    var new_table_body = document.createElement('tbody');
    for (const id of Object.keys(current_game_state)) {
        var new_row = new_table_body.insertRow();
        var dcs_id_cell = new_row.insertCell(0);
        var value_cell = new_row.insertCell(1);
        dcs_id_cell.appendChild(document.createTextNode(id));
        value_cell.appendChild(document.createTextNode(current_game_state[id]));
    }
    var document_table_body = document.getElementById("game_state_table").getElementsByTagName('tbody')[0];
    document_table_body.parentNode.replaceChild(new_table_body, document_table_body);
}

/** Functions for communication with original window **/

/**
 * Sends a message to the DCS Interface plugin with the "event" type and a json payload.
 * 
 * @param {string} event 
 * @param {json} payload 
 */
function sendmessage(event, payload) {
    // Calls function from original (Property Inspector) window.
    var msg = { "event": event }
    if (payload) {
        msg["payload"] = payload;
    }
    window.opener.gotCallbackFromCommsWindow(msg);
}