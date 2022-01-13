import { fireEvent, render, screen } from "@testing-library/react";
import { defaultStreamdeckSocketSettings, useStreamdeckWebsocket } from "./StreamdeckWebsocket";
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
            <button onClick={() => { sdApi.commFns.setGlobalSettings("last_search_query", "query") }}>setGlobalSettings</button>
            <button onClick={() => { sdApi.commFns.logMessage("test log message") }}>logMessage</button>
            <button onClick={() => { sdApi.commFns.sendSettingsToPlugin(exampleButtonSettings) }}>sendSettingsToPlugin</button>
            <button onClick={() => { sdApi.commFns.requestSimulationState() }}>requestSimulationState</button>
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
    for (let i = 0; i < 4; i++) {
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
    const expectedButtonSettingsUpdateRequest = JSON.stringify({ event: "getSettings", context: socketSettings.propertyInspectorUUID });
    const expectedRequestModuleListRequest = JSON.stringify({ action: "", event: "sendToPlugin", context: socketSettings.propertyInspectorUUID, payload: { event: "requestModuleList", path: "" } });

    expect(mockServer).toHaveReceivedMessages([expectedRegistrationMessage,
        expectedGlobalSettingsUpdateRequest,
        expectedButtonSettingsUpdateRequest,
        expectedRequestModuleListRequest
    ]);
});

test("send setSettings", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ event: "setSettings", context: socketSettings.propertyInspectorUUID, payload: exampleButtonSettings });
    const expectedMessageToPlugin = JSON.stringify({ action: "", event: "sendToPlugin", context: socketSettings.propertyInspectorUUID, payload: { event: "SettingsUpdate", settings: exampleButtonSettings } });
    // Action
    fireEvent.click(screen.getByText(/setSettings/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
    await expect(mockServer).toReceiveMessage(expectedMessageToPlugin);
});

test("send setGlobalSettings", async () => {
    // Setup
    const expectedGlobalSettings = exampleGlobalSettings;
    expectedGlobalSettings["last_search_query"] = "query"; // as defined in the onClick function.
    const expectedMessage = JSON.stringify({ event: "setGlobalSettings", context: socketSettings.propertyInspectorUUID, payload: expectedGlobalSettings });
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

test("send sendSettingsToPlugin", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ action: "", event: "sendToPlugin", context: socketSettings.propertyInspectorUUID, payload: { event: "SettingsUpdate", settings: exampleButtonSettings } });
    // Action
    fireEvent.click(screen.getByText(/sendSettingsToPlugin/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});

test("send requestSimulationState", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ action: "", event: "sendToPlugin", context: socketSettings.propertyInspectorUUID, payload: { event: "RequestDcsStateUpdate" } });
    // Action
    fireEvent.click(screen.getByText(/requestSimulationState/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});

test("send requestModuleList", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ action: "", event: "sendToPlugin", context: socketSettings.propertyInspectorUUID, payload: { event: "requestModuleList", path: "test_path" } });
    // Action
    fireEvent.click(screen.getByText(/requestModuleList/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});

test("send requestModule", async () => {
    // Setup
    const expectedMessage = JSON.stringify({ action: "", event: "sendToPlugin", context: socketSettings.propertyInspectorUUID, payload: { event: "requestControlReferenceJson", filename: "test_filename" } });
    // Action
    fireEvent.click(screen.getByText(/requestModuleFile/i));
    // Verification
    await expect(mockServer).toReceiveMessage(expectedMessage);
});
