import { useState, useEffect, useRef, useMemo } from 'react';
import StreamdeckApi, { defaultButtonSettings, defaultGlobalSettings, StreamdeckButtonSettings, StreamdeckCommFns } from './StreamdeckApi';
import { ModuleControlsJson } from '../components/control-reference/ControlReferenceInterface';

export interface StreamdeckSocketSettings {
    port: number,
    propertyInspectorUUID: string,
    registerEvent: string,
    info: Record<string, unknown>, // A large json object that only needs to be gatewayed
}

export function defaultStreamdeckSocketSettings(): StreamdeckSocketSettings {
    return {
        port: 28196,
        propertyInspectorUUID: "EC904BE464FFF7594DD6008FA3561A18",
        registerEvent: "registerPropertyInspector",
        info: {}
    };
}

export function useStreamdeckWebsocket(socketSettings: StreamdeckSocketSettings): StreamdeckApi {
    const [buttonAction, setButtonAction] = useState("");
    const [buttonSettings, setButtonSettings] = useState(defaultButtonSettings());
    const [globalSettings, setGlobalSettingsState] = useState(defaultGlobalSettings());
    const [moduleList, setModuleList] = useState<string[]>(["No modules found..."]);
    const [moduleControlRefs, setModuleControlRefs] = useState<ModuleControlsJson>();
    const websocket = useRef<WebSocket | null>(null);

    // Definition of the API that is used by other components.
    const commFns: StreamdeckCommFns = useMemo(() => {
        // Protocol to send messages to the Streamdeck application.
        function send(event: string, payload: Record<string, unknown>) {
            const json = Object.assign({}, { event: event, context: socketSettings.propertyInspectorUUID }, payload);
            websocket.current?.send(JSON.stringify(json));
            console.debug("Sent message (", event, "):", json);
        }

        // Sends a message to the C++ plugin executable.
        async function sendToPlugin(payload: Record<string, unknown>) {
            const json = {
                action: buttonAction,
                event: "sendToPlugin",
                context: socketSettings.propertyInspectorUUID,
                payload: payload,
            };
            websocket.current?.send(JSON.stringify(json));
            console.debug("Sent message: ", payload);
        }

        return {
            getSettings: function () {
                send('getSettings', {});
            },

            setSettings: function (settings: StreamdeckButtonSettings) {
                send('setSettings', { payload: settings });
                this.sendSettingsToPlugin(settings)
            },

            getGlobalSettings: function () {
                send('getGlobalSettings', {});
            },

            setGlobalSettings: function (setting: string, value: string) {
                const updatedGlobalSettings = Object.assign({}, globalSettings, { [setting]: value });
                setGlobalSettingsState(updatedGlobalSettings);
                send('setGlobalSettings', { payload: updatedGlobalSettings });
            },

            logMessage: function (message: string) {
                send('logMessage', { payload: { message: message } });
            },

            sendSettingsToPlugin: function (settings: StreamdeckButtonSettings) {
                sendToPlugin({ event: "SettingsUpdate", settings: settings });
            },

            requestSimulationState: function () {
                sendToPlugin({ event: "RequestDcsStateUpdate" });
            },

            requestModuleList: function (path: string) {
                sendToPlugin({ event: "requestModuleList", path: path });
            },

            requestModule: function (filename: string) {
                sendToPlugin({ event: "requestControlReferenceJson", filename: filename });
            },
        };
    }, [socketSettings, buttonAction]);

    // This message registers this Websocket binding as the Property Inspector
    // for the selected button in the Streamdeck GUI.
    function registerPropertyInspector() {
        const json = {
            event: socketSettings.registerEvent,
            uuid: socketSettings.propertyInspectorUUID
        };
        websocket.current?.send(JSON.stringify(json));
        console.debug("Registered myself according to:", json);
    }

    interface SDMessage {
        action?: string,
        event: string,
        payload: SDMessagePayload
    }
    interface SDMessagePayload {
        event?: string,
        settings?: Record<string, string>,
        moduleList?: string[],
        jsonFile?: ModuleControlsJson,
    }

    function onReceivedMessage(msg: string) {
        let jsonObj: SDMessage;
        try {
            jsonObj = JSON.parse(msg);
            console.debug("Received message:", jsonObj);
        } catch (e) {
            console.warn("Error in parsing received message", msg);
            return;
        }
        if (Object.prototype.hasOwnProperty.call(jsonObj, "event")) {
            handleReceivedMessageEvent(jsonObj.event, jsonObj);
        }
    }

    function handleReceivedMessageEvent(event: string, msg: SDMessage) {
        switch (event) {
            case "didReceiveSettings":
                if (msg.action) { setButtonAction(msg.action); }
                setButtonSettings(prevButtonSetting => Object.assign(prevButtonSetting, msg.payload.settings));
                console.debug("Received Button Settings", msg.payload.settings);
                break;
            case "didReceiveGlobalSettings":
                setGlobalSettingsState(prevGlobalSettings => Object.assign(prevGlobalSettings, msg.payload.settings));
                console.debug("Received Global Settings", msg.payload.settings);
                break;
            case "sendToPropertyInspector":
                switch (msg.payload.event) {
                    case "ModuleList":
                        if (msg.payload.moduleList) { console.log("ModuleList Recieved"); setModuleList(msg.payload.moduleList); }
                        break;
                    case "JsonFile":
                        if (msg.payload.jsonFile) { setModuleControlRefs(msg.payload.jsonFile); }
                        break;
                    case "DebugDcsGameState":
                        // Do Nothing.
                        break;
                }
                break;
            default:
                console.warn("Message Handler not defined for Event: ", event);
        }

    }

    // Websocket connect and shutdown setup.
    useEffect(() => {
        websocket.current = new WebSocket("ws://127.0.0.1:" + socketSettings.port)
        websocket.current.onopen = () => {
            console.debug("Connected to Streamdeck Websocket.");
            registerPropertyInspector();
            commFns.getGlobalSettings();
            commFns.getSettings();
            commFns.requestModuleList(globalSettings.dcs_bios_install_path);
        }
        websocket.current.onmessage = (msg: MessageEvent) => {
            onReceivedMessage(msg.data);
        }
        websocket.current.onerror = function (event: Event) {
            console.warn('WEBOCKET ERROR', event);
        };
        websocket.current.onclose = function (event: CloseEvent) {
            // Websocket is closed
            const reason = WEBSOCKETERROR(event);
            console.log('[STREAMDECK]***** WEBOCKET CLOSED **** reason:', reason);
        };
        return function onUnmount() {
            console.log("Closing websocket");
            websocket.current?.close();
        }
    }, []);

    console.log("render");

    return { commFns, buttonSettings, globalSettings, moduleList, moduleControlRefs } as const;
}

/**
 * Additional error-handling log messages.
 */
function WEBSOCKETERROR(evt: CloseEvent) {
    // Websocket is closed
    let reason = '';
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
