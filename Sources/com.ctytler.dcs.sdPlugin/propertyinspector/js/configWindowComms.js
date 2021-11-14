/** Functions for communication with external window (built from Sources/react-js) **/

/**
 * Opens an external window when button is clicked.
 */
function callbackConfigButtonPress() {
  if (!window.configWindow || window.configWindow.closed) {
    window.configWindow = window.open(
      "../settingsUI/index.html",
      "Button Configuration"
    );
  }
}

const configWindowChannel = new BroadcastChannel("config-window-channel");

/**
 * Callback function to allow external window to make SD.api function calls.
 */
configWindowChannel.addEventListener("message", (e) => {
  const message = e.data;

  if (message.event == "storeLastSearchQuery") {
    global_settings["last_search_query"] = message.searchQuery;
    console.log("Store Last Search Query: ", global_settings);
    $SD.api.setGlobalSettings($SD.uuid, global_settings);
  }

  if (message.event == "storeLastSelectedModule") {
    global_settings["last_selected_module"] = message.selectedModule;
    console.log("Store Last Selected Module: ", global_settings);
    $SD.api.setGlobalSettings($SD.uuid, global_settings);
  }

  if (message.event == "storeConnectionSettings") {
    global_settings["ip_address"] = paremeter.payload.ip_address;
    global_settings["listener_port"] = paremeter.payload.listener_port;
    global_settings["send_port"] = paremeter.payload.send_port;
    global_settings["multicast_address"] = paremeter.payload.multicast_address;
    console.log("Store Connection Settings: ", global_settings);
    $SD.api.setGlobalSettings($SD.uuid, global_settings);
  }

  if (message.event == "requestModuleList") {
    sendPayloadToPlugin({
      event: "requestModuleList",
      path: message.path,
    });
  }

  if (message.event == "requestControlReferenceJson") {
    sendPayloadToPlugin({
      event: "requestJsonFile",
      filename: message.filename,
    });
  }

  if (message.event == "storeButtonSettings") {
    settings["send_address"] = message.payload.send_address;
    settings["press_value"] = message.payload.press_value;
    settings["release_value"] = message.payload.release_value;
    console.log("Store Button Settings: ", settings);
    $SD.api.setSettings($SD.uuid, settings);
    sendPayloadToPlugin({ event: "SettingsUpdate", settings: settings });
  }

  console.log("Callback from Config window: ", message);
});
