import React, { useState, useEffect, useRef } from 'react';
import ButtonSettings from "../pages/ButtonSettings";
import StreamdeckApi from './StreamdeckApi';

export interface StreamdeckSocketSettings {
    port: number,
    propertyInspectorUUID: string,
    registerEvent: string,
    info: any, // A large json object that only needs to be gatewayed
}

function StreamdeckWebsocket(socketSettings: StreamdeckSocketSettings) {
    const [incomingMessage, setIncomingMessage] = useState([]);
    const websocket = useRef<WebSocket | null>(null);

    const DestinationEnum = Object.freeze({
        HARDWARE_AND_SOFTWARE: 0,
        HARDWARE_ONLY: 1,
        SOFTWARE_ONLY: 2
    });

    useEffect(() => {
        websocket.current = new WebSocket("ws://127.0.0.1:" + socketSettings.port)
        websocket.current.onopen = () => {
            console.log("Connected to Streamdeck Websocket.");
            registerPropertyInspector();
        }
        websocket.current.onmessage = (msg) => {
            console.log("Raw Message from WebSocket:", msg);
            console.log("Message from WebSocket:", msg.data);
            setIncomingMessage(msg.data);
        }

        return function onUnmount() {
            websocket.current?.close();
        }
    });

    function registerPropertyInspector() {
        const json = {
            event: socketSettings.registerEvent,
            uuid: socketSettings.propertyInspectorUUID
        };
        websocket.current?.send(JSON.stringify(json));
        console.log("Registered myself according to:", json);
    };

    function send(event: string, payload: object) {
        const json = {
            event: event,
            context: socketSettings.propertyInspectorUUID,
            payload: payload,
        };
        websocket.current?.send(JSON.stringify(json));
    }

    const sdApi: StreamdeckApi = {
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
    }

    return (
        <div>
            <ButtonSettings streamdeckApi={sdApi} />
        </div>
    );
}

export default StreamdeckWebsocket;
