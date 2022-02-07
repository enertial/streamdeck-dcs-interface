import { useState } from "react";
import StreamdeckApi from "../api/Streamdeck/StreamdeckApi";
import SendCommand, { defaultSendCommandSettings } from "../forms/SendCommand";
import StateMonitor, { defaultStateMonitorSettings } from "../forms/StateMonitor";
import TitleMonitor, { defaultTitleMonitorSettings } from "../forms/TitleMonitor";
import classes from "./ButtonSettings.module.css";

interface Props {
    sdApi: StreamdeckApi
}

function ButtonSettings({ sdApi }: Props): JSX.Element {

    const [commandSettings, setCommandSettings] = useState(defaultSendCommandSettings);
    const [titleMonitorSettings, setTitleMonitorSettings] = useState(defaultTitleMonitorSettings);
    const [stateMonitorSettings, setStateMonitorSettings] = useState(defaultStateMonitorSettings);

    function handleApplyButtonClick() {
        const updatedSettings = Object.assign({}, sdApi.buttonSettings, {
            send_address: commandSettings.send_address,
            press_value: commandSettings.press_value,
            release_value: commandSettings.release_value,
            dcs_id_string_monitor: titleMonitorSettings.dcs_id_string_monitor,
            string_monitor_address: titleMonitorSettings.string_monitor_address,
            string_monitor_mask: titleMonitorSettings.string_monitor_mask,
            string_monitor_shift: titleMonitorSettings.string_monitor_shift,
            string_monitor_max_length: titleMonitorSettings.string_monitor_max_length,
            dcs_id_compare_monitor: stateMonitorSettings.dcs_id_compare_monitor,
            compare_monitor_address: stateMonitorSettings.compare_monitor_address,
            compare_monitor_mask: stateMonitorSettings.compare_monitor_mask,
            compare_monitor_shift: stateMonitorSettings.compare_monitor_shift,
            compare_monitor_max_length: stateMonitorSettings.compare_monitor_max_length,
            dcs_id_compare_condition: stateMonitorSettings.dcs_id_compare_condition,
            dcs_id_comparison_value: stateMonitorSettings.dcs_id_comparison_value
        });
        sdApi.commFns.setSettings(updatedSettings);
    }

    return (
        <div className={classes.settings}>
            <SendCommand sdApi={sdApi} setSettings={setCommandSettings} />
            <TitleMonitor settings={titleMonitorSettings} setSettings={setTitleMonitorSettings} />
            <StateMonitor settings={stateMonitorSettings} setSettings={setStateMonitorSettings} />
            <div className={classes.buttonRow}>
                <button className="btn" onClick={handleApplyButtonClick}> Apply Settings </button>
                <button className="btn" onClick={() => { sdApi.commFns.requestSimulationState() }}> Debug </button>
            </div>
        </div>
    );
}

export default ButtonSettings;
