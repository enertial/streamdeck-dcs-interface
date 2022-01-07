import { fireEvent, render, screen } from "@testing-library/react";
import { defaultStreamdeckSocketSettings, useStreamdeckWebsocket } from "./StreamdeckWebSocket";
import WS from "jest-websocket-mock";
import { defaultGlobalSettings } from "./StreamdeckApi";

// Settings used for unit under test.
const socketSettings = defaultStreamdeckSocketSettings();
const exampleButtonSettings = { send_value: "test_send_value" };
const exampleGlobalSettings = defaultGlobalSettings();

// Server that can listen to communications from unit under test.
const mockServer = new WS("ws://127.0.0.1:" + socketSettings.port)

// React function that uses custom hook.
function UserOfStreamdeckWebsocket() {
    const sdApi = useStreamdeckWebsocket(socketSettings);
    return (
        <div>
            <button id="setSettings" onClick={() => { sdApi.commFns.setSettings(exampleButtonSettings) }}>setSettings</button>
            <button onClick={() => { sdApi.commFns.setGlobalSettings(exampleGlobalSettings) }}>setGlobalSettings</button>
            <button onClick={() => { sdApi.commFns.logMessage("test log message") }}>logMessage</button>
            <button onClick={() => { sdApi.commFns.sendToPlugin("test_action", { test_key: "test_value" }) }}>sendToPlugin</button>
            <button onClick={() => { sdApi.commFns.requestModuleList("test_path") }}>requestModuleList</button>
            <button onClick={() => { sdApi.commFns.requestModule("test_filename") }}>requestModuleFile</button>
            <div>buttonSettings: {JSON.stringify(sdApi.buttonSettings)}</div>
            <div>globalSettings: {JSON.stringify(sdApi.globalSettings)}</div>
        </div>
    )
}

beforeEach(async () => {
    render(<UserOfStreamdeckWebsocket />);
    await mockServer.connected;
    // StreamdeckWebsocket is expected to publish 2 messages on open.
    for (let i = 0; i < 2; i++) {
        await mockServer.nextMessage;
    }
})

afterAll(() => {
    mockServer.close();
})

test("connect to Streamdeck websocket and onopen publishes registerPropertyInspector message to Streamdeck and requests Global settings", async () => {
    // Messages expected on opening of websocket.
    const expectedRegistrationMessage = JSON.stringify({ event: "registerPropertyInspector", uuid: socketSettings.propertyInspectorUUID });
    const expectedGlobalSettingsUpdateRequest = JSON.stringify({ event: "getGlobalSettings", context: socketSettings.propertyInspectorUUID });

    expect(mockServer).toHaveReceivedMessages([expectedRegistrationMessage, expectedGlobalSettingsUpdateRequest]);
});

test("send setSettings", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ event: "setSettings", context: socketSettings.propertyInspectorUUID, payload: exampleButtonSettings });
    // Action
    fireEvent.click(screen.getByText(/setSettings/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});

test("send setGlobalSettings", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ event: "setGlobalSettings", context: socketSettings.propertyInspectorUUID, payload: exampleGlobalSettings });
    // Action
    fireEvent.click(screen.getByText(/setGlobalSettings/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});

test("send logMessage", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ event: "logMessage", context: socketSettings.propertyInspectorUUID, payload: { message: "test log message" } });
    // Action
    fireEvent.click(screen.getByText(/logMessage/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});

test("send sendToPlugin", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ event: "sendToPlugin", context: socketSettings.propertyInspectorUUID, action: "test_action", payload: { test_key: "test_value" } });
    // Action
    fireEvent.click(screen.getByText(/sendToPlugin/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});

test("send requestModuleList", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ event: "sendToPlugin", context: socketSettings.propertyInspectorUUID, action: "", payload: { event: "requestModuleList", path: "test_path" } });
    // Action
    fireEvent.click(screen.getByText(/requestModuleList/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});

test("send requestModule", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ event: "sendToPlugin", context: socketSettings.propertyInspectorUUID, action: "", payload: { event: "requestControlReferenceJson", filename: "test_filename" } });
    // Action
    fireEvent.click(screen.getByText(/requestModuleFile/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});
