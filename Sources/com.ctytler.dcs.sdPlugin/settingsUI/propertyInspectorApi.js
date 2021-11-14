/** Functions for communication with original window **/

let dataFromPI;

/**
 * Sends a message to the DCS Interface plugin with the "event" type and a json payload.
 *
 * @param {string} event
 * @param {json} payload
 */
function sendmessage(event, payload) {
  // Calls function from original (Property Inspector) window.
  var msg = { event: event };
  if (payload) {
    msg["payload"] = payload;
  }
  window.opener.handleMessageFromConfigWindow(msg);
}

function handlePropInspectorMessage(payload) {
  console.log("Message from Property Inspector:");
  console.log(payload);
  dataFromPI = payload;
}
