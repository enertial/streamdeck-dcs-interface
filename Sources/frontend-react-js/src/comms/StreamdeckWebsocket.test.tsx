import { render, screen } from "@testing-library/react";
import { DefaultStreamdeckSocketSettings, StreamdeckSocketSettings, useStreamdeckWebsocket } from "./StreamdeckWebSocket";
import WS from "jest-websocket-mock";

// Settings used for unit under test.
const settings = DefaultStreamdeckSocketSettings();
// Server that can listen to communications from unit under test.
const testServer = new WS("ws://127.0.0.1:" + settings.port)

function UserOfStreamdeckWebsocket() {
    const { sdApi, buttonSettings, globalSettings } = useStreamdeckWebsocket(settings);
    return (
        <div>
            <div>buttonSettings: {JSON.stringify(buttonSettings)}</div>
            <div>globalSettings: {JSON.stringify(globalSettings)}</div>
        </div>
    )
}

afterAll(() => {
    testServer.close();
})

test("connects to Streamdeck websocket and onopen publishes registerPropertyInspector message to Streamdeck and requests Global settings", async () => {
    // Setup
    const expectedRegistrationMessage = JSON.stringify({ event: "registerPropertyInspector", uuid: settings.propertyInspectorUUID });
    // Action
    render(<UserOfStreamdeckWebsocket />);
    // Verification
    await testServer.connected;
    await expect(testServer).toReceiveMessage(expectedRegistrationMessage);

    const expectedSecondMessage = JSON.stringify({ event: "getGlobalSettings", context: settings.propertyInspectorUUID, payload: {} });
    await expect(testServer).toReceiveMessage(expectedSecondMessage);
});
