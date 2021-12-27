import { useState, useEffect, useRef, useMemo } from 'react';
import { StreamdeckApi, defaultButtonSettings, defaultGlobalSettings } from './StreamdeckApi';

export interface StreamdeckSocketSettings {
    port: number,
    propertyInspectorUUID: string,
    registerEvent: string,
    info: any, // A large json object that only needs to be gatewayed
}

export function defaultStreamdeckSocketSettings(): StreamdeckSocketSettings {
    return {
        port: 8000,
        propertyInspectorUUID: "123",
        registerEvent: "registerPropertyInspector",
        info: {}
    };
}

export function useStreamdeckWebsocket(socketSettings: StreamdeckSocketSettings) {
    const [buttonSettings, setButtonSettings] = useState(defaultButtonSettings());
    const [globalSettings, setGlobalSettings] = useState(defaultGlobalSettings());
    const websocket = useRef<WebSocket | null>(null);

    // Definition of the API that is used by other components.
    const sdApi: StreamdeckApi = useMemo(() => {
        // Protocol to send messages to the Streamdeck application.
        function send(event: string, payload: object) {
            const json = Object.assign({}, { event: event, context: socketSettings.propertyInspectorUUID }, payload);
            websocket.current?.send(JSON.stringify(json));
            console.log("Sent message (", event, "):", json);
        }

        return {
            getSettings: function () {
                send('getSettings', {});
            },

            setSettings: function (payload: object) {
                send('setSettings', { payload: payload });
            },

            getGlobalSettings: function () {
                send('getGlobalSettings', {});
            },

            setGlobalSettings: function (payload: object) {
                send('setGlobalSettings', { payload: payload });
            },

            logMessage: function (message: string) {
                send('logMessage', { payload: { message: message } });
            },

            sendToPlugin: function (action: string, payload: object) {
                send('sendToPlugin',
                    {
                        action: action,
                        payload: payload || {}
                    });
            },

            requestModuleList: function (path: string) {
                this.sendToPlugin("", { event: "requestModuleList", path: path });
            },

            requestModule: function (filename: string) {
                this.sendToPlugin("", { event: "requestControlReferenceJson", filename: filename });
            },
        };
    }, [socketSettings]);

    // Websocket connect and shutdown setup.
    useEffect(() => {
        websocket.current = new WebSocket("ws://127.0.0.1:" + socketSettings.port)
        websocket.current.onopen = () => {
            console.log("Connected to Streamdeck Websocket.");
            registerPropertyInspector();
            sdApi.getGlobalSettings();
        }
        websocket.current.onmessage = (msg: MessageEvent) => {
            handleReceivedMessage(msg.data);
        }
        websocket.current.onerror = function (event: any) {
            console.warn('WEBOCKET ERROR', event, event.data);
        };
        websocket.current.onclose = function (event: any) {
            // Websocket is closed
            var reason = WEBSOCKETERROR(event);
            console.log('[STREAMDECK]***** WEBOCKET CLOSED **** reason:', reason);
        };
        return function onUnmount() {
            websocket.current?.close();
        }

        // This message registers this Websocket binding as the Property Inspector
        // for the selected button in the Streamdeck GUI.
        function registerPropertyInspector() {
            const json = {
                event: socketSettings.registerEvent,
                uuid: socketSettings.propertyInspectorUUID
            };
            websocket.current?.send(JSON.stringify(json));
            console.log("Registered myself according to:", json);
        };

        function handleReceivedMessage(msg: any) {
            interface SDMessage {
                event: string,
                payload: SDMessagePayload
            }
            interface SDMessagePayload {
                settings?: object
            }

            let jsonObj: SDMessage;
            try {
                jsonObj = JSON.parse(msg);
                console.log("Received message:", jsonObj);
            } catch (e) {
                console.warn("Error in parsing received message", msg);
                return;
            }
            if (jsonObj.hasOwnProperty("event")) {
                switch (jsonObj.event) {
                    case "didReceiveSettings":
                        setButtonSettings(prevButtonSetting => Object.assign(prevButtonSetting, jsonObj.payload.settings));
                        console.log("Received Button Settings", jsonObj.payload.settings);
                        break;
                    case "didReceiveGlobalSettings":
                        setGlobalSettings(prevGlobalSettings => Object.assign(prevGlobalSettings, jsonObj.payload.settings));
                        console.log("Received Global Settings", jsonObj.payload.settings);
                        break;
                    default:
                        console.log("Message Handler not defined for Event: ", jsonObj.event);
                }

            }
        }

    }, [socketSettings, sdApi]);

    return { sdApi, buttonSettings, globalSettings };
}

/**
 * Additional error-handling log messages.
 */
function WEBSOCKETERROR(evt: any) {
    // Websocket is closed
    var reason = '';
    if (evt.code === 1000) {
        reason = 'Normal Closure. The purpose for which the connection was established has been fulfilled.';
    } else if (evt.code === 1001) {
        reason = 'Going Away. An endpoint is "going away", such as a server going down or a browser having navigated away from a page.';
    } else if (evt.code === 1002) {
        reason = 'Protocol error. An endpoint is terminating the connection due to a protocol error';
    } else if (evt.code === 1003) {
        reason = "Unsupported Data. An endpoint received a type of data it doesn't support.";
    } else if (evt.code === 1004) {
        reason = '--Reserved--. The specific meaning might be defined in the future.';
    } else if (evt.code === 1005) {
        reason = 'No Status. No status code was actually present.';
    } else if (evt.code === 1006) {
        reason = 'Abnormal Closure. The connection was closed abnormally, e.g., without sending or receiving a Close control frame';
    } else if (evt.code === 1007) {
        reason = 'Invalid frame payload data. The connection was closed, because the received data was not consistent with the type of the message (e.g., non-UTF-8 [http://tools.ietf.org/html/rfc3629]).';
    } else if (evt.code === 1008) {
        reason = 'Policy Violation. The connection was closed, because current message data "violates its policy". This reason is given either if there is no other suitable reason, or if there is a need to hide specific details about the policy.';
    } else if (evt.code === 1009) {
        reason = 'Message Too Big. Connection closed because the message is too big for it to process.';
    } else if (evt.code === 1010) { // Note that this status code is not used by the server, because it can fail the WebSocket handshake instead.
        reason = "Mandatory Ext. Connection is terminated the connection because the server didn't negotiate one or more extensions in the WebSocket handshake. <br /> Mandatory extensions were: " + evt.reason;
    } else if (evt.code === 1011) {
        reason = 'Internl Server Error. Connection closed because it encountered an unexpected condition that prevented it from fulfilling the request.';
    } else if (evt.code === 1015) {
        reason = "TLS Handshake. The connection was closed due to a failure to perform a TLS handshake (e.g., the server certificate can't be verified).";
    } else {
        reason = 'Unknown reason';
    }

    return reason;
}
