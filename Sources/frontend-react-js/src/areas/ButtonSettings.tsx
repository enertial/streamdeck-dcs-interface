import { useState } from "react";
import StreamdeckApi from "../api/StreamdeckApi";
import SendCommand, { defaultSendCommandSettings } from "../forms/SendCommand";
import StateMonitor, { defaultStateMonitorSettings } from "../forms/StateMonitor";
import TitleMonitor, { defaultTitleMonitorSettings } from "../forms/TitleMonitor";

interface Props {
    sdApi: StreamdeckApi
}

function ButtonSettings({ sdApi }: Props): JSX.Element {

    const [commandSettings, setCommandSettings] = useState(defaultSendCommandSettings);
    const [titleMonitorSettings, setTitleMonitorSettings] = useState(defaultTitleMonitorSettings);
    const [stateMonitorSettings, setStateMonitorSettings] = useState(defaultStateMonitorSettings);

    function handleSubmitButtonClick() {
        const updatedSettings = Object.assign({}, sdApi.buttonSettings, {
            send_address: commandSettings.send_address,
            press_value: commandSettings.press_value,
            release_value: commandSettings.release_value,
            dcs_id_string_monitor: titleMonitorSettings.dcs_id_string_monitor,
            string_monitor_max_string_length: titleMonitorSettings.string_monitor_max_string_length,
            dcs_id_compare_monitor: stateMonitorSettings.dcs_id_compare_monitor,
            dcs_id_compare_condition: stateMonitorSettings.dcs_id_compare_condition,
            dcs_id_comparison_value: stateMonitorSettings.dcs_id_comparison_value
        });
        sdApi.commFns.setSettings(updatedSettings);
    }

    return (
        <div>
            <SendCommand sdApi={sdApi} setSettings={setCommandSettings} />
            <TitleMonitor sdApi={sdApi} setSettings={setTitleMonitorSettings} />
            <StateMonitor sdApi={sdApi} setSettings={setStateMonitorSettings} />
            <br></br>
            <button onClick={handleSubmitButtonClick}> Apply Settings </button>
        </div>
    );
}

export default ButtonSettings;
