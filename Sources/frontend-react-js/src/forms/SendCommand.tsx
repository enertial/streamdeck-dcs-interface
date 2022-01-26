import { ChangeEvent, useEffect, useState } from "react";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";

export interface SendCommandSettings {
    send_address: string;
    press_value: string;
    release_value: string;
}
export const defaultSendCommandSettings: SendCommandSettings = {
    send_address: "",
    press_value: "",
    release_value: ""
}

interface Props {
    sdApi: StreamdeckApi;
    setSettings: React.Dispatch<React.SetStateAction<SendCommandSettings>>;
}

function SendCommand({ sdApi, setSettings }: Props): JSX.Element {
    const [sendAddress, setSendAddress] = useState(sdApi.buttonSettings.send_address);
    const [pressValue, setPressValue] = useState(sdApi.buttonSettings.press_value);
    const [releaseValue, setReleaseValue] = useState(sdApi.buttonSettings.press_value);

    function handleSendAddressChange(event: ChangeEvent<HTMLInputElement>) {
        setSendAddress(event.currentTarget.value);
    }
    function handlePressValueChange(event: ChangeEvent<HTMLInputElement>) {
        setPressValue(event.currentTarget.value);
    }
    function handleReleaseValueChange(event: ChangeEvent<HTMLInputElement>) {
        setReleaseValue(event.currentTarget.value);
    }

    useEffect(() => {
        const updatedSettings: SendCommandSettings = {
            send_address: sendAddress,
            press_value: pressValue,
            release_value: releaseValue
        };
        setSettings(updatedSettings);
    }, [sendAddress, pressValue, releaseValue])

    return (
        <div>
            <h2>DCS Command:</h2>
            <p>Upon press of button, set Identifier to value:</p>
            <input
                type="text"
                placeholder="Enter Control Reference"
                value={sendAddress}
                onChange={handleSendAddressChange}
            />
            <span> </span>
            <input
                type="text"
                placeholder="Press Value"
                value={pressValue}
                onChange={handlePressValueChange}
            />
            <p>Upon press of button, set Identifier to value:</p>
            <input
                type="text"
                placeholder="Enter Control Reference"
                value={sendAddress}
                onChange={handleSendAddressChange}
            />
            <span> </span>
            <input
                type="text"
                placeholder="Release Value"
                value={releaseValue}
                onChange={handleReleaseValueChange}
            />
        </div>
    );
}

export default SendCommand;