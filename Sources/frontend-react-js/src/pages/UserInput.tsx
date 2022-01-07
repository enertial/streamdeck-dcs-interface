import { ChangeEvent, useState } from "react";
import StreamdeckApi from "../comms/StreamdeckApi";

interface Props {
    sdApi: StreamdeckApi
}

function UserInput({ sdApi }: Props): JSX.Element {

    const [sendAddress, setSendAddress] = useState(sdApi.buttonSettings.send_address);
    const [pressValue, setPressValue] = useState(sdApi.buttonSettings.press_value);
    const [titleMonitorAddress, setTitleMonitorAddress] = useState(sdApi.buttonSettings.dcs_id_string_monitor);
    const [titleMonitorStrLen, setTitleMonitorStrLen] = useState(sdApi.buttonSettings.string_monitor_max_string_length);

    function handleSendAddressChange(event: ChangeEvent<HTMLInputElement>) {
        setSendAddress(event.currentTarget.value);
    }
    function handleSendValueChange(event: ChangeEvent<HTMLInputElement>) {
        setPressValue(event.currentTarget.value);
    }
    function handleTitleMonitorAddressChange(event: ChangeEvent<HTMLInputElement>) {
        setTitleMonitorAddress(event.currentTarget.value);
    }
    function handleTitleMonitorStrLenChange(event: ChangeEvent<HTMLInputElement>) {
        const maxStringLength: number = parseInt(event.currentTarget.value);
        if (!isNaN(maxStringLength)) {
            setTitleMonitorStrLen(maxStringLength);
        }
    }

    function handleSubmitButtonClick() {
        const updatedSettings = Object.assign({}, sdApi.buttonSettings, {
            send_address: sendAddress,
            press_value: pressValue,
            release_value: "0",
            dcs_id_string_monitor: titleMonitorAddress,
            string_monitor_max_string_length: titleMonitorStrLen,
        });
        sdApi.commFns.setSettings(updatedSettings);
    }

    function handleDebugButtonClick() {
        sdApi.commFns.requestSimulationState();
    }

    return (
        <div>
            <p>DCS Command:</p>
            <input
                id="sendAddress"
                type="text"
                placeholder="Enter Control Reference"
                value={sendAddress}
                onChange={handleSendAddressChange}
            />
            <span> </span>
            <input
                id="sendValue"
                type="text"
                placeholder="Send Value"
                value={pressValue}
                onChange={handleSendValueChange}
            />
            <p>DCS Title Monitor:</p>
            <input
                id="titleMonitorAddress"
                type="text"
                placeholder="Enter Control Reference"
                value={titleMonitorAddress}
                onChange={handleTitleMonitorAddressChange}
            />
            <span> </span>
            <input
                id="titleMonitorStrLen"
                type="text"
                placeholder="Max Length"
                value={titleMonitorStrLen}
                onChange={handleTitleMonitorStrLenChange}
            />
            <br></br>
            <button
                onClick={handleSubmitButtonClick}
                disabled={pressValue.length === 0 || titleMonitorAddress.length === 0}
            >
                Submit
            </button>
            <span> </span>
            <button onClick={handleDebugButtonClick} > Debug </button>
        </div>
    );
}

export default UserInput;
