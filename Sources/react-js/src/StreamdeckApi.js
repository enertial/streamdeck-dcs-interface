/* This is a bridge to the property inspector where actual SD API calls are made within propertyInspector/configWindowComms.js */

function sendToWindowOpener(event, payload) {
  if (window.opener) {
    window.opener.handleExternalWindowCall({ event: event, payload: payload });
  }
}

export function storeLastSearchQuery(searchQuery) {
  sendToWindowOpener("storeLastSearchQuery", { searchQuery: searchQuery });
}

export function storeLastSelectedModule(module) {
  sendToWindowOpener("storeLastSelectedModule", { selectedModule: module });
}

export function storeConnectionSettings(
  ip_address,
  listener_port,
  send_port,
  multicast_address
) {
  sendToWindowOpener("storeConnectionSettings", {
    ip_address: ip_address,
    listener_port: listener_port,
    send_port: send_port,
    multicast_address: multicast_address,
  });
}

export function requestModuleList(path) {
  sendToWindowOpener("requestModuleList", { path: path });
}

export function requestControlReferenceJson(path, module) {
  sendToWindowOpener("requestControlReferenceJson", {
    path: path,
    module: module,
  });
}

export function storeButtonSettings(send_address, press_value, release_value) {
  sendToWindowOpener("storeButtonSettings", {
    send_address: send_address,
    press_value: press_value,
    release_value: release_value,
  });
}
